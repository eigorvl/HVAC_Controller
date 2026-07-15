/**
  ******************************************************************************
  * @file            utils.c
  * @brief           Содержит вспомогательные функции
  * @author          Эйсвальд И.А.
  * @date            2025-06-08
  * @details         Декодирование команд ModBus<br>
  * Работа со FLASH памятью<br>
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


#include "utils.h"
#include "main.h"
#include "flash.h"
#include "usart.h"


 /**
  * @brief Table of CRC values for high order byte
  */
unsigned char auchCRCHi[256] = {
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40 
};

/**
 * @brief Table of CRC values for low order byte
 */
unsigned char auchCRCLo[256] = {
0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06, 0x07, 0xC7, 0x05, 0xC5, 0xC4, 0x04,
0xCC, 0x0C, 0x0D, 0xCD, 0x0F, 0xCF, 0xCE, 0x0E, 0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09, 0x08, 0xC8,
0xD8, 0x18, 0x19, 0xD9, 0x1B, 0xDB, 0xDA, 0x1A, 0x1E, 0xDE, 0xDF, 0x1F, 0xDD, 0x1D, 0x1C, 0xDC,
0x14, 0xD4, 0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3, 0x11, 0xD1, 0xD0, 0x10,
0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3, 0xF2, 0x32, 0x36, 0xF6, 0xF7, 0x37, 0xF5, 0x35, 0x34, 0xF4,
0x3C, 0xFC, 0xFD, 0x3D, 0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A, 0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38,
0x28, 0xE8, 0xE9, 0x29, 0xEB, 0x2B, 0x2A, 0xEA, 0xEE, 0x2E, 0x2F, 0xEF, 0x2D, 0xED, 0xEC, 0x2C,
0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26, 0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0,
0xA0, 0x60, 0x61, 0xA1, 0x63, 0xA3, 0xA2, 0x62, 0x66, 0xA6, 0xA7, 0x67, 0xA5, 0x65, 0x64, 0xA4,
0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F, 0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 0x69, 0xA9, 0xA8, 0x68,
0x78, 0xB8, 0xB9, 0x79, 0xBB, 0x7B, 0x7A, 0xBA, 0xBE, 0x7E, 0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C,
0xB4, 0x74, 0x75, 0xB5, 0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71, 0x70, 0xB0,
0x50, 0x90, 0x91, 0x51, 0x93, 0x53, 0x52, 0x92, 0x96, 0x56, 0x57, 0x97, 0x55, 0x95, 0x94, 0x54,
0x9C, 0x5C, 0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E, 0x5A, 0x9A, 0x9B, 0x5B, 0x99, 0x59, 0x58, 0x98,
0x88, 0x48, 0x49, 0x89, 0x4B, 0x8B, 0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,
0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42, 0x43, 0x83, 0x41, 0x81, 0x80, 0x40
};

/**
 * @brief Расчет CRC16 для протокола ModBus
 *
 * @param puchMsg - указатель на входной  массив
 * @param len - кол-во байт для расчета
 * @return Значение CRC16
 */

WORD CRC16(BYTE* puchMsg, BYTE len)
{
	unsigned char uchCRCHi = 0xFF ;
	unsigned char uchCRCLo = 0xFF ;
	unsigned char uIndex ;
	while (len--)
	{
		uIndex = uchCRCHi ^ *puchMsg++;
		uchCRCHi = uchCRCLo ^ auchCRCHi[uIndex];
		uchCRCLo = auchCRCLo[uIndex];
	}
	return (uchCRCHi << 8 | uchCRCLo);
}

/**
 * @brief Table of CRC8 values
 */
unsigned char crc8_tabl[]={
0, 94, 188, 226, 97, 63, 221, 131, 194, 156, 126, 32, 163, 253, 31, 65,
157, 195, 33, 127, 252, 162, 64, 30, 95, 1, 227, 189, 62, 96, 130, 220,
35, 125, 159, 193, 66, 28, 254, 160, 225, 191, 93, 3, 128, 222, 60, 98,
190, 224, 2, 92, 223, 129, 99, 61, 124, 34, 192, 158, 29, 67, 161, 255,
70, 24, 250, 164, 39, 121, 155, 197, 132, 218, 56, 102, 229, 187, 89, 7,
219, 133, 103, 57, 186, 228, 6, 88, 25, 71, 165, 251, 120, 38, 196, 154,
101, 59, 217, 135, 4, 90, 184, 230, 167, 249, 27, 69, 198, 152, 122, 36,
248, 166, 68, 26, 153, 199, 37, 123, 58, 100, 134, 216, 91, 5, 231, 185,
140, 210, 48, 110, 237, 179, 81, 15, 78, 16, 242, 172, 47, 113, 147, 205,
17, 79, 173, 243, 112, 46, 204, 146, 211, 141, 111, 49, 178, 236, 14, 80,
175, 241, 19, 77, 206, 144, 114, 44, 109, 51, 209, 143, 12, 82, 176, 238,
50, 108, 142, 208, 83, 13, 239, 177, 240, 174, 76, 18, 145, 207, 45, 115,
202, 148, 118, 40, 171, 245, 23, 73, 8, 86, 180, 234, 105, 55, 213, 139,
87, 9, 235, 181, 54, 104, 138, 212, 149, 203, 41, 119, 244, 170, 72, 22,
233, 183, 85, 11, 136, 214, 52, 106, 43, 117, 151, 201, 74, 20, 246, 168,
116, 42, 200, 150, 21, 75, 169, 247, 182, 232, 10, 84, 215, 137, 107, 53};

/**
 * @brief Расчет CRC8 для протокола uLAN (1-wire)
 *
 * @param str - указатель на входной  массив
 * @param len - кол-во байт для расчета
 * @return Значение CRC8
 */
BYTE CRC8(BYTE* str, BYTE len)
{
	unsigned char crc8=0;
	while (len){
	crc8=crc8_tabl[crc8 ^ * str++];
	--len;}
	return crc8;
}

//##################################################################################################
// декодирвание ответа  Modbus полученых от Slave
//##################################################################################################
void ModBusDecode_Master(unsigned char* receive_USART, unsigned char len)
{

}
//##################################################################################################
// декодирвание команд  Modbus полученых от мастера и формирование ответа локальный
//##################################################################################################
void ModBusDecode_local(uint8_t* rxBuf, uint8_t rxPush)
{
	// uint8_t send_USART2[32]
	// uint8_t receive_USART2[32]

		g_txBuf[0] = S_SLAVE_ADDRESS;
		g_txBuf[1] = rxBuf[1];
		//01 (0x01) Read Coils
		if(rxBuf[1] == 0x01)
		{
			if(rxPush != 8)  return; // ������ �� �����

			unsigned int starting_address = (rxBuf[2] << 8) | rxBuf[3];
			unsigned int regs_count = (rxBuf[4] << 8) | rxBuf[5];
			if((regs_count < 1) || (regs_count > 0x20))//ExceptionCode = 03
			{
				g_txPush = 5;
				g_txBuf[1] = 0x81;
				g_txBuf[2] = 0x03;
				return;
			}
			if(((starting_address + regs_count + 15) / 16) > MODBUS_BUFFER_LEN)//ExceptionCode = 02
			{
				g_txPush = 5;
				g_txBuf[1] = 0x81;
				g_txBuf[2] = 0x02;
				return;
			}
			g_txBuf[1] = 0x01;
			g_txBuf[2] = (regs_count + 7) / 8;
			for(unsigned int j = 0; j < g_txBuf[2]; j++)
				g_txBuf[3 + j] = 0;
			for(unsigned int j = 0; j < regs_count; j++)
				g_txBuf[3 + j / 8] |= READ_COIL(starting_address + j) << (j % 8);
			g_txPush = 5 + g_txBuf[2];
		}
		//03 (0x03) Read Holding Registers
		else if(rxBuf[1] == 0x03)
		{
			if(rxPush != 8)
				return;
			unsigned int starting_address = (rxBuf[2] << 8) | rxBuf[3];
			unsigned int regs_count = (rxBuf[4] << 8) | rxBuf[5];
			if((regs_count < 1) || (regs_count > 125))//ExceptionCode = 03
			{
				g_txPush = 5;
				g_txBuf[1] = 0x83;
				g_txBuf[2] = 0x03;
				return;
			}
			if((starting_address + regs_count) > MODBUS_BUFFER_LEN)//ExceptionCode = 02
			{
				g_txPush = 5;
				g_txBuf[1] = 0x83;
				g_txBuf[2] = 0x02;
				return;
			}
			g_txBuf[1] = 0x03;
			g_txBuf[2] = 2 * regs_count;
			for(unsigned int i = 0; i < regs_count; i++)
			{

				g_txBuf[3 + i * 2] = (MODBUS_REG(starting_address + i) >> 8) & 0xFF;
				g_txBuf[4 + i * 2] =  MODBUS_REG(starting_address + i) & 0xFF;
			}
			g_txPush = 5 + g_txBuf[2];
		}
		//05 (0x05) Write Single Coil
		else if(rxBuf[1] == 0x05)
		{
			if(rxPush != 8)
				return;
			unsigned short starting_address = (rxBuf[2] << 8) | rxBuf[3];
			unsigned short value = (rxBuf[4] << 8) | rxBuf[5];
			if((value != 0x0000) && (value != 0xFF00))//ExceptionCode = 03
			{
				g_txPush = 5;
				g_txBuf[1] = 0x85;
				g_txBuf[2] = 0x03;
				return;
			}
			if(((starting_address + 15) / 16) > MODBUS_BUFFER_LEN)//ExceptionCode = 02
			{
				g_txPush = 5;
				g_txBuf[1] = 0x85;
				g_txBuf[2] = 0x02;
				return;
			}
			g_txBuf[1] = rxBuf[1];
			g_txBuf[2] = rxBuf[2];
			g_txBuf[3] = rxBuf[3];
			g_txBuf[4] = rxBuf[4];
			g_txBuf[5] = rxBuf[5];
			if(value == 0xFF00)
				SET_COIL(starting_address)
			else
				CLR_COIL(starting_address)
			g_txPush = 8;
		}
		//06 (0x06) Write Single Register
		else if(rxBuf[1] == 0x06)
		{
			if(rxPush != 8)
				return;
			unsigned int starting_address = (rxBuf[2] << 8) | rxBuf[3];
			if(starting_address > MODBUS_BUFFER_LEN)//ExceptionCode = 02
			{
				g_txPush = 5;
				g_txBuf[1] = 0x86;
				g_txBuf[2] = 0x02;
				return;
			}
			MODBUS_REG(starting_address) = (rxBuf[4] << 8) | rxBuf[5];
			g_txBuf[1] = rxBuf[1];
			g_txBuf[2] = rxBuf[2];
			g_txBuf[3] = rxBuf[3];
			g_txBuf[4] = rxBuf[4];
			g_txBuf[5] = rxBuf[5];
			g_txPush = 8;
		}
		//15 (0x0F) Write Multiple Coils
		else if(rxBuf[1] == 0x0F)
		{
			if(rxPush < 10)
				return;
			unsigned short starting_address = (rxBuf[2] << 8) | rxBuf[3];
			unsigned int regs_count = (rxBuf[4] << 8) | rxBuf[5];
			unsigned int byte_count = rxBuf[6];
			if((regs_count < 1) || (regs_count > 1968) || (((regs_count + 7) / 8) != byte_count))//ExceptionCode = 03
			{
				g_txPush = 5;
				g_txBuf[1] = 0x8F;
				g_txBuf[2] = 0x03;
				return;
			}
			if(((starting_address + regs_count + 15) / 16) > MODBUS_BUFFER_LEN)//ExceptionCode = 02
			{
				g_txPush = 5;
				g_txBuf[1] = 0x8F;
				g_txBuf[2] = 0x02;
				return;
			}
			g_txBuf[1] = rxBuf[1];
			g_txBuf[2] = rxBuf[2];
			g_txBuf[3] = rxBuf[3];
			g_txBuf[4] = rxBuf[4];
			g_txBuf[5] = rxBuf[5];
			for(unsigned int j = 0; j < regs_count; j++)
			{
				if(rxBuf[7 + j / 8] & (1 << (j % 8)))
					SET_COIL(starting_address + j)
				else
					CLR_COIL(starting_address + j)
			}
			g_txPush = 8;
		}
		//16 (0x10) Write Multiple registers
		else if(rxBuf[1] == 0x10)
		{
			if(rxPush < 11)
				return;
			unsigned int starting_address = (rxBuf[2] << 8) | rxBuf[3];
			unsigned int regs_count = (rxBuf[4] << 8) | rxBuf[5];
			unsigned int byte_count = rxBuf[6];
			if((regs_count < 1) || (regs_count > 123) || ((regs_count * 2) != byte_count))//ExceptionCode = 03
			{
				g_txPush = 5;
				g_txBuf[1] = 0x90;
				g_txBuf[2] = 0x03;
				return;
			}
			if((starting_address + regs_count) > MODBUS_BUFFER_LEN)//ExceptionCode = 02
			{
				g_txPush = 5;
				g_txBuf[1] = 0x90;
				g_txBuf[2] = 0x02;
				return;
			}
			for(unsigned int i = 0; i < regs_count; i++)
				MODBUS_REG(starting_address + i) = (rxBuf[7 + i * 2] << 8) | rxBuf[8 + i * 2];
			g_txBuf[1] = rxBuf[1];
			g_txBuf[2] = rxBuf[2];
			g_txBuf[3] = rxBuf[3];
			g_txBuf[4] = rxBuf[4];
			g_txBuf[5] = rxBuf[5];
			g_txPush = 8;
		}

		//������� 0x07 Set RS485 Device Address
		else if(rxBuf[1] == 0x07)
		{
			S_SLAVE_ADDRESS = rxBuf[2];
			//SaveRegsToEEPROM();
			g_txPush = 4;
		}

		// 0x71 SaveRegsToEEPROM() со стиранием страницы
		else if(rxBuf[1] == 0x71)
		{

			//SaveRegsToFLASH();
	                //////////////////////////
	                //Pack_Word(BlockFlash);
	                /////////////////////////
	                uint32_t result = SaveBlockToFLASH(rxBuf[2]*256+rxBuf[3],rxBuf[4]*256+rxBuf[5],126,1);
                   // result = 0x12345678;
	                g_txBuf[0] = rxBuf[0];  // Addr
	    			g_txBuf[1] = rxBuf[1];  // func 0x71

	    			g_txBuf[2] = (result >> 24) & 0xff; // result
	    			g_txBuf[3] = (result >> 16) & 0xff;
	    			g_txBuf[4] = (result >> 8) & 0xff;
	    			g_txBuf[5] = result & 0xff;

	                ///////////////////////
	                g_txPush = 8;

	                 /* Reload IWDG counter = 1*/
	                // IWDG ����� 0.8 �� ������������ ����������
	                /* Enable write access to IWDG_PR and IWDG_RLR registers */
	//                IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);

	                /* Set counter reload value to 1 */
	//                IWDG_SetReload(RLR_IWDG_IMMID);

	                /* Reload IWDG counter */
	//                IWDG_ReloadCounter();
		}
		// 0x72 AddRegsToEEPROM() без стирания страницы
		else if(rxBuf[1] == 0x72){
            //////////////////////////
            //Pack_Word(BlockFlash);
            /////////////////////////
            uint32_t result = AddBlockToFLASH(rxBuf[2]*256+rxBuf[3],rxBuf[4]*256+rxBuf[5],126,rxBuf[6]*256+rxBuf[7]);
           // result = 0x12345678;
            g_txBuf[0] = rxBuf[0];  // Addr
			g_txBuf[1] = rxBuf[1];  // func 0x72

			g_txBuf[2] = (result >> 24) & 0xff; // result
			g_txBuf[3] = (result >> 16) & 0xff;
			g_txBuf[4] = (result >> 8) & 0xff;
			g_txBuf[5] = result & 0xff;

            ///////////////////////
            g_txPush = 8;

             /* Reload IWDG counter = 1*/
            // IWDG ����� 0.8 �� ������������ ����������
            /* Enable write access to IWDG_PR and IWDG_RLR registers */
//                IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);

            /* Set counter reload value to 1 */
//                IWDG_SetReload(RLR_IWDG_IMMID);

            /* Reload IWDG counter */
//                IWDG_ReloadCounter();
		}

}
//##################################################################################################
// декодирвание команд  Modbus полученых от мастера и формирование ответа
//##################################################################################################
void ModBusDecode(uint8_t* g_rxBuf, uint8_t g_rxPush)
{
// uint8_t send_USART2[32]
// uint8_t receive_USART2[32]
    
	g_txBuf[0] = S_SLAVE_ADDRESS;
	g_txBuf[1] = g_rxBuf[1];
	//01 (0x01) Read Coils
	if(g_rxBuf[1] == 0x01)
	{
		if(g_rxPush != 8)  return; // ������ �� �����
                
		unsigned int starting_address = (g_rxBuf[2] << 8) | g_rxBuf[3];
		unsigned int regs_count = (g_rxBuf[4] << 8) | g_rxBuf[5];
		if((regs_count < 1) || (regs_count > 0x20))//ExceptionCode = 03
		{
			g_txPush = 5;
			g_txBuf[1] = 0x81;
			g_txBuf[2] = 0x03;
			return;
		}
		if(((starting_address + regs_count + 15) / 16) > MODBUS_BUFFER_LEN)//ExceptionCode = 02
		{
			g_txPush = 5;
			g_txBuf[1] = 0x81;
			g_txBuf[2] = 0x02;
			return;
		}
		g_txBuf[1] = 0x01;
		g_txBuf[2] = (regs_count + 7) / 8;
		for(unsigned int j = 0; j < g_txBuf[2]; j++)
			g_txBuf[3 + j] = 0;
		for(unsigned int j = 0; j < regs_count; j++)
			g_txBuf[3 + j / 8] |= READ_COIL(starting_address + j) << (j % 8);
		g_txPush = 5 + g_txBuf[2];
	}
	//03 (0x03) Read Holding Registers
	else if(g_rxBuf[1] == 0x03)
	{
		if(g_rxPush != 8)
			return;
		unsigned int starting_address = (g_rxBuf[2] << 8) | g_rxBuf[3];
		unsigned int regs_count = (g_rxBuf[4] << 8) | g_rxBuf[5];
		if((regs_count < 1) || (regs_count > 125))//ExceptionCode = 03
		{
			g_txPush = 5;
			g_txBuf[1] = 0x83;
			g_txBuf[2] = 0x03;
			return;
		}
		if((starting_address + regs_count) > MODBUS_BUFFER_LEN)//ExceptionCode = 02
		{
			g_txPush = 5;
			g_txBuf[1] = 0x83;
			g_txBuf[2] = 0x02;
			return;
		}
		g_txBuf[1] = 0x03;
		g_txBuf[2] = 2 * regs_count;
		for(unsigned int i = 0; i < regs_count; i++)
		{

			g_txBuf[3 + i * 2] = (MODBUS_REG(starting_address + i) >> 8) & 0xFF;
			g_txBuf[4 + i * 2] =  MODBUS_REG(starting_address + i) & 0xFF;
		}
		g_txPush = 5 + g_txBuf[2];
	}
	//05 (0x05) Write Single Coil
	else if(g_rxBuf[1] == 0x05)
	{
		if(g_rxPush != 8)
			return;
		unsigned short starting_address = (g_rxBuf[2] << 8) | g_rxBuf[3];
		unsigned short value = (g_rxBuf[4] << 8) | g_rxBuf[5];
		if((value != 0x0000) && (value != 0xFF00))//ExceptionCode = 03
		{
			g_txPush = 5;
			g_txBuf[1] = 0x85;
			g_txBuf[2] = 0x03;
			return;
		}
		if(((starting_address + 15) / 16) > MODBUS_BUFFER_LEN)//ExceptionCode = 02
		{
			g_txPush = 5;
			g_txBuf[1] = 0x85;
			g_txBuf[2] = 0x02;
			return;
		}
		g_txBuf[1] = g_rxBuf[1];
		g_txBuf[2] = g_rxBuf[2];
		g_txBuf[3] = g_rxBuf[3];
		g_txBuf[4] = g_rxBuf[4];
		g_txBuf[5] = g_rxBuf[5];
		if(value == 0xFF00)
			SET_COIL(starting_address)
		else
			CLR_COIL(starting_address)
		g_txPush = 8;
	}
	//06 (0x06) Write Single Register
	else if(g_rxBuf[1] == 0x06)
	{
		if(g_rxPush != 8)
			return;
		unsigned int starting_address = (g_rxBuf[2] << 8) | g_rxBuf[3];
		if(starting_address > MODBUS_BUFFER_LEN)//ExceptionCode = 02
		{
			g_txPush = 5;
			g_txBuf[1] = 0x86;
			g_txBuf[2] = 0x02;
			return;
		}
		MODBUS_REG(starting_address) = (g_rxBuf[4] << 8) | g_rxBuf[5];
		g_txBuf[1] = g_rxBuf[1];
		g_txBuf[2] = g_rxBuf[2];
		g_txBuf[3] = g_rxBuf[3];
		g_txBuf[4] = g_rxBuf[4];
		g_txBuf[5] = g_rxBuf[5];
		g_txPush = 8;
	}
	//15 (0x0F) Write Multiple Coils
	else if(g_rxBuf[1] == 0x0F)
	{
		if(g_rxPush < 10)
			return;
		unsigned short starting_address = (g_rxBuf[2] << 8) | g_rxBuf[3];
		unsigned int regs_count = (g_rxBuf[4] << 8) | g_rxBuf[5];
		unsigned int byte_count = g_rxBuf[6];
		if((regs_count < 1) || (regs_count > 1968) || (((regs_count + 7) / 8) != byte_count))//ExceptionCode = 03
		{
			g_txPush = 5;
			g_txBuf[1] = 0x8F;
			g_txBuf[2] = 0x03;
			return;
		}
		if(((starting_address + regs_count + 15) / 16) > MODBUS_BUFFER_LEN)//ExceptionCode = 02
		{
			g_txPush = 5;
			g_txBuf[1] = 0x8F;
			g_txBuf[2] = 0x02;
			return;
		}
		g_txBuf[1] = g_rxBuf[1];
		g_txBuf[2] = g_rxBuf[2];
		g_txBuf[3] = g_rxBuf[3];
		g_txBuf[4] = g_rxBuf[4];
		g_txBuf[5] = g_rxBuf[5];
		for(unsigned int j = 0; j < regs_count; j++)
		{
			if(g_rxBuf[7 + j / 8] & (1 << (j % 8)))
				SET_COIL(starting_address + j)
			else
				CLR_COIL(starting_address + j)
		}
		g_txPush = 8;
	}
	//16 (0x10) Write Multiple registers
	else if(g_rxBuf[1] == 0x10)
	{
		if(g_rxPush < 11)
			return;
		unsigned int starting_address = (g_rxBuf[2] << 8) | g_rxBuf[3];
		unsigned int regs_count = (g_rxBuf[4] << 8) | g_rxBuf[5];
		unsigned int byte_count = g_rxBuf[6];
		if((regs_count < 1) || (regs_count > 123) || ((regs_count * 2) != byte_count))//ExceptionCode = 03
		{
			g_txPush = 5;
			g_txBuf[1] = 0x90;
			g_txBuf[2] = 0x03;
			return;
		}
		if((starting_address + regs_count) > MODBUS_BUFFER_LEN)//ExceptionCode = 02
		{
			g_txPush = 5;
			g_txBuf[1] = 0x90;
			g_txBuf[2] = 0x02;
			return;
		}
		for(unsigned int i = 0; i < regs_count; i++)
			MODBUS_REG(starting_address + i) = (g_rxBuf[7 + i * 2] << 8) | g_rxBuf[8 + i * 2];
		g_txBuf[1] = g_rxBuf[1];
		g_txBuf[2] = g_rxBuf[2];
		g_txBuf[3] = g_rxBuf[3];
		g_txBuf[4] = g_rxBuf[4];
		g_txBuf[5] = g_rxBuf[5];
		g_txPush = 8;
	}
        
	//������� 0x07 Set RS485 Device Address 
	else if(g_rxBuf[1] == 0x07)
	{
		S_SLAVE_ADDRESS = g_rxBuf[2];
		//SaveRegsToEEPROM();
		g_txPush = 4;
	}

	//������� 0x71 SaveRegsToEEPROM() 
	else if(g_rxBuf[1] == 0x71)
	{
		
		//SaveRegsToFLASH();
                //////////////////////////
                //Pack_Word(BlockFlash);
                /////////////////////////
                SaveBlockToFLASH(0,0,0,0);
                ///////////////////////
                g_txPush = 4;
                
                 /* Reload IWDG counter = 1*/
                // IWDG ����� 0.8 �� ������������ ����������
                /* Enable write access to IWDG_PR and IWDG_RLR registers */
//                IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
              
                /* Set counter reload value to 1 */
//                IWDG_SetReload(RLR_IWDG_IMMID);
              
                /* Reload IWDG counter */
//                IWDG_ReloadCounter();
	}

}

////##############################################################################
//// ������������� ������ ��2
////##############################################################################
//void DecodeCO2(uint8_t *CHAR, uint16_t* CO2, uint16_t* Temp)
//{
// //������� �� ������� ����� �����
// // ���� ��� �� ������� � �� ������ ������ �������� 
// for(char i=0;i<5;i++)
// {
// 
//  if(!((CHAR[i] >= 0x30)&&(CHAR[i] <= 0x39)&&(CHAR[5] == 0x20)&&(CHAR[i+6] >= 0x30)&&(CHAR[i+6] <= 0x39)))
//  return;  
//    
// }  
// *CO2 = (CHAR[4]-0x30) + (CHAR[3]-0x30)*10 + (CHAR[2]-0x30)*100 + 
//        (CHAR[1]-0x30)*1000 + (CHAR[0]-0x30)*10000;
// 
// *Temp = (CHAR[9]-0x30) + (CHAR[8]-0x30)*10 + 
//         (CHAR[7]-0x30)*100 + (CHAR[6]-0x30)*1000; 
//}
////------------------------------------------------------------

//##############################################################################
// ������������� ������ ��2
//##############################################################################
//char DecodeCO2(uint8_t *CHAR, uint16_t* CO2, uint16_t* Temp)
//{
// //������� �� ������� ����� �����d
// // ���� ��� �� ������� � �� ������ ������ ��������
// for(char i=0;i<5;i++)
// {
//
//  if(!((CHAR[i] >= 0x30)&&(CHAR[i] <= 0x39)&&(CHAR[5] == 0x20)&&(CHAR[i+6] >= 0x30)&&(CHAR[i+6] <= 0x39)
//       &&(CHAR[11] == 0x0d)))
//  return 0;
//
// }
// *CO2 = (CHAR[4]-0x30) + (CHAR[3]-0x30)*10 + (CHAR[2]-0x30)*100 +
//        (CHAR[1]-0x30)*1000 + (CHAR[0]-0x30)*10000;
//
// *Temp = (CHAR[9]-0x30) + (CHAR[8]-0x30)*10 +
//         (CHAR[7]-0x30)*100 + (CHAR[6]-0x30)*1000;
//
// return 1;
//}
//------------------------------------------------------------


//------------------------------------------------------------
void SaveRegsToFLASH()
{
//   // ���������� ��������� �� FLASH
//  uint32_t addr_flash = 0x0800fffc;
//  uint32_t data_flash = (uint32_t)MODBUS_REG(N_SAVED_REG);
//  write_WordFLASH(addr_flash, data_flash);
}
//------------------------------------------------------------

//------------------------------------------------------------
// чтение регистров из FLASH
//------------------------------------------------------------
void LoadRegsFromFLASH()
{
 uint32_t addr_flash = 0x0800fffc; 
 MODBUS_REG(N_SAVED_REG) = (uint16_t)((*(__IO uint32_t*)addr_flash)&(0xffff));
}
//------------------------------------------------------------

//*************************************************************
/**
  * @brief  запись блока во Flash со стиранием страниц по 2КB.
  * @param  uint16_t: startModbusReg -  start ModbusReg to save
  * @param  uint16_t: numRegs - 		number of Regs
  * @param  uint8_t:  Page -            номер страницы
  * @param  uint8_t: numPages - 		number of Pages
  * @retval 0 - success;
  * @retval 1 - error;
  */
//************************************************************
uint32_t SaveBlockToFLASH(uint16_t startModbusReg, uint16_t numReg, uint8_t Page, uint8_t numPages)
{
  // запись блока во FLASH
  uint32_t 	startAdr = Page * 2048 + 0x08000000;

	if((startAdr != 0x0803f000) && (startAdr != 0x0803e800) && (startAdr != 0x0803e000) && (startAdr != 0x0803d800) &&
	   (startAdr != 0x0803d000)	&& (startAdr != 0x0803c800) && (startAdr != 0x0803c000)){
		return startAdr; // wrong address page
	}

	if((startAdr + numPages * 0x800 - 1) > 0x0803f800){
		return 101; // over address page
	}

	//uint32_t startAddr = startAdr;
	HAL_StatusTypeDef result = HAL_FLASH_Unlock();
		if(result == HAL_OK){
		//	if(startAddr == END_ADDR_LAST_PAGE){
				// стираем страницу и выставляем для записи начальный адрес страницы
				static FLASH_EraseInitTypeDef EraseInitStruct;
				uint32_t PageError;
				uint8_t sofar = 0;
				EraseInitStruct.TypeErase = FLASH_TYPEERASE_PAGES;
				EraseInitStruct.PageAddress = startAdr;
				EraseInitStruct.NbPages = 1; //((END_ADDR_LAST_PAGE - START_ADDR_LAST_PAGE)/FLASH_PAGE_SIZE)+1;
				if(HAL_FLASHEx_Erase(&EraseInitStruct, &PageError) != HAL_OK){
					//TODO - handle error HAL_FLASHEx_Erase(&EraseInitStruct, &PageError) != HAL_OK
					return HAL_FLASH_GetError();
				}
//				startAddr = START_ADDR_126_PAGE;

			}

		    uint32_t wordFLASH;

	        // to pack (uint16_t)MODBUS_REG -> (uint32_t)
//			for(uint16_t i=0, j=0; i < numReg;i=i+2,j++  ){
//				  wordFLASH = (uint32_t)MODBUS_REG(startModbusReg + i) + ((uint16_t)MODBUS_REG(startModbusReg + i+1) << 16);
//			      HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, startAdr + j*4, wordFLASH/*(uint32_t)MODBUS_REG(startModbusReg + i)*/);  // Set Point
//			}
		    			for(uint16_t i=0; i < numReg;i++){
		    			      HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, startAdr + i*2, (uint16_t)MODBUS_REG(startModbusReg + i));  // Set Point
		    			}

		      HAL_FLASH_Lock();

		      return 0;
}
//------------------------------------------------------------
//*************************************************************
/**
  * @brief  добавление блока во Flash без стирания страницы.
  * @param  uint16_t: startModbusReg -  start ModbusReg to save
  * @param  uint16_t: numRegs - 		number of Regs
  * @param  uint8_t:  Page -            номер страницы
  * @param  uint16_t: shiftAddr - 		смещение адреса в странице
  * @retval 0 - success;
  * @retval > 0 - error;
  */
//************************************************************
uint32_t AddBlockToFLASH(uint16_t startModbusReg, uint16_t numReg, uint8_t Page, uint16_t shiftAddr)
{
  // запись блока во FLASH
  uint32_t 	startAdr = Page * 2048 + shiftAddr;

  if((Page > 126) || (Page < 116)){
	  return 100; // number page is out
  }

	if((shiftAddr + numReg * 2 + 1) > 0x800){
		return 101; // over address page
	}

	HAL_StatusTypeDef result = HAL_FLASH_Unlock();
		if(result != HAL_OK){
			return HAL_FLASH_GetError();
			}

	    uint32_t wordFLASH;

        // to pack (uint16_t)MODBUS_REG -> (uint32_t)
//		for(uint16_t i=0, j=0; i < numReg;i=i+2,j++  ){
//			  wordFLASH = (uint32_t)MODBUS_REG(startModbusReg + i) + ((uint16_t)MODBUS_REG(startModbusReg + i+1) << 16);
//		      HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, startAdr + j*4, wordFLASH/*(uint32_t)MODBUS_REG(startModbusReg + i)*/);  // Set Point
//		}

//			for(uint16_t i=0; i < numReg;i++ ){
//			      HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, startAdr + i*4, (uint32_t)MODBUS_REG(startModbusReg + i));  // Set Point
//			}
		for(uint16_t i=0; i < numReg;i++){
		      HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, startAdr + i*2, (uint16_t)MODBUS_REG(startModbusReg + i));  // Set Point
		}

		      HAL_FLASH_Lock();

		      return 0;
}
//------------------------------------------------------------

//------------------------------------------------------------
void LoadBlockFromFLASH()
{
// uint16_t x, y, z, t, u, m;
// uint32_t addr_flash = START_ADDR_LAST_PAGE;
// for(uint8_t i=START_REG_MODBUS_SAVE;i<START_REG_MODBUS_SAVE+N_REG_MODBUS_SAVE;i=i+2)
// {
//  MODBUS_REG(i) = (uint16_t)((*(__IO uint32_t*)addr_flash)&(0xffff));
//  x=(uint16_t)((*(__IO uint32_t*)addr_flash)&(0xffff));
//
//  MODBUS_REG(i+1) = (uint16_t)(((*(__IO uint32_t*)addr_flash) >> 16)&(0xffff));
//  y = (uint16_t)(((*(__IO uint32_t*)addr_flash) >> 16)&(0xffff));
//
//  addr_flash += 4;
////////////////////////////////////////////
//  printf("%x %x %x\n",x, y, addr_flash);
////////////////////////////////////////////
// }
// x = (uint16_t)((*(__IO uint32_t*)0x0800FC00)&(0xffff));
// y = (uint16_t)((*(__IO uint32_t*)0x0800FC00)&(0xffff));
// z = (uint16_t)((*(__IO uint32_t*)0x0800FC00)&(0xffff));
// t = (uint16_t)((*(__IO uint32_t*)addr_flash)&(0xffff));
// u = (uint16_t)((*(__IO uint32_t*)addr_flash)&(0xffff));
// m = (uint16_t)((*(__IO uint32_t*)addr_flash)&(0xffff));
// printf("%x %x %x %x %x %x %x\n",x, y, z, t,u,m,addr_flash);
}

//------------------------------------------------------------
void Delay_us_10(__IO uint32_t nTime)
{ 
//  TimingDelay = nTime;
//
//  while(TimingDelay != 0);
}
//------------------------------------------------------------
void TimingDelay_Decrement()
{
//  if (TimingDelay != 0x00)
//  {
//    TimingDelay--;
//  }
}
//------------------------------------------------------------
/////////////////////////////////////////////////////////////////
/**
  * @brief  Преобразование числа в шеснацатиричную строку.
  * @param [in]  param int число
  * @param [out] str char* - указатель на полученую строку
  */
void HexToStr(char* str, int param)
{
         if(((param & 0xF0) >>4 ) > 9) 
           str[2] = ((param & 0xF0) >>4 ) + 0x41 - 0x0a;
         else str[2] = ((param & 0xF0) >>4 ) + 0x30;
         
         if((param & 0x0F) > 9) 
           str[3] = (param & 0x0f)  + 0x41 - 0x0a;
         else str[3] = (param & 0x0f)  + 0x30;

         if(((param & 0xF000) >> 12) > 9) 
           str[0] = ((param & 0xF000) >>12 ) + 0x41 - 0x0a;
         else str[0] = ((param & 0xF000) >>12 ) + 0x30;
         
         if(((param & 0x0f00) >> 8) > 9)  
           str[1] = ((param & 0x0f00) >> 8) + 0x41 - 0x0a;
         else str[1] = ((param & 0x0f00) >> 8)  + 0x30;         
}

///////////////////////////////////////////////////////////////////////////////
void IntToStr_16(char* str, int16_t dat)
{
 if(dat >= 0)
 {
  str[0] = 0x20; 
  str[1] = (dat/10000)+0x30;
  str[2] = (dat - (dat/10000)*10000)/1000 + 0x30;
  str[3] = (dat - (dat/1000)*1000)/100 + 0x30;  
  str[4] = (dat - (dat/100)*100)/10 + 0x30;  
  str[5] = dat%10 + 0x30;
  
  if(str[1] == 0x30) str[1]=0x20;
  if((str[1] == 0x20)&&(str[2]==0x30)) str[2]=0x20;
  if((str[1] == 0x20)&&(str[2]==0x20)&&(str[3]==0x30)) str[3]=0x20;
  if((str[1] == 0x20)&&(str[2]==0x20)&&(str[3]==0x20)&&(str[4]==0x30)) str[4]=0x20;

 }
 else if(dat < 0)
 {
  dat = dat*(-1);
  str[0] = 0x2d;
  str[1] = (dat/10000)+0x30;
  str[2] = (dat - (dat/10000)*10000)/1000 + 0x30;
  str[3] = (dat - (dat/1000)*1000)/100 + 0x30;  
  str[4] = (dat - (dat/100)*100)/10 + 0x30;  
  str[5] = dat%10 + 0x30;


  if(str[1] == 0x30) {str[0]=0x20; str[1]=0x2d;}
  if((str[1] == 0x2d)&&(str[2]==0x30)) {str[1]=0x20; str[2]=0x2d;}
  if((str[1] == 0x20)&&(str[2]==0x2d)&&(str[3]==0x30)) {str[2]=0x20; str[3]=0x2d;}
  if((str[1] == 0x20)&&(str[2]==0x20)&&(str[3]==0x2d)&&(str[4]==0x30)) 
  {
    str[3]=0x20;
    str[4]=0x2d;
  }  
  
  //if(str[1] == 0x30)
  //{
  // str[0] = 0x20;
  // str[1] = 0x2d;  
  //}  
 }  // end else

  
}

///////////////////////////////////////////////////////////////////////
void IntToStr_100(char* str, int16_t dat)
{
 if(dat > 100) dat = 100;
 if(dat < -100) dat = -100;
 
 if(dat >= 0)
 {
  str[0] = (dat/100)+0x30;
  str[1] = (dat - (dat/100)*100)/10 + 0x30;
  str[2] = dat%10 + 0x30;

 }
 else if(dat < 0)
 {
  str[0] = 0x2d;
  dat = dat*(-1);
  str[1] = (dat - (dat/100)*100)/10 + 0x30;
  str[2] = dat%10 + 0x30;

 }  
}

///////////////////////////////////////////////////////////////////////////////

void IntToStr(char* str, signed char dat)
{ 
 if(dat >= 0)
 {
  str[0] = (dat/100)+0x30;
  str[1] = (dat - (dat/100)*100)/10 + 0x30;
  str[2] = dat%10 + 0x30;
  if(str[0] == 0x30) str[0]=0x20;
  if((str[0] == 0x20)&&(str[1]==0x30)) str[1]=0x20;
 }
 else if(dat < 0)
 {
  str[0] = 0x2d;
  dat = dat*(-1);
  str[1] = (dat - (dat/100)*100)/10 + 0x30;
  str[2] = dat%10 + 0x30;
  if(str[1] == 0x30)
  {
   str[0] = 0x20;
   str[1] = 0x2d;  
  }  
 }  
}
//==================================================================
// New 
// void Change_str_Down();
//void Change_str_Up();
//void Change_str_Right();
//void Change_str_ESC();
//void Change_str_Enter();
//==================================================================
//void ChangeStrDown()
//{
//   int16_t regStart;
//   int16_t reg;
//   int16_t bit;
//
// if(menu.cursorAllowed[(menu.mode_Menu & BIT_SUB_MENU) >> 4] == 1)
// {
//  // move menu
//  if((menu.mode_Menu & BIT_LINE) > 0) menu.mode_Menu--;
//  else menu.mode_Menu = (menu.mode_Menu & 0xfff0) + menu.pointsOfMenu[(menu.mode_Menu & BIT_SUB_MENU) >> 4];
//
//  // if View changed set BIT_VIEW_CH
//  if(IsViewMenuChanged())   menu.mode_Menu |= BIT_VIEW_CH; // bChange_View = 1
//  else menu.mode_Menu &= (~BIT_VIEW_CH);
// }
// else if(menu.cursorAllowed[(menu.mode_Menu & BIT_SUB_MENU) >> 4] == 2)
// {
//  // change parametrs
//  if((menu.mode_Menu & BIT_SUB_MENU) == MENU_SET_POINTS)
//  {
//   // change Set Pionts
//   // ...
//
//   regStart = 0x96; // 0x3a - 0x49
//   reg = regStart + (menu.mode_Menu & BIT_LINE);
//
//   MODBUS_REG(reg) += 1;
//   SetBIT_UPDATE_VALUE();
//   printf("reg[%1x] = %4x\n",reg,MODBUS_REG(reg));
//
//  }
//  if((menu.mode_Menu & BIT_SUB_MENU) == MENU_ANALOG_OUT)
//  {
//   // change Analog Out
//   //...
//   // (int16_t)MODBUS_REG(reg + menu.numScreen*3 + i-1)
//   regStart = 0x3a; // 0x3a - 0x49
//   reg = regStart + (menu.mode_Menu & BIT_LINE);
//
//   MODBUS_REG(reg) += 1;
//   SetBIT_UPDATE_VALUE();
//   printf("reg[%1x] = %4x\n",reg,MODBUS_REG(reg));
//  }
//
//  if((menu.mode_Menu & BIT_SUB_MENU) == MENU_DIGITAL_OUT)
//  {
//   // change DIGITAL OUT
//   //...
//   // (int16_t)MODBUS_REG(reg + menu.numScreen*3 + i-1)
//   regStart = 0x28; // 0x3a - 0x49
//   bit = 1<<(menu.mode_Menu & BIT_LINE);
//
//   MODBUS_REG(regStart) |= bit;
//   SetBIT_UPDATE_VALUE();
//   printf("reg[%1x] = %4x\n",reg,MODBUS_REG(reg));
//  }
// }
// printf("menu.numScreen = %4x\n",menu.numScreen);
// printf("menu.mode_Menu = %4x\n", menu.mode_Menu);
//

//}


//void ChangeStrUp()
//{
//   int16_t regStart;
//   int16_t reg;
//   int16_t bit;
//
//  if(menu.cursorAllowed[(menu.mode_Menu & BIT_SUB_MENU) >> 4] == 1)
//  {
//   // move menu
//   if((menu.mode_Menu & BIT_LINE) < menu.pointsOfMenu[(menu.mode_Menu & BIT_SUB_MENU) >> 4]) menu.mode_Menu++;
//   else menu.mode_Menu &= 0xfff0;
//
//   if(IsViewMenuChanged())   menu.mode_Menu |= BIT_VIEW_CH; // bChange_View = 1
//   else menu.mode_Menu &= (~BIT_VIEW_CH);
//  }
//  else if(menu.cursorAllowed[(menu.mode_Menu & BIT_SUB_MENU) >> 4] == 2)
//  {
//   // change value
//
//    if((menu.mode_Menu & BIT_SUB_MENU) == MENU_SET_POINTS)
//    {
//     // change Set Pionts
//     regStart = 0x96; // 0x3a - 0x49
//     reg = regStart + (menu.mode_Menu & BIT_LINE);
//
//   MODBUS_REG(reg) -= 1;
//   SetBIT_UPDATE_VALUE();
//   printf("reg[%1x] = %4x\n",reg,MODBUS_REG(reg));
//
//    }
//
//    if((menu.mode_Menu & BIT_SUB_MENU) == MENU_ANALOG_OUT)
//    {
//   // change Analog Out
//   //...
//   // (int16_t)MODBUS_REG(reg + menu.numScreen*3 + i-1)
//   regStart = 0x3a; // 0x3a - 0x49
//   reg = regStart + (menu.mode_Menu & BIT_LINE);
//
//   MODBUS_REG(reg) -= 1;
//   SetBIT_UPDATE_VALUE();
//   printf("reg[%1x] = %4x\n",reg,MODBUS_REG(reg));
//    }
//
//   if((menu.mode_Menu & BIT_SUB_MENU) == MENU_DIGITAL_OUT)
//   {
//    // change DIGITAL OUT
//    //...
//    // (int16_t)MODBUS_REG(reg + menu.numScreen*3 + i-1)
//    regStart = 0x28; // 0x3a - 0x49
//    bit = 1<<(menu.mode_Menu & BIT_LINE);
//
//    MODBUS_REG(regStart) &= (~bit);
//    SetBIT_UPDATE_VALUE();
//    printf("reg[%1x] = %4x\n",reg,MODBUS_REG(reg));
//   }
//  }
// printf("menu.numScreen = %4x\n",menu.numScreen);
// printf("menu.mode_Menu = %4x\n", menu.mode_Menu);
//

//}


//void ChangeStrRight()
//{

//  menu.mode_Menu = menu.mode_Menu | BIT_VIEW_CH | BIT_MENU_CH; // bChange_View = 1
//
//
// if((menu.mode_Menu & BIT_SUB_MENU) == 0)
// {
//  // Enter to the deep Menu
//  menu.mode_Menu = ((menu.mode_Menu | ((menu.mode_Menu & BIT_LINE) << 4)) + 0x0010) & (~BIT_LINE);
//
//  printf("menu.mode_Menu=%4x\n",menu.mode_Menu);
// }
// else if(((menu.mode_Menu & BIT_SUB_MENU) == MENU_STATUS) || ((menu.mode_Menu & BIT_SUB_MENU) == MENU_ANALOG_IN))
// {
//  // exit to the up Menu
//
//   ClearLCDScreen();
//   menu.mode_Menu = menu.mode_Menu & (((menu.mode_Menu & BIT_SUB_MENU) >> 4) + 0x0001);
//   menu.mode_Menu = menu.mode_Menu & (~BIT_SUB_MENU);
//   menu.mode_Menu = menu.mode_Menu | BIT_VIEW_CH | BIT_MENU_CH;
// }
// else if(((menu.mode_Menu & BIT_SUB_MENU) == MENU_SAVE_SP) && ((menu.mode_Menu & BIT_LINE) == 0))
// {
//  // exit without save
//   ChangeStrESC();
// }
// else if(((menu.mode_Menu & BIT_SUB_MENU) == MENU_SAVE_SP) && ((menu.mode_Menu & BIT_LINE) == 1))
// {
//  // save and exit
//  SaveBlockToFLASH();
//  menu.countDurTempMenu = 0;
//  menu.mode_Menu &= (~BIT_SUB_MENU);
//  menu.mode_Menu |= MENU_TEMP1;
//  SetBIT_VIEW_CH();
// }
//
 
//}
//------------------------------------------------------------------
//void ChangeStrESC()
//{
//  menu.mode_Menu = menu.mode_Menu | BIT_VIEW_CH | BIT_MENU_CH; // bChange_View = 1
//
//  ClearData();
//  if((menu.mode_Menu & BIT_SUB_MENU) == MENU_VERSION)
//  {
//   // Clear Screen
//   ClearLCDScreen();
//  }
//
//
// if((menu.mode_Menu & BIT_SUB_MENU) != 0)
// {
//  uint16_t n;
//  //cursorAllowed = 1
//    if(menu.cursorAllowed[(menu.mode_Menu & BIT_SUB_MENU) >> 4] == 2)
//       menu.cursorAllowed[(menu.mode_Menu & BIT_SUB_MENU) >> 4] = 1;
//
//  // ESC go to the up Menu
//  n = ((menu.mode_Menu & BIT_SUB_MENU) >> 4)-1;
//  menu.mode_Menu = menu.mode_Menu & (~BIT_SUB_MENU) & (~BIT_LINE);
//  menu.mode_Menu |= n;
// }
//
// printf("menu.mode_Menu = %4x\n", menu.mode_Menu);

//}

//void ChangeStrEnter()
//{
// menu.mode_Menu = menu.mode_Menu | BIT_VIEW_CH | BIT_MENU_CH; // bChange_View = 1
//
//      if(((menu.mode_Menu & BIT_SUB_MENU) == MENU_SET_POINTS) ||
//         ((menu.mode_Menu & BIT_SUB_MENU) == MENU_DIGITAL_OUT) ||
//         ((menu.mode_Menu & BIT_SUB_MENU) == MENU_ANALOG_OUT))
//      {
//        if((menu.cursorAllowed[(menu.mode_Menu & BIT_SUB_MENU) >> 4] == 1)
//           && (menu.cursorType[(menu.mode_Menu & BIT_SUB_MENU) >> 4] == 2))
//          menu.cursorAllowed[(menu.mode_Menu & BIT_SUB_MENU) >> 4] = 2;
//        else if(menu.cursorAllowed[(menu.mode_Menu & BIT_SUB_MENU) >> 4] == 2)
//        {
//          menu.cursorAllowed[(menu.mode_Menu & BIT_SUB_MENU) >> 4] = 1;
//          // save reg
//          //
//        }
//      }
//
//      if((menu.mode_Menu & BIT_SUB_MENU) == MENU_UNIT_TYPE)
//      {
//       //
//       // uint16_t n;
//       menu.unitType = menu.mode_Menu & BIT_LINE;
//       menu.strMenu[0][9] = menu.strMenu[10][menu.unitType];  //"Unit Type  ";
//
//       MODBUS_REG(START_REG_MODBUS_SAVE+0x25) &= 0xff0f;
//       MODBUS_REG(START_REG_MODBUS_SAVE+0x25) |= (menu.unitType << 4) & 0xf0;
//       ChangeStrESC();
//      }
//
//      if((menu.mode_Menu & BIT_SUB_MENU) == MENU_UNIT_MODE)
//      {
//       //
//       menu.unitMode = menu.mode_Menu & BIT_LINE;
//       menu.strMenu[0][10] =menu.strMenu[11][menu.unitMode];  //"Mode       ";
//       MODBUS_REG(START_REG_MODBUS_SAVE+0x25) &= 0xfffc;
//       MODBUS_REG(START_REG_MODBUS_SAVE+0x25) |= menu.unitMode & 0x03;
//       ChangeStrESC();
//      }
//}


//void ChangeStrAlarm()
//{
 //...
//  menu.mode_Menu &= (~BIT_SUB_MENU);
//  menu.mode_Menu |= MENU_ALARM;
//  menu.mode_Menu = menu.mode_Menu | BIT_VIEW_CH | BIT_MENU_CH;
//}
//==================================================================




///////////////////////////////////////////////////////////////////////////////
//  Execute button Up
///////////////////////////////////////////////////////////////////////////////
//uint16_t Change_str_Up(uint16_t imenu)
//{
//
// if(IsScreenMenuChanged(g_mode_Menu))   imenu = imenu | BIT_VIEW_CH; // bChange_View = 1
// else imenu = imenu & (~BIT_VIEW_CH);
//  /*
// if(((imenu & BIT_LINE) == 2) || ((imenu & BIT_LINE) == 5) || ((imenu & BIT_LINE) == 7))
// {
//  imenu = imenu | BIT_VIEW_CH; // bChange_View = 1
// }
//*/
// if((imenu & BITS_LINE) < 0x0e) imenu++;
// else imenu = imenu & 0xfff0;
//
//  if(((imenu & BITS_SUB_MENU) == MENU_SET_POINTS) && (imenu & BIT_CH_ALLOW))
//  {
//   // change Set Pionts
//
//  }
//
//  if(((imenu & BIT_SUB_MENU) == MENU_ANALOG_OUT) && (imenu & BIT_CH_ALLOW))
//  {
//   // change Analog OUT
//
//  }
// printf("menu.numScreen = %4x\n",menu.numScreen);
// printf("imenu = %4x\n", imenu);

// return imenu;
//}

///////////////////////////////////////////////////////////////////////////////
//  Execute button Down
///////////////////////////////////////////////////////////////////////////////
//uint16_t Change_str_Down(uint16_t imenu)
//{
// if(IsScreenMenuChanged(g_mode_Menu))   imenu = imenu | BIT_VIEW_CH; // bChange_View = 1
// else imenu = imenu & (~BIT_VIEW_CH);
// /*
// if(((imenu & BIT_LINE) == 0) || ((imenu & BIT_LINE) == 3) || ((imenu & BIT_LINE) == 6))
// {
//  imenu = imenu | BIT_VIEW_CH; // bChange_View = 1
// }
//  */
// if((imenu & BIT_LINE) > 0) imenu--;
// else imenu = imenu & 0xfffe;
//
//  if(((imenu & BIT_SUB_MENU) == MENU_SET_POINTS) && (imenu & BIT_CH_ALLOW))
//  {
//   // change Set Pionts
//   // ...
//  }
//  if(((imenu & MENU_ANALOG_OUT) == MENU_SET_POINTS) && (imenu & BIT_CH_ALLOW))
//  {
//   // change Analog Out
//   //...
//  }
// printf("menu.numScreen = %4x\n",menu.numScreen);
// printf("imenu = %4x\n", imenu);
// return imenu;
  
//}

///////////////////////////////////////////////////////////////////////////////
// Execute button Right. Enter to the deep Menu
//////////////////////////////////////////////////////////////////////
uint16_t Change_str_Right(uint16_t imenu)
{

//  imenu = imenu | BIT_VIEW_CH | BIT_MENU_CH; // bChange_View = 1
//
//
// if((imenu & BIT_SUB_MENU) == 0)
// {
//  // Enter to the deep Menu
//  //menu = (menu | (4 <<((menu & BIT_LINE)+1))) & (~BIT_LINE);
////  printf("x=%4x\n",(menu & BIT_LINE));
////  printf("y=%4x\n",((menu & BIT_LINE) << 4));
////  printf("z=%4x\n",~BIT_LINE);
////  printf("a=%4x\n",(menu | ((menu & BIT_LINE) << 4)));
//
////  printf("menu_a=%4x\n",menu);
////  printf("menu_b=%4x\n",(menu | ((menu & BIT_LINE) << 4)));
////  printf("menu_c=%4x\n",(menu | ((menu & BIT_LINE) << 4)) + 0x10);
//  imenu = ((imenu | ((imenu & BIT_LINE) << 4)) + 0x0010) & (~BIT_LINE);
////  printf("menu_0=%4x\n",menu);
// }
//
//  if(((imenu & BIT_SUB_MENU) == MENU_DIGITAL_OUT) && (imenu & BIT_CH_ALLOW))
//  {
//    // Invert Didital OUT bit
//    MODBUS_REG(0x2a) = MODBUS_REG(0x2a) ^ (1 << (imenu & BIT_LINE));
//  }
//
//  if(((imenu & BIT_SUB_MENU) == MENU_SET_POINTS) && (imenu & BIT_CH_ALLOW))
//  {
//   // move along Set Pionts
//   //g_mode_Menu = ((g_mode_Menu & BITS_CURSOR) >> 11) + 1
//    if(((g_mode_Menu & BITS_CURSOR) >> 11) == 0)
//    {
//     g_mode_Menu = g_mode_Menu + 0x0800;
//    }
//    else if(((g_mode_Menu & BITS_CURSOR) >> 11) == 1)
//    {
//     g_mode_Menu = g_mode_Menu + 0x0800;
//    }
//    else if(((g_mode_Menu & BITS_CURSOR) >> 11) == 2)
//    {
//      g_mode_Menu = g_mode_Menu + 0x0800;
//    }
//    else if(((g_mode_Menu & BITS_CURSOR) >> 11) == 3)
//    {
//        g_mode_Menu = g_mode_Menu & (~BITS_CURSOR);
//    }
//  }
//
// printf("imenu = %4x\n", imenu);
 return imenu;
  
}

///////////////////////////////////////////////////////////////////////////////
// ESC go to the up Menu
//////////////////////////////////////////////////////////////////////
uint16_t Change_str_ESC(uint16_t imenu)
{

//  imenu = imenu | BIT_VIEW_CH | BIT_MENU_CH; // bChange_View = 1
//
//
// if((imenu & BIT_SUB_MENU) != 0)
// {
//  // ESC go to the up Menu
//  imenu = imenu & (~BIT_SUB_MENU);
// }
//
// printf("imenu = %4x\n", imenu);
 return imenu;
  
}

///////////////////////////////////////////////////////////////////////////////
// Execute button Enter
//////////////////////////////////////////////////////////////////////
uint16_t Change_str_Enter(uint16_t imenu)
{

//   // If we changed already
//   if((imenu & BIT_CH_ALLOW) && ((imenu | BIT_SUB_MENU)== MENU_SET_POINTS))
//   {
//     // Save registers SetPoint0 - Save registers SetPoint14
//     // ...
//   }
//
//  imenu = imenu | BIT_VIEW_CH | BIT_MENU_CH; // bChange_View = 1
//
//  //
//  if(((imenu | BIT_SUB_MENU)== MENU_SET_POINTS) || ((imenu | BIT_SUB_MENU)== MENU_ANALOG_OUT))
//  {
//    imenu = imenu ^ BIT_CH_ALLOW;
//  }
//
//
//
// printf("imenu = %4x\n", imenu);
 return imenu;
  
}

///////////////////////////////////////////////////////////////////////////////
// выбор точки обмена MODBUS из списка
// проход по всму списку точек pointList
// -1 - ниодна точка в этом проходе не опрашивается
// i - индекс опрашиваемой точки
//////////////////////////////////////////////////////////////////////
int8_t GetNumPoint(ModbusHVACx* pointList, int8_t i_old) // выбор точки обмена MODBUS из списка
{

 int8_t i = i_old;

 // проход от точки до конца списка
 for(;i < (pointList->num); i++)
 {

	 if(pointList->modbusPoint[i].period == 1)
	 {
		 // выдать этот номер
		 return i;
	 }
	 else if(pointList->modbusPoint[i].period == 0)
	 {
		// пропустить данный датчик
		continue;
	 }
	 else
	 {
		if(pointList->modbusPoint[i].count >= pointList->modbusPoint[i].period - 1)
		{
			pointList->modbusPoint[i].count=0;
			return i;
		}
		else
		{
			pointList->modbusPoint[i].count++;
		}
	 }
 }

 // проход от начала  до первоначальной точки
 for(i=0;i < i_old; i++)
 {

	 if(pointList->modbusPoint[i].period == 1)
	 {
		 // выдать этот номер
		 return i;
	 }
	 else if(pointList->modbusPoint[i].period == 0)
	 {
		// пропустить данный датчик
		continue;
	 }
	 else
	 {
		if(pointList->modbusPoint[i].count >= pointList->modbusPoint[i].period - 1)
		{
			pointList->modbusPoint[i].count=0;
			return i;
		}
		else
		{
			pointList->modbusPoint[i].count++;
		}
	 }
 }

 return -1;
}

