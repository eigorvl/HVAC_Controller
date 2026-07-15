#ifndef MENU_H
#define MENU_H

/*
#define BIT_VIEW_CH             0x8000 // The View was changed
#define BITS_CURSOR             0x7800 // Bits cursor position when change
                                       // Set Point
#define BIT_UPDATE_VALUE        0x0400 // Update value
#define BIT_CH_ALLOW            0x0200 // Change is allowed
#define BIT_MENU_CH             0x0100 // The Menu was changed
#define BIT_SUB_MENU            0x00f0 // SubMenu of the Menu
#define BIT_LINE                0x000f // Line of the Menu
*/
//============================================================
//define to Inint menu

#define RUS_MENU
//#define ENG_MENU

#define LAST_POINT_MENU_MAIN        13  // Last point Menu Main
#define LAST_POINT_MENU_STATUS      15  // Last point Menu Status
#define LAST_POINT_MENU_SETPOINTS   15  // Last point Menu SetPoint
#define LAST_POINT_MENU_SENSORS     15  // Last point Menu Sensors
#define LAST_POINT_MENU_DIGITOUT    15  // Last point Menu Digit Out
#define LAST_POINT_MENU_ANALOGOUT   15  // Last point Menu Analog Out
#define LAST_POINT_MENU_ANALOGIN    15  // Last point Menu Analog In
#define LAST_POINT_MENU_ALARMS      15   // Last point Menu Alarms
#define LAST_POINT_MENU_VERSION      2   // Last point Menu Version
#define LAST_POINT_MENU_SAVE      1   // Last point Menu Save SP
#define LAST_POINT_MENU_UNINTYPE  15   // Last point Menu Unit Type
#define LAST_POINT_MENU_MODE   4   // Last point Menu Mode
#define LAST_POINT_MENU_RESERVE   2   // Last point Menu Reserv 1
#define LAST_POINT_MENU_TIME   10   // Last point Menu TIME

#define LAST_POINT_MENU_PASSWORD   4   // Last point Menu TIME
//============================================================
// слово состояния меню
#define BIT_CLEAR_SCREEN        0x00800000  // очистить экран
#define BIT_UPDATE_TIME         0x00400000  // обновить время
#define BIT_UPDATE_CHECKBOXES   0x00200000  // обновить все чекбоксы
#define BIT_UPDATE_CHECKBOX     0x00100000  // обновить только один чекбокс
#define BIT_UPDATE_REG          0x00080000  // обновить только одно значение
#define BIT_UPDATE_STATUS       0x00040000  // обновить статус
#define BIT_COUNT_EC11          0x00020000  // счетчик
#define BIT_BUTTON_EC11         0x00010000  // кнопка

#define BIT_UPDATE_VALUE          0x8000    // обновить значения датчиков и входов
//#define BIT_UPDATE_CURSOR1          0x8000 // обновить курсор первогопункта
#define BIT_UPDATE_CURSOR           0x4000 // обновить курсор
#define BIT_UPDATE_SUBMENU          0x2000 // обновить побменю перерисовать пункты меню
                                           // и их значения
#define BIT_UPDATE_ALL_REGS         0x1000 // обновить вид вводимых значений
//#define BIT_VIEW_CHANGE             0x0800 // The View was changed
#define BIT_UPDATE_MENU             0x0800 // Сменить заголовок меню
#define BIT_CHANGE_VALUE            0x0400 // изменить значение цифрового выхода на противоположное
#define BIT_MODIFY_REG              0x0200 // Находимся в режиме обновления рег-ра
#define BIT_ENTER_SUB_MENU          0x0100 // Вошли в подменю
#define BITS_SUB_MENU               0x0000000f // SubMenu of the Menu
#define BITS_LINE                   0x000000f0 // Line of the Menu
#define BITS_MENU                   0x000000f0 // пункт меню

// Пункты меню
#define BIT_PICTURE       0x0000 // bits start menu PICTURE
#define MENU_PICTURE       0x0000 // bits start menu PICTURE
#define MENU_STATUS        0x0010 // bits start menu STATUS
#define MENU_SET_POINTS    0x0020 // bits start menu  SET_POINTS
#define MENU_SENSORS       0x0030 // bits start menu SENSORS
#define MENU_DIGITAL_OUT   0x0040 // bits start menu DIGITAL_OUT
#define MENU_ANALOG_OUT    0x0050 // bits start menu ANALOG_OUT
#define MENU_ANALOG_IN     0x0060 // bits start menu ANALOG_IN
#define MENU_ALARM         0x0070 // bits start menu ALARMS
#define MENU_UNIT_TYPE     0x00A0 // bits start menu BIT_UNIT_TYPE_MENU
#define MENU_UNIT_MODE 		 0x00B0 // bits start menu BIT_UNIT_MODE_MENU
#define MENU_SAVE_SP 		 0x0090 // bits start menu SAVE_SET_POINT
#define MENU_VERSION 		 0x0080 // bits start menu VERSION
#define MENU_TEMP1  	     0x00D0 // bits start menu TEMPARATY 1 screen

#define MENU_TIME      0x00C0 // bit menu BIT_TIME_MENU
#define MENU_PASSWORD  0x00D0 // bit menu BIT_TIME_MENU

// screen position
#define SHIFT_VALUE          12      //
#define SHIFT_VALUE_SP       12      //
#define SHIFT_VALUE_ON_OFF   13      //
#define SHIFT_UNIT           15      //
#define SHIFT_SQUARE_CURSOR  17      //

#define SHIFT_PRINT_TIME     	149
#define SHIFT_PRINT_REGS     	150
#define SHIFT_X_PRINT_SUBMENU 	5
#define SHIFT_Y_PRINT_SUBMENU 	45
#define STEP_Y_PRINT_SUBMENU    30
#define SHIFT_PRINT_MINUTE     	48
#define SHIFT_PRINT_CURSOR     	214

// type of unit
//#define SLAVE 0x00
//#define HVAC1 0x10
//#define HVAC2 0x20
//#define HVAC3 0x30
//#define HVAC4 0x40

// work mode
#define COOL 0x00
#define FAN  0x01
#define HEAT 0x02

// update data
#define TIME_UPDATE_VALUR    300      // pass update data
#define TIME_VIEW_TEMP_MENU  50000    // pass duration temporary menu

// CURSOR
#define CURSOR_NON     0  // курсор не отображается
#define CURSOR_POINT   1  // курсор указывает
#define CURSOR_MODIFY   2  // курсор при изменении параметра

// Типы меню
#define MENU_TYPE_VIEW               0
#define MENU_TYPE_ANALOG_INPUT       1
#define MENU_TYPE_DIGIT_INPUT        2
#define MENU_TYPE_DIGIT_CHOICE       3
#define MENU_TYPE_DIGIT_MULT_CHOICE  4
#define MENU_TYPE_TIME               5
#define MENU_TYPE_INFO               6
#define MENU_TYPE_SAVE               7

#define MENU_TYPE_PASSWORD           8
//============================================================
typedef struct
{
	  char line_menu;  // пункт в главном меню
	  char line_sub_menu; // пункт в подменю
	  char old_line_menu;  // пункт в главном меню
	  char old_line_sub_menu; // пункт в подменю
	  char bit_menu_changed;  // меню поменялось
	  int mode_Menu;
	  int modeMenu_old; // old state menu
	  char numScreen;  // 0 - strMenu[x][0] - strMenu[x][2]
	                   // 1 - strMenu[x][3] - strMenu[x][5]
	                   // 2 - strMenu[x][6] - strMenu[x][8]
	                   // 3 - strMenu[x][9] - strMenu[x][11]
	                   // 4 - strMenu[x][12] - strMenu[x][14]
	  char pictureDraw;  // 1 - перерисовать картинку

}statusMenu;

//============================================================
/*
 * Структура меню
 */
typedef struct
{
  uint8_t line_menu;      ///< пункт в главном меню
  uint8_t line_sub_menu; ///< пункт в подменю

  uint8_t bit_menu_changed;  ///< меню поменялось

  uint32_t mode_Menu;     ///< словосостояние меню
  uint32_t modeMenu_old; ///< предыдущее словосостояние меню

  /*
   * Номер экрана
   * 0 - strMenu[x][0] - strMenu[x][4]
   * 1 - strMenu[x][5] - strMenu[x][9]
   * 2 - strMenu[x][10] - strMenu[x][14]
   */
  uint8_t numScreen;



  char* strMenu[16][16];
  char* strMain_Menu[15];     ///< Массив указателей на строки заглавия пунктов меню
  char* strTempScreen[5][4];  ///< Массив указателей на временные строки
  char* strMode[4];           // режим работы
  char* strUnitsOfMeasure[6]; // единицы измерения
  uint8_t uOfMeasure[15]; // единицы измерения
  
  uint8_t pointsOfMenu[16];  ///< numbers of points Menu
  uint8_t pointsOfSubMenu[16];  ///< numbers of points subMenu

/**
 * Тип курсора:<br>
 * 0 -  non<br>
 * 1 -  <-<br>
 * 2 -  O<-<br>
 */
  uint8_t cursorType[16];

  /**
   * Тип курсора:<br>
   * 0 -  non<br>
   * 1 -  <-<br>
   * 2 -  O<-<br>
   */
  uint8_t cursorAllowed[16];

  uint8_t typeMenu[16];   // 0 - просмотр серии параметров
                          //  * Analog In * * Digital In * * Sensors *
                          // 1 - аналоговый ввод
  	  	  	  	  	  	  // 2 - цифровой ввод
  	  	  	  	  	  	  // 3 - цифровой выбор одного  * Mode *
                	  	  // 4 - множественный выбор  * Unit Type *
	                      // 5 - время
                          // 6 - информационный * Version *

  int8_t minValue[16][16];     // минимальное значение параметра
  int8_t maxValue[16][16];     // максимальное значение параметра

  //  цвета
  uint16_t colorsOfFontsMenu;  // цвет нормального шрифта меню
  uint16_t colorsOfFontsChoose;  // цвет выделеного шрифта меню
  uint16_t colorsOfFontModify;  // цвет шрифта модифицируемого пункта меню
  uint16_t colorsOfAlarmFontsMenu;  // цвет аварийного шрифта меню

  uint16_t colorsOfBackMenu;  // цвет нормального фона меню
  uint16_t colorsOfBackChoose;  // цвет выделеного фона меню
  uint16_t colorsOfBackModify;  // цвет фона модифицируемого пункта меню
  uint16_t colorsOfBackAlarm;  // цвет аварийного фона меню

  int stepUpdate;  // value = value + stepUpdate
  uint8_t countDurTempMenu;   // duration view temporary menu
  uint8_t unitType;           ///<  Тип системы Slave, HVAC1 ... HVAC14
  uint8_t unitMode;          ///< Режим работы Hand, Cool, Fan, Heat
}s_Menu;

/*
 * Состояние системы
 */
typedef enum   {Off, On, Alarm, Norm} picState;

/*
 * Структура HVAC5
 */
typedef struct
{
	picState pic_M1;   ///< Состояние насоса М1 On/Off/Alarm
	picState pic_Y1;	    ///< Состояние клапана Y3  Norm/Alarm
	picState pic_Alarm;     ///< Состояние  системы Norm/Alarm

} picHVAC1;

/*
 * Структура HVAC2
 */
typedef struct
{
	picState pic_n1; ///< Состояние вентилятора П1 On/Off/Alarm
	picState pic_B1; ///< Состояние вентилятора B1 On/Off/Alarm
	picState pic_Yn1; ///< Состояние клапана YП1 On/Off/Alarm
	picState pic_YB1; ///< Состояние клапана YB1 On/Off/Alarm
	picState pic_Y3; ///< Состояние клапана Y3  Norm/Alarm
	picState pic_Y4; ///< Состояние клапана Y4  Norm/Alarm
	picState pic_Filtr_n1;   ///< Состояние  приточного  фильтра Norm/Alarm
	picState pic_Filtr_B1;   ///< Состояние  вытяжного  фильтра Norm/Alarm
	picState pic_Recup_2;	 ///< состояние пластинчатого рекуператора Norm/Alarm
	picState pic_Alarm;  ///< Состояние  системы Norm/Alarm

} picHVAC2;

/*
 * Структура HVAC3
 */
typedef struct
{
	picState pic_n1; ///< Состояние вентилятора П1 On/Off/Alarm
	picState pic_Y1; ///< Состояние клапана Y1  Norm/Alarm
	picState pic_Y3; ///< Состояние клапана Y3  Norm/Alarm
	picState pic_Filtr_n1;   ///< Состояние  приточного  фильтра Norm/Alarm
	picState pic_Alarm; ///< Состояние  системы Norm/Alarm
} picHVAC3;

/*
 * Структура HVAC4
 */
typedef struct
{
	picState pic_n1; ///< Состояние вентилятора П1 On/Off/Alarm
	picState pic_Y1; ///< Состояние клапана Y1  Norm/Alarm
	picState pic_Y3; ///< Состояние клапана Y3  Norm/Alarm
	picState pic_TEN; ///< Состояние электронагревателя  On/Off/Alarm
	picState pic_Filtr_n1; ///< Состояние  приточного  фильтра Norm/Alarm
	picState pic_Alarm;    ///< Состояние  системы Norm/Alarm

} picHVAC4;

/*
 * Структура HVAC5
 */
typedef struct
{
	picState pic_n1;  ///< Состояние вентилятора П1 On/Off/Alarm
	picState pic_B1;  ///< Состояние вентилятора В1 On/Off/Alarm
	picState pic_Yn1; ///< Состояние клапана YП1 On/Off/Alarm
	picState pic_YB1; ///< Состояние клапана YВ1 On/Off/Alarm
	picState pic_TEN; ///< Состояние электронагревателя  On/Off/Alarm
	picState pic_Y4;  ///< Состояние клапана Y4  Norm/Alarm
	picState pic_Filtr_n1; ///< Состояние  приточного  фильтра Norm/Alarm
	picState pic_Filtr_B1; ///< Состояние  вытяжного  фильтра Norm/Alarm
	picState pic_Recup_2;	    ///< состояние пластинчатого рекуператора Norm/Alarm
	picState pic_Alarm;   ///< Состояние  системы Norm/Alarm
} picHVAC5;

void DrawHVAC1(int8_t x, int8_t y);
void DrawHVAC2(int8_t x, int8_t y);
void DrawHVAC3(int8_t x, int8_t y);
void DrawHVAC4(int8_t x, int8_t y);
void DrawHVAC5(int8_t x, int8_t y);

void DrawData1(int16_t x, int16_t y);
void DrawData2(int16_t x, int16_t y);
void DrawData3(int16_t x, int16_t y);
void DrawData4(int16_t x, int16_t y);
void DrawData5(int16_t x, int16_t y);

uint8_t NumOfScreen(uint32_t menu);
uint8_t IsChangeScreen(uint32_t mode_Menu, uint32_t mode_Menu_old);

//char IsViewMenuChanged();
char InitMenu();

//void SetBIT_BUTTON_EC11();
//void ClearBIT_BUTTON_EC11();

//void SetBIT_COUNT_EC11();
//void ClearBIT_COUNT_EC11();

//void SetBIT_VIEW_CH();
//void ClearBIT_VIEW_CH();

//void SetBIT_MENU_CH();
//void ClearBIT_MENU_CH();

//void SetBIT_CH_ALLOW();
//void ClearBIT_CH_ALLOW();

void SetBIT_UPDATE_VALUE();
void ClearBIT_UPDATE_VALUE();

void OnButtonEC11();
void OnCountEC11();

picHVAC1 GetState_HVAC1();
picHVAC2 GetState_HVAC2();
picHVAC3 GetState_HVAC3();
picHVAC4 GetState_HVAC4();
picHVAC5 GetState_HVAC5();
picHVAC1 GetState_Slave();

#endif
