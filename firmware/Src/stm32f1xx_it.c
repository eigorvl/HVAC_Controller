/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    stm32f1xx_it.c
  * @brief   Interrupt Service Routines.
  * @note Interrupts for work UART1 Modbus Slave, UART2 Modbus Master
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
#include "stm32f1xx_it.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "usart.h"
#include "cmsis_os.h"
#include "utils.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN TD */

/* USER CODE END TD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */
 extern USART_ModBus USART2_Modbus;
 extern osTimerId ModBus_TimeoutHandle;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/* External variables --------------------------------------------------------*/
extern DMA_HandleTypeDef hdma_spi3_tx;
extern SPI_HandleTypeDef hspi3;
extern TIM_HandleTypeDef htim6;
extern DMA_HandleTypeDef hdma_usart2_rx;
extern DMA_HandleTypeDef hdma_usart2_tx;
extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;
extern TIM_HandleTypeDef htim7;

/* USER CODE BEGIN EV */

/* USER CODE END EV */

/******************************************************************************/
/*           Cortex-M3 Processor Interruption and Exception Handlers          */
/******************************************************************************/
/**
  * @brief This function handles Non maskable interrupt.
  */
void NMI_Handler(void)
{
  /* USER CODE BEGIN NonMaskableInt_IRQn 0 */

  /* USER CODE END NonMaskableInt_IRQn 0 */
  /* USER CODE BEGIN NonMaskableInt_IRQn 1 */
  while (1)
  {
  }
  /* USER CODE END NonMaskableInt_IRQn 1 */
}

/**
  * @brief This function handles Hard fault interrupt.
  */
void HardFault_Handler(void)
{
  /* USER CODE BEGIN HardFault_IRQn 0 */

  /* USER CODE END HardFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_HardFault_IRQn 0 */
    /* USER CODE END W1_HardFault_IRQn 0 */
  }
}

/**
  * @brief This function handles Memory management fault.
  */
void MemManage_Handler(void)
{
  /* USER CODE BEGIN MemoryManagement_IRQn 0 */

  /* USER CODE END MemoryManagement_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_MemoryManagement_IRQn 0 */
    /* USER CODE END W1_MemoryManagement_IRQn 0 */
  }
}

/**
  * @brief This function handles Prefetch fault, memory access fault.
  */
void BusFault_Handler(void)
{
  /* USER CODE BEGIN BusFault_IRQn 0 */

  /* USER CODE END BusFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_BusFault_IRQn 0 */
    /* USER CODE END W1_BusFault_IRQn 0 */
  }
}

/**
  * @brief This function handles Undefined instruction or illegal state.
  */
void UsageFault_Handler(void)
{
  /* USER CODE BEGIN UsageFault_IRQn 0 */

  /* USER CODE END UsageFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_UsageFault_IRQn 0 */
    /* USER CODE END W1_UsageFault_IRQn 0 */
  }
}

/**
  * @brief This function handles Debug monitor.
  */
void DebugMon_Handler(void)
{
  /* USER CODE BEGIN DebugMonitor_IRQn 0 */

  /* USER CODE END DebugMonitor_IRQn 0 */
  /* USER CODE BEGIN DebugMonitor_IRQn 1 */

  /* USER CODE END DebugMonitor_IRQn 1 */
}

/******************************************************************************/
/* STM32F1xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32f1xx.s).                    */
/******************************************************************************/

/**
  * @brief This function handles DMA1 channel6 global interrupt.
  */
void DMA1_Channel6_IRQHandler(void)
{
  /* USER CODE BEGIN DMA1_Channel6_IRQn 0 */

  /* USER CODE END DMA1_Channel6_IRQn 0 */
  HAL_DMA_IRQHandler(&hdma_usart2_rx);
  /* USER CODE BEGIN DMA1_Channel6_IRQn 1 */

  /* USER CODE END DMA1_Channel6_IRQn 1 */
}

/**
  * @brief This function handles DMA1 channel7 global interrupt.
  */
void DMA1_Channel7_IRQHandler(void)
{
  /* USER CODE BEGIN DMA1_Channel7_IRQn 0 */
//	  if(hdma_usart2_tx.State==HAL_DMA_STATE_READY)
//	  {

//		  do
//		  {

//		  } while(HAL_UART_GetState(&huart2) != HAL_UART_STATE_READY);
//		  GPIOA->BSRR = GPIO_BSRR_BR1;

//	  }
  /* USER CODE END DMA1_Channel7_IRQn 0 */
  HAL_DMA_IRQHandler(&hdma_usart2_tx);
  /* USER CODE BEGIN DMA1_Channel7_IRQn 1 */

  /* USER CODE END DMA1_Channel7_IRQn 1 */
}

/**
  * @brief This function handles USART1 global interrupt.
  */
void USART1_IRQHandler(void)
{
  /* USER CODE BEGIN USART1_IRQn 0 */

  /* USER CODE END USART1_IRQn 0 */
  HAL_UART_IRQHandler(&huart1);
  /* USER CODE BEGIN USART1_IRQn 1 */

  /* USER CODE END USART1_IRQn 1 */
}

/**
  * @brief This function handles USART2 global interrupt.
  */
void USART2_IRQHandler(void)
{
  /* USER CODE BEGIN USART2_IRQn 0 */

  /* USER CODE END USART2_IRQn 0 */
  HAL_UART_IRQHandler(&huart2);
  /* USER CODE BEGIN USART2_IRQn 1 */

  /* USER CODE END USART2_IRQn 1 */
}

/**
  * @brief This function handles SPI3 global interrupt.
  */
void SPI3_IRQHandler(void)
{
  /* USER CODE BEGIN SPI3_IRQn 0 */

  /* USER CODE END SPI3_IRQn 0 */
  HAL_SPI_IRQHandler(&hspi3);
  /* USER CODE BEGIN SPI3_IRQn 1 */

  /* USER CODE END SPI3_IRQn 1 */
}

/**
  * @brief This function handles TIM6 global interrupt.
  */
void TIM6_IRQHandler(void)
{
  /* USER CODE BEGIN TIM6_IRQn 0 */
//	GPIOB->BSRR = GPIO_PIN_13;
  /* USER CODE END TIM6_IRQn 0 */
  HAL_TIM_IRQHandler(&htim6);
  /* USER CODE BEGIN TIM6_IRQn 1 */

 //   static uint16_t silent_USART1 = 0;



  //==============================================
  // USART1
  //==============================================
  if ((USART1_Modbus.count_bytes_receive > 0) && (USART1_Modbus.state == waitFrame))
  {
	  USART1_Modbus.Timer_wait++;

      // 3.5 символа для 9600 = ~4 мс;  38400  2 ms
      if (USART1_Modbus.Timer_wait >= USART1_Modbus.Read_Timeout_Byte)
      {
    	  // Пакет пришел весь
    	  USART1_Modbus.state = decodeFrame;
          //mb1.frameReady = 1;
    	  GPIOB->BSRR = (uint32_t)GPIO_PIN_13 << 16U;   // маркер OFF
//    	  USART1_Modbus.count_bytes_receive = 0;        ///< кол-во прочитанных байт
    	  USART1_Modbus.Timer_wait = 0;                // счетчик ожидания прихода байта

    	  HAL_UART_AbortReceive_IT(&huart1);  ///< Закрываем UART

      }
  }
  else
  {
	  USART1_Modbus.Timer_wait = 0;
  }


//
//  if(USART1_Modbus.Wait_response > USART1_Modbus.Read_Timeout_Byte_ms)
//  {
//
//	  // произошел таймаут
//	  // USART2_Modbus.count_bytes_receive = 0;
//	  USART1_Modbus.Wait_response=0;
//
//
//
//
//	  // пакет нам?
//	  uint16_t crc16 = CRC16(USART1_Modbus.rx_Buff, USART1_Modbus.count_bytes_receive - 2);
//
//	  if((USART1_Modbus.rx_Buff[0] == S_SLAVE_ADDRESS) && ((crc16 >> 8) == USART1_Modbus.rx_Buff[USART1_Modbus.count_bytes_receive - 2]) &&
//		 ((crc16 & 0xff) == USART1_Modbus.rx_Buff[USART1_Modbus.count_bytes_receive - 1]))
//	  {
//		 // пакет нам
//	     // переключаем на передачу, формируем ответ и отсылаем
//
//		  		// перешли в режим декодирования
//		  		// формируем ответ
//
//		  //**
//	      GPIOB->BSRR = GPIO_PIN_15;
//
//
//		        USART1_Modbus.Wait_response = 0;
//		  		ModBusDecode_local(USART1_Modbus.rx_Buff, USART1_Modbus.count_bytes_receive);
//
//		  		// перешли в режим отправки
//		  	    USART1_Modbus.state = sendFrame;
//
//		  	    crc16 = CRC16(g_txBuf, g_txPush - 2);
//		  		// отправляем пакет
//		  	  	USART1_DE_TX;
//
//		  	  	g_txBuf[g_txPush - 2] = crc16 >> 8;
//		  	  	g_txBuf[g_txPush - 1] = crc16 & 0xff;
//
//		  	    HAL_UART_Transmit_DMA(&huart1, g_txBuf, g_txPush);
//
//
//	  }
//	  else
//	  {
//		  // пакет не нам
//		  // переключаемся на прием
//		  USART1_Modbus.state = waitFrame;
//		  USART1_Modbus.Wait_response = 0;
//		  USART1_Modbus.count_bytes_receive = 0;  // обнуляем счетчик принятых байт
//		  USART1_DE_RX;
//
//		  HAL_UART_Receive_DMA(&huart1, &(USART1_Modbus.Temp), 1);
//
//	  }
//
//	  USART1_Modbus.count_bytes_receive = 0; // обнуляем кол-во принятых байт
//
//  }

  //==============================================
  // USART2
  //==============================================
  if(USART2_Modbus.state == waitFrame)
  {
	// увеличиваем счетчик ожидания
	USART2_Modbus.Wait_response++;
  }
  else
  {
	 USART2_Modbus.Wait_response = 0;
  }


  if(USART2_Modbus.Wait_response > USART2_Modbus.Read_Timeout_Byte_ms)
  {
	  // произошел таймаут
	  // USART2_Modbus.count_bytes_receive = 0;
	  USART2_Modbus.Wait_response=0;

	  	 USART2_Modbus.state = decodeFrame;
	  	// USART2_Modbus.Data[USART2_Modbus.i]=-1001;
	//  	 MODBUS_REG(USART2_Modbus.i + START_REG_MODBUS) = -1001;
	//  	 USART2_Modbus.i++;
	//  	GPIOB->BSRR = (uint32_t)GPIO_PIN_13 << 16U;  // TimeOut
  }

  /* USER CODE END TIM6_IRQn 1 */
}

/**
  * @brief This function handles TIM7 global interrupt.
  */
void TIM7_IRQHandler(void)
{
  /* USER CODE BEGIN TIM7_IRQn 0 */

  /* USER CODE END TIM7_IRQn 0 */
  HAL_TIM_IRQHandler(&htim7);
  /* USER CODE BEGIN TIM7_IRQn 1 */

  /* USER CODE END TIM7_IRQn 1 */
}

/**
  * @brief This function handles DMA2 channel2 global interrupt.
  */
void DMA2_Channel2_IRQHandler(void)
{
  /* USER CODE BEGIN DMA2_Channel2_IRQn 0 */

  /* USER CODE END DMA2_Channel2_IRQn 0 */
  HAL_DMA_IRQHandler(&hdma_spi3_tx);
  /* USER CODE BEGIN DMA2_Channel2_IRQn 1 */

  /* USER CODE END DMA2_Channel2_IRQn 1 */
}

/* USER CODE BEGIN 1 */
// пакет отправлен целиком
/**
  * @brief Прерывание "Байт отправлен".
  */
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
  if(huart->Instance == USART1)
  {
	  GPIOB->BSRR = (uint32_t)GPIO_PIN_15 << 16U;   // маркер OFF

	  // пакет ушел
	  if(USART1_Modbus.state == sendFrame)
	  {

	   USART1_Modbus.state = waitFrame;
	   USART1_Modbus.count_bytes_receive = 0;
	   USART1_Modbus.Wait_response = 0;
	   USART1_DE_RX;

	  }
  }

  if (huart->Instance == USART2)
  {
    // USART2 завершил отправку данных
	 if(USART2_Modbus.state == sendFrame)
	 {
	 // Переключаемся на прием
	  USART2_DE_RX;

	  // очищаем буферперед приемом пакета
	  for(uint8_t i=0;i<8;i++) USART2_Modbus.rx_Buff[i] = 0;
	  USART2_Modbus.state = waitFrame;

//	  USART2_Modbus.Timer_wait = 0; 		    // таймер-счетчик ожидания прихода байта обнуляем
	  USART2_Modbus.Wait_response = 0;          // таймер-счетчик ожидания прихода байта обнуляем
	  USART2_Modbus.Read_Timeout_Byte_ms = 50;  // максимальное время ожидания прихода байта
	  USART2_Modbus.count_bytes_receive = 0;    // обнуляем счетчик принятых байт

	  // запуск приема через DMA одного байта
	  HAL_UART_Receive_DMA(&huart2, USART2_Modbus.rx_Buff, 1);

	 } // end if sendFrame
  }
}

// пакет принят целиком
/**
  * @brief Прерывание "Байт принят".
  */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
//	 //======================================================================
//	 // USART1
//	 //======================================================================
	if(huart->Instance == USART1)
	{

	    if (huart->Instance == USART1)
	    {
	    	USART1_Modbus.rx_Buff[USART1_Modbus.count_bytes_receive++] = USART1_Modbus.Temp;
	    	USART1_Modbus.Timer_wait = 0;

	        // Защита от переполнения
	        if (USART1_Modbus.count_bytes_receive >= sizeof(USART1_Modbus.rx_Buff))
	        	USART1_Modbus.count_bytes_receive = 0;

	        GPIOB->BSRR = GPIO_PIN_13;   // маркер приема байта

	        // Перезапуск приёма следующего байта
	        HAL_UART_Receive_IT(&huart1, &(USART1_Modbus.Temp), 1);



	        // Сброс таймера Modbus T3.5
//	        __HAL_TIM_SET_COUNTER(&htim1, 0);
	    }



	}
//	 if(huart->Instance == USART1)
//	 {
//		 GPIOB->BSRR = GPIO_PIN_15;
//
//		// байт на прием пришел
//		if((USART1_Modbus.state == waitFrame) || (USART1_Modbus.state == receiveFrame))
//		{
//			USART1_Modbus.rx_Buff[USART1_Modbus.count_bytes_receive] = USART1_Modbus.Temp;
//			USART1_Modbus.count_bytes_receive++;  // увеличиваем счетчик принятых байт
//			USART1_Modbus.Wait_response = 0;      // обнуляем счетчик таймаута
//
//			MODBUS_REG(0x39) = 1;
//			// проверка длинны пакета
//			// если длина пакета больше 32 байт заканчиваем прием и начинаем деодировать
//			if(USART1_Modbus.count_bytes_receive > 32)
//			{
//				  // пакет не нам
//				  // переключаемся на прием
//				  USART1_Modbus.state = waitFrame;
//				  USART1_Modbus.Wait_response = 0;
//				  USART1_Modbus.count_bytes_receive = 0;  // обнуляем счетчик принятых байт
//				  USART1_DE_RX;
//
//				  HAL_UART_Receive_DMA(&huart1, &(USART1_Modbus.Temp), 1);  // продолжаем слушать UART1
//			}
//			else
//			{
//				// переключаемся в режим ожидания остатка пакета
//				USART1_Modbus.state = receiveFrame;
//
//				// запускаем таймаут на прием следующего байта
//				HAL_UART_Receive_DMA(&huart1, &(USART1_Modbus.Temp), 1);
//			}
//		}
//
//		GPIOB->BSRR = (uint32_t)GPIO_PIN_15 << 16U;
//	 }


	 //======================================================================
	 // USART2
	 //======================================================================

	 if (huart->Instance == USART2)
	 {
		USART2_Modbus.Wait_response = 0;      // обнулить время ожидания байта т.к. байт пришел
		USART2_Modbus.count_bytes_receive++;  // увеличиваем счетчик принятых байт

       if(USART2_Modbus.state == waitFrame){
        // пришел байт с ответного пакета

    	//USART2_Modbus.Wait_response = 0;
    	//USART2_Modbus.count_bytes_receive++;

    	if(USART2_Modbus.count_bytes_receive < USART2_Modbus.wait_for_bytes)
    	{
    		// пакет пришел не весь

//    		 GPIOB->BSRR = (uint32_t)GPIO_PIN_14 << 16U;

    		// запрашиваем следующий байт
    		HAL_UART_Receive_DMA(&huart2, USART2_Modbus.rx_Buff + USART2_Modbus.count_bytes_receive, 1);
    	}
    	else
    	{
         // пришел последний байт то есть пакет целиком
   		// USART2_Modbus.count_bytes_receive = 0;
   		 USART2_Modbus.state = decodeFrame;

//   		 GPIOB->BSRR = (uint32_t)GPIO_PIN_14 << 16U;

   		 //пакет какой-то пришел - декодируем его
   	    //HAL_TIM_Base_Stop_IT(&htim6);
   	     //htim6.Init.AutoReloadPreload = 200;
    	}

//    	      GPIOB->BSRR = GPIO_PIN_14;
       } // END  if waitFrame

       else if(USART2_Modbus.state == decodeFrame)
       {
//    	   GPIOB->BSRR = GPIO_PIN_14;
    	  //пакет какой-то пришел - декодируем его

       }

	 }  // end if USART2
}

void HAL_TIM_ElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim->Instance == TIM6)
		{
		 //HAL_GPIO_TogglePin(GPIOC, TASK_4_Pin);
		///
		//GPIOC->BSRR = TASK_4_Pin;
		///
		}
}
/* USER CODE END 1 */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
