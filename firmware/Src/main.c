/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "cmsis_os.h"
#include "adc.h"
#include "dma.h"
#include "rtc.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include "st7789.h"
#include "st7789_1.h"
#include "fonts.h"
#include "EC11.h"
#include "utils.h"
#include "usart1.h"

#include "Fan.hpp"
//#include "iwdg.h"
#include "app_globals.h"
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

/* USER CODE BEGIN PV */

USART_ModBus USART1_Modbus; ///< структура для обмена по Modbus UART1 Slave
USART_ModBus USART2_Modbus; ///< структура для обмена по Modbus UART2 Master
uint8_t bound; ///< скорость обмена 0  - 9600; 1 - 19200;  x - 38400
ModbusHVACx  modbusHVAC1;                   ///< Структуры точек обмена Modbus для текущего HVAC

ModbusRTU_t modbus1;   ///< структура для обмена по Modbus UART1 Slave


uint16_t g_UpDate_Count;              ///< time update data on the screen

//uint8_t g_mask_Buttons;               ///< Mask buttons

volatile uint32_t g_mode_Menu;        ///< Текущее Словосостояние  Меню
volatile uint32_t g_mode_Menu_old;    ///< Прежнее Словосостояние  Меню

uint8_t g_change_Menu;                ///< if the Menu was changed?
s_Menu menu;                          ///< struct Menu
volatile statusMenu status_Menu;      ///< Структура статуса меню

uint8_t g_txBuf[64];  ///< буффер пакета  на передачу
uint8_t g_txPush = 0; ///< длинна пакета на передачу
uint8_t g_regs[0x3A0];   ///< глобальный массив регистров
uint8_t S_SLAVE_ADDRESS = 0x80; ///< адрес  контроллера
//uint8_t N_SAVED_REG=0xff;  // номер сохраняемого регистра  0xXX 0xXX
//                           //                             Adr  Bound
//volatile char strT1[32] = {0};

 uint16_t dimForChar[3850] = {0}; ///< массив одной строки символов для рисования
 uint8_t password[4] = {0};       ///< четыре цифры для ввода пароля 0x0000 - 0xFFFF
 uint16_t password_corect = 0x2020; ///< парооль user
 uint16_t password_admin =  0x2121;  ///< парооль admin
 /**
  * @brief 0 - пароль guest;
  * 1 - пароль user;
  * 2 - пароль admin
  */
 uint8_t password_OK = 0;

 /**
  * @brief  1- задача запущена
  * 1- задача запущена;
  */
 uint8_t uRunningTask = 0x00;     ///< Словосостояние запущеных задач; бит = 1 - задача запущена

 SemaphoreHandle_t gSensorMutex;

//HAL_SPI_StateTypeDef stateSPI3;
RTC_TimeTypeDef sTime = {0};        ///< Структура работы со временем
RTC_DateTypeDef DateTypeUpdate ={0};  ///< Структура работы с датой
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void MX_FREERTOS_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
  int result = add_numbers(10,  20);   // Тест ассемблерной функции
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
  MX_DMA_Init();

  MX_SPI3_Init();
  MX_TIM3_Init();

  MX_USART2_UART_Init();
  MX_TIM4_Init();
  MX_ADC1_Init();
  MX_TIM6_Init();
  MX_RTC_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */

  // Переинициализация порта UART2
  uint32_t bound;
  //uint16_t typeBound_UART2 = (*((uint16_t*)(0x0803f000 + MODBUS_REG(REG_UNIT_TYPE)*256)));
  uint16_t typeBound_UART2 = (*((uint16_t*)(0x0803f000))) >> 8;
  if(typeBound_UART2 == 0x00) bound = 9600;
  else if(typeBound_UART2 == 0x01) bound = 19200;
  else bound = 38400;

  huart2.Init.BaudRate = bound;
  if(HAL_UART_DeInit(&huart2) != HAL_OK){
	  Error_Handler();
  }
  if(HAL_UART_Init(&huart2) != HAL_OK){
	  Error_Handler();
  }

  // Настройка ADC
  ADC1->CR2 |= ADC_CR1_SCAN;
  ADC1->CR2 |= ADC_CR2_CONT;
  ADC1->CR2 |= ADC_CR2_ADON;
//  ADC1->CR2 |= ADC_CR2_SWSTART;
//  ADC1->CR2 |= ADC_CR2_EXTSEL_0 | ADC_CR2_EXTSEL_1 | ADC_CR2_EXTSEL_2;
  ADC1->CR2 |= ADC_CR2_JEXTTRIG;
  ADC1->CR2 |= ADC_CR2_JEXTSEL_0 | ADC_CR2_JEXTSEL_1 | ADC_CR2_JEXTSEL_2;  // 111 - JSWSTART
  ADC1->SMPR1 |= 0x00000fff;  // SMP13[2:0] = 111, SMP12[2:0] = 111, SMP11[2:0] = 111, SMP10[2:0] = 111
  ADC1->JSQR |=  (0x03 << 20) | (0x0D << 15) | (0x0c << 10) | (0x0B << 5) | 0x0A;

  ADC1->CR2 &= ~ADC_CR2_DMA;
  //======================================
  // TIM4 CH3 CH4    TIM3 CH3 CH4
  //======================================
  // Enable ALT function GPIOB and TIM4
  RCC->APB2ENR |= RCC_APB2ENR_IOPBEN | RCC_APB2ENR_AFIOEN;
  RCC->APB1ENR |= RCC_APB1ENR_TIM4EN;

  // PB8  PB9  Configure pins as ALT function OUTPUT PUSH PULL 50MHz
  GPIOB->CRH |= GPIO_CRH_MODE8_0 | GPIO_CRH_MODE8_1 | GPIO_CRH_CNF8_1;
  GPIOB->CRH &= ~GPIO_CRH_CNF8_0;
  GPIOB->CRH |= GPIO_CRH_MODE9_0 | GPIO_CRH_MODE9_1 | GPIO_CRH_CNF9_1;
  GPIOB->CRH &= ~GPIO_CRH_CNF9_0;

  // PB6 PB7 Configure pins as ALT function OUTPUT PUSH PULL 50MHz
  GPIOB->CRL |= GPIO_CRL_MODE6_0 | GPIO_CRL_MODE6_1 | GPIO_CRL_CNF6_1;
  GPIOB->CRL &= ~GPIO_CRL_CNF6_0;
  GPIOB->CRL |= GPIO_CRL_MODE7_0 | GPIO_CRL_MODE7_1 | GPIO_CRL_CNF7_1;
  GPIOB->CRL &= ~GPIO_CRL_CNF7_0;

  // compare 1,2 output enable
  TIM4->CCER |= TIM_CCER_CC1E;
  TIM4->CCER |= TIM_CCER_CC2E;
  // compare 3,4 output enable
  TIM4->CCER |= TIM_CCER_CC3E;
  TIM4->CCER |= TIM_CCER_CC4E;

  // теневой регистр
  TIM4->CR1 |= TIM_CR1_ARPE;

  // PWM compare mode 2, TIMx_CNT<TIMx_CCR1 inactive
  TIM4->CCMR1 |= TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1PE;
  TIM4->CCMR1 |= TIM_CCMR1_OC2M_1 | TIM_CCMR1_OC2M_2 | TIM_CCMR1_OC2PE;

 TIM4->CCMR2 |= TIM_CCMR2_OC3M_1 | TIM_CCMR2_OC3M_2 | TIM_CCMR2_OC3PE;
 TIM4->CCMR2 |= TIM_CCMR2_OC4M_1 | TIM_CCMR2_OC4M_2 | TIM_CCMR2_OC4PE;

  TIM4->PSC = 35;
  TIM4->ARR = 100;

  TIM4->CCR1 = 0;
  TIM4->CCR2 = 0;
  TIM4->CCR3 = 0;
  TIM4->CCR4 = 0;

  TIM4->EGR |= TIM_EGR_UG;
  TIM4->CR1 |= TIM_CR1_CEN;


/*  GPIOC->CRH |= GPIO_CRH_MODE8_0 | GPIO_CRH_MODE8_1 | GPIO_CRH_CNF8_1;
  GPIOC->CRH &= ~GPIO_CRH_CNF8_0;
  GPIOC->CRH |= GPIO_CRH_MODE9_0 | GPIO_CRH_MODE9_1 | GPIO_CRH_CNF9_1;
  GPIOC->CRH &= ~GPIO_CRH_CNF9_0;
  // compare 3,4 output enable
  TIM3->CCER |= TIM_CCER_CC4E;
  TIM3->CCER |= TIM_CCER_CC3E;

  // теневой регистр
  TIM3->CR1 |= TIM_CR1_ARPE;

  TIM3->CCMR2 |= TIM_CCMR2_OC3M_1 | TIM_CCMR2_OC3M_2 | TIM_CCMR2_OC3PE;
  TIM3->CCMR2 |= TIM_CCMR2_OC4M_1 | TIM_CCMR2_OC4M_2 | TIM_CCMR2_OC4PE;

  TIM3->PSC = 35;
  TIM3->ARR = 100;

  TIM3->CCR3 = 0;
  TIM3->CCR4 = 0;

  TIM3->EGR |= TIM_EGR_UG;
  TIM3->CR1 |= TIM_CR1_CEN;*/

  //======================================
  // END    TIM4 CH3 CH4    TIM3 CH3 CH4
  //======================================

  //======================================
  // ADC
  //======================================
//  ADC1->CR2 |= ADC_CR2_RSTCAL;
//    while((ADC1->CR2 & ADC_CR2_RSTCAL));

 //   ADC1->CR2 |= ADC_CR2_CAL;
 //   while(ADC1->CR2 & ADC_CR2_CAL);
  //======================================
  // END ADC
  //======================================





  HAL_Delay(500);

 // stateSPI3 = HAL_SPI_GetState(SPI3);
 // HAL_StatusTypeDef result;
//  uint8_t buff_test[8] = {0x01, 0x03, 0x00, 0x00, 0x00, 0x01, 0x84, 0x0a};
//  result = HAL_DMA_Init(&hdma_spi3_tx);

  //uint16_t cr1 = ST7789_SPI_HAL.Instance->CR1;
  //uint16_t cr2 = ST7789_SPI_HAL.Instance->CR2;
  //uint16_t sr = ST7789_SPI_HAL.Instance->SR;
  //result = HAL_SPI_Transmit_DMA(&ST7789_SPI_HAL, buff_test, 8);
  //cr1 = ST7789_SPI_HAL.Instance->CR1;
  //cr2 = ST7789_SPI_HAL.Instance->CR2;
  //sr = ST7789_SPI_HAL.Instance->SR;
  //stateSPI3 = HAL_SPI_GetState(SPI3);



      ST7789_Init_u(); ///< �?нициализация дисплея

      ST7789_rotation_u(2); ///< ротация экрана
     // Тест вывода основных цветов
     //ST7789_FillScreen_u(BLACK);
     //ST7789_SetBL(100);

//     HAL_IWDG_Refresh(&hiwdg);
     IWDG->KR = 0x0000AAAAU;
     //HAL_Delay(2000);
     uint16_t color = RGB565(255, 0, 0);
     ST7789_FillScreen_u(color);
     HAL_Delay(200);
     color = RGB565(0, 255, 0);
     ST7789_FillScreen_u(color);
     HAL_Delay(200);
     color = RGB565(0, 0, 255);
     ST7789_FillScreen_u(color);
     HAL_Delay(200);
     color = RGB565(255, 255, 255);
     ST7789_FillScreen_u(color);
     HAL_Delay(200);

//     ST7789_DrawChar_u(10, 10, RGB565(0, 0, 0), RGB565(255, 0, 0), 1, &Font_16x28, 1, '1');
//     HAL_Delay(500);
//     ST7789_DrawChar_u(50, 10, RGB565(0, 0, 0), RGB565(0, 255, 0), 1, &Font_16x28, 2, '2');
//     HAL_Delay(500);
//     ST7789_DrawChar_u(100, 10, RGB565(0, 0, 0), RGB565(0, 0, 255), 1, &Font_16x28, 2, '3');
//     HAL_Delay(500);

     // староый вариант вывода логотипа
     /*
     ST7789_DrawImage_u(0, 0, 240, 120, logo);
     ST7789_DrawImage_u(0, 120, 240, 120, logo+0x7080);
     */
     // разархивируем часть массива
     // ...
     // CHECK_OK ST7789_Draw_ArchImage_u
     // вывод архивированого  изображения
///////////////     ST7789_Draw_ArchImage_u(0, 0, 240, 240, logo);

     HAL_Delay(2000);

  /* USER CODE END 2 */

  /* Call init function for freertos objects (in freertos.c) */
  MX_FREERTOS_Init();

  gSensorMutex = xSemaphoreCreateMutex();

  if(gSensorMutex == NULL)
  {
	  Error_Handler();
  }

  /* Start scheduler */
  osKernelStart();

  /* We should never get here as control is now taken by the scheduler */
  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
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
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE|RCC_OSCILLATORTYPE_LSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV2;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_RTC|RCC_PERIPHCLK_ADC;
  PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
  PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV8;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM7 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM7) {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
}

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

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
