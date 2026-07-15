/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file            freertos.c
  * @brief           �?нициализация и обработка задач FreeRTOS
  * @author          Эйсвальд �?.А.
  * @date            2025-06-08
  * @details         �?нициализируются и запускаются задачи:<br>
  * void Start_ST7789_Task(void const * argument) - отбражение показаний на дисплее<br>
  * void Start_EC11(void const * argument) - обработка данных с валкодера<br>
  * void StartUSART_2(void const * argument) - работа с портом UART2 ModBus RTU Master<br>
  * void StartAlgoritm(void const * argument) - задача пользовательского алгоритма<br>
  *
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


/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "usart.h"
#include "fonts.h"
#include "st7789_1.h"
#include "utils.h"
#include "flash.h"
#include "rtc.h"
#include "tim.h"
#include "Fan.hpp"
#include "digital_bloks.h"
#include "analog_bloks.h"
#include "algorithm.h"
//#include "iwdg.h"
#include "app_globals.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
//#define PROBE
#define OPTIMIZATION
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

QueueHandle_t queueTFT;           ///< очередь сообщений в дисплей
QueueHandle_t queueTimer;         ///< очередь от таймера

/* USER CODE END Variables */
osThreadId TFT_TaskHandle;
uint32_t TFT_TaskBuffer[ 256 ];
osStaticThreadDef_t TFT_TaskControlBlock;
osThreadId EC11_TaskHandle;
uint32_t EC11_TaskBuffer[ 256 ];
osStaticThreadDef_t EC11_TaskControlBlock;
osThreadId USART_2Handle;
uint32_t USART_2Buffer[ 128 ];
osStaticThreadDef_t USART_2ControlBlock;
osThreadId AlgoritmTaskHandle;
uint32_t AlgoritmTaskBuffer[ 512 ];
osStaticThreadDef_t AlgoritmTaskControlBlock;
osThreadId USART_1Handle;
uint32_t USART_1Buffer[ 128 ];
osStaticThreadDef_t USART_1ControlBlock;
osTimerId ModBus_TimeoutHandle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
void UpdateValueTimer_Fun(void const * argument);
/* Создаем таймер однократного запуска xOneShotTimer. */


/* USER CODE END FunctionPrototypes */

void Start_ST7789_Task(void const * argument);
void Start_EC11(void const * argument);
void StartUSART_2(void const * argument);
void StartAlgoritm(void const * argument);
void StartUSART_1(void const * argument);
void ModBus_TimeOut_Fun(void const * argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* GetIdleTaskMemory prototype (linked to static allocation support) */
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize );

/* GetTimerTaskMemory prototype (linked to static allocation support) */
void vApplicationGetTimerTaskMemory( StaticTask_t **ppxTimerTaskTCBBuffer, StackType_t **ppxTimerTaskStackBuffer, uint32_t *pulTimerTaskStackSize );

/* Hook prototypes */
void vApplicationIdleHook(void);

/* USER CODE BEGIN 2 */
__weak void vApplicationIdleHook( void )
{
   /* vApplicationIdleHook() will only be called if configUSE_IDLE_HOOK is set
   to 1 in FreeRTOSConfig.h. It will be called on each iteration of the idle
   task. It is essential that code added to this hook function never attempts
   to block in any way (for example, call xQueueReceive() with a block time
   specified, or call vTaskDelay()). If the application makes use of the
   vTaskDelete() API function (as this demo application does) then it is also
   important that vApplicationIdleHook() is permitted to return to its calling
   function, because it is the responsibility of the idle task to clean up
   memory allocated by the kernel to any task that has since been deleted. */

//	GPIOB->BSRR = GPIO_PIN_15;
//	__NOP();
//	GPIOB->BSRR = (uint32_t)GPIO_PIN_15 << 16U;
}
/* USER CODE END 2 */

/* USER CODE BEGIN PREPOSTSLEEP */
__weak void PreSleepProcessing(uint32_t *ulExpectedIdleTime)
{
/* place for user code */
}

__weak void PostSleepProcessing(uint32_t *ulExpectedIdleTime)
{
/* place for user code */
}
/* USER CODE END PREPOSTSLEEP */

/* USER CODE BEGIN GET_IDLE_TASK_MEMORY */
static StaticTask_t xIdleTaskTCBBuffer;
static StackType_t xIdleStack[configMINIMAL_STACK_SIZE];

void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize )
{
  *ppxIdleTaskTCBBuffer = &xIdleTaskTCBBuffer;
  *ppxIdleTaskStackBuffer = &xIdleStack[0];
  *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
  /* place for user code */
}
/* USER CODE END GET_IDLE_TASK_MEMORY */

/* USER CODE BEGIN GET_TIMER_TASK_MEMORY */
static StaticTask_t xTimerTaskTCBBuffer;
static StackType_t xTimerStack[configTIMER_TASK_STACK_DEPTH];

void vApplicationGetTimerTaskMemory( StaticTask_t **ppxTimerTaskTCBBuffer, StackType_t **ppxTimerTaskStackBuffer, uint32_t *pulTimerTaskStackSize )
{
  *ppxTimerTaskTCBBuffer = &xTimerTaskTCBBuffer;
  *ppxTimerTaskStackBuffer = &xTimerStack[0];
  *pulTimerTaskStackSize = configTIMER_TASK_STACK_DEPTH;
  /* place for user code */
}
/* USER CODE END GET_TIMER_TASK_MEMORY */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* Create the timer(s) */
  /* definition and creation of ModBus_Timeout */
  osTimerDef(ModBus_Timeout, ModBus_TimeOut_Fun);
  ModBus_TimeoutHandle = osTimerCreate(osTimer(ModBus_Timeout), osTimerOnce, NULL);

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  TimerHandle_t xTimerUpdateValue = xTimerCreate("UpDateValue", 1000, pdTRUE, 0, UpdateValueTimer_Fun);
  BaseType_t UpdateValueStarted = xTimerStart(xTimerUpdateValue, 0);


  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */

  queueTFT = xQueueCreate(1, sizeof(uint32_t)); ///< очередь статусов в экран от валкодера

  queueTimer = xQueueCreate(1, sizeof(uint32_t)); ///< очередь статусов в экран от таймера
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of TFT_Task */
  osThreadStaticDef(TFT_Task, Start_ST7789_Task, osPriorityNormal, 0, 256, TFT_TaskBuffer, &TFT_TaskControlBlock);
  TFT_TaskHandle = osThreadCreate(osThread(TFT_Task), NULL);

  /* definition and creation of EC11_Task */
  osThreadStaticDef(EC11_Task, Start_EC11, osPriorityNormal, 0, 256, EC11_TaskBuffer, &EC11_TaskControlBlock);
  EC11_TaskHandle = osThreadCreate(osThread(EC11_Task), NULL);

  /* definition and creation of USART_2 */
  osThreadStaticDef(USART_2, StartUSART_2, osPriorityNormal, 0, 128, USART_2Buffer, &USART_2ControlBlock);
  USART_2Handle = osThreadCreate(osThread(USART_2), NULL);

  /* definition and creation of AlgoritmTask */
  osThreadStaticDef(AlgoritmTask, StartAlgoritm, osPriorityIdle, 0, 512, AlgoritmTaskBuffer, &AlgoritmTaskControlBlock);
  AlgoritmTaskHandle = osThreadCreate(osThread(AlgoritmTask), NULL);

  /* definition and creation of USART_1 */
  osThreadStaticDef(USART_1, StartUSART_1, osPriorityNormal, 0, 128, USART_1Buffer, &USART_1ControlBlock);
  USART_1Handle = osThreadCreate(osThread(USART_1), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */

  /* USER CODE END RTOS_THREADS */

}

/* USER CODE BEGIN Header_Start_ST7789_Task */
/**
  * @brief  Function implementing the TFT_Task thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_Start_ST7789_Task */
void Start_ST7789_Task(void const * argument)
{
  /* USER CODE BEGIN Start_ST7789_Task */
	  sFan n1, B1;

	  uint16_t colorWhite, colorBlack, colorGray, colorLightGray, colorRed, colorBlue, colorGreen;
//	  char strEC11[9] = {0,0,0,0,0,0,0,0,0};
	  char strEC11Btn[9] = {0,0,0,0,0,0,0,0,0};
//	  char strTime[16] = {0};
	  // строка пробелов для очистки строк
	  const char str[16] = {' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',
			                ' ',' ',' ',' ',0};

	  char strTmp[16], strTmp1[20];

//	  static uint16_t drawMenu = 0; // отображенное меню

	  colorWhite = RGB565(255, 255, 255);
	  colorBlack = RGB565(0, 0, 0);
	  colorRed = RGB565(255, 0, 0);
	  colorBlue = RGB565(0, 0, 255);
	  colorGreen = RGB565(0, 255, 0);
	  colorGray = RGB565(10, 10, 10);
	  colorLightGray = RGB565(200, 200, 200);

	  portBASE_TYPE xStatus;

	    vTaskSetApplicationTaskTag(NULL,(void*)1);  // делаем метку задачи "1"

	  // адрес блока FLASH памяти с которого нужно считать данные
	  char str1[10] = {0,};
	  uint32_t startAddr = findStartAddrToRead(0x0803f800, 0x0803ffff, 4, 21);

		// считываем данные в регистры 0х20 - 0х2Е -  уставки
		//                                    0х48 -  Unit Mode
		//                                    0х49 -  Unit Type
		//                                    0x40  - Start Time
		//                                    0x41  - Stop Time
		//                                    0x42  - Day Time
		//                                    0x43  - Night Time

		for(uint8_t i=0;i < 15;i++) {
			MODBUS_REG(0x20+i) = (int16_t)(*((uint32_t*)(startAddr + i*4)));
		}
		MODBUS_REG(0x48)= (int16_t)(*((uint32_t*)(startAddr + 15*4)));
		MODBUS_REG(0x49)= (int16_t)(*((uint32_t*)(startAddr + 16*4)));

		for(uint8_t i=0;i < 4;i++) {
			MODBUS_REG(0x40+i) = (int16_t)(*((uint32_t*)(startAddr + (17+i)*4)));
		}

		  // очистить экран и вывод картинки
		    ST7789_FillScreen_u(colorWhite);

	    	// вывод картинки
	    	if(MODBUS_REG(REG_UNIT_TYPE) == HVAC1)
	    	DrawHVAC1(0, 0);

		    if(MODBUS_REG(REG_UNIT_TYPE) == HVAC2)
		    DrawHVAC2(0, 0);

		    if(MODBUS_REG(REG_UNIT_TYPE) == HVAC3)
		    DrawHVAC3(0, 0);

		    if(MODBUS_REG(REG_UNIT_TYPE) == HVAC4)
		    DrawHVAC4(0, 0);

		    if(MODBUS_REG(REG_UNIT_TYPE) == HVAC5)
		    DrawHVAC5(0, 0);

			// инициализируем меню
		    InitMenu();

		   	// нанодимся в меню и выводим заголовок жирным
		   // ST7789_print_u(0, 0, colorWhite, colorBlue, 1, &Font_16x26, 1,menu.strMain_Menu[(g_mode_Menu & BITS_MENU) >> 4]/*menu.strMain_Menu[status_Menu.line_menu]*/);
	    	   strncpy(strTmp, menu.strMenu[10][MODBUS_REG(0x49)], 6);
	    	   sprintf(strTmp1, "*   %s    *",strTmp);
	    	   ST7789_print_u(0, 0, colorWhite, colorBlue, 1, &Font_16x26, 1, strTmp1);
		    // замок
		    ST7789_DrawImage_u(222, 0, 18, 26, lock_orange_18x26);

		    // печать кода меню
			sprintf(strEC11Btn,"%06x", g_mode_Menu);
			ST7789_print_u(0, 212, colorBlack, colorLightGray, 1, &Font_16x26, 1, strEC11Btn);



  /* Infinite loop */
  for(;;)
  {
	    // номер экрана
	    uint8_t numScreen;

////////////////////////////	    GPIOB->BSRR = (uint32_t)GPIO_PIN_15  << 16U;

#ifdef PROBE

	  if(g_mode_Menu & BIT_COUNT_EC11)
	  {
	  // изменилось состояние меню переделать!!

	 if(((g_mode_Menu_old & 0x000000F0) == 0x00))
	  {
		 // предыдущий пункт меню была картинка - значит очистить экран
		     ST7789_FillScreen_u(colorWhite);
		     g_mode_Menu_old = g_mode_Menu;
	  }



     if((g_mode_Menu & 0x000000F0) == 0x00)
     {
    	 // вывести картинку и запомнить что вывели картинку
///////    	  ST7789_DrawImage_u(0, 35, 233, 169, TP_icon);
//    	    ST7789_print_u(20, 95, RGB565(0, 200, 0), colorWhite, 1, &Font_11x18, 1,"100%");
//    	    ST7789_print_u(170, 95, colorBlue, colorWhite, 1, &Font_11x18, 1,"10 C");
    	  g_mode_Menu_old = g_mode_Menu;
     }

      // вывод заголовка перделать!!
     if((g_mode_Menu & 0x0000000F) == 0x00){
    	 // нанодимся в меню и выводим заголовок жирным
       ST7789_print_u(0, 0, colorWhite, colorBlue, 1, &Font_16x26, 1,menu.strMain_Menu[status_Menu.line_menu]);
     }
//     else{
//    	 // нанодимся в подменю и выводим заголовок обычным шрифтом
//       ST7789_print_u(0, 0, colorBlack, colorWhite, 1, &Font_16x26, 1,menu.strMain_Menu[status_Menu.line_menu]);
//     }

     // конец вывода залоловка
     ////////////////////////////////////////////////////

    /////////////////////////////////////////////////////
  	// вывод пунктов меню
      if((g_mode_Menu & 0x000000F0) > 0x00)
      {
    	  g_mode_Menu_old = g_mode_Menu;
    	  for(uint8_t i=0;i<5;i++)  ST7789_print_u(5, 45+i*30, colorBlack, colorWhite, 1, &Font_16x26, 1, menu.strMenu[(g_mode_Menu & 0x000000F0) >> 4][(i+5*numScreen)]);
      }


    /////////////////////////////////////////////////////


      ///////////////////////////////////////////////////////////////////
      // вывод значений
      if((g_mode_Menu & 0x000000F0) == MENU_SET_POINTS){
      // вывод значений SetPoints
    	  g_mode_Menu_old = g_mode_Menu;
		  for(char i=0;i<5;i++){
				  sprintf(USART2_Modbus.strRes,"%3i", MODBUS_REG(0x20+i+5*numScreen));
				  if((((g_mode_Menu & BITS_SUB_MENU)-1)%5 == i) && ((g_mode_Menu & BITS_SUB_MENU) != 0))
				  {
				   if(g_mode_Menu & BIT_MODIFY_REG)
					   // модификация
					  ST7789_print_u(150, 45+i*30, colorRed, colorWhite, 1, &Font_16x26, 1, USART2_Modbus.strRes);
				   else
					   // выбор
					  ST7789_print_u(150, 45+i*30, colorWhite, colorBlue, 1, &Font_16x26, 1, USART2_Modbus.strRes);
				  }
				  else
					  // пункт не выбран
					  ST7789_print_u(150, 45+i*30, colorBlue, colorWhite, 1, &Font_16x26, 1, USART2_Modbus.strRes);

		  }
      }  // конец вывода значений уставок

	  if((g_mode_Menu & 0x000000F0) == MENU_STATUS){
		  // вывести показания статусов ЗАГЛУШКА!
		  for(char i=0;i<5;i++){
			  if(USART2_Modbus.Data[i+numScreen*5] > 0)
			  ST7789_print_u(150, 45+i*30, colorGreen, colorWhite, 1, &Font_16x26, 1, "ON   ");
			  else{
				  ST7789_print_u(150, 45+i*30, colorRed, colorWhite, 1, &Font_16x26, 1, "OFF  ");
			  }

		  }
	  }
	  if((g_mode_Menu & 0x000000F0) == MENU_TIME){
		  // вывести показания времени ЗАГЛУШКА!
    	  g_mode_Menu_old = g_mode_Menu;
		  for(char i=0;i<6;i++){
				  sprintf(USART2_Modbus.strRes,"%2i", MODBUS_REG(0x20+i+5*numScreen));
				  if((((g_mode_Menu & BITS_SUB_MENU)-1)%5 == i) && ((g_mode_Menu & BITS_SUB_MENU) != 0))
				  {
				   if(g_mode_Menu & BIT_MODIFY_REG){
					  // модификация
					  ST7789_print_u(120, 45+i*30, colorRed, colorWhite, 1, &Font_16x26, 1, USART2_Modbus.strRes);
				   }
				   else{
					   // выбор
					  ST7789_print_u(120, 45+i*30, colorWhite, colorBlue, 1, &Font_16x26, 1, USART2_Modbus.strRes);
				   }
				  }
				  else{
					  // пункт не выбран
					  ST7789_print_u(152, 45+i*30, colorBlue, colorWhite, 1, &Font_16x26, 1, USART2_Modbus.strRes);
					  ST7789_print_u(184, 45+i*30, colorBlue, colorWhite, 1, &Font_16x26, 1, ":");
					  ST7789_print_u(200, 45+i*30, colorBlue, colorWhite, 1, &Font_16x26, 1, USART2_Modbus.strRes);
				  }

		  }
	  }  // конец вывода показания времени

	  if((g_mode_Menu & 0x000000F0) == MENU_UNIT_MODE){
		  for(char i=0;i<3;i++){
			  ST7789_DrawRectangle_u(152, 45+i*30, 152+20, 45+i*30+20, 0);

		  }

	  } // конец вывода показания UNIT_MODE


	  // Конец вывода значений
	  /////////////////////////////////////////////////////////////////

      // вывод кода меню
		  sprintf(strEC11Btn,"%8x", g_mode_Menu);
		  ST7789_print_u(0, 212, RGB565(10, 10, 10), RGB565(200, 200, 200), 1, &Font_16x26, 1, strEC11Btn);
		  ST7789_print_u(150, 212, RGB565(10, 10, 10), RGB565(200, 200, 200), 1, &Font_16x26, 1, "12:00");
//	  	     sprintf(strTmp,"%2x %2x %2x %2x %2x %2x %2x", USART2_Modbus.rx_Buff[0], USART2_Modbus.rx_Buff[1],
//	  	          USART2_Modbus.rx_Buff[2], USART2_Modbus.rx_Buff[3], USART2_Modbus.rx_Buff[4], USART2_Modbus.rx_Buff[5],
//				  USART2_Modbus.rx_Buff[6]);
////	  	     ST7789_print_u(0, 80, colorBlack, colorWhite, 1, &Font_11x18, 1, strTmp);

	  	   g_mode_Menu &= ~(BIT_COUNT_EC11);
	  }


	  if(g_mode_Menu & BIT_UPDATE_VALUE){

		  // обновить данные
		  if((g_mode_Menu & 0x000000F0) == MENU_SENSORS){
			  // вывести показания датчиков
			  for(char i=0;i<5;i++){
				  if(USART2_Modbus.Data[i+numScreen*5] == -100)
				  ST7789_print_u(150, 45+i*30, colorBlue, colorWhite, 1, &Font_16x26, 1, "---  ");
				  else{
					  sprintf(USART2_Modbus.strRes,"%3i C", USART2_Modbus.Data[i+numScreen*5]);
					  ST7789_print_u(150, 45+i*30, colorBlue, colorWhite, 1, &Font_16x26, 1, USART2_Modbus.strRes);
				  }

			  }
		  }


			  if((g_mode_Menu & 0x000000F0) == MENU_ANALOG_IN){
				  // вывести показания аналоговых входов ЗАГЛУШКА!
				  for(char i=0;i<5;i++){
					  sprintf(USART2_Modbus.strRes,"%3i %%", USART2_Modbus.Data[i+numScreen*5]*3);
					  ST7789_print_u(150, 45+i*30, colorBlue, colorWhite, 1, &Font_16x26, 1, USART2_Modbus.strRes);
				  }
			  }

	    	  g_mode_Menu &= ~BIT_UPDATE_VALUE;

		  }




	  if(g_mode_Menu & BIT_BUTTON_EC11)
	  {
		  // перерисовываем значения по нажатию кнопки
		  if((g_mode_Menu & 0x000000F0) == MENU_SET_POINTS){
			  // вывести показания уставок ЗАГЛУШКА!
			  for(char i=0;i<5;i++){
					  sprintf(USART2_Modbus.strRes,"%3i", MODBUS_REG(0x20+i+5*numScreen));
					  if((((g_mode_Menu & BITS_SUB_MENU)-1)%5 == i) && ((g_mode_Menu & BITS_SUB_MENU) != 0))
					  {
					   if(g_mode_Menu & BIT_MODIFY_REG)
						  ST7789_print_u(150, 45+i*30, colorRed, colorWhite, 1, &Font_16x26, 1, USART2_Modbus.strRes);
					   else
						  ST7789_print_u(150, 45+i*30, colorWhite, colorBlue, 1, &Font_16x26, 1, USART2_Modbus.strRes);

					  }
					  else
						  ST7789_print_u(150, 45+i*30, colorBlue, colorWhite, 1, &Font_16x26, 1, USART2_Modbus.strRes);

			  }
		  }


		  sprintf(strEC11Btn,"%8x", g_mode_Menu);
		  ST7789_print_u(0, 212, RGB565(10, 10, 10), RGB565(200, 200, 200), 1, &Font_16x26, 1, strEC11Btn);
		  ST7789_print_u(150, 212, RGB565(10, 10, 10), RGB565(200, 200, 200), 1, &Font_16x26, 1, "12:00");

	  	g_mode_Menu &= ~(BIT_BUTTON_EC11);
	  }
#endif

#ifdef OPTIMIZATION

	  static uint32_t mode_Menu = 0;      // статус меню
	  static uint32_t mode_Menu_old = 0;  // предыдущий статус меню
	  static uint32_t mode_Menu_cursor_old = 0;  // предыдущее положение курсора
	  uint32_t countTime;      // кол-во тиков таймера
	  static uint32_t countWithoutReadEC11 = 0;       // кол-во проходов без обновления EC11
      static uint8_t startCount = 0;                  // Разрешение увеличения отсчетов

      static uint16_t menu_printed = 0;            // напечатанный заголовок меню ВСЕГДА ВЕРНО!!!
      static uint8_t sub_menu_printed = 0;         // напечатанные пункты подменю
      static uint8_t all_regs_printed = 0;         // напечатанные значения регистров
      static uint32_t status_printed = 0;          // напечатанный статус меню
      static uint32_t old_menu_printed = 0;        // значеие старого меню при котором начался вывод

      static uint8_t draw_picture = 1;             // 1 - была выведена картинка


      // устанавливаем бит runrunST7789 задача работает
      uRunningTask |= runST7789;

      if(uRunningTask == 0x0f)
      {
    	  // гладим IWDT
//    	  HAL_IWDG_Refresh(&hiwdg);
    	  uRunningTask = 0;
//          GPIOB->BSRR = GPIO_PIN_15;
      }

//       GPIOB->BSRR = (uint32_t)GPIO_PIN_15 << 16;

	  //читаем очередь от валкодера
	  xStatus = xQueueReceive(queueTFT, &mode_Menu, 0 );

	       if( xStatus == pdPASS )
	       {
	           /* Данные успешно приняты из очереди */
	 	       // берем данные только состояния меню
	 		   mode_Menu = mode_Menu & 0x7FF;

	           mode_Menu |= BIT_UPDATE_STATUS;
	           countWithoutReadEC11 = 0;
	           startCount = 1;            // разрешение подсчета проходов
	       }
	       else
	       {
	           /* Данные не были приняты из очереди */
	    	   //mode_Menu |= 0x40000000;
	    	   if(startCount) countWithoutReadEC11++;
	       }

	       // читаем очередь таймера
	       xStatus = xQueueReceive(queueTimer, &countTime, 0 );

	 	       if( xStatus == pdPASS )
	 	       {
	 	           /* Тик от таймера успешно считан */
	 	          mode_Menu |= BIT_UPDATE_VALUE;
	 	          mode_Menu |= BIT_UPDATE_TIME;
		          mode_Menu |= BIT_UPDATE_STATUS;

	 	       }
	 	       else
	 	       {
	 	           /* Данныех от таймера не было */

	 	       }

               // определяем на каком экране мы находимся
	 		   numScreen = NumOfScreen(mode_Menu);

//====================================================
// после некоторой задержки после перехода по основому меню можно изменить экран
 if((countWithoutReadEC11 > 1000) && ((mode_Menu & BITS_SUB_MENU) == 0)){
//	 mode_Menu = mode_Menu | BIT_UPDATE_MENU | BIT_UPDATE_SUBMENU | BIT_UPDATE_ALL_REGS
//	 					   | BIT_UPDATE_VALUE;
	 startCount = 0;
	 countWithoutReadEC11 = 0;
	 // прверка правильности вывода на экран
	 // MENU          - заголовок меню   ВСЕГДА ПРАВ�?ЛЬНОЕ!!!
	 // SUB_MENU      - пункты меню
	 // ALL_REGS      - значения регистров
	 // STATUS        - статус меню
	 uint32_t temp = menu_printed;
	 temp = sub_menu_printed;
	 temp = all_regs_printed;
	 temp = status_printed;
	 temp = old_menu_printed;
	 if(( menu_printed != sub_menu_printed) ||  (menu_printed != all_regs_printed) || (menu_printed != (status_printed & 0xff)))
	 {
	//		mode_Menu = mode_Menu | BIT_UPDATE_MENU | BIT_UPDATE_SUBMENU | BIT_UPDATE_ALL_REGS
	//				              | BIT_UPDATE_VALUE;
	     temp = 0;
	 }

	 else
		 temp = 1;


    }

 // TODO убрать инициализацию едениц измерения из цикла
    // единицы измерения
	menu.uOfMeasure[0] = 0;
	menu.uOfMeasure[1] = 0;
	menu.uOfMeasure[2] = 0;
	menu.uOfMeasure[3] = 0;
	menu.uOfMeasure[4] = 0;
	menu.uOfMeasure[5] = 0;
	menu.uOfMeasure[6] = 0;
	menu.uOfMeasure[7] = 0;
	menu.uOfMeasure[8] = 0;
	menu.uOfMeasure[9] = 0;
	menu.uOfMeasure[10] = 0;
	menu.uOfMeasure[11] = 0;
	menu.uOfMeasure[12] = 0;
	menu.uOfMeasure[13] = 0;
	menu.uOfMeasure[14] = 0;

// установка единиц измерений
 if(MODBUS_REG(REG_UNIT_TYPE) == HVAC1)
 {
	menu.uOfMeasure[0] = 1;
	menu.uOfMeasure[1] = 1;
	menu.uOfMeasure[2] = 1;
	menu.uOfMeasure[3] = 1;
	menu.uOfMeasure[4] = 1;

 }

 else  if(MODBUS_REG(REG_UNIT_TYPE) == HVAC2)
 {
	menu.uOfMeasure[0] = 1;
	menu.uOfMeasure[1] = 1;
	menu.uOfMeasure[2] = 1;
	menu.uOfMeasure[3] = 1;
	menu.uOfMeasure[4] = 1;

 }
 else  if(MODBUS_REG(REG_UNIT_TYPE) == HVAC3)
 {
	menu.uOfMeasure[0] = 1;
	menu.uOfMeasure[1] = 1;
	menu.uOfMeasure[2] = 1;
	menu.uOfMeasure[3] = 4;
	menu.uOfMeasure[4] = 1;

 }
 else  if(MODBUS_REG(REG_UNIT_TYPE) == HVAC4)
 {
	menu.uOfMeasure[0] = 1;
	menu.uOfMeasure[1] = 1;
	menu.uOfMeasure[2] = 1;
	menu.uOfMeasure[3] = 4;
	menu.uOfMeasure[4] = 1;

 }
 else if(MODBUS_REG(REG_UNIT_TYPE) == HVAC5)
 {
	menu.uOfMeasure[0] = 1;
	menu.uOfMeasure[1] = 1;
	menu.uOfMeasure[2] = 1;
	menu.uOfMeasure[3] = 1;
	menu.uOfMeasure[4] = 1;

 }
 else
 {
	menu.uOfMeasure[0] = 1;
	menu.uOfMeasure[1] = 1;
	menu.uOfMeasure[2] = 1;
	menu.uOfMeasure[3] = 1;
	menu.uOfMeasure[4] = 1;
	menu.uOfMeasure[5] = 0;
	menu.uOfMeasure[6] = 0;
	menu.uOfMeasure[7] = 0;
	menu.uOfMeasure[8] = 0;
	menu.uOfMeasure[9] = 0;
	menu.uOfMeasure[10] = 0;
	menu.uOfMeasure[11] = 0;
	menu.uOfMeasure[12] = 0;
	menu.uOfMeasure[13] = 0;
	menu.uOfMeasure[14] = 0;
 }
// установка битов обновлений экрана
 if((mode_Menu & BITS_MENU) != (mode_Menu_old & BITS_MENU)){
	// переход по основному меню: обновляем экран целиком
	mode_Menu = mode_Menu | BIT_UPDATE_MENU | BIT_UPDATE_SUBMENU | BIT_UPDATE_ALL_REGS
			              | BIT_UPDATE_VALUE;
	old_menu_printed = mode_Menu_old;
	mode_Menu_old = mode_Menu;

 }
 else if((mode_Menu & BITS_SUB_MENU) != (mode_Menu_old & BITS_SUB_MENU)){
	 // переход по SUB_MENU: обновляем подменю и значения экран
	 if(IsChangeScreen(mode_Menu, mode_Menu_old)){
		 // обновить пункты подменю и значения
			mode_Menu = mode_Menu | BIT_UPDATE_SUBMENU | BIT_UPDATE_ALL_REGS
					              | BIT_UPDATE_VALUE;

			if((mode_Menu & BITS_SUB_MENU)!=0) mode_Menu |= BIT_UPDATE_CURSOR;
         }
	 else{
		 // экран остался тотже
		 // перерисовать курсор
			mode_Menu = mode_Menu | BIT_UPDATE_CURSOR;
         }

//	    	mode_Menu_old = mode_Menu;
 }

 if((mode_Menu & BIT_MODIFY_REG) ||(mode_Menu_old & BIT_MODIFY_REG) ){
		 // модификация значения
		 // перерисовываем только текущее значение
	     mode_Menu |= BIT_UPDATE_REG;

	 }

// конец установки битов обновлений экрана
//====================================================

//====================================================
// обновление экрана
//====================================================

 //////////////////////////////////////////////////////////////
 // BIT_UPDATE_TIME - перерисовываем время
 //////////////////////////////////////////////////////////////

if(mode_Menu & BIT_UPDATE_TIME){
  // обновляем время
     HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
     //MODBUS_REG(0x45) = (sTime.Hours << 8) +sTime.Minutes;
     MODBUS_REG(0x45) = (sTime.Hours * 60) +sTime.Minutes;
     sprintf(strEC11Btn,"%02i:%02i:%02i", sTime.Hours, sTime.Minutes, sTime.Seconds);
     ST7789_print_u(110, 212, colorGray , colorLightGray , 1, &Font_16x26, 1, strEC11Btn);
     mode_Menu &= ~BIT_UPDATE_TIME;
     mode_Menu |= BIT_UPDATE_STATUS;
  }

//////////////////////////////////////////////////////////////
// BIT_UPDATE_STATUS - перерисовываем статус меню
//////////////////////////////////////////////////////////////

   if(mode_Menu & BIT_UPDATE_STATUS){
	 // обновляем статус меню
		  sprintf(strEC11Btn,"%06x", mode_Menu);
		  ST7789_print_u(0, 212, colorGray, colorLightGray, 1, &Font_16x26, 1, strEC11Btn);
		  mode_Menu &= ~BIT_UPDATE_STATUS;
		  status_printed = mode_Menu;
   }

   //////////////////////////////////////////////////////////////
   // BIT_UPDATE_MENU - перерисовываем заголовок меню
   //////////////////////////////////////////////////////////////

   if(mode_Menu & BIT_UPDATE_MENU){
	       // меню поменялось - устанавливаем бит "Перерисовать все регистры"
	       //mode_Menu |= BIT_UPDATE_ALL_REGS;
	       // сбрасываем бит обновления заголовка меню

	       mode_Menu &= ~BIT_UPDATE_MENU;
	       // вывод заголовка
	       if((mode_Menu & BITS_MENU) != 0){
	          ST7789_print_u(0, 0, colorWhite, colorBlue, 1, &Font_16x26, 1,menu.strMain_Menu[(mode_Menu & BITS_MENU) >> 4]);

	       }
	       else
	       {
	    	   //sprintf(strTmp,"*    %s", menu.strMenu[10][MODBUS_REG(0x49)]);
	    	   strncpy(strTmp, menu.strMenu[10][MODBUS_REG(REG_UNIT_TYPE)], 6);
	    	   sprintf(strTmp1, "*   %s    *",strTmp);
	    	   ST7789_print_u(0, 0, colorWhite , colorBlue, 1, &Font_16x26, 1, strTmp1);

	       }

	       if(password_OK == 0) ST7789_DrawImage_u(222, 0, 18, 26, lock_orange_18x26);
	       if(password_OK == 2) ST7789_DrawImage_u(222, 0, 18, 26, key_18x26);

	       menu_printed = mode_Menu & BITS_MENU;
   }

   //////////////////////////////////////////////////////////////
   // BIT_UPDATE_SUBMENU - перерисовываем пункты меню (подменю)
   //////////////////////////////////////////////////////////////

   if(mode_Menu & BIT_UPDATE_SUBMENU){
       // сбрасываем бит обновления пунктов подменю
       mode_Menu &= ~BIT_UPDATE_SUBMENU;
	 // обновляем подменю
	 /////////////////////////////////////////////////////
     // стирание экрана если предыдущая была картинка
      //if((mode_Menu_old & BITS_MENU) == MENU_PICTURE)
      if(draw_picture)
  	  {
  		 // предыдущий пункт меню была картинка - значит очистить экран
  		     ST7789_FillRect_u(0, 26, 240, 181, colorWhite);
  		     draw_picture = 0;       // флаг "схема нарисована"  сброшен
  	  }
  	 // вывод картинки
       if((mode_Menu & BITS_MENU) == 0x00)
       {
    	   // очищаем картинку
    	   ST7789_FillRect_u(0, 25, 240, 180, colorWhite);

    	   // вывод картинки
    	   if(MODBUS_REG(REG_UNIT_TYPE) == HVAC1)
    	   DrawHVAC1(0, 0);

    	   if(MODBUS_REG(REG_UNIT_TYPE) == HVAC2)
    	   {
    	    DrawHVAC2(0, 0); // вывод схемы
//    	    DrawData2(0, 0); // вывод данных
    	   }

    	   if(MODBUS_REG(REG_UNIT_TYPE) == HVAC3)
    	   {
    	    DrawHVAC3(0, 0); // вывод схемы
//    	    DrawData3(0, 15); // вывод данных
    	   }

    	   if(MODBUS_REG(REG_UNIT_TYPE) == HVAC4)
    	   {
    	    DrawHVAC4(0, 0); // вывод схемы
//    	    DrawData4(0, 15); // вывод данных
    	   }

    	   if(MODBUS_REG(REG_UNIT_TYPE) == HVAC5)
    	   {
    	    DrawHVAC5(0, 0); // вывод схемы
//    	    DrawData5(0, 15); // вывод данных
    	   }

        	mode_Menu_old = mode_Menu;
        	sub_menu_printed = mode_Menu & BITS_MENU;
        	draw_picture = 1;                           // схема нарисована

       }

	 // вывод пунктов меню
       else   if((mode_Menu & BITS_MENU) > 0x00)
	      {
//	    	  mode_Menu_old = mode_Menu;
	    	  for(uint8_t i=0;i<5;i++)  ST7789_print_u(SHIFT_X_PRINT_SUBMENU, SHIFT_Y_PRINT_SUBMENU+i*STEP_Y_PRINT_SUBMENU, colorBlack, colorWhite, 1, &Font_16x26, 1, menu.strMenu[(mode_Menu & 0x000000F0) >> 4][(i+5*numScreen)]);
	          sub_menu_printed = mode_Menu & BITS_MENU;
	      }
   }

   //------------------------------------------------------------
   // end BIT_UPDATE_SUBMENU
   //------------------------------------------------------------

   //////////////////////////////////////////////////////////////
   // BIT_UPDATE_ALL_REGS - перерисовываем изменение всех регистров
   //////////////////////////////////////////////////////////////
   if(mode_Menu & BIT_UPDATE_ALL_REGS){
   // сбросить бит обновить все значения
	   mode_Menu &= ~BIT_UPDATE_ALL_REGS;
   // обновляем значения всех регистров
		  if((mode_Menu & BITS_MENU) == MENU_SET_POINTS){
			  // вывести показания уставок
			  for(char i=0;i<5;i++){
					  sprintf(strTmp,"%3i", MODBUS_REG(START_REG_SETPOINTS+i+5*numScreen));
					  if((((mode_Menu & BITS_SUB_MENU)-1)%5 == i) && ((mode_Menu & BITS_SUB_MENU) != 0))
					  {
					   // если регистр выделен
					   if(mode_Menu & BIT_MODIFY_REG)
						  ST7789_print_u(SHIFT_PRINT_REGS, SHIFT_Y_PRINT_SUBMENU+i*STEP_Y_PRINT_SUBMENU, colorRed, colorWhite , 1, &Font_16x26, 1, strTmp);  // Red/White
					   else
						  ST7789_print_u(SHIFT_PRINT_REGS, SHIFT_Y_PRINT_SUBMENU+i*STEP_Y_PRINT_SUBMENU, colorWhite , colorBlue, 1, &Font_16x26, 1, strTmp);  // White/Blue

					  }
					  // регистр не выделен
					  else
						  ST7789_print_u(SHIFT_PRINT_REGS, SHIFT_Y_PRINT_SUBMENU+i*STEP_Y_PRINT_SUBMENU, colorBlue, colorWhite, 1, &Font_16x26, 1, strTmp);   // Blue/White

			  }
	          all_regs_printed = mode_Menu & BITS_MENU;
		  }  // end MENU_SET_POINTS

		  // MENU_ANALOG_OUT
		  if((mode_Menu & BITS_MENU) == MENU_ANALOG_OUT){
			  // вывести показания MENU_ANALOG_OUT!

			  for(char i=0;i<5;i++){
					  sprintf(strTmp,"%3i%%", MODBUS_REG(START_REG_AO_HND+i+5*numScreen));
					  if((((mode_Menu & BITS_SUB_MENU)-1)%5 == i) && ((mode_Menu & BITS_SUB_MENU) != 0))
					  {
					   // если регистр выделен
					   if(mode_Menu & BIT_MODIFY_REG)
						  ST7789_print_u(SHIFT_PRINT_REGS, SHIFT_Y_PRINT_SUBMENU+i*STEP_Y_PRINT_SUBMENU, colorRed, colorWhite , 1, &Font_16x26, 1, strTmp);  // Red/White
					   else
						  ST7789_print_u(SHIFT_PRINT_REGS, SHIFT_Y_PRINT_SUBMENU+i*STEP_Y_PRINT_SUBMENU, colorWhite , colorBlue, 1, &Font_16x26, 1, strTmp);  // White/Blue

					  }
					  // если регистр не выделен
					  else
						  ST7789_print_u(SHIFT_PRINT_REGS, SHIFT_Y_PRINT_SUBMENU+i*STEP_Y_PRINT_SUBMENU, colorBlue, colorWhite , 1, &Font_16x26, 1, strTmp);   // Blue/White

			  }

	          all_regs_printed = mode_Menu & BITS_MENU;

		  }  // end MENU_ANALOG_OUT

		  if(((mode_Menu & BITS_MENU) == MENU_SENSORS) || ((mode_Menu & BITS_MENU) == MENU_STATUS) ||
			 ((mode_Menu & BITS_MENU) == MENU_ANALOG_IN)){
		  // показания не выводим сделаем это по таймеру

			  all_regs_printed = mode_Menu & BITS_MENU;
		  }  // end MENU_SENSORS MENU_STATUS MENU_ANALOG_IN

		  if(((mode_Menu & BITS_MENU) == MENU_PICTURE) || ((mode_Menu & BITS_MENU) == MENU_VERSION) ||
			 ((mode_Menu & BITS_MENU) == MENU_SAVE_SP)){
		     // пункты меню без вывода значений

			  all_regs_printed = mode_Menu & BITS_MENU;
		  }  // end MENU_SENSORS MENU_STATUS MENU_ANALOG_IN

		  if((mode_Menu & BITS_MENU) == MENU_DIGITAL_OUT){
			  // вывести показания цифровых выходов
			  for(char i=0;i<5;i++){
					  if(((MODBUS_REG(REG_DO_HND) >> (i + 5*numScreen)) & 0x01) == 1)
					  ST7789_print_u(SHIFT_PRINT_REGS, SHIFT_Y_PRINT_SUBMENU+i*STEP_Y_PRINT_SUBMENU, colorGreen, colorWhite , 1, &Font_16x26, 1, "ON   ");
					  else{
						  // выход выключен
						  ST7789_print_u(SHIFT_PRINT_REGS, SHIFT_Y_PRINT_SUBMENU+i*STEP_Y_PRINT_SUBMENU, colorRed, colorWhite , 1, &Font_16x26, 1, "OFF  ");
					  }
		     }  //end for

	          all_regs_printed = mode_Menu & BITS_MENU;

		  }	  // end MENU_DIGITAL_OUT

		  // MENU_UNIT_MODE
		  if((mode_Menu & BITS_MENU) == MENU_UNIT_MODE){

			  for(uint8_t i=0; i < menu.pointsOfMenu[MENU_UNIT_MODE >> 4];i++){
				if(MODBUS_REG(REG_MODE) >= menu.pointsOfMenu[MENU_UNIT_MODE >> 4]){
					// ограничение
					MODBUS_REG(REG_MODE) = 0;
				}
			    if(MODBUS_REG(REG_MODE) == i){
			    	// выделенный пункт меню
			    	ST7789_DrawImage_u(SHIFT_PRINT_REGS, SHIFT_Y_PRINT_SUBMENU+i*STEP_Y_PRINT_SUBMENU, 26, 26, circle_check_26x26);
			    }
			    else{
			    	ST7789_DrawImage_u(SHIFT_PRINT_REGS, SHIFT_Y_PRINT_SUBMENU+i*STEP_Y_PRINT_SUBMENU, 26, 26, circle_26x26);
			    }
			  }

	          all_regs_printed = mode_Menu & BITS_MENU;

		  } //end MENU_UNIT_MODE

		  // MENU_ALARM  можно сделать только по таймеру!!!
		  if((mode_Menu & 0x000000F0) == MENU_ALARM){
			  uint16_t alarm = MODBUS_REG(REG_ALARM);
			  for(uint8_t i=0; i < 5;i++){

			    if((alarm >> (i + 5* numScreen)) & 0x01 == 1){
			    	// рисуем треугольник предупреждения !
			    	ST7789_DrawImage_u(SHIFT_PRINT_REGS, SHIFT_Y_PRINT_SUBMENU+i*STEP_Y_PRINT_SUBMENU, 26, 26, alarm_26x26);
			    }
			    else{
			    	ST7789_print_u(SHIFT_PRINT_REGS, SHIFT_Y_PRINT_SUBMENU+i*STEP_Y_PRINT_SUBMENU, colorWhite, colorWhite, 1, &Font_16x26, 1, "  ");
			    }
			  }

	          all_regs_printed = mode_Menu & BITS_MENU;

		  } //end MENU_UNIT_MODE

		  // NOTE: MENU_UNIT_TYPE
		  if((mode_Menu & BITS_MENU) == MENU_UNIT_TYPE){
			  uint16_t temp = MODBUS_REG(0x49);
			  for(uint8_t i=0; i < 5/* menu.pointsOfMenu[MENU_UNIT_MODE >> 4]*/ ;i++){
				if((MODBUS_REG(REG_UNIT_TYPE) >= 5 ) || (MODBUS_REG(0x49) < 0 )){
					// ограничение
				//	MODBUS_REG(0x49) = 0;
				}
			    if(MODBUS_REG(REG_UNIT_TYPE) == i + 5*numScreen){
			    	// рисуем кружок с точкой (выбран)
			    	ST7789_DrawImage_u(SHIFT_PRINT_REGS, SHIFT_Y_PRINT_SUBMENU+i*STEP_Y_PRINT_SUBMENU, 26, 26, circle_check_26x26);
			    }
			    else{
			    	ST7789_DrawImage_u(SHIFT_PRINT_REGS, SHIFT_Y_PRINT_SUBMENU+i*STEP_Y_PRINT_SUBMENU, 26, 26, circle_26x26);
			    }
			  }

	          all_regs_printed = mode_Menu & BITS_MENU;

		  } //end MENU_UNIT_TYPE

		  //  MENU_TIME
		  if((mode_Menu & 0x000000F0) == MENU_TIME){
			  for(uint8_t i=0; i < 5; i++){
//			   uint8_t k = i % 2;
			   sprintf(strTmp,"%02i:", MODBUS_REG(REG_TIME_START + i) >> 8);
			   ST7789_print_u(SHIFT_PRINT_TIME , SHIFT_Y_PRINT_SUBMENU+i*STEP_Y_PRINT_SUBMENU, colorBlue, colorWhite , 1, &Font_16x26, 1, strTmp);
			   sprintf(strTmp,"%02i", MODBUS_REG(REG_TIME_START + i) & 0xFF);
			   ST7789_print_u(SHIFT_PRINT_TIME+SHIFT_PRINT_MINUTE , SHIFT_Y_PRINT_SUBMENU+i*STEP_Y_PRINT_SUBMENU, colorBlue, colorWhite , 1, &Font_16x26, 1, strTmp);
			  }

	          all_regs_printed = mode_Menu & BITS_MENU;

		  }  // end MENU_TIME

		  //  MENU_PASSWORD
		  if((mode_Menu & 0x000000F0) == MENU_PASSWORD){
			  for(uint8_t i=0; i < 4; i++){
			   sprintf(strTmp,"%1x", password[i] /*(MODBUS_REG(0x40) >> (4*i))*/  & 0xf);
			   ST7789_print_u(60+i*30 , SHIFT_Y_PRINT_SUBMENU, colorBlue, colorWhite , 1, &Font_16x26, 1, strTmp);
			  }

	          all_regs_printed = mode_Menu & BITS_MENU;

		  }  // end MENU_PASSWORD
   } // конец обновления всех регистров

   //------------------------------------------------------------
   // end BIT_UPDATE_ALL_REGS
   //------------------------------------------------------------

   //////////////////////////////////////////////////////////////
   // BIT_UPDATE_CURSOR - перерисовываем изменение курсора
   //////////////////////////////////////////////////////////////

   if(mode_Menu & BIT_UPDATE_CURSOR){
	   // обновление позиции курсора
	   mode_Menu &= ~BIT_UPDATE_CURSOR;
	   uint8_t i= ((mode_Menu & BITS_SUB_MENU)-1) % 5;
	   uint8_t j= ((mode_Menu_old & BITS_SUB_MENU)-1) % 5;

	   //  MENU_SET_POINTS
	   if((mode_Menu & BITS_MENU) == MENU_SET_POINTS){
		   static uint8_t j=0;
		   if(((mode_Menu & BITS_SUB_MENU) == 1) && ((mode_Menu_old & BITS_SUB_MENU) == 0)){
		   sprintf(strTmp,"%3i", MODBUS_REG(0x20+i+5*numScreen));
		   ST7789_print_u(SHIFT_PRINT_REGS, SHIFT_Y_PRINT_SUBMENU+0*STEP_Y_PRINT_SUBMENU, colorWhite, colorBlue, 1, &Font_16x26, 1, strTmp);
		   }
		   else{
			   sprintf(strTmp,"%3i", MODBUS_REG(START_REG_SETPOINTS+j+5*numScreen));
			   ST7789_print_u(SHIFT_PRINT_REGS, SHIFT_Y_PRINT_SUBMENU+j*STEP_Y_PRINT_SUBMENU, colorBlue, colorWhite, 1, &Font_16x26, 1, strTmp);
			   sprintf(strTmp,"%3i", MODBUS_REG(START_REG_SETPOINTS+i+5*numScreen));
			   ST7789_print_u(SHIFT_PRINT_REGS, SHIFT_Y_PRINT_SUBMENU+i*STEP_Y_PRINT_SUBMENU, colorWhite, colorBlue, 1, &Font_16x26, 1, strTmp);
		   }
		   j = i;
	   }  //end  MENU_SET_POINTS

	   // MENU_ANALOG_OUT
	   if((mode_Menu & BITS_MENU) == MENU_ANALOG_OUT){
		   static uint8_t j=0;
		   if(((mode_Menu & BITS_SUB_MENU) == 1) && ((mode_Menu_old & BITS_SUB_MENU) == 0)){
		   sprintf(strTmp,"%3i%%", MODBUS_REG(START_REG_AO_HND+i+5*numScreen));
		   ST7789_print_u(SHIFT_PRINT_REGS, SHIFT_Y_PRINT_SUBMENU+0*STEP_Y_PRINT_SUBMENU, colorWhite, colorBlue, 1, &Font_16x26, 1, strTmp);
		   }
		   else{
			   sprintf(strTmp,"%3i%%", MODBUS_REG(START_REG_AO_HND+j+5*numScreen));
			   ST7789_print_u(SHIFT_PRINT_REGS, SHIFT_Y_PRINT_SUBMENU+j*STEP_Y_PRINT_SUBMENU, colorBlue, colorWhite, 1, &Font_16x26, 1, strTmp);
			   sprintf(strTmp,"%3i%%", MODBUS_REG(START_REG_AO_HND+i+5*numScreen));
			   ST7789_print_u(SHIFT_PRINT_REGS, SHIFT_Y_PRINT_SUBMENU+i*STEP_Y_PRINT_SUBMENU, colorWhite, colorBlue, 1, &Font_16x26, 1, strTmp);
		   }
		   j = i;
	   }  //end  MENU_ANALOG_OUT

	   // end MENU_ANALOG_OUT

	   // MENU_PASSWORD
	   if((mode_Menu & BITS_MENU) == MENU_PASSWORD){
		   uint8_t l= ((mode_Menu & BITS_SUB_MENU)-1) % 4;
		   static uint8_t j=0;
		   if(((mode_Menu & BITS_SUB_MENU) == 0) && ((mode_Menu_old & BITS_SUB_MENU) == 4)){
			   sprintf(strTmp,"%1x", password[l] & 0xf); // sprintf(strTmp,"%1x", (MODBUS_REG(0x46) >> (4*l)) & 0xf);
			   ST7789_print_u(60+3*30, SHIFT_Y_PRINT_SUBMENU, colorBlue, colorWhite, 1, &Font_16x26, 1, strTmp);
			   // записываем введеный пароль  пароль в MODBUS_REG(46)
			   MODBUS_REG(REG_PASSWORD) = password[0]*4096 + password[1]*256  + password[2]*16  + password[3];
			   for(uint8_t i=0; i<4; i++) password[i] = 0; // обнуляем прмежуточный буфер
			   // проврка правильности пароля
			   if(MODBUS_REG(REG_PASSWORD) == password_corect)
			   {
				    password_OK = 1;
				    mode_Menu |= 0x100000;
				    ST7789_print_u(0, 0, colorWhite, colorBlue, 1, &Font_16x26, 1,menu.strMain_Menu[(mode_Menu & BITS_MENU) >> 4]);
			   }
			   else if(MODBUS_REG(REG_PASSWORD) == password_admin)
			   {
				   password_OK = 2;
				   mode_Menu |= 0x100000;
				   ST7789_DrawImage_u(222, 0, 18, 26, key_18x26);
			   }
			   else
			   {
				   password_OK = 0;
				   mode_Menu &= ~0x100000;
				   ST7789_DrawImage_u(222, 0, 18, 26, lock_orange_18x26);
			   }
		   }
		   else if(((mode_Menu & BITS_SUB_MENU) == 1) && ((mode_Menu_old & BITS_SUB_MENU) == 0)){
			   sprintf(strTmp,"%1x", password[l] & 0xf); //sprintf(strTmp,"%1x", (MODBUS_REG(0x46) >> (4*l)) & 0xf);
		   ST7789_print_u(60+l*30, SHIFT_Y_PRINT_SUBMENU, colorWhite, colorBlue, 1, &Font_16x26, 1, strTmp);
		   }
		   else{
			   sprintf(strTmp,"%1x", password[j] & 0xf); //sprintf(strTmp,"%1x", (MODBUS_REG(0x46) >> (4*j)) & 0xf);
			   ST7789_print_u(60+j*30, SHIFT_Y_PRINT_SUBMENU, colorBlue, colorWhite, 1, &Font_16x26, 1, strTmp);
			   sprintf(strTmp,"%1x", password[l] & 0xf); //sprintf(strTmp,"%1x", (MODBUS_REG(0x46) >> (4*l)) & 0xf);
			   ST7789_print_u(60+l*30, SHIFT_Y_PRINT_SUBMENU, colorWhite, colorBlue, 1, &Font_16x26, 1, strTmp);
		   }
		   j = l;
	   }  //end  MENU_PASSWORD


	   // MENU_DIGITAL_OUT
	   if((mode_Menu & BITS_MENU) == MENU_DIGITAL_OUT){
		   if(((mode_Menu & BITS_SUB_MENU) == 1) && ((mode_Menu_old & BITS_SUB_MENU) == 0)){
            // рисуем курсоср в первой строке
			if((MODBUS_REG(REG_DO_HND)  & 0x01) == 1)
				  ST7789_print_u(SHIFT_PRINT_REGS, SHIFT_Y_PRINT_SUBMENU+0*30, colorGreen, colorBlue, 1, &Font_16x26, 1, "ON   ");
			else{
					  ST7789_print_u(SHIFT_PRINT_REGS, SHIFT_Y_PRINT_SUBMENU+0*30, colorRed, colorBlue, 1, &Font_16x26, 1, "OFF  ");
				}

		   }
		   else{
			   // стираем курсор в предыдущей позиции и рисуем в новой
			   // j - стираем
				  if(((MODBUS_REG(REG_DO_HND) >> (j + 5*numScreen)) & 0x01) == 1)
				  ST7789_print_u(SHIFT_PRINT_REGS, SHIFT_Y_PRINT_SUBMENU+j*STEP_Y_PRINT_SUBMENU, colorGreen, colorWhite, 1, &Font_16x26, 1, "ON   ");
				  else{
					  ST7789_print_u(SHIFT_PRINT_REGS, SHIFT_Y_PRINT_SUBMENU+j*STEP_Y_PRINT_SUBMENU, colorRed, colorWhite, 1, &Font_16x26, 1, "OFF  ");
				  }
				   // i - рисуем
					  if(((MODBUS_REG(REG_DO_HND) >> (i + 5*numScreen)) & 0x01) == 1)
					      ST7789_print_u(SHIFT_PRINT_REGS, SHIFT_Y_PRINT_SUBMENU+i*STEP_Y_PRINT_SUBMENU, colorGreen, colorBlue, 1, &Font_16x26, 1, "ON   ");
					  else{
						  ST7789_print_u(SHIFT_PRINT_REGS, SHIFT_Y_PRINT_SUBMENU+i*STEP_Y_PRINT_SUBMENU, colorRed, colorBlue, 1, &Font_16x26, 1, "OFF  ");
					  }
		   }
	   }  //end  MENU_DIGITAL_OUT


	   // MENU_PICTURE
	   if((mode_Menu & BITS_MENU) == MENU_PICTURE){
		uint16_t backColor, backColor_Error;

		if((mode_Menu & 0x0F) == 2)  backColor = RGB565(100,100,255);
   		else backColor = RGB565(200,200,255);

		// прорисовка переключателя СТОП/ПСК/АВТО
   		  if(mode_Menu & BIT_MODIFY_REG)
   			  ST7789_print_u(25, 187, colorRed, backColor, 1, &Font_11x18, 1, menu.strMenu[14][MODBUS_REG(REG_START_AUTO)]);
   		  else
   			  ST7789_print_u(25, 187, RGB565(10, 10, 10), backColor, 1, &Font_11x18, 1, menu.strMenu[14][MODBUS_REG(REG_START_AUTO)]);

//  		if((mode_Menu & 0x0F) == 1)  backColor_Error = RGB565(255,200,200);
//     		else backColor_Error = colorWhite;

  		if(MODBUS_REG(REG_ALARM))
  		   ST7789_print_u(90, 30, colorRed, colorWhite, 1, &Font_11x18, 1, " Ошибка!");
  		else  ST7789_print_u(90, 30, colorRed, colorWhite, 1, &Font_11x18, 1, "        ");


  		 // кнопка
  		 if((mode_Menu & 0x7ff) == 0x101){
  		   ST7789_DrawImage_u(115+0, 181, 120, 24, Reset_21_120x24);
  		 }

  		 else if((mode_Menu & 0x7ff) == 0x301){
  		   ST7789_DrawImage_u(115+0, 181, 120, 24, Reset_31_120x24);
  		 }

  		 else ST7789_DrawImage_u(115+0, 181, 120, 24, Reset_11_120x24);

//  		 // кнопка
//  		 if((g_mode_Menu & 0x0F) == 1){
//  		   ST7789_DrawImage_u(185+0, 15+15, 49, 30, Button_3_49x30);
//  		 }
//  		 else if((g_mode_Menu & 0x0F) == 2){
//  		   ST7789_DrawImage_u(185+0, 15+15, 49, 30, Button_3_49x30);
//  		 }
//  		 else  ST7789_DrawImage_u(185+0, 15+15, 49, 30, Button_1_49x30);

	   }//end  MENU_PICTURE



	   //  MENU_UNIT_MODE
	   if((mode_Menu & BITS_MENU) == MENU_UNIT_MODE){

		   // предыдущее значение рег-ра 0х48 чтобы знать что стирать
		   static uint16_t modBusReg0x48_old = 0;

		   if(((mode_Menu & BITS_SUB_MENU) == 1) && ((mode_Menu_old & BITS_SUB_MENU) == 0)){
            // рисуем курсоср в первой строке
			if(MODBUS_REG(REG_MODE) == 0)
				 ST7789_DrawImage_u(SHIFT_PRINT_REGS, SHIFT_Y_PRINT_SUBMENU+0*STEP_Y_PRINT_SUBMENU, 26, 26, circle_check_cursor_26x26);
			else{
				 ST7789_DrawImage_u(SHIFT_PRINT_REGS, SHIFT_Y_PRINT_SUBMENU+0*STEP_Y_PRINT_SUBMENU, 26, 26, circle_cursor_26x26);
				}

		   }
		   else{
			   // стираем курсор в предыдущей позиции и рисуем в новой
			   // вычисляем предыдущее значение
			   //uint16_t k = (1 << modBusReg0x48_old ) - 1;
			   // стираем предыдущее значение
			   for(uint8_t k=0; k< 4;k++){
					  if(MODBUS_REG(REG_MODE) == k)
						  ST7789_DrawImage_u(SHIFT_PRINT_REGS, SHIFT_Y_PRINT_SUBMENU+k*STEP_Y_PRINT_SUBMENU, 26, 26, circle_check_26x26);
					  else{
						  ST7789_DrawImage_u(SHIFT_PRINT_REGS, SHIFT_Y_PRINT_SUBMENU+k*STEP_Y_PRINT_SUBMENU, 26, 26, circle_26x26);
					  }
			   }
			   if((mode_Menu & BITS_SUB_MENU) !=  0){
				      // если курсор не в позиции 0 то рисуем курсос
				      // i - рисуем
					  if(MODBUS_REG(0x48) == i)
						  ST7789_DrawImage_u(SHIFT_PRINT_REGS, SHIFT_Y_PRINT_SUBMENU+i*STEP_Y_PRINT_SUBMENU, 26, 26, circle_check_cursor_26x26);
					  else{
						  ST7789_DrawImage_u(SHIFT_PRINT_REGS, SHIFT_Y_PRINT_SUBMENU+i*STEP_Y_PRINT_SUBMENU, 26, 26, circle_cursor_26x26);
					  }
			   } // конец рисования курсора
		   }

		mode_Menu_old = mode_Menu;
		modBusReg0x48_old = MODBUS_REG(REG_MODE);
	   }  //end  MENU_UNIT_MODE


	   //  MENU_UNIT_TYPE
	   if((mode_Menu & BITS_MENU) == MENU_UNIT_TYPE){

		   // предыдущее значение рег-ра 0х48 чтобы знать что стирать
		   static uint16_t modBusReg0x49_old = 0;

		   if(((mode_Menu & BITS_SUB_MENU) == 1) && ((mode_Menu_old & BITS_SUB_MENU) == 0)){
            // рисуем курсоср в первой строке
			if(MODBUS_REG(REG_UNIT_TYPE) == 0)
				 ST7789_DrawImage_u(SHIFT_PRINT_REGS, SHIFT_Y_PRINT_SUBMENU+0*STEP_Y_PRINT_SUBMENU, 26, 26, circle_check_cursor_26x26);
			else{
				 ST7789_DrawImage_u(SHIFT_PRINT_REGS, SHIFT_Y_PRINT_SUBMENU+0*STEP_Y_PRINT_SUBMENU, 26, 26, circle_cursor_26x26);
				}

		   }
		   else{
			   // стираем курсор в предыдущей позиции и рисуем в новой
			   // вычисляем предыдущее значение
			   //uint16_t k = (1 << modBusReg0x48_old ) - 1;
			   // стираем предыдущее значение
			   for(uint8_t k=0; k< 5;k++){
					  if(MODBUS_REG(REG_UNIT_TYPE) == k + 5*numScreen)
						  ST7789_DrawImage_u(SHIFT_PRINT_REGS, SHIFT_Y_PRINT_SUBMENU+k*STEP_Y_PRINT_SUBMENU, 26, 26, circle_check_26x26);
					  else{
						  ST7789_DrawImage_u(SHIFT_PRINT_REGS, SHIFT_Y_PRINT_SUBMENU+k*STEP_Y_PRINT_SUBMENU, 26, 26, circle_26x26);
					  }
			   }
			   if((mode_Menu & BITS_SUB_MENU) !=  0){
				      // если курсор не в позиции 0 то рисуем курсор
				      // i - рисуем
					  if(MODBUS_REG(REG_UNIT_TYPE) == (i + 5*numScreen))
						  ST7789_DrawImage_u(SHIFT_PRINT_REGS, SHIFT_Y_PRINT_SUBMENU+i*STEP_Y_PRINT_SUBMENU, 26, 26, circle_check_cursor_26x26);
					  else{
						  ST7789_DrawImage_u(SHIFT_PRINT_REGS, SHIFT_Y_PRINT_SUBMENU+i*STEP_Y_PRINT_SUBMENU, 26, 26, circle_cursor_26x26);
					  }
			   } // конец рисования курсора
		   }

		mode_Menu_old = mode_Menu;
		modBusReg0x49_old = MODBUS_REG(REG_UNIT_TYPE);
	   }  //end  MENU_UNIT_TYPE


	   //MENU_SENSORS
	   if((mode_Menu & BITS_MENU) == MENU_SENSORS){

		   if(((mode_Menu & BITS_SUB_MENU) == 1) && ((mode_Menu_old & BITS_SUB_MENU) == 0)){
               // рисуем курсоср в первой строке
			   ST7789_print_u(SHIFT_PRINT_CURSOR, SHIFT_Y_PRINT_SUBMENU+0*STEP_Y_PRINT_SUBMENU, colorBlue, colorWhite, 1, &Font_16x26, 1, "<");
			   }
		       else if ((mode_Menu & BITS_SUB_MENU) == 0){
			   // стираем курсор в первой позиции
			   ST7789_print_u(SHIFT_PRINT_CURSOR, SHIFT_Y_PRINT_SUBMENU+0*STEP_Y_PRINT_SUBMENU, colorWhite, colorWhite, 1, &Font_16x26, 1, " ");
		       }
			   else{
				   // стираем курсор в предыдущей позиции и рисуем в новой
				   ST7789_print_u(SHIFT_PRINT_CURSOR, SHIFT_Y_PRINT_SUBMENU+j*STEP_Y_PRINT_SUBMENU, colorWhite, colorWhite, 1, &Font_16x26, 1, " ");
				   ST7789_print_u(SHIFT_PRINT_CURSOR, SHIFT_Y_PRINT_SUBMENU+i*STEP_Y_PRINT_SUBMENU, colorBlue, colorWhite, 1, &Font_16x26, 1, "<");
			   }

	   } //end  MENU_SENSORS

	   // MENU_STATUS
	   if((mode_Menu & BITS_MENU) == MENU_STATUS){

		   if(((mode_Menu & BITS_SUB_MENU) == 1) && ((mode_Menu_old & BITS_SUB_MENU) == 0)){
               // рисуем курсоср в первой строке
			   ST7789_print_u(SHIFT_PRINT_CURSOR, SHIFT_Y_PRINT_SUBMENU+0*STEP_Y_PRINT_SUBMENU, colorBlue, colorWhite, 1, &Font_16x26, 1, "<");
			   }
		       else if ((mode_Menu & BITS_SUB_MENU) == 0){
			   // стираем курсор в первой позиции
			   ST7789_print_u(SHIFT_PRINT_CURSOR, SHIFT_Y_PRINT_SUBMENU+0*STEP_Y_PRINT_SUBMENU, colorWhite, colorWhite, 1, &Font_16x26, 1, " ");
		       }
			   else{
				   // стираем курсор в предыдущей позиции и рисуем в новой
				   ST7789_print_u(SHIFT_PRINT_CURSOR, SHIFT_Y_PRINT_SUBMENU+j*STEP_Y_PRINT_SUBMENU, colorWhite, colorWhite, 1, &Font_16x26, 1, " ");
				   ST7789_print_u(SHIFT_PRINT_CURSOR, SHIFT_Y_PRINT_SUBMENU+i*STEP_Y_PRINT_SUBMENU, colorBlue, colorWhite, 1, &Font_16x26, 1, "<");
			   }

	   } // end MENU_STATUS

	   // MENU_ANALOG_IN
	   if((mode_Menu & BITS_MENU) == MENU_ANALOG_IN){

		   if(((mode_Menu & BITS_SUB_MENU) == 1) && ((mode_Menu_old & BITS_SUB_MENU) == 0)){
               // рисуем курсоср в первой строке
			   ST7789_print_u(SHIFT_PRINT_CURSOR, SHIFT_Y_PRINT_SUBMENU+0*STEP_Y_PRINT_SUBMENU, colorBlue, colorWhite, 1, &Font_16x26, 1, "<");
			   }
		       else if ((mode_Menu & BITS_SUB_MENU) == 0){
			   // стираем курсор в первой позиции
			   ST7789_print_u(SHIFT_PRINT_CURSOR, SHIFT_Y_PRINT_SUBMENU+0*STEP_Y_PRINT_SUBMENU, colorWhite, colorWhite, 1, &Font_16x26, 1, " ");
		       }
			   else{
				   // стираем курсор в предыдущей позиции и рисуем в новой
				   ST7789_print_u(SHIFT_PRINT_CURSOR, SHIFT_Y_PRINT_SUBMENU+j*STEP_Y_PRINT_SUBMENU, colorWhite, colorWhite, 1, &Font_16x26, 1, " ");
				   ST7789_print_u(SHIFT_PRINT_CURSOR, SHIFT_Y_PRINT_SUBMENU+i*STEP_Y_PRINT_SUBMENU, colorBlue, colorWhite, 1, &Font_16x26, 1, "<");
			   }

	   } // end MENU_ANALOG_IN

	   // MENU_ALARM
	   if((mode_Menu & BITS_MENU) == MENU_ALARM){

		   if(((mode_Menu & BITS_SUB_MENU) == 1) && ((mode_Menu_old & BITS_SUB_MENU) == 0)){
               // рисуем курсоср в первой строке
			   ST7789_print_u(SHIFT_PRINT_CURSOR, SHIFT_Y_PRINT_SUBMENU+0*STEP_Y_PRINT_SUBMENU, colorBlue, colorWhite, 1, &Font_16x26, 1, "<");
			   }
		       else if ((mode_Menu & BITS_SUB_MENU) == 0){
			   // стираем курсор в первой позиции
			   ST7789_print_u(SHIFT_PRINT_CURSOR, SHIFT_Y_PRINT_SUBMENU+0*STEP_Y_PRINT_SUBMENU, colorWhite, colorWhite, 1, &Font_16x26, 1, " ");
		       }
			   else{
				   // стираем курсор в предыдущей позиции и рисуем в новой
				   ST7789_print_u(SHIFT_PRINT_CURSOR, SHIFT_Y_PRINT_SUBMENU+j*STEP_Y_PRINT_SUBMENU, colorWhite, colorWhite, 1, &Font_16x26, 1, " ");
				   ST7789_print_u(SHIFT_PRINT_CURSOR, SHIFT_Y_PRINT_SUBMENU+i*STEP_Y_PRINT_SUBMENU, colorBlue, colorWhite, 1, &Font_16x26, 1, "<");
			   }

	   } // end MENU_ALARM


	   // MENU_TIME
	   if((mode_Menu & BITS_MENU) == MENU_TIME){
		  uint8_t d= ((mode_Menu & BITS_SUB_MENU)-1) % 10; // пункт меню
		  uint8_t k = d % 2;  // строка
		  // предыдущая позиция курсосра
		  static uint8_t d_old = 0;

		   if(((mode_Menu & BITS_SUB_MENU) == 1) && ((mode_Menu_old & BITS_SUB_MENU) == 0)){

			   sprintf(strTmp,"%02i", MODBUS_REG(REG_TIME_START) >> 8);
			   ST7789_print_u(SHIFT_PRINT_REGS , SHIFT_Y_PRINT_SUBMENU+0*STEP_Y_PRINT_SUBMENU, colorWhite, colorBlue, 1, &Font_16x26, 1, strTmp);
               d_old = 0;
		   }
		   else{
			   // стираем прошлый курсор
			   if(!(d_old % 2)){
			   	sprintf(strTmp,"%02i", MODBUS_REG(REG_TIME_START + d_old/2) >> 8);
			   }
			   else{
				sprintf(strTmp,"%02i", MODBUS_REG(REG_TIME_START + d_old/2) & 0xff);
			   }

			   ST7789_print_u(SHIFT_PRINT_REGS + (d_old % 2)*SHIFT_PRINT_MINUTE , SHIFT_Y_PRINT_SUBMENU+(d_old/2)*STEP_Y_PRINT_SUBMENU, colorBlue, colorWhite, 1, &Font_16x26, 1, strTmp);

		   }
			   // рисуем курсор в новом месте если подменю != 0
			   if((mode_Menu & BITS_SUB_MENU) != 0){
			   if(!(d % 2)){
			   	sprintf(strTmp,"%02i", MODBUS_REG(0x40 + d/2) >> 8);

			   }
			   else{
				sprintf(strTmp,"%02i", MODBUS_REG(0x40 + d/2) & 0xff);

			   }
			    ST7789_print_u(SHIFT_PRINT_REGS + k*SHIFT_PRINT_MINUTE , SHIFT_Y_PRINT_SUBMENU+(d/2)*STEP_Y_PRINT_SUBMENU, colorWhite, colorBlue, 1, &Font_16x26, 1, strTmp);
			    d_old = d;
			  }

	   }  // end MENU_TIME


	   // MENU_SAVE_SP
	   if((mode_Menu & BITS_MENU) == MENU_SAVE_SP){
		   if((mode_Menu & BITS_SUB_MENU) == 1){

			   ST7789_print_u(SHIFT_X_PRINT_SUBMENU, SHIFT_Y_PRINT_SUBMENU+0*STEP_Y_PRINT_SUBMENU, colorWhite, colorBlue, 1, &Font_16x26, 1, menu.strMenu[(mode_Menu & 0x000000F0) >> 4][(0+5*numScreen)]);
		   }
		   else{
			   ST7789_print_u(SHIFT_X_PRINT_SUBMENU, SHIFT_Y_PRINT_SUBMENU+0*STEP_Y_PRINT_SUBMENU, colorBlack, colorWhite, 1, &Font_16x26, 1, menu.strMenu[(mode_Menu & 0x000000F0) >> 4][(0+5*numScreen)]);
		   }
	   } // end MENU_SAVE_SP

   } // end BIT_UPDATE_CURSOR
   //------------------------------------------------------------
   // end BIT_UPDATE_CURSOR
   //------------------------------------------------------------

   //////////////////////////////////////////////////////////////
   // BIT_UPDATE_REG  - перерисовываем изменение аналоговых сигналов
   //////////////////////////////////////////////////////////////
   // Перерисовка одного рег-ра
   if(mode_Menu & BIT_UPDATE_REG){
      mode_Menu &= ~BIT_UPDATE_REG;

      // MENU_SET_POINTS
      if((mode_Menu & BITS_MENU) == MENU_SET_POINTS){
      uint8_t i= ((mode_Menu & BITS_SUB_MENU)-1) % 5;
	  sprintf(strTmp,"%3i", MODBUS_REG(START_REG_SETPOINTS+i+5*numScreen));
	  if(mode_Menu & BIT_MODIFY_REG)
	  ST7789_print_u(SHIFT_PRINT_REGS, SHIFT_Y_PRINT_SUBMENU+i*STEP_Y_PRINT_SUBMENU, colorRed, colorWhite, 1, &Font_16x26, 1, strTmp);
	  else
		  ST7789_print_u(SHIFT_PRINT_REGS, SHIFT_Y_PRINT_SUBMENU+i*STEP_Y_PRINT_SUBMENU, colorWhite, colorBlue, 1, &Font_16x26, 1, strTmp);
      }  // end MENU_SET_POINTS

      // MENU_ANALOG_OUT
      if((mode_Menu & BITS_MENU) == MENU_ANALOG_OUT){
      uint8_t i= ((mode_Menu & BITS_SUB_MENU)-1) % 5;
	  sprintf(strTmp,"%3i%%", MODBUS_REG(START_REG_AO_HND+i+5*numScreen));
	  if(mode_Menu & BIT_MODIFY_REG)
	  ST7789_print_u(SHIFT_PRINT_REGS, SHIFT_Y_PRINT_SUBMENU+i*STEP_Y_PRINT_SUBMENU, colorRed, colorWhite, 1, &Font_16x26, 1, strTmp);
	  else
		  ST7789_print_u(SHIFT_PRINT_REGS, SHIFT_Y_PRINT_SUBMENU+i*STEP_Y_PRINT_SUBMENU, colorWhite, colorBlue, 1, &Font_16x26, 1, strTmp);
      }  // end MENU_ANALOG_OUT


      //=================================================================================================
//	   if((mode_Menu & BITS_MENU) == MENU_PASSWORD){
//		   uint8_t l= ((mode_Menu & BITS_SUB_MENU)-1) % 4;
//		   static uint8_t j=0;
//		   if(((mode_Menu & BITS_SUB_MENU) == 0) && ((mode_Menu_old & BITS_SUB_MENU) == 4)){
//			   sprintf(strTmp,"%1x", (MODBUS_REG(0x40) >> (4*l)) & 0xf);
//			   ST7789_print_u(60+3*30, SHIFT_Y_PRINT_SUBMENU, colorBlue, colorWhite, 1, &Font_16x26, 1, strTmp);
//		   }
//		   else if(((mode_Menu & BITS_SUB_MENU) == 1) && ((mode_Menu_old & BITS_SUB_MENU) == 0)){
//		   sprintf(strTmp,"%1x", (MODBUS_REG(0x40) >> (4*l)) & 0xf);
//		   ST7789_print_u(60+l*30, SHIFT_Y_PRINT_SUBMENU, colorWhite, colorBlue, 1, &Font_16x26, 1, strTmp);
//		   }
//		   else{
//			   sprintf(strTmp,"%1x", (MODBUS_REG(0x40) >> (4*j)) & 0xf);
//			   ST7789_print_u(60+j*30, SHIFT_Y_PRINT_SUBMENU, colorBlue, colorWhite, 1, &Font_16x26, 1, strTmp);
//			   sprintf(strTmp,"%1x", (MODBUS_REG(0x40) >> (4*l)) & 0xf);
//			   ST7789_print_u(60+l*30, SHIFT_Y_PRINT_SUBMENU, colorWhite, colorBlue, 1, &Font_16x26, 1, strTmp);
//		   }
//		   j = l;
//	   }  //end  MENU_PASSWORD
     //==================================================================================================
      // MENU_PASSWORD
      if((mode_Menu & BITS_MENU) == MENU_PASSWORD){
		uint8_t l= ((mode_Menu & BITS_SUB_MENU)-1) % 4;
//	    static uint8_t j=0;
		 sprintf(strTmp,"%1x", password[l] & 0xf); //sprintf(strTmp,"%1x", (MODBUS_REG(0x46) >> (4*l)) & 0xf);
	    if(mode_Menu & BIT_MODIFY_REG)
        ST7789_print_u(60+l*30, SHIFT_Y_PRINT_SUBMENU, colorRed, colorWhite, 1, &Font_16x26, 1, strTmp);
//	    ST7789_print_u(SHIFT_PRINT_REGS, SHIFT_Y_PRINT_SUBMENU+i*STEP_Y_PRINT_SUBMENU, colorRed, colorWhite, 1, &Font_16x26, 1, strTmp);
	    else
      ST7789_print_u(60+l*30, SHIFT_Y_PRINT_SUBMENU, colorWhite, colorBlue, 1, &Font_16x26, 1, strTmp);
//		ST7789_print_u(SHIFT_PRINT_REGS, SHIFT_Y_PRINT_SUBMENU+i*STEP_Y_PRINT_SUBMENU, colorWhite, colorBlue, 1, &Font_16x26, 1, strTmp);
      }  // end MENU_PASSWORD
     //==================================================================================================



      // MENU_TIME
      if((mode_Menu & BITS_MENU) == MENU_TIME){

		   // рисуем курсор в новом месте если подменю != 0
		   if((mode_Menu & BITS_SUB_MENU) != 0){
		   uint8_t d= ((mode_Menu & BITS_SUB_MENU)-1) % 10; // пункт меню
		   uint8_t k = d % 2;  // строка

		   if(!(d % 2)){
		   	sprintf(strTmp,"%02i", MODBUS_REG(REG_TIME_START + d/2) >> 8);

		   }
		   else{
			sprintf(strTmp,"%02i", MODBUS_REG(REG_TIME_START + d/2) & 0xff);

		   }
		   if(mode_Menu & BIT_MODIFY_REG){
		    ST7789_print_u(SHIFT_PRINT_REGS + k*SHIFT_PRINT_MINUTE , SHIFT_Y_PRINT_SUBMENU+(d/2)*STEP_Y_PRINT_SUBMENU, colorRed, colorWhite, 1, &Font_16x26, 1, strTmp);
		   }
		   else{
			    ST7789_print_u(SHIFT_PRINT_REGS + k*SHIFT_PRINT_MINUTE , SHIFT_Y_PRINT_SUBMENU+(d/2)*STEP_Y_PRINT_SUBMENU, colorWhite, colorBlue, 1, &Font_16x26, 1, strTmp);
		   }
		  }
      }  // end MENU_TIME

      // переключение тумблера ПУСК / СТОП / Календарь
      if((mode_Menu & BITS_MENU) == MENU_PICTURE){

    		uint16_t backColor;
    		if((mode_Menu & 0x0F) == 2) backColor = RGB565(100,100,255);
    		else backColor = RGB565(200,200,255);

    		  if((mode_Menu & BIT_MODIFY_REG) && ((mode_Menu & 0x0F) == 2))
    			  ST7789_print_u(25, 187, colorRed, backColor, 1, &Font_11x18, 1, menu.strMenu[14][MODBUS_REG(0x3F)]);
    		  else
    			  ST7789_print_u(25, 187, RGB565(10, 10, 10), backColor, 1, &Font_11x18, 1, menu.strMenu[14][MODBUS_REG(0x3F)]);

      }

   }   // end BIT_UPDATE_REG
   //------------------------------------------------------------
   // end BIT_UPDATE_REG
   //------------------------------------------------------------

   //////////////////////////////////////////////////////////////
    // BIT_CHANGE_VALUE  - перерисовываем изменение цифровых сигналов или  сигналов выбора
    //////////////////////////////////////////////////////////////
    if(mode_Menu & BIT_CHANGE_VALUE){
    	mode_Menu &= ~BIT_CHANGE_VALUE;
    	// текущая позиция
    	uint8_t i= ((mode_Menu & BITS_SUB_MENU)-1) % 5;

    	// перекинуть состояние цифрового выхода
 		if((mode_Menu & BITS_MENU) == MENU_DIGITAL_OUT){
 		  // перерисовать "ON"/"OFF"
		  // i - рисуем
		   if(((MODBUS_REG(REG_DO_HND) >> (i + 5*numScreen)) & 0x01) == 1)
				      ST7789_print_u(SHIFT_PRINT_REGS, SHIFT_Y_PRINT_SUBMENU+i*STEP_Y_PRINT_SUBMENU, colorGreen, colorBlue, 1, &Font_16x26, 1, "ON   ");
		   else{
					  ST7789_print_u(SHIFT_PRINT_REGS, SHIFT_Y_PRINT_SUBMENU+i*STEP_Y_PRINT_SUBMENU, colorRed, colorBlue, 1, &Font_16x26, 1, "OFF  ");
			   }
 		  }  //end MENU_DIGITAL_OUT

 		// SAVE_SP
    	// записать во FLASH MODBUS_REG 0x20 - 0x2E - SETPOINTS
 		//                   MODBUS_REG 0x48 - UNIT_MODE
 		//                   MODBUS_REG 0x49 - UNIT_TYPE
 		if((mode_Menu & BITS_MENU) == MENU_SAVE_SP){

 		char str[20];
 		uint32_t startAddr = findStartAddrToWrite(START_ADDR_LAST_PAGE, END_ADDR_LAST_PAGE, 4, 21);
// 		sprintf(str, "%08x", startAddr);
// 		ST7789_print_u(5, 45+0*30, colorBlue, colorWhite, 1, &Font_16x26, 1, str);
 			HAL_StatusTypeDef result = HAL_FLASH_Unlock();
 			if(result == HAL_OK){
 				if(startAddr == END_ADDR_LAST_PAGE){
 					// стираем страницу и выставляем для записи начальный адрес страницы
 					static FLASH_EraseInitTypeDef EraseInitStruct;
 					uint32_t PageError;
 					uint8_t sofar = 0;
 					EraseInitStruct.TypeErase = FLASH_TYPEERASE_PAGES;
 					EraseInitStruct.PageAddress = START_ADDR_LAST_PAGE;
 					EraseInitStruct.NbPages = 1; //((END_ADDR_LAST_PAGE - START_ADDR_LAST_PAGE)/FLASH_PAGE_SIZE)+1;
 					if(HAL_FLASHEx_Erase(&EraseInitStruct, &PageError) != HAL_OK){

 						return HAL_FLASH_GetError();
 					}
 					startAddr = START_ADDR_LAST_PAGE;

 				}
 				for(uint8_t i=0; i < 15;i++ ){
 				      HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, startAddr + i*4, (uint32_t)MODBUS_REG(0x20 + i));  // Set Point
 				}
			      HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, startAddr + 15*4, (uint32_t)MODBUS_REG(0x48));         // Mode
				  HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, startAddr + 16*4, (uint32_t)MODBUS_REG(0x49));         // Type HVAC

	 				for(uint8_t i=0; i < 4;i++ ){
	 				      HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, startAddr + (17+i)*4, (uint32_t)MODBUS_REG(0x40 + i));  // Time Start/Stop/Day/Night
	 				}

				      HAL_FLASH_Lock();
				      ST7789_print_u(SHIFT_X_PRINT_SUBMENU, SHIFT_Y_PRINT_SUBMENU+0*STEP_Y_PRINT_SUBMENU, colorBlue, colorWhite, 1, &Font_16x26, 1, menu.strMenu[15][1]); // Сохранено !!!

 			}
 			else if(result == HAL_ERROR){
			      ST7789_print_u(5, 45+0*30, colorBlack, colorWhite, 1, &Font_16x26, 1, "   ERROR");
 			}

 			else if(result == HAL_BUSY){
			      ST7789_print_u(5, 45+0*30, colorBlue, colorWhite, 1, &Font_16x26, 1, "   BUSY");
 			}

 			else if(result == HAL_TIMEOUT){
			      ST7789_print_u(5, 45+0*30, colorBlue, colorWhite, 1, &Font_16x26, 1, "   TIMEOUT");
 			}

 		}  // end SAVE_SP

 		else if((mode_Menu & BITS_MENU) == MENU_UNIT_MODE){
// 		   uint16_t temp;
 		   // находим и стираем предыдущее значение
 		  for(uint8_t k=0; k < menu.pointsOfMenu[MENU_UNIT_MODE >> 4];k++){
		       ST7789_DrawImage_u(SHIFT_PRINT_REGS, SHIFT_Y_PRINT_SUBMENU+k*STEP_Y_PRINT_SUBMENU, 26, 26, circle_26x26);
 		  }
		   // находим новое значение схраняем его и рисуем его
//		   temp = (mode_Menu & BITS_SUB_MENU) - 1;

		   ST7789_DrawImage_u(SHIFT_PRINT_REGS, SHIFT_Y_PRINT_SUBMENU+i*STEP_Y_PRINT_SUBMENU, 26, 26, circle_check_cursor_26x26);
 		}  //end MENU_UNIT_MODE

 		else if((mode_Menu & BITS_MENU) == MENU_UNIT_TYPE){
// 		   uint16_t temp;
  		   // находим и стираем предыдущее значение
  		  for(uint8_t k=0; k < 5/*menu.pointsOfMenu[MENU_UNIT_TYPE >> 4]*/;k++){
 		       ST7789_DrawImage_u(SHIFT_PRINT_REGS, SHIFT_Y_PRINT_SUBMENU+k*STEP_Y_PRINT_SUBMENU, 26, 26, circle_26x26);
  		  }
 		   // находим новое значение схраняем его и рисуем его
// 		   temp = (mode_Menu & BITS_SUB_MENU) - 1;

 		   ST7789_DrawImage_u(SHIFT_PRINT_REGS, SHIFT_Y_PRINT_SUBMENU+i*STEP_Y_PRINT_SUBMENU, 26, 26, circle_check_cursor_26x26);

 		} //end MENU_UNIT_TYPE



    }   // end BIT_CHANGE_VALUE
    //-----------------------------------------------------------
    // end BIT_CHANGE_VALUE
    //------------------------------------------------------------

   //////////////////////////////////////////////////////////////
   // BIT_UPDATE_VALUE  - перерисовываем значения считываемых из вне
   //////////////////////////////////////////////////////////////
   if(mode_Menu & BIT_UPDATE_VALUE){
	 //  перерисовка входных значений
	   mode_Menu &= ~BIT_UPDATE_VALUE;
		  // обновить данные

	      if((mode_Menu & 0xf0) == 0){
	    	// обновление данных на мнемосхеме
		    if(MODBUS_REG(REG_UNIT_TYPE) == HVAC1)
		       DrawData1(0, 0);

	    	if(MODBUS_REG(REG_UNIT_TYPE) == HVAC2)
	    	   DrawData2(0, 0);

	    	if(MODBUS_REG(REG_UNIT_TYPE) == HVAC3)
	    	   DrawData3(0, 0);

	    	if(MODBUS_REG(REG_UNIT_TYPE) == HVAC4)
	    	   DrawData4(0, 0);

	    	if(MODBUS_REG(REG_UNIT_TYPE) == HVAC5)
	    	   DrawData5(0, 0);
	      }
		  if((mode_Menu & BITS_MENU) == MENU_SENSORS){
			  // вывести показания датчиков
			  for(char i=0;i<5;i++){
				  //if(MODBUS_REG(i+numScreen*5) == -1001) //if(USART2_Modbus.Data[i+numScreen*5] == -100)
				  if(!READ_COIL(REG_POINT_MODBUS_OK * 0x10 + i+numScreen*5))
				  ST7789_print_u(100 /*110*/, 45+i*30, colorBlue, colorWhite, 1, &Font_16x26, 1, "--- ");
				  else{
					  //sprintf(strTmp,"%3i'", USART2_Modbus.Data[i+numScreen*5]);

					  sprintf(strTmp,"%3i %s", MODBUS_REG(i+numScreen*5),menu.strUnitsOfMeasure[menu.uOfMeasure[i+numScreen*5]]);
					  ST7789_print_u(100 /*110*/, 45+i*30, colorBlue, colorWhite, 1, &Font_16x26, 1, strTmp);
				  }

			  }
		  }  // end MENU_SENSORS

		  if((mode_Menu & BITS_MENU) == MENU_STATUS){
			  // вывести показания статусов (цифровых сигналов)
			  uint16_t reg_modbus;
			  uint16_t shift_bits;                  // смещение для битов которое нужно выводить shift_bits = 0  для DI1 ... DI5  и DO1 ... DO5; shift_bits = 1  для DO6 ... DO10

			  if(numScreen < 2) shift_bits = 0;     // shift_bits = 0  для DI1 ... DI5  и DO1 ... DO5;
			  else shift_bits = 1;                  // shift_bits = 1  для DO6 ... DO10

			  if(numScreen == 0) reg_modbus = REG_DI; // регистр цифровых входов  DI1 ... DI5
			  else reg_modbus = REG_DO;               // регистр цифровых сигналов управленияв DO1 ... DO10

			  for(char i=0;i<5;i++){
					  if((MODBUS_REG(reg_modbus) >> i+/*numScreen*/shift_bits * 5) & 0x01)
					  ST7789_print_u(150, 45+i*30, colorGreen, colorWhite, 1, &Font_16x26, 1, "ON ");
					  else{
						  ST7789_print_u(150, 45+i*30, colorRed, colorWhite, 1, &Font_16x26, 1, "OFF");
					  }

			  }
		  } // end MENU_STATUS

		  if((mode_Menu & BITS_MENU) == MENU_ANALOG_IN){

			  uint16_t reg_modbus;
			  uint16_t shift_bits;                  // смещение для регистров которое нужно выводить shift_bits = 0  для AI1 ... AI5  и DO1 ... DO5; shift_bits = 1  для AO6 ... AO10

			  if(numScreen < 2) shift_bits = 0;     // shift_bits = 0  для AI1 ... AI5  и AO1 ... AO5;
			  else shift_bits = 1;                  // shift_bits = 1  для AO6 ... AO10

			  if(numScreen == 0) reg_modbus = START_REG_AI;        // регистр аналоговых входов  AI1 ... AI5
			  else reg_modbus = START_REG_AO;                       // регистр аналоговых сигналов управленияв AO1 ... AO10

			  // вывести показания аналоговых сигналов
			  for(char i=0;i<5;i++){
	                       sprintf(strTmp,"%*i %%",4, MODBUS_REG(reg_modbus + i+shift_bits*5));
						   ST7789_print_u(110, 45+i*30, colorBlue, colorWhite, 1, &Font_16x26, 1, strTmp);
			  }
		  } // end MENU_ANALOG_IN

		  // MENU_ALARM
		  if((mode_Menu & 0x000000F0) == MENU_ALARM){
			  uint16_t alarm = MODBUS_REG(REG_ALARM);
			  for(uint8_t i=0; i < 5;i++){

			    if((alarm >> (i + 5* numScreen)) & 0x01 == 1){
			    	ST7789_DrawImage_u(SHIFT_PRINT_REGS, SHIFT_Y_PRINT_SUBMENU+i*STEP_Y_PRINT_SUBMENU, 26, 26, alarm_26x26);
			    }
			    else{
			    	ST7789_print_u(SHIFT_PRINT_REGS, SHIFT_Y_PRINT_SUBMENU+i*STEP_Y_PRINT_SUBMENU, colorWhite, colorWhite, 1, &Font_16x26, 1, "  ");
			    }
			  }

	       //   all_regs_printed = mode_Menu & BITS_MENU;

		  } //end MENU_ALARM

   }  // end BIT_UPDATE_VALUE

   //------------------------------------------------------------
   // end BIT_UPDATE_VALUE
   //------------------------------------------------------------

   mode_Menu_old = mode_Menu;

#endif

   // Test
//   GPIOB->BSRR = (uint32_t)GPIO_PIN_15 << 16U;
//   MODBUS_REG(0x14) = TIM6->CNT;


    osDelay(10);
  }      //   конец бесонечного цикла

  /* USER CODE END Start_ST7789_Task */
}

/* USER CODE BEGIN Header_Start_EC11 */
/**
* @brief Function implementing the EC11_Task thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_Start_EC11 */
void Start_EC11(void const * argument)
{
  /* USER CODE BEGIN Start_EC11 */
	   EC11_Init();
	   ec11.timePush =2;
	   vTaskSetApplicationTaskTag(NULL,(void*)2);
  /* Infinite loop */
  for(;;)
  {
	//  __NOP();
      // устанавливаем бит runEC11 задача работает
      uRunningTask |= runEC11;

		ec11 = EC11_Read();

		// Кнопка нажата
		if(ec11.Btn) {
			OnButtonEC11();
			g_mode_Menu |= BIT_BUTTON_EC11;
		}

		// была ли прокрутка
		if((ec11.count - ec11.prev_count) != 0) {
			OnCountEC11();
			g_mode_Menu |= BIT_COUNT_EC11;

		}
    if((g_mode_Menu & BIT_BUTTON_EC11) || (g_mode_Menu & BIT_COUNT_EC11)){
    	// если произошло изменение валкодера ставим статус меню в очередь
    	portBASE_TYPE queueSendTFT = xQueueSendToFront( queueTFT, &g_mode_Menu, 0 );

    	g_mode_Menu &= ~(BIT_BUTTON_EC11 | BIT_COUNT_EC11);

    	if(queueSendTFT == pdPASS){
    	//	g_mode_Menu |= 0x80000000;
    		// если поставили в очередь сбросим все временные биты
    		g_mode_Menu &= ~BIT_CHANGE_VALUE;
    	}
    }
    /* Разрешение выполняться для других задач, отправляющих данные
            в очередь. Вызов taskYIELD() информирует шедулер, что сейчас сразу
            нужно переключиться к другой задаче, а не поддерживать эту задачу в
            состоянии Running до окончания текущего слайса времени. */
         taskYIELD();
    osDelay(50);
  }
  /* USER CODE END Start_EC11 */
}

/* USER CODE BEGIN Header_StartUSART_2 */
/**
* @brief Function implementing the USART_2 thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartUSART_2 */
void StartUSART_2(void const * argument)
{
  /* USER CODE BEGIN StartUSART_2 */
//  uint8_t buf_RX[0x20]={0};
  uint8_t buf_TX[8] = {0x01, 0x03, 0x00, 0x00, 0x00, 0x01, 0x84, 0x0a};
  USART2_Modbus.state = sendFrame;
  vTaskSetApplicationTaskTag(NULL,(void*)3);
  static uint8_t adr = 0;
  static uint16_t reg = 0;
  static uint8_t func = 0;
  static uint16_t data = 0;
  static uint16_t ok = 0;
  static uint8_t len_TX = 0;
  static uint8_t len_RX = 0;
  static uint16_t def = 0;

  uint16_t crc16;

 // InitUSART2_Modbus();
  USART2_Modbus.state = idelFrame;

  HAL_TIM_Base_Start_IT(&htim6);
  TIM6->CR1 = 0x01;

   osDelay(2000);





  // инициализация точек опроса
  //  Init Point List
  //TODO - сделать чтение ModBus Points из Flash

  //// кол-во Points в данном алгоритме
  //uint16_t numPionts= (*((uint16_t*)(0x0803f000 + MODBUS_REG(REG_UNIT_TYPE)*256 + 2)));

  // modbusHVAC1.num   = (*((uint16_t*)(0x0803f000))) & 0xff;
  modbusHVAC1.num = (*((uint16_t*)(0x0803f000 + MODBUS_REG(REG_UNIT_TYPE)*256))) &  0xff;

//  MODBUS_REG(0x150)= (*((uint16_t*)(0x0803f000)));
//  MODBUS_REG(0x151)= (*((uint16_t*)(0x0803f002)));
//  MODBUS_REG(0x152)= (*((uint16_t*)(0x0803f004)));
//  MODBUS_REG(0x153)= (*((uint16_t*)(0x0803f006)));
//  MODBUS_REG(0x154)= (*((uint16_t*)(0x0803f008)));
//  MODBUS_REG(0x155)= (*((uint16_t*)(0x0803f00A)));
//  MODBUS_REG(0x156)= (*((uint16_t*)(0x0803f00C)));
//  MODBUS_REG(0x157)= (*((uint16_t*)(0x0803f00E)));
//  MODBUS_REG(0x158)= (*((uint16_t*)(0x0803f010)));


//  for(int i=0;i< modbusHVAC1.num; i++)
//  {
//
//	   modbusHVAC1.modbusPoint[i].addr = (uint8_t)((*((uint16_t*)(0x0803f000 + i*0x10 + 2))) >> 8);//0x82;
//	   modbusHVAC1.modbusPoint[i].func = (uint8_t)((*((uint16_t*)(0x0803f000 + i*0x10 + 2))) & 0xFF);//0x03;
//	   modbusHVAC1.modbusPoint[i].reg =  (*((uint16_t*)(0x0803f000 + i*0x10 + 4)));//0x0000;
//	   modbusHVAC1.modbusPoint[i].data = (*((uint16_t*)(0x0803f000 + i*0x10 + 6)));//START_REG_MODBUS;
//	   modbusHVAC1.modbusPoint[i].ok =   (*((uint16_t*)(0x0803f000 + i*0x10 + 8)));//REG_POINT_MODBUS_OK * 0x10 + 0;       // куда будем складывать 1 - ответил
//	   modbusHVAC1.modbusPoint[i].period = (uint8_t)((*((uint16_t*)(0x0803f000 + i*0x10 + 0x0A))) >> 8);// 1;    // каждый раз
//	   modbusHVAC1.modbusPoint[i].timeout = (uint8_t)((*((uint16_t*)(0x0803f000 + i*0x10 + 0x0A))) & 0xFF); //50;  // 50 ms
//	   modbusHVAC1.modbusPoint[i].def =  (*((uint16_t*)(0x0803f000 + i*0x10 + 0x0C)));//-1000;
//	   modbusHVAC1.modbusPoint[i].len_TX =  (uint8_t)((*((uint16_t*)(0x0803f000 + i*0x10 + 0x0E))) >> 8);//8;
//	   modbusHVAC1.modbusPoint[i].len_RX =  (uint8_t)((*((uint16_t*)(0x0803f000 + i*0x10 + 0x0E))) & 0xFF);//7;
//	   modbusHVAC1.modbusPoint[i].div = (uint8_t)((*((uint16_t*)(0x0803f000 + i*0x10 + 0x10))) & 0xff);//10;
//	   menu.uOfMeasure[i] = (uint8_t)((*((uint16_t*)(0x0803f000 + i*0x10 + 0x10))) >> 8);
//  }

    if((modbusHVAC1.num != 0) && (modbusHVAC1.num != 0xFF))
    {
    	// блок заполнен исходными данными
		for(int i=0;i< modbusHVAC1.num; i++)
		{
		   modbusHVAC1.modbusPoint[i].addr = (uint8_t)((*((uint16_t*)(0x0803f000 + i*0x10 + 2 + MODBUS_REG(REG_UNIT_TYPE)*256))) >> 8);//0x82;
		   modbusHVAC1.modbusPoint[i].func = (uint8_t)((*((uint16_t*)(0x0803f000 + i*0x10 + 2 + MODBUS_REG(REG_UNIT_TYPE)*256))) & 0xFF);//0x03;
		   modbusHVAC1.modbusPoint[i].reg =  (*((uint16_t*)(0x0803f000 + i*0x10 + 4 + MODBUS_REG(REG_UNIT_TYPE)*256)));//0x0000;
		   modbusHVAC1.modbusPoint[i].data = (*((uint16_t*)(0x0803f000 + i*0x10 + 6 + MODBUS_REG(REG_UNIT_TYPE)*256)));//START_REG_MODBUS;
		   modbusHVAC1.modbusPoint[i].ok =   (*((uint16_t*)(0x0803f000 + i*0x10 + 8 + MODBUS_REG(REG_UNIT_TYPE)*256)));//REG_POINT_MODBUS_OK * 0x10 + 0;       // куда будем складывать 1 - ответил
		   modbusHVAC1.modbusPoint[i].period = (uint8_t)((*((uint16_t*)(0x0803f000 + i*0x10 + 0x0A + MODBUS_REG(REG_UNIT_TYPE)*256))) >> 8);// 1;    // каждый раз
		   modbusHVAC1.modbusPoint[i].timeout = (uint8_t)((*((uint16_t*)(0x0803f000 + i*0x10 + 0x0A + MODBUS_REG(REG_UNIT_TYPE)*256))) & 0xFF); //50;  // 50 ms
		   modbusHVAC1.modbusPoint[i].def =  (*((uint16_t*)(0x0803f000 + i*0x10 + 0x0C + MODBUS_REG(REG_UNIT_TYPE)*256)));//-1000;
		   modbusHVAC1.modbusPoint[i].len_TX =  (uint8_t)((*((uint16_t*)(0x0803f000 + i*0x10 + 0x0E + MODBUS_REG(REG_UNIT_TYPE)*256))) >> 8);//8;
		   modbusHVAC1.modbusPoint[i].len_RX =  (uint8_t)((*((uint16_t*)(0x0803f000 + i*0x10 + 0x0E + MODBUS_REG(REG_UNIT_TYPE)*256))) & 0xFF);//7;
		   modbusHVAC1.modbusPoint[i].div = (uint8_t)((*((uint16_t*)(0x0803f000 + i*0x10 + 0x10 + MODBUS_REG(REG_UNIT_TYPE)*256))) & 0xff);//10;
		   modbusHVAC1.modbusPoint[i].no_answer = 0;
		   menu.uOfMeasure[i] =             (uint8_t)((*((uint16_t*)(0x0803f000 + i*0x10 + 0x10 + MODBUS_REG(REG_UNIT_TYPE)*256))) >> 8);
		}
    }

	else
	{
		// блок не был заполнен. Заполняем данными по умолчанию 4 точки
		modbusHVAC1.num = 4;
		for(int i=0;i< modbusHVAC1.num; i++)
		{
		   modbusHVAC1.modbusPoint[i].addr = 1+i;//0x82;
		   modbusHVAC1.modbusPoint[i].func = 0x03;
		   modbusHVAC1.modbusPoint[i].reg =  0;//0x0000;
		   modbusHVAC1.modbusPoint[i].data = START_REG_MODBUS + i;
		   modbusHVAC1.modbusPoint[i].ok =   REG_POINT_MODBUS_OK * 0x10 + i;       // куда будем складывать 1 - ответил
		   modbusHVAC1.modbusPoint[i].period =  1;   // каждый раз
		   modbusHVAC1.modbusPoint[i].timeout = 50;  // 50 ms
		   modbusHVAC1.modbusPoint[i].def =  -1000;
		   modbusHVAC1.modbusPoint[i].len_TX =  8;
		   modbusHVAC1.modbusPoint[i].len_RX = 7;
		   modbusHVAC1.modbusPoint[i].div = 10;
		   modbusHVAC1.modbusPoint[i].no_answer = 0;  // количество неответов подряд от данной точки
		   menu.uOfMeasure[i] =             1;       // *C
		   CLR_COIL(modbusHVAC1.modbusPoint[i].ok);  //   обозначаем все датчики как неответившие COIL(ok) = 0
		}
	}

    // проверка на незаполненость
//    START_REG_MODBUS;
//    REG_POINT_MODBUS_OK;
/*
  switch MODBUS_REG(REG_UNIT_TYPE)
//  if(MODBUS_REG(REG_UNIT_TYPE)  == HVAC1)
  {
  case HVAC1:
   modbusHVAC1.num = 5;
   modbusHVAC1.modbusPoint[0].addr = 0x82;
   modbusHVAC1.modbusPoint[0].func = 0x03;
   modbusHVAC1.modbusPoint[0].reg =  0x0000;
   modbusHVAC1.modbusPoint[0].data = START_REG_MODBUS;
   modbusHVAC1.modbusPoint[0].ok =   REG_POINT_MODBUS_OK * 0x10 + 0;       // куда будем складывать 1 - ответил
   modbusHVAC1.modbusPoint[0].period =  1;    // каждый раз
   modbusHVAC1.modbusPoint[0].timeout =  50;  // 50 ms
   modbusHVAC1.modbusPoint[0].def =  -1000;
   modbusHVAC1.modbusPoint[0].len_TX =  8;
   modbusHVAC1.modbusPoint[0].len_RX =  7;
   modbusHVAC1.modbusPoint[0].div = 10;

  modbusHVAC1.modbusPoint[1].addr = 0x82;
  modbusHVAC1.modbusPoint[1].func = 0x03;
  modbusHVAC1.modbusPoint[1].reg =  0x0001;
  modbusHVAC1.modbusPoint[1].data = START_REG_MODBUS + 0x01;
  modbusHVAC1.modbusPoint[1].ok =   REG_POINT_MODBUS_OK * 0x10 + 1;       // куда будем складывать 1 - ответил
  modbusHVAC1.modbusPoint[1].period =  2;    // каждый раз
  modbusHVAC1.modbusPoint[1].timeout =  50;  // 50 ms
  modbusHVAC1.modbusPoint[1].def =  -1000;
  modbusHVAC1.modbusPoint[1].len_TX =  8;
  modbusHVAC1.modbusPoint[1].len_RX =  7;
  modbusHVAC1.modbusPoint[1].div = 10;

  modbusHVAC1.modbusPoint[2].addr = 0x82;
  modbusHVAC1.modbusPoint[2].func = 0x03;
  modbusHVAC1.modbusPoint[2].reg =  0x0002;
  modbusHVAC1.modbusPoint[2].data = START_REG_MODBUS + 0x02;
  modbusHVAC1.modbusPoint[2].ok =   REG_POINT_MODBUS_OK * 0x10 + 2;       // куда будем складывать 1 - ответил
  modbusHVAC1.modbusPoint[2].period =  2;    // каждый раз
  modbusHVAC1.modbusPoint[2].timeout =  50;  // 50 ms
  modbusHVAC1.modbusPoint[2].def =  -1000;
  modbusHVAC1.modbusPoint[2].len_TX =  8;
  modbusHVAC1.modbusPoint[2].len_RX =  7;
  modbusHVAC1.modbusPoint[2].div = 10;

  modbusHVAC1.modbusPoint[3].addr = 0x82;
  modbusHVAC1.modbusPoint[3].func = 0x03;
  modbusHVAC1.modbusPoint[3].reg =  0x0003;
  modbusHVAC1.modbusPoint[3].data = START_REG_MODBUS + 0x03;
  modbusHVAC1.modbusPoint[3].ok =   REG_POINT_MODBUS_OK * 0x10 + 3;       // куда будем складывать 1 - ответил
  modbusHVAC1.modbusPoint[3].period =  2;    // каждый раз
  modbusHVAC1.modbusPoint[3].timeout =  50;  // 50 ms
  modbusHVAC1.modbusPoint[3].def =  -1000;
  modbusHVAC1.modbusPoint[3].len_TX =  8;
  modbusHVAC1.modbusPoint[3].len_RX =  7;
  modbusHVAC1.modbusPoint[3].div = 10;

  modbusHVAC1.modbusPoint[4].addr = 0x82;
  modbusHVAC1.modbusPoint[4].func = 0x03;
  modbusHVAC1.modbusPoint[4].reg =  0x0004;
  modbusHVAC1.modbusPoint[4].data = START_REG_MODBUS + 0x04;
  modbusHVAC1.modbusPoint[4].ok =   REG_POINT_MODBUS_OK * 0x10 + 4;       // куда будем складывать 1 - ответил
  modbusHVAC1.modbusPoint[4].period =  2;    // каждый раз
  modbusHVAC1.modbusPoint[4].timeout =  50;  // 50 ms
  modbusHVAC1.modbusPoint[4].def =  -1000;
  modbusHVAC1.modbusPoint[4].len_TX =  8;
  modbusHVAC1.modbusPoint[4].len_RX =  7;
  modbusHVAC1.modbusPoint[4].div = 10;

  break;

  case HVAC2:
	   modbusHVAC1.num = 5;
	   modbusHVAC1.modbusPoint[0].addr = 0x82;
	   modbusHVAC1.modbusPoint[0].func = 0x03;
	   modbusHVAC1.modbusPoint[0].reg =  0x0000;
	   modbusHVAC1.modbusPoint[0].data = START_REG_MODBUS;
	   modbusHVAC1.modbusPoint[0].ok =   REG_POINT_MODBUS_OK * 0x10 + 0;       // куда будем складывать 1 - ответил
	   modbusHVAC1.modbusPoint[0].period =  1;    // каждый раз
	   modbusHVAC1.modbusPoint[0].timeout =  50;  // 50 ms
	   modbusHVAC1.modbusPoint[0].def =  -1000;
	   modbusHVAC1.modbusPoint[0].len_TX =  8;
	   modbusHVAC1.modbusPoint[0].len_RX =  7;
	   modbusHVAC1.modbusPoint[0].div = 10;

	  modbusHVAC1.modbusPoint[1].addr = 0x82;
	  modbusHVAC1.modbusPoint[1].func = 0x03;
	  modbusHVAC1.modbusPoint[1].reg =  0x0001;
	  modbusHVAC1.modbusPoint[1].data = START_REG_MODBUS + 0x01;
	  modbusHVAC1.modbusPoint[1].ok =   REG_POINT_MODBUS_OK * 0x10 + 1;       // куда будем складывать 1 - ответил
	  modbusHVAC1.modbusPoint[1].period =  2;    // каждый раз
	  modbusHVAC1.modbusPoint[1].timeout =  50;  // 50 ms
	  modbusHVAC1.modbusPoint[1].def =  -1000;
	  modbusHVAC1.modbusPoint[1].len_TX =  8;
	  modbusHVAC1.modbusPoint[1].len_RX =  7;
	  modbusHVAC1.modbusPoint[1].div = 10;

	  modbusHVAC1.modbusPoint[2].addr = 0x82;
	  modbusHVAC1.modbusPoint[2].func = 0x03;
	  modbusHVAC1.modbusPoint[2].reg =  0x0002;
	  modbusHVAC1.modbusPoint[2].data = START_REG_MODBUS + 0x02;
	  modbusHVAC1.modbusPoint[2].ok =   REG_POINT_MODBUS_OK * 0x10 + 2;       // куда будем складывать 1 - ответил
	  modbusHVAC1.modbusPoint[2].period =  2;    // каждый раз
	  modbusHVAC1.modbusPoint[2].timeout =  50;  // 50 ms
	  modbusHVAC1.modbusPoint[2].def =  -1000;
	  modbusHVAC1.modbusPoint[2].len_TX =  8;
	  modbusHVAC1.modbusPoint[2].len_RX =  7;
	  modbusHVAC1.modbusPoint[2].div = 10;

	  modbusHVAC1.modbusPoint[3].addr = 0x82;
	  modbusHVAC1.modbusPoint[3].func = 0x03;
	  modbusHVAC1.modbusPoint[3].reg =  0x0003;
	  modbusHVAC1.modbusPoint[3].data = START_REG_MODBUS + 0x03;
	  modbusHVAC1.modbusPoint[3].ok =   REG_POINT_MODBUS_OK * 0x10 + 3;       // куда будем складывать 1 - ответил
	  modbusHVAC1.modbusPoint[3].period =  2;    // каждый раз
	  modbusHVAC1.modbusPoint[3].timeout =  50;  // 50 ms
	  modbusHVAC1.modbusPoint[3].def =  -1000;
	  modbusHVAC1.modbusPoint[3].len_TX =  8;
	  modbusHVAC1.modbusPoint[3].len_RX =  7;
	  modbusHVAC1.modbusPoint[3].div = 10;

	  modbusHVAC1.modbusPoint[4].addr = 0x82;
	  modbusHVAC1.modbusPoint[4].func = 0x03;
	  modbusHVAC1.modbusPoint[4].reg =  0x0004;
	  modbusHVAC1.modbusPoint[4].data = START_REG_MODBUS + 0x04;
	  modbusHVAC1.modbusPoint[4].ok =   REG_POINT_MODBUS_OK * 0x10 + 4;       // куда будем складывать 1 - ответил
	  modbusHVAC1.modbusPoint[4].period =  2;    // каждый раз
	  modbusHVAC1.modbusPoint[4].timeout =  50;  // 50 ms
	  modbusHVAC1.modbusPoint[4].def =  -1000;
	  modbusHVAC1.modbusPoint[4].len_TX =  8;
	  modbusHVAC1.modbusPoint[4].len_RX =  7;
	  modbusHVAC1.modbusPoint[4].div = 10;

  break;

  case HVAC3:
	   modbusHVAC1.num = 5;
	   modbusHVAC1.modbusPoint[0].addr = 0x82;
	   modbusHVAC1.modbusPoint[0].func = 0x03;
	   modbusHVAC1.modbusPoint[0].reg =  0x0000;
	   modbusHVAC1.modbusPoint[0].data = START_REG_MODBUS;
	   modbusHVAC1.modbusPoint[0].ok =   REG_POINT_MODBUS_OK * 0x10 + 0;       // куда будем складывать 1 - ответил
	   modbusHVAC1.modbusPoint[0].period =  1;    // каждый раз
	   modbusHVAC1.modbusPoint[0].timeout =  50;  // 50 ms
	   modbusHVAC1.modbusPoint[0].def =  -1000;
	   modbusHVAC1.modbusPoint[0].len_TX =  8;
	   modbusHVAC1.modbusPoint[0].len_RX =  7;
	   modbusHVAC1.modbusPoint[0].div = 10;

	  modbusHVAC1.modbusPoint[1].addr = 0x82;
	  modbusHVAC1.modbusPoint[1].func = 0x03;
	  modbusHVAC1.modbusPoint[1].reg =  0x0001;
	  modbusHVAC1.modbusPoint[1].data = START_REG_MODBUS + 0x01;
	  modbusHVAC1.modbusPoint[1].ok =   REG_POINT_MODBUS_OK * 0x10 + 1;       // куда будем складывать 1 - ответил
	  modbusHVAC1.modbusPoint[1].period =  2;    // каждый раз
	  modbusHVAC1.modbusPoint[1].timeout =  50;  // 50 ms
	  modbusHVAC1.modbusPoint[1].def =  -1000;
	  modbusHVAC1.modbusPoint[1].len_TX =  8;
	  modbusHVAC1.modbusPoint[1].len_RX =  7;
	  modbusHVAC1.modbusPoint[1].div = 10;

	  modbusHVAC1.modbusPoint[2].addr = 0x82;
	  modbusHVAC1.modbusPoint[2].func = 0x03;
	  modbusHVAC1.modbusPoint[2].reg =  0x0002;
	  modbusHVAC1.modbusPoint[2].data = START_REG_MODBUS + 0x02;
	  modbusHVAC1.modbusPoint[2].ok =   REG_POINT_MODBUS_OK * 0x10 + 2;       // куда будем складывать 1 - ответил
	  modbusHVAC1.modbusPoint[2].period =  2;    // каждый раз
	  modbusHVAC1.modbusPoint[2].timeout =  50;  // 50 ms
	  modbusHVAC1.modbusPoint[2].def =  -1000;
	  modbusHVAC1.modbusPoint[2].len_TX =  8;
	  modbusHVAC1.modbusPoint[2].len_RX =  7;
	  modbusHVAC1.modbusPoint[2].div = 10;

	  modbusHVAC1.modbusPoint[3].addr = 0x05;
	  modbusHVAC1.modbusPoint[3].func = 0x03;
	  modbusHVAC1.modbusPoint[3].reg =  0x0000;
	  modbusHVAC1.modbusPoint[3].data = START_REG_MODBUS + 0x03;
	  modbusHVAC1.modbusPoint[3].ok =   REG_POINT_MODBUS_OK * 0x10 + 3;       // куда будем складывать 1 - ответил
	  modbusHVAC1.modbusPoint[3].period =  2;    // каждый раз
	  modbusHVAC1.modbusPoint[3].timeout =  50;  // 50 ms
	  modbusHVAC1.modbusPoint[3].def =  -1000;
	  modbusHVAC1.modbusPoint[3].len_TX =  8;
	  modbusHVAC1.modbusPoint[3].len_RX =  7;
	  modbusHVAC1.modbusPoint[3].div = 1;

	  modbusHVAC1.modbusPoint[4].addr = 0x82;
	  modbusHVAC1.modbusPoint[4].func = 0x03;
	  modbusHVAC1.modbusPoint[4].reg =  0x0004;
	  modbusHVAC1.modbusPoint[4].data = START_REG_MODBUS + 0x04;
	  modbusHVAC1.modbusPoint[4].ok =   REG_POINT_MODBUS_OK * 0x10 + 4;       // куда будем складывать 1 - ответил
	  modbusHVAC1.modbusPoint[4].period =  2;    // каждый раз
	  modbusHVAC1.modbusPoint[4].timeout =  50;  // 50 ms
	  modbusHVAC1.modbusPoint[4].def =  -1000;
	  modbusHVAC1.modbusPoint[4].len_TX =  8;
	  modbusHVAC1.modbusPoint[4].len_RX =  7;
	  modbusHVAC1.modbusPoint[4].div = 10;

  break;

  case HVAC4:
	   modbusHVAC1.num = 5;
	   modbusHVAC1.modbusPoint[0].addr = 0x82;
	   modbusHVAC1.modbusPoint[0].func = 0x03;
	   modbusHVAC1.modbusPoint[0].reg =  0x0000;
	   modbusHVAC1.modbusPoint[0].data = START_REG_MODBUS;
	   modbusHVAC1.modbusPoint[0].ok =   REG_POINT_MODBUS_OK * 0x10 + 0;       // куда будем складывать 1 - ответил
	   modbusHVAC1.modbusPoint[0].period =  1;    // каждый раз
	   modbusHVAC1.modbusPoint[0].timeout =  50;  // 50 ms
	   modbusHVAC1.modbusPoint[0].def =  -1000;
	   modbusHVAC1.modbusPoint[0].len_TX =  8;
	   modbusHVAC1.modbusPoint[0].len_RX =  7;
	   modbusHVAC1.modbusPoint[0].div = 10;

	  modbusHVAC1.modbusPoint[1].addr = 0x82;
	  modbusHVAC1.modbusPoint[1].func = 0x03;
	  modbusHVAC1.modbusPoint[1].reg =  0x0001;
	  modbusHVAC1.modbusPoint[1].data = START_REG_MODBUS + 0x01;
	  modbusHVAC1.modbusPoint[1].ok =   REG_POINT_MODBUS_OK * 0x10 + 1;       // куда будем складывать 1 - ответил
	  modbusHVAC1.modbusPoint[1].period =  2;    // каждый раз
	  modbusHVAC1.modbusPoint[1].timeout =  50;  // 50 ms
	  modbusHVAC1.modbusPoint[1].def =  -1000;
	  modbusHVAC1.modbusPoint[1].len_TX =  8;
	  modbusHVAC1.modbusPoint[1].len_RX =  7;
	  modbusHVAC1.modbusPoint[1].div = 10;

	  modbusHVAC1.modbusPoint[2].addr = 0x82;
	  modbusHVAC1.modbusPoint[2].func = 0x03;
	  modbusHVAC1.modbusPoint[2].reg =  0x0002;
	  modbusHVAC1.modbusPoint[2].data = START_REG_MODBUS + 0x02;
	  modbusHVAC1.modbusPoint[2].ok =   REG_POINT_MODBUS_OK * 0x10 + 2;       // куда будем складывать 1 - ответил
	  modbusHVAC1.modbusPoint[2].period =  2;    // каждый раз
	  modbusHVAC1.modbusPoint[2].timeout =  50;  // 50 ms
	  modbusHVAC1.modbusPoint[2].def =  -1000;
	  modbusHVAC1.modbusPoint[2].len_TX =  8;
	  modbusHVAC1.modbusPoint[2].len_RX =  7;
	  modbusHVAC1.modbusPoint[2].div = 10;

	  modbusHVAC1.modbusPoint[3].addr = 0x05;
	  modbusHVAC1.modbusPoint[3].func = 0x03;
	  modbusHVAC1.modbusPoint[3].reg =  0x0003;
	  modbusHVAC1.modbusPoint[3].data = START_REG_MODBUS + 0x03;
	  modbusHVAC1.modbusPoint[3].ok =   REG_POINT_MODBUS_OK * 0x10 + 3;       // куда будем складывать 1 - ответил
	  modbusHVAC1.modbusPoint[3].period =  2;    // каждый раз
	  modbusHVAC1.modbusPoint[3].timeout =  50;  // 50 ms
	  modbusHVAC1.modbusPoint[3].def =  -1000;
	  modbusHVAC1.modbusPoint[3].len_TX =  8;
	  modbusHVAC1.modbusPoint[3].len_RX =  7;
	  modbusHVAC1.modbusPoint[3].div = 1;

	  modbusHVAC1.modbusPoint[4].addr = 0x82;
	  modbusHVAC1.modbusPoint[4].func = 0x03;
	  modbusHVAC1.modbusPoint[4].reg =  0x0004;
	  modbusHVAC1.modbusPoint[4].data = START_REG_MODBUS + 0x04;
	  modbusHVAC1.modbusPoint[4].ok =   REG_POINT_MODBUS_OK * 0x10 + 4;       // куда будем складывать 1 - ответил
	  modbusHVAC1.modbusPoint[4].period =  2;    // каждый раз
	  modbusHVAC1.modbusPoint[4].timeout =  50;  // 50 ms
	  modbusHVAC1.modbusPoint[4].def =  -1000;
	  modbusHVAC1.modbusPoint[4].len_TX =  8;
	  modbusHVAC1.modbusPoint[4].len_RX =  7;
	  modbusHVAC1.modbusPoint[4].div = 10;

  break;

  case HVAC5:
	   modbusHVAC1.num = 5;
	   modbusHVAC1.modbusPoint[0].addr = 0x82;
	   modbusHVAC1.modbusPoint[0].func = 0x03;
	   modbusHVAC1.modbusPoint[0].reg =  0x0000;
	   modbusHVAC1.modbusPoint[0].data = START_REG_MODBUS;
	   modbusHVAC1.modbusPoint[0].ok =   REG_POINT_MODBUS_OK * 0x10 + 0;       // куда будем складывать 1 - ответил
	   modbusHVAC1.modbusPoint[0].period =  1;    // каждый раз
	   modbusHVAC1.modbusPoint[0].timeout =  50;  // 50 ms
	   modbusHVAC1.modbusPoint[0].def =  -1000;
	   modbusHVAC1.modbusPoint[0].len_TX =  8;
	   modbusHVAC1.modbusPoint[0].len_RX =  7;
	   modbusHVAC1.modbusPoint[0].div = 10;

	  modbusHVAC1.modbusPoint[1].addr = 0x82;
	  modbusHVAC1.modbusPoint[1].func = 0x03;
	  modbusHVAC1.modbusPoint[1].reg =  0x0001;
	  modbusHVAC1.modbusPoint[1].data = START_REG_MODBUS + 0x01;
	  modbusHVAC1.modbusPoint[1].ok =   REG_POINT_MODBUS_OK * 0x10 + 1;       // куда будем складывать 1 - ответил
	  modbusHVAC1.modbusPoint[1].period =  2;    // каждый раз
	  modbusHVAC1.modbusPoint[1].timeout =  50;  // 50 ms
	  modbusHVAC1.modbusPoint[1].def =  -1000;
	  modbusHVAC1.modbusPoint[1].len_TX =  8;
	  modbusHVAC1.modbusPoint[1].len_RX =  7;
	  modbusHVAC1.modbusPoint[1].div = 10;

	  modbusHVAC1.modbusPoint[2].addr = 0x82;
	  modbusHVAC1.modbusPoint[2].func = 0x03;
	  modbusHVAC1.modbusPoint[2].reg =  0x0002;
	  modbusHVAC1.modbusPoint[2].data = START_REG_MODBUS + 0x02;
	  modbusHVAC1.modbusPoint[2].ok =   REG_POINT_MODBUS_OK * 0x10 + 2;       // куда будем складывать 1 - ответил
	  modbusHVAC1.modbusPoint[2].period =  2;    // каждый раз
	  modbusHVAC1.modbusPoint[2].timeout =  50;  // 50 ms
	  modbusHVAC1.modbusPoint[2].def =  -1000;
	  modbusHVAC1.modbusPoint[2].len_TX =  8;
	  modbusHVAC1.modbusPoint[2].len_RX =  7;
	  modbusHVAC1.modbusPoint[2].div = 10;

	  modbusHVAC1.modbusPoint[3].addr = 0x82;
	  modbusHVAC1.modbusPoint[3].func = 0x03;
	  modbusHVAC1.modbusPoint[3].reg =  0x0003;
	  modbusHVAC1.modbusPoint[3].data = START_REG_MODBUS + 0x03;
	  modbusHVAC1.modbusPoint[3].ok =   REG_POINT_MODBUS_OK * 0x10 + 3;       // куда будем складывать 1 - ответил
	  modbusHVAC1.modbusPoint[3].period =  2;    // каждый раз
	  modbusHVAC1.modbusPoint[3].timeout =  50;  // 50 ms
	  modbusHVAC1.modbusPoint[3].def =  -1000;
	  modbusHVAC1.modbusPoint[3].len_TX =  8;
	  modbusHVAC1.modbusPoint[3].len_RX =  7;
	  modbusHVAC1.modbusPoint[3].div = 10;

	  modbusHVAC1.modbusPoint[4].addr = 0x82;
	  modbusHVAC1.modbusPoint[4].func = 0x03;
	  modbusHVAC1.modbusPoint[4].reg =  0x0004;
	  modbusHVAC1.modbusPoint[4].data = START_REG_MODBUS + 0x04;
	  modbusHVAC1.modbusPoint[4].ok =   REG_POINT_MODBUS_OK * 0x10 + 4;       // куда будем складывать 1 - ответил
	  modbusHVAC1.modbusPoint[4].period =  2;    // каждый раз
	  modbusHVAC1.modbusPoint[4].timeout =  50;  // 50 ms
	  modbusHVAC1.modbusPoint[4].def =  -1000;
	  modbusHVAC1.modbusPoint[4].len_TX =  8;
	  modbusHVAC1.modbusPoint[4].len_RX =  7;
	  modbusHVAC1.modbusPoint[4].div = 10;

  break;

  default:
	   modbusHVAC1.num = 9;
	   modbusHVAC1.modbusPoint[0].addr = 0x82;
	   modbusHVAC1.modbusPoint[0].func = 0x03;
	   modbusHVAC1.modbusPoint[0].reg =  0x0000;
	   modbusHVAC1.modbusPoint[0].data = START_REG_MODBUS;
	   modbusHVAC1.modbusPoint[0].ok =   REG_POINT_MODBUS_OK * 0x10 + 0;       // куда будем складывать 1 - ответил
	   modbusHVAC1.modbusPoint[0].period =  1;    // каждый раз
	   modbusHVAC1.modbusPoint[0].timeout =  50;  // 50 ms
	   modbusHVAC1.modbusPoint[0].def =  -1000;
	   modbusHVAC1.modbusPoint[0].len_TX =  8;
	   modbusHVAC1.modbusPoint[0].len_RX =  7;

	  modbusHVAC1.modbusPoint[1].addr = 0x82;
	  modbusHVAC1.modbusPoint[1].func = 0x03;
	  modbusHVAC1.modbusPoint[1].reg =  0x0001;
	  modbusHVAC1.modbusPoint[1].data = START_REG_MODBUS + 0x01;
	  modbusHVAC1.modbusPoint[1].ok =   REG_POINT_MODBUS_OK * 0x10 + 1;       // куда будем складывать 1 - ответил
	  modbusHVAC1.modbusPoint[1].period =  2;    // каждый раз
	  modbusHVAC1.modbusPoint[1].timeout =  50;  // 50 ms
	  modbusHVAC1.modbusPoint[1].def =  -1000;
	  modbusHVAC1.modbusPoint[1].len_TX =  8;
	  modbusHVAC1.modbusPoint[1].len_RX =  7;

	  modbusHVAC1.modbusPoint[2].addr = 0x82;
	  modbusHVAC1.modbusPoint[2].func = 0x03;
	  modbusHVAC1.modbusPoint[2].reg =  0x0002;
	  modbusHVAC1.modbusPoint[2].data = START_REG_MODBUS + 0x02;
	  modbusHVAC1.modbusPoint[2].ok =   REG_POINT_MODBUS_OK * 0x10 + 2;       // куда будем складывать 1 - ответил
	  modbusHVAC1.modbusPoint[2].period =  2;    // каждый раз
	  modbusHVAC1.modbusPoint[2].timeout =  50;  // 50 ms
	  modbusHVAC1.modbusPoint[2].def =  -1000;
	  modbusHVAC1.modbusPoint[2].len_TX =  8;
	  modbusHVAC1.modbusPoint[2].len_RX =  7;

	  modbusHVAC1.modbusPoint[3].addr = 0x82;
	  modbusHVAC1.modbusPoint[3].func = 0x03;
	  modbusHVAC1.modbusPoint[3].reg =  0x0003;
	  modbusHVAC1.modbusPoint[3].data = START_REG_MODBUS + 0x03;
	  modbusHVAC1.modbusPoint[3].ok =   REG_POINT_MODBUS_OK * 0x10 + 3;       // куда будем складывать 1 - ответил
	  modbusHVAC1.modbusPoint[3].period =  2;    // каждый раз
	  modbusHVAC1.modbusPoint[3].timeout =  50;  // 50 ms
	  modbusHVAC1.modbusPoint[3].def =  -1000;
	  modbusHVAC1.modbusPoint[3].len_TX =  8;
	  modbusHVAC1.modbusPoint[3].len_RX =  7;

	  modbusHVAC1.modbusPoint[4].addr = 0x82;
	  modbusHVAC1.modbusPoint[4].func = 0x03;
	  modbusHVAC1.modbusPoint[4].reg =  0x0004;
	  modbusHVAC1.modbusPoint[4].data = START_REG_MODBUS + 0x04;
	  modbusHVAC1.modbusPoint[4].ok =   REG_POINT_MODBUS_OK * 0x10 + 4;       // куда будем складывать 1 - ответил
	  modbusHVAC1.modbusPoint[4].period =  2;    // каждый раз
	  modbusHVAC1.modbusPoint[4].timeout =  50;  // 50 ms
	  modbusHVAC1.modbusPoint[4].def =  -1000;
	  modbusHVAC1.modbusPoint[4].len_TX =  8;
	  modbusHVAC1.modbusPoint[4].len_RX =  7;

	  modbusHVAC1.modbusPoint[5].addr = 0x82;
	  modbusHVAC1.modbusPoint[5].func = 0x06;
	  modbusHVAC1.modbusPoint[5].reg =  0x0008;
	  modbusHVAC1.modbusPoint[5].data = START_REG_AO_HND + 0x08;
	  modbusHVAC1.modbusPoint[5].ok =   REG_POINT_MODBUS_OK * 0x10 + 5;       // куда будем складывать 1 - ответил
	  modbusHVAC1.modbusPoint[5].period =  2;    // каждый раз
	  modbusHVAC1.modbusPoint[5].timeout =  50;  // 50 ms
	  modbusHVAC1.modbusPoint[5].def =  -1000;
	  modbusHVAC1.modbusPoint[5].len_TX =  8;
	  modbusHVAC1.modbusPoint[5].len_RX =  8;

	  modbusHVAC1.modbusPoint[6].addr = 0x05;
	  modbusHVAC1.modbusPoint[6].func = 0x05;
	  modbusHVAC1.modbusPoint[6].reg =  0x01e1;
	  modbusHVAC1.modbusPoint[6].data = REG_DO_HND * 0x10 + 0x05;
	  modbusHVAC1.modbusPoint[6].ok =   REG_POINT_MODBUS_OK * 0x10 + 6;       // куда будем складывать 1 - ответил
	  modbusHVAC1.modbusPoint[6].period =  5;    // каждый раз
	  modbusHVAC1.modbusPoint[6].timeout =  50;  // 50 ms
	  modbusHVAC1.modbusPoint[6].def =  -1000;
	  modbusHVAC1.modbusPoint[6].len_TX =  8;
	  modbusHVAC1.modbusPoint[6].len_RX =  8;

	  modbusHVAC1.modbusPoint[7].addr = 0x05;
	  modbusHVAC1.modbusPoint[7].func = 0x01;
	  modbusHVAC1.modbusPoint[7].reg =  0x01e0;
	  modbusHVAC1.modbusPoint[7].data = REG_DI * 0x10 + 0x04;
	  modbusHVAC1.modbusPoint[7].ok =   REG_POINT_MODBUS_OK * 0x10 + 7;       // куда будем складывать 1 - ответил
	  modbusHVAC1.modbusPoint[7].period =  0;    // каждый раз
	  modbusHVAC1.modbusPoint[7].timeout =  50;  // 50 ms
	  modbusHVAC1.modbusPoint[7].def =  -1000;
	  modbusHVAC1.modbusPoint[7].len_TX =  8;
	  modbusHVAC1.modbusPoint[7].len_RX =  6;

	  modbusHVAC1.modbusPoint[8].addr = 0x05;
	  modbusHVAC1.modbusPoint[8].func = 0x03;
	  modbusHVAC1.modbusPoint[8].reg =  0x0000;
	  modbusHVAC1.modbusPoint[8].data = START_REG_MODBUS + 0x08;
	  modbusHVAC1.modbusPoint[8].ok =   REG_POINT_MODBUS_OK * 0x10 + 8;       // куда будем складывать 1 - ответил
	  modbusHVAC1.modbusPoint[8].period =  1;    // каждый раз
	  modbusHVAC1.modbusPoint[8].timeout =  50;  // 50 ms
	  modbusHVAC1.modbusPoint[8].def =  -1000;
	  modbusHVAC1.modbusPoint[8].len_TX =  8;
	  modbusHVAC1.modbusPoint[8].len_RX =  7;

  break;

  //  end Init Point List
  }

  */
  /* Infinite loop */
  for(;;)
  {

      // устанавливаем бит runUSART задача работает
      uRunningTask |= runUSART;

	//====================================================================
	// USART1
	//====================================================================
//	if(USART1_Modbus.state == decodeFrame)
//	{
//		// перешли в режим декодирования
//		// формируем ответ
//
//		ModBusDecode(USART1_Modbus.rx_Buff, 8);
//
//		// перешли в режим отправки
//	    USART1_Modbus.state = sendFrame;
//
//	    uint16_t crc16 = CRC16(g_txBuf, 5);
//		// отправляем пакет
//	  	USART1_DE_TX;
//
//	  	g_txBuf[5] = crc16 >> 8;
//	  	g_txBuf[6] = crc16 & 0xff;
//	    HAL_UART_Transmit_DMA(&huart1, g_txBuf, 7);
//	  		  //**
//	    GPIOB->BSRR = GPIO_PIN_15;
//	}


	//====================================================================
	// USART2
	//====================================================================

	//GPIOA->BSRR = GPIO_BSRR_BS1
	// есть ли обмен по UART?
	if(modbusHVAC1.num < 1)  USART2_Modbus.state = stopState;

	if(USART2_Modbus.state == idelFrame)
	{

		// получить следующюю точку для обмена по MODBUS
		int8_t i =  GetNumPoint(&modbusHVAC1, USART2_Modbus.i);
        if(i == -1)
        {
			  USART2_Modbus.i++;
			  if(USART2_Modbus.i >= modbusHVAC1.num) USART2_Modbus.i = 0;
        	goto pass;
        }

        USART2_Modbus.i = i;
		MODBUS_REG(0x0e) = USART2_Modbus.i;
	//==============================================================================
		  // загрузка точеки опроса
//		  if(MODBUS_REG(REG_UNIT_TYPE) == HVAC1)
//		  {
			adr = modbusHVAC1.modbusPoint[USART2_Modbus.i].addr;
			func = modbusHVAC1.modbusPoint[USART2_Modbus.i].func;
			reg = modbusHVAC1.modbusPoint[USART2_Modbus.i].reg;
			data = modbusHVAC1.modbusPoint[USART2_Modbus.i].data;
			ok = modbusHVAC1.modbusPoint[USART2_Modbus.i].ok;
			len_TX = modbusHVAC1.modbusPoint[USART2_Modbus.i].len_TX;
			len_RX = modbusHVAC1.modbusPoint[USART2_Modbus.i].len_RX;
			def = modbusHVAC1.modbusPoint[USART2_Modbus.i].def;
//		  }
	//==============================================================================

		 // GPIOB->BSRR = GPIO_PIN_15;


// формирую пакет
	 buf_TX[0] = adr;
	 buf_TX[1] = func;
	 buf_TX[2] = reg >> 8;
	 buf_TX[3] = reg & 0xff;

	 switch(func)
	 {
	 case 0x01:
	 case 0x03:
		 buf_TX[4] = 0x00; //
		 buf_TX[5] = 0x01; //
	 break;

	 case 0x06:
		 buf_TX[4] = MODBUS_REG(data) >> 8;    //
		 buf_TX[5] = MODBUS_REG(data) & 0xff;  //
	 break;

	 case 0x05:
		 if(READ_COIL(data)) buf_TX[4] = 0xff;
		 else buf_TX[4] = 0x00;

		 buf_TX[5] = 0x00;
	 break;

	 default:
		 buf_TX[4] = 0xaa;    //
		 buf_TX[5] = 0xaa;    //
	 }

	 crc16 = CRC16(buf_TX, len_TX-2);
	 buf_TX[6] = crc16 >>  8;
	 buf_TX[7] = crc16 & 0xFF;

	 USART2_Modbus.wait_for_bytes = len_RX;
	 USART2_DE_TX;

	 // отправляем запрос к SLAVE устройству
	 HAL_UART_Transmit_DMA(&huart2, buf_TX, len_TX);

	 USART2_Modbus.state = sendFrame;
	}

	else if(USART2_Modbus.state == sendFrame)
	{
		// отправка пакета
	}
	else if(USART2_Modbus.state == waitFrame)
	{
		// ожидание пакета
	}
	else if(USART2_Modbus.state == decodeFrame)
	{
		// декодирование пакета

				 USART2_Modbus.Wait_response = 0;  //  счетчик мс для определения таймаута

				  ///
				  //GPIOB->BSRR = (uint32_t)GPIO_PIN_15 << 16U;
				  ///

				  // запись принятого рег-ра
				  uint16_t crc16 = CRC16(USART2_Modbus.rx_Buff, USART2_Modbus.count_bytes_receive - 2);

				  if((crc16 & 0xff) == USART2_Modbus.rx_Buff[USART2_Modbus.count_bytes_receive - 1] ||
				     (crc16 >> 8) == USART2_Modbus.rx_Buff[USART2_Modbus.count_bytes_receive - 2])
				  {
					  if(func == 0x03)
					  {
						// чтение REG заносим принятые данные в REG
					  USART2_Modbus.Data[USART2_Modbus.i] = ((int16_t)(USART2_Modbus.rx_Buff[3]*256 + USART2_Modbus.rx_Buff[4]));
					  uint16_t temp = USART2_Modbus.Data[USART2_Modbus.i];
					  MODBUS_REG(data) = temp;
					 // MODBUS_REG(USART2_Modbus.i + START_REG_MODBUS) = temp;
//					  MODBUS_REG(0x09+USART2_Modbus.i) = (((int16_t *)(USART2_Modbus.rx_Buff+1))[1]);
                      //#define MODBUS_REG(num)		(((int16_t *)g_regs)[num])
					  }
					  else if(func == 0x01)
					  {
						// чтение COIL заносим принятые данные в COIL
						if(USART2_Modbus.rx_Buff[3]) {SET_COIL(data);}
						else  {CLR_COIL(data);}
					  }
                     /////////////////// пакет пришел целый выставляем бит ок
					 // пакет пришел целый обнуляем число непринятых пакетов от этого датчика
					 modbusHVAC1.modbusPoint[USART2_Modbus.i].no_answer = 0;
					 //SET_COIL(ok);

				  }
				  else
				  {
					  ////// пакет пришел битый или не пришел сбрасываем бит ок
					  // пакет пришел битый или не пришел увеличиваем число непринятых пакетов от этого датчика
					  if(modbusHVAC1.modbusPoint[USART2_Modbus.i].no_answer < 5)
						  modbusHVAC1.modbusPoint[USART2_Modbus.i].no_answer++;

					  //CLR_COIL(ok);
					  if(func == 0x03)
					  {
//					   MODBUS_REG(data) = def;
					  }

					  // обнуляем счетчик принятых байт
					  USART2_Modbus.count_bytes_receive = 0;
					  // очищаем буфер
					  for(uint8_t i=0; i<64; i++) USART2_Modbus.rx_Buff[i] = 0;

				  }

				  if(modbusHVAC1.modbusPoint[USART2_Modbus.i].no_answer > 3){
					  CLR_COIL(0x4B * 0x10 + USART2_Modbus.i);//CLR_COIL(ok);
				  }
				  else{
					  SET_COIL(0x4B * 0x10 + USART2_Modbus.i);//SET_COIL(ok);
				  }

				  // перходим по массиву адресов устройств
				  USART2_Modbus.i++;
				//  if(USART2_Modbus.i >= USART2_Modbus.f0x03_NRead) USART2_Modbus.i = 0;
				  if(USART2_Modbus.i >= modbusHVAC1.num) USART2_Modbus.i = 0;

		USART2_Modbus.state = idelFrame;
	}


    pass:

    osDelay(100);
  }
  /* USER CODE END StartUSART_2 */
}

/* USER CODE BEGIN Header_StartAlgoritm */
/**
* @brief Function implementing the AlgoritmTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartAlgoritm */
void StartAlgoritm(void const * argument)
{
  /* USER CODE BEGIN StartAlgoritm */

  // определение констант
  CLR_COIL(START_REG_CONST_32*0x10);
  SET_COIL(START_REG_CONST_32*0x10+1);
  MODBUS_REG(START_REG_CONST_32+1) = 0;
  MODBUS_REG(START_REG_CONST_32+2) = 100;
  MODBUS_REG(START_REG_CONST_32+3) = 30;
  MODBUS_REG(START_REG_CONST_32+4) = 1;
  MODBUS_REG(START_REG_CONST_32+5) = 2;
  MODBUS_REG(START_REG_CONST_32+6) = 20;
  MODBUS_REG(START_REG_CONST_32+7) = 60;
  MODBUS_REG(START_REG_CONST_32+8) = 0x00ff;
  MODBUS_REG(START_REG_CONST_32+9) = 0xff00;
  MODBUS_REG(START_REG_CONST_32+0x0a) = 0x000f;
  MODBUS_REG(START_REG_CONST_32+0x0b) = 0x00f0;
  MODBUS_REG(START_REG_CONST_32+0x0c) = 8;
  MODBUS_REG(START_REG_CONST_32+0x0d) = 4;
  MODBUS_REG(START_REG_CONST_32+0x0e) = 6;
  MODBUS_REG(START_REG_CONST_32+0x0f) = 180;
  MODBUS_REG(START_REG_CONST_32+0x10) = 12;
  MODBUS_REG(START_REG_CONST_32+0x11) = -1;

  // определение блоков
//  GEN GEN1;
//
//  GEN1.Out = 0xf4;
//  GEN1.Ton = 10;
//  GEN1.Toff = 30;
//  GEN1.count = 0;

  // общая часть всех алгоритмов календарь
  // 0x40 Start 0x41 Stop
  // 0x41 День 0x41 Ночь
  sVent_Analog_2 START_AND, START_SHIFR_R, START_MULT, START_ADD;
  sVent_Analog_2 STOP_AND, STOP_SHIFR_R, STOP_MULT, STOP_ADD;

  START_AND.IN1 = REG_TIME_START;
  START_AND.IN2 = START_REG_CONST_32+8;
  START_AND.Out = START_NET_REG;

  START_SHIFR_R.IN1 = REG_TIME_START;
  START_SHIFR_R.IN2 = START_REG_CONST_32+0x0c;
  START_SHIFR_R.Out = START_NET_REG+ 1;

  START_MULT.IN1 = START_NET_REG+ 1;
  START_MULT.IN2 = START_REG_CONST_32+7;
  START_MULT.Out = START_NET_REG +2;

  START_ADD.IN1 = START_NET_REG;
  START_ADD.IN2 = START_NET_REG+ 2;
  START_ADD.Out = START_NET_REG +3;


  STOP_AND.IN1 = REG_TIME_STOP;
  STOP_AND.IN2 = START_REG_CONST_32+8;
  STOP_AND.Out = START_NET_REG +4;

  STOP_SHIFR_R.IN1 = REG_TIME_STOP;
  STOP_SHIFR_R.IN2 = START_REG_CONST_32+0x0c;
  STOP_SHIFR_R.Out = START_NET_REG +5;

  STOP_MULT.IN1 = START_NET_REG +5;
  STOP_MULT.IN2 = START_REG_CONST_32+7;
  STOP_MULT.Out = START_NET_REG +6;

  STOP_ADD.IN1 = START_NET_REG +4;
  STOP_ADD.IN2 = START_NET_REG +6;
  STOP_ADD.Out = START_NET_REG +7;


  // Scheduler
  sScheduler SCHEDULER;
  SCHEDULER.IN_Start = START_NET_REG +3;
  SCHEDULER.IN_Stop = START_NET_REG +7;
  SCHEDULER.IN_Time = REG_TIME;
  SCHEDULER.Out = START_NET_COILS * 0x10 + 0;

  // стоп/пуск/авто
  VENT_2 AND_1, OR_2;

  AND_1.bIN1 = BUTTON_AUTO;
  AND_1.bIN2 = START_NET_COILS  * 0x10 + 0;
  AND_1.bQ = START_NET_COILS  * 0x10+1;

  OR_2.bIN1 = BUTTON_START;
  OR_2.bIN2 = START_NET_COILS  * 0x10+1;
  OR_2.bQ = START_NET_COILS  * 0x10+2;    // сигнал ПУСК

  // общая ошибка
  VENT_8 OR_25, NAND_Modbus;
  VENT_1 NOT_ALARM;


  OR_25.bIN1 = REG_ALARM * 0x10 +1;
  OR_25.bIN2 = REG_ALARM * 0x10 +2;
  OR_25.bIN3 = REG_ALARM * 0x10 +3;
  OR_25.bIN4 = REG_ALARM * 0x10 +4;
  OR_25.bIN5 = REG_ALARM * 0x10 +5;
  OR_25.bIN6 = REG_ALARM * 0x10 +6;
  OR_25.bIN7 = REG_ALARM * 0x10 +7;
  OR_25.bIN8 = REG_ALARM * 0x10 +8;
  OR_25.bQ = REG_ALARM * 0x10 +0;

  NOT_ALARM.bIN = REG_ALARM * 0x10 +0;
  NOT_ALARM.bQ = START_NET_COILS  * 0x10 + 0x1FF;


  // выбор датчика регулирования
  sComparator_2 CMP_SP;
  sSwitcher SWT_OC, SWT_SP;

  CMP_SP.IN1 = START_REG_SETPOINTS + 0x00;
  CMP_SP.IN2 = START_REG_SETPOINTS + 0x05;
  CMP_SP.bOut = START_NET_COILS  * 0x10+ 0x1FE;

  SWT_OC.IN1 = 0x01; // T2
  SWT_OC.IN2 = 0x02; // T3
  SWT_OC.bOn = START_NET_COILS  * 0x10+ 0x1FE;
  SWT_OC.Out = START_NET_REG + 0x7F;             // выбранный для ОС сигнал датчика Т2 или Т3

  SWT_SP.IN1 = START_REG_SETPOINTS + 0x00; // Уставка в канале
  SWT_SP.IN2 = START_REG_SETPOINTS + 0x05; // Уставка в зоне
  SWT_SP.bOn = START_NET_COILS  * 0x10+ 0x1FE;
  SWT_SP.Out = START_NET_REG + 0x7E;             // выбранная уставка SP(0x20) или SP(0х25)

  // конец общей части алгоритмов
  //======================================================================================
  // CHECK_OK struct init
  // общие структуры
  VENT_2 AND_3, AND_4;
  sDelay DEL_5, DEL_6;
  VENT_2 XOR_7, XOR_10;
  sDelay DEL_8, DEL_11;
  RS RS_9, RS_12;
  sDelay DEL_13;
  RS RS_14;
  VENT_1 NOT_15;
  sVent_Analog_2 MIN_16;
  sComparator_2 CMP_17;
  RS RS_18;
  sDelta_2 DELTA_19;
  sDelay DEL_20;
  RS RS_21;
  sDelta_2 DELTA_22;
  sDelay DEL_23;
  RS RS_24;
  sRegulator_1 REGUL_26;
  sVent_Analog_2 MIN_27;
  VENT_2 AND_28;
  sRegulator_1 REGUL_29;
  VENT_2 OR_30;
  sSwitcher SWT_31;
  sRegulator_1 REGUL_32;
  sMUX_4 MUX_33;
  VENT_2 NOR_34;
  VENT_2 AND_35;
  sSwitcher SWT_36;

  sRegulator_step REGUL_STEP_37;
  sRegulator_step REGUL_STEP_38;
  sCounter_2IN COUNTER_2IN_39;
  sVent_Analog_2 EQ_40;
  sMUX_4 MUX_41;

  sMUX_4 MAX_42;
  sDeCoder_4 DECODER_43;

  sPWM PWM_44;
  sPWM PWM_45;
  sEquation EQUATION_46;
  sMUX_dig_4 MUX_DIG_47;

  sDelta_2 DELTA_48;     //-
  sDelay DEL_49;         //-
  RS RS_50;               //-
  VENT_2 AND_51;          //
  sRegulator_1 REGUL_52;  //
  VENT_2 AND_53;  //-
  sHolder Buff_54, Buff_55; //
  VENT_1 NOT_56;
  sComparator_2 CMP_57;
  sComparator_2 CMP_58;
  sVent_Analog_2 ADD_59;
  sLimiter LIM_60;
  sComparator_3 CMP3_61;
  sComparator_3 CMP3_62;

  sComparator_2 CMP_63;
  sComparator_2 CMP_64;
  sSwitcher SWT_65;
  sRegulator_1 REGUL_66;
  VENT_1 NOT_67;
  VENT_2 AND_68;
  VENT_2 AND_69;
  sSwitcher SWT_70;
  sDelay DEL_71;
  sDelay DEL_72;
  sRegulator_5 REGUL_73;
  sSwitcher SWT_74;
  sSwitcher SWT_75;
  VENT_2 OR_76;
  VENT_2 OR_77;
  VENT_2 AND_78;
  VENT_2 AND_79;
  VENT_8 OR_80;
  sDelay DEL_81;
  sMUX_4 MUX_82;
  // CHECK_OK init HVAC2
  //==============================================================================
  // HVAC2
  //==============================================================================
  if(MODBUS_REG(0x49) == HVAC2)
  {
  // HVAC2

	  // ошибка датчиков Modbus
	  NAND_Modbus.bIN1 = REG_POINT_MODBUS_OK * 0x10 + 0;
	  NAND_Modbus.bIN2 = REG_POINT_MODBUS_OK * 0x10 + 1;
	  NAND_Modbus.bIN3 = REG_POINT_MODBUS_OK * 0x10 + 2;
	  NAND_Modbus.bIN4 = REG_POINT_MODBUS_OK * 0x10 + 3;
	  NAND_Modbus.bIN5 = REG_POINT_MODBUS_OK * 0x10 + 4;
	  NAND_Modbus.bIN6 = START_REG_CONST_32*0x10+1;
	  NAND_Modbus.bIN7 = START_REG_CONST_32*0x10+1;
	  NAND_Modbus.bIN8 = START_REG_CONST_32*0x10+1;
	  NAND_Modbus.bQ = REG_ALARM * 0x10 +8;

  // �?М1, �?М2


  AND_3.bIN1 = START_NET_COILS  * 0x10+2; //ПУСК
  AND_3.bIN2 = START_NET_COILS  * 0x10 + 0x1FF;  //Ошибки нет  //START_NET_COILS  * 0x10 +0x10; // рекуператор Норма
  AND_3.bQ = REG_DO * 0x10 +0;   // �?М1

  AND_4.bIN1 = START_NET_COILS  * 0x10+2; //ПУСК
  AND_4.bIN2 = START_NET_COILS  * 0x10 + 0x1FF;  //Ошибки нет //START_REG_CONST_32*0x10+1; // TRUE
  AND_4.bQ = REG_DO * 0x10 +1;   // �?М2

  // П1, В1


  DEL_5.bIN = REG_DO * 0x10 +0;
  DEL_5.bOUT = START_NET_COILS  * 0x10 +0x11;
  DEL_5.delay = 200;

  DEL_6.bIN = REG_DO * 0x10 +1;
  DEL_6.bOUT = REG_DO * 0x10 +3;   // Пуск_В1
  DEL_6.delay = 250;

  // Авария П1, В1


  XOR_7.bIN1 = REG_DO * 0x10 +2;
  XOR_7.bIN2 = REG_DI * 0x10 + 0;
  XOR_7.bQ = START_NET_COILS  * 0x10+3;

  XOR_10.bIN1 = REG_DO * 0x10 +3;
  XOR_10.bIN2 = REG_DI * 0x10 +1;
  XOR_10.bQ = START_NET_COILS  * 0x10+5;

  DEL_8.bIN = START_NET_COILS  * 0x10+3;
  DEL_8.bOUT = START_NET_COILS  * 0x10+4;
  DEL_8.delay = 100; // 10 s

  DEL_11.bIN = START_NET_COILS  * 0x10+5;
  DEL_11.bOUT = START_NET_COILS  * 0x10+6;
  DEL_11.delay = 100; // 10s

  RS_9.bSet = START_NET_COILS  * 0x10+4;
  RS_9.bReset = RESET_BUTTON;
  RS_9.bQ = REG_ALARM * 0x10 +1;

  RS_12.bSet = START_NET_COILS  * 0x10+6;
  RS_12.bReset = RESET_BUTTON;
  RS_12.bQ = REG_ALARM * 0x10 +2;

  // Рекуператор авария
  NOT_15.bIN = REG_DI * 0x10 +3;
  NOT_15.bQ = START_NET_COILS  * 0x10+7;

  DEL_13.bIN = START_NET_COILS  * 0x10+7;
  DEL_13.bOUT = START_NET_COILS  * 0x10+8;
  DEL_13.delay = 20; // 2 s

  RS_14.bSet = START_NET_COILS  * 0x10+8;
  RS_14.bReset = RESET_BUTTON;
  RS_14.bQ = REG_ALARM * 0x10 +3;

  // угроза заморозки рекуператора

  MIN_16.IN1 = 0x00;  // T1
  MIN_16.IN2 = 0x03;  // T4
  MIN_16.Out = START_NET_REG +8;

  // сравнение тем-ры с минимальной

  CMP_17.IN1 = START_REG_CONST_32+0x0d;  // 4 град
  CMP_17.IN2 = START_NET_REG +8;
  CMP_17.bOut = START_NET_COILS  * 0x10+9;

  // фиксация ошибки

  RS_18.bSet = START_NET_COILS  * 0x10+9;
  RS_18.bReset = RESET_BUTTON;
  RS_18.bQ = REG_ALARM * 0x10 +4;   // угроза заморозки рекуператора

  // ошибка привода Y3

  DELTA_19.IN1 = START_REG_AO + 0;
  DELTA_19.IN2 = START_REG_AI + 0;
  DELTA_19.delta = 30; // разница 30%
  DELTA_19.bOut = START_NET_COILS  * 0x10+0x0A;


  DEL_20.bIN = START_NET_COILS  * 0x10+0x0a;
  DEL_20.bOUT = START_NET_COILS  * 0x10+0x0b;
  DEL_20.delay = 600; // 60 s


  RS_21.bSet = START_NET_COILS  * 0x10+0x0b;
  RS_21.bReset = RESET_BUTTON;
  RS_21.bQ = REG_ALARM * 0x10 +5;

  // ошибка привода Y4

  DELTA_22.IN1 = START_REG_AO + 1;
  DELTA_22.IN2 = START_REG_AI + 1;
  DELTA_22.delta = 30; // разница 30%
  DELTA_22.bOut = START_NET_COILS  * 0x10+0x0c;


  DEL_23.bIN = START_NET_COILS  * 0x10+0x0c;
  DEL_23.bOUT = START_NET_COILS  * 0x10+0x0d;
  DEL_23.delay = 600; // 60 s


  RS_24.bSet = START_NET_COILS  * 0x10+0x0d;
  RS_24.bReset = RESET_BUTTON;
  RS_24.bQ = REG_ALARM * 0x10 +6;



  // Y3 управление

  REGUL_26.SetPoint = START_NET_REG + 0x7E; //START_REG_SETPOINTS + 0x00;
  REGUL_26.Threshold = START_REG_SETPOINTS + 0x01;
  REGUL_26.Time = START_REG_SETPOINTS + 0x02; // 4 sec

  REGUL_26.Max = 100;
  REGUL_26.Min = 0;

  REGUL_26.Reverce = REG_MODE * 0x10 +1;  // Reverce
  REGUL_26.Enable =  START_NET_COILS  * 0x10 +2;  // включить регулятор
  REGUL_26.Reset =   REG_MODE * 0x10 + 0;  // reset регулятор 0 - регулятор выключен

  REGUL_26.Y_OC = START_NET_REG + 0x7F;  // сигнал с выбранного датчика Т2 или Т3
  REGUL_26.Y = START_NET_REG + 0x0A;  // управление Y3 Auto //0x30;

  // вычисление минимальной температуры после рекуператора

  MIN_27.IN1 = 0x00; // T1
  MIN_27.IN2 = 0x03; // T4
  MIN_27.Out = START_NET_REG + 0x09;

  // выявление режима "З�?МА"

  AND_28.bIN1 = REG_MODE  * 0x10+0;
  AND_28.bIN2 = REG_MODE  * 0x10+1;
  AND_28.bQ = START_NET_COILS  * 0x10 +0x0E;


  // Y4 управление зима

  REGUL_29.SetPoint = START_REG_CONST_32+0x0e;     // 6 град
  REGUL_29.Threshold = START_REG_CONST_32+4; // 1
  REGUL_29.Time = START_REG_CONST_32+6; // 2 sec

  REGUL_29.Max = 100;
  REGUL_29.Min = 0;

  REGUL_29.Reverce = START_REG_CONST_32*0x10+1;  // 1 Reverce
  REGUL_29.Enable =  START_NET_COILS  * 0x10 +2;  // включить регулятор
  REGUL_29.Reset =   START_NET_COILS  * 0x10 +0x0E;  // reset регулятор 0 - регулятор выключен зима = 1 регулятор включен

  REGUL_29.Y_OC = START_NET_REG +0x09; // минимальная тем-ра после рекуператора
  REGUL_29.Y = START_NET_REG + 0x0B;  // управление Y4 Auto //0x31;

  // ручн/авто

  OR_30.bIN1 = REG_MODE * 0x10 +0;
  OR_30.bIN2 = REG_MODE * 0x10 +1;
  OR_30.bQ = START_NET_COILS  * 0x10 +0x0F; // ручн = 0; авто = 1

  // Управление Y3 ручн/авто
  //====================================================
  // Switch
  // IN1 -  адрес MODBUS_REG
  // IN2 -  адрес MODBUS_REG
  // Out - адрес MODBUS_REG
  // bOn - адрес COIL
  //====================================================
  //void  Switcher(sSwitcher* SWT)

  SWT_31.IN1 = START_NET_REG + 0x0A;
  SWT_31.IN2 =  0x50;  // команда АО1  в ручном
  SWT_31.bOn = START_NET_COILS  * 0x10 +0x0F;
  SWT_31.Out = START_NET_REG + 0x0d;


  // Y4 управление переходный

  REGUL_32.SetPoint = START_NET_REG + 0x7E; //0x20  или 0x25;
  REGUL_32.Threshold = START_REG_SETPOINTS + 0x01; //0x21;
  REGUL_32.Time = START_REG_SETPOINTS + 0x04; //0x24; // 4 sec

  REGUL_32.Max = 100;
  REGUL_32.Min = 0;

  REGUL_32.Reverce = 0x48 * 0x10 +0;  // Direct
  REGUL_32.Enable =  START_NET_COILS  * 0x10 +2;  // включить регулятор
  REGUL_32.Reset =   START_NET_COILS  * 0x10 +2;  // reset регулятор 0 - регулятор выключен

  REGUL_32.Y_OC = START_NET_REG + 0x7F;  // сигнал с выбранного датчика Т2 или Т3
  REGUL_32.Y = START_NET_REG + 0x0C;  // управление Y4 переходн //0x31;

  // Y4 выбор сигнала управления

  MUX_33.IN1 = 0x51; // AO1
  MUX_33.IN2 = START_REG_CONST_32 + 2; //Лето 100%
  MUX_33.IN3 = START_NET_REG + 0x0C; // Y4_упр_переход
  MUX_33.IN4 = START_REG_CONST_32 + 1; // Зима 0%   //START_NET_REG + 0x0B; // Y4_упр_зима
  MUX_33.ADR = 0x48; // режим руч = 0, лето = 1, переход = 2, зима = 3
  MUX_33.Out = 0x31;  // управление Y4

  // рекуператор в норме?

  NOR_34.bIN1 = 0x1F * 0x10 +3;
  NOR_34.bIN2 = 0x1F * 0x10 +4;
  NOR_34.bQ = START_NET_COILS  * 0x10 +0x10; // рекуператор в норме = 1;


  AND_35.bIN1 = START_NET_COILS  * 0x10 +0x11;
  AND_35.bIN2 = START_NET_COILS  * 0x10 +0x10;
  AND_35.bQ = REG_DO * 0x10 + 2; // Пуск_П1 = 1;

  // Открытие  Y3 на полную в случае проблем с рекуператором или низкой тем-ры перед теплообменником
  //====================================================
  // Switch
  // IN1 -  адрес MODBUS_REG
  // IN2 -  адрес MODBUS_REG
  // Out - адрес MODBUS_REG
  // bOn - адрес COIL
  //====================================================

  SWT_36.IN1 = START_NET_REG + 0x0d;
  SWT_36.IN2 =  START_REG_CONST_32 + 2; // 100%
  SWT_36.bOn = START_NET_COILS  * 0x10 +0x10;
  SWT_36.Out = 0x30;




//  REGUL_26.Reverce = REG_MODE * 0x10 +1;  // Reverce
//  REGUL_26.Enable =  START_NET_COILS  * 0x10 +2;  // включить регулятор
  } // end HVAC2

  //==============================================================================
  // HVAC1
  //==============================================================================
  // CHECK_OK init HVAC1
  if(MODBUS_REG(0x49) == HVAC1)
  {
	  // определение констант
//	  CLR_COIL(START_REG_CONST_32*0x10);
//	  SET_COIL(START_REG_CONST_32*0x10+1);
//	  MODBUS_REG(START_REG_CONST_32+1) = 0;
//	  MODBUS_REG(START_REG_CONST_32+2) = 100;
//	  MODBUS_REG(START_REG_CONST_32+3) = 30;
//	  MODBUS_REG(START_REG_CONST_32+4) = 1;
//	  MODBUS_REG(START_REG_CONST_32+5) = 2;
//	  MODBUS_REG(START_REG_CONST_32+6) = 20;
//	  MODBUS_REG(START_REG_CONST_32+7) = 60;
//	  MODBUS_REG(START_REG_CONST_32+8) = 0x00ff;
//	  MODBUS_REG(START_REG_CONST_32+9) = 0xff00;
//	  MODBUS_REG(START_REG_CONST_32+0x0a) = 0x000f;
//	  MODBUS_REG(START_REG_CONST_32+0x0b) = 0x00f0;
//	  MODBUS_REG(START_REG_CONST_32+0x0c) = 8;
//	  MODBUS_REG(START_REG_CONST_32+0x0d) = 4;
//	  MODBUS_REG(START_REG_CONST_32+0x0e) = 6;
//	  MODBUS_REG(START_REG_CONST_32+0x0f) = 180;
//	  MODBUS_REG(START_REG_CONST_32+0x10) = 12;

	  // общие структуры
//	  VENT_2 AND_3;   //-
//	  VENT_2 AND_4;  //-
//	  sDelay DEL_5;
//	  sDelay DEL_6;
//	  VENT_2 XOR_7;  //-
//	  VENT_2 XOR_10;
//	  sDelay DEL_8;   //-
//	  sDelay DEL_11;
//	  RS RS_9;           //-
//	  RS RS_12;          //-
//	  sDelay DEL_13;
//	  RS RS_14;
//	  VENT_1 NOT_15;  //-
//	  sVent_Analog_2 MIN_16;
//	  sComparator_2 CMP_17;  //-
//	  RS RS_18;
//	  sDelta_2 DELTA_19;     //-
//	  sDelay DEL_20;         //-
//	  RS RS_21;              //-
//	  sDelta_2 DELTA_22;
//	  sDelay DEL_23;
//	  RS RS_24;
//	  sRegulator_1 REGUL_26;          //-
//	  sVent_Analog_2 MIN_27;
//	  VENT_2 AND_28;
//	  sRegulator_1 REGUL_29;
//	  VENT_2 OR_30;                     //-
//	  sSwitcher SWT_31;
//	  sRegulator_1 REGUL_32;
//	  sMUX_4 MUX_33;                   //-
//	  VENT_2 NOR_34;
//	  VENT_2 AND_35;
//	  sSwitcher SWT_36;
//
//	  sRegulator_step REGUL_STEP_37;
//	  sRegulator_step REGUL_STEP_38;
//	  sCounter_2IN COUNTER_2IN_39;
//	  sVent_Analog_2 EQ_40;
//	  sMUX_4 MUX_41;
//
//	  sMUX_4 MAX_42; //-
//	  sDeCoder_4 DECODER_43   //-

//    sPWM  PWM_44;
//    sPWM  PWM_45;
//	    sEquation EQUATION_46;   //-
	  //   sMUX_dig_4 MUX_DIG_47;
//	  sDelta_2 DELTA_48;
//	  sDelay DEL_49;
//	  RS RS_50;
//	  VENT_2 AND_51;          //
//	  sRegulator_1 REGUL_52;  //
//	  VENT_2 AND_53;  //
//    VENT_2 AND_68;
//	  VENT_2 AND_69;


  // по какой уставке будет регулирование
//   MAX_42.IN1 = START_REG_SETPOINTS + 0x00;
//   MAX_42.IN2 = START_REG_SETPOINTS + 0x01;
//   MAX_42.IN3 = START_REG_SETPOINTS + 0x02;
//   MAX_42.IN4 = START_REG_SETPOINTS + 0x02;
//   MAX_42.Out = START_NET_REG + 0x08;



	// в зависимости от уличной тем-ры выбирается уставка
	EQUATION_46.x1 = START_REG_SETPOINTS + 0x00;
	EQUATION_46.y1 = START_REG_SETPOINTS + 0x01;
	EQUATION_46.x2 = START_REG_SETPOINTS + 0x02;
	EQUATION_46.y2 = START_REG_SETPOINTS + 0x03;
	EQUATION_46.x = 0x04;  // уличная тем-ра
	EQUATION_46.y = START_NET_REG + 0x08;

   // температура в теплоцентрали низкая
   CMP_17.IN1 = START_REG_SETPOINTS + 0x06;  // Уставка минимальной тем-ры теплоносителя
   CMP_17.IN2 = START_REG_MODBUS_RD + 0x02;  // T3
   CMP_17.bOut = START_NET_COILS * 0x10 +0x03;      // тем-ра в теплоцентрали меньше чем уставка

   // защелка аварии низкая тем-ра теплоцентрали
   RS_9.bSet = START_NET_COILS * 0x10 + 0x03;
   RS_9.bReset = RESET_BUTTON;
   RS_9.bQ = REG_ALARM * 0x10 +1; // ошибка! температура в теплоцентрали низкая!

   NOT_15.bIN = REG_ALARM * 0x10 +0;
   NOT_15.bQ = START_NET_COILS * 0x10 + 0x04;

   AND_3.bIN1 = START_NET_COILS * 0x10 + 0x02;
   AND_3.bIN2 = START_NET_COILS * 0x10 + 0x04;
   AND_3.bQ = START_NET_COILS * 0x10 + 0x0A;     //REG_DO * 0x10 + 0x02;                // D2  Реле насоса М1

   DECODER_43.IN1 = REG_MODE;
   DECODER_43.OUT1 = START_NET_COILS * 0x10 + 0x05;  // Ручной
   DECODER_43.OUT2 = START_NET_COILS * 0x10 + 0x06;  // Лето
   DECODER_43.OUT3 = START_NET_COILS * 0x10 + 0x07;  // Переходный
   DECODER_43.OUT4 = START_NET_COILS * 0x10 + 0x08;  // Зима

   OR_30.bIN1 = START_NET_COILS * 0x10 + 0x05; //
   OR_30.bIN2 = START_NET_COILS * 0x10 + 0x08; //
   OR_30.bQ = START_NET_COILS * 0x10 + 0x09; //

   AND_4.bIN1 = START_NET_COILS * 0x10 + 0x0A;
   AND_4.bIN2 = START_NET_COILS * 0x10 + 0x09;
   AND_4.bQ = REG_DO * 0x10 + 0x02;                // D2  Реле насоса М1

   REGUL_26.SetPoint = START_NET_REG + 0x08;
   REGUL_26.Threshold = START_REG_SETPOINTS + 0x04;
   REGUL_26.Time = START_REG_SETPOINTS + 0x05; // 4 sec

   REGUL_26.Max = 100;
   REGUL_26.Min = 0;

   REGUL_26.Reverce = START_NET_COILS * 0x10 + 0x08;  // Зима  // Reverce  0x03 это реверс чем больше Т2 тем меньше открывем клапан
   REGUL_26.Enable =  REG_DO * 0x10 + 2;  // включить регулятор
   REGUL_26.Reset =   START_NET_COILS * 0x10 + 0x08;   // Зима // reset регулятор 0 - регулятор выключен   зима = 1

   REGUL_26.Y_OC = 0x01;               // T2 тем-ра подачи теплоносителя во внутреннем контуре
   REGUL_26.Y = START_NET_REG + 0x09;  // управление Y1 Auto ;

   MUX_33.ADR = REG_MODE;
   MUX_33.IN1 = START_REG_AO_HND + 0;
   MUX_33.IN2 = START_REG_CONST_32+1;
   MUX_33.IN3 = START_REG_CONST_32+1;
   MUX_33.IN4 = START_NET_REG + 0x09;
   MUX_33.Out = START_REG_AO + 0; // //0x30

	  // ошибка датчиков Modbus
	  NAND_Modbus.bIN1 = REG_POINT_MODBUS_OK * 0x10 + 0;
	  NAND_Modbus.bIN2 = REG_POINT_MODBUS_OK * 0x10 + 1;
	  NAND_Modbus.bIN3 = REG_POINT_MODBUS_OK * 0x10 + 2;
	  NAND_Modbus.bIN4 = REG_POINT_MODBUS_OK * 0x10 + 3;
	  NAND_Modbus.bIN5 = REG_POINT_MODBUS_OK * 0x10 + 4;
	  NAND_Modbus.bIN6 = START_REG_CONST_32*0x10+1;
	  NAND_Modbus.bIN7 = START_REG_CONST_32*0x10+1;
	  NAND_Modbus.bIN8 = START_REG_CONST_32*0x10+1;
	  NAND_Modbus.bQ = REG_ALARM * 0x10 +8;

   // ошибка насоса
   XOR_7.bIN1 = REG_DO*0x10 + 0x02;
   XOR_7.bIN2 = REG_DI * 0x10 + 0x00;
   XOR_7.bQ = START_NET_COILS * 0x10 + 0x0b;

   DEL_8.bIN = START_NET_COILS * 0x10 + 0x0b;
   DEL_8.bOUT = START_NET_COILS * 0x10 + 0x0c;
   DEL_8.delay = 100; // 10 s

   RS_12.bSet = START_NET_COILS * 0x10 + 0x0c;
   RS_12.bReset = RESET_BUTTON;
   RS_12.bQ = REG_ALARM * 0x10 +2;   // ошибка насоса

   // ошибка привода Y1

   DELTA_19.IN1 = START_REG_AO + 0;
   DELTA_19.IN2 = START_REG_AI + 0;
   DELTA_19.delta = 30; // разница 30%
   DELTA_19.bOut = START_NET_COILS  * 0x10+0x0d;

   DEL_20.bIN = START_NET_COILS  * 0x10+0x0d;
   DEL_20.bOUT = START_NET_COILS  * 0x10+0x0e;
   DEL_20.delay = 600; // 60 s

   RS_21.bSet = START_NET_COILS  * 0x10+0x0e;
   RS_21.bReset = RESET_BUTTON;
   RS_21.bQ = REG_ALARM * 0x10 +3; // ошибка привода Y1

//   // тест PWM
//   PWM_44.counter = 0;
//   PWM_44.Period = 40;
//   PWM_44.AI = START_REG_AO_HND + 1;
//   PWM_44.bOUT = REG_DO * 0x10 + 0x03;                // D3  PWM
//
//   PWM_45.counter = 0;
//   PWM_45.Period = 30;
//   PWM_45.AI = START_REG_AO_HND + 2;
//   PWM_45.bOUT = REG_DO * 0x10 + 0x00;                // D0  PWM

  } // end HVAC1

  //==============================================================================
  // HVAC3
  //==============================================================================
  // CHECK_OK init HVAC3
  if(MODBUS_REG(0x49) == HVAC3)
  {
	  // определение констант
//	  CLR_COIL(START_REG_CONST_32*0x10);
//	  SET_COIL(START_REG_CONST_32*0x10+1);
//	  MODBUS_REG(START_REG_CONST_32+1) = 0;
//	  MODBUS_REG(START_REG_CONST_32+2) = 100;
//	  MODBUS_REG(START_REG_CONST_32+3) = 30;
//	  MODBUS_REG(START_REG_CONST_32+4) = 1;
//	  MODBUS_REG(START_REG_CONST_32+5) = 2;
//	  MODBUS_REG(START_REG_CONST_32+6) = 20;
//	  MODBUS_REG(START_REG_CONST_32+7) = 60;
//	  MODBUS_REG(START_REG_CONST_32+8) = 0x00ff;
//	  MODBUS_REG(START_REG_CONST_32+9) = 0xff00;
//	  MODBUS_REG(START_REG_CONST_32+0x0a) = 0x000f;
//	  MODBUS_REG(START_REG_CONST_32+0x0b) = 0x00f0;
//	  MODBUS_REG(START_REG_CONST_32+0x0c) = 8;
//	  MODBUS_REG(START_REG_CONST_32+0x0d) = 4;
//	  MODBUS_REG(START_REG_CONST_32+0x0e) = 6;
//	  MODBUS_REG(START_REG_CONST_32+0x0f) = 180;
//	  MODBUS_REG(START_REG_CONST_32+0x10) = 12;


	  // общие структуры
//	  VENT_2 AND_3;   //
//	  VENT_2 AND_4;   //
//	  sDelay DEL_5;
//	  sDelay DEL_6;
//	  VENT_2 XOR_7;   //
//	  VENT_2 XOR_10;
//	  sDelay DEL_8;   //
//	  sDelay DEL_11;
//	  RS RS_9;           //
//	  RS RS_12;          //
//	  sDelay DEL_13;
//	  RS RS_14;
//	  VENT_1 NOT_15;  //
//	  sVent_Analog_2 MIN_16;
//	  sComparator_2 CMP_17;  //
//	  RS RS_18;
//	  sDelta_2 DELTA_19;     //
//	  sDelay DEL_20;         //
//	  RS RS_21;              //
//	  sDelta_2 DELTA_22;
//	  sDelay DEL_23;
//	  RS RS_24;
//	  sRegulator_1 REGUL_26;          //
//	  sVent_Analog_2 MIN_27;
//	  VENT_2 AND_28;
//	  sRegulator_1 REGUL_29;
//	  VENT_2 OR_30;                     //
//	  sSwitcher SWT_31;               //
//	  sRegulator_1 REGUL_32;
//	  sMUX_4 MUX_33;                   //
//	  VENT_2 NOR_34;
//	  VENT_2 AND_35;
//	  sSwitcher SWT_36;  //
//
//	  sRegulator_step REGUL_STEP_37;
//	  sRegulator_step REGUL_STEP_38;
//	  sCounter_2IN COUNTER_2IN_39;
//	  sVent_Analog_2 EQ_40;
//	  sMUX_4 MUX_41;
//
//	  sMUX_4 MAX_42; //-
//	  sDeCoder_4 DECODER_43

//    sPWM  PWM_44;
//    sPWM  PWM_45;
 //	    sEquation EQUATION_46;
	  //   sMUX_dig_4 MUX_DIG_47;
//	  sDelta_2 DELTA_48;
//	  sDelay DEL_49;
//	  RS RS_50;
//	  VENT_2 AND_51;          //
//	  sRegulator_1 REGUL_52;  //
//	  VENT_2 AND_53;  //
//	  VENT_1 NOT_56;  //
	  // SWT_65
//	  VENT_2 AND_68;
//	  VENT_2 AND_69;
//  sRegulator_5 REGUL_73;
//	  VENT_2 OR_76;
//	  VENT_2 OR_77;
//	  VENT_2 AND_78;
//	  VENT_2 AND_79;
//	  VENT_8 OR_80;

	  // ошибка датчиков Modbus
	  NAND_Modbus.bIN1 = REG_POINT_MODBUS_OK * 0x10 + 0;
	  NAND_Modbus.bIN2 = REG_POINT_MODBUS_OK * 0x10 + 1;
	  NAND_Modbus.bIN3 = REG_POINT_MODBUS_OK * 0x10 + 2;
	  NAND_Modbus.bIN4 = REG_POINT_MODBUS_OK * 0x10 + 3;
	  NAND_Modbus.bIN5 = REG_POINT_MODBUS_OK * 0x10 + 4;
	  NAND_Modbus.bIN6 = START_REG_CONST_32*0x10+1;
	  NAND_Modbus.bIN7 = START_REG_CONST_32*0x10+1;
	  NAND_Modbus.bIN8 = START_REG_CONST_32*0x10+1;
	  NAND_Modbus.bQ = REG_ALARM * 0x10 +8;



	  DEL_5.bIN = START_NET_COILS  * 0x10 +0x02; //ПУСК
	  DEL_5.bOUT = START_NET_COILS  * 0x10 +0x03;
	  DEL_5.delay = 20;

	  AND_3.bIN1 = START_NET_COILS  * 0x10+3;
	  AND_3.bIN2 = START_NET_COILS  * 0x10 + 0x1FF; // NOT_ALARM // START_REG_CONST_32*0x10 + 1; //  TRUE заглушка
	  AND_3.bQ = REG_DO * 0x10 +2;   // DO2   Пуск П1

	  // Авария П1
	  XOR_7.bIN1 = REG_DO * 0x10 +2;
	  XOR_7.bIN2 = REG_DI * 0x10 + 0;
	  XOR_7.bQ = START_NET_COILS  * 0x10+4;

	  DEL_8.bIN = START_NET_COILS  * 0x10+4;
	  DEL_8.bOUT = START_NET_COILS  * 0x10+5;
	  DEL_8.delay = 100; // 10 s

	  RS_9.bSet = START_NET_COILS  * 0x10+5;
	  RS_9.bReset = RESET_BUTTON;
	  RS_9.bQ = REG_ALARM * 0x10 +1;


	  // ошибка привода Y1
	  DELTA_19.IN1 = START_REG_AO + 0;
	  DELTA_19.IN2 = START_REG_AI + 0;
	  DELTA_19.delta = 30; // разница 30%
	  DELTA_19.bOut = START_NET_COILS  * 0x10+0x06;


	  DEL_20.bIN = START_NET_COILS  * 0x10+0x06;
	  DEL_20.bOUT = START_NET_COILS  * 0x10+0x07;
	  DEL_20.delay = 600; // 60 s


	  RS_21.bSet = START_NET_COILS  * 0x10+0x07;
	  RS_21.bReset = RESET_BUTTON;
	  RS_21.bQ = REG_ALARM * 0x10 +5;

	  // ошибка привода Y3
	  DELTA_22.IN1 = START_REG_AO + 1;
	  DELTA_22.IN2 = START_REG_AI + 1;
	  DELTA_22.delta = 30; // разница 30%
	  DELTA_22.bOut = START_NET_COILS  * 0x10+0x08;


	  DEL_23.bIN = START_NET_COILS  * 0x10+0x08;
	  DEL_23.bOUT = START_NET_COILS  * 0x10+0x09;
	  DEL_23.delay = 600; // 60 s


	  RS_24.bSet = START_NET_COILS  * 0x10+0x09;
	  RS_24.bReset = RESET_BUTTON;
	  RS_24.bQ = REG_ALARM * 0x10 +6;

	  // угроза заморозки рекуператора

//	  MIN_16.IN1 = 0x00;  // T1
//	  MIN_16.IN2 = 0x03;  // T4
//	  MIN_16.Out = START_NET_REG +8;

	  // сравнение тем-ры с минимальной

	  CMP_17.IN1 = START_REG_CONST_32+0x0d;  // 4 град  // START_REG_SETPOINTS + 0x06;//
	  CMP_17.IN2 = 0x00;  // T1
	  CMP_17.bOut = REG_ALARM * 0x10 + 4;   // угроза заморозки теплообменика

	  // Y3 управление
	  REGUL_26.SetPoint = START_REG_SETPOINTS + 0x00;
	  REGUL_26.Threshold = START_REG_SETPOINTS + 0x01;
	  REGUL_26.Time = START_REG_SETPOINTS + 0x02; // 20 = 2 sec  реакция водяного клапана Y3

	  REGUL_26.Max = 100;
	  REGUL_26.Min = 0;

	  REGUL_26.Reverce = REG_MODE * 0x10 +1;  // Reverce
	  REGUL_26.Enable =  REG_DI * 0x10 + 0;   // включить регулятор
	  REGUL_26.Reset =   REG_MODE * 0x10 + 0;  // reset регулятор 0 - регулятор выключен

	  REGUL_26.Y_OC = 0x02;  // T3
	  REGUL_26.Y = START_NET_REG + 0x08;  // управление Y3 Auto //0x30;

	  NOT_15.bIN = REG_MODE * 0x10 +1;
	  NOT_15.bQ = START_NET_COILS  * 0x10 + 0x0A;

	  // регулирование Y1 по температуре при переходном режиме / по СО2 зима и лето
	  SWT_74.IN1 = START_REG_SETPOINTS + 0x00;
	  SWT_74.IN2 = START_REG_SETPOINTS + 0x03;
	  SWT_74.bOn = START_NET_COILS  * 0x10 + 0x0D;
	  SWT_74.Out = START_NET_REG + 0x11;

	  SWT_75.IN1 = 0x02;  // T3
	  SWT_75.IN2 = 0x03;  // CO2
	  SWT_75.bOn = START_NET_COILS  * 0x10 + 0x0D;
	  SWT_75.Out = START_NET_REG + 0x12;


	  REGUL_29.SetPoint = START_NET_REG + 0x11;
	  REGUL_29.Threshold = START_REG_SETPOINTS + 0x01;
	  REGUL_29.Time = START_REG_SETPOINTS + 0x04; // Уст5 = 40 = 4 sec

	  REGUL_29.Max = 45;
	  REGUL_29.Min = 12;

	  REGUL_29.Reverce = START_REG_CONST_32*0x10 + 0x01; //Reverce всегда                 //REG_MODE * 0x10 +1;  // лето / вентиляция Reverce = 1;  лето = 0
	  REGUL_29.Enable =  REG_DI * 0x10 + 0;   // включить регулятор
	  REGUL_29.Reset =   START_NET_COILS * 0x10 + 0x1F; //START_REG_CONST_32*0x10 + 0x01;  // reset регулятор 0 - регулятор выключен

	  REGUL_29.Y_OC = START_NET_REG + 0x12;  // СО2 / T3
	  REGUL_29.Y = START_NET_REG + 0x09;  // уставка нужной температуры

	  //====================================================================
	  REGUL_73.SetPoint = START_NET_REG + 0x09;
	  REGUL_73.Threshold = START_REG_SETPOINTS + 0x01;
	  REGUL_73.Time = START_REG_SETPOINTS + 0x04; // 40 = sec

	  REGUL_73.Max = 100;
	  REGUL_73.Min = START_REG_SETPOINTS + 0x05;  //Уст.6 = 10 = 10% Минимальное открытие

	  REGUL_73.Reverce = START_REG_CONST_32*0x10 + 0x00;// Reverce//START_NET_COILS  * 0x10 + 0x0A;  // зима / вентиляция Direct = 1;  лето = 0//START_NET_COILS  * 0x10 + 0x0A;  // зима / переходный - Direct//REG_MODE * 0x10 +1;  // Reverce
	  REGUL_73.Enable =  REG_DI * 0x10 + 0;   // включить регулятор
	  REGUL_73.Reset =   START_REG_CONST_32*0x10 + 0x01;  // reset регулятор 0 - регулятор выключен  //START_NET_COILS * 0x10 + 0x18;  // reset регулятор 0 - регулятор выключен; 1 = клапан Y3 закрыт

	  REGUL_73.Y_OC = 0x00;  // T1
	  REGUL_73.Y = START_NET_REG + 0x0C;  // управление Y1 Auto //0x31;
      //=====================================================================

	  // Отклонение температуры от нормы
	  DELTA_48.IN1 = START_REG_SETPOINTS + 0x00; // уставка
	  DELTA_48.IN2 = 0x02; // T3
	  DELTA_48.delta = 1; // разница 1 градус
	  DELTA_48.bOut = START_NET_COILS  * 0x10+0x0F;

	  NOT_56.bIN = START_NET_COILS  * 0x10+0x0F;
	  NOT_56.bQ = START_NET_COILS  * 0x10+0x10;

	  DEL_49.bIN = START_NET_COILS  * 0x10+0x10;
	  DEL_49.bOUT = START_NET_COILS  * 0x10+0x11;
	  DEL_49.delay = 600; // 60 s

	  // температура Т1 перед теплообменником > 12 градусов
	  CMP_57.IN1 = 0x00;   // T1
	  CMP_57.IN2 = START_REG_CONST_32+0x10; // 12 градусов
	  CMP_57.bOut = START_NET_COILS  * 0x10+0x12;

	  // концентрация СО2 превысила уставку
	  CMP_58.IN1 = 0x03;   // T4 СО2
	  CMP_58.IN2 = START_REG_SETPOINTS + 0x03; // Уставка СО2
	  CMP_58.bOut = START_NET_COILS  * 0x10+0x13;

	  AND_4.bIN1 = START_NET_COILS  * 0x10+0x11;
	  AND_4.bIN2 = START_NET_COILS  * 0x10+0x12;
	  AND_4.bQ = START_NET_COILS  * 0x10+0x14;

	  AND_28.bIN1 = START_NET_COILS  * 0x10+0x14;
	  AND_28.bIN2 = START_NET_COILS  * 0x10+0x13;
	  AND_28.bQ = START_NET_COILS  * 0x10+0x15;      // можно открывать Y1 по СО2

	  // счетчик на два входа
//	  COUNTER_2IN_39.IN1 = START_NET_REG + 0x0E; // 1 / -1  Если Т1 < 15 град то -1
//	  COUNTER_2IN_39.IN2 = START_REG_CONST_32+1;  // 0
//	  COUNTER_2IN_39.Time = START_REG_SETPOINTS + 0x02; // интервал обновления счетчика
//
//	  COUNTER_2IN_39.Max = 100;
//	  COUNTER_2IN_39.Min = 0;
//
//	  COUNTER_2IN_39.Reset = START_NET_COILS * 0x10+0x15;
//
//	  COUNTER_2IN_39.Y = START_NET_REG + 0x0A;

	  ADD_59.IN1 = START_NET_REG + 0x0C;
	  ADD_59.IN2 = START_NET_REG + 0x0A;
	  ADD_59.Out = START_NET_REG + 0x0D;

	  LIM_60.IN = START_NET_REG + 0x0D;
	  LIM_60.Out_Hi = 100;
	  LIM_60.Out_Lo = 0;
	  LIM_60.Out = START_NET_REG + 0x0B;


	  CMP3_61.IN1 = 0x20; // Уставка Т
	  CMP3_61.IN2 = 0x02; // Т зоны
	  CMP3_61.delta = 1;
	  CMP3_61.bOut_GT = COIL_T_LOW;
	  CMP3_61.bOut_EQ = COIL_T_NORM;
	  CMP3_61.bOut_LT = COIL_T_HIGHT;

	  CMP3_62.IN1 = 0x03; // CO2
	  CMP3_62.IN2 = 0x23; // Уставка CO2
	  CMP3_62.delta = 50;  // ppm
	  CMP3_62.bOut_GT = COIL_CO2_HIGHT;
	  CMP3_62.bOut_EQ = START_NET_COILS * 0x10+0x16;
	  CMP3_62.bOut_LT = START_NET_COILS * 0x10+0x17;

	  OR_30.bIN1 = START_NET_COILS * 0x10+0x16;
	  OR_30.bIN2 = START_NET_COILS * 0x10+0x17;
	  OR_30.bQ = COIL_CO2_NORM;

	  //  sRegulator_step REGUL_STEP_37;
	  //   sRegulator_step REGUL_STEP_38;
	  //   sCounter_2IN COUNTER_2IN_39;
	  //   sVent_Analog_2 EQ_40;

	  // регулирование Y1 по температуре
//	    REGUL_STEP_37.SetPoint = START_REG_SETPOINTS + 0x00;
//	    REGUL_STEP_37.Threshold = START_REG_SETPOINTS + 0x01;
//
//	    REGUL_STEP_37.Reverce = REG_MODE * 0x10 +1;  // Reverce 0 - руч; 1 - лето; 2 - перех; 3 - зима
//	    REGUL_STEP_37.Enable =  REG_DI * 0x10 + 0;   // включить регулятор П1 - работает
//	    REGUL_STEP_37.Y_OC =    0x02;
//
//	    REGUL_STEP_37.dY = START_NET_REG + 0x09;  // изменение управления Y1 Auto //0x30;
//
//	    // регулирование Y1 по СО2
//	    REGUL_STEP_38.SetPoint = START_REG_SETPOINTS + 0x03;
//	    REGUL_STEP_38.Threshold =  START_REG_CONST_32+7;  // 60 ppm
//
//	    REGUL_STEP_38.Reverce = START_REG_CONST_32*0x10+0;  // 0
//	    REGUL_STEP_38.Enable =  START_NET_COILS  * 0x10 + 0x0A;   // регулировки по температуре нет т.е. температура в норме
//	    REGUL_STEP_38.Y_OC =    0x03;  // CO2
//
//	    REGUL_STEP_38.dY = START_NET_REG + 0x0A;  // изменение управления Y1 Auto //0x30;
//
//	    // счетчик на два входа
//	    COUNTER_2IN_39.IN1 = START_NET_REG + 0x09;
//	    COUNTER_2IN_39.IN2 = START_NET_REG + 0x0A;
//	    COUNTER_2IN_39.Time = START_REG_SETPOINTS + 0x02; // интервал обновления счетчика
//
//	    COUNTER_2IN_39.Max = 100;
//	    COUNTER_2IN_39.Min = 0;
//
//	    COUNTER_2IN_39.Reset = REG_DI * 0x10 + 0;
//
//	    COUNTER_2IN_39.Y = START_NET_REG + 0x0B;
//
//	    // обнаружение того что регулировки по температуре нет т.е. температура в норме
//	    EQ_40.IN1 = START_REG_CONST_32 + 1; // сравниваем с нулем
//	    EQ_40.IN2 = START_NET_REG + 0x09;
//	    EQ_40.bOut = START_NET_COILS  * 0x10+0x0A;  // если 1 то регулировки температуры нет
//

	    // Y1 выбор сигнала управления

	    MUX_33.IN1 = 0x51; // AO2
	    MUX_33.IN2 = START_NET_REG + 0x0C; // Y1_упр_лето
	    MUX_33.IN3 = START_NET_REG + 0x0C; // Y1_упр_переход
	    MUX_33.IN4 = START_NET_REG + 0x0C; // Y1_упр_зима
	    MUX_33.ADR = 0x48; // режим руч = 0, лето = 1, переход = 2, зима = 3
	    MUX_33.Out = START_NET_REG + 0x0F;  // управление Y1

	    // Y3 выбор сигнала управления

	    MUX_41.IN1 = 0x50; // AO1
	    MUX_41.IN2 = START_NET_REG + 0x08; // Y3_упр_лето
	    MUX_41.IN3 = START_REG_CONST_32+1; // 0% Y3_упр_переход
	    MUX_41.IN4 = START_NET_REG + 0x08; // Y3_упр_зима
	    MUX_41.ADR = 0x48; // режим руч = 0, лето = 1, переход = 2, зима = 3
	    MUX_41.Out = START_NET_REG + 0x10;  // управление Y3

	    Buff_54.IN = START_REG_AO_HND + 0x02;
	    Buff_54.Out =  START_REG_AO + 0x02;

	    Buff_55.IN = START_REG_AO_HND + 0x03;
	    Buff_55.Out =  START_REG_AO + 0x03;

	    DECODER_43.IN1 = REG_MODE;
	    DECODER_43.OUT1 = START_NET_COILS * 0x10 + 0x0B;  // Ручной
	    DECODER_43.OUT2 = START_NET_COILS * 0x10 + 0x0C;  // Лето
	    DECODER_43.OUT3 = START_NET_COILS * 0x10 + 0x0D;  // Переходный
	    DECODER_43.OUT4 = START_NET_COILS * 0x10 + 0x0E;  // Зима

	    CMP_63.IN1 = START_REG_CONST_32 + 0x0c; // 8%
	    CMP_63.IN2 = 0x10;    // Y3_OC
	    CMP_63.bOut = START_NET_COILS * 0x10 + 0x18;

	    CMP_64.IN1 = 0x00;  // T1
	    CMP_64.IN2 = START_REG_CONST_32 + 0x0A; // 15 град
	    CMP_64.bOut = START_NET_COILS * 0x10 + 0x1A;

	    SWT_36.IN1 = START_REG_CONST_32 + 0x04; // 1
	    SWT_36.IN2 = START_REG_CONST_32 + 0x11; // -1
	    SWT_36.bOn = START_NET_COILS * 0x10 + 0x1A;
	    SWT_36.Out = START_NET_REG + 0x0E; // 1 / -1


	    SWT_31.IN1 = START_NET_REG + 0x0F;
	    SWT_31.IN2 = START_REG_CONST_32 + 0x01; // 0
	    SWT_31.bOn = START_NET_COILS * 0x10 + 0x19;  // фанкойл в работе без аварии разрешение управлять клапанами
	    SWT_31.Out = 0x31; // AO2   Y1

	    SWT_65.IN1 = START_NET_REG + 0x10;
	    SWT_65.IN2 = START_REG_CONST_32 + 0x02; // 100 %
	    SWT_65.bOn = START_NET_COILS * 0x10 + 0x19;  // фанкойл в работе без аварии разрешение управлять клапанами
	    SWT_65.Out = 0x30; // AO1   Y3

	    AND_35.bIN1 = START_NET_COILS * 0x10 + 0x1FF; // NOT_ALARM
	    AND_35.bIN2 = 0xF0;                           // П1 в работе
	    AND_35.bQ = START_NET_COILS * 0x10 + 0x19;    // фанкойл в работе и без аварий, разрешается управлять клапанами


	    // Условие регулирования клапана Y1
	    OR_76.bIN1 = COIL_T_NORM;
	    OR_76.bIN2 = COIL_T_HIGHT;
	    OR_76.bQ = START_NET_COILS * 0x10 + 0x1B;    // Норма или жарко

	    OR_77.bIN1 = COIL_T_NORM;
	    OR_77.bIN2 = COIL_T_LOW;
	    OR_77.bQ = START_NET_COILS * 0x10 + 0x1C;    // Норма или холодно

	    AND_78.bIN1 = START_NET_COILS * 0x10 + 0x0E;
	    AND_78.bIN2 = START_NET_COILS * 0x10 + 0x1B;
	    AND_78.bQ = START_NET_COILS * 0x10 + 0x1D;

	    AND_79.bIN1 = START_NET_COILS * 0x10 + 0x0C;
	    AND_79.bIN2 = START_NET_COILS * 0x10 + 0x1C;
	    AND_79.bQ = START_NET_COILS * 0x10 + 0x1E;

	    OR_80.bIN1 = START_NET_COILS * 0x10 + 0x0D;
	    OR_80.bIN2 = START_NET_COILS * 0x10 + 0x1D;
	    OR_80.bIN3 = START_NET_COILS * 0x10 + 0x1E;
	    OR_80.bIN4 = START_REG_CONST_32*0x10+0;
	    OR_80.bIN5 = START_REG_CONST_32*0x10+0;
	    OR_80.bIN6 = START_REG_CONST_32*0x10+0;
	    OR_80.bIN7 = START_REG_CONST_32*0x10+0;
	    OR_80.bIN8 = START_REG_CONST_32*0x10+0;
	    OR_80.bQ = START_NET_COILS * 0x10 + 0x1F;

//	    		  CMP3_61.bOut_GT = COIL_T_LOW;
//	    		  CMP3_61.bOut_EQ = COIL_T_NORM;
//	    		  CMP3_61.bOut_LT = COIL_T_HIGHT
  }// end HVAC3

  //==============================================================================
  // HVAC4
  //==============================================================================
  // CHECK_OK init HVAC4
  // определение констант
//	  CLR_COIL(START_REG_CONST_32*0x10);
//	  SET_COIL(START_REG_CONST_32*0x10+1);
//	  MODBUS_REG(START_REG_CONST_32+1) = 0;
//	  MODBUS_REG(START_REG_CONST_32+2) = 100;
//	  MODBUS_REG(START_REG_CONST_32+3) = 30;
//	  MODBUS_REG(START_REG_CONST_32+4) = 1;
//	  MODBUS_REG(START_REG_CONST_32+5) = 2;
//	  MODBUS_REG(START_REG_CONST_32+6) = 20;
//	  MODBUS_REG(START_REG_CONST_32+7) = 60;
//	  MODBUS_REG(START_REG_CONST_32+8) = 0x00ff;
//	  MODBUS_REG(START_REG_CONST_32+9) = 0xff00;
//	  MODBUS_REG(START_REG_CONST_32+0x0a) = 0x000f;
//	  MODBUS_REG(START_REG_CONST_32+0x0b) = 0x00f0;
//	  MODBUS_REG(START_REG_CONST_32+0x0c) = 8;
//	  MODBUS_REG(START_REG_CONST_32+0x0d) = 4;
//	  MODBUS_REG(START_REG_CONST_32+0x0e) = 6;
//	  MODBUS_REG(START_REG_CONST_32+0x0f) = 180;


  // общие структуры
//	  VENT_2 AND_3;   //-
//	  VENT_2 AND_4;  //-
//	  sDelay DEL_5;   //-
//	  sDelay DEL_6;   //-
//	  VENT_2 XOR_7;  //-
//	  VENT_2 XOR_10;
//	  sDelay DEL_8;   //-
//	  sDelay DEL_11;
//	  RS RS_9;           //-
//	  RS RS_12;          //
//	  sDelay DEL_13;
//	  RS RS_14;
//	  VENT_1 NOT_15;     //
//	  sVent_Analog_2 MIN_16;
//	  sComparator_2 CMP_17;  //
//	  RS RS_18;
//	  sDelta_2 DELTA_19;     //
//	  sDelay DEL_20;         //-
//	  RS RS_21;              //-
//	  sDelta_2 DELTA_22;
//	  sDelay DEL_23;
//	  RS RS_24;
//	  sRegulator_1 REGUL_26;          //-
//	  sVent_Analog_2 MIN_27;
//	  VENT_2 AND_28;
//	  sRegulator_1 REGUL_29;
//	  VENT_2 OR_30;                     //-
//	  sSwitcher SWT_31;
//	  sRegulator_1 REGUL_32;           //
//	  sMUX_4 MUX_33;                   //
//	  VENT_2 NOR_34;
//	  VENT_2 AND_35;
//	  sSwitcher SWT_36;
//
//	  sRegulator_step REGUL_STEP_37;          //-
//	  sRegulator_step REGUL_STEP_38;          //-
//	  sCounter_2IN COUNTER_2IN_39;          //-
//	  sVent_Analog_2 EQ_40;
//	  sMUX_4 MUX_41;          //-
//
//	  sMUX_4 MAX_42; //
//	  sDeCoder_4 DECODER_43         //-

//    sPWM  PWM_44;     //-
//    sPWM  PWM_45;                 //-
  //      sEquation EQUATION_46;     //-
  //   sMUX_dig_4 MUX_DIG_47;     //-
//  sDelta_2 DELTA_48;
//  sDelay DEL_49;
//  RS RS_50;
//	  VENT_2 AND_51;          //
//	  sRegulator_1 REGUL_52;  //
//	  VENT_2 AND_53;  //
//  VENT_1 NOT_67;
//  VENT_2 AND_68;
//  VENT_2 AND_69;
  //  sSwitcher SWT_70;
  //  sDelay DEL_71; //
  //  sDelay DEL_72; //

  if(MODBUS_REG(0x49) == HVAC4)
  {
	  // определение констант
//	  CLR_COIL(START_REG_CONST_32*0x10);
//	  SET_COIL(START_REG_CONST_32*0x10+1);
//	  MODBUS_REG(START_REG_CONST_32+1) = 0;
//	  MODBUS_REG(START_REG_CONST_32+2) = 100;
//	  MODBUS_REG(START_REG_CONST_32+3) = 30;
//	  MODBUS_REG(START_REG_CONST_32+4) = 1;
//	  MODBUS_REG(START_REG_CONST_32+5) = 2;
//	  MODBUS_REG(START_REG_CONST_32+6) = 20;
//	  MODBUS_REG(START_REG_CONST_32+7) = 60;
//	  MODBUS_REG(START_REG_CONST_32+8) = 0x00ff;
//	  MODBUS_REG(START_REG_CONST_32+9) = 0xff00;
//	  MODBUS_REG(START_REG_CONST_32+0x0a) = 0x000f;
//	  MODBUS_REG(START_REG_CONST_32+0x0b) = 0x00f0;
//	  MODBUS_REG(START_REG_CONST_32+0x0c) = 8;
//	  MODBUS_REG(START_REG_CONST_32+0x0d) = 4;
//	  MODBUS_REG(START_REG_CONST_32+0x0e) = 6;
//	  MODBUS_REG(START_REG_CONST_32+0x0f) = 180;
//	  MODBUS_REG(START_REG_CONST_32+0x10) = 12;



// ==== REGION

      // CHECK_OK  23.02.2026
	  // ошибка датчиков Modbus
	  NAND_Modbus.bIN1 = REG_POINT_MODBUS_OK * 0x10 + 0;
	  NAND_Modbus.bIN2 = REG_POINT_MODBUS_OK * 0x10 + 1;
	  NAND_Modbus.bIN3 = REG_POINT_MODBUS_OK * 0x10 + 2;
	  NAND_Modbus.bIN4 = REG_POINT_MODBUS_OK * 0x10 + 3;
	  NAND_Modbus.bIN5 = REG_POINT_MODBUS_OK * 0x10 + 4;
	  NAND_Modbus.bIN6 = START_REG_CONST_32*0x10+1;
	  NAND_Modbus.bIN7 = START_REG_CONST_32*0x10+1;
	  NAND_Modbus.bIN8 = START_REG_CONST_32*0x10+1;
	  NAND_Modbus.bQ = REG_ALARM * 0x10 + 8;  // ошибка датчика Modbus



//	  DEL_72.bIN = START_NET_COILS  * 0x10 +0x1F;
//	  DEL_72.delay = 30;  // 3 sec
//	  DEL_72.bOUT = REG_ALARM * 0x10 +8; // ошибка датчика Modbus


	  DEL_5.bIN = START_NET_COILS  * 0x10 +0x02; //ПУСК
	  DEL_5.bOUT = START_NET_COILS  * 0x10 +0x03;
	  DEL_5.delay = 20;

	  AND_3.bIN1 = START_NET_COILS  * 0x10+3;
	  AND_3.bIN2 = START_NET_COILS  * 0x10 + 0x1FF; // NOT_ALARM // START_REG_CONST_32*0x10 + 1; //  TRUE заглушка
	  AND_3.bQ =   START_NET_COILS  * 0x10 +0x1A;   // "Запустить П1"

	  DEL_6.bIN = START_NET_COILS  * 0x10 +0x1A;   // "Запустить П1"
	  DEL_6.bOUT =  REG_DO * 0x10 +2;   // DO2   Пуск П1
	  DEL_6.delay = 400;



	  //Включить контактор
      NOT_67.bIN = START_NET_COILS  * 0x10 +0x0C;   // "Лето"
      NOT_67.bQ = START_NET_COILS  * 0x10 +0x1C;

      AND_68.bIN1 = START_NET_COILS  * 0x10 +0x1C;    // не лето
      AND_68.bIN2 = START_NET_COILS  * 0x10 +0x1B;   // "Запустить П1" и "П1 в работе"
      AND_68.bQ = START_NET_COILS  * 0x10 +0x1D;

      AND_69.bIN1 = START_NET_COILS  * 0x10 +0x1D;
      AND_69.bIN2 = START_NET_COILS  * 0x10 +0x1FF;  //  NOt_ALARM
      AND_69.bQ = REG_DO * 0x10 +3;   // DO3   Включить контактор

	  AND_51.bIN1 = START_NET_COILS  * 0x10+0x1A;    // "Запустить П1"
	  AND_51.bIN2 = REG_DI * 0x10 + 0;   // "П1 в работе"
	  AND_51.bQ =   START_NET_COILS  * 0x10 +0x1B;   // "Запустить П1" и "П1 в работе"

	  // Авария П1
	  XOR_7.bIN1 = REG_DO * 0x10 +2;
	  XOR_7.bIN2 = REG_DI * 0x10 + 0;
	  XOR_7.bQ = START_NET_COILS  * 0x10+4;

	  DEL_8.bIN = START_NET_COILS  * 0x10+4;
	  DEL_8.bOUT = START_NET_COILS  * 0x10+5;
	  DEL_8.delay = 100; // 10 s

	  RS_9.bSet = START_NET_COILS  * 0x10+5;
	  RS_9.bReset = RESET_BUTTON;
	  RS_9.bQ = REG_ALARM * 0x10 +1;


	  // ошибка привода Y1
	  DELTA_19.IN1 = START_REG_AO + 1;
	  DELTA_19.IN2 = START_REG_AI + 1;
	  DELTA_19.delta = 30; // разница 30%
	  DELTA_19.bOut = START_NET_COILS  * 0x10+0x06;


	  DEL_20.bIN = START_NET_COILS  * 0x10+0x06;
	  DEL_20.bOUT = START_NET_COILS  * 0x10+0x07;
	  DEL_20.delay = 600; // 60 s


	  RS_21.bSet = START_NET_COILS  * 0x10+0x07;
	  RS_21.bReset = RESET_BUTTON;
	  RS_21.bQ = REG_ALARM * 0x10 +5;



	  // ошибка привода Y3
//	  DELTA_22.IN1 = START_REG_AO + 1;
//	  DELTA_22.IN2 = START_REG_AI + 1;
//	  DELTA_22.delta = 30; // разница 30%
//	  DELTA_22.bOut = START_NET_COILS  * 0x10+0x08;
//
//
//	  DEL_23.bIN = START_NET_COILS  * 0x10+0x08;
//	  DEL_23.bOUT = START_NET_COILS  * 0x10+0x09;
//	  DEL_23.delay = 600; // 60 s
//
//
//	  RS_24.bSet = START_NET_COILS  * 0x10+0x09;
//	  RS_24.bReset = RESET_BUTTON;
//	  RS_24.bQ = REG_ALARM * 0x10 +6;

	  // угроза заморозки рекуператора

//	  MIN_16.IN1 = 0x00;  // T1
//	  MIN_16.IN2 = 0x03;  // T4
//	  MIN_16.Out = START_NET_REG +8;

	  // сравнение тем-ры с минимальной

	  CMP_17.IN1 = START_REG_CONST_32+0x0d;  // 4 град
	  CMP_17.IN2 = 0x00;  // T1
	  CMP_17.bOut = REG_ALARM * 0x10 + 4;   // угроза заморозки теплообменика

	  //  CHECK_OK 23_02_2026 END


	  // Задание уставки в канале
	  REGUL_26.SetPoint = START_REG_SETPOINTS + 0x00;
	  REGUL_26.Threshold = START_REG_SETPOINTS + 0x01;
	  REGUL_26.Time = START_REG_SETPOINTS + 0x02; // 4 sec

	  REGUL_26.Max = 45;
	  REGUL_26.Min = 12;

	  REGUL_26.Reverce = START_REG_CONST_32*0x10+1;  // 1 = Reverce
	  REGUL_26.Enable =  REG_DI * 0x10 + 0;   // включить регулятор
	  REGUL_26.Reset =   START_NET_COILS  * 0x10 + 0x1B;   // "Запустить П1" и "П1 в работе"

	  REGUL_26.Y_OC = 0x02;  // T3
	  REGUL_26.Y = START_NET_REG + 0x0A;  // Уставка температуры в канале 12 - 45

	  // ТЭН управление
	  REGUL_66.SetPoint = START_NET_REG + 0x0A;  // Уставка температуры в канале 12 - 45
	  REGUL_66.Threshold = START_REG_SETPOINTS + 0x01;
	  REGUL_66.Time = START_REG_SETPOINTS + 0x02; // 4 sec

	  REGUL_66.Max = 100;
	  REGUL_66.Min = 0;

	  REGUL_66.Reverce = START_REG_CONST_32*0x10+1;  // 1 = Reverce
	  REGUL_66.Enable =  REG_DI * 0x10 + 0;   // включить регулятор
	  REGUL_66.Reset =   START_NET_COILS  * 0x10 + 0x1B;   // "Запустить П1" и "П1 в работе"

	  REGUL_66.Y_OC = 0x01;  // T2
	  REGUL_66.Y = START_NET_REG + 0x08;  // ТЭН упр Авто


	  NOT_15.bIN = REG_MODE * 0x10 +1;
	  NOT_15.bQ = START_NET_COILS  * 0x10 + 0x0A;


	  // регулирование Y1 по температуре при переходном режиме / по СО2 зима и лето
	  SWT_74.IN1 = START_REG_SETPOINTS + 0x00;
	  SWT_74.IN2 = START_REG_SETPOINTS + 0x03;
	  SWT_74.bOn = START_NET_COILS  * 0x10 + 0x0D;
	  SWT_74.Out = START_NET_REG + 0x11;

	  SWT_75.IN1 = 0x02;  // T3
	  SWT_75.IN2 = 0x03;  // CO2
	  SWT_75.bOn = START_NET_COILS  * 0x10 + 0x0D;
	  SWT_75.Out = START_NET_REG + 0x12;


	  REGUL_29.SetPoint = START_NET_REG + 0x11;
	  REGUL_29.Threshold = START_REG_SETPOINTS + 0x01;
	  REGUL_29.Time = START_REG_SETPOINTS + 0x04; // Уст5 = 40 = 4 sec

	  REGUL_29.Max = 45;
	  REGUL_29.Min = 12;

	  REGUL_29.Reverce = START_REG_CONST_32*0x10 + 0x01; //Reverce всегда                 //REG_MODE * 0x10 +1;  // лето / вентиляция Reverce = 1;  лето = 0
	  REGUL_29.Enable =  REG_DI * 0x10 + 0;   // включить регулятор
	  REGUL_29.Reset =   START_NET_COILS * 0x10 + 0x1F; //START_REG_CONST_32*0x10 + 0x01;  // reset регулятор 0 - регулятор выключен

	  REGUL_29.Y_OC = START_NET_REG + 0x12;  // СО2 / T3
	  REGUL_29.Y = START_NET_REG + 0x09;  // уставка нужной температуры

	  //====================================================================
	  REGUL_73.SetPoint = START_NET_REG + 0x09;
	  REGUL_73.Threshold = START_REG_SETPOINTS + 0x01;
	  REGUL_73.Time = START_REG_SETPOINTS + 0x04; // 40 = sec

	  REGUL_73.Max = 100;
	  REGUL_73.Min = START_REG_SETPOINTS + 0x05;  //Уст.6 = 10 = 10% Минимальное открытие

	  REGUL_73.Reverce = START_REG_CONST_32*0x10 + 0x00;// Reverce//START_NET_COILS  * 0x10 + 0x0A;  // зима / вентиляция Direct = 1;  лето = 0//START_NET_COILS  * 0x10 + 0x0A;  // зима / переходный - Direct//REG_MODE * 0x10 +1;  // Reverce
	  REGUL_73.Enable =  REG_DI * 0x10 + 0;   // включить регулятор
	  REGUL_73.Reset =   START_REG_CONST_32*0x10 + 0x01;  // reset регулятор 0 - регулятор выключен  //START_NET_COILS * 0x10 + 0x18;  // reset регулятор 0 - регулятор выключен; 1 = клапан Y3 закрыт

	  REGUL_73.Y_OC = 0x00;  // T1
	  REGUL_73.Y = START_NET_REG + 0x0B;  // управление Y1 Auto //0x31;
      //=====================================================================


//	  // регулирование Y1 по температуре
//	  REGUL_29.SetPoint = START_REG_SETPOINTS + 0x00;
//	  REGUL_29.Threshold = START_REG_SETPOINTS + 0x01;
//	  REGUL_29.Time = START_REG_SETPOINTS + 0x02; // 4 sec
//
//	  REGUL_29.Max = 45;
//	  REGUL_29.Min = 12;
//
//	  REGUL_29.Reverce = START_REG_CONST_32*0x10 + 0x01; //Reverce всегда//REG_MODE * 0x10 +1;  // лето / вентиляция Reverce = 1;  лето = 0
//	  REGUL_29.Enable =  REG_DI * 0x10 + 0;   // включить регулятор
//	  REGUL_29.Reset =   START_REG_CONST_32*0x10 + 0x01;  // reset регулятор 0 - регулятор выключен
//
//	  REGUL_29.Y_OC = 0x02;  // T3
//	  REGUL_29.Y = START_NET_REG + 0x09;  // уставка нужной температуры
//
//	  REGUL_32.SetPoint = START_NET_REG + 0x09;
//	  REGUL_32.Threshold = START_REG_SETPOINTS + 0x01;
//	  REGUL_32.Time = START_REG_SETPOINTS + 0x02; // 4 sec
//
//	  REGUL_32.Max = 100;
//	  REGUL_32.Min = 0;
//
//	  REGUL_32.Reverce = START_NET_COILS  * 0x10 + 0x0A;  // зима / вентиляция Direct = 1;  лето = 0//START_NET_COILS  * 0x10 + 0x0A;  // зима / переходный - Direct//REG_MODE * 0x10 +1;  // Reverce
//	  REGUL_32.Enable =  REG_DI * 0x10 + 0;   // включить регулятор
//	  REGUL_32.Reset =   START_NET_COILS * 0x10 + 0x18;  // reset регулятор 0 - регулятор выключен; 1 = клапан Y3 закрыт
//
//	  REGUL_32.Y_OC = 0x01;  // T2
//	  REGUL_32.Y = START_NET_REG + 0x12;  // управление Y1 Auto //0x31;


//	  DEL_71.bIN =  REG_DI * 0x10 + 0;
//	  DEL_71.delay = 600;  // 60 s
//	  DEL_71.bOUT = START_NET_COILS * 0x10 + 0x1E;
//
//	  SWT_70.IN1 = START_NET_REG + 0x12;
//	  SWT_70.IN2 = START_REG_CONST_32+1;
//	  SWT_70.bOn = START_NET_COILS * 0x10 + 0x1E;
//	  SWT_70.Out = START_NET_REG + 0x0C;

	  //  sSwitcher SWT_70;


	  // Отклонение температуры от нормы
	  DELTA_48.IN1 = START_REG_SETPOINTS + 0x00; // уставка
	  DELTA_48.IN2 = 0x02; // T3
	  DELTA_48.delta = 1; // разница 1 градус
	  DELTA_48.bOut = START_NET_COILS  * 0x10+0x0F;

	  NOT_56.bIN = START_NET_COILS  * 0x10+0x0F;
	  NOT_56.bQ = START_NET_COILS  * 0x10+0x10;

	  DEL_49.bIN = START_NET_COILS  * 0x10+0x10;
	  DEL_49.bOUT = START_NET_COILS  * 0x10+0x11;
	  DEL_49.delay = 600; // 60 s

	  // температура Т1 перед теплообменником > 12 градусов
	  CMP_57.IN1 = 0x00;   // T1
	  CMP_57.IN2 = START_REG_CONST_32+0x10; // 12 градусов
	  CMP_57.bOut = START_NET_COILS  * 0x10+0x12;

	  // концентрация СО2 превысила уставку
	  CMP_58.IN1 = 0x03;   // T4 СО2
	  CMP_58.IN2 = START_REG_SETPOINTS + 0x03; // Уставка СО2
	  CMP_58.bOut = START_NET_COILS  * 0x10+0x13;

	  AND_4.bIN1 = START_NET_COILS  * 0x10+0x11;
	  AND_4.bIN2 = START_NET_COILS  * 0x10+0x12;
	  AND_4.bQ = START_NET_COILS  * 0x10+0x14;

	  AND_28.bIN1 = START_NET_COILS  * 0x10+0x14;
	  AND_28.bIN2 = START_NET_COILS  * 0x10+0x13;
	  AND_28.bQ = START_NET_COILS  * 0x10+0x15;      // можно открывать Y1 по СО2

//	  // счетчик на два входа
//	  COUNTER_2IN_39.IN1 = START_NET_REG + 0x0E; // 1 / -1  Если Т1 < 15 град то -1
//	  COUNTER_2IN_39.IN2 = START_REG_CONST_32+1;  // 0
//	  COUNTER_2IN_39.Time = START_REG_SETPOINTS + 0x02; // интервал обновления счетчика
//
//	  COUNTER_2IN_39.Max = 100;
//	  COUNTER_2IN_39.Min = 0;
//
//	  COUNTER_2IN_39.Reset = START_NET_COILS * 0x10+0x15;
//
//	  COUNTER_2IN_39.Y = START_NET_REG + 0x0A;

//	  ADD_59.IN1 = START_NET_REG + 0x0C;
//	  ADD_59.IN2 = START_NET_REG + 0x0A;
//	  ADD_59.Out = START_NET_REG + 0x0D;
//
//	  LIM_60.IN = START_NET_REG + 0x0D;
//	  LIM_60.Out_Hi = 100;
//	  LIM_60.Out_Lo = 0;
//	  LIM_60.Out = START_NET_REG + 0x0B;


	  CMP3_61.IN1 = 0x20; // Уставка Т
	  CMP3_61.IN2 = 0x02; // Т зоны
	  CMP3_61.delta = 1;
	  CMP3_61.bOut_GT = COIL_T_LOW;
	  CMP3_61.bOut_EQ = COIL_T_NORM;
	  CMP3_61.bOut_LT = COIL_T_HIGHT;

	  CMP3_62.IN1 = 0x03; // CO2
	  CMP3_62.IN2 = 0x23; // Уставка CO2
	  CMP3_62.delta = 5;  // Было 50 ppm
	  CMP3_62.bOut_GT = COIL_CO2_HIGHT;
	  CMP3_62.bOut_EQ = START_NET_COILS * 0x10+0x16;
	  CMP3_62.bOut_LT = START_NET_COILS * 0x10+0x17;

	  OR_30.bIN1 = START_NET_COILS * 0x10+0x16;
	  OR_30.bIN2 = START_NET_COILS * 0x10+0x17;
	  OR_30.bQ = COIL_CO2_NORM;

	  //  sRegulator_step REGUL_STEP_37;
	  //   sRegulator_step REGUL_STEP_38;
	  //   sCounter_2IN COUNTER_2IN_39;
	  //   sVent_Analog_2 EQ_40;

	  // регулирование Y1 по температуре
//	    REGUL_STEP_37.SetPoint = START_REG_SETPOINTS + 0x00;
//	    REGUL_STEP_37.Threshold = START_REG_SETPOINTS + 0x01;
//
//	    REGUL_STEP_37.Reverce = REG_MODE * 0x10 +1;  // Reverce 0 - руч; 1 - лето; 2 - перех; 3 - зима
//	    REGUL_STEP_37.Enable =  REG_DI * 0x10 + 0;   // включить регулятор П1 - работает
//	    REGUL_STEP_37.Y_OC =    0x02;
//
//	    REGUL_STEP_37.dY = START_NET_REG + 0x09;  // изменение управления Y1 Auto //0x30;
//
//	    // регулирование Y1 по СО2
//	    REGUL_STEP_38.SetPoint = START_REG_SETPOINTS + 0x03;
//	    REGUL_STEP_38.Threshold =  START_REG_CONST_32+7;  // 60 ppm
//
//	    REGUL_STEP_38.Reverce = START_REG_CONST_32*0x10+0;  // 0
//	    REGUL_STEP_38.Enable =  START_NET_COILS  * 0x10 + 0x0A;   // регулировки по температуре нет т.е. температура в норме
//	    REGUL_STEP_38.Y_OC =    0x03;  // CO2
//
//	    REGUL_STEP_38.dY = START_NET_REG + 0x0A;  // изменение управления Y1 Auto //0x30;
//
//	    // счетчик на два входа
//	    COUNTER_2IN_39.IN1 = START_NET_REG + 0x09;
//	    COUNTER_2IN_39.IN2 = START_NET_REG + 0x0A;
//	    COUNTER_2IN_39.Time = START_REG_SETPOINTS + 0x02; // интервал обновления счетчика
//
//	    COUNTER_2IN_39.Max = 100;
//	    COUNTER_2IN_39.Min = 0;
//
//	    COUNTER_2IN_39.Reset = REG_DI * 0x10 + 0;
//
//	    COUNTER_2IN_39.Y = START_NET_REG + 0x0B;
//
//	    // обнаружение того что регулировки по температуре нет т.е. температура в норме
//	    EQ_40.IN1 = START_REG_CONST_32 + 1; // сравниваем с нулем
//	    EQ_40.IN2 = START_NET_REG + 0x09;
//	    EQ_40.bOut = START_NET_COILS  * 0x10+0x0A;  // если 1 то регулировки температуры нет
//

	    // Y1 выбор сигнала управления

	    MUX_33.IN1 = 0x51; // AO2
	    MUX_33.IN2 = START_NET_REG + 0x0B; // Y1_упр_лето
	    MUX_33.IN3 = START_NET_REG + 0x0B; // Y1_упр_переход
	    MUX_33.IN4 = START_NET_REG + 0x0B; // Y1_упр_зима
	    MUX_33.ADR = 0x48; // режим руч = 0, лето = 1, переход = 2, зима = 3
	    MUX_33.Out = START_NET_REG + 0x0F;  // управление Y1

	    // Y3 выбор сигнала управления

	    MUX_41.IN1 = 0x50; // AO1
	    MUX_41.IN2 = START_REG_CONST_32+1; //0% Y3_упр_лето
	    MUX_41.IN3 = START_NET_REG + 0x08; //  Y3_упр_переход
	    MUX_41.IN4 = START_NET_REG + 0x08; // Y3_упр_зима
	    MUX_41.ADR = 0x48; // режим руч = 0, лето = 1, переход = 2, зима = 3
	    MUX_41.Out = START_NET_REG + 0x10;  // управление Y3

	    Buff_54.IN = START_REG_AO_HND + 0x02;
	    Buff_54.Out =  START_REG_AO + 0x02;

	    Buff_55.IN = START_REG_AO_HND + 0x03;
	    Buff_55.Out =  START_REG_AO + 0x03;

	    DECODER_43.IN1 = REG_MODE;
	    DECODER_43.OUT1 = START_NET_COILS * 0x10 + 0x0B;  // Ручной
	    DECODER_43.OUT2 = START_NET_COILS * 0x10 + 0x0C;  // Лето
	    DECODER_43.OUT3 = START_NET_COILS * 0x10 + 0x0D;  // Переходный
	    DECODER_43.OUT4 = START_NET_COILS * 0x10 + 0x0E;  // Зима

//	    CMP_63.IN1 = START_REG_CONST_32 + 0x01; // 0%
//	    CMP_63.IN2 = 0x30;    // ТЭН упр
//	    CMP_63.bOut = START_NET_COILS * 0x10 + 0x18;

//	    CMP_64.IN1 = 0x00;  // T1
//	    CMP_64.IN2 = START_REG_CONST_32 + 0x0A; // 15 град
//	    CMP_64.bOut = START_NET_COILS * 0x10 + 0x17;
//
//	    SWT_36.IN1 = START_REG_CONST_32 + 0x04; // 1
//	    SWT_36.IN2 = START_REG_CONST_32 + 0x11; // -1
//	    SWT_36.bOn = START_NET_COILS * 0x10 + 0x17;
//	    SWT_36.Out = START_NET_REG + 0x0E; // 1 / -1


	    SWT_31.IN1 = START_NET_REG + 0x0F;
	    SWT_31.IN2 = START_REG_CONST_32 + 0x01; // 0
	    SWT_31.bOn = START_NET_COILS * 0x10 + 0x19;  // фанкойл в работе без аварии разрешение управлять клапанами
	    SWT_31.Out = 0x31; // AO2   Y1

	    SWT_65.IN1 = START_NET_REG + 0x10;
	    SWT_65.IN2 = START_REG_CONST_32 + 0x01; // 0 %
	    SWT_65.bOn = START_NET_COILS * 0x10 + 0x19;  // фанкойл в работе без аварии разрешение управлять клапанами
	    SWT_65.Out = 0x30; // AO1   ТЭН

	    AND_35.bIN1 = START_NET_COILS * 0x10 + 0x1FF; // NOT_ALARM
	    AND_35.bIN2 = 0xF0;                           // П1 в работе
	    AND_35.bQ = START_NET_COILS * 0x10 + 0x19;    // фанкойл в работе и без аварий, разрешается управлять клапанами



	    // Условие регулирования клапана Y1
	    OR_76.bIN1 = COIL_T_NORM;
	    OR_76.bIN2 = COIL_T_HIGHT;
	    OR_76.bQ = START_NET_COILS * 0x10 + 0x1B;    // Норма или жарко

	    OR_77.bIN1 = COIL_T_NORM;
	    OR_77.bIN2 = COIL_T_LOW;
	    OR_77.bQ = START_NET_COILS * 0x10 + 0x1C;    // Норма или холодно

	    AND_78.bIN1 = START_NET_COILS * 0x10 + 0x0E;
	    AND_78.bIN2 = START_NET_COILS * 0x10 + 0x1B;
	    AND_78.bQ = START_NET_COILS * 0x10 + 0x1D;

	    AND_79.bIN1 = START_NET_COILS * 0x10 + 0x0C;
	    AND_79.bIN2 = START_NET_COILS * 0x10 + 0x1C;
	    AND_79.bQ = START_NET_COILS * 0x10 + 0x1E;

	    OR_80.bIN1 = START_NET_COILS * 0x10 + 0x0D;
	    OR_80.bIN2 = START_NET_COILS * 0x10 + 0x1D;
	    OR_80.bIN3 = START_NET_COILS * 0x10 + 0x1E;
	    OR_80.bIN4 = START_REG_CONST_32*0x10+0;
	    OR_80.bIN5 = START_REG_CONST_32*0x10+0;
	    OR_80.bIN6 = START_REG_CONST_32*0x10+0;
	    OR_80.bIN7 = START_REG_CONST_32*0x10+0;
	    OR_80.bIN8 = START_REG_CONST_32*0x10+0;
	    OR_80.bQ = START_NET_COILS * 0x10 + 0x1F;


  }  // end HVAC4

//----------------------------------------------------------------------------------------------------------
  // определение констант
//	  CLR_COIL(START_REG_CONST_32*0x10);
//	  SET_COIL(START_REG_CONST_32*0x10+1);
//	  MODBUS_REG(START_REG_CONST_32+1) = 0;
//	  MODBUS_REG(START_REG_CONST_32+2) = 100;
//	  MODBUS_REG(START_REG_CONST_32+3) = 30;
//	  MODBUS_REG(START_REG_CONST_32+4) = 1;
//	  MODBUS_REG(START_REG_CONST_32+5) = 2;
//	  MODBUS_REG(START_REG_CONST_32+6) = 20;
//	  MODBUS_REG(START_REG_CONST_32+7) = 60;
//	  MODBUS_REG(START_REG_CONST_32+8) = 0x00ff;
//	  MODBUS_REG(START_REG_CONST_32+9) = 0xff00;
//	  MODBUS_REG(START_REG_CONST_32+0x0a) = 0x000f;
//	  MODBUS_REG(START_REG_CONST_32+0x0b) = 0x00f0;
//	  MODBUS_REG(START_REG_CONST_32+0x0c) = 8;
//	  MODBUS_REG(START_REG_CONST_32+0x0d) = 4;
//	  MODBUS_REG(START_REG_CONST_32+0x0e) = 6;
//	  MODBUS_REG(START_REG_CONST_32+0x0f) = 180;


  // общие структуры
//	  VENT_2 AND_3;   //-
//	  VENT_2 AND_4;  //-
//	  sDelay DEL_5;   //-
//	  sDelay DEL_6;   //-
//	  VENT_2 XOR_7;  //-
//	  VENT_2 XOR_10;  //-
//	  sDelay DEL_8;   //-
//	  sDelay DEL_11;  //-
//	  RS RS_9;           //-
//	  RS RS_12;          //-
//	  sDelay DEL_13;     //-
//	  RS RS_14;          //-
//	  VENT_1 NOT_15;     //-
//	  sVent_Analog_2 MIN_16;     //-
//	  sComparator_2 CMP_17;  //-
//	  RS RS_18;           //-
//	  sDelta_2 DELTA_19;     //-
//	  sDelay DEL_20;         //-
//	  RS RS_21;              //-
//	  sDelta_2 DELTA_22;     //-
//	  sDelay DEL_23;         //-
//	  RS RS_24;               //-
//	  sRegulator_1 REGUL_26;          //-
//	  sVent_Analog_2 MIN_27;
//	  VENT_2 AND_28;                  //-
//	  sRegulator_1 REGUL_29;          //-
//	  VENT_2 OR_30;                     //
//	  sSwitcher SWT_31;                //-
//	  sRegulator_1 REGUL_32;           //
//	  sMUX_4 MUX_33;                   //-
//	  VENT_2 NOR_34;
//	  VENT_2 AND_35;                   //-
//	  sSwitcher SWT_36;                //-
//
//	  sRegulator_step REGUL_STEP_37;          //
//	  sRegulator_step REGUL_STEP_38;          //
//	  sCounter_2IN COUNTER_2IN_39;          //
//	  sVent_Analog_2 EQ_40;
//	  sMUX_4 MUX_41;          //
//
//	  sMUX_4 MAX_42; //
//	  sDeCoder_4 DECODER_43          //-

//    sPWM  PWM_44;                  //-
//    sPWM  PWM_45;                  //-
  //      sEquation EQUATION_46;     //-
  //   sMUX_dig_4 MUX_DIG_47;        //-
//  sDelta_2 DELTA_48;               //-
//  sDelay DEL_49;                   //-
//  RS RS_50;                        //-
//	  VENT_2 AND_51;                 //-
//	  sRegulator_1 REGUL_52;         //-
//	  VENT_2 AND_53;                 //-
// VENT_2 AND_68;                   // -

  // CHECK_OK init HVAC5
  if(MODBUS_REG(0x49) == HVAC5)
  {
	  // HVAC5

	 // ошибка датчиков Modbus
	NAND_Modbus.bIN1 = REG_POINT_MODBUS_OK * 0x10 + 0;
	NAND_Modbus.bIN2 = REG_POINT_MODBUS_OK * 0x10 + 1;
	NAND_Modbus.bIN3 = REG_POINT_MODBUS_OK * 0x10 + 2;
	NAND_Modbus.bIN4 = REG_POINT_MODBUS_OK * 0x10 + 3;
	NAND_Modbus.bIN5 = REG_POINT_MODBUS_OK * 0x10 + 4;
	NAND_Modbus.bIN6 = START_REG_CONST_32*0x10+1;
	NAND_Modbus.bIN7 = START_REG_CONST_32*0x10+1;
	NAND_Modbus.bIN8 = START_REG_CONST_32*0x10+1;
	NAND_Modbus.bQ = REG_ALARM * 0x10 +8;

	  // �?М1, �?М2


	  AND_3.bIN1 = START_NET_COILS  * 0x10+2; //ПУСК
	  AND_3.bIN2 = START_NET_COILS  * 0x10 + 0x1FF;  //Ошибки нет  //START_NET_COILS  * 0x10 +0x10; // рекуператор Норма
	  AND_3.bQ =  START_NET_COILS  * 0x10 + 0x20;

	  DEL_71.bIN = START_NET_COILS  * 0x10 + 0x20; //
	  DEL_71.delay = 400;  // 40 s
	  DEL_71.bOUT = START_NET_COILS  * 0x10 + 0x21; //

	  DEL_72.bIN = START_NET_COILS  * 0x10 + 0x21; //
	  DEL_72.delay = 400;  // 40 s
	  DEL_72.bOUT = START_NET_COILS  * 0x10 + 0x22; // Запустить П1

	  DEL_81.bIN = START_NET_COILS  * 0x10 + 0x20; //
	  DEL_81.delay = 400;  // 40 s
	  DEL_81.bOUT = START_NET_COILS  * 0x10 + 0x23; //  Открыть Y1

	  CMP_57.IN1 =   START_REG_AI + 2;     // OC_Y1
	  CMP_57.IN2 = 	START_REG_CONST_32+7;  // 60%
	  CMP_57.bOut = REG_DI * 0x10 + 4;     // Y1 открылся > чем на 60%

	  AND_51.bIN1 = START_NET_COILS  * 0x10 + 0x20; // Сигнал на запуск П1
	  AND_51.bIN2 = REG_DI * 0x10 + 0;              // П1 в работе
	  AND_51.bQ = START_NET_COILS  * 0x10 + 0x1B;

	  DECODER_43.IN1 = REG_MODE;
	  DECODER_43.OUT1 = START_NET_COILS  * 0x10 + 0x2C;
	  DECODER_43.OUT2 = START_NET_COILS  * 0x10 + 0x2D;
	  DECODER_43.OUT3 = START_NET_COILS  * 0x10 + 0x2E;
	  DECODER_43.OUT4 = START_NET_COILS  * 0x10 + 0x2F;

	  AND_68.bIN1 = START_NET_COILS  * 0x10 + 0x2F;
	  AND_68.bIN2 = START_NET_COILS  * 0x10 + 0x1B;
	  AND_68.bQ = START_NET_COILS  * 0x10 + 0x1D;

	  AND_69.bIN1 = START_NET_COILS  * 0x10 + 0x1D;
	  AND_69.bIN2 = START_NET_COILS  * 0x10 + 0x1FF;  //Ошибки нет
	  AND_69.bQ = REG_DO * 0x10 + 0;  // Контактор ТЭН

	  // открыть Y1
	  SWT_65.IN1 = START_REG_CONST_32 + 0x02;   // 100%
	  SWT_65.IN2 = START_REG_CONST_32 + 0x01;   // 0%
	  SWT_65.bOn =  START_NET_COILS  * 0x10 + 0x23; //  Открыть Y1
	  SWT_65.Out = START_NET_REG + 0x11;           // Управление Y1

	  AND_4.bIN1 = START_NET_COILS  * 0x10+0x23; //  Открыть Y1
	  AND_4.bIN2 = START_NET_COILS  * 0x10 + 0x1FF;  //Ошибки нет //START_REG_CONST_32*0x10+1; // TRUE
	  AND_4.bQ = REG_DO * 0x10 +1;   // DO1 Y2 - открыть

	  // П1, В1
	  AND_35.bIN1 = START_NET_COILS  * 0x10 + 0x22; // Запустить П1
	  AND_35.bIN2 = START_NET_COILS  * 0x10 +0x10;  // Рекуператор в норме
	  AND_35.bQ = REG_DO * 0x10 + 2; // Пуск_П1 = 1;

//	  DEL_5.bIN = REG_DO * 0x10 +0;
//	  DEL_5.bOUT = START_NET_COILS  * 0x10 +0x11;
//	  DEL_5.delay = 200;

	  DEL_6.bIN = START_NET_COILS  * 0x10 + 0x22; // Запустить П1
	  DEL_6.bOUT = REG_DO * 0x10 +3;   // Пуск_В1
	  DEL_6.delay = 30;

	  // Авария П1, В1
	  XOR_7.bIN1 = REG_DO * 0x10 +2;
	  XOR_7.bIN2 = REG_DI * 0x10 + 0;
	  XOR_7.bQ = START_NET_COILS  * 0x10+3;

	  XOR_10.bIN1 = REG_DO * 0x10 +3;
	  XOR_10.bIN2 = REG_DI * 0x10 +1;
	  XOR_10.bQ = START_NET_COILS  * 0x10+5;

	  DEL_8.bIN = START_NET_COILS  * 0x10+3;
	  DEL_8.bOUT = START_NET_COILS  * 0x10+4;
	  DEL_8.delay = 100; // 10 s

	  DEL_11.bIN = START_NET_COILS  * 0x10+5;
	  DEL_11.bOUT = START_NET_COILS  * 0x10+6;
	  DEL_11.delay = 100; // 10s

	  RS_9.bSet = START_NET_COILS  * 0x10+4;
	  RS_9.bReset = RESET_BUTTON;
	  RS_9.bQ = REG_ALARM * 0x10 +1;

	  RS_12.bSet = START_NET_COILS  * 0x10+6;
	  RS_12.bReset = RESET_BUTTON;
	  RS_12.bQ = REG_ALARM * 0x10 +2;


	  // Рекуператор авария
	  NOT_15.bIN = REG_DI * 0x10 +3;
	  NOT_15.bQ = START_NET_COILS  * 0x10+7;

	  DEL_13.bIN = START_NET_COILS  * 0x10+7;
	  DEL_13.bOUT = START_NET_COILS  * 0x10+8;
	  DEL_13.delay = 20; // 2 s

	  RS_14.bSet = START_NET_COILS  * 0x10+8;
	  RS_14.bReset = RESET_BUTTON;
	  RS_14.bQ = REG_ALARM * 0x10 +3;

	  // угроза заморозки рекуператора

	  MIN_16.IN1 = 0x00;  // T1
	  MIN_16.IN2 = 0x03;  // T4
	  MIN_16.Out = START_NET_REG +8;

	  // сравнение тем-ры с минимальной

	  CMP_17.IN1 = START_REG_CONST_32+0x0d;  // 4 град
	  CMP_17.IN2 = START_NET_REG +8;
	  CMP_17.bOut = START_NET_COILS  * 0x10+9;

	  // фиксация ошибки
	  RS_18.bSet = START_NET_COILS  * 0x10+9;
	  RS_18.bReset = RESET_BUTTON;
	  RS_18.bQ = REG_ALARM * 0x10 +4;   // угроза заморозки рекуператора

	  // рекуператор в норме?
	  NOR_34.bIN1 = 0x1F * 0x10 +3;
	  NOR_34.bIN2 = 0x1F * 0x10 +4;
	  NOR_34.bQ = START_NET_COILS  * 0x10 +0x10; // рекуператор в норме = 1;


	  // ошибка привода Y1
	  DELTA_19.IN1 = START_REG_AO + 2;
	  DELTA_19.IN2 = START_REG_AI + 2;
	  DELTA_19.delta = 30; // разница 30%
	  DELTA_19.bOut = START_NET_COILS  * 0x10+0x0A;


	  DEL_20.bIN = START_NET_COILS  * 0x10+0x0a;
	  DEL_20.bOUT = START_NET_COILS  * 0x10+0x0b;
	  DEL_20.delay = 600; // 60 s


	  RS_21.bSet = START_NET_COILS  * 0x10+0x0b;
	  RS_21.bReset = RESET_BUTTON;
	  RS_21.bQ = REG_ALARM * 0x10 +5;

	  // ошибка привода Y4

	  DELTA_22.IN1 = START_REG_AO + 1;
	  DELTA_22.IN2 = START_REG_AI + 1;
	  DELTA_22.delta = 30; // разница 30%
	  DELTA_22.bOut = START_NET_COILS  * 0x10+0x0c;


	  DEL_23.bIN = START_NET_COILS  * 0x10+0x0c;
	  DEL_23.bOUT = START_NET_COILS  * 0x10+0x0d;
	  DEL_23.delay = 600; // 60 s


	  RS_24.bSet = START_NET_COILS  * 0x10+0x0d;
	  RS_24.bReset = RESET_BUTTON;
	  RS_24.bQ = REG_ALARM * 0x10 +6;


	  // ТЭН
	  // Температура в канале
	  REGUL_26.SetPoint = START_REG_SETPOINTS + 0x00;
	  REGUL_26.Threshold = START_REG_SETPOINTS + 0x01;
	  REGUL_26.Time = START_REG_SETPOINTS + 0x02; // 4 sec

	  REGUL_26.Max = 45;
	  REGUL_26.Min = 12;

	  REGUL_26.Reverce = START_REG_CONST_32 * 0x10 + 1;  // Reverce = 1
	  REGUL_26.Enable =  START_NET_COILS  * 0x10 + 0x2F;  // включить регулятор
	  REGUL_26.Reset =   START_NET_COILS  * 0x10 + 0x1B;  //  "Запустить П1"  и "П1  в работе"

	  REGUL_26.Y_OC = 0x02;  // сигнал с  датчика  Т3
	  REGUL_26.Y = START_NET_REG + 0x0A;  // задаваемая температура в канале;

	  // ТЭН  управление
	  REGUL_66.SetPoint = START_NET_REG + 0x0A;  // задаваемая температура в канале;
	  REGUL_66.Threshold = START_REG_SETPOINTS + 0x01;
	  REGUL_66.Time = START_REG_SETPOINTS + 0x02; // 4 sec

	  REGUL_66.Max = 100;
	  REGUL_66.Min = 0;

	  REGUL_66.Reverce = START_REG_CONST_32 * 0x10 + 1;  // Reverce = 1
	  REGUL_66.Enable =  START_NET_COILS  * 0x10 + 0x2F;  // включить регулятор
	  REGUL_66.Reset =   START_NET_COILS  * 0x10 + 0x1B;  //  "Запустить П1"  и "П1  в работе"

	  REGUL_66.Y_OC = 0x01;  // сигнал с  датчика  Т2
	  REGUL_66.Y = START_NET_REG + 0x10;  // управление ТЭН Auto //0x30;

	  // вычисление минимальной температуры после рекуператора

	  MIN_27.IN1 = 0x00; // T1
	  MIN_27.IN2 = 0x03; // T4
	  MIN_27.Out = START_NET_REG + 0x09;

	  // выявление режима "З�?МА"

	  AND_28.bIN1 = REG_MODE  * 0x10+0;
	  AND_28.bIN2 = REG_MODE  * 0x10+1;
	  AND_28.bQ = START_NET_COILS  * 0x10 +0x0E;


	  // Y4 управление зима

	  REGUL_29.SetPoint = START_REG_CONST_32+0x0e;     // 6 град
	  REGUL_29.Threshold = START_REG_CONST_32+4; // 1
	  REGUL_29.Time = START_REG_CONST_32+6; // 2 sec

	  REGUL_29.Max = 100;
	  REGUL_29.Min = 0;

	  REGUL_29.Reverce = START_REG_CONST_32*0x10+1;  // 1 Reverce
	  REGUL_29.Enable =  START_NET_COILS  * 0x10 +2;  // включить регулятор
	  REGUL_29.Reset =   START_NET_COILS  * 0x10 +0x0E;  // reset регулятор 0 - регулятор выключен зима = 1 регулятор включен

	  REGUL_29.Y_OC = START_NET_REG +0x09; // минимальная тем-ра после рекуператора
	  REGUL_29.Y = START_NET_REG + 0x0B;  // управление Y4 Auto //0x31;

	  // ручн/авто

	  OR_30.bIN1 = REG_MODE * 0x10 +0;
	  OR_30.bIN2 = REG_MODE * 0x10 +1;
	  OR_30.bQ = START_NET_COILS  * 0x10 +0x0F; // ручн = 0; авто = 1




	  // Y4 управление переходный
	  REGUL_32.SetPoint = START_REG_SETPOINTS + 0x00; //0x20 ;
	  REGUL_32.Threshold = START_REG_SETPOINTS + 0x01; //0x21;
	  REGUL_32.Time = START_REG_SETPOINTS + 0x04; //0x24; // 4 sec

	  REGUL_32.Max = 100;
	  REGUL_32.Min = 0;

	  REGUL_32.Reverce = START_REG_CONST_32*0x10 + 0;  // 0 Direct
	  REGUL_32.Enable =  START_NET_COILS  * 0x10 +0x2E;  // Переходный
	  REGUL_32.Reset =   REG_DI * 0x10 + 0;  //  П1 в работе

	  REGUL_32.Y_OC = 0x02;  // сигнал с  датчика  Т3
	  REGUL_32.Y = START_NET_REG + 0x0C;  // управление Y4 переходн //0x31;

	  // Y4 выбор сигнала управления
	  MUX_33.IN1 = 0x51; // AO1
	  MUX_33.IN2 = START_REG_CONST_32 + 2; //Лето 100%
	  MUX_33.IN3 = START_NET_REG + 0x0C; // Y4_упр_переход
	  MUX_33.IN4 = START_REG_CONST_32 + 1; // Зима 0%   //START_NET_REG + 0x0B; // Y4_упр_зима
	  MUX_33.ADR = 0x48; // режим руч = 0, лето = 1, переход = 2, зима = 3
	  MUX_33.Out = 0x31;  // управление Y4

	  // ТЭН выбор сигнала управления
	  MUX_41.IN1 = 0x50; // AO1
	  MUX_41.IN2 = START_REG_CONST_32 + 1; //Лето 0%
	  MUX_41.IN3 = START_REG_CONST_32 + 1; //Переход 0%
	  MUX_41.IN4 = START_NET_REG + 0x10;  // управление ТЭН Auto
	  MUX_41.ADR = 0x48; // режим руч = 0, лето = 1, переход = 2, зима = 3
	  MUX_41.Out = 0x30;  // управление ТЭН


	  // Y1 выбор сигнала управления
	  MUX_82.IN1 = 0x52; // AO1
	  MUX_82.IN2 = START_NET_REG + 0x11; //Лето авто
	  MUX_82.IN3 = START_NET_REG + 0x11; //Переход авто
	  MUX_82.IN4 = START_NET_REG + 0x11; //Зима авто
	  MUX_82.ADR = 0x48; // режим руч = 0, лето = 1, переход = 2, зима = 3
	  MUX_82.Out = 0x32;  // управление Y1



  }

  //----------------------------------------------------------------------


  // определение констант
//  CLR_COIL(START_REG_CONST_32*0x10 + 0);
//  SET_COIL(START_REG_CONST_32*0x10+1);
//  MODBUS_REG(START_REG_CONST_32+1) = 0;
//  MODBUS_REG(START_REG_CONST_32+2) = 100;
//  MODBUS_REG(START_REG_CONST_32+3) = 30;
//  MODBUS_REG(START_REG_CONST_32+4) = 1;
//  MODBUS_REG(START_REG_CONST_32+5) = 2;
//  MODBUS_REG(START_REG_CONST_32+6) = 20;
//  MODBUS_REG(START_REG_CONST_32+7) = 60;
//  MODBUS_REG(START_REG_CONST_32+8) = 0x00ff;
//  MODBUS_REG(START_REG_CONST_32+9) = 0xff00;
//  MODBUS_REG(START_REG_CONST_32+0x0a) = 0x000f;
//  MODBUS_REG(START_REG_CONST_32+0x0b) = 0x00f0;
//  MODBUS_REG(START_REG_CONST_32+0x0c) = 8;
//  MODBUS_REG(START_REG_CONST_32+0x0d) = 4;
//  MODBUS_REG(START_REG_CONST_32+0x0e) = 6;

  /* Infinite loop */
  for(;;)
  {
//	  GPIOB->BSRR = GPIO_PIN_15;

      // устанавливаем бит runAlgoritm задача работает
      uRunningTask |= runAlgoritm;

	  // преобразование считанных данных ModBus регистров
	  for(uint8_t i=0; i< modbusHVAC1.num ;i++)
	  {
		 // modbusHVAC1.modbusPoint[i].
		  MODBUS_REG(i) = MODBUS_REG(i + START_REG_MODBUS) / modbusHVAC1.modbusPoint[i].div;
	  }

	  	//======================================================
	  	// Управления реле
	  	//======================================================
	  	if(!(MODBUS_REG(0x2F)  & 0x01))
	  	{
	  	 //
	       GPIOC->BSRR = (uint32_t)GPIO_PIN_4 << 16U;
	  	}
	  	else
	  	{
	  	 GPIOC->BSRR = GPIO_PIN_4;
	  	}
	  	if(!(MODBUS_REG(0x2F)  & 0x02))
	  	{
	  	 //
	   	 GPIOC->BSRR = (uint32_t)GPIO_PIN_5 << 16U;
	  	}
	  	else
	  	{
	  	GPIOC->BSRR = GPIO_PIN_5;
	  	}

	  	if(!(MODBUS_REG(0x2F)  & 0x04))
	  	{
	  	 //
	   	 GPIOB->BSRR = (uint32_t)GPIO_PIN_12 << 16U;
	  	}
	  	else
	  	{
	  	GPIOB->BSRR = GPIO_PIN_12;
	  	}

	  	if(!(MODBUS_REG(0x2F)  & 0x08))
	  	{
	  	 //
	   	 GPIOA->BSRR = (uint32_t)GPIO_PIN_5 << 16U;
	  	}
	  	else
	  	{
	  	GPIOA->BSRR = GPIO_PIN_5;
	  	}
	  	//======================================================
	  	// Кнец управления реле
	  	//======================================================

	  	//======================================================
	  	// TIM4 PWM
	  	//======================================================
	      TIM4->CCR3 = MODBUS_REG(0x31);
	      TIM4->CCR4 = MODBUS_REG(0x30);
	      TIM4->CCR1 = MODBUS_REG(0x32);
	      TIM4->CCR2 = MODBUS_REG(0x33);
	  	//======================================================
	  	// End TIM4 PWM
	  	//======================================================


	  	//-----------------------------------------------------------------------
	  	// Конец ручного ржима
	  	//-----------------------------------------------------------------------

	  	//======================================================
	  	// DI
	  	//======================================================
	      if(!((GPIOB->IDR) & GPIO_PIN_11))
	      {
	      	MODBUS_REG(REG_DI) |= 0x0001;
	      }
	      else
	      {
	      	MODBUS_REG(REG_DI) &= ~((uint16_t)0x0001);
	      }

	      if(!((GPIOB->IDR) & GPIO_PIN_10))
	      {
	      	MODBUS_REG(REG_DI) |= 0x0002;
	      }
	      else
	      {
	      	MODBUS_REG(REG_DI) &= ~((uint16_t)0x0002);
	      }

	      if(!((GPIOC->IDR) & GPIO_PIN_6))
	      {
	      	MODBUS_REG(REG_DI) |= 0x0004;
	      }
	      else
	      {
	      	MODBUS_REG(REG_DI) &= ~((uint16_t)0x0004);
	      }

	      if(!((GPIOC->IDR) & GPIO_PIN_7))
	      {
	      	MODBUS_REG(REG_DI) |= 0x0008;
	      }
	      else
	      {
	      	MODBUS_REG(REG_DI) &= ~((uint16_t)0x0008);
	      }
	  	//======================================================
	  	// End DI
	  	//======================================================

	  	//======================================================
	  	// AI ADC1
	  	//======================================================

	       ADC1->CR2 |= ADC_CR2_JSWSTART;
	       while(!(ADC1->SR & ADC_SR_JEOC));

	      uint16_t AI4 = (((float)(ADC1->JDR1))*100)/4196;
	      uint16_t AI3 = (((float)(ADC1->JDR2))*100)/4196;
	      uint16_t AI2 = (((float)(ADC1->JDR3))*100)/4196;
	      uint16_t AI1 = (((float)(ADC1->JDR4))*100)/4196;

	      MODBUS_REG(START_REG_AI + 0)=AI2;
	      MODBUS_REG(START_REG_AI + 1)=AI1;
	      MODBUS_REG(START_REG_AI + 2)=AI3;
	      MODBUS_REG(START_REG_AI + 3)=AI4;
//	      MODBUS_REG(0x12)=AI4;
//	      MODBUS_REG(0x13)=AI3;
	  	//======================================================
	  	// End AI  End ADC1
	  	//======================================================

//	      GPIOB->BSRR = (uint32_t)GPIO_PIN_15 << 16U;   // конец работы алгоритма

	    //===================================================================
	    //  Общая части алгоритма
	    //  Календарь
	    //===================================================================


        Bits_AND(&START_AND);
        Shift_Right(&START_SHIFR_R);
        Mult(&START_MULT);
        Add(&START_ADD);

        Bits_AND(&STOP_AND);
        Shift_Right(&STOP_SHIFR_R);
        Mult(&STOP_MULT);
        Add(&STOP_ADD);

        // Scheduler
        Scheduler(&SCHEDULER);



        // общая ошибка
        OR8(&OR_25);       // ошибка в системе
        NOT(&NOT_ALARM);   // ошибки нет
        // стоп/пуск/авто
        AND2(&AND_1);
        OR2(&OR_2);

        // выбор датчика для обратной связи
        Cmp(&CMP_SP);
        Switcher(&SWT_OC);
        Switcher(&SWT_SP);

        // конец общей части алгоритма
        //===============================================

        // CHECK_OK work HVAC2
        //===============================================
        // алгоритм HVAC2
        //===============================================
        if(MODBUS_REG(0x49) == HVAC2)
        {
          // ошибка датчиков
          NAND8(&NAND_Modbus);


        // �?М1, �?М2 открыть
        AND2(&AND_3);
        AND2(&AND_4);
        Delay_on(&DEL_72);

        // П1, В1 включить
	    Delay_on(&DEL_5);
	    Delay_on(&DEL_6);

	    // Авария П1, В1
	    XOR2(&XOR_7);
	    Delay_on(&DEL_8);
	    RS_Triger(&RS_9);

	    XOR2(&XOR_10);
	    Delay_on(&DEL_11);
	    RS_Triger(&RS_12);

	    // Рекуператор авария
	    NOT(&NOT_15);
	    Delay_on(&DEL_13);
	    RS_Triger(&RS_14);

	    // угроза заморозки рекуператора
        Min(&MIN_16);  // минимальная температура на приетоке и вутяжке
        Cmp(&CMP_17);  // сравнение  минимальной тем-ры в рекуператоре с 6 град цельсия
        RS_Triger(&RS_18);

        // авария привода Y3
        Delta(&DELTA_19);
        Delay_on(&DEL_20);
        RS_Triger(&RS_21);

        // авария привода Y4
        Delta(&DELTA_22);
        Delay_on(&DEL_23);
        RS_Triger(&RS_24);

//        // общая ошибка
//        OR8(&OR_25);

        // Y3 управление
        // пропорциональное регулирование с задержкой
        Regulator_1(&REGUL_26);

        Min(&MIN_27);  // минимальная на выходе рекуператора
        AND2(&AND_28);   // определение режима "З�?МА"

        // Y4 управление байпасом  зима
        // пропорциональное регулирование с задержкой
        Regulator_2(&REGUL_29);

        // ручн/авто
        OR2(&OR_30);

        // Y3 выбор управления руч/авто
        Switcher(&SWT_31);

        // Y4 управление байпасом переходный режим
        // пропорциональное регулирование с задержкой
        Regulator_3(&REGUL_32);

        // Y4 выбор управления руч/лето/переход/зима
        Mux_4(&MUX_33);

        // рекуператор в норме
        NOR2(&NOR_34);

        // Пуск_П1
        AND2(&AND_35);

        // Открыти Y3 на 100% при низкой температуре перед теплообменником или прблемме с рекуператором
        Switcher(&SWT_36);

        // Ручное задание аналогового выхода
        MODBUS_REG(0x32) = MODBUS_REG(0x52);
        MODBUS_REG(0x33) = MODBUS_REG(0x53);
        }  //  end алгоритм HVAC2

        //===============================================
        //  end алгоритм HVAC2
        //===============================================

        //===============================================
        //  Start алгоритм HVAC1
        //===============================================
        if(MODBUS_REG(0x49) == HVAC1)
        {

            // ошибка датчиков
            NAND8(&NAND_Modbus);
            //Delay_on(&DEL_72);

        	//----------------------------------------- лист 2

        	Cmp(&CMP_17);
        	RS_Triger(&RS_9);

        	NOT(&NOT_15);
        	AND2(&AND_3);
        	OR2(&OR_30);
        	AND2(&AND_4);

        	DeCoder_4(&DECODER_43);

        	//----------------------------------------- лист 3

           	Regulator_1(&REGUL_26);
            Mux_4(&MUX_33);

        	Equation_2_piont(&EQUATION_46);

        	//----------------------------------------- лист 4

    	    // Авария M1
    	    XOR2(&XOR_7);
    	    Delay_on(&DEL_8);
    	    RS_Triger(&RS_12);

            // авария привода Y1
            Delta(&DELTA_19);
            Delay_on(&DEL_20);
            RS_Triger(&RS_21);


            // Ручное задание аналогового выхода
            MODBUS_REG(0x31) = MODBUS_REG(0x51);
            MODBUS_REG(0x32) = MODBUS_REG(0x52);
            MODBUS_REG(0x33) = MODBUS_REG(0x53);


        }

        //===============================================
        //  end алгоритм HVAC1
        //===============================================

        if(MODBUS_REG(0x49) == HVAC3)
        {
            // ошибка датчиков
            NAND8(&NAND_Modbus);
            //Delay_on(&DEL_72);
        	//----------------------------------------- лист 2
            // П1 включить
    	    Delay_on(&DEL_5);
            AND2(&AND_3);

    	    // Авария П1
    	    XOR2(&XOR_7);
    	    Delay_on(&DEL_8);
    	    RS_Triger(&RS_9);

            // авария привода Y1
            Delta(&DELTA_19);
            Delay_on(&DEL_20);
            RS_Triger(&RS_21);

            // авария привода Y3
            Delta(&DELTA_22);
            Delay_on(&DEL_23);
            RS_Triger(&RS_24);

            Cmp(&CMP_17);  // сравнение  тем-ры перед теплообменника  с 4 град цельсия

        	//----------------------------------------- лист 3

            // Y3 управление
            // пропорциональное регулирование с задержкой
            Regulator_1(&REGUL_26);

            // Y1 управление
            // пропорциональное регулирование с задержкой
            Switcher(&SWT_74);
            Switcher(&SWT_75);
    	    NOT(&NOT_15);
            Regulator_2(&REGUL_29);
            Regulator_5(&REGUL_73);

            // руч/лето/переход/зима
            DeCoder_4(&DECODER_43);

            // температура в норме
            Delta(&DELTA_48);
    	    NOT(&NOT_56);
            Delay_on(&DEL_49);

            Cmp(&CMP_57);  // тем-ра перед теплообменником  выше 12 град цельсия ?
            Cmp(&CMP_58);  // Концентрация СО2 превысила уставку ?

            AND2(&AND_4);
            AND2(&AND_28);

 //           Counter_2IN(&COUNTER_2IN_39);

            Add(&ADD_59);
            Limiter(&LIM_60);

            // сравнение уставок с нормой
            Cmp_3(&CMP3_62);
            Cmp_3(&CMP3_61);
            OR2(&OR_30);
        	//----------------------------------------- лист 4

            // регулирование Y1 по температуре
//            Regulator_Step(&REGUL_STEP_37);

            // регулирование Y1 по СО2
//             Regulator_Step(&REGUL_STEP_38);

             // счетчик сигналов
//             Counter_2IN(&COUNTER_2IN_39);

             // 1 значит регулировки клапаном Y1 по температуре нет
//             Equal(&EQ_40);

             // выбор источника управления Y1
             Mux_4(&MUX_33);

             // выбор источника управления Y3
             Mux_4(&MUX_41);

             Cmp(&CMP_63);  // сравниваем Y3_OC с 8 %

             // Если Т1 < 15 град Y1 закрывать
             Cmp(&CMP_64);
             Switcher(&SWT_36);

             // переключение клапанов при аварии
             Switcher(&SWT_31);     // при аварии приточный клапан закрыть
             Switcher(&SWT_65);     // при аварии водяной клапан открыть

             AND2(&AND_35);     // фанкойл в работе и без аварии. Разрешение управлять клапанами


             // Ручное задание аналогового выхода
             MODBUS_REG(0x32) = MODBUS_REG(0x52);
             MODBUS_REG(0x33) = MODBUS_REG(0x53);

             // Разрешение управлять клапаном Y1
             OR2(&OR_76);
             OR2(&OR_77);
             AND2(&AND_78);
             AND2(&AND_79);

             OR8(&OR_80);

        }  // end HVAC3

        //===============================================
        //  end алгоритм HVAC3
        //===============================================

        // VERIFIED HVAC4
        if(MODBUS_REG(0x49) == HVAC4)
        {
            // ошибка датчиков
            NAND8(&NAND_Modbus);
           // Delay_on(&DEL_72);

        	//----------------------------------------- лист 2
            // П1 включить
    	    Delay_on(&DEL_5);
            AND2(&AND_3);
            Delay_off(&DEL_6);

            // Включить контактор
            AND2(&AND_51);
            NOT(&NOT_67);
            AND2(&AND_68);
            AND2(&AND_69);

    	    // Авария П1
    	    XOR2(&XOR_7);
    	    Delay_on(&DEL_8);
    	    RS_Triger(&RS_9);

            // авария привода Y1
            Delta(&DELTA_19);
            Delay_on(&DEL_20);
            RS_Triger(&RS_21);

            // авария привода Y3
//            Delta(&DELTA_22);
//            Delay_on(&DEL_23);
//            RS_Triger(&RS_24);

            Cmp(&CMP_17);  // сравнение  тем-ры перед теплообменника  с 4 град цельсия

        	//----------------------------------------- лист 3

            // ТЭН управление
            // пропорциональное регулирование с задержкой
            Regulator_3(&REGUL_26);
            Regulator_4(&REGUL_66);

            // Y1 управление
            // пропорциональное регулирование с задержкой
    	    NOT(&NOT_15);
//            Regulator_1(&REGUL_29);
//            Regulator_2(&REGUL_32);

            // Y1 управление
            // пропорциональное регулирование с задержкой
            Switcher(&SWT_74);  // +
            Switcher(&SWT_75);  // +
    	    NOT(&NOT_15);
            Regulator_2(&REGUL_29);  // +
            Regulator_5(&REGUL_73);  // +

//            Delay_on(&DEL_71);
//            Switcher(&SWT_70);
            // руч/лето/переход/зима
            DeCoder_4(&DECODER_43); // +

            // температура в норме
            Delta(&DELTA_48);   // +
    	    NOT(&NOT_56);       // +
            Delay_on(&DEL_49);  // +

            Cmp(&CMP_57);  // + // тем-ра перед теплообменником  выше 12 град цельсия ?
            Cmp(&CMP_58);  // + // Концентрация СО2 превысила уставку ?

            AND2(&AND_4);
            AND2(&AND_28);

//            Counter_2IN(&COUNTER_2IN_39);

//            Add(&ADD_59);
//            Limiter(&LIM_60);

            // сравнение уставок с нормой
            Cmp_3(&CMP3_61);
            Cmp_3(&CMP3_62);
            OR2(&OR_30);
        	//----------------------------------------- лист 4

            // регулирование Y1 по температуре
//            Regulator_Step(&REGUL_STEP_37);

            // регулирование Y1 по СО2
//             Regulator_Step(&REGUL_STEP_38);

             // счетчик сигналов
//             Counter_2IN(&COUNTER_2IN_39);

             // 1 значит регулировки клапаном Y1 по температуре нет
//             Equal(&EQ_40);

             // выбор источника управления Y1
             Mux_4(&MUX_33);

             // выбор источника управления Y3
             Mux_4(&MUX_41);

 //            Cmp(&CMP_63);  // сравниваем Y3_OC с 8 %

             // Если Т1 < 15 град Y1 закрывать
//             Cmp(&CMP_64);
//             Switcher(&SWT_36);

             // переключение клапанов при аварии
             Switcher(&SWT_31);     // при аварии приточный клапан закрыть
             Switcher(&SWT_65);     // при аварии водяной клапан открыть

             AND2(&AND_35);     // фанкойл в работе и без аварии. Разрешение управлять клапанами


             // Ручное задание аналогового выхода
             MODBUS_REG(0x32) = MODBUS_REG(0x52);
             MODBUS_REG(0x33) = MODBUS_REG(0x53);

             MODBUS_REG(START_NET_REG + 0x13) = MODBUS_REG(0x4B);

             // Разрешение управлять клапаном Y1
             OR2(&OR_76);
             OR2(&OR_77);
             AND2(&AND_78);
             AND2(&AND_79);

             OR8(&OR_80);

        }
        // end HVAC4
        //-----------------------------------------------------------------------------------------------------------------------

       // CHECK_OK work HVACK5
        if(MODBUS_REG(0x49) == HVAC5)
        {
         // ошибка датчиков
         NAND8(&NAND_Modbus);  //+


         // П1 включить
         AND2(&AND_3);       //+
         Delay_on(&DEL_71);  // +
         Delay_off(&DEL_72); //+
         Delay_off(&DEL_81); //+

         // Y1 0 - 10В  открыть
         Switcher(&SWT_65);   // AO2  Y1 - открыть

        // Свормировать сигнал что клапан Y1 открыт больше чем на 60%
         Cmp(&CMP_57);

         // DO0 - включить контактор ТЭНа
         AND2(&AND_51);
         AND2(&AND_68);
         AND2(&AND_69);     // DO0 - Контактор на ТЭН
         AND2(&AND_35);     // DO2 - Пуск П1

         // DO1 - Y2 открыть
         AND2(&AND_4);      // DO1 - Y2- открыть

         // В1 включить
  	    Delay_on(&DEL_6);   // DO3 - Пуск В1

        // руч/лето/переход/зима
        DeCoder_4(&DECODER_43);


  	    // Авария П1, В1
  	    XOR2(&XOR_7);
  	    Delay_on(&DEL_8);
  	    RS_Triger(&RS_9);

  	    XOR2(&XOR_10);
  	    Delay_on(&DEL_11);
  	    RS_Triger(&RS_12);

  	    // Рекуператор авария
  	    NOT(&NOT_15);
  	    Delay_on(&DEL_13);
  	    RS_Triger(&RS_14);

  	    // угроза заморозки рекуператора
          Min(&MIN_16);  // минимальная температура на приетоке и вутяжке
          Cmp(&CMP_17);  // сравнение  минимальной тем-ры в рекуператоре с 6 град цельсия
          RS_Triger(&RS_18);

          // рекуператор в норме
          NOR2(&NOR_34);


          // авария привода Y1
          Delta(&DELTA_19);
          Delay_on(&DEL_20);
          RS_Triger(&RS_21);

          // авария привода Y4
          Delta(&DELTA_22);
          Delay_on(&DEL_23);
          RS_Triger(&RS_24);


          // ТЭН управление
          // Первая ступень пропорциональное регулирования
          Regulator_1(&REGUL_26);

          // Вторая ступень пропорциональное регулирование в канале 12 - 45 градусов
          Regulator_4(&REGUL_66);

          Min(&MIN_27);  // минимальная на выходе рекуператора
          AND2(&AND_28);   // определение режима "З�?МА"

          // Y4 управление байпасом  зима
//          // пропорциональное регулирование с задержкой
//          Regulator_2(&REGUL_29);

          // ручн/авто
          OR2(&OR_30);

          // Y4 управление байпасом переходный режим
          // пропорциональное регулирование с задержкой
          Regulator_3(&REGUL_32);

          // Y4 выбор управления руч/лето/переход/зима
          Mux_4(&MUX_33);

          // ТЭН выбор управления руч/лето/переход/зима
          Mux_4(&MUX_41);

          // Y1 выбор управления руч/лето/переход/зима
          Mux_4(&MUX_82);

        }  // end HVAC5

		//===================================================================
		// Конец тестового алгоритма
		//===================================================================

    osDelay(100);
  }
  /* USER CODE END StartAlgoritm */
}

/* USER CODE BEGIN Header_StartUSART_1 */
/**
* @brief Function implementing the USART_1 thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartUSART_1 */
void StartUSART_1(void const * argument)
{
  /* USER CODE BEGIN StartUSART_1 */
	  // запуск USART1 Slave на прием одного байта по IRQ
	  USART1_Modbus.state = waitFrame;
	  USART1_DE_RX;
	  HAL_UART_Receive_IT(&huart1, &(USART1_Modbus.Temp), 1);
	  USART1_Modbus.Read_Timeout_Byte = 2;  // максимальное время ожидания прихода очередног байта в пакете 38400 - 2 ms  вышло по анализатору 1.2 мс

  /* Infinite loop */
  for(;;)
  {
    if(USART1_Modbus.state == decodeFrame)
    {
  		GPIOB->BSRR = GPIO_PIN_14;
      // декодирование пакета
      //..
//-----------------------------------------------------------------------------------
  			  // пакет нам?
  			  uint16_t crc16 = CRC16(USART1_Modbus.rx_Buff, USART1_Modbus.count_bytes_receive - 2);

  			  if((USART1_Modbus.rx_Buff[0] == S_SLAVE_ADDRESS) && ((crc16 >> 8) == USART1_Modbus.rx_Buff[USART1_Modbus.count_bytes_receive - 2]) &&
  				 ((crc16 & 0xff) == USART1_Modbus.rx_Buff[USART1_Modbus.count_bytes_receive - 1]))
  			  {



  				 // пакет нам
  				 USART1_Modbus.Wait_response = 0;
  				 ModBusDecode_local(USART1_Modbus.rx_Buff,  USART1_Modbus.count_bytes_receive);

  				  crc16 = CRC16(g_txBuf, g_txPush - 2);

  				  g_txBuf[g_txPush - 2] = crc16 >> 8;
  				  g_txBuf[g_txPush - 1] = crc16 & 0xff;

    			  GPIOB->BSRR = GPIO_PIN_15;   ///< маркер отправки пакета
  				  USART1_DE_TX;
     			  HAL_UART_Transmit_IT(&huart1, g_txBuf, g_txPush);

  				  // перешли в режим отправки
  				  USART1_Modbus.state = sendFrame;

  			  }
  			  else
  			  {
  				  // пакет не нам
  				  // переключаемся на прием
  				  USART1_Modbus.state = waitFrame;
  				  USART1_Modbus.Wait_response = 0;
  				  USART1_Modbus.count_bytes_receive = 0;  // обнуляем счетчик принятых байт
  				  USART1_DE_RX;

  			  }
//--------------------------------------------------------------------------------------------

    }  // end decoderFrame


    if(USART1_Modbus.state == sendFrame)
    {
        // отправка пакета
        // ...
        // Включаем передачу (RS485)
    	USART1_DE_TX;

//    	uint8_t data[] = { 0x01, 0x03, 0x02, 0x00, 0x10 };
//    	uint8_t len = sizeof(data);

    	// отправляем пакет
//        HAL_UART_Transmit_IT(&huart1, g_txBuf, g_txPush);

//        // Ждём завершения отправки (TXE + TC)
//        while (!(USART1->SR & USART_SR_TC));

//        USART1_Modbus.state = waitFrame;

    }

    if(USART1_Modbus.state == waitFrame)
    {
        // переключение на  прием
        USART1_DE_RX;
        HAL_UART_Receive_IT(&huart1, &(USART1_Modbus.Temp), 1);  // запускаем ожидание одного байта

  		GPIOB->BSRR = (uint32_t)GPIO_PIN_14 << 16U;
    }


    osDelay(1);
  }
  /* USER CODE END StartUSART_1 */
}

/* ModBus_TimeOut_Fun function */
void ModBus_TimeOut_Fun(void const * argument)
{
  /* USER CODE BEGIN ModBus_TimeOut_Fun */
	//TickType_t xTimeNow;


	/* Получение текущего времени. */
//	USART2_Modbus.Wait_response = xTaskGetTickCount();

//	USART2_Modbus.state = sendFrame;


  /* USER CODE END ModBus_TimeOut_Fun */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

void UpdateValueTimer_Fun(void const * argument)
{
    static uint32_t countTimer = 0;

    countTimer++;
	//g_mode_Menu |= BIT_UPDATE_VALUE;
	portBASE_TYPE queueSendTFT = xQueueSendToFront( queueTimer, &countTimer, 0 );


}

void TaskSwitchedIn(int tag)
{
switch (tag)
{
case 1:
//GPIOB->BSRR = GPIO_PIN_14;//TASK_1_Pin;//GPIO_PIN_10;
break;
case 2:
//GPIOB->BSRR = GPIO_PIN_14;//TASK_2_Pin;//GPIO_PIN_11;
break;
case 3:
//GPIOB->BSRR = GPIO_PIN_15;//TASK_3_Pin;//GPIO_PIN_4;
break;
case 4:
//	GPIOB->BSRR = (uint32_t)GPIO_PIN_15 << 16U;
//GPIOB->BSRR = GPIO_PIN_15;//GPIO_PIN_5;
break;
}
}

void TaskSwitchedOut(int tag)
{
switch (tag)
{
case 1:
//GPIOB->BSRR = (uint32_t)GPIO_PIN_14 << 16U; //GPIO_PIN_15; //(uint32_t)TASK_1_Pin << 16U;
break;
case 2:
//GPIOB->BSRR = (uint32_t)GPIO_PIN_14 << 16U; //(uint32_t)TASK_2_Pin << 16U;
break;
case 3:
//GPIOB->BSRR = (uint32_t)GPIO_PIN_15 << 16U; //(uint32_t)TASK_3_Pin << 16U;
break;
case 4:
//	GPIOB->BSRR = GPIO_PIN_15;
//GPIOB->BSRR = (uint32_t)GPIO_PIN_15 << 16U;//(uint32_t)TASK_4_Pin << 16U;
break;

}
}

/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
