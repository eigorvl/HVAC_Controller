/**
  ******************************************************************************
  * @file            Flash.c
  * @brief           Содержит функции для работы со Flash памятью
  * @author          Эйсвальд И.А.
  * @date            2025-06-08
  * @details         Декодирование команд ModBus<br>
  * Работа со FLASH памятью<br>
  ******************************************************************************
 */

#include "main.h"
#include "utils.h"
#include "flash.h"

//volatile FLASH_Status FLASHStatus = FLASH_COMPLETE;
//volatile FLASH_Status FLASHStatus = FLASH_COMPLETE;

////  How to use
////  // ������ FLASH
////  uint32_t addr_flash = 0x0800fffc;
////  uint32_t data_flash = 0;
////  write_WordFLASH(addr_flash, (uint32_t)0x55555555);
////  
////  data_flash = (*(__IO uint32_t*)addr_flash);


///////////////////////////////////////////////////////////////////////
// addrPage - стартовый адрес страницы
// addrEndPage - конечный адрес страницы
// type     - тип данных 1 - byte; 2 - uint16_t; 4 - uint32_t
// len      - кол-во данных
// return   startAddr  -  адрес для записи
//////////////////////////////////////////////////////////////////////
/**
 * @brief Нахождение свободного адреса для записи блока во Flash память
 * @param addrPage -  стартовый адрес страницы
 * @param addrEndPage - конечный адрес страницы
 * @param type - тип данных 1 - byte; 2 - uint16_t; 4 - uint32_t
 * @param len - кол-во данных
 * @return uint32_t - адрес для записи
 * @note  Поиск конца записи, находим чистый блок для следующей записи<br>
 * 		если такого блока не находим, то считаем сколько 0xffffffff подряд<br>
 * 		или после до конца страницы были только 0xffffffff<br>
 * 		зная последнюю запись находим начальный адрес последнего записанного блока
 */
uint32_t findStartAddrToWrite(uint32_t addrPage, uint32_t addrEndPage, uint8_t type, uint16_t len)
{
 uint32_t addr = addrPage;
 uint16_t count = 0;
 uint32_t data;
 data =12;

 while(addr < addrEndPage){
	 data = *(__IO uint32_t*)addr;
  if(data == 0xffffffff) count++;
  else count = 0;

  if(count == len) return addr - (len-1)*4;

  addr += 4;
 }

 return addrEndPage;
}

///////////////////////////////////////////////////////////////////////
// Поиск конца записи, находим чистый блок для следующей записи
// если такого блока не находим, то считаем сколько 0xffffffff подряд
// или после до конца страницы были только 0xffffffff
// зная последнюю запись находим начальный адрес последнего записанного блока
// addrPage - стартовый адрес страницы
// addrEndPage - конечный адрес страницы
// type     - тип данных 1 - byte; 2 - uint16_t; 4 - uint32_t
// len      - кол-во данных
// return   startAddr  -  адрес для записи
//////////////////////////////////////////////////////////////////////
/**
 * @brief Адреса для чтения блока из Flash памяти
 * @param addrPage -  стартовый адрес страницы
 * @param addrEndPage - конечный адрес страницы
 * @param type - тип данных 1 - byte; 2 - uint16_t; 4 - uint32_t
 * @param len - кол-во данных
 * @return uint32_t - адрес для записи
 * @note  Поиск конца записи, находим чистый блок для следующей записи<br>
 * 		если такого блока не находим, то считаем сколько 0xffffffff подряд<br>
 * 		или после до конца страницы были только 0xffffffff<br>
 * 		зная последнюю запись находим начальный адрес последнего записанного блока
 */
uint32_t findStartAddrToRead(uint32_t addrPage, uint32_t addrEndPage, uint8_t type, uint16_t len)
{
 uint32_t addr = addrPage;
 uint16_t count = 0;
 uint32_t data;
// data =12;

 while(addr < addrEndPage){
	 data = *(__IO uint32_t*)addr;
  if(data == 0xffffffff) count++;
  else count = 0;

  addr += 4;

  if(count == len) {
	  // свободный блок для следующей записи найден
	  // возвращаем адрес для чтения
	  return addr - 2*(len*4);
  }

 }
 // целый свободный блок не найден
 return addrEndPage - count*4 - len*4 + 1;
}


/////////////////////////////////////
// запаковываем MODBUS_REG в слова  uint32_t
////////////////////////////////////
void Pack_Word(uint32_t *block)
{
// static uint32_t hi,lo,wd, reg;
// static uint8_t i;
 
 for(uint8_t i=START_REG_MODBUS_SAVE;i<START_REG_MODBUS_SAVE+N_REG_MODBUS_SAVE;i=i+2)
 {  
 *block =  ((uint32_t)(MODBUS_REG(i)))&0xffff;
// lo = ((uint32_t)(MODBUS_REG(i)))&0xffff;
// hi = ((uint32_t)(MODBUS_REG(i+1)) << 16);
 
// wd = hi|lo;
// reg = ((uint32_t)(MODBUS_REG(0x83)) << 16);
 *block |= ((uint32_t)(MODBUS_REG(i+1)) << 16);
 block++;
 } 
}


//////////////////////////////////////////////
// ���������� ��������� USART_Modbus �� ���������
//////////////////////////////////////////////
void FillStructDefault()
{
//   S_SLAVE_ADDRESS = 0x82;
//
//  //set bound
//   g_sUSART1.USART_struct.USART_BaudRate=38400;
//   g_sUSART2.USART_struct.USART_BaudRate=38400;
//
//
//  g_sUSART1.Wait_response = 500; // 50 ms
//  g_sUSART1.Time_between_send = 500; // reg 0x84 SS SS  1 ms
//  g_sUSART1.default_value = -2000;
//
//
//  // ����������� ���-�� ��������� �� ��������� = 3
//  g_sUSART1.Critical_no_Answer = 3;
//  // ���������� ������� ������� ��������
//  for(uint8_t i=1;i<=32;i++)
//  {
//   g_sUSART1.Addr_req[i] = i;
//  }
//
//  // ���������� ������� ��������� �������� �� ��������� 0�00
//  for(uint8_t i=1;i<=32;i++)
//  {
//   g_sUSART1.Reg_req[i] = 0;
//  }
}
//////////////////////////////////////////////
// ���������� ��������� USART_Modbus �� Flash
//////////////////////////////////////////////
void FillStructFromFLASH()
{
//   S_SLAVE_ADDRESS = (MODBUS_REG(START_REG_MODBUS_SAVE) >> 8) & 0xFF;
//
//  //set bound
//   g_sUSART1.USART_struct.USART_BaudRate=9600;
//   g_sUSART2.USART_struct.USART_BaudRate=9600;
//
//  // USART2
//  if(((MODBUS_REG(START_REG_MODBUS_SAVE)) & 0xff) == bound_19200)
//  {
//   g_sUSART2.USART_struct.USART_BaudRate=19200;
//  }
//  else if(((MODBUS_REG(START_REG_MODBUS_SAVE)) & 0xff) == bound_38400)
//  {
//   g_sUSART2.USART_struct.USART_BaudRate=38400;
//  }
//  else if(((MODBUS_REG(START_REG_MODBUS_SAVE)) & 0xff) == bound_9600)
//  {
//   g_sUSART2.USART_struct.USART_BaudRate=9600;
//  }
//
//  // USART1
//  if(((MODBUS_REG(START_REG_MODBUS_SAVE+2)) & 0xff) == bound_19200)
//  {
//   g_sUSART1.USART_struct.USART_BaudRate=19200;
//  }
//  else if(((MODBUS_REG(START_REG_MODBUS_SAVE+2)) & 0xff) == bound_38400)
//  {
//   g_sUSART1.USART_struct.USART_BaudRate=38400;
//  }
//  else if(((MODBUS_REG(START_REG_MODBUS_SAVE+2)) & 0xff) == bound_9600)
//  {
//   g_sUSART1.USART_struct.USART_BaudRate=9600;
//  }
//
//  g_sUSART1.Wait_response = MODBUS_REG(START_REG_MODBUS_SAVE+3);
//  g_sUSART1.Time_between_send = MODBUS_REG(START_REG_MODBUS_SAVE+4); // reg 0x84 SS SS
//  g_sUSART1.default_value = MODBUS_REG(START_REG_MODBUS_SAVE+5);
//
//  // ����������� ���-�� ���������
//  g_sUSART1.Critical_no_Answer = (MODBUS_REG(START_REG_MODBUS_SAVE+1) >> 8);
//
//  // ���������� ������� ������� ��������
//  for(uint8_t i=0;i<16;i=i+2)
//  {
//   g_sUSART1.Addr_req[i] = (MODBUS_REG(START_REG_MODBUS_SAVE+6+i/2) & 0xff);
//   g_sUSART1.Addr_req[i+1] = (MODBUS_REG(START_REG_MODBUS_SAVE+6+i/2) >> 8);
//
//  }
//
//  // ���������� ������� ��������� ��������
//  for(uint8_t i=0;i<16;i=i+2)
//  {
//   g_sUSART1.Reg_req[i] =  (MODBUS_REG(START_REG_MODBUS_SAVE+14+i/2) & 0xff);
//   g_sUSART1.Reg_req[i+1] = (MODBUS_REG(START_REG_MODBUS_SAVE+14+i/2) >> 8);
//  }
//
//  // Fill unitType
//  menu.unitType = ((MODBUS_REG(START_REG_MODBUS_SAVE+0x25) & 0xf0) >> 4);
//
//  // Fill unitMode
//  menu.unitMode = (MODBUS_REG(START_REG_MODBUS_SAVE+0x25) & 0x03);
}
///////////////////////////////////////
//   записываем 20 uint32_t  в последнюю страницу 1 кБ
//   0x0803f800 - 0x803ffff
//   ищем первые подряд 20 uint32_t равные 0xffffffff
//   Если место есть то:
//   вычисляем адрес с которого начать запись address_start =
//   Иначе:
//   Erase FLASH page_127
//   address_start = 0x0803f800
//   подготавливаем данные из uint16_t в uint32_t  0x0020  -> 0xffff0020
//   Unlock
//   Write
//   Lock
///////////////////////////////////////
void write_WordFLASH(uint32_t address, uint32_t data)
{ 
// // Unlock FLASH Bank1
	HAL_FLASH_Unlock();

// FLASH_UnlockBank1();
//
// // Clear all pending flags
// FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);
//
// // Erase FLASH page
// FLASHStatus = FLASH_ErasePage(address);
//
// // ���� ����� ������� ��������
// while(FLASHStatus != FLASH_COMPLETE);
//
// // Program FLASH
// FLASHStatus = FLASH_ProgramWord(address, data);
//
// // ���� ����� ������� ������
// while(FLASHStatus != FLASH_COMPLETE);
//
// // Lock FLASH Bank1
// FLASH_LockBank1();
}
/////////////////////////////////////////////////////////////////
// ����� ��������
//////////////////////////////////////////////////////////////////
uint16_t find_shift_addr()
{
  uint32_t point = (*(__IO uint32_t*)POITER_WRITE_ADDR);
  uint8_t shift;
  for(shift=0; shift<32; shift++)
  {
   if(((point >> shift) & 0x01) == 1) break; 
  }
  
  return shift;  
}
//////////////////////////////////////////////////////////////////
// ������ ����� ������ �� Flash
//////////////////////////////////////////////////////////////////
void write_BlockFLASH(uint32_t *block, uint8_t len)
{
// uint16_t shift;
// uint32_t write_addr;
// uint32_t poiter_new; // ����� ��������� �� ������ ����� ������ ������
// //1. ������ point_Flash ������� ������ ������� � ��������� ����� � ������
// //   ������ ���������� ����
// shift = find_shift_addr();
// // ����������� ������
// Pack_Word(BlockFlash);
//
// // ����� �� �� �������� ������
// if(shift < 32)
// {
//  write_addr = START_ADDR_LAST_PAGE + shift * LEN_STRUCT_FOR_SAVE;
//  poiter_new = (*(__IO uint32_t*)POITER_WRITE_ADDR);
//  // ����� ��������� �� ������
//  poiter_new = poiter_new << 1;
//
//  // ���������� ����� ���������
//  // ...
//  // Unlock FLASH Bank1
//  FLASH_UnlockBank1();
//  // Program FLASH point
////  FLASHStatus = FLASH_ProgramWord(POITER_WRITE_ADDR, poiter_new);
////   while((FLASHStatus != FLASH_ERROR_WRP)||(FLASHStatus != FLASH_COMPLETE));
//
//
//  // ��������� ������
//  //
//  for(uint8_t i=0; i<len; i++)
//  {
//   FLASHStatus = FLASH_ProgramWord(write_addr, *block);
//   while(FLASHStatus != FLASH_COMPLETE);
//
//   block++;
//   write_addr+=4;
//  }
//  // Lock FLASH Bank1
//  FLASH_LockBank1();
//  // ���������� ���� ������
//  //
// }
//
//
// //2. ���������� � point_Flash �� ����� ��������� ������� ���� � ����������
// //   ���� ������ ������������� ������
//
// //3. ���� point_Flash == 0 ������� �������� � ���������� ���� ������ �
// //   ������ ��������
  
}

//////////////////////////////////////////////////////////////////
// ���������� ����� ������ �� Flash �� ��������� ��������
//////////////////////////////////////////////////////////////////
void Rewrite_BlockFLASH(uint32_t *block, uint8_t len)
{
// uint32_t write_addr;
//
// // ����������� ������
// Pack_Word(BlockFlash);
//
// write_addr = START_ADDR_LAST_PAGE;
// // UnLock FLASH Bank1
// FLASH_UnlockBank1();
//
//  // Erase FLASH page
// FLASHStatus = FLASH_ErasePage(write_addr);
//
// // ���� ����� ������� ��������
// while(FLASHStatus != FLASH_COMPLETE);
//
//
//  // ��������� ������
//  for(uint8_t i=0; i<len; i++)
//  {
//   FLASHStatus = FLASH_ProgramWord(write_addr, *block);
//   while(FLASHStatus != FLASH_COMPLETE);
//
//   block++;
//   write_addr+=4;
//  }
//  // Lock FLASH Bank1
//  FLASH_LockBank1();
}
