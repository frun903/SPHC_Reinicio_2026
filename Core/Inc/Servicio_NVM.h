/*
 * Servivio_NVM.h: Servicio dedicado al guardado en memoria flash de la bluepill.
 *
 *  Created on: Feb 11, 2026
 *      Author: Fran
 */

#ifndef INC_SERVICIO_NVM_H_
#define INC_SERVICIO_NVM_H_



#include "stm32f1xx_hal.h"
#include <stdint.h>

#define NVM_SSID_MAX   32
#define NVM_PASS_MAX   64
#define NVM_NAME_MAX   64

typedef struct {
  char ssid[NVM_SSID_MAX];
  char pass[NVM_PASS_MAX];
  char name[NVM_PASS_MAX];
} NVM_Creds_t;

/**
 * @brief  Carga credenciales desde Flash. Devuelve 1 si encontró datos válidos, 0 si no.
 */
uint8_t NVM_LoadCreds(NVM_Creds_t *out);

/**
 * @brief  Guarda credenciales en Flash (A/B slots). Devuelve 1 si ok, 0 si error.
 */
uint8_t NVM_SaveCreds(const NVM_Creds_t *in);

/**
 * @brief  Borra ambas páginas reservadas (A y B). Útil para "factory reset".
 */
uint8_t NVM_EraseAll(void);


#endif /* INC_SERVICIO_NVM_H_ */
