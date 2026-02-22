/*
 * Servicio_Wifi_ESP_01.h
 *
 *  Created on: Nov 3, 2024
 *      Author: Fran
 */
#ifndef INC_SERVICIO_WIFI_ESP_01_H_
#define INC_SERVICIO_WIFI_ESP_01_H_
#include <stdint.h>

extern int countB;
extern int countN;
extern int countF;
extern int countT;


void Wifi_ESP_Init(void);

void Wifi_ESP_UpRed();

void Get_Respuesta();

void Get_respuesta_Sensores(char *cadena_de_texto_de_Temperatura,char *cadena_de_texto_de_humedad  );

void  Get_primera_respuesta_Sensores(char *cadena_de_texto_de_Temperatura,char *cadena_de_texto_de_humedad);


void Get_respuesta_Sensores_con_estado(char *cadena_de_texto_de_Temperatura,char *cadena_de_texto_de_humedad, int estado);


//2026


void Wifi_ESP_UpRed_SoftAP(void);


uint8_t Wifi_ESP_PortalLoop_GetCredentials(char *out_ssid, uint16_t ssid_len,
                                           char *out_pass, uint16_t pass_len);

uint8_t Wifi_ESP_UpRed_STA(void);

// POST fijo por ahora:
// name = "Ringo"
// value = string (ej: "27.7")
uint8_t ESP_HTTP_Post_Ringo(const char *host, uint16_t port, const char *value_str);

uint8_t ESP_HTTP_Post_Item(const char *host, uint16_t port,
                           const char *name,
                           const char *value_str);

#endif /* INC_SERVICIO_WIFI_ESP_01_H_ */
