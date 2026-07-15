/**
  ******************************************************************************
  * @file            menu.c
  * @brief           Работа с меню дисплея
  * @author          Эйсвальд И.А.
  * @date            2025-06-08
  * @details         Навигация по меню;<br>
  *                  функции вывода информации на дисплей
  ******************************************************************************
 */
#include <stdio.h>
#include "main.h"
#include "menu.h"
#include "st7789_1.h"
#include "flash.h"
#include "utils.h"
#include "rtc.h"
#include "app_globals.h"

#define COLOR_T_NORM  RGB565(0,255,0)
#define COLOR_T_HIGH  RGB565(255,128,0)
#define COLOR_T_LOW   RGB565(0,128,255)
#define COLOR_CO2_NORM  RGB565(0,255,240)
#define COLOR_CO2_HIGH  RGB565(255,128,0)
//char* strTmp[16]={0};


/**
  * @brief  Фунция обработки нажатия кнопки енкодера
  * @note  Эта фун-я не принимает аргументов и ничего не возвращает<br>
  *        - изменяет глобальную переменную g_mode_Menu (навигация по меню);<br>
  *        - выполняет действия по модификации Reg_Modbus и Coils;<br>
  *        - записи уставок во FLASH;<br>
  *        - перегрузка контроллера<br>
*/
///////////////////////////////////////////////////////////////
//  Обработка нажатия кнопки
///////////////////////////////////////////////////////////////
void OnButtonEC11()
{
 if((g_mode_Menu & BITS_SUB_MENU) == 0)
 {
	 // переход в подменю
	 g_mode_Menu +=1;
	 status_Menu.line_sub_menu +=1;
	 g_mode_Menu |= BIT_ENTER_SUB_MENU;

 }
 else
 {
	 if((menu.typeMenu[(g_mode_Menu & BITS_LINE) >> 4] == 0)){
     // выход из подменю так как вход в зменение параметра запрещен
	 g_mode_Menu &= ~BIT_ENTER_SUB_MENU;
	 g_mode_Menu &= 0xfffffff0;

	 } // end просмотр пунктов

	 else if((menu.typeMenu[(g_mode_Menu & BITS_LINE) >> 4] == MENU_TYPE_SAVE)){
     // выход из подменю так как вход в зменение параметра запрещен
	 g_mode_Menu &= ~BIT_ENTER_SUB_MENU;
	 g_mode_Menu &= 0xfffffff0;
	 g_mode_Menu |= BIT_CHANGE_VALUE;
	 } // end просотр пунктов

	 else if(((password_OK == 1) || (password_OK == 2)) && (((menu.typeMenu[(g_mode_Menu & BITS_LINE) >> 4] == MENU_TYPE_ANALOG_INPUT)) ||
			 ((menu.typeMenu[(g_mode_Menu & BITS_LINE) >> 4] == MENU_TYPE_TIME))))
	 {
     // вход в изменение параметра разрешен
	  if((g_mode_Menu & BIT_MODIFY_REG) == 0){
	  // в изменение параметра еще не вошли -значит входим
	  g_mode_Menu |= BIT_MODIFY_REG;
	  }
	  else{
		  // в изменении параметра были - значит выходим
		  g_mode_Menu &= ~BIT_MODIFY_REG;
		  if(((g_mode_Menu & BITS_MENU) == MENU_TIME) &&
		     (((g_mode_Menu & BITS_SUB_MENU) == 0x09) || ((g_mode_Menu & BITS_SUB_MENU) == 0x0A))){
		  sTime.Hours = (uint8_t)(MODBUS_REG(0x44) >> 8);
		  sTime.Minutes = (uint8_t)(MODBUS_REG(0x44) & 0xFF);
		  sTime.Seconds = 0;
		  }
		  HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
	  }
	 } // end аналоговый ввод и ввод часов

	 // MENU_TYPE_PASSWORD
	 else if(menu.typeMenu[(g_mode_Menu & BITS_LINE) >> 4] == MENU_TYPE_PASSWORD)
	 {
     // вход в изменение параметра разрешен
	  if((g_mode_Menu & BIT_MODIFY_REG) == 0){
	  // в изменение параметра еще не вошли -значит входим
	  g_mode_Menu |= BIT_MODIFY_REG;
	  }
	  else{
		   // в изменении параметра были - значит выходим
		   g_mode_Menu &= ~BIT_MODIFY_REG;
	      }
	 } // end MENU_TYPE_PASSWORD


	 /////

	 else if(((password_OK == 1) || (password_OK == 2)) && ((menu.typeMenu[(g_mode_Menu & BITS_LINE) >> 4] == MENU_TYPE_DIGIT_INPUT))){
     // ввод цифрового значения  меняем его на противоположный
	 uint16_t temp;
	 temp = (g_mode_Menu & BITS_SUB_MENU) - 1;
	 temp = 1 << temp;

	 temp = MODBUS_REG(0x5F) ^ temp;

     MODBUS_REG(0x5F) = temp;

     g_mode_Menu |= BIT_CHANGE_VALUE;
	 } // end цифровой ввод

	 else if(((password_OK == 1) || (password_OK == 2)) && ((menu.typeMenu[(g_mode_Menu & BITS_LINE) >> 4] == MENU_TYPE_DIGIT_CHOICE))){
     // выбор одного из значениц Radio Button
		 if((g_mode_Menu & 0x000000F0) == MENU_UNIT_MODE){
			 uint16_t temp;
			 temp = (g_mode_Menu & BITS_SUB_MENU) - 1;

			 MODBUS_REG(0x48) = temp;
			 g_mode_Menu |= BIT_CHANGE_VALUE;
		 } // end MENU_UNIT_MODE

		 // CHECK_OK PASSWORD MENU_UNIT_TYPE
	     // выбор одного из значениц Radio Button
			 if(((password_OK == 2)) && (g_mode_Menu & 0x000000F0) == MENU_UNIT_TYPE && (((g_mode_Menu & BITS_SUB_MENU) < 7) && (g_mode_Menu & BITS_SUB_MENU) > 1)){
				 uint16_t temp;
				 temp = (g_mode_Menu & BITS_SUB_MENU) - 1;

				 MODBUS_REG(0x49) = temp;
				 g_mode_Menu |= BIT_CHANGE_VALUE;


				 /////////////////////////////////////////////////////////////////////////////////////////////////////
			 		char str[20];
			 		uint32_t startAddr = findStartAddrToWrite(START_ADDR_LAST_PAGE, END_ADDR_LAST_PAGE, 4, 21);

			 			HAL_StatusTypeDef result = HAL_FLASH_Unlock();
			 			if(result == HAL_OK){
			 				if(startAddr == END_ADDR_LAST_PAGE){
			 					// стираем страницу и выставляем для записи начальный адрес страницы
			 					static FLASH_EraseInitTypeDef EraseInitStruct;
			 					uint32_t PageError;
			 					uint8_t sofar = 0;
			 					EraseInitStruct.TypeErase = FLASH_TYPEERASE_PAGES;
			 					EraseInitStruct.PageAddress = START_ADDR_LAST_PAGE;
			 					EraseInitStruct.NbPages = 1; //((END_ADDR_LAST_PAGE - START_ADDR_LAST_PAGE)/FLASH_PAGE_SIZE)+1;
			 					if(HAL_FLASHEx_Erase(&EraseInitStruct, &PageError) != HAL_OK){

			 						return HAL_FLASH_GetError();
			 					}
			 					startAddr = START_ADDR_LAST_PAGE;

			 				}
			 				for(uint8_t i=0; i < 15;i++ ){
			 				      HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, startAddr + i*4, (uint32_t)MODBUS_REG(0x20 + i));
			 				}
						      HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, startAddr + 15*4, (uint32_t)MODBUS_REG(0x48));
							  HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, startAddr + 16*4, (uint32_t)MODBUS_REG(0x49));

				 				for(uint8_t i=0; i < 4;i++ ){
				 				      HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, startAddr + (17+i)*4, (uint32_t)MODBUS_REG(0x40 + i));
				 				}

							      HAL_FLASH_Lock();
							      ST7789_print_u(SHIFT_X_PRINT_SUBMENU, SHIFT_Y_PRINT_SUBMENU+0*STEP_Y_PRINT_SUBMENU, ST7789_BLUE, ST7789_WHITE, 1, &Font_16x26, 1, menu.strMenu[15][1]);

			 			}
			 			else if(result == HAL_ERROR){
						      ST7789_print_u(5, 45+0*30, RGB565(0, 0, 0), ST7789_WHITE, 1, &Font_16x26, 1, "   ERROR");
			 			}

			 			else if(result == HAL_BUSY){
						      ST7789_print_u(5, 45+0*30, ST7789_BLUE, ST7789_WHITE, 1, &Font_16x26, 1, "   BUSY");
			 			}

			 			else if(result == HAL_TIMEOUT){
						      ST7789_print_u(5, 45+0*30, ST7789_BLUE, ST7789_WHITE, 1, &Font_16x26, 1, "   TIMEOUT");
			 			}

				 /////////////////////////////////////////////////////////////////////////////////////////////////////


				 HAL_NVIC_SystemReset();  // сброс контроллера
			 } // end MENU_UNIT_TYPE

	 } // end цифровой выбора

 }

// // прроверяем возможность модифицировать параметры
// if((menu.typeMenu[(g_mode_Menu & BITS_LINE) >> 4] == 1)) {
//	 // модифицировть параметр можно переходим в режиммодификации
//	 if((g_mode_Menu & BIT_MODIFY_REG) == 0)
//	 g_mode_Menu |= BIT_MODIFY_REG;
//	 else
//		 g_mode_Menu &= ~BIT_MODIFY_REG;
// }

 g_mode_Menu |= BIT_BUTTON_EC11;
}

//-------------------------------------------------------------
//  конец Обработки нажатия кнопки
//-------------------------------------------------------------
/**
  * @brief  Фунция поворота  енкодера
  * @note  Эта фун-я не принимает аргументов и ничего не возвращает<br>
  *        - изменяет глобальную переменную g_mode_Menu (навигация по меню);<br>
  *        - выполняет действия по модификации Reg_Modbus;<br>
  *        - переключение тумблера на экране мнемосхемы<br>
*/
void OnCountEC11()
{
 int8_t delta = (ec11.count - ec11.prev_count);

 if((g_mode_Menu & BITS_SUB_MENU) == 0){
	 // перелистываем главное меню
     // сбрасываем флаг нахождения в подменю
	 g_mode_Menu &= ~BIT_ENTER_SUB_MENU;
	 // перелистываем меню
     if((status_Menu.line_menu + delta) < 0 )
    	 status_Menu.line_menu = LAST_POINT_MENU_MAIN;
     else if((status_Menu.line_menu + delta) > LAST_POINT_MENU_MAIN )
    	 status_Menu.line_menu =0;
     else
    	 status_Menu.line_menu = (status_Menu.line_menu + delta);

     g_mode_Menu = (g_mode_Menu & 0xFFFFFF0F) + ((status_Menu.line_menu) << 4);
     delta =0;
     // устанавливаем биты изменились: - меню + пункты подменю + значения + чекбоксы + стутс
     /////////////g_mode_Menu = g_mode_Menu | BIT_UPDATE_MENU | BIT_UPDATE_SUBMENU |
     /////////////		   BIT_UPDATE_VALUE | BIT_UPDATE_ALL_REGS | BIT_UPDATE_CHECKBOXES;

 }
 else {
	 if(!(g_mode_Menu & BIT_MODIFY_REG)){
	   // перелистываем подменю
       if((status_Menu.line_sub_menu + delta) <= 0 ) status_Menu.line_sub_menu = menu.pointsOfMenu[status_Menu.line_menu];
       else if((status_Menu.line_sub_menu + delta) > menu.pointsOfMenu[status_Menu.line_menu] ) status_Menu.line_sub_menu =0;
       else status_Menu.line_sub_menu = (status_Menu.line_sub_menu + delta);

       g_mode_Menu = (g_mode_Menu & 0xFFFFFFF0) + status_Menu.line_sub_menu;
       delta =0;
       // проверяем нужно обновить пункты меню и значения или только значения
       // ...
   	 }
	 else{
		 // меняем параметр

		 if((g_mode_Menu & BITS_MENU) == MENU_SET_POINTS){
		 // меняем уставки
		 MODBUS_REG(0x20 + (g_mode_Menu & BITS_SUB_MENU)- 1) += delta;
		 }

		 if((g_mode_Menu & BITS_MENU) == MENU_ANALOG_OUT){
		 // меняем ANALOG_OUT
		 uint16_t temp= MODBUS_REG(START_REG_AO_HND + (g_mode_Menu & BITS_SUB_MENU)- 1) + delta;
		 if(temp >= 100){
			 MODBUS_REG(START_REG_AO_HND + (g_mode_Menu & BITS_SUB_MENU)- 1) = 100;
		 }
		 else if((temp <= 0)){
			 MODBUS_REG(START_REG_AO_HND + (g_mode_Menu & BITS_SUB_MENU)- 1) = 0;
		 }
		 else{
			 MODBUS_REG(START_REG_AO_HND + (g_mode_Menu & BITS_SUB_MENU)- 1) += delta;
		 }

		 }  // end MENU_ANALOG_OUT

		 //=================================================================================================
		 // MENU_PASSWORD
		 if((g_mode_Menu & BITS_MENU) == MENU_PASSWORD){
		 // меняем один из четырех символов PASSWORD
//		 uint16_t temp= MODBUS_REG(0x46) + (((g_mode_Menu & BITS_SUB_MENU)- 1) * delta);

		password[(g_mode_Menu & BITS_SUB_MENU) - 1] += delta;
//		 if((g_mode_Menu & BITS_SUB_MENU) == 1){
//			  MODBUS_REG(0x46) += delta;
//		 }
//		 else if((g_mode_Menu & BITS_SUB_MENU) == 2){
//			 MODBUS_REG(0x46) += delta * 16;
//		 }
//		 else if((g_mode_Menu & BITS_SUB_MENU) == 3){
//			 MODBUS_REG(0x46) += delta * 256;
//		 }
//		 else{
//			 MODBUS_REG(0x46) += delta * 4096;
//		 }
		 }  // end MENU_PASSWORD
		 //=================================================================================================



		 if((g_mode_Menu & BITS_MENU) == MENU_TIME){
	      uint16_t temp=0;
		  // меняем время
          if((g_mode_Menu & BITS_SUB_MENU) % 2){
        	 temp = MODBUS_REG(0x40 + ((g_mode_Menu & BITS_SUB_MENU)- 1)/2) + (delta << 8);
        	 if((temp >> 8) < 24)
		       MODBUS_REG(0x40 + ((g_mode_Menu & BITS_SUB_MENU)- 1)/2) += (delta << 8);
        	 else
        	   MODBUS_REG(0x40 + ((g_mode_Menu & BITS_SUB_MENU)- 1)/2) = MODBUS_REG(0x40 + ((g_mode_Menu & BITS_SUB_MENU)- 1)/2) & 0x00ff + 0x1700;
          }
		  else{
			 temp = (MODBUS_REG(0x40 + ((g_mode_Menu & BITS_SUB_MENU)- 1)/2) + delta) & 0x00ff;
			 if(temp <= 59)
        	  MODBUS_REG(0x40 + ((g_mode_Menu & BITS_SUB_MENU)- 1)/2) += delta;
			 else
				 MODBUS_REG(0x40 + ((g_mode_Menu & BITS_SUB_MENU)- 1)/2) &= 0xff00;

          }
		 }

		 // переключение тумблера на экране мнемосхемы
		 if(((g_mode_Menu & BITS_MENU) == MENU_PICTURE) && ((g_mode_Menu & 0x0f) == 2)){
			 int8_t TempReg = (MODBUS_REG(0x3F) + delta) % 3;

			 if(TempReg <= 0) MODBUS_REG(0x3F) = 0;

			 else MODBUS_REG(0x3F) = (MODBUS_REG(0x3F) + delta) % 3;
		 }

		 // обновляем только одно значение
		 ////////////g_mode_Menu = g_mode_Menu | BIT_UPDATE_REG | BIT_UPDATE_CHECKBOX;
	 }
 }

     g_mode_Menu |= BIT_COUNT_EC11;
}

/**
  * @brief  Фунция получения номера экрана подменю
  * @param [in] smenu  uint32_t словосостояния меню
  * @return - номер экрана подменю 0-2
*/
uint8_t NumOfScreen(uint32_t smenu)
{
  if((smenu & BITS_SUB_MENU) < 6)
  return 0;
  else if((smenu & BITS_SUB_MENU) < 11)
	  return 1;
  else if((smenu & BITS_SUB_MENU) < 16)
	  return 2;
//  return (smenu & BITS_SUB_MENU) / 5;
}

//===============================================================
/**
  * @brief  Фунция определяет нужно ли перерисовывать экран?
  * @param [in] mode_Menu  uint32_t текущее состояние меню
  * @param [in] mode_Menu_old  uint32_t предыдущее состояние меню
  * @return  uint8_t 0 - нет перехода на другой экран; 1 - перехода на другой экран
*/
uint8_t IsChangeScreen(uint32_t mode_Menu, uint32_t mode_Menu_old)
{
	if((mode_Menu & BITS_MENU) == MENU_TIME)
		// в меню времени перехода нет
		return 0;

	if(NumOfScreen(mode_Menu & BITS_SUB_MENU)!= NumOfScreen(mode_Menu_old & BITS_SUB_MENU)){
	 // переход на другой экран
	 return 1;
	}
	else{
	 // перехода на другой экран нет
	 return 0;
	}
}

//char IsViewMenuChanged()
//{
// uint16_t screenNew = (menu.mode_Menu & BITS_LINE)/3;
// uint16_t screenOld = (menu.modeMenu_old & BITS_LINE)/3;
//
// menu.modeMenu_old = menu.mode_Menu;
//
// if(screenNew != screenOld) return 1;
// else return 0;
//
//}

// вид меню (элементы экрана)поменялся сместился курсор
// или данные обновились
//void SetBIT_VIEW_CH()
//{
//// menu.mode_Menu |= BIT_VIEW_CHANGE;
//// g_mode_Menu  |= BIT_VIEW_CHANGE;
//}

// изменение вида мею отработано
//void ClearBIT_VIEW_CH()
//{
//// menu.mode_Menu &= (~BIT_VIEW_CHANGE);
//// g_mode_Menu &= (~BIT_VIEW_CHANGE);
//}

// меню изменилось целиком
//void SetBIT_MENU_CH()
//{
// //menu.mode_Menu |= BIT_MENU_CH;
// //g_mode_Menu |= BIT_MENU_CH; // The Menu was changed
//}

// меню перерисовано
//void ClearBIT_MENU_CH()
//{
// //menu.mode_Menu &= (~BIT_MENU_CH);
// //g_mode_Menu &= (~BIT_MENU_CH);
//}


//void SetBIT_CH_ALLOW()
//{
//// menu.mode_Menu |= BIT_CH_ALLOW;
//// g_mode_Menu |= BIT_CH_ALLOW; // The Menu was changed
//}

//void ClearBIT_CH_ALLOW()
//{
//// menu.mode_Menu &= (~BIT_CH_ALLOW);
//// g_mode_Menu &= (~BIT_CH_ALLOW);
//}

/**
 * @brief данные требуют обновления бит "ОБНОВИТЬ ДАННЫЕ" установить
 */
// установить бит "ОБНОВИТЬ ДАННЫЕ"
void SetBIT_UPDATE_VALUE()
{
 menu.mode_Menu |= BIT_UPDATE_VALUE;
 g_mode_Menu |= BIT_UPDATE_VALUE;
}

/**
 * @brief данные обновлены бит "ОБНОВИТЬ ДАННЫЕ" сбросить
 */
// данные обновлены бит "ОБНОВИТЬ ДАННЫЕ" сбросить
void ClearBIT_UPDATE_VALUE()
{
 menu.mode_Menu &= (~BIT_UPDATE_VALUE);
 g_mode_Menu &= (~BIT_UPDATE_VALUE);
}

/**
 * @brief Инициализация Меню
 * @note инициализируются пункты Меню
 */
char InitMenu()
{
   menu.mode_Menu = 0;
   menu.stepUpdate = 1;

   menu.colorsOfFontsMenu = RGB565(0, 0, 0);
   menu.colorsOfBackMenu = ST7789_WHITE;

 //  menu.pointsOfMenu[0] = LAST_POINT_MENU_MAIN;  // Last point Main Menu
   menu.pointsOfMenu[0] = 2;  // Last point Img Menu
   menu.pointsOfMenu[1] = LAST_POINT_MENU_STATUS;  // Last point Menu Status
   menu.pointsOfMenu[2] = LAST_POINT_MENU_SETPOINTS;  // SetPoint
   menu.pointsOfMenu[3] = LAST_POINT_MENU_SENSORS;  // Sensors
   menu.pointsOfMenu[4] = LAST_POINT_MENU_DIGITOUT;  // Digit Out
   menu.pointsOfMenu[5] = LAST_POINT_MENU_ANALOGOUT;  // Analog Out
   menu.pointsOfMenu[6] = LAST_POINT_MENU_ANALOGIN;  // Analog IN
   menu.pointsOfMenu[7] = LAST_POINT_MENU_ALARMS;   // Alarms
   menu.pointsOfMenu[10] = LAST_POINT_MENU_UNINTYPE; // Unit Type
   menu.pointsOfMenu[11] = LAST_POINT_MENU_MODE;  // Mode
   menu.pointsOfMenu[9] = LAST_POINT_MENU_SAVE;   // Save SP
   menu.pointsOfMenu[8] = LAST_POINT_MENU_VERSION;   // Version
   menu.pointsOfMenu[12] = LAST_POINT_MENU_TIME;  // Time
   menu.pointsOfMenu[13] = LAST_POINT_MENU_PASSWORD;  // Password

   menu.cursorType[0] = CURSOR_POINT;
   menu.cursorType[1] = 0;
   menu.cursorType[2] = CURSOR_POINT;
   menu.cursorType[3] = 2;
   menu.cursorType[4] = CURSOR_POINT;
   menu.cursorType[5] = 2;
   menu.cursorType[6] = 2;
   menu.cursorType[7] = CURSOR_POINT;
   menu.cursorType[8] = 0;
   menu.cursorType[9] = 0;
   menu.cursorType[10] = 2;
   menu.cursorType[11] = 2;
   menu.cursorType[12] = 2;
   
   menu.cursorType[13] = 2;

   menu.cursorAllowed[0] = 1;
   menu.cursorAllowed[1] = 0;
   menu.cursorAllowed[2] = 1;
   menu.cursorAllowed[3] = 1;
   menu.cursorAllowed[4] = 1;
   menu.cursorAllowed[5] = 1;
   menu.cursorAllowed[6] = 1;
   menu.cursorAllowed[7] = 1;
   menu.cursorAllowed[8] = 0;  // Cursor absent
   menu.cursorAllowed[9] = 0;  // Cursor absent
   menu.cursorAllowed[10] = 1;
   menu.cursorAllowed[11] = 1;
   menu.cursorAllowed[12] = 1;

   menu.cursorAllowed[13] = 1;

   // возможность модификации
   menu.typeMenu[0] = MENU_TYPE_ANALOG_INPUT;//"*  Picture  *";   // тумблер ПУСК / СТОП / Календарь
   menu.typeMenu[1] = 0;//"<   Status  >";
   menu.typeMenu[2] = MENU_TYPE_ANALOG_INPUT;     //"* SetPoints *";
   menu.typeMenu[3] = 0;//"*  Sensors  *";
   menu.typeMenu[4] = MENU_TYPE_DIGIT_INPUT;      //"* Digit Out *";
   menu.typeMenu[5] = MENU_TYPE_ANALOG_INPUT;     //"* AnalogOut *";
   menu.typeMenu[6] = 0;//"* Analog In *";
   menu.typeMenu[7] = 0;//"*  Alarms   *";
   menu.typeMenu[8] = MENU_TYPE_INFO;              //"*  Version  *";
   menu.typeMenu[9] = MENU_TYPE_SAVE;              //"*  Save SP  *";
   menu.typeMenu[10] = MENU_TYPE_DIGIT_CHOICE; //"* Unit Type *";
   menu.typeMenu[11] = MENU_TYPE_DIGIT_CHOICE;      //"*    Mode   *";
   menu.typeMenu[12] = MENU_TYPE_TIME;              //"*   Time    *";
   menu.typeMenu[13] = MENU_TYPE_PASSWORD;      // MENU_TYPE_PASSWORD;              //"*   Password    *";

#ifdef ENG_MENU
   // заголовки меню
   menu.strMain_Menu[0] = "*   Picture   *";
   menu.strMain_Menu[1] = "<   Status    >";
   menu.strMain_Menu[2] = "*  SetPoints  *";
   menu.strMain_Menu[3] = "*   Sensors   *";
   menu.strMain_Menu[4] = "*  Digit Out  *";
   menu.strMain_Menu[5] = "*  AnalogOut  *";
   menu.strMain_Menu[6] = "*  Analog In  *";
   menu.strMain_Menu[7] = "*   Alarms    *";
   menu.strMain_Menu[8] = "*   Version   *";
   menu.strMain_Menu[9] = "*   Save SP   *";
   menu.strMain_Menu[10] = "*  Unit Type  *";
   menu.strMain_Menu[11] = "*    Mode     *";
   menu.strMain_Menu[12] = "*    Time     *";

   menu.strMenu[0][0] = "Picture    ";
   menu.strMenu[0][1] = "Status     ";
   menu.strMenu[0][2] = "Set Points ";
   menu.strMenu[0][3] = "Sensors    ";
   menu.strMenu[0][4] = "Digit. Out ";
   menu.strMenu[0][5] = "Analog Out ";
   menu.strMenu[0][6] = "Analog IN  ";
   menu.strMenu[0][7] = "Alarms     ";
   menu.strMenu[0][8] = "Version    ";
   menu.strMenu[0][9] = "Save SP    ";
   menu.strMenu[0][10] = "HVAC 1     ";//menu.strMenu[9][menu.unitType];  //"Unit Type  ";
   menu.strMenu[0][11] = "Cool       ";//menu.strMenu[10][menu.unitMode];  //"Mode       ";
   menu.strMenu[0][12] = "Calendar   ";
   menu.strMenu[0][13] = "Reserv 2   ";
   menu.strMenu[0][14] = "Reserv 3   ";
   menu.strMenu[0][15] = "Reserv 4   ";
   menu.strMenu[0][16] = "Reserv 5   ";

//   menu.strMenu[1][0] = "Picture 1";
//   menu.strMenu[1][1] = " ";
//   menu.strMenu[1][2] = " ";
//   menu.strMenu[1][3] = " ";
//   menu.strMenu[1][4] = " ";
//   menu.strMenu[1][5] = " ";
//   menu.strMenu[1][6] = " ";
//   menu.strMenu[1][7] = " ";
//   menu.strMenu[1][8] = " ";
//   menu.strMenu[1][9] = " ";
//   menu.strMenu[1][10] = " ";
//   menu.strMenu[1][11] = " ";
//   menu.strMenu[1][12] = " ";
//   menu.strMenu[1][13] = " ";
//   menu.strMenu[1][14] = " ";
//   menu.strMenu[1][15] = " ";

   // * Status *
//   menu.strMenu[1][0] = "[1][0]        ";
   menu.strMenu[1][0] = "Fan 1         ";
   menu.strMenu[1][1] = "Fan 2         ";
   menu.strMenu[1][2] = "DI3           ";
   menu.strMenu[1][3] = "DI4           ";
   menu.strMenu[1][4] = "DI5           ";
   menu.strMenu[1][5] = "DI6           ";
   menu.strMenu[1][6] = "DI7           ";
   menu.strMenu[1][7] = "DI8           ";
   menu.strMenu[1][8] = "DI9           ";
   menu.strMenu[1][9] = "DI10          ";
   menu.strMenu[1][10] = "DI11          ";
   menu.strMenu[1][11] = "DI12          ";
   menu.strMenu[1][12] = "DI13          ";
   menu.strMenu[1][13] = "DI14          ";
   menu.strMenu[1][14] = "DI15          ";
   menu.strMenu[1][15] = "DI16          ";

   // * Set Points *
  // menu.strMenu[2][0] = "[2][0]        ";
   menu.strMenu[2][0] = "SetP 1        ";
   menu.strMenu[2][1] = "SetP 2        ";
   menu.strMenu[2][2] = "SetP 3        ";
   menu.strMenu[2][3] = "SetP 4        ";
   menu.strMenu[2][4] = "SetP 5        ";
   menu.strMenu[2][5] = "SetP 6        ";
   menu.strMenu[2][6] = "SetP 7        ";
   menu.strMenu[2][7] = "SetP 8        ";
   menu.strMenu[2][8] = "SetP 9        ";
   menu.strMenu[2][9] = "SetP 10       ";
   menu.strMenu[2][10] = "SetP 11      ";
   menu.strMenu[2][11] = "SetP 12       ";
   menu.strMenu[2][12] = "SetP 13       ";
   menu.strMenu[2][13] = "SetP 14       ";
   menu.strMenu[2][14] = "SetP 15       ";
   menu.strMenu[2][15] = "SetP 16       ";

   menu.strMenu[3][0] = "T1              ";
   menu.strMenu[3][1] = "T2              ";
   menu.strMenu[3][2] = "T3              ";
   menu.strMenu[3][3] = "T4              ";
   menu.strMenu[3][4] = "T5              ";
   menu.strMenu[3][5] = "T6              ";
   menu.strMenu[3][6] = "T7              ";
   menu.strMenu[3][7] = "T8              ";
   menu.strMenu[3][8] = "T9              ";
   menu.strMenu[3][9] = "T10             ";
   menu.strMenu[3][10] = "T11             ";
   menu.strMenu[3][11] = "T12             ";
   menu.strMenu[3][12] = "T13             ";
   menu.strMenu[3][13] = "T14             ";
   menu.strMenu[3][14] = "T15             ";
   menu.strMenu[3][15] = "T16             ";

   menu.strMenu[4][0] = "DO1            ";
   menu.strMenu[4][1] = "DO2            ";
   menu.strMenu[4][2] = "DO3            ";
   menu.strMenu[4][3] = "DO4            ";
   menu.strMenu[4][4] = "DO5            ";
   menu.strMenu[4][5] = "DO6            ";
   menu.strMenu[4][6] = "DO7            ";
   menu.strMenu[4][7] = "DO8            ";
   menu.strMenu[4][8] = "DO9            ";
   menu.strMenu[4][9] = "DO10           ";
   menu.strMenu[4][10] = "DO11           ";
   menu.strMenu[4][11] = "DO12           ";
   menu.strMenu[4][12] = "DO13           ";
   menu.strMenu[4][13] = "DO14           ";
   menu.strMenu[4][14] = "DO15           ";
   menu.strMenu[4][15] = "DO16           ";

   menu.strMenu[5][0] = "AO1            ";
   menu.strMenu[5][1] = "AO2            ";
   menu.strMenu[5][2] = "AO3            ";
   menu.strMenu[5][3] = "AO4            ";
   menu.strMenu[5][4] = "AO5            ";
   menu.strMenu[5][5] = "AO6            ";
   menu.strMenu[5][6] = "AO7            ";
   menu.strMenu[5][7] = "AO8            ";
   menu.strMenu[5][8] = "AO9            ";
   menu.strMenu[5][9] = "AO10           ";
   menu.strMenu[5][10] = "AO11           ";
   menu.strMenu[5][11] = "AO12           ";
   menu.strMenu[5][12] = "AO13           ";
   menu.strMenu[5][13] = "AO14           ";
   menu.strMenu[5][14] = "AO15           ";
   menu.strMenu[5][15] = "AO16           ";

   menu.strMenu[6][0] = "AI1            ";
   menu.strMenu[6][1] = "AI2            ";
   menu.strMenu[6][2] = "AI3            ";
   menu.strMenu[6][3] = "AI4            ";
   menu.strMenu[6][4] = "AI5            ";
   menu.strMenu[6][5] = "AI6            ";
   menu.strMenu[6][6] = "AI7            ";
   menu.strMenu[6][7] = "AI8            ";
   menu.strMenu[6][8] = "AI9            ";
   menu.strMenu[6][9] = "AI10           ";
   menu.strMenu[6][10] = "AI11           ";
   menu.strMenu[6][11] = "AI12           ";
   menu.strMenu[6][12] = "AI13           ";
   menu.strMenu[6][13] = "AI14           ";
   menu.strMenu[6][14] = "AI16           ";

   menu.strMenu[7][0] = "Alm1           ";
   menu.strMenu[7][1] = "Alm2           ";
   menu.strMenu[7][2] = "Alm3           ";
   menu.strMenu[7][3] = "Alm4           ";
   menu.strMenu[7][4] = "Alm5           ";
   menu.strMenu[7][5] = "Alm6           ";
   menu.strMenu[7][6] = "Alm7           ";
   menu.strMenu[7][7] = "Alm8           ";
   menu.strMenu[7][8] = "Alm9           ";
   menu.strMenu[7][9] = "Alm10          ";
   menu.strMenu[7][10] = "Alm11          ";
   menu.strMenu[7][11] = "Alm12          ";
   menu.strMenu[7][12] = "Alm13          ";
   menu.strMenu[7][13] = "Alm14          ";
   menu.strMenu[7][14] = "Alm15          ";
   menu.strMenu[7][15] = "Alm16          ";

   menu.strMenu[8][0] = " Effectovent   ";
   menu.strMenu[8][1] = "  Standart     ";
   menu.strMenu[8][2] = "  Ver 1.00     ";
   menu.strMenu[8][3] = "               ";
   menu.strMenu[8][4] = "               ";
   menu.strMenu[8][5] = "               ";
   menu.strMenu[8][6] = "";
   menu.strMenu[8][7] = "";
   menu.strMenu[8][8] = "";
   menu.strMenu[8][9] = "";
   menu.strMenu[8][10] = "";
   menu.strMenu[8][11] = "";
   menu.strMenu[8][12] = "";
   menu.strMenu[8][13] = "";
   menu.strMenu[8][14] = "";
   menu.strMenu[8][15] = "";

   menu.strMenu[9][0] = "   Save        ";
   menu.strMenu[9][1] = "               ";
   menu.strMenu[9][2] = "               ";
   menu.strMenu[9][3] = "               ";
   menu.strMenu[9][4] = "               ";
   menu.strMenu[9][5] = "";
   menu.strMenu[9][6] = "";
   menu.strMenu[9][7] = "";
   menu.strMenu[9][8] = "";
   menu.strMenu[9][9] = "";
   menu.strMenu[9][10] = "";
   menu.strMenu[9][11] = "";
   menu.strMenu[9][12] = "";
   menu.strMenu[9][13] = "";
   menu.strMenu[9][14] = "";
   menu.strMenu[9][15] = "";

   menu.strMenu[10][0] = "Slave         ";
   menu.strMenu[10][1] = "HVAC1         ";
   menu.strMenu[10][2] = "HVAC2         ";
   menu.strMenu[10][3] = "HVAC3         ";
   menu.strMenu[10][4] = "HVAC4         ";
   menu.strMenu[10][5] = "HVAC5         ";
   menu.strMenu[10][6] = "HVAC6         ";
   menu.strMenu[10][7] = "HVAC7         ";
   menu.strMenu[10][8] = "HVAC8         ";
   menu.strMenu[10][9] = "HVAC9         ";
   menu.strMenu[10][10] = "HVAC10        ";
   menu.strMenu[10][11] = "HVAC11        ";
   menu.strMenu[10][12] = "HVAC12        ";
   menu.strMenu[10][13] = "HVAC13        ";
   menu.strMenu[10][14] = "HVAC14        ";
   menu.strMenu[10][15] = "HVAC15        ";
   
   menu.strMenu[11][0] = "Hand          ";
   menu.strMenu[11][1] = "Cool          ";
   menu.strMenu[11][2] = "Fan           ";
   menu.strMenu[11][3] = "Heat          ";
   menu.strMenu[11][4] = "              ";
   menu.strMenu[11][5] = "";
   menu.strMenu[11][6] = "";
   menu.strMenu[11][7] = "";
   menu.strMenu[11][8] = "";
   menu.strMenu[11][9] = "";
   menu.strMenu[11][10] = "";
   menu.strMenu[11][11] = "";
   menu.strMenu[11][12] = "";
   menu.strMenu[11][13] = "";
   menu.strMenu[11][14] = "";
   menu.strMenu[11][15] = "";

   menu.strMenu[12][0] = "Start         ";
   menu.strMenu[12][1] = "Stop          ";
   menu.strMenu[12][2] = "Day           ";
   menu.strMenu[12][3] = "Night         ";
   menu.strMenu[12][4] = "Time          ";
   menu.strMenu[12][5] = "";
   menu.strMenu[12][6] = "";
   menu.strMenu[12][7] = "";
   menu.strMenu[12][8] = "";
   menu.strMenu[12][9] = "";
   menu.strMenu[12][10] = "";
   menu.strMenu[12][11] = "";
   menu.strMenu[12][12] = "";
   menu.strMenu[12][13] = "";
   menu.strMenu[12][14] = "";
   menu.strMenu[12][15] = "";

   menu.strMenu[15][0] = "                    ";
   menu.strMenu[15][1] = "       Saved!       ";
   menu.strMenu[15][2] = "                    ";


   menu.strTempScreen[0][0] = " ";
   menu.strTempScreen[0][0] = " ";
   menu.strTempScreen[0][0] = " ";


   menu.strMode[0] = "Hand";
   menu.strMode[1] = "Cool";
   menu.strMode[2] = "Fan ";
   menu.strMode[3] = "Heat";

#endif

#ifdef RUS_MENU
   // заголовки меню
   menu.strMain_Menu[0] = "*    Схема    *";
   menu.strMain_Menu[1] = "*  Цифр. Сиг  *";//"<   Статусы   >";
   menu.strMain_Menu[2] = "*   Уставки   *";
   menu.strMain_Menu[3] = "*   Датчики   *";
   menu.strMain_Menu[4] = "*  Цифр.  Упр *";
   menu.strMain_Menu[5] = "* Аналог. Упр *";
   menu.strMain_Menu[6] = "* Аналог. Сиг *";
   menu.strMain_Menu[7] = "*   Ошибки    *";
   menu.strMain_Menu[8] = "*   Версия    *";
   menu.strMain_Menu[9] = "* Сохранение  *";
   menu.strMain_Menu[10] = "* Тип системы *";
   menu.strMain_Menu[11] = "*    Режим    *";
   menu.strMain_Menu[12] = "*    Время    *";

   menu.strMain_Menu[13] = "*   Пароль    *";

   menu.strMenu[0][0] = "Picture    ";
   menu.strMenu[0][1] = "Status     ";
   menu.strMenu[0][2] = "Set Points ";
   menu.strMenu[0][3] = "Sensors    ";
   menu.strMenu[0][4] = "Digit. Out ";
   menu.strMenu[0][5] = "Analog Out ";
   menu.strMenu[0][6] = "Analog IN  ";
   menu.strMenu[0][7] = "Alarms     ";
   menu.strMenu[0][8] = "Version    ";
   menu.strMenu[0][9] = "Save SP    ";
   menu.strMenu[0][10] = "HVAC 1     ";//menu.strMenu[9][menu.unitType];  //"Unit Type  ";
   menu.strMenu[0][11] = "Cool       ";//menu.strMenu[10][menu.unitMode];  //"Mode       ";
   menu.strMenu[0][12] = "Calendar   ";
   menu.strMenu[0][13] = "Reserv 2   ";
   menu.strMenu[0][14] = "Reserv 3   ";
   menu.strMenu[0][15] = "Reserv 4   ";
   menu.strMenu[0][16] = "Reserv 5   ";
   
//   menu.strMenu[1][0] = "Picture 1";
//   menu.strMenu[1][1] = " ";
//   menu.strMenu[1][2] = " ";
//   menu.strMenu[1][3] = " ";
//   menu.strMenu[1][4] = " ";
//   menu.strMenu[1][5] = " ";
//   menu.strMenu[1][6] = " ";
//   menu.strMenu[1][7] = " ";
//   menu.strMenu[1][8] = " ";
//   menu.strMenu[1][9] = " ";
//   menu.strMenu[1][10] = " ";
//   menu.strMenu[1][11] = " ";
//   menu.strMenu[1][12] = " ";
//   menu.strMenu[1][13] = " ";
//   menu.strMenu[1][14] = " ";
//   menu.strMenu[1][15] = " ";

   // * Status *
//   menu.strMenu[1][0] = "Насос 1       ";
//   menu.strMenu[1][1] = "Циф.Вх2       ";
//   menu.strMenu[1][2] = "Цифр. Вх3     ";
//   menu.strMenu[1][3] = "Цифр. Вх4     ";
//   menu.strMenu[1][4] = "Цифр. Вх5     ";
//   menu.strMenu[1][5] = "Цифр. Вх6     ";
//   menu.strMenu[1][6] = "Цифр. Вх7     ";
//   menu.strMenu[1][7] = "Цифр. Вх8     ";
//   menu.strMenu[1][8] = "Цифр. Вх9     ";
//   menu.strMenu[1][9] = "Цифр. Вх10    ";
//   menu.strMenu[1][10] = "Цифр. Вх11    ";
//   menu.strMenu[1][11] = "Цифр. Вх12    ";
//   menu.strMenu[1][12] = "Цифр. Вх13    ";
//   menu.strMenu[1][13] = "Цифр. Вх14    ";
//   menu.strMenu[1][14] = "Цифр. Вх15    ";
//   menu.strMenu[1][15] = "Цифр. Вх16    ";
   menu.strMenu[1][0] = "DI1           ";
   menu.strMenu[1][1] = "DI2           ";
   menu.strMenu[1][2] = "DI3           ";
   menu.strMenu[1][3] = "DI4           ";
   menu.strMenu[1][4] = "DI5           ";
   menu.strMenu[1][5] = "DO1           ";
   menu.strMenu[1][6] = "DO2           ";
   menu.strMenu[1][7] = "DO3           ";
   menu.strMenu[1][8] = "DO4           ";
   menu.strMenu[1][9] = "DO5           ";
   menu.strMenu[1][10] = "DO6           ";
   menu.strMenu[1][11] = "DO7           ";
   menu.strMenu[1][12] = "DO8           ";
   menu.strMenu[1][13] = "DO9           ";
   menu.strMenu[1][14] = "DO10          ";
   menu.strMenu[1][15] = "DO11          ";
   
   // * Set Points *
   menu.strMenu[2][0] = "Уст. 1         ";
   menu.strMenu[2][1] = "Уст. 2         ";
   menu.strMenu[2][2] = "Уст. 3         ";
   menu.strMenu[2][3] = "Уст. 4         ";
   menu.strMenu[2][4] = "Уст. 5         ";
   menu.strMenu[2][5] = "Уст. 6         ";
   menu.strMenu[2][6] = "Уст. 7         ";
   menu.strMenu[2][7] = "Уст. 8         ";
   menu.strMenu[2][8] = "Уст. 9         ";
   menu.strMenu[2][9] = "Уст. 10         ";
   menu.strMenu[2][10] = "Уст. 11        ";
   menu.strMenu[2][11] = "Уст. 12        ";
   menu.strMenu[2][12] = "Уст. 13        ";
   menu.strMenu[2][13] = "Уст. 14        ";
   menu.strMenu[2][14] = "Уст. 15        ";
   menu.strMenu[2][15] = "Уст. 16        ";
   
   menu.strMenu[3][0] = "T1              ";
   menu.strMenu[3][1] = "T2              ";
   menu.strMenu[3][2] = "T3              ";
   menu.strMenu[3][3] = "T4              ";
   menu.strMenu[3][4] = "T5              ";
   menu.strMenu[3][5] = "T6              ";
   menu.strMenu[3][6] = "T7              ";
   menu.strMenu[3][7] = "T8              ";
   menu.strMenu[3][8] = "T9              ";
   menu.strMenu[3][9] = "T10             ";
   menu.strMenu[3][10] = "T11             ";
   menu.strMenu[3][11] = "T12             ";
   menu.strMenu[3][12] = "T13             ";
   menu.strMenu[3][13] = "T14             ";
   menu.strMenu[3][14] = "T15             ";
   menu.strMenu[3][15] = "T16             ";
   
//   menu.strMenu[4][0] = "Цифр. Вых1      ";
//   menu.strMenu[4][1] = "Цифр. Вых2      ";
//   menu.strMenu[4][2] = "Цифр. Вых3      ";
//   menu.strMenu[4][3] = "Цифр. Вых4      ";
//   menu.strMenu[4][4] = "Цифр. Вых5      ";
//   menu.strMenu[4][5] = "Цифр. Вых6      ";
//   menu.strMenu[4][6] = "Цифр. Вых7      ";
//   menu.strMenu[4][7] = "Цифр. Вых8      ";
//   menu.strMenu[4][8] = "Цифр. Вых9      ";
//   menu.strMenu[4][9] = "Цифр. Вых10     ";
//   menu.strMenu[4][10] = "Цифр. Вых11     ";
//   menu.strMenu[4][11] = "Цифр. Вых12     ";
//   menu.strMenu[4][12] = "Цифр. Вых13     ";
//   menu.strMenu[4][13] = "Цифр. Вых14     ";
//   menu.strMenu[4][14] = "Цифр. Вых15     ";
//   menu.strMenu[4][15] = "Цифр. Вых16     ";
//
//   menu.strMenu[5][0] = "Аналог. Вых1   ";
//   menu.strMenu[5][1] = "Аналог. Вых2   ";
//   menu.strMenu[5][2] = "Аналог. Вых3   ";
//   menu.strMenu[5][3] = "Аналог. Вых4   ";
//   menu.strMenu[5][4] = "Аналог. Вых5   ";
//   menu.strMenu[5][5] = "Аналог. Вых6   ";
//   menu.strMenu[5][6] = "Аналог. Вых7   ";
//   menu.strMenu[5][7] = "Аналог. Вых8   ";
//   menu.strMenu[5][8] = "Аналог. Вых9   ";
//   menu.strMenu[5][9] = "Аналог. Вых10  ";
//   menu.strMenu[5][10] = "Аналог. Вых11  ";
//   menu.strMenu[5][11] = "Аналог. Вых12  ";
//   menu.strMenu[5][12] = "Аналог. Вых13  ";
//   menu.strMenu[5][13] = "Аналог. Вых14  ";
//   menu.strMenu[5][14] = "Аналог. Вых15  ";
//   menu.strMenu[5][15] = "Аналог. Вых16  ";
//
//   menu.strMenu[6][0] = "Аналог. Вх1    ";
//   menu.strMenu[6][1] = "Аналог. Вх2    ";
//   menu.strMenu[6][2] = "Аналог. Вх3    ";
//   menu.strMenu[6][3] = "Аналог. Вх4    ";
//   menu.strMenu[6][4] = "Аналог. Вх5    ";
//   menu.strMenu[6][5] = "Аналог. Вх6    ";
//   menu.strMenu[6][6] = "Аналог. Вх7    ";
//   menu.strMenu[6][7] = "Аналог. Вх8    ";
//   menu.strMenu[6][8] = "Аналог. Вх9    ";
//   menu.strMenu[6][9] = "Аналог. Вх10   ";
//   menu.strMenu[6][10] = "Аналог. Вх11   ";
//   menu.strMenu[6][11] = "Аналог. Вх12   ";
//   menu.strMenu[6][12] = "Аналог. Вх13   ";
//   menu.strMenu[6][13] = "Аналог. Вх14   ";
//   menu.strMenu[6][14] = "Аналог. Вх15   ";

   menu.strMenu[4][0] = "DO1            ";
   menu.strMenu[4][1] = "DO2            ";
   menu.strMenu[4][2] = "DO3            ";
   menu.strMenu[4][3] = "DO4            ";
   menu.strMenu[4][4] = "DO5            ";
   menu.strMenu[4][5] = "DO6            ";
   menu.strMenu[4][6] = "DO7            ";
   menu.strMenu[4][7] = "DO8            ";
   menu.strMenu[4][8] = "DO9            ";
   menu.strMenu[4][9] = "DO10           ";
   menu.strMenu[4][10] = "DO11           ";
   menu.strMenu[4][11] = "DO12           ";
   menu.strMenu[4][12] = "DO13           ";
   menu.strMenu[4][13] = "DO14           ";
   menu.strMenu[4][14] = "DO15           ";
   menu.strMenu[4][15] = "DO16           ";

   menu.strMenu[5][0] = "AO1            ";
   menu.strMenu[5][1] = "AO2            ";
   menu.strMenu[5][2] = "AO3            ";
   menu.strMenu[5][3] = "AO4            ";
   menu.strMenu[5][4] = "AO5            ";
   menu.strMenu[5][5] = "AO6            ";
   menu.strMenu[5][6] = "AO7            ";
   menu.strMenu[5][7] = "AO8            ";
   menu.strMenu[5][8] = "AO9            ";
   menu.strMenu[5][9] = "AO10           ";
   menu.strMenu[5][10] = "AO11           ";
   menu.strMenu[5][11] = "AO12           ";
   menu.strMenu[5][12] = "AO13           ";
   menu.strMenu[5][13] = "AO14           ";
   menu.strMenu[5][14] = "AO15           ";
   menu.strMenu[5][15] = "AO16           ";

   menu.strMenu[6][0] = "AI1            ";
   menu.strMenu[6][1] = "AI2            ";
   menu.strMenu[6][2] = "AI3            ";
   menu.strMenu[6][3] = "AI4            ";
   menu.strMenu[6][4] = "AI5            ";
   menu.strMenu[6][5] = "AO1            ";
   menu.strMenu[6][6] = "AO2            ";
   menu.strMenu[6][7] = "AO3            ";
   menu.strMenu[6][8] = "AO4            ";
   menu.strMenu[6][9] = "AO5            ";
   menu.strMenu[6][10] = "AO6            ";
   menu.strMenu[6][11] = "AO7            ";
   menu.strMenu[6][12] = "AO8            ";
   menu.strMenu[6][13] = "AO9            ";
   menu.strMenu[6][14] = "AO10           ";
   
   
   menu.strMenu[7][0] = "Авар.1         ";
   menu.strMenu[7][1] = "Авар.2         ";
   menu.strMenu[7][2] = "Авар.3         ";
   menu.strMenu[7][3] = "Авар.4         ";
   menu.strMenu[7][4] = "Авар.5         ";
   menu.strMenu[7][5] = "Авар.6         ";
   menu.strMenu[7][6] = "Авар.7         ";
   menu.strMenu[7][7] = "Авар.8         ";
   menu.strMenu[7][8] = "Авар.9         ";
   menu.strMenu[7][9] = "Авар.10         ";
   menu.strMenu[7][10] = "Авар.11        ";
   menu.strMenu[7][11] = "Авар.12        ";
   menu.strMenu[7][12] = "Авар.13        ";
   menu.strMenu[7][13] = "Авар.14        ";
   menu.strMenu[7][14] = "Авар.15        ";
   menu.strMenu[7][15] = "Авар.16        ";

//   menu.strMenu[8][0] = "  Эффектовент  ";
//   menu.strMenu[8][1] = "   Стандарт    ";
//   menu.strMenu[8][2] = "  Вер.  1.00   ";
     menu.strMenu[8][0] = "  Контроллер   ";
     menu.strMenu[8][1] = "  HVAC1...5    ";
     menu.strMenu[8][2] = "  Вер.  1.06   ";
   menu.strMenu[8][3] = "               ";
   menu.strMenu[8][4] = "               ";
   menu.strMenu[8][5] = "               ";
   menu.strMenu[8][6] = "";
   menu.strMenu[8][7] = "";
   menu.strMenu[8][8] = "";
   menu.strMenu[8][9] = "";
   menu.strMenu[8][10] = "";
   menu.strMenu[8][11] = "";
   menu.strMenu[8][12] = "";
   menu.strMenu[8][13] = "";
   menu.strMenu[8][14] = "";
   menu.strMenu[8][15] = "";

   menu.strMenu[9][0] = "  Сохранить    ";
   menu.strMenu[9][1] = "               ";
   menu.strMenu[9][2] = "               ";
   menu.strMenu[9][3] = "               ";
   menu.strMenu[9][4] = "               ";
   menu.strMenu[9][5] = "";
   menu.strMenu[9][6] = "";
   menu.strMenu[9][7] = "";
   menu.strMenu[9][8] = "";
   menu.strMenu[9][9] = "";
   menu.strMenu[9][10] = "";
   menu.strMenu[9][11] = "";
   menu.strMenu[9][12] = "";
   menu.strMenu[9][13] = "";
   menu.strMenu[9][14] = "";
   menu.strMenu[9][15] = "";

   menu.strMenu[10][0] = "Ведомый       ";
   menu.strMenu[10][1] = "HVAC1         ";
   menu.strMenu[10][2] = "HVAC2         ";
   menu.strMenu[10][3] = "HVAC3         ";
   menu.strMenu[10][4] = "HVAC4         ";
   menu.strMenu[10][5] = "HVAC5         ";
   menu.strMenu[10][6] = "HVAC6         ";
   menu.strMenu[10][7] = "HVAC7         ";
   menu.strMenu[10][8] = "HVAC8         ";
   menu.strMenu[10][9] = "HVAC9         ";
   menu.strMenu[10][10] = "HVAC10        ";
   menu.strMenu[10][11] = "HVAC11        ";
   menu.strMenu[10][12] = "HVAC12        ";
   menu.strMenu[10][13] = "HVAC13        ";
   menu.strMenu[10][14] = "HVAC14        ";
   menu.strMenu[10][15] = "HVAC15        ";
   
   menu.strMenu[11][0] = "Ручной        ";
   menu.strMenu[11][1] = "Лето          ";
   menu.strMenu[11][2] = "Вент.         ";
   menu.strMenu[11][3] = "Зима          ";
   menu.strMenu[11][4] = "              ";
   menu.strMenu[11][5] = "";
   menu.strMenu[11][6] = "";
   menu.strMenu[11][7] = "";
   menu.strMenu[11][8] = "";
   menu.strMenu[11][9] = "";
   menu.strMenu[11][10] = "";
   menu.strMenu[11][11] = "";
   menu.strMenu[11][12] = "";
   menu.strMenu[11][13] = "";
   menu.strMenu[11][14] = "";
   menu.strMenu[11][15] = "";
   
   menu.strMenu[12][0] = "Пуск          ";
   menu.strMenu[12][1] = "Стоп          ";
   menu.strMenu[12][2] = "День          ";
   menu.strMenu[12][3] = "Ночь          ";
   menu.strMenu[12][4] = "Время         ";
   menu.strMenu[12][5] = "";
   menu.strMenu[12][6] = "";
   menu.strMenu[12][7] = "";
   menu.strMenu[12][8] = "";
   menu.strMenu[12][9] = "";
   menu.strMenu[12][10] = "";
   menu.strMenu[12][11] = "";
   menu.strMenu[12][12] = "";
   menu.strMenu[12][13] = "";
   menu.strMenu[12][14] = "";
   menu.strMenu[12][15] = "";

   menu.strMenu[13][0] = "              ";
   menu.strMenu[13][1] = "              ";
   menu.strMenu[13][2] = "              ";
   menu.strMenu[13][3] = "              ";
   menu.strMenu[13][4] = "              ";

   menu.strMenu[15][0] = "              ";
   menu.strMenu[15][1] = "Сохранено !!! ";
   menu.strMenu[15][2] = "              ";
   
   
   menu.strTempScreen[0][0] = " ";
   menu.strTempScreen[0][0] = " ";
   menu.strTempScreen[0][0] = " ";
   

   menu.strMode[0] = "Ручн";
   menu.strMode[1] = "Лето";
   menu.strMode[2] = "Вент";
   menu.strMode[3] = "Зима";

   // Подпись тумблера
   menu.strMenu[14][0] = " СТОП ";
   menu.strMenu[14][1] = " ПУСК ";
   menu.strMenu[14][2] = " АВТО ";

   menu.strUnitsOfMeasure[0] = "   ";
   menu.strUnitsOfMeasure[1] = "'  ";
   menu.strUnitsOfMeasure[2] = "%  ";
   menu.strUnitsOfMeasure[3] = "Па ";
   menu.strUnitsOfMeasure[4] = "ppm";

#endif

  return 1;   
}

/**
 * @brief Полная прорисовка мнемосхемы HVAC1
 * @param x   int8_t смещение по оси х
 * @param y   int8_t смещение по оси y
 * @return viod
 */
void DrawHVAC1(int8_t x, int8_t y)
{

	ST7789_DrawImage_u(2+x, 75+y, 6, 11, Left_Water_Hot_6x11);

	ST7789_DrawImage_u(10+x, 70+y, 10, 20, pipe_H_10x20);

	ST7789_DrawImage_u(20+x, 62+y, 45, 36, valve_H_45x36);

	ST7789_DrawImage_u(65+x, 70+y, 10, 20, pipe_H_10x20);

	ST7789_DrawImage_u(75+x, 60+y, 40, 115, heatExchanger_H_40x115);

	ST7789_DrawImage_u(115+x, 70+y, 10, 20, pipe_H_10x20);
	ST7789_DrawImage_u(125+x, 70+y, 10, 20, pipe_H_10x20);
	ST7789_DrawImage_u(135+x, 70+y, 10, 20, pipe_H_10x20);
	ST7789_DrawImage_u(145+x, 70+y, 10, 20, pipe_H_10x20);
	ST7789_DrawImage_u(155+x, 70+y, 10, 20, pipe_H_10x20);
	ST7789_DrawImage_u(165+x, 70+y, 10, 20, pipe_H_10x20);
	ST7789_DrawImage_u(175+x, 70+y, 10, 20, pipe_H_10x20);
	ST7789_DrawImage_u(185+x, 70+y, 10, 20, pipe_H_10x20);
	ST7789_DrawImage_u(195+x, 70+y, 10, 20, pipe_H_10x20);
	ST7789_DrawImage_u(205+x, 70+y, 10, 20, pipe_H_10x20);

	ST7789_DrawImage_u(217+x, 75+y, 6, 11, Right_Water_Cold_6x11);

	ST7789_DrawImage_u(2+x, 147+y, 6, 11, Right_Water_Hot_6x11);

	ST7789_DrawImage_u(10+x, 142+y, 10, 20, pipe_H_10x20);
	ST7789_DrawImage_u(20+x, 142+y, 10, 20, pipe_H_10x20);
	ST7789_DrawImage_u(30+x, 142+y, 10, 20, pipe_H_10x20);
	ST7789_DrawImage_u(40+x, 142+y, 10, 20, pipe_H_10x20);
	ST7789_DrawImage_u(50+x, 142+y, 10, 20, pipe_H_10x20);
	ST7789_DrawImage_u(60+x, 142+y, 10, 20, pipe_H_10x20);
	ST7789_DrawImage_u(65+x, 142+y, 10, 20, pipe_H_10x20);
	ST7789_DrawImage_u(115+x, 142+y, 10, 20, pipe_H_10x20);
	ST7789_DrawImage_u(125+x, 142+y, 10, 20, pipe_H_10x20);

	ST7789_DrawImage_u(135+x, 143+y, 62, 44, Pump_H_62x44);

	ST7789_DrawImage_u(197+x, 156+y, 10, 20, pipe_H_10x20);
	ST7789_DrawImage_u(205+x, 156+y, 10, 20, pipe_H_10x20);

	ST7789_DrawImage_u(217+x, 161+y, 6, 11, Left_Water_Cold_6x11);

	ST7789_print_u(5+x, 27+y, ST7789_BLUE, ST7789_WHITE, 1, &Font_11x18, 1, "Тул");

}

/**
 * @brief Перерисовка динамических элементов мнемосхемы HVAC1
 * @param x   int8_t смещение по оси х
 * @param y   int8_t смещение по оси y
 * @return viod
 */
//================================================================================
void DrawData1(int16_t x, int16_t y)
{
    // 10 элементов схемы
	static picHVAC1 picNew, picOld;  // нужно ли обновлять элемент схемы

	char strTmp[16];  // временное хранение строки

	// GPIOB->BSRR = GPIO_PIN_15;

	// получить статус графических элементов
	picNew = GetState_HVAC1();


    // вывод положения клапанов
   	sprintf(strTmp,"%3i%%", MODBUS_REG(0x10));
   	ST7789_print_u(10+x, 45+y, RGB565(0, 200, 0), ST7789_WHITE, 1, &Font_11x18, 1, strTmp);

   	// термодатчики
   	if(!READ_COIL(REG_POINT_MODBUS_OK * 0x10 +0x00))//if(MODBUS_REG(0x01) == -1001)          // T1
   		ST7789_print_u(120+x, 122+y, ST7789_BLUE, ST7789_WHITE, 1, &Font_11x18, 1, "---  ");
   	else{
   		sprintf(strTmp,"%3i'", MODBUS_REG(0x00));
	    ST7789_print_u(120+x, 122+y, ST7789_BLUE, ST7789_WHITE, 1, &Font_11x18, 1, strTmp);  // T1
    	}

   	if(!READ_COIL(REG_POINT_MODBUS_OK * 0x10 +0x01))//if(MODBUS_REG(0x02) == -1001)
   		ST7789_print_u(120+x, 93+y, ST7789_BLUE, ST7789_WHITE, 1, &Font_11x18, 1, "---  ");   // T2
   	else{
	    sprintf(strTmp,"%3i'", MODBUS_REG(0x01));
	    ST7789_print_u(120+x, 93+y, ST7789_BLUE, ST7789_WHITE, 1, &Font_11x18, 1, strTmp);   // T2
   	    }

   	if(!READ_COIL(REG_POINT_MODBUS_OK * 0x10 +0x02))//if(MODBUS_REG(0x03) == -1001)          // T3
   		ST7789_print_u(21+x, 124+y, ST7789_BLUE, ST7789_WHITE, 1, &Font_11x18, 1, "---  ");
   	else{
   		sprintf(strTmp,"%3i'", MODBUS_REG(0x02));
   		ST7789_print_u(21+x, 124+y, ST7789_BLUE, ST7789_WHITE, 1, &Font_11x18, 1,strTmp);    // T3
   		}

   	if(!READ_COIL(REG_POINT_MODBUS_OK * 0x10 +0x03))//if(MODBUS_REG(0x03) == -1001)          // T4 зона
   		ST7789_print_u(180+x, 108+y, ST7789_BLUE, ST7789_WHITE, 1, &Font_11x18, 1, "---  ");
   	else{
   		sprintf(strTmp,"%3i'", MODBUS_REG(0x03));
   		ST7789_print_u(180+x, 108+y, ST7789_BLUE, ST7789_WHITE, 1, &Font_11x18, 1,strTmp);    // T4
   		}

   	if(!READ_COIL(REG_POINT_MODBUS_OK * 0x10 +0x04))//if(MODBUS_REG(0x04) == -1001)        // T5  улица
   		ST7789_print_u(45+x, 27+y, ST7789_BLUE, ST7789_WHITE, 1, &Font_11x18, 1, "---  ");
   	else{
   		sprintf(strTmp,"%3i'", MODBUS_REG(0x04));
   		ST7789_print_u(45+x, 27+y, ST7789_BLUE, ST7789_WHITE, 1, &Font_11x18, 1,strTmp);    // T5  улица
   		}



// -----------------------------------------------------------------------------------
	// Прорисовка работы насоса M1

	// прорисовка П1 нужна?
//	if(picNew.pic_n1 != picOld.pic_n1)
//	{
		// перерисовать статус M1
		switch (picNew.pic_M1)
		{
		 case On:
		   	ST7789_DrawImage_u(180+x, 135+y, 12, 12, led_on_12x12);
		    break;
		 case Off:
			ST7789_DrawImage_u(180+x, 135+y, 12, 12, led_off_12x12);
			break;
		 case Alarm:
			ST7789_DrawImage_u(180+x, 135+y, 12, 12, led_Alarm_12x12);
			break;
		 default:
			 ST7789_DrawImage_u(180+x, 135+y, 12, 12, led_Alarm_12x12);
		}
//	}

	// прорисовка B1 нужна?
//	if(picNew.pic_B1 != picOld.pic_B1)
//	{
		// перерисовать статус B1
//		switch (picNew.pic_B1)
//		{
//	    	case On:
//	    		ST7789_DrawImage_u(132+x, 48+y, 12, 12, led_on_12x12);
//	    		break;
//	    	case Off:
//	    		ST7789_DrawImage_u(132+x, 48+y, 12, 12, led_off_12x12);
//	    		break;
//	    	case Alarm:
//	    		ST7789_DrawImage_u(132+x, 48+y, 12, 12, led_Alarm_12x12);
//	    		break;
//	    	default:
//	    		ST7789_DrawImage_u(132+x, 48+y, 12, 12, led_on_12x12);
//		}
//	}

//   	if(!(MODBUS_REG(0x0F) & 0x08))
//	{
//		if(MODBUS_REG(0x0F) & 0x01) ST7789_DrawImage_u(132+x, 94+y, 12, 12, led_on_12x12);
//		else ST7789_DrawImage_u(132+x, 94+y, 12, 12, led_off_12x12);
//	}
//   	else
//   	{
//   		// вентилятор П1 в аварии
//   		ST7789_DrawImage_u(132+x, 94+y, 12, 12, led_Alarm_12x12);
//   	}

//   	if(!(MODBUS_REG(0x0F) & 0x04))
//	{
//   		if(MODBUS_REG(0x0F) & 0x02) ST7789_DrawImage_u(132+x, 48+y, 12, 12, led_on_12x12);
//   		else ST7789_DrawImage_u(132+x, 48+y, 12, 12, led_off_12x12);
//	}
//   	else
//   	{
//   		// вентилятор В1 в аварии
//   		ST7789_DrawImage_u(132+x, 48+y, 12, 12, led_Alarm_12x12);
//   	}

// -----------------------------------------------------------------------------------
	// Прорисовка фильтров воздушных
	// прорисовка filter_П1 нужна?
//	if((picNew.pic_Filtr_n1 != picOld.pic_Filtr_n1) || (picNew.pic_Filtr_B1 != picOld.pic_Filtr_B1) )
//	{
//		if((picNew.pic_Filtr_n1 == Alarm) || (picNew.pic_Filtr_B1 == Alarm))
//		{
//			ST7789_DrawImage_u(35+x, 48+y, 14, 46, Filt_Alarm_14x46);
//			ST7789_DrawImage_u(144+x, 48+y, 14, 46, Filt_Alarm_14x46);
//		}
//		else
//		{
//			ST7789_DrawImage_u(35+x, 48+y, 14, 46, filter_14x46);
//			ST7789_DrawImage_u(144+x, 48+y, 14, 46, filter_14x46);
//		}

//	}

// -----------------------------------------------------------------------------------
	// Прорисовка клапанов On/Off
	// прорисовка Yn1 нужна?
//	if(picNew.pic_Yn1 != picOld.pic_Yn1)
//	{

//    	if(picNew.pic_Yn1 == On) ST7789_DrawImage_u(20+x, 48+y, 15, 46, Air_ON_15x46);
//    	else ST7789_DrawImage_u(20+x, 48+y, 15, 46, Air_OFF_15x46);
//	}

	// прорисовка YB1 нужна?
//	if(picNew.pic_YB1 != picOld.pic_YB1)
//	{
//		if(picNew.pic_YB1 == On) ST7789_DrawImage_u(20+x, 94+y, 15, 46, Air_ON_15x46);
//		else ST7789_DrawImage_u(20+x, 94+y, 15, 46, Air_OFF_15x46);
//	}
// -----------------------------------------------------------------------------------
	// Прорисовка водяног клапана Y3
//	if(picNew.pic_Y3 != picOld.pic_Y3)
//	{
		if(picNew.pic_Y1 == Alarm)
		{
			ST7789_DrawImage_u(20+x, 62+y, 45, 36, valve_H_alarm_45x36);
		}
		else
		{
			ST7789_DrawImage_u(20+x, 62+y, 45, 36, valve_H_45x36);
		}
//	}
// -----------------------------------------------------------------------------------
		// Пререрисовка рекуператора
//	if(picNew.pic_Recup_2 != picOld.pic_Recup_2)
//	{
//		if(picNew.pic_Recup_2 != Alarm)
//		{
//			ST7789_DrawImage_u(49+x, 48+y, 49, 92, Recup_2_49x92);
//		}
//		else
//		{
//			ST7789_DrawImage_u(49+x, 48+y, 49, 92, Recup_2_Alarm_49x92);
//		}
//	}

// -----------------------------------------------------------------------------------
		// Пререрисовка воздушного клапана в рекуператоре Y4
//	if(picNew.pic_Y4 != picOld.pic_Y4)
//	{
//		if(picNew.pic_Y4 != Alarm)
//		{
//			ST7789_DrawImage_u(65+x, 48+y, 15, 25, Air_Small_15x25);
//		}
//		else
//		{
//			ST7789_DrawImage_u(65+x, 48+y, 15, 25, Air_Small_Alarm_15x25);
//		}
//	}

// -----------------------------------------------------------------------------------
	// Прорисовка ручн/лето/вент/зима/ 0/1/2/3
	ST7789_print_u(190+x, 30+y, ST7789_BLUE, ST7789_WHITE, 1, &Font_11x18, 1, menu.strMode[MODBUS_REG(0x48)]/* menu.strMenu[(mode_Menu & 0x000000F0) >> 4][MODBUS_REG(0x48)]*/);

	//ST7789_print_u(20+x, 164+y, RGB565(10, 10, 10), RGB565(100, 100, 255), 1, &Font_16x26, 1, " СТОП "/*menu.strMenu[10][MODBUS_REG(0x49)]/* menu.strMenu[(mode_Menu & 0x000000F0) >> 4][MODBUS_REG(0x48)]*/);

	// Прорисовка тумблера ПУСК/СТОП/КАЛЕНДАРЬ
	uint16_t backColor;
	if((g_mode_Menu & 0x0F) == 2)  backColor = RGB565(100,100,255);
	else backColor = RGB565(200,200,255);


	  if((g_mode_Menu & BIT_MODIFY_REG) && ((g_mode_Menu & 0x0F) == 2))
		  ST7789_print_u(25, 187, ST7789_RED, backColor, 1, &Font_11x18, 1, menu.strMenu[14][MODBUS_REG(0x3F)]);
	  else
		  ST7789_print_u(25, 187, RGB565(10, 10, 10), backColor, 1, &Font_11x18, 1, menu.strMenu[14][MODBUS_REG(0x3F)]);



// -----------------------------------------------------------------------------------
	  // Сообщение об ошибке
	  // Прорисовка кнопки "Сброс ошибки"
	  uint16_t backColor_Error;

//		if((g_mode_Menu & 0x0F) == 1)  backColor_Error = RGB565(255,200,200);
//   	    backColor_Error = ST7789_WHITE;


	if(MODBUS_REG(0x1F))
	   ST7789_print_u(90+x, 30+y, ST7789_RED, ST7789_WHITE, 1, &Font_11x18, 1, " Ошибка!");
	else  ST7789_print_u(90+x, 30+y, ST7789_RED, ST7789_WHITE, 1, &Font_11x18, 1, "        ");


	 picOld = picNew; // изменения сделаны



	 // кнопка
	 if((g_mode_Menu & 0x7ff) == 0x101){
	   ST7789_DrawImage_u(115+x, 181+y, 120, 24, Reset_21_120x24);
	   MODBUS_REG(0x47) &= ~0x8000;
	 }

	 else if((g_mode_Menu & 0x7ff) == 0x301){
	   ST7789_DrawImage_u(115+x, 181+y, 120, 24, Reset_31_120x24);
	   MODBUS_REG(0x47) |= 0x8000;
	 }

	 else{
		 ST7789_DrawImage_u(115+x, 181+y, 120, 24, Reset_11_120x24);
		 MODBUS_REG(0x47) &= ~0x8000;
	 }
//	// кнопка
//	if((g_mode_Menu & 0x0F) == 1){
//	   	ST7789_DrawImage_u(185+0, 15+15, 49, 30, Button_3_49x30);
//	 }
//	else if((g_mode_Menu & 0x0F) == 2){
//	   	ST7789_DrawImage_u(185+0, 15+15, 49, 30, Button_3_49x30);
//	 }
//	 else  ST7789_DrawImage_u(185+0, 15+15, 49, 30, Button_1_49x30);

//	  GPIOB->BSRR = (uint32_t)GPIO_PIN_15 << 16U;

}

//================================================================================

//================================================================================
/**
 * @brief Полная прорисовка мнемосхемы HVAC2
 * @param x   int8_t смещение по оси х
 * @param y   int8_t смещение по оси y
 * @return viod
 */
void DrawHVAC2(int8_t x, int8_t y)
{

	ST7789_DrawImage_u(5+x, 71+y, 7, 26, Right_7x26);
	ST7789_DrawImage_u(20+x, 63+y, 15, 46, Air_Valve_V_15x46);
	ST7789_DrawImage_u(35+x, 63+y, 14, 46, filter_14x46);
	ST7789_DrawImage_u(49+x, 63+y, 49, 92, Recup_2_49x92);
	ST7789_DrawImage_u(98+x, 63+y, 46, 46, fan1_46x46);
	ST7789_DrawImage_u(132+x, 63+y, 12, 12, led_off_12x12);
	ST7789_DrawImage_u(144+x, 63+y, 14, 46, filter_14x46);
	ST7789_DrawImage_u(158+x, 63+y, 6, 46, Pipe_Air_1_6x46);
	ST7789_DrawImage_u(164+x, 63+y, 6, 46, Pipe_Air_1_6x46);
	ST7789_DrawImage_u(170+x, 63+y, 6, 46, Pipe_Air_1_6x46);
	ST7789_DrawImage_u(176+x, 63+y, 6, 46, Pipe_Air_1_6x46);
	ST7789_DrawImage_u(182+x, 63+y, 6, 46, Pipe_Air_1_6x46);
	ST7789_DrawImage_u(188+x, 63+y, 6, 46, Pipe_Air_1_6x46);
	ST7789_DrawImage_u(194+x, 63+y, 6, 46, Pipe_Air_1_6x46);
	ST7789_DrawImage_u(200+x, 63+y, 6, 46, Pipe_Air_1_6x46);
	ST7789_DrawImage_u(206+x, 63+y, 6, 46, Pipe_Air_1_6x46);
	ST7789_DrawImage_u(212+x, 63+y, 6, 46, Pipe_Air_1_6x46);
	ST7789_DrawImage_u(218+x, 63+y, 6, 46, Pipe_Air_1_6x46);
	ST7789_DrawImage_u(221+x, 63+y, 6, 46, Pipe_Air_1_6x46);

	ST7789_DrawImage_u(230+x, 71+y, 7, 26, Left_7x26);

//	ST7789_DrawImage_u(180, 160, 12, 12, led_off_12x12); //

	ST7789_DrawImage_u(5+x, 117+y, 7, 26, Left_7x26);//
	ST7789_DrawImage_u(20+x, 109+y, 15, 46, Air_Valve_V_15x46);
	ST7789_DrawImage_u(35+x, 109+y, 6, 46, Pipe_Air_1_6x46);
	ST7789_DrawImage_u(41+x, 109+y, 6, 46, Pipe_Air_1_6x46);
	ST7789_DrawImage_u(43+x, 109+y, 6, 46, Pipe_Air_1_6x46);
	ST7789_DrawImage_u(98+x, 109+y, 46, 46, fan1_46x46);
	ST7789_DrawImage_u(132+x, 109+y, 12, 12, led_off_12x12);
	ST7789_DrawImage_u(144+x, 109+y, 23, 46, Heater_23x46_color);

	ST7789_DrawImage_u(167+x, 109+y, 6, 46, Pipe_Air_1_6x46);
	ST7789_DrawImage_u(170+x, 109+y, 6, 46, Pipe_Air_1_6x46);
	ST7789_DrawImage_u(176+x, 109+y, 6, 46, Pipe_Air_1_6x46);
	ST7789_DrawImage_u(182+x, 109+y, 6, 46, Pipe_Air_1_6x46);
	ST7789_DrawImage_u(188+x, 109+y, 6, 46, Pipe_Air_1_6x46);
	ST7789_DrawImage_u(194+x, 109+y, 6, 46, Pipe_Air_1_6x46);
	ST7789_DrawImage_u(200+x, 109+y, 6, 46, Pipe_Air_1_6x46);
	ST7789_DrawImage_u(206+x, 109+y, 6, 46, Pipe_Air_1_6x46);
	ST7789_DrawImage_u(212+x, 109+y, 6, 46, Pipe_Air_1_6x46);
	ST7789_DrawImage_u(218+x, 109+y, 6, 46, Pipe_Air_1_6x46);
	ST7789_DrawImage_u(221+x, 109+y, 6, 46, Pipe_Air_1_6x46);

	ST7789_DrawImage_u(230+x, 117+y, 7, 26, Right_7x26);

	ST7789_DrawImage_u(144+x, 155+y, 22, 21, WaterPipe_R_22x21);
	ST7789_DrawImage_u(166+x, 158+y, 19, 19, WaterValve_R_19x19);

	ST7789_print_u(5+x, 27+y, ST7789_BLUE, ST7789_WHITE, 1, &Font_11x18, 1, "Тул  ");
}

//=========================================================================
/**
 * @brief Перерисовка динамических элементов мнемосхемы HVAC2
 * @param x   int8_t смещение по оси х
 * @param y   int8_t смещение по оси y
 * @return viod
 */
void DrawData2(int16_t x, int16_t y)
{
    // 10 элементов схемы
	static picHVAC2 picNew, picOld;  // нужно ли обновлять элемент схемы

	char strTmp[16];  // временное хранение строки

//	 GPIOB->BSRR = GPIO_PIN_15;

	// получить статус графических элементов
	picNew = GetState_HVAC2();



   	sprintf(strTmp,"%3i%%", MODBUS_REG(0x10));
	ST7789_print_u(190+x, 158+y, RGB565(0, 200, 0), ST7789_WHITE, 1, &Font_11x18, 1,strTmp);
   	sprintf(strTmp,"%3i%%", MODBUS_REG(0x11));
   	ST7789_print_u(50+x, 45+y, RGB565(0, 200, 0), ST7789_WHITE, 1, &Font_11x18, 1, strTmp);

   	if(!READ_COIL(REG_POINT_MODBUS_OK * 0x10 +0x01))//if(MODBUS_REG(0x01) == -1001)
   		ST7789_print_u(170+x, 123+y, ST7789_BLUE, ST7789_WHITE, 1, &Font_11x18, 1, "---  ");
   	else{
   		sprintf(strTmp,"%3i'", MODBUS_REG(0x01));
	    ST7789_print_u(170+x, 123+y, ST7789_BLUE, ST7789_WHITE, 1, &Font_11x18, 1, strTmp);
    	}

   	if(!READ_COIL(REG_POINT_MODBUS_OK * 0x10 +0x02))//if(MODBUS_REG(0x02) == -1001)
   		ST7789_print_u(170+x, 77+y, ST7789_BLUE, ST7789_WHITE, 1, &Font_11x18, 1, "---  ");
   	else{
	    sprintf(strTmp,"%3i'", MODBUS_REG(0x02));
	    ST7789_print_u(170+x, 77+y, ST7789_BLUE, ST7789_WHITE, 1, &Font_11x18, 1, strTmp);
   	    }

   	if(!READ_COIL(REG_POINT_MODBUS_OK * 0x10 +0x03))//if(MODBUS_REG(0x03) == -1001)
   		ST7789_print_u(21+x, 158+y, ST7789_BLUE, ST7789_WHITE, 1, &Font_11x18, 1, "---  ");
   	else{
   		sprintf(strTmp,"%3i'", MODBUS_REG(0x03));
   		ST7789_print_u(21+x, 158+y, ST7789_BLUE, ST7789_WHITE, 1, &Font_11x18, 1,strTmp);
   		}

   	if(!READ_COIL(REG_POINT_MODBUS_OK * 0x10 +0x00))//if(MODBUS_REG(0x00) == -1001)
   		ST7789_print_u(66+x, 158+y, ST7789_BLUE, ST7789_WHITE, 1, &Font_11x18, 1, "---  ");
   	else{
   		sprintf(strTmp,"%3i'", MODBUS_REG(0x00));
   		ST7789_print_u(66+x, 158+y, ST7789_BLUE, ST7789_WHITE, 1, &Font_11x18, 1,strTmp);
   		}

   	if(!READ_COIL(REG_POINT_MODBUS_OK * 0x10 +0x04))//if(MODBUS_REG(0x04) == -1001)
   		ST7789_print_u(45+x, 27+y, ST7789_BLUE, ST7789_WHITE, 1, &Font_11x18, 1, "---  ");
   	else{
   		sprintf(strTmp,"%3i'", MODBUS_REG(0x04));
   		ST7789_print_u(45+x, 27+y, ST7789_BLUE, ST7789_WHITE, 1, &Font_11x18, 1,strTmp);
   		}



// -----------------------------------------------------------------------------------
	// Прорисовка работы вентилятора
	// прорисовка П1 нужна?
//	if(picNew.pic_n1 != picOld.pic_n1)
//	{
		// перерисовать статус П1
		switch (picNew.pic_n1)
		{
		 case On:
		   	ST7789_DrawImage_u(132+x, 109+y, 12, 12, led_on_12x12);
		    break;
		 case Off:
			ST7789_DrawImage_u(132+x, 109+y, 12, 12, led_off_12x12);
			break;
		 case Alarm:
			ST7789_DrawImage_u(132+x, 109+y, 12, 12, led_Alarm_12x12);
			break;
		 default:
			 ST7789_DrawImage_u(132+x, 109+y, 12, 12, led_Alarm_12x12);
		}
//	}

	// прорисовка B1 нужна?
//	if(picNew.pic_B1 != picOld.pic_B1)
//	{
		// перерисовать статус B1
		switch (picNew.pic_B1)
		{
	    	case On:
	    		ST7789_DrawImage_u(132+x, 63+y, 12, 12, led_on_12x12);
	    		break;
	    	case Off:
	    		ST7789_DrawImage_u(132+x, 63+y, 12, 12, led_off_12x12);
	    		break;
	    	case Alarm:
	    		ST7789_DrawImage_u(132+x, 63+y, 12, 12, led_Alarm_12x12);
	    		break;
	    	default:
	    		ST7789_DrawImage_u(132+x, 63+y, 12, 12, led_on_12x12);
		}
//	}

//   	if(!(MODBUS_REG(0x0F) & 0x08))
//	{
//		if(MODBUS_REG(0x0F) & 0x01) ST7789_DrawImage_u(132+x, 94+y, 12, 12, led_on_12x12);
//		else ST7789_DrawImage_u(132+x, 94+y, 12, 12, led_off_12x12);
//	}
//   	else
//   	{
//   		// вентилятор П1 в аварии
//   		ST7789_DrawImage_u(132+x, 94+y, 12, 12, led_Alarm_12x12);
//   	}

//   	if(!(MODBUS_REG(0x0F) & 0x04))
//	{
//   		if(MODBUS_REG(0x0F) & 0x02) ST7789_DrawImage_u(132+x, 48+y, 12, 12, led_on_12x12);
//   		else ST7789_DrawImage_u(132+x, 48+y, 12, 12, led_off_12x12);
//	}
//   	else
//   	{
//   		// вентилятор В1 в аварии
//   		ST7789_DrawImage_u(132+x, 48+y, 12, 12, led_Alarm_12x12);
//   	}

// -----------------------------------------------------------------------------------
	// Прорисовка фильтров воздушных
	// прорисовка filter_П1 нужна?
//	if((picNew.pic_Filtr_n1 != picOld.pic_Filtr_n1) || (picNew.pic_Filtr_B1 != picOld.pic_Filtr_B1) )
//	{
		if((picNew.pic_Filtr_n1 == Alarm) || (picNew.pic_Filtr_B1 == Alarm))
		{
			ST7789_DrawImage_u(35+x, 63+y, 14, 46, Filt_Alarm_14x46);
			ST7789_DrawImage_u(144+x, 63+y, 14, 46, Filt_Alarm_14x46);
		}
		else
		{
			ST7789_DrawImage_u(35+x, 63+y, 14, 46, filter_14x46);
			ST7789_DrawImage_u(144+x, 63+y, 14, 46, filter_14x46);
		}

//	}

// -----------------------------------------------------------------------------------
	// Прорисовка клапанов On/Off
	// прорисовка Yn1 нужна?
//	if(picNew.pic_Yn1 != picOld.pic_Yn1)
//	{

    	if(picNew.pic_Yn1 == On) ST7789_DrawImage_u(20+x, 63+y, 15, 46, Air_ON_15x46);
    	else ST7789_DrawImage_u(20+x, 63+y, 15, 46, Air_OFF_15x46);
//	}

	// прорисовка YB1 нужна?
//	if(picNew.pic_YB1 != picOld.pic_YB1)
//	{
		if(picNew.pic_YB1 == On) ST7789_DrawImage_u(20+x, 109+y, 15, 46, Air_ON_15x46);
		else ST7789_DrawImage_u(20+x, 109+y, 15, 46, Air_OFF_15x46);
//	}
// -----------------------------------------------------------------------------------
	// Прорисовка водяног клапана Y3
//	if(picNew.pic_Y3 != picOld.pic_Y3)
//	{
		if(picNew.pic_Y3 == Alarm)
		{
			ST7789_DrawImage_u(166+x, 158+y, 19, 19, WaterValve_Alarm_R_19x19);
		}
		else
		{
			ST7789_DrawImage_u(166+x, 158+y, 19, 19, WaterValve_R_19x19);
		}
//	}
// -----------------------------------------------------------------------------------
		// Пререрисовка рекуператора
//	if(picNew.pic_Recup_2 != picOld.pic_Recup_2)
//	{
		if(picNew.pic_Recup_2 != Alarm)
		{
			ST7789_DrawImage_u(49+x, 63+y, 49, 92, Recup_2_49x92);
		}
		else
		{
			ST7789_DrawImage_u(49+x, 63+y, 49, 92, Recup_2_Alarm_49x92);
		}
//	}

// -----------------------------------------------------------------------------------
		// Пререрисовка воздушного клапана в рекуператоре Y4
//	if(picNew.pic_Y4 != picOld.pic_Y4)
//	{
		if(picNew.pic_Y4 != Alarm)
		{
			ST7789_DrawImage_u(65+x, 63+y, 15, 25, Air_Small_15x25);
		}
		else
		{
			ST7789_DrawImage_u(65+x, 63+y, 15, 25, Air_Small_Alarm_15x25);
		}
//	}

// -----------------------------------------------------------------------------------
	// Прорисовка ручн/лето/вент/зима/ 0/1/2/3
	ST7789_print_u(190+x, 30+y, ST7789_BLUE, ST7789_WHITE, 1, &Font_11x18, 1, menu.strMode[MODBUS_REG(0x48)]/* menu.strMenu[(mode_Menu & 0x000000F0) >> 4][MODBUS_REG(0x48)]*/);

	//ST7789_print_u(20+x, 164+y, RGB565(10, 10, 10), RGB565(100, 100, 255), 1, &Font_16x26, 1, " СТОП "/*menu.strMenu[10][MODBUS_REG(0x49)]/* menu.strMenu[(mode_Menu & 0x000000F0) >> 4][MODBUS_REG(0x48)]*/);

	// Прорисовка тумблера ПУСК/СТОП/КАЛЕНДАРЬ
	uint16_t backColor;
	if((g_mode_Menu & 0x0F) == 2)  backColor = RGB565(100,100,255);
	else backColor = RGB565(200,200,255);


	  if((g_mode_Menu & BIT_MODIFY_REG) && ((g_mode_Menu & 0x0F) == 2))
		  ST7789_print_u(25, 187, ST7789_RED, backColor, 1, &Font_11x18, 1, menu.strMenu[14][MODBUS_REG(0x3F)]);
	  else
		  ST7789_print_u(25, 187, RGB565(10, 10, 10), backColor, 1, &Font_11x18, 1, menu.strMenu[14][MODBUS_REG(0x3F)]);



// -----------------------------------------------------------------------------------
	  // Сообщение об ошибке
	  // Прорисовка кнопки "Сброс ошибки"
	  uint16_t backColor_Error;

//		if((g_mode_Menu & 0x0F) == 1)  backColor_Error = RGB565(255,200,200);
//   	    backColor_Error = ST7789_WHITE;


	if(MODBUS_REG(0x1F))
	   ST7789_print_u(90+x, 30+y, ST7789_RED, ST7789_WHITE, 1, &Font_11x18, 1, " Ошибка!");
	else  ST7789_print_u(90+x, 30+y, ST7789_RED, ST7789_WHITE, 1, &Font_11x18, 1, "        ");


	 picOld = picNew; // изменения сделаны



	 // кнопка
	 if((g_mode_Menu & 0x7ff) == 0x101){
	   ST7789_DrawImage_u(115+x, 181+y, 120, 24, Reset_21_120x24);
	   MODBUS_REG(0x47) &= ~0x8000;
	 }

	 else if((g_mode_Menu & 0x7ff) == 0x301){
	   ST7789_DrawImage_u(115+x, 181+y, 120, 24, Reset_31_120x24);
	   MODBUS_REG(0x47) |= 0x8000;
	 }

	 else{
		 ST7789_DrawImage_u(115+x, 181+y, 120, 24, Reset_11_120x24);
		 MODBUS_REG(0x47) &= ~0x8000;
	 }

//	  GPIOB->BSRR = (uint32_t)GPIO_PIN_15 << 16U;

}

//=====================================================================
/**
 * @brief Получить текущее состояние элементов системы HVAC1
 * @return picHVAC1
 * @note Функция возвращает структцру picHVAC1 описывающую состояние системы HVAC1
 * 		 исходя из сигналов обратной связи и показпний датчиков
 */
picHVAC1 GetState_HVAC1()
{
	picHVAC1 Tmp;

	// насос М1
	if(MODBUS_REG(0x0f) & 0x01) Tmp.pic_M1 = On;
	else Tmp.pic_M1 = Off;
	if(MODBUS_REG(0x1f) & 0x04) Tmp.pic_M1 = Alarm;

	// фильтры
//	if(MODBUS_REG(0x0f) & 0x04)
//	{
//		Tmp.pic_Filtr_n1 = Alarm; Tmp.pic_Filtr_B1 = Alarm;
//	}
//	else
//	{
//		Tmp.pic_Filtr_n1 = Norm; Tmp.pic_Filtr_B1 = Norm;
//	}

	// рекуператоры
//	if(MODBUS_REG(0x0f) & 0x08) Tmp.pic_Recup_2 = Norm;
//	else Tmp.pic_Recup_2 = Alarm;
//
//	// клапаны On/Off
//	if(MODBUS_REG(0x2F) & 0x01) Tmp.pic_Yn1 = On;
//	else Tmp.pic_Yn1 = Off;
//
//	if(MODBUS_REG(0x2F) & 0x02) Tmp.pic_YB1 = On;
//	else Tmp.pic_YB1 = Off;

	// клапанн Y1
	if(MODBUS_REG(0x1f) & 0x08) Tmp.pic_Y1 = Alarm;
	else Tmp.pic_Y1 = Norm;

//	// клапанн Y4
//	if(MODBUS_REG(0x1f) & 0x40) Tmp.pic_Y1 = Alarm;
//	else Tmp.pic_Y1 = Norm;

return Tmp;
}

//==================================================================


//==================================================================
/**
 * @brief Получить текущее состояние элементов системы HVAC2
 * @return picHVAC2
 * @note Функция возвращает структцру picHVAC2 описывающую состояние системы HVAC2
 * 		 исходя из сигналов обратной связи и показпний датчиков
 */
picHVAC2 GetState_HVAC2()
{
	picHVAC2 Tmp;

	// вентиляторы
	if(MODBUS_REG(0x0f) & 0x01) Tmp.pic_n1 = On;
	else Tmp.pic_n1 = Off;
	if(MODBUS_REG(0x1f) & 0x02) Tmp.pic_n1 = Alarm;

	if(MODBUS_REG(0x0f) & 0x02) Tmp.pic_B1 = On;
	else Tmp.pic_B1 = Off;
	if(MODBUS_REG(0x1f) & 0x04) Tmp.pic_B1 = Alarm;


	// фильтры
	if(MODBUS_REG(0x0f) & 0x04)
	{
		Tmp.pic_Filtr_n1 = Alarm; Tmp.pic_Filtr_B1 = Alarm;
	}
	else
	{
		Tmp.pic_Filtr_n1 = Norm; Tmp.pic_Filtr_B1 = Norm;
	}

	// рекуператоры
	if(MODBUS_REG(0x0f) & 0x08) Tmp.pic_Recup_2 = Norm;
	else Tmp.pic_Recup_2 = Alarm;

	// клапаны On/Off
	if(MODBUS_REG(0x2F) & 0x01) Tmp.pic_Yn1 = On;
	else Tmp.pic_Yn1 = Off;

	if(MODBUS_REG(0x2F) & 0x02) Tmp.pic_YB1 = On;
	else Tmp.pic_YB1 = Off;

	// клапанн Y3
	if(MODBUS_REG(0x1f) & 0x20) Tmp.pic_Y3 = Alarm;
	else Tmp.pic_Y3 = Norm;

	// клапанн Y4
	if(MODBUS_REG(0x1f) & 0x40) Tmp.pic_Y4 = Alarm;
	else Tmp.pic_Y4 = Norm;

return Tmp;
}

//==================================================================

//==================================================================
/**
 * @brief Получить текущее состояние элементов системы HVAC3
 * @return picHVAC3
 * @note Функция возвращает структцру picHVAC3 описывающую состояние системы HVAC3
 * 		 исходя из сигналов обратной связи и показпний датчиков
 */
picHVAC3 GetState_HVAC3()
{
	picHVAC3 Tmp;

	// вентиляторы
	if(MODBUS_REG(0x0f) & 0x01) Tmp.pic_n1 = On;
	else Tmp.pic_n1 = Off;
	if(MODBUS_REG(0x1f) & 0x02) Tmp.pic_n1 = Alarm;

//	if(MODBUS_REG(0x0f) & 0x02) Tmp.pic_B1 = On;
//	else Tmp.pic_B1 = Off;
//	if(MODBUS_REG(0x1f) & 0x04) Tmp.pic_B1 = Alarm;


	// фильтры
	if(MODBUS_REG(0x0f) & 0x04)
	{
		Tmp.pic_Filtr_n1 = Alarm;
	}
	else
	{
		Tmp.pic_Filtr_n1 = Norm;
	}

	// рекуператоры
//	if(MODBUS_REG(0x0f) & 0x08) Tmp.pic_Recup_2 = Norm;
//	else Tmp.pic_Recup_2 = Alarm;

	// клапаны On/Off
//	if(MODBUS_REG(0x2F) & 0x01) Tmp.pic_Yn1 = On;
//	else Tmp.pic_Yn1 = Off;

//	if(MODBUS_REG(0x2F) & 0x02) Tmp.pic_YB1 = On;
//	else Tmp.pic_YB1 = Off;

	// клапанн Y3
	if(MODBUS_REG(0x1f) & 0x20) Tmp.pic_Y3 = Alarm;
	else Tmp.pic_Y3 = Norm;

	// клапанн Y4
	if(MODBUS_REG(0x1f) & 0x40) Tmp.pic_Y1 = Alarm;
	else Tmp.pic_Y1 = Norm;

return Tmp;
}

//==================================================================

//==================================================================
/**
 * @brief Получить текущее состояние элементов системы HVAC4
 * @return picHVAC4
 * @note Функция возвращает структцру picHVAC4 описывающую состояние системы HVAC4
 * 		 исходя из сигналов обратной связи и показпний датчиков
 */
picHVAC4 GetState_HVAC4()
{
	picHVAC4 Tmp;

	// вентиляторы
	if(MODBUS_REG(0x0f) & 0x01) Tmp.pic_n1 = On;
	else Tmp.pic_n1 = Off;
	if(MODBUS_REG(0x1f) & 0x02) Tmp.pic_n1 = Alarm;

	// ТЭН
	if(MODBUS_REG(0x0f) & 0x02) Tmp.pic_TEN = On;
	else Tmp.pic_TEN = Off;
	if(MODBUS_REG(0x1f) & 0x04) Tmp.pic_TEN = Alarm;


	// фильтры
	if(MODBUS_REG(0x0f) & 0x04)
	{
		Tmp.pic_Filtr_n1 = Alarm;
	}
	else
	{
		Tmp.pic_Filtr_n1 = Norm;
	}

	// рекуператоры
//	if(MODBUS_REG(0x0f) & 0x08) Tmp.pic_Recup_2 = Norm;
//	else Tmp.pic_Recup_2 = Alarm;

	// клапаны On/Off
//	if(MODBUS_REG(0x2F) & 0x01) Tmp.pic_Yn1 = On;
//	else Tmp.pic_Yn1 = Off;
//
//	if(MODBUS_REG(0x2F) & 0x02) Tmp.pic_YB1 = On;
//	else Tmp.pic_YB1 = Off;

	// клапанн Y3
	if(MODBUS_REG(0x1f) & 0x20) Tmp.pic_Y3 = Alarm;
	else Tmp.pic_Y3 = Norm;

	// клапанн Y4
	if(MODBUS_REG(0x1f) & 0x40) Tmp.pic_Y1 = Alarm;
	else Tmp.pic_Y1 = Norm;

return Tmp;
}

//==================================================================

//==================================================================
/**
 * @brief Получить текущее состояние элементов системы HVAC5
 * @return picHVAC5
 * @note Функция возвращает структцру picHVAC5 описывающую состояние системы HVAC5
 * 		 исходя из сигналов обратной связи и показпний датчиков
 */
picHVAC5 GetState_HVAC5()
{
	picHVAC5 Tmp;

	// вентиляторы
	if(MODBUS_REG(0x0f) & 0x01) Tmp.pic_n1 = On;
	else Tmp.pic_n1 = Off;
	if(MODBUS_REG(0x1f) & 0x02) Tmp.pic_n1 = Alarm;

	if(MODBUS_REG(0x0f) & 0x02) Tmp.pic_B1 = On;
	else Tmp.pic_B1 = Off;
	if(MODBUS_REG(0x1f) & 0x04) Tmp.pic_B1 = Alarm;


	// фильтры
	if(MODBUS_REG(0x0f) & 0x04)
	{
		Tmp.pic_Filtr_n1 = Alarm; Tmp.pic_Filtr_B1 = Alarm;
	}
	else
	{
		Tmp.pic_Filtr_n1 = Norm; Tmp.pic_Filtr_B1 = Norm;
	}

	// рекуператоры
	if(MODBUS_REG(0x0f) & 0x08) Tmp.pic_Recup_2 = Norm;
	else Tmp.pic_Recup_2 = Alarm;

	// это контактор ТЭНа
	if(MODBUS_REG(0x2F) & 0x01) Tmp.pic_TEN = On;
	else Tmp.pic_TEN = Off;

	// клапаны On/Off
	// DI4
	if(MODBUS_REG(0x0F) & 0x10) Tmp.pic_Yn1 = On;
	else Tmp.pic_Yn1 = Off;

	// DO1
	if(MODBUS_REG(0x2F) & 0x02) Tmp.pic_YB1 = On;
	else Tmp.pic_YB1 = Off;

	// клапан Y3
//	if(MODBUS_REG(0x1f) & 0x20) Tmp.pic_Y3 = Alarm;
//	else Tmp.pic_Y3 = Norm;

	// клапанн Y4
	if(MODBUS_REG(0x1f) & 0x40) Tmp.pic_Y4 = Alarm;
	else Tmp.pic_Y4 = Norm;

return Tmp;
}

//==================================================================


//================================================================================
/**
 * @brief Полная прорисовка мнемосхемы HVAC3
 * @param x   int8_t смещение по оси х
 * @param y   int8_t смещение по оси y
 * @return viod
 */
void DrawHVAC3(int8_t x, int8_t y)
{

	ST7789_DrawImage_u(5+x, 111+y, 7, 26, Right_7x26); //
	ST7789_DrawImage_u(20+x, 103+y, 15, 46, Air_Valve_Alarm_15x46);
	ST7789_DrawImage_u(35+x, 103+y, 14, 46, filter_14x46);
	ST7789_FillRect_u(49+x, 148+y, 35, 1, RGB565(0,0,0));

	ST7789_DrawImage_u(53+x, 79+y, 26, 7, Down_26x7); // Down

	ST7789_DrawImage_u(49+x, 88+y, 35, 15, Air_Valve_H_35x15);

	ST7789_DrawImage_u(84+x, 103+y, 46, 46, fan1_46x46);

	ST7789_DrawImage_u(130+x, 103+y, 23, 46, Heater_23x46_color);

	ST7789_DrawImage_u(132+x, 149+y, 22, 21, WaterPipe_R_22x21);
	ST7789_DrawImage_u(154+x, 151+y, 19, 19, WaterValve_R_19x19);

	ST7789_DrawImage_u(153+x, 103+y, 6, 46, Pipe_Air_1_6x46);
	ST7789_DrawImage_u(159+x, 103+y, 6, 46, Pipe_Air_1_6x46);
	ST7789_DrawImage_u(165+x, 103+y, 6, 46, Pipe_Air_1_6x46);
	ST7789_DrawImage_u(171+x, 103+y, 6, 46, Pipe_Air_1_6x46);
	ST7789_DrawImage_u(177+x, 103+y, 6, 46, Pipe_Air_1_6x46);
	ST7789_DrawImage_u(183+x, 103+y, 6, 46, Pipe_Air_1_6x46);
	ST7789_DrawImage_u(189+x, 103+y, 6, 46, Pipe_Air_1_6x46);
	ST7789_DrawImage_u(195+x, 103+y, 6, 46, Pipe_Air_1_6x46);
	ST7789_DrawImage_u(201+x, 103+y, 6, 46, Pipe_Air_1_6x46);
	ST7789_DrawImage_u(207+x, 103+y, 6, 46, Pipe_Air_1_6x46);
	ST7789_DrawImage_u(213+x, 103+y, 6, 46, Pipe_Air_1_6x46);
	ST7789_DrawImage_u(219+x, 103+y, 6, 46, Pipe_Air_1_6x46);
	ST7789_DrawImage_u(230+x, 111+y, 7, 26, Right_7x26);

	ST7789_print_u(5+x, 27+y, ST7789_BLUE, ST7789_WHITE, 1, &Font_11x18, 1, "Тул");


}


//==================================================================
/**
 * @brief Перерисовка динамических элементов мнемосхемы HVAC3
 * @param x   int8_t смещение по оси х
 * @param y   int8_t смещение по оси y
 * @return viod
 */
void DrawData3(int16_t x, int16_t y)
{
    // 10 элементов схемы
	static picHVAC3 picNew, picOld;  // нужно ли обновлять элемент схемы

	char strTmp[16];  // временное хранение строки

//	 GPIOB->BSRR = GPIO_PIN_15;

	// получить статус графических элементов
	picNew = GetState_HVAC3();

    // цвета индикаторов температуры
	uint16_t color_T, color_CO2;

	// температура
	if(READ_COIL(COIL_T_LOW))  {color_T = COLOR_T_LOW;}
	else if(READ_COIL(COIL_T_NORM)){color_T = COLOR_T_NORM;}
	else if(READ_COIL(COIL_T_HIGHT)){color_T = COLOR_T_HIGH;}
	else {
		color_T = ST7789_WHITE;
	}

	// CO2
	if(READ_COIL(COIL_CO2_NORM))  {color_CO2 = COLOR_CO2_NORM;}
	else if(READ_COIL(COIL_CO2_HIGHT)){color_CO2 = COLOR_T_HIGH;}
	else {
		color_CO2 = ST7789_WHITE;
	}
    // вывод положения клапанов
   	sprintf(strTmp,"%3i%%", MODBUS_REG(0x10));
	ST7789_print_u(180+x, 152+y, RGB565(0, 200, 0), ST7789_WHITE, 1, &Font_11x18, 1,strTmp);
   	sprintf(strTmp,"%3i%%", MODBUS_REG(0x11));
   	ST7789_print_u(2+x, 85+y, RGB565(0, 200, 0), ST7789_WHITE, 1, &Font_11x18, 1, strTmp);

   	// термодатчики

   	if(!READ_COIL(REG_POINT_MODBUS_OK * 0x10 +0x00))//if(MODBUS_REG(0x03) == -1001)          Т смеси
   		ST7789_print_u(43+x, 152+y, ST7789_BLUE, ST7789_WHITE, 1, &Font_11x18, 1, "---  ");
   	else{
   		sprintf(strTmp,"%3i'", MODBUS_REG(0x00));
   		ST7789_print_u(43+x, 152+y, ST7789_BLUE, ST7789_WHITE, 1, &Font_11x18, 1,strTmp);    // Т смеси 0х00
   		}


   	if(!READ_COIL(REG_POINT_MODBUS_OK * 0x10 +0x01))//if(MODBUS_REG(0x01) == -1001)           Т подачи
   		ST7789_print_u(175+x, 117+y, ST7789_BLUE, ST7789_WHITE, 1, &Font_11x18, 1, "---  ");
   	else{
   		sprintf(strTmp,"%3i'", MODBUS_REG(0x01));
	    ST7789_print_u(175+x, 117+y, ST7789_BLUE, ST7789_WHITE, 1, &Font_11x18, 1, strTmp);  // Т подачи 0х01
    	}


   	if(!READ_COIL(REG_POINT_MODBUS_OK * 0x10 +0x02))//if(MODBUS_REG(0x02) == -1001)            Т зоны
   		ST7789_print_u(120+x, 60+y, ST7789_BLUE, ST7789_WHITE, 1, &Font_11x18, 1, "---   ");
   	else{
	    sprintf(strTmp,"%3i'", MODBUS_REG(0x02));
	    ST7789_print_u(120+x, 60+y, ST7789_BLUE, ST7789_WHITE, 1, &Font_11x18, 1, strTmp);   // Т зоны 0х02
	    ST7789_print_u(120+(5*11)+x, 60+y, ST7789_WHITE,color_T, 1, &Font_11x18, 1, " ");   // Т зоны 0х02
   	    }

   	if(!READ_COIL(REG_POINT_MODBUS_OK * 0x10 +0x03))//if(MODBUS_REG(0x02) == -1001)            CO2 зоны
   		ST7789_print_u(120+x, 80+y, ST7789_BLUE, ST7789_WHITE, 1, &Font_11x18, 1, "---      ");
   	else{
	    sprintf(strTmp,"%3ippm", MODBUS_REG(0x03));
	    ST7789_print_u(120+x, 80+y, ST7789_BLUE, ST7789_WHITE, 1, &Font_11x18, 1, strTmp);   // CO2 0х03
	    ST7789_print_u(120+(8*11)+x, 80+y, ST7789_WHITE, color_CO2, 1, &Font_11x18, 1, " ");
   	    }



   	if(!READ_COIL(REG_POINT_MODBUS_OK * 0x10 +0x04))//if(MODBUS_REG(0x04) == -1001)         Т ул 0х04
   		ST7789_print_u(45+x, 27+y, ST7789_BLUE, ST7789_WHITE, 1, &Font_11x18, 1, "---  ");
   	else{
   		sprintf(strTmp,"%3i'", MODBUS_REG(0x04));
   		ST7789_print_u(45+x, 27+y, ST7789_BLUE, ST7789_WHITE, 1, &Font_11x18, 1,strTmp);    // Т ул 0х04
   		}



// -----------------------------------------------------------------------------------
	// Прорисовка работы вентилятора П1
	// прорисовка П1 нужна?
//	if(picNew.pic_n1 != picOld.pic_n1)
//	{
		// перерисовать статус П1
		switch (picNew.pic_n1)
		{
		 case On:
		   	ST7789_DrawImage_u(118+x, 104+y, 12, 12, led_on_12x12);
		    break;
		 case Off:
			ST7789_DrawImage_u(118+x, 104+y, 12, 12, led_off_12x12);
			break;
		 case Alarm:
			ST7789_DrawImage_u(118+x, 104+y, 12, 12, led_Alarm_12x12);
			break;
		 default:
			 ST7789_DrawImage_u(118+x, 104+y, 12, 12, led_Alarm_12x12);
		}
//	}

	// прорисовка B1 нужна?
//	if(picNew.pic_B1 != picOld.pic_B1)
//	{
		// перерисовать статус B1
//		switch (picNew.pic_B1)
//		{
//	    	case On:
//	    		ST7789_DrawImage_u(132+x, 48+y, 12, 12, led_on_12x12);
//	    		break;
//	    	case Off:
//	    		ST7789_DrawImage_u(132+x, 48+y, 12, 12, led_off_12x12);
//	    		break;
//	    	case Alarm:
//	    		ST7789_DrawImage_u(132+x, 48+y, 12, 12, led_Alarm_12x12);
//	    		break;
//	    	default:
//	    		ST7789_DrawImage_u(132+x, 48+y, 12, 12, led_on_12x12);
//		}
//	}

//   	if(!(MODBUS_REG(0x0F) & 0x08))
//	{
//		if(MODBUS_REG(0x0F) & 0x01) ST7789_DrawImage_u(132+x, 94+y, 12, 12, led_on_12x12);
//		else ST7789_DrawImage_u(132+x, 94+y, 12, 12, led_off_12x12);
//	}
//   	else
//   	{
//   		// вентилятор П1 в аварии
//   		ST7789_DrawImage_u(132+x, 94+y, 12, 12, led_Alarm_12x12);
//   	}

//   	if(!(MODBUS_REG(0x0F) & 0x04))
//	{
//   		if(MODBUS_REG(0x0F) & 0x02) ST7789_DrawImage_u(132+x, 48+y, 12, 12, led_on_12x12);
//   		else ST7789_DrawImage_u(132+x, 48+y, 12, 12, led_off_12x12);
//	}
//   	else
//   	{
//   		// вентилятор В1 в аварии
//   		ST7789_DrawImage_u(132+x, 48+y, 12, 12, led_Alarm_12x12);
//   	}

// -----------------------------------------------------------------------------------
	// Прорисовка фильтров воздушных
	// прорисовка filter_П1 нужна?
//	if((picNew.pic_Filtr_n1 != picOld.pic_Filtr_n1) || (picNew.pic_Filtr_B1 != picOld.pic_Filtr_B1) )
//	{
		if(picNew.pic_Filtr_n1 == Alarm)
		{
			ST7789_DrawImage_u(35+x, 103+y, 14, 46, Filt_Alarm_14x46);
		}
		else
		{
			ST7789_DrawImage_u(35+x, 103+y, 14, 46, filter_14x46);
		}

//	}

// -----------------------------------------------------------------------------------
	// Прорисовка клапанов On/Off
	// прорисовка Yn1 нужна?
//	if(picNew.pic_Yn1 != picOld.pic_Yn1)
//	{

//    	if(picNew.pic_Yn1 == On) ST7789_DrawImage_u(20+x, 48+y, 15, 46, Air_ON_15x46);
//    	else ST7789_DrawImage_u(20+x, 48+y, 15, 46, Air_OFF_15x46);
//	}

	// прорисовка YB1 нужна?
//	if(picNew.pic_YB1 != picOld.pic_YB1)
//	{
//		if(picNew.pic_YB1 == On) ST7789_DrawImage_u(20+x, 94+y, 15, 46, Air_ON_15x46);
//		else ST7789_DrawImage_u(20+x, 94+y, 15, 46, Air_OFF_15x46);
//	}
// -----------------------------------------------------------------------------------
	// Прорисовка водяног клапана Y3
//	if(picNew.pic_Y3 != picOld.pic_Y3)
//	{
		if(picNew.pic_Y3 == Alarm)
		{
			ST7789_DrawImage_u(154+x, 151+y, 19, 19, WaterValve_Alarm_R_19x19);
		}
		else
		{
			ST7789_DrawImage_u(154+x, 151+y, 19, 19, WaterValve_R_19x19);
		}
//	}
		// -----------------------------------------------------------------------------------
		// Прорисовка воздушного клапана Y1
	//	if(picNew.pic_Y1 != picOld.pic_Y1)
	//	{
			if(picNew.pic_Y1 == Alarm)
			{
				ST7789_DrawImage_u(20+x, 103+y, 15, 46, Air_Valve_Alarm_15x46);
			}
			else
			{
				ST7789_DrawImage_u(20+x, 103+y, 15, 46, Air_Valve_V_15x46);
			}
	//	}
// -----------------------------------------------------------------------------------
		// Пререрисовка рекуператора
//	if(picNew.pic_Recup_2 != picOld.pic_Recup_2)
//	{
//		if(picNew.pic_Recup_2 != Alarm)
//		{
//			ST7789_DrawImage_u(49+x, 48+y, 49, 92, Recup_2_49x92);
//		}
//		else
//		{
//			ST7789_DrawImage_u(49+x, 48+y, 49, 92, Recup_2_Alarm_49x92);
//		}
//	}

// -----------------------------------------------------------------------------------
		// Пререрисовка воздушного клапана в рекуператоре Y4
//	if(picNew.pic_Y4 != picOld.pic_Y4)
//	{
//		if(picNew.pic_Y4 != Alarm)
//		{
//			ST7789_DrawImage_u(65+x, 48+y, 15, 25, Air_Small_15x25);
//		}
//		else
//		{
//			ST7789_DrawImage_u(65+x, 48+y, 15, 25, Air_Small_Alarm_15x25);
//		}
//	}

// -----------------------------------------------------------------------------------
	// Прорисовка ручн/лето/вент/зима/ 0/1/2/3
	ST7789_print_u(190+x, 30+y, ST7789_BLUE, ST7789_WHITE, 1, &Font_11x18, 1, menu.strMode[MODBUS_REG(0x48)]/* menu.strMenu[(mode_Menu & 0x000000F0) >> 4][MODBUS_REG(0x48)]*/);

	//ST7789_print_u(20+x, 164+y, RGB565(10, 10, 10), RGB565(100, 100, 255), 1, &Font_16x26, 1, " СТОП "/*menu.strMenu[10][MODBUS_REG(0x49)]/* menu.strMenu[(mode_Menu & 0x000000F0) >> 4][MODBUS_REG(0x48)]*/);

	// Прорисовка тумблера ПУСК/СТОП/КАЛЕНДАРЬ
	uint16_t backColor;
	if((g_mode_Menu & 0x0F) == 2)  backColor = RGB565(100,100,255);
	else backColor = RGB565(200,200,255);


	  if((g_mode_Menu & BIT_MODIFY_REG) && ((g_mode_Menu & 0x0F) == 2))
		  ST7789_print_u(25, 187, ST7789_RED, backColor, 1, &Font_11x18, 1, menu.strMenu[14][MODBUS_REG(0x3F)]);
	  else
		  ST7789_print_u(25, 187, RGB565(10, 10, 10), backColor, 1, &Font_11x18, 1, menu.strMenu[14][MODBUS_REG(0x3F)]);



// -----------------------------------------------------------------------------------
	  // Сообщение об ошибке
	  // Прорисовка кнопки "Сброс ошибки"
	  uint16_t backColor_Error;

//		if((g_mode_Menu & 0x0F) == 1)  backColor_Error = RGB565(255,200,200);
//   	    backColor_Error = ST7789_WHITE;


	if(MODBUS_REG(0x1F))
	   ST7789_print_u(90+x, 30+y, ST7789_RED, ST7789_WHITE, 1, &Font_11x18, 1, " Ошибка!");
	else  ST7789_print_u(90+x, 30+y, ST7789_RED, ST7789_WHITE, 1, &Font_11x18, 1, "        ");


	 picOld = picNew; // изменения сделаны



	 // кнопка
	 if((g_mode_Menu & 0x7ff) == 0x101){
	   ST7789_DrawImage_u(115+x, 181+y, 120, 24, Reset_21_120x24);
	   MODBUS_REG(0x47) &= ~0x8000;
	 }

	 else if((g_mode_Menu & 0x7ff) == 0x301){
	   ST7789_DrawImage_u(115+x, 181+y, 120, 24, Reset_31_120x24);
	   MODBUS_REG(0x47) |= 0x8000;
	 }

	 else{
		 ST7789_DrawImage_u(115+x, 181+y, 120, 24, Reset_11_120x24);
		 MODBUS_REG(0x47) &= ~0x8000;
	 }
//	// кнопка
//	if((g_mode_Menu & 0x0F) == 1){
//	   	ST7789_DrawImage_u(185+0, 15+15, 49, 30, Button_3_49x30);
//	 }
//	else if((g_mode_Menu & 0x0F) == 2){
//	   	ST7789_DrawImage_u(185+0, 15+15, 49, 30, Button_3_49x30);
//	 }
//	 else  ST7789_DrawImage_u(185+0, 15+15, 49, 30, Button_1_49x30);

//	  GPIOB->BSRR = (uint32_t)GPIO_PIN_15 << 16U;

}

//================================================================================

//================================================================================
/**
 * @brief Полная прорисовка мнемосхемы HVAC4
 * @param x   int8_t смещение по оси х
 * @param y   int8_t смещение по оси y
 * @return viod
 */
void DrawHVAC4(int8_t x, int8_t y)
{

	ST7789_DrawImage_u(5+x, 111+y, 7, 26, Right_7x26);
	ST7789_DrawImage_u(20+x, 103+y, 15, 46, Air_Valve_Alarm_15x46);
	ST7789_DrawImage_u(35+x, 103+y, 14, 46, filter_14x46);
	ST7789_FillRect_u(49+x, 148+y, 35, 1, RGB565(0,0,0));

	ST7789_DrawImage_u(53+x, 79+y, 26, 7, Down_26x7); // Down

	ST7789_DrawImage_u(49+x, 88+y, 35, 15, Air_Valve_H_35x15);


//	ST7789_DrawImage_u(49+x, 48+y, 49, 92, Recup_2_49x92);
	ST7789_DrawImage_u(84+x, 103+y, 46, 46, fan1_46x46);
//	ST7789_DrawImage_u(49+35+46-12+x, 48+y, 12, 12, led_off_12x12);

	ST7789_DrawImage_u(130+x, 103+y, 23, 46, TEN_Off_23x46);
//	ST7789_DrawImage_u(144+x, 48+y, 14, 46, filter_14x46);
//	ST7789_DrawImage_u(167+x, 48+y, 6, 46, Pipe_Air_1_6x46);
//	ST7789_DrawImage_u(173+x, 48+y, 6, 46, Pipe_Air_1_6x46);
//	ST7789_DrawImage_u(179+x, 48+y, 6, 46, Pipe_Air_1_6x46);

//	ST7789_DrawImage_u(132+x, 94+y, 22, 21, WaterPipe_R_22x21);
//	ST7789_DrawImage_u(154+x, 96+y, 19, 19, WaterValve_R_19x19);

	ST7789_DrawImage_u(153+x, 103+y, 6, 46, Pipe_Air_1_6x46);
	ST7789_DrawImage_u(159+x, 103+y, 6, 46, Pipe_Air_1_6x46);
	ST7789_DrawImage_u(165+x, 103+y, 6, 46, Pipe_Air_1_6x46);
	ST7789_DrawImage_u(171+x, 103+y, 6, 46, Pipe_Air_1_6x46);
	ST7789_DrawImage_u(177+x, 103+y, 6, 46, Pipe_Air_1_6x46);
	ST7789_DrawImage_u(183+x, 103+y, 6, 46, Pipe_Air_1_6x46);
	ST7789_DrawImage_u(189+x, 103+y, 6, 46, Pipe_Air_1_6x46);
	ST7789_DrawImage_u(195+x, 103+y, 6, 46, Pipe_Air_1_6x46);
	ST7789_DrawImage_u(201+x, 103+y, 6, 46, Pipe_Air_1_6x46);
	ST7789_DrawImage_u(207+x, 103+y, 6, 46, Pipe_Air_1_6x46);
	ST7789_DrawImage_u(213+x, 103+y, 6, 46, Pipe_Air_1_6x46);
	ST7789_DrawImage_u(219+x, 103+y, 6, 46, Pipe_Air_1_6x46);
	ST7789_DrawImage_u(230+x, 111+y, 7, 26, Right_7x26);

	ST7789_print_u(5+x, 27+y, ST7789_BLUE, ST7789_WHITE, 1, &Font_11x18, 1, "Тул");


}


//==================================================================
//==================================================================
/**
 * @brief Перерисовка динамических элементов мнемосхемы HVAC4
 * @param x   int8_t смещение по оси х
 * @param y   int8_t смещение по оси y
 * @return viod
 */
void DrawData4(int16_t x, int16_t y)
{
    // 10 элементов схемы
	static picHVAC4 picNew, picOld;  // нужно ли обновлять элемент схемы

	char strTmp[16];  // временное хранение строки

//	 GPIOB->BSRR = GPIO_PIN_15;

	// получить статус графических элементов
	picNew = GetState_HVAC4();

    // цвета индикаторов температуры
	uint16_t color_T, color_CO2;

	// температура
	if(READ_COIL(COIL_T_LOW))  {color_T = COLOR_T_LOW;}
	else if(READ_COIL(COIL_T_NORM)){color_T = COLOR_T_NORM;}
	else if(READ_COIL(COIL_T_HIGHT)){color_T = COLOR_T_HIGH;}
	else {
		color_T = ST7789_WHITE;
	}

	// CO2
	if(READ_COIL(COIL_CO2_NORM))  {color_CO2 = COLOR_CO2_NORM;}
	else if(READ_COIL(COIL_CO2_HIGHT)){color_CO2 = COLOR_T_HIGH;}
	else {
		color_CO2 = ST7789_WHITE;
	}


    // вывод положения клапанов
   	sprintf(strTmp,"%3i%%", MODBUS_REG(0x10));
	ST7789_print_u(120+x, 152+y, RGB565(0, 200, 0), ST7789_WHITE, 1, &Font_11x18, 1,strTmp);
   	sprintf(strTmp,"%3i%%", MODBUS_REG(0x11));
   	ST7789_print_u(2+x, 85+y, RGB565(0, 200, 0), ST7789_WHITE, 1, &Font_11x18, 1, strTmp);

   	// термодатчики
   	if(!READ_COIL(REG_POINT_MODBUS_OK * 0x10 +0x00))//if(MODBUS_REG(0x03) == -1001)
   		ST7789_print_u(43+x, 152+y, ST7789_BLUE, ST7789_WHITE, 1, &Font_11x18, 1, "---  ");   // T притока
   	else{
   		sprintf(strTmp,"%3i'", MODBUS_REG(0x00));
   		ST7789_print_u(43+x, 152+y, ST7789_BLUE, ST7789_WHITE, 1, &Font_11x18, 1,strTmp);    // T притока
   		}


   	if(!READ_COIL(REG_POINT_MODBUS_OK * 0x10 +0x01))//if(MODBUS_REG(0x01) == -1001)
   		ST7789_print_u(175+x, 117+y, ST7789_BLUE, ST7789_WHITE, 1, &Font_11x18, 1, "---  "); // T подачи
   	else{
   		sprintf(strTmp,"%3i'", MODBUS_REG(0x01));
	    ST7789_print_u(175+x, 117+y, ST7789_BLUE, ST7789_WHITE, 1, &Font_11x18, 1, strTmp);  // T подачи
    	}

//---------------------------------------------------------------------------------------
//   	if(!READ_COIL(REG_POINT_MODBUS_OK * 0x10 +0x02))//if(MODBUS_REG(0x02) == -1001)           // T зоны
//   		ST7789_print_u(120+x, 60+y, ST7789_BLUE, ST7789_WHITE, 1, &Font_11x18, 1, "---  ");
//   	else{
//	    sprintf(strTmp,"%3i'", MODBUS_REG(0x02));
//	    ST7789_print_u(120+x, 60+y, ST7789_BLUE, ST7789_WHITE, 1, &Font_11x18, 1, strTmp);   // T зоны
//   	    }
//
//   	if(!READ_COIL(REG_POINT_MODBUS_OK * 0x10 +0x03))//if(MODBUS_REG(0x02) == -1001)            CO2 зоны
//   		ST7789_print_u(120+x, 80+y, ST7789_BLUE, ST7789_WHITE, 1, &Font_11x18, 1, "---  ");
//   	else{
//	    sprintf(strTmp,"%3ippm", MODBUS_REG(0x03));
//	    ST7789_print_u(120+x, 80+y, ST7789_BLUE, ST7789_WHITE, 1, &Font_11x18, 1, strTmp);   // CO2 0х03
//   	    }
//---------------------------------------------------------------------------------------
   	if(!READ_COIL(REG_POINT_MODBUS_OK * 0x10 +0x02))//if(MODBUS_REG(0x02) == -1001)            Т зоны
   		ST7789_print_u(120+x, 60+y, ST7789_BLUE, ST7789_WHITE, 1, &Font_11x18, 1, "---   ");
   	else{
	    sprintf(strTmp,"%3i'", MODBUS_REG(0x02));
	    ST7789_print_u(120+x, 60+y, ST7789_BLUE, ST7789_WHITE, 1, &Font_11x18, 1, strTmp);   // Т зоны 0х02
	    ST7789_print_u(120+(5*11)+x, 60+y, ST7789_WHITE,color_T, 1, &Font_11x18, 1, " ");   // Т зоны 0х02
   	    }

   	if(!READ_COIL(REG_POINT_MODBUS_OK * 0x10 +0x03))//if(MODBUS_REG(0x02) == -1001)            CO2 зоны
   		ST7789_print_u(120+x, 80+y, ST7789_BLUE, ST7789_WHITE, 1, &Font_11x18, 1, "---      ");
   	else{
	    sprintf(strTmp,"%3ippm", MODBUS_REG(0x03));
	    ST7789_print_u(120+x, 80+y, ST7789_BLUE, ST7789_WHITE, 1, &Font_11x18, 1, strTmp);   // CO2 0х03
	    ST7789_print_u(120+(8*11)+x, 80+y, ST7789_WHITE, color_CO2, 1, &Font_11x18, 1, " ");
   	    }

//======================================================================================
   	if(!READ_COIL(REG_POINT_MODBUS_OK * 0x10 +0x04))//if(MODBUS_REG(0x04) == -1001)
   		ST7789_print_u(45+x, 27+y, ST7789_BLUE, ST7789_WHITE, 1, &Font_11x18, 1, "---  ");   // T ул
   	else{
   		sprintf(strTmp,"%3i'", MODBUS_REG(0x04));
   		ST7789_print_u(45+x, 27+y, ST7789_BLUE, ST7789_WHITE, 1, &Font_11x18, 1,strTmp);    // T ул
   		}



// -----------------------------------------------------------------------------------
	// Прорисовка работы вентилятора П1
	// прорисовка П1 нужна?
//	if(picNew.pic_n1 != picOld.pic_n1)
//	{
		// перерисовать статус П1
		switch (picNew.pic_n1)
		{
		 case On:
		   	ST7789_DrawImage_u(118+x, 104+y, 12, 12, led_on_12x12);
		    break;
		 case Off:
			ST7789_DrawImage_u(118+x, 104+y, 12, 12, led_off_12x12);
			break;
		 case Alarm:
			ST7789_DrawImage_u(118+x, 104+y, 12, 12, led_Alarm_12x12);
			break;
		 default:
			 ST7789_DrawImage_u(118+x, 104+y, 12, 12, led_Alarm_12x12);
		}
//	}

	// прорисовка B1 нужна?
//	if(picNew.pic_B1 != picOld.pic_B1)
//	{
		// перерисовать статус ТЭН1
		switch (picNew.pic_TEN)
		{
	    	case On:
	    		ST7789_DrawImage_u(141+x, 104+y, 12, 12, led_on_12x12);
	    		break;
	    	case Off:
	    		ST7789_DrawImage_u(141+x, 104+y, 12, 12, led_off_12x12);
	    		break;
	    	case Alarm:
	    		ST7789_DrawImage_u(141+x, 104+y, 12, 12, led_Alarm_12x12);
	    		break;
	    	default:
	    		ST7789_DrawImage_u(141+x, 104+y, 12, 12, led_Alarm_12x12);
		}
//	}

//   	if(!(MODBUS_REG(0x0F) & 0x08))
//	{
//		if(MODBUS_REG(0x0F) & 0x01) ST7789_DrawImage_u(132+x, 94+y, 12, 12, led_on_12x12);
//		else ST7789_DrawImage_u(132+x, 94+y, 12, 12, led_off_12x12);
//	}
//   	else
//   	{
//   		// вентилятор П1 в аварии
//   		ST7789_DrawImage_u(132+x, 94+y, 12, 12, led_Alarm_12x12);
//   	}

//   	if(!(MODBUS_REG(0x0F) & 0x04))
//	{
//   		if(MODBUS_REG(0x0F) & 0x02) ST7789_DrawImage_u(132+x, 48+y, 12, 12, led_on_12x12);
//   		else ST7789_DrawImage_u(132+x, 48+y, 12, 12, led_off_12x12);
//	}
//   	else
//   	{
//   		// вентилятор В1 в аварии
//   		ST7789_DrawImage_u(132+x, 48+y, 12, 12, led_Alarm_12x12);
//   	}

// -----------------------------------------------------------------------------------
	// Прорисовка фильтров воздушных
	// прорисовка filter_П1 нужна?
//	if((picNew.pic_Filtr_n1 != picOld.pic_Filtr_n1) || (picNew.pic_Filtr_B1 != picOld.pic_Filtr_B1) )
//	{
		if(picNew.pic_Filtr_n1 == Alarm)
		{
			ST7789_DrawImage_u(35+x, 103+y, 14, 46, Filt_Alarm_14x46);
		}
		else
		{
			ST7789_DrawImage_u(35+x, 103+y, 14, 46, filter_14x46);

		}

//	}

// -----------------------------------------------------------------------------------
	// Прорисовка клапанов On/Off
	// прорисовка Yn1 нужна?
//	if(picNew.pic_Yn1 != picOld.pic_Yn1)
//	{

//    	if(picNew.pic_Yn1 == On) ST7789_DrawImage_u(20+x, 48+y, 15, 46, Air_ON_15x46);
//    	else ST7789_DrawImage_u(20+x, 48+y, 15, 46, Air_OFF_15x46);
//	}

	// прорисовка YB1 нужна?
//	if(picNew.pic_YB1 != picOld.pic_YB1)
//	{
//		if(picNew.pic_YB1 == On) ST7789_DrawImage_u(20+x, 94+y, 15, 46, Air_ON_15x46);
//		else ST7789_DrawImage_u(20+x, 94+y, 15, 46, Air_OFF_15x46);
//	}
// -----------------------------------------------------------------------------------
	// Прорисовка водяног клапана Y1
//	if(picNew.pic_Y1 != picOld.pic_Y1)
//	{
		if(picNew.pic_Y1 == Alarm)
		{
			ST7789_DrawImage_u(20+x, 103+y, 15, 46, Air_Valve_Alarm_15x46);
		}
		else
		{
			ST7789_DrawImage_u(20+x, 103+y, 15, 46, Air_Valve_V_15x46);
		}
//	}
// -----------------------------------------------------------------------------------
		// Пререрисовка рекуператора
//	if(picNew.pic_Recup_2 != picOld.pic_Recup_2)
//	{
//		if(picNew.pic_Recup_2 != Alarm)
//		{
//			ST7789_DrawImage_u(49+x, 48+y, 49, 92, Recup_2_49x92);
//		}
//		else
//		{
//			ST7789_DrawImage_u(49+x, 48+y, 49, 92, Recup_2_Alarm_49x92);
//		}
//	}

// -----------------------------------------------------------------------------------
		// Пререрисовка воздушного клапана в рекуператоре Y4
//	if(picNew.pic_Y4 != picOld.pic_Y4)
//	{
//		if(picNew.pic_Y4 != Alarm)
//		{
//			ST7789_DrawImage_u(65+x, 48+y, 15, 25, Air_Small_15x25);
//		}
//		else
//		{
//			ST7789_DrawImage_u(65+x, 48+y, 15, 25, Air_Small_Alarm_15x25);
//		}
//	}

// -----------------------------------------------------------------------------------
	// Прорисовка ручн/лето/вент/зима/ 0/1/2/3
	ST7789_print_u(190+x, 30+y, ST7789_BLUE, ST7789_WHITE, 1, &Font_11x18, 1, menu.strMode[MODBUS_REG(0x48)]/* menu.strMenu[(mode_Menu & 0x000000F0) >> 4][MODBUS_REG(0x48)]*/);

	//ST7789_print_u(20+x, 164+y, RGB565(10, 10, 10), RGB565(100, 100, 255), 1, &Font_16x26, 1, " СТОП "/*menu.strMenu[10][MODBUS_REG(0x49)]/* menu.strMenu[(mode_Menu & 0x000000F0) >> 4][MODBUS_REG(0x48)]*/);

	// Прорисовка тумблера ПУСК/СТОП/КАЛЕНДАРЬ
	uint16_t backColor;
	if((g_mode_Menu & 0x0F) == 2)  backColor = RGB565(100,100,255);
	else backColor = RGB565(200,200,255);


	  if((g_mode_Menu & BIT_MODIFY_REG) && ((g_mode_Menu & 0x0F) == 2))
		  ST7789_print_u(25, 187, ST7789_RED, backColor, 1, &Font_11x18, 1, menu.strMenu[14][MODBUS_REG(0x3F)]);
	  else
		  ST7789_print_u(25, 187, RGB565(10, 10, 10), backColor, 1, &Font_11x18, 1, menu.strMenu[14][MODBUS_REG(0x3F)]);



// -----------------------------------------------------------------------------------
	  // Сообщение об ошибке
	  // Прорисовка кнопки "Сброс ошибки"
	  uint16_t backColor_Error;

//		if((g_mode_Menu & 0x0F) == 1)  backColor_Error = RGB565(255,200,200);
//   	    backColor_Error = ST7789_WHITE;


	if(MODBUS_REG(0x1F))
	   ST7789_print_u(90+x, 30+y, ST7789_RED, ST7789_WHITE, 1, &Font_11x18, 1, " Ошибка!");
	else  ST7789_print_u(90+x, 30+y, ST7789_RED, ST7789_WHITE, 1, &Font_11x18, 1, "        ");


	 picOld = picNew; // изменения сделаны



	 // кнопка
	 if((g_mode_Menu & 0x7ff) == 0x101){
	   ST7789_DrawImage_u(115+x, 181+y, 120, 24, Reset_21_120x24);
	   MODBUS_REG(0x47) &= ~0x8000;
	 }

	 else if((g_mode_Menu & 0x7ff) == 0x301){
	   ST7789_DrawImage_u(115+x, 181+y, 120, 24, Reset_31_120x24);
	   MODBUS_REG(0x47) |= 0x8000;
	 }

	 else{
		 ST7789_DrawImage_u(115+x, 181+y, 120, 24, Reset_11_120x24);
		 MODBUS_REG(0x47) &= ~0x8000;
	 }
//	// кнопка
//	if((g_mode_Menu & 0x0F) == 1){
//	   	ST7789_DrawImage_u(185+0, 15+15, 49, 30, Button_3_49x30);
//	 }
//	else if((g_mode_Menu & 0x0F) == 2){
//	   	ST7789_DrawImage_u(185+0, 15+15, 49, 30, Button_3_49x30);
//	 }
//	 else  ST7789_DrawImage_u(185+0, 15+15, 49, 30, Button_1_49x30);

//	  GPIOB->BSRR = (uint32_t)GPIO_PIN_15 << 16U;

}

//================================================================================

//================================================================================
/**
 * @brief Полная прорисовка мнемосхемы HVAC5
 * @param x   int8_t смещение по оси х
 * @param y   int8_t смещение по оси y
 * @return viod
 */
void DrawHVAC5(int8_t x, int8_t y)
{

	ST7789_DrawImage_u(5+x, 71+y, 7, 26, Right_7x26);
	ST7789_DrawImage_u(15+x, 63+y, 15, 46, Air_Valve_V_15x46);
	ST7789_DrawImage_u(30+x, 63+y, 14, 46, filter_14x46);
	ST7789_DrawImage_u(44+x, 63+y, 49, 92, Recup_2_49x92); //
	ST7789_DrawImage_u(93+x, 63+y, 46, 46, fan1_46x46);    //
	ST7789_DrawImage_u(127+x, 63+y, 12, 12, led_off_12x12);
	ST7789_DrawImage_u(139+x, 63+y, 14, 46, filter_14x46);
	ST7789_DrawImage_u(153+x, 63+y, 6, 46, Pipe_Air_1_6x46);
	ST7789_DrawImage_u(159+x, 63+y, 6, 46, Pipe_Air_1_6x46);
	ST7789_DrawImage_u(165+x, 63+y, 6, 46, Pipe_Air_1_6x46);
	ST7789_DrawImage_u(171+x, 63+y, 6, 46, Pipe_Air_1_6x46);
	ST7789_DrawImage_u(177+x, 63+y, 6, 46, Pipe_Air_1_6x46);
	ST7789_DrawImage_u(183+x, 63+y, 6, 46, Pipe_Air_1_6x46);
	ST7789_DrawImage_u(189+x, 63+y, 6, 46, Pipe_Air_1_6x46);
	ST7789_DrawImage_u(195+x, 63+y, 6, 46, Pipe_Air_1_6x46);
	ST7789_DrawImage_u(201+x, 63+y, 6, 46, Pipe_Air_1_6x46);
	ST7789_DrawImage_u(207+x, 63+y, 6, 46, Pipe_Air_1_6x46);
	ST7789_DrawImage_u(213+x, 63+y, 6, 46, Pipe_Air_1_6x46);
	ST7789_DrawImage_u(216+x, 63+y, 6, 46, Pipe_Air_1_6x46);
	ST7789_DrawImage_u(222+x, 63+y, 6, 46, Pipe_Air_1_6x46);

	ST7789_DrawImage_u(230+x, 71+y, 7, 26, Left_7x26);

//	ST7789_DrawImage_u(180, 145, 12, 12, led_off_12x12);
	ST7789_DrawImage_u(5+x, 117+y, 7, 26, Left_7x26);           //
	ST7789_DrawImage_u(15+x, 109+y, 15, 46, Air_Valve_V_15x46);
	ST7789_DrawImage_u(30+x, 109+y, 6, 46, Pipe_Air_1_6x46);
	ST7789_DrawImage_u(36+x, 109+y, 6, 46, Pipe_Air_1_6x46);
	ST7789_DrawImage_u(38+x, 109+y, 6, 46, Pipe_Air_1_6x46);
	ST7789_DrawImage_u(93+x, 109+y, 46, 46, fan1_46x46);
	ST7789_DrawImage_u(127+x, 109+y, 12, 12, led_off_12x12);
	ST7789_DrawImage_u(139+x, 109+y, 23, 46, Heater_23x46_color);
	ST7789_DrawImage_u(162+x, 109+y, 23, 46, TEN_Off_23x46);

	ST7789_DrawImage_u(185+x, 109+y, 6, 46, Pipe_Air_1_6x46);
	ST7789_DrawImage_u(191+x, 109+y, 6, 46, Pipe_Air_1_6x46);
	ST7789_DrawImage_u(197+x, 109+y, 6, 46, Pipe_Air_1_6x46);
	ST7789_DrawImage_u(203+x, 109+y, 6, 46, Pipe_Air_1_6x46);
	ST7789_DrawImage_u(209+x, 109+y, 6, 46, Pipe_Air_1_6x46);
	ST7789_DrawImage_u(215+x, 109+y, 6, 46, Pipe_Air_1_6x46);
	ST7789_DrawImage_u(221+x, 109+y, 6, 46, Pipe_Air_1_6x46);

	ST7789_DrawImage_u(230+x, 117+y, 7, 26, Right_7x26);

	ST7789_print_u(5+x, 27+y, ST7789_BLUE, ST7789_WHITE, 1, &Font_11x18, 1, "Тул");

}

//=========================================================================
/**
 * @brief Перерисовка динамических элементов мнемосхемы HVAC5
 * @param x   int8_t смещение по оси х
 * @param y   int8_t смещение по оси y
 * @return viod
 */
void DrawData5(int16_t x, int16_t y)
{
    // 10 элементов схемы
	static picHVAC5 picNew, picOld;  // нужно ли обновлять элемент схемы

	char strTmp[16];  // временное хранение строки

//	 GPIOB->BSRR = GPIO_PIN_15;

	// получить статус графических элементов
	picNew = GetState_HVAC5();



   	sprintf(strTmp,"%3i%%", MODBUS_REG(0x10));
	ST7789_print_u(155+x, 158+y, RGB565(0, 200, 0), ST7789_WHITE, 1, &Font_11x18, 1,strTmp);
   	sprintf(strTmp,"%3i%%", MODBUS_REG(0x11));
   	ST7789_print_u(45+x, 45+y, RGB565(0, 200, 0), ST7789_WHITE, 1, &Font_11x18, 1, strTmp);

   	if(!READ_COIL(REG_POINT_MODBUS_OK * 0x10 +0x00))//if(MODBUS_REG(0x00) == -1001)
   		ST7789_print_u(61+x, 158+y, ST7789_BLUE, ST7789_WHITE, 1, &Font_11x18, 1, "---  ");
   	else{
   		sprintf(strTmp,"%3i'", MODBUS_REG(0x00));
   		ST7789_print_u(61+x, 158+y, ST7789_BLUE, ST7789_WHITE, 1, &Font_11x18, 1,strTmp);
   		}

   	if(!READ_COIL(REG_POINT_MODBUS_OK * 0x10 +0x01))//if(MODBUS_REG(0x01) == -1001)
   		ST7789_print_u(177+x, 123+y, ST7789_BLUE, ST7789_WHITE, 1, &Font_11x18, 1, "---  ");
   	else{
   		sprintf(strTmp,"%3i'", MODBUS_REG(0x01));
	    ST7789_print_u(177+x, 123+y, ST7789_BLUE, ST7789_WHITE, 1, &Font_11x18, 1, strTmp);
    	}

   	if(!READ_COIL(REG_POINT_MODBUS_OK * 0x10 +0x02))//if(MODBUS_REG(0x02) == -1001)
   		ST7789_print_u(165+x, 77+y, ST7789_BLUE, ST7789_WHITE, 1, &Font_11x18, 1, "---  ");
   	else{
	    sprintf(strTmp,"%3i'", MODBUS_REG(0x02));
	    ST7789_print_u(165+x, 77+y, ST7789_BLUE, ST7789_WHITE, 1, &Font_11x18, 1, strTmp);
   	    }

   	if(!READ_COIL(REG_POINT_MODBUS_OK * 0x10 +0x03))//if(MODBUS_REG(0x03) == -1001)
   		ST7789_print_u(16+x, 158+y, ST7789_BLUE, ST7789_WHITE, 1, &Font_11x18, 1, "---  ");
   	else{
   		sprintf(strTmp,"%3i'", MODBUS_REG(0x03));
   		ST7789_print_u(16+x, 158+y, ST7789_BLUE, ST7789_WHITE, 1, &Font_11x18, 1,strTmp);
   		}


   	if(!READ_COIL(REG_POINT_MODBUS_OK * 0x10 +0x04))//if(MODBUS_REG(0x04) == -1001)
   		ST7789_print_u(40+x, 27+y, ST7789_BLUE, ST7789_WHITE, 1, &Font_11x18, 1, "---  ");
   	else{
   		sprintf(strTmp,"%3i'", MODBUS_REG(0x04));
   		ST7789_print_u(40+x, 27+y, ST7789_BLUE, ST7789_WHITE, 1, &Font_11x18, 1,strTmp);
   		}



// -----------------------------------------------------------------------------------
	// Прорисовка работы вентилятора
	// прорисовка П1 нужна?
//	if(picNew.pic_n1 != picOld.pic_n1)
//	{
		// перерисовать статус П1
		switch (picNew.pic_n1)
		{
		 case On:
		   	ST7789_DrawImage_u(127+x, 109+y, 12, 12, led_on_12x12);
		    break;
		 case Off:
			ST7789_DrawImage_u(127+x, 109+y, 12, 12, led_off_12x12);
			break;
		 case Alarm:
			ST7789_DrawImage_u(127+x, 109+y, 12, 12, led_Alarm_12x12);
			break;
		 default:
			 ST7789_DrawImage_u(127+x, 109+y, 12, 12, led_Alarm_12x12);
		}
//	}

	// прорисовка B1 нужна?
//	if(picNew.pic_B1 != picOld.pic_B1)
//	{
		// перерисовать статус B1
		switch (picNew.pic_B1)
		{
	    	case On:
	    		ST7789_DrawImage_u(127+x, 63+y, 12, 12, led_on_12x12);
	    		break;
	    	case Off:
	    		ST7789_DrawImage_u(127+x, 63+y, 12, 12, led_off_12x12);
	    		break;
	    	case Alarm:
	    		ST7789_DrawImage_u(127+x, 63+y, 12, 12, led_Alarm_12x12);
	    		break;
	    	default:
	    		ST7789_DrawImage_u(127+x, 63+y, 12, 12, led_on_12x12);
		}
//	}

		// прорисовка контактора ТЭНа нужна?
	//	if(picNew.pic_TEN != picOld.pic_TEN)
	//	{
			// перерисовать статус B1
			switch (picNew.pic_TEN)
			{
		    	case On:
		    		ST7789_DrawImage_u(162+x, 109+y, 12, 12, led_on_12x12);
		    		break;
		    	case Off:
		    		ST7789_DrawImage_u(162+x, 109+y, 12, 12, led_off_12x12);
		    		break;
		    	case Alarm:
		    		ST7789_DrawImage_u(162+x, 109+y, 12, 12, led_Alarm_12x12);
		    		break;
		    	default:
		    		ST7789_DrawImage_u(162+x, 109+y, 12, 12, led_on_12x12);
			}
	//	}


//   	if(!(MODBUS_REG(0x0F) & 0x08))
//	{
//		if(MODBUS_REG(0x0F) & 0x01) ST7789_DrawImage_u(132+x, 94+y, 12, 12, led_on_12x12);
//		else ST7789_DrawImage_u(132+x, 94+y, 12, 12, led_off_12x12);
//	}
//   	else
//   	{
//   		// вентилятор П1 в аварии
//   		ST7789_DrawImage_u(132+x, 94+y, 12, 12, led_Alarm_12x12);
//   	}

//   	if(!(MODBUS_REG(0x0F) & 0x04))
//	{
//   		if(MODBUS_REG(0x0F) & 0x02) ST7789_DrawImage_u(132+x, 48+y, 12, 12, led_on_12x12);
//   		else ST7789_DrawImage_u(132+x, 48+y, 12, 12, led_off_12x12);
//	}
//   	else
//   	{
//   		// вентилятор В1 в аварии
//   		ST7789_DrawImage_u(132+x, 48+y, 12, 12, led_Alarm_12x12);
//   	}

// -----------------------------------------------------------------------------------
	// Прорисовка фильтров воздушных
	// прорисовка filter_П1 нужна?
//	if((picNew.pic_Filtr_n1 != picOld.pic_Filtr_n1) || (picNew.pic_Filtr_B1 != picOld.pic_Filtr_B1) )
//	{
		if((picNew.pic_Filtr_n1 == Alarm) || (picNew.pic_Filtr_B1 == Alarm))
		{
			ST7789_DrawImage_u(30+x, 63+y, 14, 46, Filt_Alarm_14x46);
			ST7789_DrawImage_u(139+x, 63+y, 14, 46, Filt_Alarm_14x46);
		}
		else
		{
			ST7789_DrawImage_u(30+x, 63+y, 14, 46, filter_14x46);
			ST7789_DrawImage_u(139+x, 63+y, 14, 46, filter_14x46);
		}

//	}

// -----------------------------------------------------------------------------------
	// Прорисовка клапанов On/Off
	// прорисовка Yn1 нужна?
//	if(picNew.pic_Yn1 != picOld.pic_Yn1)
//	{

    	if(picNew.pic_Yn1 == On) ST7789_DrawImage_u(15+x, 63+y, 15, 46, Air_ON_15x46);
    	else ST7789_DrawImage_u(15+x, 63+y, 15, 46, Air_OFF_15x46);
//	}

	// прорисовка YB1 нужна?
//	if(picNew.pic_YB1 != picOld.pic_YB1)
//	{
		if(picNew.pic_YB1 == On) ST7789_DrawImage_u(15+x, 109+y, 15, 46, Air_ON_15x46);
		else ST7789_DrawImage_u(15+x, 109+y, 15, 46, Air_OFF_15x46);
//	}
// -----------------------------------------------------------------------------------
	// Прорисовка водяног клапана Y3
//	if(picNew.pic_Y3 != picOld.pic_Y3)
//	{
//		if(picNew.pic_Y3 == Alarm)
//		{
//			ST7789_DrawImage_u(166+x, 143+y, 19, 19, WaterValve_Alarm_R_19x19);
//		}
//		else
//		{
//			ST7789_DrawImage_u(166+x, 143+y, 19, 19, WaterValve_R_19x19);
//		}
//	}
// -----------------------------------------------------------------------------------
		// Пререрисовка рекуператора
//	if(picNew.pic_Recup_2 != picOld.pic_Recup_2)
//	{
		if(picNew.pic_Recup_2 != Alarm)
		{
			ST7789_DrawImage_u(44+x, 63+y, 49, 92, Recup_2_49x92);
		}
		else
		{
			ST7789_DrawImage_u(44+x, 63+y, 49, 92, Recup_2_Alarm_49x92);
		}
//	}

// -----------------------------------------------------------------------------------
		// Пререрисовка воздушного клапана в рекуператоре Y4
//	if(picNew.pic_Y4 != picOld.pic_Y4)
//	{
		if(picNew.pic_Y4 != Alarm)
		{
			ST7789_DrawImage_u(60+x, 63+y, 15, 25, Air_Small_15x25);
		}
		else
		{
			ST7789_DrawImage_u(60+x, 63+y, 15, 25, Air_Small_Alarm_15x25);
		}
//	}

// -----------------------------------------------------------------------------------
	// Прорисовка ручн/лето/вент/зима/ 0/1/2/3
	ST7789_print_u(190+x, 30+y, ST7789_BLUE, ST7789_WHITE, 1, &Font_11x18, 1, menu.strMode[MODBUS_REG(0x48)]/* menu.strMenu[(mode_Menu & 0x000000F0) >> 4][MODBUS_REG(0x48)]*/);

	//ST7789_print_u(20+x, 164+y, RGB565(10, 10, 10), RGB565(100, 100, 255), 1, &Font_16x26, 1, " СТОП "/*menu.strMenu[10][MODBUS_REG(0x49)]/* menu.strMenu[(mode_Menu & 0x000000F0) >> 4][MODBUS_REG(0x48)]*/);

	// Прорисовка тумблера ПУСК/СТОП/КАЛЕНДАРЬ
	uint16_t backColor;
	if((g_mode_Menu & 0x0F) == 2)  backColor = RGB565(100,100,255);
	else backColor = RGB565(200,200,255);


	  if((g_mode_Menu & BIT_MODIFY_REG) && ((g_mode_Menu & 0x0F) == 2))
		  ST7789_print_u(25, 187, ST7789_RED, backColor, 1, &Font_11x18, 1, menu.strMenu[14][MODBUS_REG(0x3F)]);
	  else
		  ST7789_print_u(25, 187, RGB565(10, 10, 10), backColor, 1, &Font_11x18, 1, menu.strMenu[14][MODBUS_REG(0x3F)]);



// -----------------------------------------------------------------------------------
	  // Сообщение об ошибке
	  // Прорисовка кнопки "Сброс ошибки"
	  uint16_t backColor_Error;

//		if((g_mode_Menu & 0x0F) == 1)  backColor_Error = RGB565(255,200,200);
//   	    backColor_Error = ST7789_WHITE;


	if(MODBUS_REG(0x1F))
	   ST7789_print_u(90+x, 30+y, ST7789_RED, ST7789_WHITE, 1, &Font_11x18, 1, " Ошибка!");
	else  ST7789_print_u(90+x, 30+y, ST7789_RED, ST7789_WHITE, 1, &Font_11x18, 1, "        ");


	 picOld = picNew; // изменения сделаны



	 // кнопка
	 if((g_mode_Menu & 0x7ff) == 0x101){
	   ST7789_DrawImage_u(115+x, 181+y, 120, 24, Reset_21_120x24);
	   MODBUS_REG(0x47) &= ~0x8000;
	 }

	 else if((g_mode_Menu & 0x7ff) == 0x301){
	   ST7789_DrawImage_u(115+x, 181+y, 120, 24, Reset_31_120x24);
	   MODBUS_REG(0x47) |= 0x8000;
	 }

	 else{
		 ST7789_DrawImage_u(115+x, 181+y, 120, 24, Reset_11_120x24);
		 MODBUS_REG(0x47) &= ~0x8000;
	 }
//	// кнопка
//	if((g_mode_Menu & 0x0F) == 1){
//	   	ST7789_DrawImage_u(185+0, 15+15, 49, 30, Button_3_49x30);
//	 }
//	else if((g_mode_Menu & 0x0F) == 2){
//	   	ST7789_DrawImage_u(185+0, 15+15, 49, 30, Button_3_49x30);
//	 }
//	 else  ST7789_DrawImage_u(185+0, 15+15, 49, 30, Button_1_49x30);

//	  GPIOB->BSRR = (uint32_t)GPIO_PIN_15 << 16U;

}

//=================================================================================================================
