/*
 * Servicio_Display.h
 *
 *  Created on: Oct 26, 2024
 *      Author: frun903
 */

// Inicializa el display SH1106 para su uso.
void Inicializo_Display();

//Limpia el Display
void Limpio_Display();


//Escribe en los renglones definidos

void Muestra_texto_Primer_Renglon(char *texto);

void Muestra_texto_Segundo_renglon(char *texto);

void Muestra_texto_Tercer_renglon(char *texto);

// Descripción: Borra el contenido actual del display y muestra un mensaje de saludo.
void SaludoInicial();

// Descripción: Borra el contenido actual del display y muestra un mensaje que indica que se está intentando establecer una conexión.
void Mensaje_de_conectando();

// Descripción: Borra el contenido actual del display y muestra un mensaje de conexión exitosa, además de la información de la red Wi-Fi y el servidor.
void Mensaje_de_Conexion_Exitosa();

// Descripción: Borra el contenido actual del display y muestra el estado y la temperatura de los sensores.
void Mostrando_temperatura_y_Estado();

// Descripción: Borra el contenido actual del display y muestra los valores de temperatura y humedad, junto con un mensaje de estado.
void Muestra_de_Temperatura(float temp, float humedad, char *buffer);

//Descripción:Borra el contenido actual del display y muestra cadena de caracteres pasada por parametros.
void Muestra_texto_Parametro(char *texto);

//Descripción:Muesuestra cadena de caracteres pasada por parametros, deb ir acompañada por funcion anterior .
void Muestra_texto_Parametro_Segundo_renglon(char *texto);

//Descripción:Muesuestra cadena de caracteres pasada por parametros, deb ir acompañada por funcion anterior
void Muestra_texto_Parametro_Tercer_renglon(char *texto);


