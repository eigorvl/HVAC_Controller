/**
  ******************************************************************************
  * @file            utils.h
  * @brief           Содержит вспомогательные функции
  * @author          Эйсвальд И.А.
  * @date            2025-06-08
  * @details         Декодирование команд ModBus<br>
  * Работа со FLASH памятью<br>
  ******************************************************************************
*/

#ifndef __UTILS_H
#define __UTILS_H

//#include "stm32f10x.h"
#include "main.h"
#include "app_globals.h"

typedef unsigned char 	BYTE;
typedef unsigned short 	WORD;
typedef unsigned long 	DWORD;

#define bitset(var,bitno) ((var) |= 1 << (bitno))
#define bitclr(var,bitno) ((var) &= ~(1 << (bitno)))

// ����������� �� ������ ���-� ��������� ����� ModBus
#define MODBUS_BUFFER_LEN 	0xfff//((S_REG_NUM + NS_REG_NUM) / 2)

#define IWDG_TIMEOUT        5000 /*5000*/  /*15 */ // 500 ms ������ ������������ �������� IWDG_RLR 
#define RLR_IWDG            4000 /*1000*/  /*25*/ // ����� 800 ms ���������� ������������ ����������� 
                                    // ���� �� �������� �������
#define RLR_IWDG_IMMID          2   // ����� 0.8 ms ����������� ������������ ����������� 

//extern unsigned char g_regs[32];  // ������ ���������

////////////////////////////// ����������� �� ������ ���-� ��������� ����� ModBus
///////////////////////////////////#define MODBUS_BUFFER_LEN 	((S_REG_NUM + NS_REG_NUM) / 2)  

////////////////////////////////////////// ������������ ����
/////////////////////////////////////////////#define ADDRESS_BEGIN_OPTION   0x90   // ����� ������ �������

/// макрос доступа к регистру
#define MODBUS_REG(num)		(((int16_t *)g_regs)[num])
/// макрос прочитать Coil num
#define READ_COIL(num)	((MODBUS_REG((num) / 16) >> ((num) % 16)) & 1)
/// макрос установить Coil num
#define SET_COIL(num) 	{MODBUS_REG((num) / 16) |= (1 << ((num) % 16));}
/// макрос сбросить  Coil num
#define CLR_COIL(num)	{MODBUS_REG((num) / 16) &= ~(1 << ((num) % 16));}



BYTE CRC8(BYTE* str, BYTE len);
WORD CRC16(BYTE* puchMsg, BYTE len);
void SaveRegsToFLASH();
uint32_t SaveBlockToFLASH(uint16_t startModbusReg, uint16_t numReg, uint8_t Page, uint8_t numPages); ///<  0 - successfully запись блока во Flash со стиранием страницы 2КB.
uint32_t AddBlockToFLASH(uint16_t startModbusReg, uint16_t numReg, uint8_t Page, uint16_t shiftAddr);///< 0 - successfully добавление блока во Flash без стирания страницы 2КB.
void LoadRegsFromFLASH();
void LoadBlockFromFLASH();

void ModBusDecode(unsigned char* receive_USART, unsigned char len);         ///< декодирвание команд  Modbus полученых от мастера и формирование ответа
void ModBusDecode_local(unsigned char* receive_USART, unsigned char len);   ///< декодирвание команд  Modbus полученых от мастера и формирование ответа

void ModBusDecode_Master(unsigned char* receive_USART, unsigned char len);  ///< декодирвание ответа  Modbus полученых от Slave


int8_t GetNumPoint(ModbusHVACx* pointList, int8_t i_old); ///< выбор точки обмена MODBUS из списка

//char DecodeCO2(uint8_t *CHAR, uint16_t* CO2, uint16_t* Temp);
//void SaveRegsToEEPROM();

void Delay_us_10(__IO uint32_t nTime);
void TimingDelay_Decrement(void);

void HexToStr(char* str, int param);
void IntToStr(char* str, signed char dat);
void IntToStr_16(char* str, int16_t dat);
void IntToStr_100(char* str, int16_t dat);

//uint16_t Change_str_Down(uint16_t);
//uint16_t Change_str_Up(uint16_t);
//uint16_t Change_str_Right(uint16_t );
//uint16_t Change_str_ESC(uint16_t );
//uint16_t Change_str_Enter(uint16_t );

//void ChangeStrDown();
//void ChangeStrUp();
//void ChangeStrRight();
//void ChangeStrESC();
//void ChangeStrEnter();
//void ChangeStrAlarm();

#endif /* __UTILS_H */
