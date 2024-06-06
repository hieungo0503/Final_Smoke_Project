/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
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
#include <ViettelSDK_Device_Management.h>
#include "BM22S2021_1.h"
#include <sht3x.h>
#include "stdlib.h"
#include "stdio.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

struct ViettelSDK sdk_handler;
struct Smoke_Data smoke_handler;


/* Data Global Variables */
//volatile uint32_t pulse;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define SMOKE_EN 1
#define SHT3X_EN 1
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;

RTC_HandleTypeDef hrtc;

TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim6;

UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;
DMA_HandleTypeDef hdma_usart1_rx;
DMA_HandleTypeDef hdma_usart2_rx;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_RTC_Init(void);
static void MX_TIM2_Init(void);
static void MX_DMA_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_I2C1_Init(void);
static void MX_TIM6_Init(void);
/* USER CODE BEGIN PFP */

//extern void initialise_monitor_handles(void);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

uint8_t test_case = 4; //Disable test mode

void ConfigTimerPeriod(TIM_HandleTypeDef *htim, uint16_t Time);
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
	if(huart == &huart1)
	{
		saveResponse(&sdk_handler, huart, Size);

		if(sdk_handler.StopMode == 1 && sdk_handler.sleep == false)
		{
			Exit_Stop1Mode(&sdk_handler, &huart1, &huart2);
			sdk_handler.StopMode = 0;
		}
	}
	else if (huart == &huart2)
	{
		saveSmokeRespond(&smoke_handler, huart, Size);
		if(test_case == 3)
			smoke_handler.AlarmSatus = 1;

		if(sdk_handler.coap_params.ReportStatus == 0 && sdk_handler.testCase !=0)
		{
			smoke_handler.AlarmSatus = 0;
			test_case = 0;
			ConfigTimerPeriod(&htim6, 200);
	  		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, 0);
	  		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, 0);
	  		sdk_handler.coap_params.ReportStatus = 1;
		}

		if(sdk_handler.StopMode == 1)
		{
			Exit_Stop1Mode(&sdk_handler, &huart1, &huart2);
			sdk_handler.StopMode = 0;
		}
	}
}

//Buton Run

uint32_t timestatrt = 0;
bool state = true;
bool btn_test = false;
uint16_t count = 0;

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if(GPIO_Pin == GPIO_PIN_5 && state == true){
		btn_test = true;
		HAL_TIM_Base_Start_IT(&htim2);
		state = false;

		timestatrt = HAL_GetTick();
	}
	else{
		__NOP();
	}
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(htim);

  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_TIM_PeriodElapsedCallback could be implemented in the user file
   */
  if(htim == &htim2){
	if(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_5) == GPIO_PIN_RESET){
		//HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_7);
		state = true;
		count ++;
		HAL_TIM_Base_Stop_IT(&htim2);
	}
  }

  else if(htim == &htim6)
  {
	  HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_7);
  }

}


//Another funcion

sht3x_handle_t handle = {
    .i2c_handle = &hi2c1,
    .device_address = SHT3X_I2C_DEVICE_ADDRESS_ADDR_PIN_LOW
};

float temperature, humidity;

int _write(int file, char *ptr, int len)
 {
	 int DataIdx;
	 for (DataIdx = 0; DataIdx < len; DataIdx++)
	 {
		 ITM_SendChar(*ptr++);
	 }
	 return len;
 }

void READ_SHT30_SENSOR(void);
void ConfigStop1ModeUART(struct ViettelSDK *self, UART_HandleTypeDef *huart);


/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
	if (!sht3x_init(&handle)) {
	    printf("SHT3x access failed.\n\r");
	}
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

  MX_RTC_Init();
  MX_TIM2_Init();
  MX_DMA_Init();

  MX_I2C1_Init();
  MX_TIM6_Init();
  MX_USART1_UART_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */
//  if (WATCHDOG_TIMER)
//  	{
//  		MX_IWDG_Init();
//  	}

  	/* USER CODE BEGIN 2 */

  	initializeSDK(&sdk_handler,NULL, &huart1, &hdma_usart1_rx, GPIOA,
  	GPIO_PIN_7);

  	//ConfigStop1ModeUART(&sdk_handler, &huart1);
//  	resetDMAInterrupt(&sdk_handler);

  	setupCOAP_Parameters(&sdk_handler, "115.78.92.253",23025, 0);

  	addDeviceID(&sdk_handler, "device_mYSg1M");

  	if(SMOKE_EN){
  	initialSomke(&smoke_handler, &huart2, &hdma_usart2_rx, 41, GPIOA, GPIO_PIN_4, 2000);
    resetDMAInterrupt_BM22(&smoke_handler);
    ConfigStop1ModeUART(&sdk_handler, &huart2);
  	}

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
//	  if(SWO_DEBUG)
//	  printf("Hello world\n");

	  if((sdk_handler.sleep == false && sdk_handler.testCase == 0) || smoke_handler.AlarmSatus)	//Periodical
	  {
		  if(SHT3X_EN)
		  READ_SHT30_SENSOR();

		  connectToPlatform(&sdk_handler,&smoke_handler);
	  }
	  else if (test_case == 2)
	  {
		  if(SHT3X_EN)
		  READ_SHT30_SENSOR();

		  connectToPlatform(&sdk_handler,&smoke_handler);

		  ConfigTimerPeriod(&htim6, 200);		//wait for another test_case

		  test_case = 0;
	  }

	  if(smoke_handler.AlarmSatus == false && btn_test != true && test_case == 4)
	  {
		  Enter_Stop1Mode(&sdk_handler, &huart1, &huart2);
	  }

	  if(btn_test == true && HAL_GetTick() - timestatrt > 400)
	  {
		  switch(count)
		  {
		  	  case 1:
		  		// Do led blink to anounce that device have online and ready to test
		  		  ConfigTimerPeriod(&htim6, 200);

				  printf("count = 1 \n");
				  test_case = 0;
				  sdk_handler.testCase = 1;
				  count = 0;
				  break;
		  	  case 2:
		  		// Do test for report data (led blink at 500ms and after complete blink follow to mos and off)
		  		  ConfigTimerPeriod(&htim6, 500);

		  		  sdk_handler.sleep = false; //wake up module
		  		  test_case = 2;
		  		  sdk_handler.testCase = 1;

		  		  printf("count = 2 \n");
				  count = 0;
				  break;
		  	  case 3:
		  		// Do test alarm buzzer (buzzer on, led blink 1s)
		  		  ConfigTimerPeriod(&htim6, 1000);

    		  	  sdk_handler.sleep = false; //wake up module
    		  	  test_case = 3;
    		  	  sdk_handler.testCase = 1;
		  		  smoke_handler.AlarmSatus = true;

		  		  printf("count = 3 \n");
		  		  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, 1);
		  		  count = 0;
		  		  break;
		  	  default:

		  		HAL_TIM_Base_Stop_IT(&htim6);
		  		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, 0);
		  		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, 0);
		  		smoke_handler.AlarmSatus = false;
		  		sdk_handler.testCase = 0;
		  		test_case = 4;
		  		count = 0;
		  		  break;
		  }

		  btn_test = false;
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

  /** Configure the main internal regulator output voltage
  */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_LSI
                              |RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSICalibrationValue = 0;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_7;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_MSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
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
  hi2c1.Init.Timing = 0x2000090E;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Analogue filter
  */
  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c1, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Digital filter
  */
  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c1, 0) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief RTC Initialization Function
  * @param None
  * @retval None
  */
static void MX_RTC_Init(void)
{

  /* USER CODE BEGIN RTC_Init 0 */

  /* USER CODE END RTC_Init 0 */

  /* USER CODE BEGIN RTC_Init 1 */

  /* USER CODE END RTC_Init 1 */
  /** Initialize RTC Only
  */
  hrtc.Instance = RTC;
  hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
  hrtc.Init.AsynchPrediv = 127;
  hrtc.Init.SynchPrediv = 255;
  hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
  hrtc.Init.OutPutRemap = RTC_OUTPUT_REMAP_NONE;
  hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
  hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
  hrtc.Init.OutPutPullUp = RTC_OUTPUT_PULLUP_NONE;
  if (HAL_RTC_Init(&hrtc) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN RTC_Init 2 */

  /* USER CODE END RTC_Init 2 */

}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 7999;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 49;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */

}

/**
  * @brief TIM6 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM6_Init(void)
{

  /* USER CODE BEGIN TIM6_Init 0 */

  /* USER CODE END TIM6_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM6_Init 1 */

  /* USER CODE END TIM6_Init 1 */
  htim6.Instance = TIM6;
  htim6.Init.Prescaler = 7999;
  htim6.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim6.Init.Period = 20000;
  htim6.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim6) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim6, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM6_Init 2 */

  /* USER CODE END TIM6_Init 2 */

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
  huart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart1) != HAL_OK)
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
  huart2.Init.BaudRate = 9600;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA2_CLK_ENABLE();
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Channel6_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel6_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel6_IRQn);
  /* DMA2_Channel7_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA2_Channel7_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA2_Channel7_IRQn);

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, Buzzer_Pin|LED_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, MCU_PSMe_Pin|Reset_Module_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : RI_Pin_Pin */
  GPIO_InitStruct.Pin = RI_Pin_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(RI_Pin_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : PA4 */
  GPIO_InitStruct.Pin = GPIO_PIN_4;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : button_Pin */
  GPIO_InitStruct.Pin = button_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(button_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : Buzzer_Pin LED_Pin */
  GPIO_InitStruct.Pin = Buzzer_Pin|LED_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : MCU_PSMe_Pin Reset_Module_Pin */
  GPIO_InitStruct.Pin = MCU_PSMe_Pin|Reset_Module_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI9_5_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);

}

/* USER CODE BEGIN 4 */

void READ_SHT30_SENSOR(void)
{
	  sht3x_read_temperature_and_humidity(&handle, &temperature, &humidity);
//	  printf("Initial temperature: %.2fC, humidity: %.2f%%RH\n\r", temperature, humidity);

	  // Enable heater for two seconds.
	  sht3x_set_header_enable(&handle, true);
	  HAL_Delay(2000);
	  sht3x_set_header_enable(&handle, false);

	  // Read temperature and humidity again.
	  sht3x_read_temperature_and_humidity(&handle, &temperature, &humidity);

	  smoke_handler.temperature = temperature;
	  smoke_handler.humidity = humidity;

}

void ConfigStop1ModeUART(struct ViettelSDK *self, UART_HandleTypeDef *huart)
{

	/* make sure that no UART transfer is on-going */
	  while(__HAL_UART_GET_FLAG(huart, USART_ISR_BUSY) == SET);
	  /* make sure that UART is ready to receive
	   * (test carried out again later in HAL_UARTEx_StopModeWakeUpSourceConfig) */
	  while(__HAL_UART_GET_FLAG(huart, USART_ISR_REACK) == RESET);

	  /* set the wake-up event:
	   * specify wake-up on RXNE flag */
	  WakeUpSelection.WakeUpEvent = UART_WAKEUP_ON_STARTBIT;
	  if (HAL_UARTEx_StopModeWakeUpSourceConfig(huart, WakeUpSelection)!= HAL_OK)
	  {
	    Error_Handler();
	  }
	  /* Enable the UART Wake UP from STOP1 mode Interrupt */
	  __HAL_UART_ENABLE_IT(huart, UART_IT_WUF);
	  //Enable Receive IT

	  /* about to enter STOP1 mode: */
	  /* enable MCU wake-up by UART */
	  HAL_UARTEx_EnableStopMode(huart);

	  sprintf(self->log_content, "Config Stop1 Mode For Uart Success \n");
	  writeLog(self, LOG_INFO, self->log_content, true);
}

void ConfigTimerPeriod(TIM_HandleTypeDef *htim, uint16_t Time)
{
	  HAL_TIM_Base_Stop_IT(htim);
	  htim->Init.Period = Time;
	  HAL_TIM_Base_Init(htim);
	  HAL_TIM_Base_Start_IT(htim);
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

