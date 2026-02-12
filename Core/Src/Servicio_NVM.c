/*
 * Servicio_NVH.c
 *
 *  Created on: Feb 11, 2026
 *      Author: Fran
 */

#include "Servicio_NVM.h"
#include <string.h>

/* =========================
 *  Configuración Flash F1
 * ========================= */
#define FLASH_BASE_ADDR      0x08000000UL
#define FLASHSIZE_BASE       0x1FFFF7E0UL   // STM32F1: Flash size in KB (uint16_t)
#define FLASH_PAGE_SIZE      0x400UL        // 1KB por página en F103 MD

/* =========================
 *  Formato de registro
 * ========================= */
#define NVM_MAGIC            0x53485043UL   // 'S''H''P''C'
#define NVM_VERSION          0x00010002UL   // major.minor (subimos versión por agregar name)

typedef struct __attribute__((packed)) {
  uint32_t magic;
  uint32_t version;
  uint32_t seq;       // contador incremental (elige el más grande)
  uint32_t len;       // bytes válidos de payload
  uint32_t crc32;     // CRC32 del payload
  // seguido por payload (NVM_Creds_t)
} NVM_Header_t;

/* Tamaño total almacenado = header + payload */
#define NVM_PAYLOAD_SIZE     (sizeof(NVM_Creds_t))
#define NVM_RECORD_SIZE      (sizeof(NVM_Header_t) + NVM_PAYLOAD_SIZE)

/* =========================
 *  CRC32 (software, estándar)
 * ========================= */
static uint32_t crc32_compute(const uint8_t *data, uint32_t len)
{
  uint32_t crc = 0xFFFFFFFFUL;
  for (uint32_t i = 0; i < len; i++) {
    crc ^= data[i];
    for (uint8_t b = 0; b < 8; b++) {
      uint32_t mask = -(crc & 1UL);
      crc = (crc >> 1) ^ (0xEDB88320UL & mask);
    }
  }
  return ~crc;
}

/* =========================
 *  Direcciones de slots (dinámicas)
 * ========================= */
static uint32_t flash_size_bytes(void)
{
  uint16_t kb = *(volatile uint16_t*)FLASHSIZE_BASE;
  if (kb == 0 || kb == 0xFFFF) {
    // fallback conservador si algo raro: 64KB
    kb = 64;
  }
  return ((uint32_t)kb) * 1024UL;
}

static uint32_t slotB_addr(void)
{
  // última página
  return FLASH_BASE_ADDR + flash_size_bytes() - FLASH_PAGE_SIZE;
}

static uint32_t slotA_addr(void)
{
  // penúltima página
  return FLASH_BASE_ADDR + flash_size_bytes() - 2UL * FLASH_PAGE_SIZE;
}

/* =========================
 *  Lectura/validación de un slot
 * ========================= */
static uint8_t slot_read(uint32_t addr, NVM_Header_t *hdr_out, NVM_Creds_t *creds_out)
{
  const NVM_Header_t *hdr = (const NVM_Header_t*)addr;

  // Chequeos básicos
  if (hdr->magic != NVM_MAGIC) return 0;
  if (hdr->version != NVM_VERSION) return 0;
  if (hdr->len != NVM_PAYLOAD_SIZE) return 0;

  // Copio payload
  const uint8_t *payload_ptr = (const uint8_t*)(addr + sizeof(NVM_Header_t));
  uint32_t crc = crc32_compute(payload_ptr, hdr->len);
  if (crc != hdr->crc32) return 0;

  // OK: devuelvo
  if (hdr_out) *hdr_out = *hdr;
  if (creds_out) {
    memcpy(creds_out, payload_ptr, sizeof(NVM_Creds_t));
    // Aseguro terminación (por seguridad)
    creds_out->ssid[NVM_SSID_MAX-1] = '\0';
    creds_out->pass[NVM_PASS_MAX-1] = '\0';
    creds_out->name[NVM_NAME_MAX-1] = '\0';
  }
  return 1;
}

/* =========================
 *  Erase de una página
 * ========================= */
static uint8_t flash_erase_page(uint32_t page_addr)
{
  FLASH_EraseInitTypeDef erase = {0};
  uint32_t page_error = 0;

  erase.TypeErase   = FLASH_TYPEERASE_PAGES;
  erase.PageAddress = page_addr;
  erase.NbPages     = 1;

  if (HAL_FLASHEx_Erase(&erase, &page_error) != HAL_OK) {
    return 0;
  }
  return 1;
}

/* =========================
 *  Programación (word)
 * ========================= */
static uint8_t flash_program_words(uint32_t dst_addr, const uint8_t *src, uint32_t len)
{
  // len debe ser múltiplo de 4 para programar WORD
  if ((len % 4) != 0) return 0;

  for (uint32_t off = 0; off < len; off += 4) {
    uint32_t w;
    memcpy(&w, src + off, 4);

    if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, dst_addr + off, w) != HAL_OK) {
      return 0;
    }
  }
  return 1;
}

/* =========================
 *  API pública
 * ========================= */
uint8_t NVM_LoadCreds(NVM_Creds_t *out)
{
  if (!out) return 0;

  NVM_Header_t ha, hb;
  NVM_Creds_t  ca, cb;
  uint8_t va = slot_read(slotA_addr(), &ha, &ca);
  uint8_t vb = slot_read(slotB_addr(), &hb, &cb);

  if (!va && !vb) return 0;

  // Elijo el válido con seq mayor
  if (va && vb) {
    if (hb.seq >= ha.seq) {
      *out = cb;
    } else {
      *out = ca;
    }
    return 1;
  }

  *out = (vb) ? cb : ca;
  return 1;
}

uint8_t NVM_SaveCreds(const NVM_Creds_t *in)
{
  if (!in) return 0;

  // Sanitizar: aseguramos terminación y no escribir basura gigante
  NVM_Creds_t tmp = *in;
  tmp.ssid[NVM_SSID_MAX-1] = '\0';
  tmp.pass[NVM_PASS_MAX-1] = '\0';
  tmp.name[NVM_NAME_MAX-1] = '\0';

  // Leo cuál slot está activo (por seq)
  NVM_Header_t ha, hb;
  uint8_t va = slot_read(slotA_addr(), &ha, NULL);
  uint8_t vb = slot_read(slotB_addr(), &hb, NULL);

  uint32_t next_seq = 1;
  if (va && vb) next_seq = (ha.seq > hb.seq ? ha.seq : hb.seq) + 1;
  else if (va)  next_seq = ha.seq + 1;
  else if (vb)  next_seq = hb.seq + 1;

  // Decido destino: escribo en el slot "opuesto" al más nuevo
  uint32_t dst = slotB_addr(); // default
  if (va && vb) {
    dst = (ha.seq >= hb.seq) ? slotB_addr() : slotA_addr();
  } else if (va) {
    dst = slotB_addr();
  } else if (vb) {
    dst = slotA_addr();
  } else {
    // ninguno válido: arranco en A para ser consistente
    dst = slotA_addr();
  }

  // Armo buffer record (header + payload) alineado a 4 bytes
  uint8_t record[NVM_RECORD_SIZE];
  NVM_Header_t h = {0};
  h.magic   = NVM_MAGIC;
  h.version = NVM_VERSION;
  h.seq     = next_seq;
  h.len     = NVM_PAYLOAD_SIZE;

  memcpy(record + sizeof(NVM_Header_t), &tmp, sizeof(NVM_Creds_t));
  h.crc32 = crc32_compute(record + sizeof(NVM_Header_t), h.len);
  memcpy(record, &h, sizeof(NVM_Header_t));

  // Asegurar múltiplo de 4 para programar en WORD
  if ((NVM_RECORD_SIZE % 4) != 0) {
    return 0;
  }

  // Escribo Flash: unlock -> erase page -> program -> lock
  HAL_FLASH_Unlock();

  // Recomendación: deshabilitar IRQ durante erase/program para minimizar riesgos
  __disable_irq();

  uint8_t ok = flash_erase_page(dst);
  if (ok) {
    ok = flash_program_words(dst, record, NVM_RECORD_SIZE);
  }

  __enable_irq();
  HAL_FLASH_Lock();

  if (!ok) return 0;

  // Verificación post-escritura (lectura + CRC)
  NVM_Creds_t verify;
  if (!slot_read(dst, NULL, &verify)) return 0;

  return 1;
}

uint8_t NVM_EraseAll(void)
{
  HAL_FLASH_Unlock();
  __disable_irq();

  uint8_t okA = flash_erase_page(slotA_addr());
  uint8_t okB = flash_erase_page(slotB_addr());

  __enable_irq();
  HAL_FLASH_Lock();

  return (okA && okB) ? 1 : 0;
}


