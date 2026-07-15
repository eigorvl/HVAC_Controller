/* USER CODE BEGIN Header */
/**

  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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
 * @mainpage Проект управления HVAC
 *
 * @section intro Введение
 * Проект на STM32f101, использующим FreeRTOS для управления меню и интерфейсом
 * @image html HVAC1_v2.png "Блок-схема HVAC1"<br>
 * @image html HVAC2_v1.png "Блок-схема HVAC2"<br>
 * @image html HVAC3_v1.png "Блок-схема HVAC3"<br>
 * @image html HVAC4_v1.png "Блок-схема HVAC4"<br>
 * @image html HVAC5_v1.png "Блок-схема HVAC5"<br>
 * @section files Структура файлов
 * main.c - точка входа<br>
 * freertos.c - задачи и очереди<br>
 * utils.c - вспомогательные функции для работы с FLASH памятью,парсинг функций ModBus RTU<br>
 * EC11.c - работа с валкодером<br>
 * digital_bloks.c - цифровые компоненты<br>
 * analog_bloks.c - аналоговые компоненты
 *
 *
 *
 */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "EC11.h"
#include "menu.h"
#include "usart.h"
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */
//////////////////////////////////////////extern const unsigned short logo[]; ///< логотип
//extern const unsigned short TP_icon[];
//////////////////////////////////////////extern const unsigned short pipe_H_10x20[];///< изображение трубы горизонтальной
//extern const unsigned short valve_H_45x36[];
//extern const unsigned short valve_H_alarm_45x36[];
//extern const unsigned short heatExchanger_H_40x115[];
//
//
//// RadioButton
//extern const unsigned short circle_26x26[];
//extern const unsigned short circle_check_26x26[];
//extern const unsigned short circle_cursor_26x26[];
//extern const unsigned short circle_check_cursor_26x26[];
//
//// треугольник
//extern const unsigned short alarm_26x26[];
//
//
//extern const unsigned short led_off_26x26[];
//extern const unsigned short led_on_26x26[];
//extern const unsigned short led_off_12x12[];
//extern const unsigned short led_on_12x12[];
//extern const unsigned short led_Alarm_12x12[];
//
//// насос
//extern const unsigned short Pump_H_62x44[];
//
//extern const unsigned short Pipe_Air_1_6x46[];
//
//// фильтры
//extern const unsigned short fan1_46x46[];
//
//
//extern const unsigned short filter_14x46[];
//extern const unsigned short Filt_Alarm_14x46[];
//extern const unsigned short filter_Alarm_14x46[];
//
//// рекуператоры
//extern const unsigned short Recup_2_49x92[];
//extern const unsigned short Recup_2_Alarm_49x92[];
//
//extern const unsigned short Heater_23x46_color[];
//extern const unsigned short TEN_Off_23x46[];
//
//extern const unsigned short WaterPipe_L_22x21[];
//
//extern const unsigned short WaterPipe_R_22x21[];
//
//// стрелки
//extern const unsigned short Right_7x26[];
//extern const unsigned short Left_7x26[];
//extern const unsigned short Down_26x7[];
//extern const unsigned short Up_26x7[];
//
//extern const unsigned short Left_Water_Cold_6x11[];
//extern const unsigned short Right_Water_Cold_6x11[];
//extern const unsigned short Left_Water_Hot_6x11[];
//extern const unsigned short Right_Water_Hot_6x11[];
//
//// воздушные клапаны
//extern const unsigned short Air_Valve_V_15x46[]; // клапан 0-10В
//extern const unsigned short Air_Valve_Alarm_15x46[];
//extern const unsigned short Air_Valve_H_46x15[]; // // клапан 0-10В
//extern const unsigned short Air_Small_15x25[];
//extern const unsigned short Air_Small_Alarm_15x25[];
//extern const unsigned short Air_Valve_H_35x15[];      //  клапан маленький 0-10В
//extern const unsigned short Air_Valve_H_Alarm_35x15[];
//
//
//// клапаны открыт/закрыт
//extern const unsigned short Air_OFF_15x46[];
//extern const unsigned short Air_ON_15x46[];
//
//// водяные клапаны
//extern const unsigned short WaterValve_L_19x19[];
//extern const unsigned short WaterValve_R_19x19[];
//extern const unsigned short WaterValve_Alarm_R_19x19[];
//
//// кнопка
////extern const unsigned short Button_1_49x30[];  ///< малая кнопка
////extern const unsigned short Button_2_49x30[];
////extern const unsigned short Button_3_49x30[];
//
//extern const unsigned short Reset_11_120x24[];  ///< большая кнопка
//extern const unsigned short Reset_21_120x24[];
//extern const unsigned short Reset_31_120x24[];
//
//extern const unsigned short lock_orange_18x26[];
//extern const unsigned short key_18x26[];


//extern USART_ModBus USART2_Modbus;

//extern uint16_t g_UpDate_Count;              // time update data on the screen
//
//extern uint8_t g_mask_Buttons;               // Mask buttons
//
//extern volatile uint32_t g_mode_Menu;                  // Mode Menu
//extern volatile uint32_t g_mode_Menu_old;                  // Mode Menu old
//
//extern uint8_t g_change_Menu;                // if the Menu was changed?

//extern RTC_TimeTypeDef sTime;
//extern RTC_DateTypeDef DateTypeUpdate;
//extern s_Menu menu;                          // struct Menu
//extern volatile statusMenu status_Menu;
//extern uint8_t g_regs[0x3A0];           // глобальный массив регистров
//extern uint8_t g_txBuf[64];  // пакет на передачу
//extern uint8_t g_txPush; // длинна пакета на передачу
//
//extern uint8_t S_SLAVE_ADDRESS; // адрес самого шлюза
//extern uint8_t N_SAVED_REG;  // номер сохраняемого регистра  0xXX 0xXX
//                           //                             Adr  Bound

//extern uint16_t dimForChar[3850];

//extern uint8_t password[4];
//extern uint16_t password_corect; // парооль user
//extern uint16_t password_admin; // парооль admin
//extern uint8_t password_OK;
//extern  uint8_t uRunningTask;   // 1- задача запущена

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */
int add_numbers(int a, int b);  // file asm_lib_n.s
/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define DO4_Pin GPIO_PIN_5
#define DO4_GPIO_Port GPIOA
#define DO1_Pin GPIO_PIN_4
#define DO1_GPIO_Port GPIOC
#define DO2_Pin GPIO_PIN_5
#define DO2_GPIO_Port GPIOC
#define EC11_D_Pin GPIO_PIN_2
#define EC11_D_GPIO_Port GPIOB
#define DO3_Pin GPIO_PIN_12
#define DO3_GPIO_Port GPIOB
#define TASK_3_Pin GPIO_PIN_13
#define TASK_3_GPIO_Port GPIOB
#define TASK_2_Pin GPIO_PIN_14
#define TASK_2_GPIO_Port GPIOB
#define TASK_1_Pin GPIO_PIN_15
#define TASK_1_GPIO_Port GPIOB
#define DE_1_Pin GPIO_PIN_8
#define DE_1_GPIO_Port GPIOA
#define DC_TFT_Pin GPIO_PIN_10
#define DC_TFT_GPIO_Port GPIOC
#define RES_TFT_Pin GPIO_PIN_11
#define RES_TFT_GPIO_Port GPIOC
/* USER CODE BEGIN Private defines */
#define START_REG_AO_HND   0x50       // регистры вода аналоговых сигналов
#define REG_DO_HND         0x5F       // регистры вода цифровых сигналов
#define START_REG_AI_RD    0x70       // регистры связанные с аналоговыми входами
#define REG_DI_RD          0x7F       // регистр связанный с цифровыми входами

#define START_REG_SETPOINTS        0x20       // регистры связанные с SetPoints
#define START_REG_AO       		   0x30       // регистры связанные с аналоговыми выходами
#define REG_START_AUTO       	   0x3F       // регистр 0 - стоп, 1- пуск, 2- авто
#define START_REG_AI       		   0x10       // регистры связанные с аналоговыми выходами
#define REG_DO         		       0x2F       // регистр связанный с цифровыми выходами
#define REG_DI         		       0x0F       // регистр связанный с цифровыми входами
#define REG_MODE         		   0x48       // регистр режима 0 - ручн, 1- лето, 2 - вент, 3 - зима
#define REG_UNIT_TYPE              0x49       // регистр типа устройсва 0 - ведомый, 1 - HVAC1, 2 - HVAC2
#define START_REG_MODBUS_RD		   0x00       // регистры связанные с modbus после преобразования
#define START_REG_MODBUS   		   0x60       // регистры чтения/записи в точки modbus 16 шт
#define COIL_POINTS_MODBUS         0x4A       // набор Coils 16 шт чтения/записи в точки modbus 16 шт
#define REG_POINT_MODBUS_OK        0x4B       // набор Coils 16 шт связанные с Reg 0x60 - 0x6F; 1 - Ok, 0 - Fault
#define COIL_POINTS_MODBUS_OK      0x4C       // набор Coils 16 шт связанные с Coils 0x4A0 - 0x4AF; 1 - Ok, 0 - Fault
#define REG_PASSWORD         	   0x46       // регистр где хранится пароль

#define COIL_T_LOW                     0x470		  // температура ноже нормы - "Холодно"
#define COIL_T_NORM                    0x471		  // температура в норме - "Норма"
#define COIL_T_HIGHT                   0x472		  // температура выше нормы - "Жарко"
#define COIL_CO2_NORM                  0x473		  // СО2 в норме - "Свежо"
#define COIL_CO2_HIGHT                 0x474		  // СО2 превышен - "Душно"

#define REG_ALARM          0x1F       // регистры alarm

#define START_REG_CONST_32  0x80       // регистры для констатнт  32 шт
#define START_NET_COILS     0xB0       // регистры для цифровых соединений 0xB0 - 0xD0 => 0x20 * 0x10
#define START_NET_REG       0xD0      // регистры для аналоговых соединений 0x17f

#define RESET_BUTTON        0x47f      // Coil кнопки Reset
#define REG_TIME_START       0x40      // Reg
#define REG_TIME_STOP        0x41      // Reg
#define REG_TIME             0x45      // Reg
#define BUTTON_START         0x3f0     // Coil
#define BUTTON_AUTO          0x3f1     // Coil

#define UNIT_SLAVE 			0x00
#define HVAC1    			0x01
#define HVAC2    			0x02
#define HVAC3    			0x03
#define HVAC4    			0x04
#define HVAC5    			0x05
#define HVAC6    			0x06
#define HVAC7    			0x07
#define HVAC8    			0x08
#define HVAC9    			0x09
#define HVAC10    			0x0a
#define HVAC11    			0x0b
#define HVAC12    			0x0c
#define HVAC13    			0x0d
#define HVAC14    			0x0e

#define runST7789           0x01
#define runEC11             0x02
#define runUSART            0x04
#define runAlgoritm         0x08

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
