/*
 * digital_bloks.h
 *
 *  Created on: Jun 19, 2024
 *      Author: dexp
 */

#ifndef INC_DIGITAL_BLOKS_H_
#define INC_DIGITAL_BLOKS_H_

#include "stm32f1xx_hal.h"
#include "utils.h"
/*
 * Структура GEN описывает генератор
 */
typedef struct {
  uint16_t Ton;    ///< время в Hight в 0.1 сек
  uint16_t Toff;   ///< время в Low в 0.1 сек
  uint16_t Out;    ///< адрес Coil выходного генератора
  uint16_t count;  ///< вспомогательный счетчик
} GEN;

/*
 * Структура RS описывает RS тригер
 */
typedef struct{
	uint16_t bSet;    ///< адрес Coil входного сигнала установки
	uint16_t bReset;  ///< адрес Coil входного сигнала сброса
	uint16_t bQ;      ///< адрес Coil выходного сигнала
} RS;

/*
 * Структура VENT_2 описывает сигналы модуля два цифровых входа и один цифрой выход
 */
typedef struct{
	uint16_t bIN1; ///< адрес Coil входного сигнала 1
	uint16_t bIN2; ///< адрес Coil входного сигнала 2
	uint16_t bQ;   ///< адрес Coil выходного сигнала
} VENT_2;

/*
 * Структура VENT_1 описывает сигналы модуля один цифровой вход и один цифрой выход
 */
typedef struct{
	uint16_t bIN; ///< адрес Coil входного сигнала
	uint16_t bQ;  ///< адрес Coil выходного сигнала
} VENT_1;

/*
 * Структура VENT_8 описывает сигналы модуля 8 цифровых входов и один цифрой выход
 */
typedef struct{
	uint16_t bIN1; ///< адрес Coil входного сигнала 1
	uint16_t bIN2; ///< адрес Coil входного сигнала 2
	uint16_t bIN3; ///< адрес Coil входного сигнала 3
	uint16_t bIN4; ///< адрес Coil входного сигнала 4
	uint16_t bIN5; ///< адрес Coil входного сигнала 5
	uint16_t bIN6; ///< адрес Coil входного сигнала 6
	uint16_t bIN7; ///< адрес Coil входного сигнала 7
	uint16_t bIN8; ///< адрес Coil входного сигнала 8
	uint16_t bQ;   ///< адрес Coil выходного сигнала
} VENT_8;

void Gen(GEN *);
void RS_Triger(RS *);
void XOR2(VENT_2 *);
void OR2(VENT_2 *);
void AND2(VENT_2 *);
void NAND2(VENT_2 *);
void NOR2(VENT_2 *);
void NOT(VENT_1 *);
void OR8(VENT_8 *);
void NAND8(VENT_8* );

#endif /* INC_DIGITAL_BLOKS_H_ */
