/*
 * esp_01_sleep.h (EN DESHUSO)
 *
 *  Created on: Feb 4, 2026
 *      Author: Fran
 *
 */

#ifndef INC_ESP_01_SLEEP_H_
#define INC_ESP_01_SLEEP_H_


#include <stdint.h>
#include "stm32f1xx_hal.h"
/**
 *  ESP01_PoweOn: Enciende la ESP-01S habilitando EN/CH_PD.
 *        Requiere EN conectado a PA1 y pullUp externo a 3.3Vyo use 4.7k
 */
void ESP01_PowerOn(void);

/**
 * ESP01_PowerOff:  Apaga la ESP-01S deshabilitando EN/CH_PD (consumo mínimo).
 *        Requiere EN conectado a PA1.
 */
void ESP01_PowerOff(void);

/**
 * ESP01:IsPowered Verifica el estado de PA1. Devuelve 1 si EN está en HIGH (ESP habilitada), 0 si está en LOW.
 */
uint8_t ESP01_IsPowered(void);



#endif /* INC_ESP_01_SLEEP_H_ */
