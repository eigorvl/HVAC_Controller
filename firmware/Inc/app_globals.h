/*
 * app_globals.h
 *
 *  Created on: 5 июн. 2026 г.
 *      Author: dexp
 */

#ifndef INC_APP_GLOBALS_H_
#define INC_APP_GLOBALS_H_

#include "cmsis_os.h"

extern SemaphoreHandle_t gSensorMutex;

extern const unsigned short logo[]; ///< логотип
//extern const unsigned short TP_icon[];
extern const unsigned short pipe_H_10x20[];///< изображение трубы горизонтальной
extern const unsigned short valve_H_45x36[];
extern const unsigned short valve_H_alarm_45x36[];
extern const unsigned short heatExchanger_H_40x115[];


// RadioButton
extern const unsigned short circle_26x26[];
extern const unsigned short circle_check_26x26[];
extern const unsigned short circle_cursor_26x26[];
extern const unsigned short circle_check_cursor_26x26[];

// треугольник
extern const unsigned short alarm_26x26[];


extern const unsigned short led_off_26x26[];
extern const unsigned short led_on_26x26[];
extern const unsigned short led_off_12x12[];
extern const unsigned short led_on_12x12[];
extern const unsigned short led_Alarm_12x12[];

// насос
extern const unsigned short Pump_H_62x44[];

extern const unsigned short Pipe_Air_1_6x46[];

// фильтры
extern const unsigned short fan1_46x46[];


extern const unsigned short filter_14x46[];
extern const unsigned short Filt_Alarm_14x46[];
extern const unsigned short filter_Alarm_14x46[];

// рекуператоры
extern const unsigned short Recup_2_49x92[];
extern const unsigned short Recup_2_Alarm_49x92[];

extern const unsigned short Heater_23x46_color[];
extern const unsigned short TEN_Off_23x46[];

extern const unsigned short WaterPipe_L_22x21[];

extern const unsigned short WaterPipe_R_22x21[];

// стрелки
extern const unsigned short Right_7x26[];
extern const unsigned short Left_7x26[];
extern const unsigned short Down_26x7[];
extern const unsigned short Up_26x7[];

extern const unsigned short Left_Water_Cold_6x11[];
extern const unsigned short Right_Water_Cold_6x11[];
extern const unsigned short Left_Water_Hot_6x11[];
extern const unsigned short Right_Water_Hot_6x11[];

// воздушные клапаны
extern const unsigned short Air_Valve_V_15x46[]; // клапан 0-10В
extern const unsigned short Air_Valve_Alarm_15x46[];
extern const unsigned short Air_Valve_H_46x15[]; // // клапан 0-10В
extern const unsigned short Air_Small_15x25[];
extern const unsigned short Air_Small_Alarm_15x25[];
extern const unsigned short Air_Valve_H_35x15[];      //  клапан маленький 0-10В
extern const unsigned short Air_Valve_H_Alarm_35x15[];


// клапаны открыт/закрыт
extern const unsigned short Air_OFF_15x46[];
extern const unsigned short Air_ON_15x46[];

// водяные клапаны
extern const unsigned short WaterValve_L_19x19[];
extern const unsigned short WaterValve_R_19x19[];
extern const unsigned short WaterValve_Alarm_R_19x19[];

// кнопка
//extern const unsigned short Button_1_49x30[];  ///< малая кнопка
//extern const unsigned short Button_2_49x30[];
//extern const unsigned short Button_3_49x30[];

extern const unsigned short Reset_11_120x24[];  ///< большая кнопка
extern const unsigned short Reset_21_120x24[];
extern const unsigned short Reset_31_120x24[];

extern const unsigned short lock_orange_18x26[];
extern const unsigned short key_18x26[];

extern uint16_t g_UpDate_Count;              // time update data on the screen

extern uint8_t g_mask_Buttons;               // Mask buttons

extern volatile uint32_t g_mode_Menu;                  // Mode Menu
extern volatile uint32_t g_mode_Menu_old;              // Mode Menu old

extern uint8_t g_change_Menu;                // if the Menu was changed?

extern RTC_TimeTypeDef sTime;
extern RTC_DateTypeDef DateTypeUpdate;

extern s_Menu menu;                          // struct Menu
extern volatile statusMenu status_Menu;
extern uint8_t g_regs[0x3A0];           // глобальный массив регистров
extern uint8_t g_txBuf[64];  // пакет на передачу
extern uint8_t g_txPush; // длинна пакета на передачу

extern uint8_t S_SLAVE_ADDRESS; // адрес самого шлюза
extern uint8_t N_SAVED_REG;  // номер сохраняемого регистра  0xXX 0xXX
                           //                             Adr  Bound
extern uint16_t dimForChar[3850];    // массив для стирания символов на экране

extern uint8_t password[4];      // введенный пароль
extern uint16_t password_corect; // парооль user
extern uint16_t password_admin; // парооль admin
extern uint8_t password_OK;
extern  uint8_t uRunningTask;   // 1- задача запущена

extern USART_ModBus USART2_Modbus; ///< Modbus point for UART2

#endif /* INC_APP_GLOBALS_H_ */
