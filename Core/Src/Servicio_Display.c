/*
 * Servicio_Display.c
 *
 *  Created on: Oct 26, 2024
 *      Author: frun903
 */

//Libreria
#include "SH1106.h"
#include "fonts.h"
#include "i2c-icd.h"
#include "stdio.h"
#include "Funciones_de_Apoyo_intermedio.h"


//Arreglos de Texto
char Saludo[50]= "Inicio: SPHC";
char Saludo_2[50]= "Por favor Espere...";

char Conectando[20]= "Por Favor Espere";
char Conectando_2[50]= "Conectando...";
char Conectando_3[30]= "Conexion Exitosa";

char Red_Wifi[10]="SPHC";
char wifi[5]="Wifi:";
char Servidor[20]="192.168.51.1";

char Temperatura[13]="Temperatura:";
char Humedad[9]="Humedad:";


//Funciones


// Inicializa el display SH1106.
// Entrada: Ninguna.
// Salida: Ninguna.
// Descripción: Llama a la función de inicialización SH1106_Init()
// para preparar el display para su uso.
void Inicializo_Display(){
	SH1106_Init();
}

// Limpia el display SH1106.
// Entrada: Ninguna.
// Salida: Ninguna.
// Descripción: Llama a la función de inicialización SH1106_Clear()

void Limpio_Display(){
	SH1106_Clear();
}


//Escribir em primer, segundo y tercer renglon.
// Entrada: Cadena de Caracteres
// Descripción: Escribe en los renglones correspondientes
void Muestra_texto_Primer_Renglon(char *texto){

		 SH1106_GotoXY(2, 10);
		 SH1106_Puts(texto, &Font_7x10, 1);
		 SH1106_UpdateScreen();

}

void Muestra_texto_Segundo_renglon(char *texto){

		 SH1106_GotoXY(2, 30);
		 SH1106_Puts(texto, &Font_7x10, 1);
	     SH1106_UpdateScreen();

}

void Muestra_texto_Tercer_renglon(char *texto){

     SH1106_GotoXY(2, 50);
     SH1106_Puts(texto, &Font_7x10, 1);
     SH1106_UpdateScreen();

}




// Muestra un mensaje inicial de saludo en el display.
// Entrada: Ninguna.
// Salida: Ninguna.
// Descripción: Borra el contenido del display y muestra un mensaje de bienvenida dividido en dos líneas.
void SaludoInicial(){
   SH1106_Clear();
   SH1106_GotoXY(2, 10);
   SH1106_Puts(Saludo, &Font_7x10, 1);
   SH1106_GotoXY(2, 30);
   SH1106_Puts(Saludo_2, &Font_7x10, 1);
   SH1106_UpdateScreen();

}

// Muestra un mensaje de "Conectando" en el display mientras se intenta establecer conexión.
// Entrada: Ninguna.
// Salida: Ninguna.
// Descripción: Borra el contenido del display y muestra el mensaje de "Por favor espere" en la primera línea y "Conectando..." en la segunda.
void Mensaje_de_conectando(){
	SH1106_Clear();
	 SH1106_GotoXY(2, 10);
	 SH1106_Puts(Conectando, &Font_7x10, 1);
	 SH1106_GotoXY(2, 30);
	 SH1106_Puts(Conectando_2, &Font_7x10, 1);
	 SH1106_UpdateScreen();

}

// Muestra un mensaje de "Conexión Exitosa" en el display y la información de la red Wi-Fi y el servidor.
// Entrada: Ninguna.
// Salida: Ninguna.
// Descripción: Borra el contenido del display, muestra el mensaje de "Conexión Exitosa" en la primera línea,
//              seguido del nombre de la red Wi-Fi y la dirección del servidor en líneas adicionales.
void Mensaje_de_Conexion_Exitosa(){

	SH1106_Clear();

	 SH1106_GotoXY(2, 10);
	 SH1106_Puts(Conectando_3, &Font_7x10, 1);
	 SH1106_UpdateScreen();


	 //Red Wifi
     SH1106_GotoXY(2, 30);
     SH1106_Puts(Red_Wifi, &Font_7x10, 1);

     //Servidor

     SH1106_GotoXY(2, 50);
     SH1106_Puts(Servidor, &Font_7x10, 1);
     SH1106_UpdateScreen();

}



// Muestra la temperatura y la humedad en el display, junto con el estado recibido en el buffer.
// Entrada:
//    - temp (float): Valor de la temperatura a mostrar.
//    - humedad (float): Valor de la humedad a mostrar.
//    - buffer (char*): Cadena de texto que representa el estado del perro.
// Salida: Ninguna.
// Descripción: Borra el contenido del display, muestra el estado en la primera línea, seguido de la temperatura en
//              la segunda línea y la humedad en la tercera línea. La temperatura y la humedad se convierten a cadenas
//              de texto antes de mostrarlas.
void Muestra_de_Temperatura(float temp, float humedad, char *buffer){

	char Temp_perro[10];
	char Humedad_ambiente[10];

	floatToString(temp, Temp_perro,3);
	floatToString(humedad, Humedad_ambiente,3);


	SH1106_Clear();

	//Estado perro
	 SH1106_GotoXY(2, 10);
	 SH1106_Puts(buffer, &Font_7x10, 1);


	 //Temperatura perro
	 SH1106_GotoXY(2, 30);
	 SH1106_Puts(Temperatura, &Font_7x10, 1);
	 SH1106_GotoXY(80, 30);
     SH1106_Puts(Temp_perro, &Font_7x10, 1);


     //Huedad del Perro
     SH1106_GotoXY(2, 50);
     SH1106_Puts(Humedad, &Font_7x10, 1);
     SH1106_GotoXY(60, 50);
     SH1106_Puts(Humedad_ambiente, &Font_7x10, 1);
     SH1106_UpdateScreen();
}


void Muestra_texto_Parametro(char *texto){

	SH1106_Clear();

		 SH1106_GotoXY(2, 10);
		 SH1106_Puts(texto, &Font_7x10, 1);
		 SH1106_UpdateScreen();



}




void Muestra_texto_Parametro_Tercer_renglon(char *texto){

    //Huedad del Perro
     SH1106_GotoXY(2, 50);
     SH1106_Puts(Humedad, &Font_7x10, 1);
     SH1106_GotoXY(60, 50);
     SH1106_Puts(texto, &Font_7x10, 1);
     SH1106_UpdateScreen();

}




