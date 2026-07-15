/**
  ******************************************************************************
  * @file            EC11.c
  * @brief           функции работы с валкодером ЕС11
  * @author          Эйсвальд И.А.
  * @date            2025-06-08
  * @details         Чтение счетчика эекодера и кнопки<br>
  * void EC11_Init() - инициализация эекодера подключение к TIM3<br>
  * struct_EC11 EC11_Read() - чтение данных с эекодера<br>
  *
  ******************************************************************************
  */

#include <EC11.h>

struct_EC11 ec11;

void EC11_Init() {
    // ... пропущено ...

    // так можно проставить начальное значение счетчика:
    // __HAL_TIM_SET_COUNTER(&htim3, 32760);

    // не забываем включить таймер!
    HAL_TIM_Encoder_Start(&htim3, TIM_CHANNEL_ALL);
}

/**
  * @brief  Чтение валкодера
  * @note  Эта фун-я не принимает аргументов и ничего не возвращает
  */
struct_EC11 EC11_Read() {
//	static int32_t prevCounter = 0;
//	struct_EC11 ec;
    ec11.countTotal += ec11.count - ec11.prev_count;
	ec11.prev_count = ec11.count;
    int currCounter = __HAL_TIM_GET_COUNTER(&htim3);
    currCounter = 32767 - ((currCounter-1) & 0xFFFF) / 2;

//    if(currCounter != prevCounter) {
//
//        prevCounter = currCounter;
//    }

    ec11.count = currCounter;

    // read Buttun D
    ec11.timePush = 5;
    if(!((GPIOB->IDR) & 0x04)) {
      	//ec11.count_0 =0;
    	ec11.count_1++;
    }
    else{
    	ec11.count_1 =0;
    	ec11.Btn = 0;
    }

    if(ec11.count_1 > ec11.timePush){
    	ec11.Btn = 1;
    	ec11.count_1 = 0;

    }
    else ec11.Btn = 0;

    return ec11;
}

int EC11_decide(struct_EC11 ec)
{
    if(ec.count > ec.prev_count) ec.up = 1;
    else ec.up = 0;

    if(ec.count < ec.prev_count) ec.down = 1;
    else ec.down = 0;
}
