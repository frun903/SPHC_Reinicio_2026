/*
 * esp_01_sleep.c
 *
 *  Created on: Feb 4, 2026 (EN DESHUSO Feb 11)
 *      Author: Fran
 */

#include "esp_01_sleep.h"
#include "stm32f1xx_hal.h"

// EN/CH_PD está en PA1
#define ESP01_EN_PORT GPIOB
#define ESP01_EN_PIN  GPIO_PIN_12


// Solo hago GPIO_PIN_SET par aponerlo en alto y de esa forma habilitar la ESP-01
void ESP01_PowerOn(void)
{
    // EN HIGH -> ESP habilitada
    HAL_GPIO_WritePin(ESP01_EN_PORT, ESP01_EN_PIN, GPIO_PIN_SET);

    // margen corto para estabilización interna
    HAL_Delay(10000);
}

// Solo hago GPIO_PIN_RESET para aponerlo en bajo y de esa forma descativar la ESP-01
void ESP01_PowerOff(void)
{
    // EN LOW -> ESP apagada (consumo mínimo)
    HAL_GPIO_WritePin(ESP01_EN_PORT, ESP01_EN_PIN, GPIO_PIN_RESET);

    // margen opcional
    HAL_Delay(20);
}


//Solicito el estado del PIN A1
uint8_t ESP01_IsPowered(void)
{
    GPIO_PinState st = HAL_GPIO_ReadPin(ESP01_EN_PORT, ESP01_EN_PIN);
    return (st == GPIO_PIN_SET) ? 1u : 0u;
}



