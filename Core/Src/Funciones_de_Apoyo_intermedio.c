/*
 * Funciones_de_Apoyo_intermedio.c
 *
 *  Created on: Nov 3, 2024
 *      Author: Fran
 */
#include "Funciones_de_Apoyo_intermedio.h"
#include"main.h"
#include "stm32f1xx_hal.h"

/*Funcion para convertir de flotante a char, ChatGPT
Este método descompone el valor en su parte entera y decimal,
y luego convierte cada una por separado.
Entrada: Flotante number, int decimalPlace, char buffer
Salida: Ninguna*/

void floatToString(float number, char *buffer, int decimalPlaces) {
    int integerPart = (int)number;  // Parte entera
    float decimalPart = number - integerPart;  // Parte decimal

    // Convierte la parte entera a cadena
    int index = 0;
    if (integerPart == 0) {
        buffer[index++] = '0';
    } else {
        if (integerPart < 0) {
            buffer[index++] = '-';
            integerPart = -integerPart;
        }
        int intTemp = integerPart, intCount = 0;
        while (intTemp != 0) {
            intTemp /= 10;
            intCount++;
        }
        for (int i = intCount - 1; i >= 0; i--) {
            buffer[index + i] = '0' + (integerPart % 10);
            integerPart /= 10;
        }
        index += intCount;
    }

    buffer[index++] = '.';  // Agrega el punto decimal

    // Convierte la parte decimal
    for (int i = 0; i < decimalPlaces; i++) {
        decimalPart *= 10;
        int decimalDigit = (int)decimalPart;
        buffer[index++] = '0' + decimalDigit;
        decimalPart -= decimalDigit;
    }

    buffer[index] = '\0';  // Fin de cadena
}

void Wait_New(){
	HAL_Delay(5000);
}



uint8_t Espera_Con_Salida(uint32_t total_ms, uint8_t (*check_event)(void))
{
    uint32_t pasos = total_ms / 50;
    if (pasos == 0) pasos = 1;

    for (uint32_t i = 0; i < pasos; i++) {
        if (check_event()) return 1;
        HAL_Delay(50);
    }

    return check_event();
}




