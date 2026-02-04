/*
 * Servicio_Wifi_ESP_01.h
 *
 *  Created on: Nov 3, 2024
 *      Author: Fran
 */
#ifndef INC_SERVICIO_WIFI_ESP_01_H_
#define INC_SERVICIO_WIFI_ESP_01_H_

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


#endif /* INC_SERVICIO_WIFI_ESP_01_H_ */
