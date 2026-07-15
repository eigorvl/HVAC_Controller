/**
  ******************************************************************************
  * @file            digital_bloks.c
  * @brief           Библтотека цифровых блоков
  * @author          Эйсвальд И.А.
  * @date            2025-06-08
  * @details         Описание цифровых блоков блоков<br>
  * void AND2(VENT_2* AND) - Логическое И двух Coils <br>
  * void Gen(GEN* gen) - генератор логического сигнала в Coil разрешение 0.1 сек<br>
  * void NAND2(VENT_2* NAND) - Логическое НЕ-И двух Coils <br>
  * void NOR2(VENT_2* NOR) - Логическое НЕ-ИЛИ двух Coils <br>
  * void OR2(VENT_2* OR) - Логическое ИЛИ  Coila <br>
  * void OR8(VENT_8* OR) - Логическое ИЛИ  Coila <br>
  * void NOT(VENT_1* NOT) - Логическое НЕ Coil <br>
  * void RS_Trigger(RS* RS) - Логический ериггер RS <br>
  * void XOR2(VENT_2* XOR) - Исключающее  ИЛИ  Coila <br>
  ******************************************************************************
  */
#include "digital_bloks.h"

/**
 * @brief Генератор логических сигналов
 *
 * @param [in,out] gen - указатель на struct   GEN
 * @return void
 */
//====================================================
// Генератор
// Ton - время в On 1 - 0.1s
// Toff - адресс COIL Set
// count -   счетчик
// bOut -   адресс COIL Out
//====================================================
void Gen(GEN* gen)
{
	uint16_t Temp = gen->Out;

    if(gen->count < gen->Ton)
    {
    	SET_COIL(Temp);
    	gen->count++;
    }
    else if(gen->count < gen->Toff)
    {
    	CLR_COIL(Temp);
    	gen->count++;
    }
    else gen->count = 0;
}


//====================================================
// RS Trigger
// bReset - адресс COIL Reset
// bSet - адресс COIL Set
// bQ -   адресс COIL Q
//====================================================
/**
 * @brief Тригер RS
 *
 * @param [in,out] RS - указатель на структуру RS
 * @return void
 */
void RS_Triger(RS* RSTrig)
{
	uint16_t Reset = READ_COIL(RSTrig->bReset);
	uint16_t Set = READ_COIL(RSTrig->bSet);

	if(Reset == 1)
	{
		CLR_COIL(RSTrig->bQ);
	}
	else if(Set == 1) SET_COIL(RSTrig->bQ);
}

//====================================================
// XOR_2
// bIN1 - адресс COIL IN1
// bIN2 - адресс COIL IN2
// bQ -   адресс COIL Q
//====================================================
/**
 * @brief XOR2
 *
 * @param [in,out] XOR - указатель на структуру VENT_2
 * @return void
 */
void XOR2(VENT_2* XOR)
{
 if(READ_COIL(XOR->bIN1) == READ_COIL(XOR->bIN2))
 {
	 CLR_COIL(XOR->bQ);
 }
 else SET_COIL(XOR->bQ);
}

//void OR_2(VENT_2 *);
//void AND_2(VENT_2 *);
//void NAND_2(VENT_2 *);

//====================================================
// OR_2
// bIN1 - адресс COIL IN1
// bIN2 - адресс COIL IN2
// bQ -   адресс COIL Q
//====================================================
/**
 * @brief OR2
 *
 * @param [in,out] OR - указатель на структуру VENT_2
 * @return void
 */
void OR2(VENT_2* OR)
{
 if(READ_COIL(OR->bIN1) || READ_COIL(OR->bIN2))
 {
	 SET_COIL(OR->bQ);
 }
 else CLR_COIL(OR->bQ);
}

//====================================================
// AND_2
// bIN1 - адресс COIL IN1
// bIN2 - адресс COIL IN2
// bQ -   адресс COIL Q
//====================================================
/**
 * @brief AND2
 *
 * @param [in,out] AND - указатель на структуру VENT_2
 * @return void
 */
void AND2(VENT_2* AND)
{
 if(READ_COIL(AND->bIN1) && READ_COIL(AND->bIN2))
 {
	 SET_COIL(AND->bQ);
 }
 else CLR_COIL(AND->bQ);
}

//====================================================
// NAND_2
// bIN1 - адресс COIL IN1
// bIN2 - адресс COIL IN2
// bQ -   адресс COIL Q
//====================================================
/**
 * @brief AND2
 *
 * @param [in,out] AND - указатель на структуру VENT_2
 * @return void
 */
void NAND2(VENT_2* NAND)
{
 if(READ_COIL(NAND->bIN1) && READ_COIL(NAND->bIN2))
 {
	 CLR_COIL(NAND->bQ);
 }
 else SET_COIL(NAND->bQ);
}

//====================================================
// NOR_2
// bIN1 - адресс COIL IN1
// bIN2 - адресс COIL IN2
// bQ -   адресс COIL Q
//====================================================
/**
 * @brief NOR2
 *
 * @param [in,out] NOR - указатель на структуру VENT_2
 * @return void
 */
void NOR2(VENT_2* NOR)
{
 if(READ_COIL(NOR->bIN1) || READ_COIL(NOR->bIN2))
 {
	 CLR_COIL(NOR->bQ);
 }
 else SET_COIL(NOR->bQ);
}

//====================================================
// NOT
// IN - адресс COIL IN
// bQ -   адресс COIL Q
//====================================================
/**
 * @brief NOT
 *
 * @param [in,out] NOT - указатель на структуру VENT_1
 * @return void
 */
void NOT(VENT_1* NOT)
{
 if(READ_COIL(NOT->bIN))
 {
	 CLR_COIL(NOT->bQ);
 }
 else SET_COIL(NOT->bQ);
}

//====================================================
// OR_8
// bIN1 - адресс COIL IN1
// ...
// bIN8 - адресс COIL IN8
// bQ -   адресс COIL Q
//====================================================
/**
 * @brief OR8
 *
 * @param [in,out] OR - указатель на структуру VENT_8
 * @return void
 */
void OR8(VENT_8* OR)
{
 if(READ_COIL(OR->bIN1) || READ_COIL(OR->bIN2) || READ_COIL(OR->bIN3) ||READ_COIL(OR->bIN4) ||
	READ_COIL(OR->bIN5) ||READ_COIL(OR->bIN6) ||READ_COIL(OR->bIN7) ||READ_COIL(OR->bIN8))
 {
	 SET_COIL(OR->bQ);
 }
 else CLR_COIL(OR->bQ);
}

//====================================================
// NAND8
// bIN1 - адресс COIL IN1
// ...
// bIN8 - адресс COIL IN8
// bQ -   адресс COIL Q
//====================================================
/**
 * @brief NAND8
 *
 * @param [in,out] nand - указатель на структуру VENT_8
 * @return void
 */
void NAND8(VENT_8* nand)
{
 if(READ_COIL(nand->bIN1) && READ_COIL(nand->bIN2) && READ_COIL(nand->bIN3) && READ_COIL(nand->bIN4) &&
	READ_COIL(nand->bIN5) && READ_COIL(nand->bIN6) && READ_COIL(nand->bIN7) && READ_COIL(nand->bIN8))
 {
	 CLR_COIL(nand->bQ);
 }
 else SET_COIL(nand->bQ);
}
