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
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
//Variables
float temperatura_corporal_canido;
float temp_izquierda;
float temp_derecha;
char temperatura_corporal_canido_texto[10];
char g_ssid[NVM_SSID_MAX+1] = {0};
char g_pass[NVM_PASS_MAX+1] = {0};
char g_name[NVM_NAME_MAX+1] = {0};
uint8_t cred_ok = 0;


typedef enum {
  Q0_BLANCO = 0,
  Q1_AZUL,
  Q2_ROJO,
  Q3_VERDE
} estado_t;

estado_t estado = Q0_BLANCO;
// char temperatura_corporal_canido_texto[10];


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
  //NVM_EraseAll();

  LED_RGB_Init();  //dejo led apagado
  //LED_RGB_SetColor(LED_RGB_WHITE);

  Inicializo_Display();
  SaludoInicial();
  HAL_Delay(2000);

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

      // Copiar cfg a tu módulo WiFi si corresponde
      // WIFI_SetCreds(cfg.ssid, cfg.pass);

      estado = Q1_AZUL;

  } else {
      Limpio_Display();
      Muestra_texto_Primer_Renglon("SIN WIFI");
      Muestra_texto_Segundo_renglon("Configurado");
      HAL_Delay(1800);

      // No hace falta reset acá
      estado = Q3_VERDE;
  }


//  Wifi_ESP_UpRed_SoftAP(); //Configuro la


//Levanto la clave de WIFI
 /* while(cred_ok == 0)
  {
	  //LED indica modo
	  LED_RGB_SetColor(LED_RGB_MAGENTA);
      // opcional: mostrar algo en display
	  Limpio_Display();
	  Muestra_texto_Primer_Renglon("MODO - NEW WIFI");
	  Muestra_texto_Segundo_renglon("Cargar WiFi en");
	  Muestra_texto_Tercer_renglon("192.168.51.1");

      cred_ok = Wifi_ESP_PortalLoop_GetCredentials(
                    g_ssid, sizeof(g_ssid),
                    g_pass, sizeof(g_pass));

      // Pequeño delay para no saturar (opcional)
      estado = Q1_AZUL;
      HAL_Delay(50);
  }*/
// LLamo a mi Funcion de obtener la temperatura promedio
  	  /*Limpio_Display();
  	  Muestra_texto_Primer_Renglon(g_ssid);
  	  Muestra_texto_Segundo_renglon(g_pass);
  	  HAL_Delay(2000);
  	  LED_RGB_SetColor(LED_RGB_WHITE);
  	  HAL_Delay(2000);
  	  Wifi_ESP_UpRed_STA();*/


 // temperatura_corporal_canido=Get_Temperatura_Sensor_Izquierdo();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  Limpio_Display();
	  Muestra_texto_Primer_Renglon("Modo");
	  HAL_Delay(2000);
	  // Entradas A y B (eventos latcheados por EXTI)
	  uint8_t A = (btn14_event != 0);
	  uint8_t B = (btn15_event != 0);

	  // Consumimos eventos (así A/B valen 1 solo una vez)
	  btn14_event = 0;
	  btn15_event = 0;

	  // A=0 y B=0 -> no pasó nada
	  if (!A && !B) {
	    HAL_Delay(20);   // tu "representación" de A=0,B=0
	  }

	  switch (estado)
	  {
	    case Q0_BLANCO:
	      LED_RGB_SetColor(LED_RGB_WHITE);

	      if (A) estado = Q1_AZUL;     // A=1 -> Q1
	      else if (B) estado = Q2_ROJO; // B=1 -> Q2
	      break;

	    case Q1_AZUL:

	    	Limpio_Display();
	        Muestra_texto_Primer_Renglon("Modo Casa");
	        Wifi_ESP_UpRed_STA();
	        HAL_Delay(2000);
	      LED_RGB_SetColor(LED_RGB_BLUE);
	      if (ESP_HTTP_Post_Ringo("192.168.100.29", 8000, "27.7"))
	      {
	          Limpio_Display();
	          Muestra_texto_Primer_Renglon("POST OK");
	          Muestra_texto_Segundo_renglon("Ringo 27.7");
	          HAL_Delay(2000);
	      }
	      else
	      {
	          Limpio_Display();
	          Muestra_texto_Primer_Renglon("POST FAIL");
	          HAL_Delay(2000);
	      }
	      estado = Q1_AZUL;     // A
	     // if (B) estado = Q2_ROJO;     // B=1 -> Q2
	      //else if (A) estado = Q3_VERDE; // A=1 -> Q3 (según tu esquema)
	      break;

	    case Q2_ROJO:
	      LED_RGB_SetColor(LED_RGB_RED);

	      if (A) estado = Q1_AZUL;     // A=1 -> Q1
	      break;

	    case Q3_VERDE:
	     // LED_RGB_SetColor(LED_RGB_GREEN);
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

	    	            estado = Q1_AZUL;   // salir del modo config
	    	        } else {
	    	            Limpio_Display();
	    	            Muestra_texto_Primer_Renglon("ERROR FLASH");
	    	            HAL_Delay(1500);
	    	        }
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
