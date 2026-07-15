//#include "stm32f10x.h"
#include "main.h"
#ifndef __FLASH_H
#define __FLASH_H

//#define FLASH_PAGE_SIZE ((uint16_t)0x400)
#define FLASH_START_ADDR  ((uint32_t)0x08000000)

#define POITER_WRITE_ADDR ((uint32_t)0x0800FFFC)  //64K
#define START_ADDR_LAST_PAGE  ((uint32_t)0x0803f800) //((uint32_t)0x0800FC00)
#define END_ADDR_LAST_PAGE  ((uint32_t)0x0803ffff) //((uint32_t)0x0800FC00)

#define START_ADDR_126_PAGE  ((uint32_t)0x0803f000) //
#define END_ADDR_126_PAGE  ((uint32_t)0x0803f7ff)   //

#define LEN_STRUCT_FOR_SAVE 35

#define START_REG_MODBUS_SAVE 0x80   // ��� �������� Modbus ������� ���������
                                     // ���� ���������� �� Flash
#define N_REG_MODBUS_SAVE 0x26       // ���-�� ���-� Modbus ����������� �� Flash

#define ADDR_POINT_UP ((uint32_t)0x0800FFEC)   // ������������ ���������� 
#define ADDR_POINT_DOWN ((uint32_t)0x0800FF80) // �� 28 ������

void write_WordFLASH(uint32_t address, uint32_t data);
void write_BlockFLASH(uint32_t *block, uint8_t len); // ������ ����� ������
                                                     // �� Flash
void Rewrite_BlockFLASH(uint32_t *block, uint8_t len); // ������ ����� ������
                                                     // �� Flash �� ��������� 
// �������� ����� ������������ � ������ ��������� ��������
// 0x0800FFFF - 0x400 = 0x0800FC00
uint16_t find_shift_addr();                //
void Pack_Word(uint32_t *block);         // упаковка
void FillStructFromFLASH();           // заполнение структуры USART_Modbus из Flash
void FillStructDefault();           //  заполнение структуры USART_Modbus по умолчанию

uint32_t findStartAddrToWrite(uint32_t addrPage, uint32_t addrEndPage, uint8_t type, uint16_t len); // нахождение адреса для записи
uint32_t findStartAddrToRead(uint32_t addrPage, uint32_t addrEndPage, uint8_t type, uint16_t len); // нахождение адреса для записи
#endif
