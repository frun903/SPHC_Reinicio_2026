/*
 * Servicio_Wifi_ESP_01.c
 *
 *  Created on: Nov 3, 2024
 *      Author: Fran
 */

//Librerias
#include "Servicio_Wifi_ESP_01.h"
#include "main.h"
#include "stdio.h"
#include "string.h"
#include <stdint.h>
#include "Servicio_Display.h"

extern UART_HandleTypeDef huart2;

//variables
 uint8_t rxBuffer[512] = {0};
 uint8_t ATisOK;
    int channel;
    int onoff;
    int led = 1;
    char html[256];

    //Defino los comandos AT
    char ATcommand[64];

    char ATcommandB[1024] = "<!DOCTYPE html><html>\n<head>\n\
    <title>SPHC - Sistema de Prevencion de Hipertermia en Canidos</title>\n<link href=\"data:image/x-icon;base64,\
    A\" rel=\"icon\" type=\"image/x-icon\"><style>\nhtml {\
    display: inline-block; margin: 0px auto; text-align: center;}\n\
    body{margin-top: 50px;}\n.button {display: block;\
    width: 70px;\nbackground-color: #008000;\nborder: none;\ncolor: white;\n\
    padding: 14px 28px;\ntext-decoration: none;\nfont-size: 24px;\n\
    margin: 0px auto 36px; \nborder-radius: 5px;}\n\
    .button-on {background-color: #008000;}\n.button-on:active\
    {background-color: #008000;}\n.button-off {background-color: #808080;}\n\
    .button-off:active {background-color: #808080;}\n\
    p {font-size: 14px;color: #808080;margin-bottom: 20px;}\n\
    </style>\n</head>\n<body>\n<h1>STM32 - ESP8266</h1>";

    char ATcommandN[100] = "<p>Light is currently on\
    </p><a class=\"button button-off\" href=\"/lightoff\">OFF</a>";

    char ATcommandF[100] = "<p>Light is currently off\
    </p><a class=\"button button-on\" href=\"/lighton\">ON</a>";

    char ATcommandT[16] = "</body></html>";


    // Estas variables viven en main.c (globales)
    extern char g_ssid[33];
    extern char g_pass[65];




    // Variables para almacenar las longitudes
    int countB;
    int countN;
    int countF;
    int countT;

    // Función para inicializar las longitudes de las cadenas
void Wifi_ESP_Init(void)
    {
        countB = strlen(ATcommandB);
        countN = strlen(ATcommandN);
        countF = strlen(ATcommandF);
        countT = strlen(ATcommandT);
    }




void Wifi_ESP_UpRed(void){

	  Mensaje_de_conectando();

    	   sprintf(ATcommand,"AT\r\n");
    	     memset(rxBuffer,0,sizeof(rxBuffer));
    	     HAL_UART_Transmit(&huart2,(uint8_t *)ATcommand,strlen(ATcommand),5000);
    	     HAL_UART_Receive (&huart2, rxBuffer, 512, 5000);
    	     HAL_Delay(2000);



    	    sprintf(ATcommand,"AT+RST\r\n");
    	    memset(rxBuffer,0,sizeof(rxBuffer));
    	    HAL_UART_Transmit(&huart2,(uint8_t *)ATcommand,strlen(ATcommand),5000);
    	    HAL_UART_Receive (&huart2, rxBuffer, 512, 5000);
    	    HAL_Delay(2000);


    	    ATisOK = 0;
    	    while(!ATisOK){
    	      sprintf(ATcommand,"AT+CWMODE=2\r\n");
    	        memset(rxBuffer,0,sizeof(rxBuffer));
    	        HAL_UART_Transmit(&huart2,(uint8_t *)ATcommand,strlen(ATcommand),5000);
    	        HAL_UART_Receive (&huart2, rxBuffer, 512, 5000);
    	      if(strstr((char *)rxBuffer,"OK")){
    	        ATisOK = 1;
    	      }
    	      HAL_Delay(2000);
    	    }

    	    ATisOK = 0;
    	    while(!ATisOK){

    	      sprintf(ATcommand,"AT+CWSAP=\"SPHC\",\"12345678\",1,3,4,0\r\n");
    	        memset(rxBuffer,0,sizeof(rxBuffer));
    	        HAL_UART_Transmit(&huart2,(uint8_t *)ATcommand,strlen(ATcommand),5000);
    	        HAL_UART_Receive (&huart2, rxBuffer, 512, 5000);

    	        if(strstr((char *)rxBuffer,"OK")){
    	        ATisOK = 1;
    	      }

    	        HAL_Delay(2000);
    	    }

    	    ATisOK = 0;
    	    while(!ATisOK){
    	      sprintf(ATcommand,"AT+CIPAP=\"192.168.51.1\"\r\n");
    	      memset(rxBuffer,0,sizeof(rxBuffer));
    	      HAL_UART_Transmit(&huart2,(uint8_t *)ATcommand,strlen(ATcommand),5000);
    	      HAL_UART_Receive (&huart2, rxBuffer, 512, 5000);
    	      if(strstr((char *)rxBuffer,"OK")){
    	        ATisOK = 1;
    	      }
    	      HAL_Delay(500);
    	    }

    	    ATisOK = 0;
    	    while(!ATisOK){
    	      sprintf(ATcommand,"AT+CIPMUX=1\r\n");
    	        memset(rxBuffer,0,sizeof(rxBuffer));
    	        HAL_UART_Transmit(&huart2,(uint8_t *)ATcommand,strlen(ATcommand),5000);
    	        HAL_UART_Receive (&huart2, rxBuffer, 512, 5000);
    	        if(strstr((char *)rxBuffer,"OK")){
    	          ATisOK = 1;
    	        }
    	        HAL_Delay(500);
    	    }

    	     ATisOK = 0;
    	    while(!ATisOK){
    	      sprintf(ATcommand,"AT+CIPSERVER=1,80\r\n");
    	      memset(rxBuffer,0,sizeof(rxBuffer));
    	      HAL_UART_Transmit(&huart2,(uint8_t *)ATcommand,strlen(ATcommand),5000);
    	      HAL_UART_Receive (&huart2, rxBuffer, 512, 5000);
    	      if(strstr((char *)rxBuffer,"OK")){
    	          ATisOK = 1;
    	      }
    	      HAL_Delay(500);
    	    }

    	     Mensaje_de_Conexion_Exitosa();
    	     HAL_Delay(5000);
    }


void Get_Respuesta(){

	  memset(rxBuffer, 0, sizeof(rxBuffer));  // Limpia el buffer
	    HAL_UART_Receive(&huart2, rxBuffer, sizeof(rxBuffer), 5000);  // Espera la solicitud HTTP

	    Muestra_texto_Parametro("Esperando Solicitud");

	    // Verifica si la solicitud contiene un GET
	    if (strstr((char *)rxBuffer, "GET /")) {
	    	const char *html = "<html><body><h1>SPHC - Sistema de Prevencion de Hipertermia en Canidos</h1></body></html>";  // Contenido de la respuesta
	        int channel = 0;  // Supongamos que estamos en el canal 0 por simplicidad

	        // Envia el comando AT para indicar el envío de datos
	        sprintf(ATcommand, "AT+CIPSEND=%d,%d\r\n", channel, strlen(html));
	        HAL_UART_Transmit(&huart2, (uint8_t *)ATcommand, strlen(ATcommand), 5000);
	        HAL_UART_Receive(&huart2, rxBuffer, sizeof(rxBuffer), 5000);  // Espera la respuesta de ">" para continuar

	        // Envía la respuesta HTML si el ESP-01S está listo
	        if (strstr((char *)rxBuffer, ">")) {
	        	 Muestra_texto_Parametro("Solicitud Si!");
	            HAL_UART_Transmit(&huart2, (uint8_t *)html, strlen(html), 5000);
	        }

	        // Cierra la conexión después de enviar la respuesta
	        sprintf(ATcommand, "AT+CIPCLOSE=%d\r\n", channel);
	        HAL_UART_Transmit(&huart2, (uint8_t *)ATcommand, strlen(ATcommand), 5000);
	    }

	    HAL_Delay(200);  // Espera antes de la siguiente solicitud
}

//Atencion: Servicio Bloqueante!
void Get_respuesta_Sensores(char *cadena_de_texto_de_Temperatura,char *cadena_de_texto_de_humedad  ){



	while (1) {
	  memset(rxBuffer, 0, sizeof(rxBuffer));  // Limpia el buffer
	    HAL_UART_Receive(&huart2, rxBuffer, sizeof(rxBuffer), 5000);  // Espera la solicitud HTTP

	Muestra_texto_Parametro("Telefono Conectado");
	Muestra_texto_Parametro_Segundo_renglon(cadena_de_texto_de_Temperatura);
	Muestra_texto_Parametro_Tercer_renglon(cadena_de_texto_de_humedad);

	if (strstr((char *)rxBuffer, "GET /")) {
	        // Genera el HTML con los valores de los sensores
	        char html[256];
	        sprintf(html, "<html><body><h1  style=\"background-color:green;\">SPHC - Sistema de Prevencion</h1><h3>Temperatura:   %s C</h3><h3>Estado:     %s%</h3></body></html>",cadena_de_texto_de_Temperatura, cadena_de_texto_de_humedad);

	        int channel = 0;  // Asumiendo que es el canal 0

	        // Prepara el comando para iniciar el envío de datos
	        sprintf(ATcommand, "AT+CIPSEND=%d,%d\r\n", channel, strlen(html));
	        HAL_UART_Transmit(&huart2, (uint8_t *)ATcommand, strlen(ATcommand), 5000);

	        // Verifica que el ESP-01S esté listo para recibir datos (espera un ">")
	        HAL_UART_Receive(&huart2, rxBuffer, sizeof(rxBuffer), 5000);
	        if (strstr((char *)rxBuffer, ">")) {
	            // Envía el contenido HTML con los valores de los sensores al cliente


	            HAL_UART_Transmit(&huart2, (uint8_t *)html, strlen(html), 5000);
	        }

	        // Cierra la conexión con el cliente después de enviar la respuesta
	        sprintf(ATcommand, "AT+CIPCLOSE=%d\r\n", channel);
	        HAL_UART_Transmit(&huart2, (uint8_t *)ATcommand, strlen(ATcommand), 5000);
	        break;
	    }
	}
	    HAL_Delay(2000);  // Espera antes de la siguiente solicitud
	}


//Atencion: Servicio Bloqueante!
void Get_respuesta_Sensores_con_estado(char *cadena_de_texto_de_Temperatura,char *cadena_de_texto_de_humedad, int estado){

 int TIME_DELAY=0;

		if(estado==1){
			// Genera el HTML con los valores de los sensores
			//char html[256];
			sprintf(html, "<html><body><h1  style=\"background-color:green;\">Sistema de Prevencion: ESTABLE</h1><h3>Temperatura:   %s C</h3><h3>Humedad:     %s%</h3></body></html>",cadena_de_texto_de_Temperatura, cadena_de_texto_de_humedad);


			}else if(estado==2){
				// Genera el HTML con los valores de los sensores
				//char html[256];
				sprintf(html, "<html><body><h1  style=\"background-color:yellow;\">Sistema de Prevencion: ALERTA</h1><h3>Temperatura:   %s C</h3><h3>Humedad:     %s%</h3><h2>Alerta: Disminya Actividad</h2></body></html>",cadena_de_texto_de_Temperatura, cadena_de_texto_de_humedad);

			}else{
				// Genera el HTML con los valores de los sensores
					//char html[256];
				    sprintf(html, "<html><body><h1  style=\"background-color:red;\">Sistema de Prevencion: PELIGRO</h1><h3>Temperatura:   %s C</h3><h3>Humedad:     %s%</h3><h2>Peligro: Desista de Actividad</h3></body></html>",cadena_de_texto_de_Temperatura, cadena_de_texto_de_humedad);
			}

		//Arranca conexion
		while (1) {
			TIME_DELAY++;

			if(TIME_DELAY==5){
				break;
			}

		  memset(rxBuffer, 0, sizeof(rxBuffer));  // Limpia el buffer
		    HAL_UART_Receive(&huart2, rxBuffer, sizeof(rxBuffer), 5000);  // Espera la solicitud HTTP

		    TIME_DELAY++;

		if (strstr((char *)rxBuffer, "GET /")) {

		        int channel = 0;  // Asumiendo que es el canal 0

		        // Prepara el comando para iniciar el envío de datos
		        sprintf(ATcommand, "AT+CIPSEND=%d,%d\r\n", channel, strlen(html));
		        HAL_UART_Transmit(&huart2, (uint8_t *)ATcommand, strlen(ATcommand), 5000);

		        // Verifica que el ESP-01S esté listo para recibir datos (espera un ">")
		        HAL_UART_Receive(&huart2, rxBuffer, sizeof(rxBuffer), 5000);
		        if (strstr((char *)rxBuffer, ">")) {
		            // Envía el contenido HTML con los valores de los sensores al cliente


		            HAL_UART_Transmit(&huart2, (uint8_t *)html, strlen(html), 5000);
		        }

		        // Cierra la conexión con el cliente después de enviar la respuesta
		        sprintf(ATcommand, "AT+CIPCLOSE=%d\r\n", channel);
		        HAL_UART_Transmit(&huart2, (uint8_t *)ATcommand, strlen(ATcommand), 5000);
		        break;
		    }
		}
		    HAL_Delay(2000);  // Espera antes de la siguiente solicitud

}



void Get_primera_respuesta_Sensores(char *cadena_de_texto_de_Temperatura,char *cadena_de_texto_de_humedad  ){

	while (1) {
	  memset(rxBuffer, 0, sizeof(rxBuffer));  // Limpia el buffer
	    HAL_UART_Receive(&huart2, rxBuffer, sizeof(rxBuffer), 5000);  // Espera la solicitud HTTP

	Muestra_texto_Parametro("Telefono Conectado");
	Muestra_texto_Parametro_Segundo_renglon(cadena_de_texto_de_Temperatura);
	Muestra_texto_Parametro_Tercer_renglon(cadena_de_texto_de_humedad);

	if (strstr((char *)rxBuffer, "GET /")) {
	        // Genera el HTML con los valores de los sensores
	        char html[256];
	        sprintf(html, "<html><body><h1  style=\"background-color:green;\">SPHC - Sistema de Prevencion</h1><h3>Temperatura:   %s C</h3><h3>Estado:     %s%</h3></body></html>",cadena_de_texto_de_Temperatura, cadena_de_texto_de_humedad);

	        int channel = 0;  // Asumiendo que es el canal 0

	        // Prepara el comando para iniciar el envío de datos
	        sprintf(ATcommand, "AT+CIPSEND=%d,%d\r\n", channel, strlen(html));
	        HAL_UART_Transmit(&huart2, (uint8_t *)ATcommand, strlen(ATcommand), 5000);

	        // Verifica que el ESP-01S esté listo para recibir datos (espera un ">")
	        HAL_UART_Receive(&huart2, rxBuffer, sizeof(rxBuffer), 5000);
	        if (strstr((char *)rxBuffer, ">")) {
	            // Envía el contenido HTML con los valores de los sensores al cliente


	            HAL_UART_Transmit(&huart2, (uint8_t *)html, strlen(html), 5000);
	        }

	        // Cierra la conexión con el cliente después de enviar la respuesta
	        sprintf(ATcommand, "AT+CIPCLOSE=%d\r\n", channel);
	        HAL_UART_Transmit(&huart2, (uint8_t *)ATcommand, strlen(ATcommand), 5000);
	        break;
	    }
	}
	    HAL_Delay(2000);  // Espera antes de la siguiente solicitud
	}



//2026

//Funcion para hacer comandos AT que solo esperan "OK" como respuesta
static uint8_t ESP_SendAT_Wait(const char *cmd, uint32_t timeout_ms)
{
    memset(rxBuffer, 0, sizeof(rxBuffer));
    HAL_UART_Transmit(&huart2, (uint8_t*)cmd, strlen(cmd), timeout_ms);
    HAL_UART_Receive(&huart2, rxBuffer, sizeof(rxBuffer), timeout_ms);
    return (strstr((char*)rxBuffer, "OK") != NULL);
}

static uint8_t ESP_RetryOK(const char *cmd, uint8_t retries, uint32_t timeout_ms, uint32_t delay_ms)
{
    for(uint8_t i=0; i<retries; i++){
        if(ESP_SendAT_Wait(cmd, timeout_ms)) return 1;
        HAL_Delay(delay_ms);
    }
    return 0;
}


static uint8_t ESP_SetWiFiMode(uint8_t mode) // 1=STA, 2=AP, 3=STA+AP
{
    snprintf(ATcommand, sizeof(ATcommand), "AT+CWMODE=%d\r\n", mode);
    return ESP_RetryOK(ATcommand, 5, 5000, 500);
}


static void ESP_BasicReset(void)
{
    ESP_SendAT_Wait("AT\r\n", 5000);
    HAL_Delay(200);

    ESP_SendAT_Wait("AT+RST\r\n", 5000);
    HAL_Delay(2000); // el reset necesita más tiempo
}

static uint8_t ESP_SetSoftAP(const char *ssid, const char *pass)
{
    snprintf(ATcommand, sizeof(ATcommand),
             "AT+CWSAP=\"%s\",\"%s\",1,3,4,0\r\n", ssid, pass);
    return ESP_RetryOK(ATcommand, 5, 5000, 500);
}


static uint8_t ESP_SetAP_IP(const char *ip)
{
    snprintf(ATcommand, sizeof(ATcommand), "AT+CIPAP=\"%s\"\r\n", ip);
    return ESP_RetryOK(ATcommand, 5, 5000, 500);
}

static uint8_t ESP_EnableMux(uint8_t enable) // 0 o 1
{
    snprintf(ATcommand, sizeof(ATcommand), "AT+CIPMUX=%d\r\n", enable);
    return ESP_RetryOK(ATcommand, 5, 5000, 200);
}

static uint8_t ESP_StartServer(uint16_t port)
{
    snprintf(ATcommand, sizeof(ATcommand), "AT+CIPSERVER=1,%d\r\n", port);
    return ESP_RetryOK(ATcommand, 5, 5000, 200);
}

void Wifi_ESP_UpRed_SoftAP(void)
{
    Mensaje_de_conectando();

    ESP_BasicReset();

    if(!ESP_SetWiFiMode(2)) return; // AP
    if(!ESP_SetSoftAP("Wifi de Puca", "12345678")) return;
    if(!ESP_SetAP_IP("192.168.51.1")) return;
    if(!ESP_EnableMux(1)) return;
    if(!ESP_StartServer(80)) return;

   Mensaje_de_Conexion_Exitosa();
    HAL_Delay(2000);
}


static int ESP_ParseLinkID(const char *s)
{
    // Busca "+IPD,"
    const char *p = strstr(s, "+IPD,");
    if(!p) return -1;
    p += 5; // salta "+IPD,"

    // id termina en coma
    int id = 0;
    while(*p >= '0' && *p <= '9'){
        id = id*10 + (*p - '0');
        p++;
    }
    if(*p != ',') return -1;
    return id;
}


static void ESP_HTTP_Send(int link_id, const char *body)
{
    char header[128];
    int body_len = (int)strlen(body);

    // Respuesta HTTP mínima (IMPORTANTE para browsers)
    snprintf(header, sizeof(header),
             "HTTP/1.1 200 OK\r\n"
             "Content-Type: text/html\r\n"
             "Content-Length: %d\r\n"
             "Connection: close\r\n"
             "\r\n", body_len);

    // 1) Pedir CIPSEND del total header+body
    int total_len = (int)strlen(header) + body_len;
    snprintf(ATcommand, sizeof(ATcommand), "AT+CIPSEND=%d,%d\r\n", link_id, total_len);

    memset(rxBuffer,0,sizeof(rxBuffer));
    HAL_UART_Transmit(&huart2, (uint8_t*)ATcommand, strlen(ATcommand), 5000);
    HAL_UART_Receive(&huart2, rxBuffer, sizeof(rxBuffer), 5000);

    // 2) esperar prompt ">"
    if(strstr((char*)rxBuffer, ">")){
        HAL_UART_Transmit(&huart2, (uint8_t*)header, strlen(header), 5000);
        HAL_UART_Transmit(&huart2, (uint8_t*)body, body_len, 5000);
    }

    // 3) cerrar
    snprintf(ATcommand, sizeof(ATcommand), "AT+CIPCLOSE=%d\r\n", link_id);
    HAL_UART_Transmit(&huart2, (uint8_t*)ATcommand, strlen(ATcommand), 5000);
}
static const char *HTML_PORTAL =
"<html><body>"
"<h2>SPHC - Configurar WiFi</h2>"
"<form action=\"/save\" method=\"get\">"
"SSID:<br><input name=\"ssid\" maxlength=\"32\"><br>"
"PASS:<br><input name=\"pass\" maxlength=\"64\" type=\"password\"><br><br>"
"<input type=\"submit\" value=\"Guardar\">"
"</form>"
"</body></html>";

static uint8_t ESP_ParseQueryParam(const char *req, const char *key, char *out, uint16_t out_len)
{
    // busca "key="
    const char *p = strstr(req, key);
    if(!p) return 0;
    p += strlen(key);
    if(*p != '=') return 0;
    p++;

    // copia hasta '&' o ' ' o fin
    uint16_t i=0;
    while(*p && *p!='&' && *p!=' ' && i < (out_len-1)){
        out[i++] = *p++;
    }
    out[i]=0;
    return (i>0);
}

uint8_t Wifi_ESP_PortalLoop_GetCredentials(char *out_ssid, uint16_t ssid_len,
                                          char *out_pass, uint16_t pass_len)
{
    memset(rxBuffer, 0, sizeof(rxBuffer));
    HAL_UART_Receive(&huart2, rxBuffer, sizeof(rxBuffer), 5000);

    if(!strstr((char*)rxBuffer, "+IPD,")) return 0;

    int link_id = ESP_ParseLinkID((char*)rxBuffer);
    if(link_id < 0) return 0;

    // Ruta pedida
    if(strstr((char*)rxBuffer, "GET /save?")){
        char ssid[33]={0};
        char pass[65]={0};

        uint8_t ok1 = ESP_ParseQueryParam((char*)rxBuffer, "ssid", ssid, sizeof(ssid));
        uint8_t ok2 = ESP_ParseQueryParam((char*)rxBuffer, "pass", pass, sizeof(pass));

        if(ok1 && ok2){
            // Copia a salida
            strncpy(out_ssid, ssid, ssid_len-1);
            out_ssid[ssid_len-1]=0;

            strncpy(out_pass, pass, pass_len-1);
            out_pass[pass_len-1]=0;

            ESP_HTTP_Send(link_id,
                "<html><body><h3>Guardado OK</h3>"
                "<p>Reiniciando / Cambiando a Menu Modo...</p></body></html>");

            return 1; // credenciales listas
        }else{
            ESP_HTTP_Send(link_id,
                "<html><body><h3>Error</h3><p>Faltan SSID o PASS</p></body></html>");
            return 0;
        }
    }

    // default: servir portal
    if(strstr((char*)rxBuffer, "GET /")){
        ESP_HTTP_Send(link_id, HTML_PORTAL);
    }

    return 0;
}




// Conexion de Modo en casa

static uint16_t ESP_ReadSome(uint8_t *buf, uint16_t maxlen, uint32_t total_timeout_ms)
{
    uint32_t t0 = HAL_GetTick();
    uint16_t n = 0;
    uint8_t ch;

    while ((HAL_GetTick() - t0) < total_timeout_ms && n < (maxlen - 1))
    {
        if (HAL_UART_Receive(&huart2, &ch, 1, 20) == HAL_OK)
        {
            buf[n++] = ch;
            t0 = HAL_GetTick(); // extiende mientras sigan entrando bytes
        }
    }
    buf[n] = 0;
    return n;
}

static uint8_t ESP_SendAT_WaitStr(const char *cmd, const char *expect, uint32_t timeout_ms)
{
    memset(rxBuffer, 0, sizeof(rxBuffer));
    HAL_UART_Transmit(&huart2, (uint8_t*)cmd, strlen(cmd), 1000);
    ESP_ReadSome(rxBuffer, sizeof(rxBuffer), timeout_ms);
    return (strstr((char*)rxBuffer, expect) != NULL);
}


static uint8_t ESP_JoinAP(const char *ssid, const char *pass)
{
    snprintf(ATcommand, sizeof(ATcommand),
             "AT+CWJAP=\"%s\",\"%s\"\r\n", ssid, pass);

    for (int i = 0; i < 3; i++)
    {
        if (ESP_SendAT_WaitStr(ATcommand, "OK", 20000)) return 1; // puede tardar
        HAL_Delay(1500);
    }
    return 0;
}

static uint8_t ESP_TCP_Get(const char *host, uint16_t port, const char *path)
{
    // 1) abrir TCP
    snprintf(ATcommand, sizeof(ATcommand),
             "AT+CIPSTART=\"TCP\",\"%s\",%u\r\n", host, (unsigned)port);

    if (!ESP_SendAT_WaitStr(ATcommand, "OK", 8000) &&
        !ESP_SendAT_WaitStr(ATcommand, "CONNECT", 8000))
    {
        return 0;
    }

    // 2) armar GET
    char req[256];
    snprintf(req, sizeof(req),
             "GET %s HTTP/1.1\r\n"
             "Host: %s:%u\r\n"
             "Connection: close\r\n"
             "\r\n",
             path, host, (unsigned)port);

    // 3) CIPSEND
    snprintf(ATcommand, sizeof(ATcommand),
             "AT+CIPSEND=%u\r\n", (unsigned)strlen(req));

    if (!ESP_SendAT_WaitStr(ATcommand, ">", 3000)) return 0;

    // 4) enviar request
    HAL_UART_Transmit(&huart2, (uint8_t*)req, strlen(req), 2000);

    // 5) leer respuesta “por ventana” hasta que cierre o timeout
    uint32_t t0 = HAL_GetTick();
    uint8_t got_ipd = 0;
    uint8_t got_200 = 0;

    memset(rxBuffer, 0, sizeof(rxBuffer));

    while ((HAL_GetTick() - t0) < 6000)   // 6s ventana
    {
        // acumulá lo que llegue
        ESP_ReadSome(rxBuffer, sizeof(rxBuffer), 200);

        if (strstr((char*)rxBuffer, "+IPD")) got_ipd = 1;
        if (strstr((char*)rxBuffer, "HTTP/1.1 200")) got_200 = 1;
        if (strstr((char*)rxBuffer, "CLOSED")) break;

        // si ya vimos datos y 200, podés cortar antes
        if (got_ipd && got_200) break;
    }

    // NO cierres de inmediato: dejá que "Connection: close" haga lo suyo
    // (si querés, podés intentar CIPCLOSE al final, pero no es necesario)
    // ESP_SendAT_WaitStr("AT+CIPCLOSE\r\n", "OK", 2000);

    // Criterio robusto:
    // - si vimos 200 => OK
    // - si no vimos 200 pero sí vimos +IPD => el server respondió (OK para health)
    if (got_200) return 1;
    if (got_ipd) return 1;

    return 0;
}

uint8_t Wifi_ESP_UpRed_STA(void)
{
    Mensaje_de_conectando();

    Limpio_Display();
    Muestra_texto_Primer_Renglon("Configurando");
    Muestra_texto_Segundo_renglon("Espere...");

    // AT + Reset
    ESP_SendAT_WaitStr("AT\r\n", "OK", 1000);
    ESP_SendAT_WaitStr("AT+RST\r\n", "OK", 2000);
    HAL_Delay(3000);

    // STA mode
    if (!ESP_SetWiFiMode(1))
    {
        Limpio_Display();
        Muestra_texto_Primer_Renglon("STA FAIL");
        return;
    }

    // Conectar con credenciales ya guardadas en g_ssid/g_pass
    Limpio_Display();
    Muestra_texto_Primer_Renglon("Conectando a...");
    Muestra_texto_Segundo_renglon(g_ssid);

    if (!ESP_JoinAP(g_ssid, g_pass))
    {
        Limpio_Display();
        Muestra_texto_Primer_Renglon("CWJAP FAIL");
        Muestra_texto_Segundo_renglon("SSID/PASS?");
        return;
    }

    // Modo single-conn (simple)
    if (!ESP_SendAT_WaitStr("AT+CIPMUX=0\r\n", "OK", 2000))
    {
        Limpio_Display();
        Muestra_texto_Primer_Renglon("MUX0 FAIL");
        return;
    }

    // (Opcional) IP
    ESP_SendAT_WaitStr("AT+CIFSR\r\n", "OK", 2000);

    // Probar server local: GET /health
    Limpio_Display();
    Muestra_texto_Primer_Renglon("WiFi OK");
    Muestra_texto_Segundo_renglon("Prob SERV");

    if (ESP_TCP_Get("192.168.100.29", 8000, "/health"))
    {
        Limpio_Display();
        Muestra_texto_Primer_Renglon("SERVER OK");
        Muestra_texto_Segundo_renglon("192.168.100.29");
    }
    else
    {
        Limpio_Display();
        Muestra_texto_Primer_Renglon("SERVER FAIL");
        Muestra_texto_Segundo_renglon("Check LAN");
    }
}



//POST
uint8_t ESP_HTTP_Post_Ringo(const char *host, uint16_t port, const char *value_str)
{
    // 1) Abrir TCP
    snprintf(ATcommand, sizeof(ATcommand),
             "AT+CIPSTART=\"TCP\",\"%s\",%u\r\n", host, (unsigned)port);

    if (!ESP_SendAT_WaitStr(ATcommand, "OK", 8000) &&
        !ESP_SendAT_WaitStr(ATcommand, "CONNECT", 8000))
    {
        return 0;
    }

    // 2) JSON body
    char body[128];
    snprintf(body, sizeof(body),
             "{\"name\":\"Ringo\",\"value\":\"%s\"}", value_str);

    // 3) HTTP POST
    char req[384];
    int body_len = strlen(body);

    snprintf(req, sizeof(req),
             "POST /items HTTP/1.1\r\n"
             "Host: %s:%u\r\n"
             "Content-Type: application/json\r\n"
             "Content-Length: %d\r\n"
             "Connection: close\r\n"
             "\r\n"
             "%s",
             host, (unsigned)port, body_len, body);

    // 4) CIPSEND
    snprintf(ATcommand, sizeof(ATcommand),
             "AT+CIPSEND=%u\r\n", (unsigned)strlen(req));

    if (!ESP_SendAT_WaitStr(ATcommand, ">", 3000))
        return 0;

    // 5) Enviar POST
    HAL_UART_Transmit(&huart2, (uint8_t*)req, strlen(req), 4000);

    // 6) Leer respuesta
    uint32_t t0 = HAL_GetTick();
    uint8_t got_201 = 0;

    memset(rxBuffer, 0, sizeof(rxBuffer));

    while ((HAL_GetTick() - t0) < 6000)
    {
        ESP_ReadSome(rxBuffer, sizeof(rxBuffer), 200);

        if (strstr((char*)rxBuffer, "201"))
        {
            got_201 = 1;
            break;
        }

        if (strstr((char*)rxBuffer, "CLOSED"))
            break;
    }

    return got_201;
}

uint8_t ESP_HTTP_Post_Item(const char *host, uint16_t port,
                           const char *name,
                           const char *value_str)
{
    // 1) Abrir TCP
    snprintf(ATcommand, sizeof(ATcommand),
             "AT+CIPSTART=\"TCP\",\"%s\",%u\r\n", host, (unsigned)port);

    if (!ESP_SendAT_WaitStr(ATcommand, "OK", 8000) &&
        !ESP_SendAT_WaitStr(ATcommand, "CONNECT", 8000))
    {
        return 0;
    }

    // 2) JSON body
    char body[192];  // amplié un poco el buffer por seguridad
    snprintf(body, sizeof(body),
             "{\"name\":\"%s\",\"value\":\"%s\"}", name, value_str);

    // 3) HTTP POST
    char req[480];   // también ampliado
    int body_len = strlen(body);

    snprintf(req, sizeof(req),
             "POST /items HTTP/1.1\r\n"
             "Host: %s:%u\r\n"
             "Content-Type: application/json\r\n"
             "Content-Length: %d\r\n"
             "Connection: close\r\n"
             "\r\n"
             "%s",
             host, (unsigned)port, body_len, body);

    // 4) CIPSEND
    snprintf(ATcommand, sizeof(ATcommand),
             "AT+CIPSEND=%u\r\n", (unsigned)strlen(req));

    if (!ESP_SendAT_WaitStr(ATcommand, ">", 3000))
        return 0;

    // 5) Enviar POST
    HAL_UART_Transmit(&huart2, (uint8_t*)req, strlen(req), 4000);

    // 6) Leer respuesta
    uint32_t t0 = HAL_GetTick();
    uint8_t got_201 = 0;

    memset(rxBuffer, 0, sizeof(rxBuffer));

    while ((HAL_GetTick() - t0) < 6000)
    {
        ESP_ReadSome(rxBuffer, sizeof(rxBuffer), 200);

        if (strstr((char*)rxBuffer, "201"))
        {
            got_201 = 1;
            break;
        }

        if (strstr((char*)rxBuffer, "CLOSED"))
            break;
    }

    return got_201;
}


