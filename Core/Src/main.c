/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "string.h"
#include "stdio.h"
#include "Servicio_Temperatura_Corporea.h"
#include "Servicio_Display.h"
#include "Funciones_de_Apoyo_intermedio.h"
#include "servicio_ledRGB.h"
#include "Servicio_Wifi_ESP_01.h"
#include "Servicio_NVM.h"
//#include "esp_01_sleep.h" Inavilitado

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;

UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */

//Aqui van las interrupciones.
volatile uint8_t btn14_event = 0;
volatile uint8_t btn15_event = 0;

static uint32_t last14_ms = 0;
static uint32_t last15_ms = 0;
static const uint32_t DEBOUNCE_MS = 30;




/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_I2C1_Init(void);
static void MX_USART2_UART_Init(void);

uint8_t Hay_Evento_Botones(void)
{
    return (btn14_event || btn15_event);
}

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
//Variables

//DS18B20 obtencion de temperatura
float temperatura_corporal_canido;
char temperatura_corporal_canido_texto[10];

//NVM Kectura de memoria FLASH
char g_ssid[NVM_SSID_MAX+1] = {0};
char g_pass[NVM_PASS_MAX+1] = {0};
char g_name[NVM_NAME_MAX+1] = {0};

// FLAG= Bandera de Credenciales si/no
uint8_t cred_ok = 0;


// EStados
uint8_t estado_prev = 0;


// Estados de la FSM
typedef enum {
  Q0_MENU = 0,
  Q1_MODO_PASEO,
  Q2_MODO_CASA,
  Q3_MODO_NEW_WIFI,
  Q4_MODO_CONFIG
} estado_t;

estado_t estado = Q0_MENU;



/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART1_UART_Init();
  MX_I2C1_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */

  NVM_Creds_t cfg; //Guardado en Flash
  //NVM_EraseAll(); //Esta funcion se uso en estados de prueba para resetear la FLASH

  LED_RGB_Init();  //dejo led apagado hasta modo paseo


  Inicializo_Display();
  SaludoInicial();
  HAL_Delay(2000);

  //INIT
  //Consultar si teiene credenciales Guardas
  if (NVM_LoadCreds(&cfg)) {
      // Confirmación en display
	  strncpy(g_ssid, cfg.ssid, sizeof(g_ssid)-1); g_ssid[sizeof(g_ssid)-1] = '\0';
	  strncpy(g_pass, cfg.pass, sizeof(g_pass)-1); g_pass[sizeof(g_pass)-1] = '\0';
	  strncpy(g_name, cfg.name, sizeof(g_name)-1); g_name[sizeof(g_name)-1] = '\0';

	  Limpio_Display();
      Muestra_texto_Primer_Renglon("Credenciales OK");
      Muestra_texto_Segundo_renglon(cfg.ssid);
      Muestra_texto_Tercer_renglon(cfg.pass);
      HAL_Delay(1800);


      estado = Q0_MENU;

  } else {
      Limpio_Display();
      Muestra_texto_Primer_Renglon("Sin Credenciales");
      Muestra_texto_Segundo_renglon("Ir a config WiFi");
      HAL_Delay(1800);

      // Defino el estado para ir a configuracion del WIFI
      estado = Q3_MODO_NEW_WIFI;
  }
  strncpy(g_name, "Puca", sizeof(g_name)-1);
  Limpio_Display();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  // Entradas A y B (eventos latcheados por EXTI)
	  uint8_t A_prev  = (btn14_event != 0);
	  uint8_t B_prev  = (btn15_event != 0);
	  // Consumimos eventos (así A/B valen 1 solo una vez)
	  btn14_event = 0;
	  btn15_event = 0;

	  switch (estado)
	  {
	  //MENU
	  case Q0_MENU:

	      Limpio_Display();
	      Muestra_texto_Primer_Renglon("MENU");
	      Muestra_texto_Segundo_renglon("A=Paseo  B=Casa");
	      estado_prev=0;
	      //  Si ya hubo click previo, NO espero y me voy al evento
	      if (!A_prev && !B_prev)
	      {
	        uint16_t i;
	        for (i = 0; i < 100; i++) {      // dejar en 700
	          if (btn14_event || btn15_event) break;
	          HAL_Delay(50);
	        }
	      }

	      // Capturo clicks DURANTE la espera
	      uint8_t A_wait = (btn14_event != 0);
	      uint8_t B_wait = (btn15_event != 0);

	      // Consumo lo ocurrido durante la espera
	      btn14_event = 0;
	      btn15_event = 0;

	      // evdento total = evento previo OR evento drante espera
	      uint8_t A = (A_prev || A_wait);
	      uint8_t B = (B_prev || B_wait);

	      if (B)      { estado_prev = 0; estado = Q2_MODO_CASA; }
	      else if (A) { estado_prev = 0; estado = Q1_MODO_PASEO; }
	      else        { estado_prev = 0; estado = Q2_MODO_CASA; }  // timeout

	      break;

	    //MODO PASEO
	    case Q1_MODO_PASEO:

	    	    if(estado_prev!=1){
	    	    	Limpio_Display();
	    	    	Muestra_texto_Primer_Renglon("Modo Paseo");
	    	    	Limpio_Display();
	    	    	Wait_New();
	    	    }

	    	    estado_prev=1;

	    	    // Si había evento previo, saltar
	    	    if (A_prev) {
	    	        estado = Q2_MODO_CASA;   // tu regla actual
	    	        break;                  // mejor que continue dentro del switch
	    	    }

	    	    // Medición
	    	    temperatura_corporal_canido = Get_Temperatura_Promedio_Canido();
	    	    floatToString(temperatura_corporal_canido, temperatura_corporal_canido_texto, 3);

	    	    // ESTABLE
	    	    if (temperatura_corporal_canido <= 32.0f)
	    	    {
	    	        Muestra_texto_Primer_Renglon("ESTABLE");
	    	        Muestra_texto_Segundo_renglon("Temperatura:");
	    	        Muestra_texto_Tercer_renglon(temperatura_corporal_canido_texto);

	    	        // Limpio eventos antes de esperar
	    	        btn14_event = 0;
	    	        btn15_event = 0;

	    	        // Espera 7s pero con salida si hay evento
	    	        Espera_Con_Salida(7000, Hay_Evento_Botones);

	    	        // Si querés: si se presionó algo, decidir transición acá (opcional)
	    	         uint8_t A = (btn14_event != 0);
	    	       //  uint8_t B = (btn15_event != 0);
	    	         btn14_event = 0; btn15_event = 0;
	    	          if (A) { estado = Q2_MODO_CASA; break; }
	    	        // if (B) { estado = Q2_MODO_CASA; break; }

	    	        continue;
	    	    }
	    	    // ALERTA
	    	    else if (temperatura_corporal_canido > 32.0f && temperatura_corporal_canido < 36.0f)
	    	    {
	    	        Muestra_texto_Primer_Renglon("ALERTA!");
	    	        Muestra_texto_Segundo_renglon("Temperatura:");
	    	        Muestra_texto_Tercer_renglon(temperatura_corporal_canido_texto);

	    	        btn14_event = 0;
	    	        btn15_event = 0;

	    	        Espera_Con_Salida(3000, Hay_Evento_Botones);
	    	         uint8_t A = (btn14_event != 0);
	    	        // uint8_t B = (btn15_event != 0);
	    	         btn14_event = 0; btn15_event = 0;
	    	          if (A) { estado = Q2_MODO_CASA; break; }

	    	        continue;
	    	    }
	    	    // PELIGRO
	    	    else // temperatura_corporal_canido >= 36.0f
	    	    {
	    	        Muestra_texto_Primer_Renglon("PELIGRO");
	    	        Muestra_texto_Segundo_renglon("Temperatura: ALTA");
	    	        Muestra_texto_Tercer_renglon(temperatura_corporal_canido_texto);

	    	        continue;
	    	    }

	    	 break;




	    case Q2_MODO_CASA:

	    	 // Mensaje solo al entrar (evita parpadeo)
	    	    if (estado_prev != 2) {
	    	        Limpio_Display();
	    	        Muestra_texto_Primer_Renglon("Modo Casa");
	    	        Limpio_Display();

	    	    }
	    	    estado_prev = 2;
	    	    temperatura_corporal_canido = Get_Temperatura_Promedio_Canido();
	    	    floatToString(temperatura_corporal_canido, temperatura_corporal_canido_texto, 3);
    	        // Levantar red STA SOLO UNA VEZ al entrar
    	        Wifi_ESP_UpRed_STA();
    	        Wait_New();


	    	    // --- En este punto, el usuario puede querer salir YA ---
	    	    // Capturo evento previo (del inicio del while)
	    	    if (A_prev) { estado = Q1_MODO_PASEO; break; }
	    	    if (B_prev) { estado = Q4_MODO_CONFIG; break; }

	    	    // --- POST ---
	    	    // (si tu post tarda, esto ya es el "bloqueo" inevitable, pero evitamos delays extra)
	    	    Limpio_Display();
	    	    Muestra_texto_Primer_Renglon("Enviando POST...");
	    	    Muestra_texto_Segundo_renglon("Servidor 192.168...");

	    	    if (ESP_HTTP_Post_Item("192.168.100.29",8000, g_name,temperatura_corporal_canido_texto))
	    	    {
	    	        Limpio_Display();
	    	        Muestra_texto_Primer_Renglon("POST OK");
	    	        Muestra_texto_Segundo_renglon("Ringo 27.7");
	    	    }
	    	    else
	    	    {
	    	        Limpio_Display();
	    	        Muestra_texto_Primer_Renglon("POST FAIL");
	    	        Muestra_texto_Segundo_renglon("Reintento...");
	    	    }

	    	    // --- Espera corta con salida (anti-bloqueo y anti-rebote) ---
	    	    // Durante esta ventana, el sistema "escucha" botones
	    	    btn14_event = 0;
	    	    btn15_event = 0;

	    	    Espera_Con_Salida(1200, Hay_Evento_Botones);  // 1.2s mostrás resultado sin congelarte “ciego”

	    	     A = (btn14_event != 0);
	    	     B = (btn15_event != 0);
	    	    btn14_event = 0;
	    	    btn15_event = 0;

	    	    if (A) { estado = Q1_MODO_PASEO; break; }
	    	    if (B) { estado = Q4_MODO_CONFIG; break; }

	    	    // Si no hubo botones, seguís en CASA (próximo ciclo repetirá POST según tu diseño)
	    	    break;


	    case Q3_MODO_NEW_WIFI:
	     // LED_RGB_SetColor(LED_RGB_GREEN);
	        if(estado_prev!=3){
	    	    	    	Limpio_Display();
	    	    	    	Muestra_texto_Primer_Renglon("Modo Conf Red");
	    	    	    	Limpio_Display();
	    	    	    	Wait_New();
	    	    	    }
	        estado_prev=3;

	    	Limpio_Display();
	    	    Muestra_texto_Primer_Renglon("Por favor espere...");
	    	    Muestra_texto_Segundo_renglon(".../...");
	    	    HAL_Delay(1000);

	    	    cred_ok = 0;

	    	    Wifi_ESP_UpRed_SoftAP();

	    	    while (cred_ok == 0)
	    	    {
	    	        LED_RGB_SetColor(LED_RGB_MAGENTA);

	    	        Limpio_Display();
	    	        Muestra_texto_Primer_Renglon("MODO - NEW WIFI");
	    	        Muestra_texto_Segundo_renglon("Cargar WiFi en");
	    	        Muestra_texto_Tercer_renglon("192.168.51.1");

	    	        cred_ok = Wifi_ESP_PortalLoop_GetCredentials(
	    	                      g_ssid, sizeof(g_ssid),
	    	                      g_pass, sizeof(g_pass));

	    	        HAL_Delay(50);
	    	    }

	    	    // Nombre fijo por ahora
	    	    strncpy(g_name, "Puh-ra", sizeof(g_name)-1);
	    	    g_name[sizeof(g_name)-1] = '\0';

	    	    NVM_Creds_t cfg_new;
	    	    memset(&cfg_new, 0, sizeof(cfg_new));

	    	    strncpy(cfg_new.ssid, g_ssid, NVM_SSID_MAX-1);
	    	    strncpy(cfg_new.pass, g_pass, NVM_PASS_MAX-1);
	    	    strncpy(cfg_new.name, g_name, NVM_NAME_MAX-1);

	    	    if (cfg_new.ssid[0] == '\0') {
	    	        Limpio_Display();
	    	        Muestra_texto_Primer_Renglon("SSID vacio");
	    	        HAL_Delay(1200);
	    	    } else {
	    	        Limpio_Display();
	    	        Muestra_texto_Primer_Renglon("Guardando FLASH...");
	    	        if (NVM_SaveCreds(&cfg_new)) {
	    	            Limpio_Display();
	    	            Muestra_texto_Primer_Renglon("FLASH OK");
	    	            Muestra_texto_Segundo_renglon(cfg_new.ssid);
	    	            Muestra_texto_Tercer_renglon(cfg_new.ssid);
	    	            HAL_Delay(1200);

	    	            // ya quedan en RAM también
	    	            strncpy(g_ssid, cfg_new.ssid, sizeof(g_ssid)-1); g_ssid[sizeof(g_ssid)-1] = '\0';
	    	            strncpy(g_pass, cfg_new.pass, sizeof(g_pass)-1); g_pass[sizeof(g_pass)-1] = '\0';
	    	            strncpy(g_name, cfg_new.name, sizeof(g_name)-1); g_name[sizeof(g_name)-1] = '\0';

	    	            estado = Q0_MENU;   // salir del modo
	    	        } else {
	    	            Limpio_Display();
	    	            Muestra_texto_Primer_Renglon("ERROR FLASH");
	    	            HAL_Delay(1500);
	    	        }
	    	    }

	    	    break;

	    case Q4_MODO_CONFIG:

	   	      Limpio_Display();
	   	      Muestra_texto_Primer_Renglon("MENU RED");
	   	      Muestra_texto_Segundo_renglon("A=Config  B=Volver");

	   	      //  Si ya hubo click previo, NO espero y me voy al evento
	   	      if (!A_prev && !B_prev)
	   	      {
	   	        uint16_t i;
	   	        for (i = 0; i < 700; i++) {      // 5s (100*50ms)
	   	          if (btn14_event || btn15_event) break;
	   	          HAL_Delay(50);
	   	        }
	   	      }

	   	      // Capturo clicks DURANTE la espera
	   	      A_wait = (btn14_event != 0);
	   	      B_wait = (btn15_event != 0);

	   	      // Consumo lo ocurrido durante la espera
	   	      btn14_event = 0;
	   	      btn15_event = 0;

	   	      // evdento total = evento previo OR evento drante espera
	   	       A = (A_prev || A_wait);
	   	       B = (B_prev || B_wait);

	   	    if (B) {
	   	           // volver al estado previo real
	   	           if (estado_prev == 1) estado = Q1_MODO_PASEO;
	   	           else if (estado_prev == 2) estado = Q2_MODO_CASA;
	   	           else estado = Q0_MENU;
	   	       }
	   	       else if (A) {
	   	           estado = Q3_MODO_NEW_WIFI;
	   	       }
	   	       else {
	   	           // timeout vuelve
	   	           if (estado_prev == 1) estado = Q1_MODO_PASEO;
	   	           else if (estado_prev == 2) estado = Q2_MODO_CASA;
	   	           else estado = Q0_MENU;
	   	       }

	   	      break;

	    	}


    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 400000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_HalfDuplex_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_12, GPIO_PIN_SET);

  /*Configure GPIO pin : PA7 */
  GPIO_InitStruct.Pin = GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PB0 PB1 PB12 */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_12;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : PB14 PB15 */
  GPIO_InitStruct.Pin = GPIO_PIN_14|GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  uint32_t now = HAL_GetTick();

  if (GPIO_Pin == GPIO_PIN_14) {
    if ((now - last14_ms) > DEBOUNCE_MS) {
      btn14_event = 1;
      last14_ms = now;
    }
  }
  else if (GPIO_Pin == GPIO_PIN_15) {
    if ((now - last15_ms) > DEBOUNCE_MS) {
      btn15_event = 1;
      last15_ms = now;
    }
  }
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
