/*
 * Funciones_de_Apoyo_intermedio.h
 *
 *  Created on: Nov 1, 2024
 *      Author: Fran
 */

#include "stdint.h"

uint8_t Espera_Con_Salida(uint32_t total_ms, uint8_t (*check_event)(void));

uint32_t SPHC_PeriodoMs_SegunTemp(float t);

void floatToString(float number, char *buffer, int decimalPlaces);

void Wait_New();
