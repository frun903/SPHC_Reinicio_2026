/*
 * ledRGB.h
 *
 *  Created on: Jan 29, 2026
 *      Author: fun902
 */

#ifndef INC_SERVICIO_LEDRGB_H_
#define INC_SERVICIO_LEDRGB_H_

#include "main.h"
#include <stdint.h>

typedef enum {
  LED_RGB_OFF = 0,
  LED_RGB_RED,
  LED_RGB_GREEN,
  LED_RGB_BLUE,
  LED_RGB_YELLOW,
  LED_RGB_CYAN,
  LED_RGB_MAGENTA,
  LED_RGB_WHITE
} led_rgb_color_t;

void LED_RGB_Init(void);                    // deja el LED apagado

void LED_RGB_Write(uint8_t r, uint8_t g, uint8_t b); // r,g,b: 0/1

void LED_RGB_SetColor(led_rgb_color_t c);

#endif /* INC_SERVICIO_LEDRGB_H_ */


