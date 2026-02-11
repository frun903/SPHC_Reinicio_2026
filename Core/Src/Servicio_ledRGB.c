/*
 * ledRGB.c
 *
 *  Created on: Jan 29, 2026
 *      Author: Fran
 */
// Este modulo implementa control RGB fijo por GPIO.
#include <Servicio_ledRGB.h>

// ánodo común: LOW = ON, HIGH = OFF
#define LED_ON  GPIO_PIN_RESET
#define LED_OFF GPIO_PIN_SET

// Mapeo de pines (los tuyos de coles del led)
#define B_PORT GPIOB
#define B_PIN  GPIO_PIN_0 // este enciende el color azul

#define G_PORT GPIOB
#define G_PIN  GPIO_PIN_1 // este enciende el color verde

#define R_PORT GPIOA
#define R_PIN  GPIO_PIN_7   // este pin enciende el color ROJO

void LED_RGB_Init(void)
{
  // Los GPIO ya los inicializa MX_GPIO_Init()
  // Aca defino el estado inicial: apagado
  LED_RGB_Write(0,0,0);
}

void LED_RGB_Write(uint8_t r, uint8_t g, uint8_t b)
{
  HAL_GPIO_WritePin(R_PORT, R_PIN, r ? LED_ON : LED_OFF);
  HAL_GPIO_WritePin(G_PORT, G_PIN, g ? LED_ON : LED_OFF);
  HAL_GPIO_WritePin(B_PORT, B_PIN, b ? LED_ON : LED_OFF);
}

void LED_RGB_SetColor(led_rgb_color_t c)
{
  switch (c) {
    case LED_RGB_OFF:     LED_RGB_Write(0,0,0); break;
    case LED_RGB_RED:     LED_RGB_Write(1,0,0); break;
    case LED_RGB_GREEN:   LED_RGB_Write(0,1,0); break;
    case LED_RGB_BLUE:    LED_RGB_Write(0,0,1); break;
    case LED_RGB_YELLOW:  LED_RGB_Write(1,1,0); break;
    case LED_RGB_CYAN:    LED_RGB_Write(0,1,1); break;
    case LED_RGB_MAGENTA: LED_RGB_Write(1,0,1); break;
    case LED_RGB_WHITE:   LED_RGB_Write(1,1,1); break;
    default:              LED_RGB_Write(0,0,0); break;
  }
}






