/*
 * EC11.h
 *
 *  Created on: Jul 14, 2023
 *      Author: ef_se
 */

#ifndef INC_EC11_H_
#define INC_EC11_H_

#include "main.h"

#include "stdlib.h"
#include "string.h"

typedef struct{
	int32_t countTotal;  //
	uint32_t count;      // значение счетчика валкодера
	uint32_t prev_count; // прндыдущее значение счетчика валкодера
	uint8_t Btn;            // Btn = 1 кнопка нажата
	uint16_t count_1;   // количество чтений "1" подряд
	uint16_t count_0;   // количество чтений "0" подряд

	uint16_t countBtn;   // количество нажатий кнопок
	uint8_t timePush;       // время защиты от дребезга
	uint8_t up;
	uint8_t down;
} struct_EC11;


extern struct_EC11 ec11;
extern TIM_HandleTypeDef htim3;

void EC11_Init();
struct_EC11 EC11_Read();
int EC11_decide(struct_EC11); // решает на склько пунктов изменилсясчетчик
                              // + Up  - Down
#endif /* INC_EC11_H_ */
