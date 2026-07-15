
/**
  ******************************************************************************
  * @file            analog_bloks.c
  * @brief           Библтотека аналоговых блоков
  * @author          Эйсвальд И.А.
  * @date            2025-06-08
  * @details         Описание аналоговых блоков<br>
  * void  Add(sVent_Analog_2* ADD) - Сложение двух слов int16_t <br>
  * void  Bits_AND(sVent_Analog_2* BITS_AND)- побитное И двух слов int16_t<br>
  * void  Bits_OR(sVent_Analog_2* BITS_OR)- побитное ИЛИ двух слов int16_t<br>
  * void  Cmp(sComparator_2* CMP) - сравнение двух слов int16_t<br>
  ******************************************************************************
  */
#include "analog_bloks.h"

/**
 * @brief Пердача содержимого вхоного регистра  int16_t в выходной регистр
 *
 * @param BUFF - указатель на struct   sHolder
 * @return void
 */
//====================================================
// BUFF
// IN1 -  адрес MODBUS_REG
// Out - адрес MODBUS_REG
//====================================================
void  Buff(sHolder* BUFF)
{
  MODBUS_REG(BUFF->Out) = MODBUS_REG(BUFF->IN);
}
//====================================================


int16_t Inc(int16_t Y, int16_t Max)
{
 if(Y < Max) Y++;
 return Y;
}

int16_t Dec(int16_t Y, int16_t Min)
{
 if(Y > Min) Y--;

 return Y;
}


void  Delay_on(sDelay* Delay)
{
 if(READ_COIL(Delay->bIN) == 1)
 {
	if(Delay->count < Delay->delay)
	{
		Delay->count++;
	}
	else
	{
		SET_COIL(Delay->bOUT);
		Delay->count = Delay->delay;
	}
 }
 else
 {
	Delay->count = 0;
	CLR_COIL(Delay->bOUT);
 }
}

void  Delay_off(sDelay* Delay)
{
	if(READ_COIL(Delay->bIN) == 0)
 {
	if(Delay->count < Delay->delay)
	{
		Delay->count++;
	}
	else
	{
		CLR_COIL(Delay->bOUT);
		Delay->count = Delay->delay;
	}
 }
 else
 {
	Delay->count = 0;
	SET_COIL(Delay->bOUT);
 }
}

//====================================================
// Threshold
// IN -   адрес MODBUS_REG
// bOut - адрес COIL
// Out_On - порог включения
// Out_Off - порог выключения
//====================================================
void  Threshold(sThreshold* THR)
{
	int16_t Temp = MODBUS_REG(THR->IN);

    if(Temp > THR->Out_On)
    {
    	SET_COIL(THR->bOut);
    }
    else if(Temp < THR->Out_Off) CLR_COIL(THR->bOut);
}

//====================================================
// Limiter
// IN -  адрес MODBUS_REG
// Out - адрес MODBUS_REG
// Out_Lo
// Out_Hi
//====================================================
void  Limiter(sLimiter* LIM)
{
	int16_t Temp = MODBUS_REG(LIM->IN);

    if(Temp > LIM->Out_Hi)
    {
    	MODBUS_REG(LIM->Out) = LIM->Out_Hi;
    }
    else if(Temp < LIM->Out_Lo)
    {
    	MODBUS_REG(LIM->Out) = LIM->Out_Lo;
    }
    else MODBUS_REG(LIM->Out) = Temp;

}

//====================================================
// Holder
// IN -  адрес MODBUS_REG
// Out - адрес MODBUS_REG
// bOn - адрес COIL
//====================================================
void  Holder(sHolder* HOLD)
{
    if(READ_COIL(HOLD->bOn))
    {
    	MODBUS_REG(HOLD->Out) = MODBUS_REG(HOLD->IN);
    }
}

//====================================================
// Switch
// IN1 -  адрес MODBUS_REG
// IN2 -  адрес MODBUS_REG
// Out - адрес MODBUS_REG
// bOn - адрес COIL
//====================================================
void  Switcher(sSwitcher* SWT)
{
	if(READ_COIL(SWT->bOn) & 0x01)
	{
		MODBUS_REG(SWT->Out) = MODBUS_REG(SWT->IN1);
	}
	else
	{
		MODBUS_REG(SWT->Out) = MODBUS_REG(SWT->IN2);
	}
}

/**
 * @brief Сложение двух слов int16_t
 *
 * @param ADD - указатель на struct   sVent_Analog_2
 * @return void
 */
//====================================================
// Add
// IN1 -  адрес MODBUS_REG
// IN2 -  адрес MODBUS_REG
// Out - адрес MODBUS_REG
//====================================================
void  Add(sVent_Analog_2* ADD)
{
  MODBUS_REG(ADD->Out) = MODBUS_REG(ADD->IN1) + MODBUS_REG(ADD->IN2);
}

void  Sub(sVent_Analog_2* SUB)
{
  MODBUS_REG(SUB->Out) = MODBUS_REG(SUB->IN1) - MODBUS_REG(SUB->IN2);
}

//====================================================
// Multiply
// IN1 -  адрес MODBUS_REG
// IN2 -  адрес MODBUS_REG
// Out - адрес MODBUS_REG
//====================================================
void  Mult(sVent_Analog_2* MULT)
{
  MODBUS_REG(MULT->Out) = MODBUS_REG(MULT->IN1) * MODBUS_REG(MULT->IN2);
}


//====================================================
// Equal // = 1 если два числа равны
// IN1 -  адрес MODBUS_REG
// IN2 -  адрес MODBUS_REG
// bOut - адрес MODBUS_Coil
//====================================================
void  Equal(sVent_Analog_2 *Eq)
{
 	if(MODBUS_REG(Eq->IN1) == MODBUS_REG(Eq->IN2)) { SET_COIL(Eq->bOut); }
 	else CLR_COIL(Eq->bOut);
}
//====================================================




//====================================================
// Shift_Left
// IN1 -  адрес MODBUS_REG сдвигаемого регистра
// IN2 -  адрес MODBUS_REG на сколько бит сдвигаем
// Out - адрес MODBUS_REG
//====================================================
void  Shift_Left(sVent_Analog_2* SHIFT_LEFT)
{
  MODBUS_REG(SHIFT_LEFT->Out) = MODBUS_REG(SHIFT_LEFT->IN1) << MODBUS_REG(SHIFT_LEFT->IN2);
}

//====================================================
// Shift_Right
// IN1 -  адрес MODBUS_REG сдвигаемого регистра
// IN2 -  адрес MODBUS_REG на сколько бит сдвигаем
// Out - адрес MODBUS_REG
//====================================================
void  Shift_Right(sVent_Analog_2* SHIFT_RIGHT)
{
  MODBUS_REG(SHIFT_RIGHT->Out) = MODBUS_REG(SHIFT_RIGHT->IN1) >> MODBUS_REG(SHIFT_RIGHT->IN2);
}

/**
 * @brief Побитное И двух слов int16_t
 *
 * @param BITS_AND - указатель на struct   sVent_Analog_2
 * @return void
 */
//====================================================
// Bits_AND
// IN1 -  адрес MODBUS_REG сдвигаемого регистра
// IN2 -  адрес MODBUS_REG на сколько бит сдвигаем
// Out - адрес MODBUS_REG
//====================================================
void  Bits_AND(sVent_Analog_2* BITS_AND)
{
  MODBUS_REG(BITS_AND->Out) = MODBUS_REG(BITS_AND->IN1) & MODBUS_REG(BITS_AND->IN2);
}

//----------------------------------------------------
/**
 * @brief Побитное ИЛИ двух слов int16_t
 *
 * @param BITS_OR - указатель на struct   sVent_Analog_2
 * @return void
 */
//====================================================
// Bits_OR
// IN1 -  адрес MODBUS_REG сдвигаемого регистра
// IN2 -  адрес MODBUS_REG на сколько бит сдвигаем
// Out - адрес MODBUS_REG
//====================================================
void  Bits_OR(sVent_Analog_2* BITS_OR)
{
  MODBUS_REG(BITS_OR->Out) = MODBUS_REG(BITS_OR->IN1) | MODBUS_REG(BITS_OR->IN2);
}

//====================================================
// Counter
//int16_t bClock;     - тактовый сигнал
//int16_t bClock_old; - предыдущее значение тактового сигнала
//int16_t bUp;     - счет вверх
//int16_t bReset;
//int16_t Out;
//====================================================
void Counter(sCounter* COUNT)
{
 uint16_t bClock = READ_COIL(COUNT->bClock);
 uint16_t bUp = READ_COIL(COUNT->bUp);

 if(!READ_COIL(COUNT->bReset))
 {
	 if((bClock == 1) && (COUNT->bClock_old == 0))
	 {
		 // пришел передний фронт
		 if(bUp)
		 {
			 // увеличение
			 MODBUS_REG(COUNT->Out)++;
		 }
		 else
		 {
			 MODBUS_REG(COUNT->Out)--;
		 }
	 }
 }
 else
 {
	 MODBUS_REG(COUNT->Out) = 0;
 }

 COUNT->bClock_old = bClock;
}

//====================================================
// Scheduler
//
//
// IN1 -  адрес MODBUS_REG сдвигаемого регистра
// IN2 -  адрес MODBUS_REG на сколько бит сдвигаем
// Out - адрес Coil
//====================================================

void Scheduler(sScheduler* SCHEDULER )
{
	int16_t Start = MODBUS_REG(SCHEDULER->IN_Start);
	int16_t Stop = MODBUS_REG(SCHEDULER->IN_Stop);
	int16_t Time = MODBUS_REG(SCHEDULER->IN_Time);

	if(Stop < Start) Stop += 24*60;  // переход в следующие сутки

	if((Time >= Start) && (Time < Stop))
	{
		SET_COIL(SCHEDULER->Out);
	}
	else CLR_COIL(SCHEDULER->Out);

}

//====================================================
// Min
// определение минимального значения из двух
//
// IN1 -  адрес MODBUS_REG сравниваемое А
// IN2 -  адрес MODBUS_REG сравниваемое В
// Out - адрес MODBUS_REG минимальное значение
//====================================================
void  Min(sVent_Analog_2* MIN)
{
	int16_t A = MODBUS_REG(MIN->IN1);
	int16_t B = MODBUS_REG(MIN->IN2);

	if(A <= B) MODBUS_REG(MIN->Out) = A;
	else MODBUS_REG(MIN->Out) = B;
}

//====================================================
// Max
// определение max значения из четырех
//
// IN1 -  адрес MODBUS_REG сравниваемое А
// IN2 -  адрес MODBUS_REG сравниваемое В
// IN3 -  адрес MODBUS_REG сравниваемое C
// IN4 -  адрес MODBUS_REG сравниваемое D
// Out - адрес MODBUS_REG максимальное значение
//====================================================
void  Max_4(sMUX_4* MAX)
{
	int16_t A = MODBUS_REG(MAX->IN1);
	int16_t B = MODBUS_REG(MAX->IN2);
	int16_t C = MODBUS_REG(MAX->IN3);
	int16_t D = MODBUS_REG(MAX->IN4);

	int16_t E = A;

	if(B > E) E = B;
	if(C > E) E = C;
	if(D > E) E = D;

	MODBUS_REG(MAX->Out) = E;
}

//====================================================

/**
 * @brief Сравнение двух слов int16_t
 *
 * @param CMP - указатель на struct   sComparator_2
 * @return void
 */
//====================================================
// Compare
// сравнение
//
// IN1 -  адрес MODBUS_REG сравниваемое А
// IN2 -  адрес MODBUS_REG сравниваемое В
// bOut - адрес Coil
//====================================================
void  Cmp(sComparator_2* CMP)
{
	int16_t A = MODBUS_REG(CMP->IN1);
	int16_t B = MODBUS_REG(CMP->IN2);

	if(A >= B)
	{
	 SET_COIL(CMP->bOut);
	}
	else CLR_COIL(CMP->bOut);

}

//====================================================

/**
 * @brief Сравнение двух слов int16_t
 *
 * @param CMP - указатель на struct   sComparator_2
 * @return void
 */
//====================================================
// Compare
// сравнение
//
// IN1 -  адрес MODBUS_REG сравниваемое А
// IN2 -  адрес MODBUS_REG сравниваемое В
// delta - значение порог нечуствительности
// bOut_GT - адрес Coil A > (B + delta) выход
// bOut_LT - адрес Coil A < (B - delta) выход
// bOut_LT - адрес Coil A == (B +- delta) выход
//====================================================
void  Cmp_3(sComparator_3* CMP)
{
	int16_t A = MODBUS_REG(CMP->IN1);
	int16_t B = MODBUS_REG(CMP->IN2);
    int16_t delta = CMP->delta;

	if(A > B + delta)
	{
	 SET_COIL(CMP->bOut_GT);
	 CLR_COIL(CMP->bOut_LT);
	 CLR_COIL(CMP->bOut_EQ);
	}
	else if(A < B - delta)
	{
		 SET_COIL(CMP->bOut_LT);
		 CLR_COIL(CMP->bOut_GT);
		 CLR_COIL(CMP->bOut_EQ);
	}
	else
	{
		 SET_COIL(CMP->bOut_EQ);
		 CLR_COIL(CMP->bOut_GT);
		 CLR_COIL(CMP->bOut_LT);
	}

}

//====================================================

//====================================================
// Delta
// сравнение
//
// IN1 -  адрес MODBUS_REG сравниваемое А
// IN2 -  адрес MODBUS_REG сравниваемое В
// delta - предельная разница между входами А и В
// bOut - адрес Coil
//
// если разница на входах больше дольты то выход = 1
//====================================================
void Delta(sDelta_2 * DELTA)
{
  uint16_t A;
  uint16_t B;

  if(MODBUS_REG(DELTA->IN1) > MODBUS_REG(DELTA->IN2))
  {
	  A = MODBUS_REG(DELTA->IN1);
	  B = MODBUS_REG(DELTA->IN2);
  }
  else
  {
	  A = MODBUS_REG(DELTA->IN2);
	  B = MODBUS_REG(DELTA->IN1);
  }

  	  if((A - B) > DELTA->delta)
  	  {
  		  SET_COIL(DELTA->bOut);
  	  }
  	  else
  	  {
  		  CLR_COIL(DELTA->bOut);
	  }
}

//====================================================
//
/// пропорциональное регулирование с задержкой
//int16_t SetPoint;  // REG  AI
//int16_t PI;        // REG  AI
//int16_t TI;        // REG  AI
//int16_t Threshold; // REG AI зона нечуствительности
//int16_t Time;       // REG AI
//
//int16_t Y_OC;       // REG
//int16_t Reverce;  // Coil
//int16_t Enable;   // Coil
//int16_t Reset;    // Coil сброс регулятора в минимум
//
//int16_t Max;        // int
//int16_t Min;        // int
//
//int16_t Y;          // REG сигнал управления
//
// Reverce = 0 if(Y_OC > SetPoint) Y++;
// else Y--;
//====================================================
void  Regulator_1(sRegulator_1* R)
{
 static	int16_t countTime;     // счетчик проходов регулирования

 int16_t Delta;

 // регулятор в ресете ?
 if(!(READ_COIL(R->Reset)))
 {
	 MODBUS_REG(R->Y) = R->Max;
//	 MODBUS_REG(0x209) = 0xaaaa;  // ОТЛАДКА
	 goto Exit;
 }

 countTime++;

 // регулятор включен ?
 if(READ_COIL(R->Enable))
 {
//	  MODBUS_REG(0x20c) = countTime;

	 if(countTime >= MODBUS_REG(R->Time))
	 {
	  // интервал между регулированиями заончился
	  // проводим регулировку
	  countTime = 0;

	 // процесс регулирования

	 // считаем рассогласование сигналов по одулю
	 if(MODBUS_REG(R->SetPoint) >= MODBUS_REG(R->Y_OC))
	 {
		 Delta = MODBUS_REG(R->SetPoint) - MODBUS_REG(R->Y_OC);
	 }
	 else
	 {
		 Delta = MODBUS_REG(R->Y_OC) - MODBUS_REG(R->SetPoint);
	 }

     // Сравниваем рассогласование сигналов с порогом рассогласования
	   if(Delta > MODBUS_REG(R->Threshold))
	   {
		 // разница сигналов > SetDelta_Y делаем шаг регулировки
		   if(MODBUS_REG(R->Y_OC) > MODBUS_REG(R->SetPoint))
		   {
			   // сигнал больше уставки
			   if(!(READ_COIL(R->Reverce)))
			   {
				  MODBUS_REG(R->Y) = Inc((MODBUS_REG(R->Y)), (R->Max));
			   }
			   else
			   {
				  MODBUS_REG(R->Y) = Dec((MODBUS_REG(R->Y)), (R->Min)); // READ_COIL(R->Reverce) = 1; сигнал MODBUS_REG(R->Y_OC) повышается -> управление делаем меньше клапан калорифера зима
			   }
		   }
		   else
		   {
			   // сигнал меньше уставки
			   if(!(READ_COIL(R->Reverce)))
			   {
				   MODBUS_REG(R->Y) = Dec((MODBUS_REG(R->Y)), (R->Min));
			   }
			   else
			   {
				   MODBUS_REG(R->Y) = Inc((MODBUS_REG(R->Y)), (R->Max));
			   }
		   }

	   }

	 }

 } // end R->Enable
 else
 {
	 MODBUS_REG(R->Y) = R->Max;

 }

 Exit:;


}

//====================================================

//====================================================
void  Regulator_2(sRegulator_1* R)
{
 static	int16_t countTime;     // счетчик проходов регулирования

 int16_t Delta;

 // регулятор в ресете ?
 if(!(READ_COIL(R->Reset)))
 {
	 MODBUS_REG(R->Y) = R->Max;
	 goto Exit;
 }

 countTime++;
 // регулятор включен ?
 if(READ_COIL(R->Enable))
 {

	 if(countTime >= MODBUS_REG(R->Time))
	 {
	  // интервал между регулированиями заончился
	  // проводим регулировку
//	  MODBUS_REG(0x209) = countTime;
	  countTime = 0;

	 // процесс регулирования

	 // считаем рассогласование сигналов по одулю
	 if(MODBUS_REG(R->SetPoint) >= MODBUS_REG(R->Y_OC))
	 {
		 Delta = MODBUS_REG(R->SetPoint) - MODBUS_REG(R->Y_OC);
	 }
	 else
	 {
		 Delta = MODBUS_REG(R->Y_OC) - MODBUS_REG(R->SetPoint);
	 }

     // Сравниваем рассогласование сигналов с порогом рассогласования
	   if(Delta > MODBUS_REG(R->Threshold))
	   {
		 // разница сигналов > Threshold делаем шаг регулировки
		   if(MODBUS_REG(R->Y_OC) > MODBUS_REG(R->SetPoint))
		   {
			   // сигнал больше уставки
			   if(!(READ_COIL(R->Reverce)))
			   {
				  // прямое регулирование: Добавляем сигнал управления
				  MODBUS_REG(R->Y) = Inc((MODBUS_REG(R->Y)), (R->Max));  // MODBUS_REG(R->Y) глобальная переменная reg[0x160] начение в нем сохраняются
			   }
			   else
			   {
				  // обратное регулирование: Уменьшаем сигнал управления
				  MODBUS_REG(R->Y) = Dec((MODBUS_REG(R->Y)), (R->Min));
			   }
		   }
		   else
		   {
			   // сигнал меньше уставки
			   if(!(READ_COIL(R->Reverce)))
			   {
				   MODBUS_REG(R->Y) = Dec((MODBUS_REG(R->Y)), (R->Min));
			   }
			   else
			   {
				   MODBUS_REG(R->Y) = Inc((MODBUS_REG(R->Y)), (R->Max));
			   }
		   }

	   }

	 }

 } // end R->Enable
 else
 {
	 MODBUS_REG(R->Y) = R->Max;
 }

 Exit:;

}

//====================================================
void  Regulator_3(sRegulator_1* R)
{
 static	int16_t countTime;     // счетчик проходов регулирования

 int16_t Delta;

 // регулятор в ресете ?
 if(!(READ_COIL(R->Reset)))
 {
	 MODBUS_REG(R->Y) = R->Min;
	 goto Exit;
 }

 countTime++;
 // регулятор включен ?
 if(READ_COIL(R->Enable))
 {

	 if(countTime >= MODBUS_REG(R->Time))
	 {
	  // интервал между регулированиями заончился
	  // проводим регулировку
//	  MODBUS_REG(0x209) = countTime;
	  countTime = 0;

	 // процесс регулирования

	 // считаем рассогласование сигналов по одулю
	 if(MODBUS_REG(R->SetPoint) >= MODBUS_REG(R->Y_OC))
	 {
		 Delta = MODBUS_REG(R->SetPoint) - MODBUS_REG(R->Y_OC);
	 }
	 else
	 {
		 Delta = MODBUS_REG(R->Y_OC) - MODBUS_REG(R->SetPoint);
	 }

     // Сравниваем рассогласование сигналов с порогом рассогласования
	   if(Delta > MODBUS_REG(R->Threshold))
	   {
		 // разница сигналов > Threshold делаем шаг регулировки
		   if(MODBUS_REG(R->Y_OC) > MODBUS_REG(R->SetPoint))
		   {
			   // сигнал больше уставки
			   if(!(READ_COIL(R->Reverce)))
			   {
				  // прямое регулирование: Добавляем сигнал управления
				  MODBUS_REG(R->Y) = Inc((MODBUS_REG(R->Y)), (R->Max));  // MODBUS_REG(R->Y) глобальная переменная reg[0x160] начение в нем сохраняются
			   }
			   else
			   {
				  // обратное регулирование: Уменьшаем сигнал управления
				  MODBUS_REG(R->Y) = Dec((MODBUS_REG(R->Y)), (R->Min));
			   }
		   }
		   else
		   {
			   // сигнал меньше уставки
			   if(!(READ_COIL(R->Reverce)))
			   {
				   MODBUS_REG(R->Y) = Dec((MODBUS_REG(R->Y)), (R->Min));
			   }
			   else
			   {
				   MODBUS_REG(R->Y) = Inc((MODBUS_REG(R->Y)), (R->Max));
			   }
		   }

	   }

	 }

 } // end R->Enable
 else
 {
	 MODBUS_REG(R->Y) = R->Max;
 }

 Exit:;

}

//====================================================

//====================================================
void  Regulator_4(sRegulator_1* R)
{
 static	int16_t countTime;     // счетчик проходов регулирования

 int16_t Delta;

 // регулятор в ресете ?
 if(!(READ_COIL(R->Reset)))
 {
	 MODBUS_REG(R->Y) = R->Min;
	 goto Exit;
 }

 countTime++;
 // регулятор включен ?
 if(READ_COIL(R->Enable))
 {

	 if(countTime >= MODBUS_REG(R->Time))
	 {
	  // интервал между регулированиями заончился
	  // проводим регулировку
//	  MODBUS_REG(0x209) = countTime;
	  countTime = 0;

	 // процесс регулирования

	 // считаем рассогласование сигналов по одулю
	 if(MODBUS_REG(R->SetPoint) >= MODBUS_REG(R->Y_OC))
	 {
		 Delta = MODBUS_REG(R->SetPoint) - MODBUS_REG(R->Y_OC);
	 }
	 else
	 {
		 Delta = MODBUS_REG(R->Y_OC) - MODBUS_REG(R->SetPoint);
	 }

     // Сравниваем рассогласование сигналов с порогом рассогласования
	   if(Delta > MODBUS_REG(R->Threshold))
	   {
		 // разница сигналов > Threshold делаем шаг регулировки
		   if(MODBUS_REG(R->Y_OC) > MODBUS_REG(R->SetPoint))
		   {
			   // сигнал больше уставки
			   if(!(READ_COIL(R->Reverce)))
			   {
				  // прямое регулирование: Добавляем сигнал управления
				  MODBUS_REG(R->Y) = Inc((MODBUS_REG(R->Y)), (R->Max));  // MODBUS_REG(R->Y) глобальная переменная reg[0x160] начение в нем сохраняются
			   }
			   else
			   {
				  // обратное регулирование: Уменьшаем сигнал управления
				  MODBUS_REG(R->Y) = Dec((MODBUS_REG(R->Y)), (R->Min));
			   }
		   }
		   else
		   {
			   // сигнал меньше уставки
			   if(!(READ_COIL(R->Reverce)))
			   {
				   MODBUS_REG(R->Y) = Dec((MODBUS_REG(R->Y)), (R->Min));
			   }
			   else
			   {
				   MODBUS_REG(R->Y) = Inc((MODBUS_REG(R->Y)), (R->Max));
			   }
		   }

	   }

	 }

 } // end R->Enable
 else
 {
	 MODBUS_REG(R->Y) = R->Max;
 }

 Exit:;

}

//====================================================
//====================================================
void  Regulator_5(sRegulator_5* R)
{
 static	int16_t countTime;     // счетчик проходов регулирования

 int16_t Delta;

 // регулятор в ресете ?
 if(!(READ_COIL(R->Reset)))
 {
	 MODBUS_REG(R->Y) = MODBUS_REG(R->Min);
	 goto Exit;
 }

 countTime++;
 // регулятор включен ?
 if(READ_COIL(R->Enable))
 {

	 if(countTime >= MODBUS_REG(R->Time))
	 {
	  // интервал между регулированиями заончился
	  // проводим регулировку
//	  MODBUS_REG(0x209) = countTime;
	  countTime = 0;

	 // процесс регулирования

	 // считаем рассогласование сигналов по одулю
	 if(MODBUS_REG(R->SetPoint) >= MODBUS_REG(R->Y_OC))
	 {
		 Delta = MODBUS_REG(R->SetPoint) - MODBUS_REG(R->Y_OC);
	 }
	 else
	 {
		 Delta = MODBUS_REG(R->Y_OC) - MODBUS_REG(R->SetPoint);
	 }

     // Сравниваем рассогласование сигналов с порогом рассогласования
	   if(Delta > MODBUS_REG(R->Threshold))
	   {
		 // разница сигналов > Threshold делаем шаг регулировки
		   if(MODBUS_REG(R->Y_OC) > MODBUS_REG(R->SetPoint))
		   {
			   // сигнал больше уставки
			   if(!(READ_COIL(R->Reverce)))
			   {
				  // прямое регулирование: Добавляем сигнал управления
				  MODBUS_REG(R->Y) = Inc((MODBUS_REG(R->Y)), (R->Max));  // MODBUS_REG(R->Y) глобальная переменная reg[0x160] начение в нем сохраняются
			   }
			   else
			   {
				  // обратное регулирование: Уменьшаем сигнал управления
				  MODBUS_REG(R->Y) = Dec((MODBUS_REG(R->Y)), MODBUS_REG(R->Min));
			   }
		   }
		   else
		   {
			   // сигнал меньше уставки
			   if(!(READ_COIL(R->Reverce)))
			   {
				   MODBUS_REG(R->Y) = Dec((MODBUS_REG(R->Y)), MODBUS_REG(R->Min));
			   }
			   else
			   {
				   MODBUS_REG(R->Y) = Inc((MODBUS_REG(R->Y)), (R->Max));
			   }
		   }

	   }

	 }

 } // end R->Enable
 else
 {
	 MODBUS_REG(R->Y) = MODBUS_REG(R->Min);
 }

 Exit:;

}

//====================================================


//====================================================
// ПИ - регулятор
///
//int16_t SetPoint;  // REG  AI
//int16_t PI;        // REG  AI
//int16_t TI;        // REG  AI
//int16_t SetDelta_Y; // REG AI зона нечуствительности
//int16_t Time;       // REG AI
//
//int16_t Y_OC;       // REG
//int16_t Reverce;  // Coil
//int16_t Enable;   // Coil
//int16_t Reset;    // Coil сброс регулятора в минимум
//
//int16_t Max;        // int
//int16_t Min;        // int
//
//int16_t Y;          // REG сигнал управления
//
// Reverce = 0 if(Y_OC > SetPoint) Y++;
// else Y--;
//====================================================
void  Regulator_PI(sRegulator_1* R)
{
 static	int16_t countTime;     // счетчик проходов регулирования

 int16_t Delta;

 // регулятор в ресете ?
 if(!(READ_COIL(R->Reset)))
 {
	 MODBUS_REG(R->Y) = R->Min;
//	 MODBUS_REG(0x209) = 0xaaaa;  // ОТЛАДКА
	 goto Exit;
 }

 countTime++;

 // регулятор включен ?
 if(READ_COIL(R->Enable))
 {
//	  MODBUS_REG(0x20c) = countTime;

	 if(countTime >= MODBUS_REG(R->Time))
	 {
	  // интервал между регулированиями заончился
	  // проводим регулировку
	  countTime = 0;

	 // процесс регулирования

	 // считаем рассогласование сигналов по модулю
	 if(MODBUS_REG(R->SetPoint) >= MODBUS_REG(R->Y_OC))
	 {
		 Delta = MODBUS_REG(R->SetPoint) - MODBUS_REG(R->Y_OC);
	 }
	 else
	 {
		 Delta = MODBUS_REG(R->Y_OC) - MODBUS_REG(R->SetPoint);
	 }

     // Сравниваем рассогласование сигналов с порогом рассогласования
	   if(Delta > MODBUS_REG(R->Threshold))
	   {
		 // разница сигналов > Threshold делаем шаг регулировки

			   // сигнал больше уставки
			   if(!(READ_COIL(R->Reverce)))
			   {
				  // прямое регулирование: Добавляем сигнал управления
				  MODBUS_REG(R->I) += MODBUS_REG(R->SetPoint) - MODBUS_REG(R->Y_OC);
				  MODBUS_REG(R->deltaY) = (MODBUS_REG(R->SetPoint) - MODBUS_REG(R->Y_OC)) * MODBUS_REG(R->PI) + MODBUS_REG(R->I) * MODBUS_REG(R->TI);  // MODBUS_REG(R->Y) глобальная переменная reg[0x160] начение в нем сохраняются
			   }
			   else
			   {
				  // обратное регулирование: Уменьшаем сигнал управления
				  MODBUS_REG(R->I) += MODBUS_REG(R->Y_OC) - MODBUS_REG(R->SetPoint);
				  MODBUS_REG(R->deltaY) = (MODBUS_REG(R->Y_OC) - MODBUS_REG(R->SetPoint)) * MODBUS_REG(R->PI) + MODBUS_REG(R->I) * MODBUS_REG(R->TI);
			   }

	   }
	   else
	   {
		 // рассоглсование меньше порога Thresold
		 // значит изменение управляющего сигнала нет
		   MODBUS_REG(R->deltaY) = 0;
		   MODBUS_REG(R->I) = 0;


	   }

	   if((MODBUS_REG(R->Y) + MODBUS_REG(R->deltaY)) >  MODBUS_REG(R->Max))  MODBUS_REG(R->Y) = MODBUS_REG(R->Max);
	   else if((MODBUS_REG(R->Y) + MODBUS_REG(R->deltaY)) <  MODBUS_REG(R->Min))  MODBUS_REG(R->Y) = MODBUS_REG(R->Min);
	   else MODBUS_REG(R->Y) += MODBUS_REG(R->deltaY);

	 }  // end время регулирования
	 else
	 {
		 MODBUS_REG(R->deltaY) = 0;
	 }

 } // end R->Enable иначе регулятор замирает

 Exit:;


}

//====================================================

//====================================================
// шаговый  регулятор
///
//int16_t SetPoint;  // REG  AI
//int16_t PI;        // REG  AI
//int16_t TI;        // REG  AI
//int16_t SetDelta_Y; // REG AI зона нечуствительности
//int16_t Time;       // REG AI
//
//int16_t Y_OC;       // REG
//int16_t Reverce;  // Coil
//int16_t Enable;   // Coil
//int16_t Reset;    // Coil сброс регулятора в минимум
//
//int16_t Max;        // int
//int16_t Min;        // int
//
//int16_t dY;          // REG выходной сигнал на счетчик +1 / 0 / -1
//
//
//
//====================================================
void  Regulator_Step(sRegulator_step* R)
{
// static	int16_t countTime;     // счетчик проходов регулирования

 int16_t Delta;



 // регулятор включен ?
 if(READ_COIL(R->Enable))
 {
	 // процесс регулирования
	 // считаем рассогласование сигналов по модулю
	 if(MODBUS_REG(R->SetPoint) >= MODBUS_REG(R->Y_OC))
	 {
		 Delta = MODBUS_REG(R->SetPoint) - MODBUS_REG(R->Y_OC);
	     // Сравниваем рассогласование сигналов с порогом рассогласования
		   if(Delta > MODBUS_REG(R->Threshold))
		   {
			 // разница сигналов > Threshold делаем шаг регулировки

				   // сигнал больше уставки
				   if(!(READ_COIL(R->Reverce)))
				   {
					  // прямое регулирование: Добавляем сигнал управления
					  MODBUS_REG(R->dY) = 1;
				   }
				   else
				   {
					  // обратное регулирование: Уменьшаем сигнал управления
					  MODBUS_REG(R->dY) = -1;
				   }

		   }
		   else
		   {
			 // рассоглсование меньше порога Thresold
			 // значит изменение управляющего сигнала нет
			   MODBUS_REG(R->dY) = 0;
		   }

	 }
	 else
	 {
		 Delta = MODBUS_REG(R->Y_OC) - MODBUS_REG(R->SetPoint);
	     // Сравниваем рассогласование сигналов с порогом рассогласования
		   if(Delta > MODBUS_REG(R->Threshold))
		   {
			 // разница сигналов > Threshold делаем шаг регулировки

				   // сигнал больше уставки
				   if(!(READ_COIL(R->Reverce)))
				   {
					  // прямое регулирование: Добавляем сигнал управления
					  MODBUS_REG(R->dY) = -1;
				   }
				   else
				   {
					  // обратное регулирование: Уменьшаем сигнал управления
					  MODBUS_REG(R->dY) = 1;
				   }

		   }
		   else
		   {
			 // рассоглсование меньше порога Thresold
			 // значит изменение управляющего сигнала нет
			   MODBUS_REG(R->dY) = 0;
		   }

	 }


 } // end R->Enable  dY = 0 иначе регулятор замирает
 else
 {
	  MODBUS_REG(R->dY) = 0;
 }


}

//====================================================
// //счетчик с двумя входами
//
//int16_t IN1;        // REG  AI
//int16_t IN2;        // REG  AI
//int16_t Time;       // REG AI  интрвал времени через которое происходит изменение счетчика
//
//int16_t Reset;      // Coil сброс счетчика в минимум
//
//int16_t Max;        // int
//int16_t Min;        // int
//
//int16_t Y;          // REG  накопленый сигнал управления
//
//
//====================================================
void Counter_2IN(sCounter_2IN* COUNTER_2IN)
{
	 static	int16_t countTime;     // счетчик проходов регулирования


	 // регулятор в ресете ?
	 if(!(READ_COIL(COUNTER_2IN->Reset)))
	 {
		 MODBUS_REG(COUNTER_2IN->Y) = COUNTER_2IN->Min;
	//	 MODBUS_REG(0x209) = 0xaaaa;  // ОТЛАДКА
		 goto Exit;
	 }

	 countTime++;

		 if(countTime >= MODBUS_REG(COUNTER_2IN->Time))
		 {
			 // проводим процесс реглировки
			 MODBUS_REG(COUNTER_2IN->Y) =  MODBUS_REG(COUNTER_2IN->Y) + MODBUS_REG(COUNTER_2IN->IN1) + MODBUS_REG(COUNTER_2IN->IN2);

			 if(MODBUS_REG(COUNTER_2IN->Y) >= COUNTER_2IN->Max) MODBUS_REG(COUNTER_2IN->Y) = COUNTER_2IN->Max;
			 else if(MODBUS_REG(COUNTER_2IN->Y) <= COUNTER_2IN->Min) MODBUS_REG(COUNTER_2IN->Y) = COUNTER_2IN->Min;

			 countTime = 0;

		 }

	Exit:;

}

//====================================================
// аналоговый ключ на 4 входа
//int16_t IN1; // номера регистров
//int16_t IN2;
//int16_t IN3;
//int16_t IN4;
//int16_t ADR; // адрес выбираемого входа
//int16_t Out;
//====================================================
void Mux_4(sMUX_4* MUX_4)
{
 switch (MODBUS_REG(MUX_4 ->ADR))
 {
 case 0:
	 MODBUS_REG(MUX_4->Out) = MODBUS_REG(MUX_4->IN1);
	 break;

 case 1:
	 MODBUS_REG(MUX_4->Out) = MODBUS_REG(MUX_4->IN2);
	 break;

 case 2:
	 MODBUS_REG(MUX_4->Out) = MODBUS_REG(MUX_4->IN3);
	 break;

 case 3:
	 MODBUS_REG(MUX_4->Out) = MODBUS_REG(MUX_4->IN4);
	 break;

 default:
	 MODBUS_REG(MUX_4->Out) = 0xffff;
 }
}

//====================================================
// ключ на 4 цифровых входа
//int16_t IN1; // адрес Coil
//int16_t IN2; // адрес Coil
//int16_t IN3; // адрес Coil
//int16_t IN4; // адрес Coil
//int16_t ADR; // адрес Reg который определяет номер  выбираемого входа
//int16_t Out;
//====================================================
void Mux_dig_4(sMUX_dig_4* MUX_4)
{
 switch (MODBUS_REG(MUX_4 ->ADR))
 {
 case 0:
	 if(READ_COIL(MUX_4->IN1)){
	   SET_COIL(MUX_4->Out);
	 }
	 else {
		 CLR_COIL(MUX_4->Out);
	 }
	 break;

 case 1:
	 if(READ_COIL(MUX_4->IN2)){
	   SET_COIL(MUX_4->Out);
	 }
	 else {
		 CLR_COIL(MUX_4->Out);
	 }
	 break;


 case 2:
	 if(READ_COIL(MUX_4->IN3)){
	   SET_COIL(MUX_4->Out);
	 }
	 else {
		 CLR_COIL(MUX_4->Out);
	 }
	 break;

 case 3:
	 if(READ_COIL(MUX_4->IN4)){
	   SET_COIL(MUX_4->Out);
	 }
	 else {
		 CLR_COIL(MUX_4->Out);
	 }
	 break;

 default:
	 CLR_COIL(MUX_4->Out);
 }
}


//====================================================

//====================================================
// декодирование регистра MODE в биты режима 0 - ручной, 1- лето, 2- вентиляция, 3- зима
//int16_t IN1;    // адоес входного регистра
//int16_t OUT1;   // адоес Coil ручной
//int16_t OUT2;   // адоес Coil лето
//int16_t OUT3;   // адоес Coil вентиляция
//int16_t OUT4;   // адоес Coil зима
//====================================================
void  DeCoder_4(sDeCoder_4* DECODER_4)
{
 //sDeCoder_4 Temp;
 switch (MODBUS_REG(DECODER_4->IN1))
 {
  case 0:
	  SET_COIL(DECODER_4->OUT1);
	  CLR_COIL(DECODER_4->OUT2);
	  CLR_COIL(DECODER_4->OUT3);
	  CLR_COIL(DECODER_4->OUT4);
  break;
  case 1:
	  CLR_COIL(DECODER_4->OUT1);
	  SET_COIL(DECODER_4->OUT2);
	  CLR_COIL(DECODER_4->OUT3);
	  CLR_COIL(DECODER_4->OUT4);
  break;
  case 2:
	  CLR_COIL(DECODER_4->OUT1);
	  CLR_COIL(DECODER_4->OUT2);
	  SET_COIL(DECODER_4->OUT3);
	  CLR_COIL(DECODER_4->OUT4);
  break;
  case 3:
	  CLR_COIL(DECODER_4->OUT1);
	  CLR_COIL(DECODER_4->OUT2);
	  CLR_COIL(DECODER_4->OUT3);
	  SET_COIL(DECODER_4->OUT4);
  break;
  default:
	  CLR_COIL(DECODER_4->OUT1);
	  CLR_COIL(DECODER_4->OUT2);
	  CLR_COIL(DECODER_4->OUT3);
	  CLR_COIL(DECODER_4->OUT4);
  break;

 }

}

//====================================================
// Скалер:
// По двум точкам строится прямая
// И по Тул выбирается тем-ра обратного теплоносителя Тоб
//int16_t Tout1;     // адоес Reg входного регистра уставка самая холодная тем-ра на улице
//int16_t Twater1;   // адоес Reg входного регистра уставка самая горячая тем-ра воды
//int16_t Tout2;     // адоес Reg входного регистра уставка самая теплая тем-ра на улице
//int16_t Twater2;   // адоес Reg входного регистра уставка самая холодная тем-ра воды
//
//int16_t Tout_now;  // адоес Reg входного регистра текущая тем-ра на улице
//
//int16_t OUT;   //  адоес Reg выодного регистра вычесленная необходимая тем-ра обратки
//====================================================
void Equation_2_piont(sEquation* SCALER)
{
 if(MODBUS_REG(SCALER->x) >= MODBUS_REG(SCALER->x2))
 {
   MODBUS_REG(SCALER->y) =  MODBUS_REG(SCALER->y2); // 40 градусов
 }
 else if(MODBUS_REG(SCALER->x) <= MODBUS_REG(SCALER->x1))
 {
	 MODBUS_REG(SCALER->y) =  MODBUS_REG(SCALER->y1); // 80 градусов
 }
 else
 {
   MODBUS_REG(SCALER->y) = ( (MODBUS_REG(SCALER->x) - MODBUS_REG(SCALER->x1)) * (MODBUS_REG(SCALER->y2) - MODBUS_REG(SCALER->y1)) ) / (MODBUS_REG(SCALER->x2) - MODBUS_REG(SCALER->x1)) + MODBUS_REG(SCALER->y1);
 }
}


void Pwm(sPWM*  PWM)
{
  PWM->counter = PWM->counter + 1;

  if((PWM->counter) > ((PWM->Period)*10))
  {
	  PWM->counter = 0;
  }

  if((MODBUS_REG(PWM->AI)*(PWM->Period))/10 > PWM->counter)
  {
	 SET_COIL(PWM->bOUT);
  }
  else
  {
	 CLR_COIL(PWM->bOUT);
  }
}
