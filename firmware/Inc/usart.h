/**
  ******************************************************************************
  * @file    usart.h
  * @brief   This file contains all the function prototypes for
  *          the usart.c file
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USART_H__
#define __USART_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */
//extern UART_HandleTypeDef huart2;
/* USER CODE END Includes */

extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;

/* USER CODE BEGIN Private defines */
#define USART2_DE_TX GPIOA->BSRR = GPIO_BSRR_BS1; //DE TX
#define USART2_DE_RX GPIOA->BSRR = GPIO_BSRR_BR1;     //DE  RX

#define USART1_DE_TX GPIOA->BSRR = GPIO_BSRR_BS8; //DE  TX
#define USART1_DE_RX GPIOA->BSRR =  GPIO_BSRR_BR8;    //RX  DE
/* USER CODE END Private defines */

void MX_USART1_UART_Init(void);
void MX_USART2_UART_Init(void);

/* USER CODE BEGIN Prototypes */


void InitUSART2_Modbus();
typedef enum
{
  waitStart=0,    // ???? ?????? ??????
  waitTimeout=1   // ????? ??????????? ???? ????? ?????? ?? ????????
} USART_ReceiveStatus;

/// Набор состояния обмена ModBus
typedef enum
{
  waitFrame=0,    ///< ожидание пакета
  receiveFrame=1,  ///< прием пакета
  decodeFrame =2, ///<  декодирование пакета
  sendFrame =3,    ///< отправка пакета
  wait_endFrame =4, ///< ожидание конца пакета
  idelFrame = 5,     ///< промежуток между посылками пакетов
  stopState = 6     ///< обмен по UART не происходит
} USART_Status;

/// Режим ModBus
typedef enum
{
  slaveMode=0,    // Slave
  masterMode=1  // Master
} USART_Mode;

/// Структура USART_ModBus описывает свойство порта и состояние обмена  ModBus RTU
typedef struct
{
 uint8_t Time_Frame_send;  //
 uint16_t Timer_send;  //

 uint8_t Read_Timeout_Byte;    ///<  максимальное время в пакете между байтами 3.5 символа
 uint16_t Timer_wait;          ///< счетчик  ожидания прихода байта

 uint16_t Read_Timeout_Byte_ms; // время таймаута (например 50 мс)

 uint16_t Wait_response;  ///<  счетчик мс для определения таймаута

 USART_Mode mode;        ///<  slaveMode/masterMode
 USART_Status state;     ///< waitFrame ... sendFrame

 uint8_t Addr_req[16]; ///< массив адресов опрашиваемых устройств

 uint8_t Reg_req[16];  // массив регистров опрашиваемых устройств
 uint8_t Function[16];  ///< массив функций 0x03 - Read   0x06 - Write
 uint16_t UnitType;     ///<           Unit Type             Mode
                        // 0 0 0 0  0 0 0 0  0 0 0 0  |  0 0 0 0
                        // Unit Type = 0 -  Slave
                        // Unit Type = 1 -  HVAC_1
                        // Mode = 0 - Cool
                        // Mode = 1 - Fan
                        // Mode = 2 - Heat
                        // Mode = 3 - Auto
                        // Mode = ... - Reserv

 uint8_t count_no_Answer[16];  ///< массив счетчиков неответов по каждому устройству

 uint8_t Critical_no_Answer;    ///< критическое кол-во неответов от устройства для признания что уст-во не отвечает
 ////////////////////////////////////////////////////////////

 uint8_t count_req;                   ///< порядковый номер запрашиваемого устройства в массиве адресов Addr_req[16]
 uint16_t Time_between_send;          ///< время которое нужно выдержать между посылками пакетов
 uint16_t Time_counter_between_send;  ///< счетчик времени которое нужно выдержать между посылками пакетов
 int16_t default_value;               ///< значение по умолчанию при неответе
 uint8_t count_no_answer[16];         ///< кол-во неответов отданного устройства


 uint8_t resByte;      //  DMA
 uint8_t rx_Buff[256];  //
 uint8_t rx_Push;      //
 uint8_t tx_Buff[256];  //
 uint8_t tx_Push;      //

 uint8_t tx_Push_count;      //

 uint8_t restByte_USART_DMA1; ///< оставшиеся количество байт которые нужно принять через DMA

 uint32_t work;         // waitFrame
 uint32_t work_old;     //  waitFrame

 //uint16_t f0x03_NRead;  // Number of read reg
 uint16_t f0x06_NWrite;  // Number of write reg
 uint8_t Adr[16];       ///< Array of address
 uint16_t Reg[16];       ///< Array of registers
 int16_t Data[16];       ///< Принятые данные
 char strRes[16];        ///< string of responce
 char strRes_old[16];    ///< string of responce before
 uint8_t i;              ///< цикл

 uint8_t count_bytes_receive;  ///< кол-во прочитанных байт
 uint8_t wait_for_bytes;       ///< кол-во  байт ожидаемых байт в ответе
 uint8_t Temp;                 ///< временная переменная

}USART_ModBus;


//  структура для работы с UART1 SLAVE
typedef struct
{
    uint8_t buf[256];        // буфер приёма
    uint16_t count;          // число принятых байт
    uint8_t temp;            // временный байт
    volatile uint8_t frameReady; // флаг "кадр готов"
} ModbusRTU_t;


typedef enum
{
 bound_9600 = 0xff,
 bound_19200 = 0,
 bound_38400 = 1
}Bound_modbus;

typedef struct
{
 uint8_t addr_modbus;
 Bound_modbus bound;
}Saved_regs;

/// Структура Modbus_Point одной точки обмена ModBus
typedef struct
{
 uint8_t addr;    ///< адрес устройства
 uint8_t func;    ///< функция обращения
 uint16_t reg;    ///< номер Reg или Coil к которому обращаемся в ведомом устройстве на запись или чтение
 uint16_t data;    ///<  адрес Reg или Coil куда запишем считанные данные или от куда возьмем данные для отправки
 uint16_t ok;     ///<  адрес Reg или Coil куда запишем 1 - ответ правильный, 0 - ответ ошибочный или ответа нет
 int16_t def;      ///<  значение по умолчанию
 uint16_t period;  ///<  период отправки запросов 0 - не отправлять, 1 - при каждом проходе, 2 - через раз и т.п.
 uint16_t count;   ///<  счетчик просмотра точки MODBUS при достижнии period послать запрос
 uint16_t timeout; ///<  таймаут на ответ ms
 uint8_t len_TX;   ///<  длина отправляемого пакета целиком
 uint8_t len_RX;   ///<  длина принимаемого пакета целиком
 int8_t mult;      ///< множитель на который умножим результат
 int8_t div;       ///< делитель на который поделим результат
 uint8_t text_id;  ///< еденицы измерения
 uint8_t no_answer; ///< количество неответов подряд от данной точки
}Modbus_Point;

typedef struct {
 uint8_t addr;
 uint8_t func;
}sModbusBlock;

/// Структура точек обмена для HVACx
typedef struct
{
 uint8_t        num;              ///<количество точек обмена для данного HVAC максимально 32
 Modbus_Point   modbusPoint[32];  ///< массив точек обмена
} ModbusHVACx;

extern USART_ModBus USART2_Modbus, USART1_Modbus;
extern ModbusHVACx  modbusHVAC1;
extern ModbusRTU_t modbus1;   ///< структура для обмена по Modbus UART1 Slave

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __USART_H__ */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
