/*

  ******************************************************************************
  * @file 			( фаил ):   ST7789.c
  * @brief 		( описание ):  	
  ******************************************************************************
  * @attention 	( внимание ):	 author: Golinskiy Konstantin	e-mail: golinskiy.konstantin@gmail.com
  ******************************************************************************
  
*/
/**
  ******************************************************************************
  * @file            ST7789_1.c
  * @brief           Функции работы с дисплеем ST7789
  * @author          Golinskiy Konstantin
  ******************************************************************************
 */

#include <ST7789_1.h>
#include "app_globals.h"


uint16_t ST7789_x_Start = ST7789_XSTART;
uint16_t ST7789_y_Start = ST7789_YSTART;

uint16_t ST7789_Width, ST7789_Height;

//==== данные для инициализации дисплея ST7789_240X320 ==========

// инициализация для всех дисплеев одна, так как драйвер расчитан на максимальный размер 240x320
// для подгона пор свой размер двигаем в функции ротации дисплея
static const uint8_t init_cmds[] = {
		9,                       			// 9 commands in list:
		ST7789_SWRESET,   DELAY,    		// 1: Software reset, no args, w/delay
		  150,                     			//    150 ms delay
		ST7789_SLPOUT ,  DELAY,    			// 2: Out of sleep mode, no args, w/delay
		  255,                            	//    255 = 500 ms delay
		ST7789_COLMOD , 1+DELAY,    		// 3: Set color mode, 1 arg + delay:
		  (ST7789_ColorMode_65K | ST7789_ColorMode_16bit),           //    16-bit color 0x55
		  10,                             	//    10 ms delay
		ST7789_MADCTL , 1,                 	// 4: Memory access ctrl (directions), 1 arg:
		  0x00,                           	//    Row addr/col addr, bottom to top refresh
		ST7789_CASET  , 4,                 	// 5: Column addr set, 4 args, no delay:
		  0,0,                             	//    XSTART = 0
		  0,240,                           	//    XEND = 240
		ST7789_RASET  , 4,                 	// 6: Row addr set, 4 args, no delay:
		  0,0,                             	//    YSTART = 0
		  320>>8,320&0xff,                 	//    YEND = 240   320>>8,320&0xff,
		ST7789_INVON ,   DELAY,     		// 7: Inversion ON
		  10,
		ST7789_NORON  ,   DELAY,    		// 8: Normal display on, no args, w/delay
		  10,                              	// 10 ms delay
		ST7789_DISPON ,   DELAY,    		// 9: Main screen turn on, no args, w/delay
		  10 
	};
	//---------------------------------------------------------------------------------------------
	
//===============================================================
	
	
//##############################################################################
	  
	  
//==============================================================================
	  
	  
	  
//==============================================================================
// Процедура инициализации дисплея
//==============================================================================
void ST7789_Init_u(void){
	
	// Задержка после подачи питания
	// если при старте не всегда запускаеться дисплей увеличиваем время задержки
	HAL_Delay(200);

	ST7789_Width = ST7789_WIDTH;
	ST7789_Height = ST7789_HEIGHT;


    ST7789_Select_u(); // CS выключен
	
    ST7789_HardReset_u();  // RST = low, RST = high
    ST7789_ExecuteCommandList_u(init_cmds);
	
    ST7789_Unselect_u();

}
//==============================================================================


//==============================================================================
// Процедура управления SPI
//==============================================================================
void ST7789_Select_u(void) {
	
//    #ifdef CS_PORT
//
//			//-- если захотим переделать под HAL ------------------
//			#ifdef ST7789_SPI_HAL
//				HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, GPIO_PIN_RESET);
//			#endif
//			//-----------------------------------------------------
//
//			//-- если захотим переделать под CMSIS  ---------------
//			#ifdef ST7789_SPI_CMSIS
//				CS_GPIO_Port->BSRR = ( CS_Pin << 16 );
//			#endif
//			//-----------------------------------------------------
//	#endif
	
}
//==============================================================================


//==============================================================================
// Процедура управления SPI
//==============================================================================
void ST7789_Unselect_u(void) {
	
//    #ifdef CS_PORT
//
//			//-- если захотим переделать под HAL ------------------
//			#ifdef ST7789_SPI_HAL
//				HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, GPIO_PIN_SET);
//			#endif
//			//-----------------------------------------------------
//
//			//-- если захотим переделать под CMSIS  ---------------
//			#ifdef ST7789_SPI_CMSIS
//					 CS_GPIO_Port->BSRR = CS_Pin;
//			#endif
//			//-----------------------------------------------------
//
//	#endif
	
}
//==============================================================================


//==============================================================================
// Процедура отправки данных для инициализации дисплея
//==============================================================================
static void ST7789_ExecuteCommandList_u(const uint8_t *addr) {
	
    uint8_t numCommands, numArgs;
    uint16_t ms;

    numCommands = *addr++;  // читаем кол-во команд
    while(numCommands--) {
        uint8_t cmd = *addr++;  // читаем команду
        ST7789_SendCmd_u(cmd);

        numArgs = *addr++;      // читаем кол-во аргументов в команде  и задержку
        // If high bit set, delay follows args
        ms = numArgs & DELAY;
        numArgs &= ~DELAY;
        if(numArgs) {
        	// передаем аргументы
            ST7789_SendDataMASS((uint8_t*)addr, numArgs);
        	//ST7789_SendDataMASS_u((uint8_t*)addr, numArgs);
            addr += numArgs;
        }

        if(ms) {
        	// организуем задержкупосле отправки аргументов
            ms = *addr++;
            if(ms == 255) ms = 500;
            HAL_Delay(ms);
        }
    }
}
//==============================================================================
//==============================================================================
// Процедура вывода архивированного цветного изображения на дисплей
// 0x00EF, 0xFFFF,   повторение,  код_цвета
// 0x00A3, 0xFFFF, 0x0012, 0x0000, 0x0038, 0xFFFF,
//==============================================================================
/**
 * @brief Функция вывода архивированного цветного изображения на дисплей
 * @param x,y - uint16_t координата  положения картинки
 * @param w,h - uint16_t размер картинки
 * @param data - uint16_t* указатель на массив архивированной картинки
 * @note  0x00EF, 0xFFFF,   повторение,  код_цвета<br>
 *        Пример: 0x00A3, 0xFFFF, 0x0012, 0x0000, 0x0038, 0xFFFF, ...
 */
void ST7789_Draw_ArchImage_u(uint16_t x, uint16_t y, uint16_t w, uint16_t h, const uint16_t* data){

    // разархивируем часть массива
    // ...
    // вывод части  изображения
/*
//    for(uint8_t i=0; i<240; i++){
//
//
//    }
*/
	// массив для одной строки
	uint16_t line[240];
    //# читаем кол-во строк  рисунка в шапке архива
    uint8_t pictureHight = data[1];

    //# итерация по строкам
    for(uint8_t current_line=0; current_line < pictureHight; current_line++){
    	uint8_t pix = 0; // номер пикселя в строке
    	//   # вычисляем кол-во черточек в строке
    	uint8_t segment = (data[1+2*current_line]-data[0+2*current_line] + 1);

    	//   # итерация по черточкам
    	for(uint8_t i=0; i < segment ;i+=2){
    		uint16_t AdrStartSeg = data[0+2*current_line] + i;
    		uint16_t AdrEndSeg =   data[0+2*current_line] + i + 1;
    		uint16_t numPix =    data[AdrStartSeg +2*pictureHight + 2] + 1; //  # кол-во повторений
    		uint16_t colorPix =  data[AdrStartSeg +2*pictureHight + 3];     //  # цвет

    		//  # итерация в одной черточке
    		for(uint16_t j=0; j < numPix; j++){
    			line[pix] = colorPix;
    			pix++;
    		}

    	}

    	// вывод строки
        ST7789_SetWindow_u(0, current_line, 240, current_line);

    	ST7789_Select_u();

        ST7789_SendDataMASS_u((uint8_t*)line, sizeof(uint16_t)*240*1);

        ST7789_Unselect_u();
    }
//# читаем кол-во строк в рисунке
//pictureHight = myModule.PixArch[1]

//
//# итерация по строкам
//for current_line in range(1, pictureHight):
//   # вычисляем кол-во черточек в строке
//   segment = (myModule.PixArch[1+2*current_line]-myModule.PixArch[0+2*current_line] + 1)
//
//   # итерация по черточкам
//   for i in range(0, segment, 2):
//
//    		AdrStartSeg = myModule.PixArch[0+2*current_line] + i
//    		AdrEndSeg =   myModule.PixArch[0+2*current_line] + i + 1
//
//    		numPix =    myModule.PixArch[AdrStartSeg +2*pictureHight + 2] + 1   # кол-во повторений
//    		colorPix =  myModule.PixArch[AdrStartSeg +2*pictureHight + 3]       # цвет
//
//    		# итерация в одной черточке
//    		for j in range(0, numPix):
//        		str = "0x{:04X}, ".format(colorPix)
//        		decompFile.write(str)
//    		print("Черта --- ")
//
//    		print("{:d} {:d} {:d}".format(i, AdrStartSeg, AdrEndSeg))
//    		print("{:d} {:d} {:d}".format(i, myModule.PixArch[AdrStartSeg +2*pictureHight + 2] + 1,  myModule.PixArch[AdrStartSeg +2*pictureHight +3 ]))
//


//    ST7789_SetWindow_u(x, y, x+w-1, y+h-1);
//
//	ST7789_Select_u();
//
//    ST7789_SendDataMASS_u((uint8_t*)data, sizeof(uint16_t)*w*h);
//
//    ST7789_Unselect_u();
}



//==============================================================================
// Процедура вывода цветного изображения на дисплей
//==============================================================================
void ST7789_DrawImage_u(uint16_t x, uint16_t y, uint16_t w, uint16_t h, const uint16_t* data) {
	
    if((x >= ST7789_Width) || (y >= ST7789_Height)){
		return;
	}
	
    if((x + w - 1) >= ST7789_Width){
		return;
	}
	
    if((y + h - 1) >= ST7789_Height){
		return;
	}
	
    ST7789_SetWindow_u(x, y, x+w-1, y+h-1);
	
	ST7789_Select_u();
	
    ST7789_SendDataMASS_u((uint8_t*)data, sizeof(uint16_t)*w*h);
	
    ST7789_Unselect_u();
}
//==============================================================================


//==============================================================================
// Процедура аппаратного сброса дисплея (ножкой RESET)
//==============================================================================
void ST7789_HardReset_u(void){

	HAL_GPIO_WritePin(RST_GPIO_Port, RST_Pin, GPIO_PIN_RESET);
	HAL_Delay(10);
	HAL_GPIO_WritePin(RST_GPIO_Port, RST_Pin, GPIO_PIN_SET);

	
}
//==============================================================================


//==============================================================================
// Процедура отправки команды в дисплей
//==============================================================================
__inline void ST7789_SendCmd_u(uint8_t Cmd){
		
	//-- если захотим переделать под HAL ------------------	
	#ifdef ST7789_SPI_HAL
	
		 // pin DC LOW
		 HAL_GPIO_WritePin(DC_GPIO_Port, DC_Pin, GPIO_PIN_RESET);
					 
		 HAL_SPI_Transmit(&ST7789_SPI_HAL, &Cmd, 1, HAL_MAX_DELAY);
		 while(HAL_SPI_GetState(&ST7789_SPI_HAL) != HAL_SPI_STATE_READY){};
				
		 // pin DC HIGH
		 HAL_GPIO_WritePin(DC_GPIO_Port, DC_Pin, GPIO_PIN_SET);
		 
	#endif
	//-----------------------------------------------------
	//-- если захотим переделать под CMSIS  ---------------------------------------------
	#ifdef ST7789_SPI_CMSIS
		
		// pin DC LOW
		DC_GPIO_Port->BSRR = ( DC_Pin << 16 );
	
		//======  FOR F-SERIES ===========================================================
			
			// Disable SPI	
			//CLEAR_BIT(ST7789_SPI_CMSIS->CR1, SPI_CR1_SPE);	// ST7789_SPI_CMSIS->CR1 &= ~SPI_CR1_SPE;
			// Enable SPI
			if((ST7789_SPI_CMSIS->CR1 & SPI_CR1_SPE) != SPI_CR1_SPE){
				// If disabled, I enable it
				SET_BIT(ST7789_SPI_CMSIS->CR1, SPI_CR1_SPE);	// ST7789_SPI_CMSIS->CR1 |= SPI_CR1_SPE;
			}
			
			// Ждем, пока не освободится буфер передатчика
			// TXE(Transmit buffer empty) – устанавливается когда буфер передачи(регистр SPI_DR) пуст, очищается при загрузке данных
			while( (ST7789_SPI_CMSIS->SR & SPI_SR_TXE) == RESET ){};	
			
			// заполняем буфер передатчика 1 байт информации--------------
			*((__IO uint8_t *)&ST7789_SPI_CMSIS->DR) = Cmd;
			
			// TXE(Transmit buffer empty) – устанавливается когда буфер передачи(регистр SPI_DR) пуст, очищается при загрузке данных
			while( (ST7789_SPI_CMSIS->SR & (SPI_SR_TXE | SPI_SR_BSY)) != SPI_SR_TXE ){};
				
			//Ждем, пока SPI освободится от предыдущей передачи
			//while((ST7789_SPI_CMSIS->SR&SPI_SR_BSY)){};	

			// Disable SPI	
			//CLEAR_BIT(ST7789_SPI_CMSIS->CR1, SPI_CR1_SPE);
			
		//================================================================================
		
/*		//======  FOR H-SERIES ===========================================================

			// Disable SPI	
			//CLEAR_BIT(ST7789_SPI_CMSIS->CR1, SPI_CR1_SPE);	// ST7789_SPI_CMSIS->CR1 &= ~SPI_CR1_SPE;
			// Enable SPI
			// Enable SPI
			if((ST7789_SPI_CMSIS->CR1 & SPI_CR1_SPE) != SPI_CR1_SPE){
				// If disabled, I enable it
				SET_BIT(ST7789_SPI_CMSIS->CR1, SPI_CR1_SPE);	// ST7789_SPI_CMSIS->CR1 |= SPI_CR1_SPE;
			}
			
			SET_BIT(ST7789_SPI_CMSIS->CR1, SPI_CR1_CSTART);	// ST7789_SPI_CMSIS->CR1 |= SPI_CR1_CSTART;
			
			// ждем пока SPI будет свободна------------
			//while (!(ST7789_SPI_CMSIS->SR & SPI_SR_TXP)){};		
		
			// передаем 1 байт информации--------------
			*((__IO uint8_t *)&ST7789_SPI_CMSIS->TXDR )  = Cmd;
				
			// Ждать завершения передачи---------------
			while (!( ST7789_SPI_CMSIS -> SR & SPI_SR_TXC )){};
			
			// Disable SPI	
			//CLEAR_BIT(ST7789_SPI_CMSIS->CR1, SPI_CR1_SPE);
			
*/		//================================================================================
		
		// pin DC HIGH
		DC_GPIO_Port->BSRR = DC_Pin;
	
	#endif
	//-----------------------------------------------------------------------------------

}
//==============================================================================


//==============================================================================
// Процедура отправки данных (параметров) в дисплей 1 BYTE
//==============================================================================
__inline void ST7789_SendData_u(uint8_t Data ){
	
	//-- если захотим переделать под HAL ------------------
	#ifdef ST7789_SPI_HAL
	
		HAL_SPI_Transmit(&ST7789_SPI_HAL, &Data, 1, HAL_MAX_DELAY);
		while(HAL_SPI_GetState(&ST7789_SPI_HAL) != HAL_SPI_STATE_READY){};
		
	#endif
	//-----------------------------------------------------
	
	

	//-- если захотим переделать под CMSIS  ---------------------------------------------
	#ifdef ST7789_SPI_CMSIS
		
		//======  FOR F-SERIES ===========================================================
			
			// Disable SPI	
			//CLEAR_BIT(ST7789_SPI_CMSIS->CR1, SPI_CR1_SPE);	// ST7789_SPI_CMSIS->CR1 &= ~SPI_CR1_SPE;
			// Enable SPI
			if((ST7789_SPI_CMSIS->CR1 & SPI_CR1_SPE) != SPI_CR1_SPE){
				// If disabled, I enable it
				SET_BIT(ST7789_SPI_CMSIS->CR1, SPI_CR1_SPE);	// ST7789_SPI_CMSIS->CR1 |= SPI_CR1_SPE;
			}

			// Ждем, пока не освободится буфер передатчика
			// TXE(Transmit buffer empty) – устанавливается когда буфер передачи(регистр SPI_DR) пуст, очищается при загрузке данных
			while( (ST7789_SPI_CMSIS->SR & SPI_SR_TXE) == RESET ){};
		
			// передаем 1 байт информации--------------
			*((__IO uint8_t *)&ST7789_SPI_CMSIS->DR) = Data;

			// TXE(Transmit buffer empty) – устанавливается когда буфер передачи(регистр SPI_DR) пуст, очищается при загрузке данных
			while( (ST7789_SPI_CMSIS->SR & (SPI_SR_TXE | SPI_SR_BSY)) != SPI_SR_TXE ){};

			// Ждем, пока не освободится буфер передатчика
			//while((ST7789_SPI_CMSIS->SR&SPI_SR_BSY)){};	
			
			// Disable SPI	
			//CLEAR_BIT(ST7789_SPI_CMSIS->CR1, SPI_CR1_SPE);
			
		//================================================================================
		
/*		//======  FOR H-SERIES ===========================================================

			// Disable SPI	
			//CLEAR_BIT(ST7789_SPI_CMSIS->CR1, SPI_CR1_SPE);	// ST7789_SPI_CMSIS->CR1 &= ~SPI_CR1_SPE;
			// Enable SPI
			if((ST7789_SPI_CMSIS->CR1 & SPI_CR1_SPE) != SPI_CR1_SPE){
				// If disabled, I enable it
				SET_BIT(ST7789_SPI_CMSIS->CR1, SPI_CR1_SPE);	// ST7789_SPI_CMSIS->CR1 |= SPI_CR1_SPE;
			}

			SET_BIT(ST7789_SPI_CMSIS->CR1, SPI_CR1_CSTART);	// ST7789_SPI_CMSIS->CR1 |= SPI_CR1_CSTART;
			
			// ждем пока SPI будет свободна------------
			//while (!(ST7789_SPI_CMSIS->SR & SPI_SR_TXP)){};		
		
			// передаем 1 байт информации--------------
			*((__IO uint8_t *)&ST7789_SPI_CMSIS->TXDR )  = Data;
				
			// Ждать завершения передачи---------------
			while (!( ST7789_SPI_CMSIS -> SR & SPI_SR_TXC )){};
			
			// Disable SPI	
			//CLEAR_BIT(ST7789_SPI_CMSIS->CR1, SPI_CR1_SPE);
			
*/		//================================================================================
		
	#endif
	//-----------------------------------------------------------------------------------

}
//==============================================================================


//==============================================================================
// Процедура отправки данных (параметров) в дисплей MASS
//==============================================================================
__inline void ST7789_SendDataMASS_u(uint8_t* buff, size_t buff_size){
	
	//-- если захотим переделать под HAL ------------------
	#ifdef ST7789_SPI_HAL

//		if( buff_size <= 0xFFFF ){
//			HAL_SPI_Transmit(&ST7789_SPI_HAL, buff, buff_size, HAL_MAX_DELAY);
//		}
//		else{
//			while( buff_size > 0xFFFF ){
//				HAL_SPI_Transmit(&ST7789_SPI_HAL, buff, 0xFFFF, HAL_MAX_DELAY);
//				buff_size-=0xFFFF;
//				buff+=0xFFFF;
//			}
//			HAL_SPI_Transmit(&ST7789_SPI_HAL, buff, buff_size, HAL_MAX_DELAY);
//		}
//
//		while(HAL_SPI_GetState(&ST7789_SPI_HAL) != HAL_SPI_STATE_READY){};
//

			if( buff_size <= 0xFFFF ){
				HAL_SPI_Transmit_DMA(&ST7789_SPI_HAL, buff, buff_size);
			}
			else{
				while( buff_size > 0xFFFF ){
					HAL_SPI_Transmit_DMA(&ST7789_SPI_HAL, buff, 0xFFFF);
					buff_size-=0xFFFF;
					buff+=0xFFFF;
				}
				HAL_SPI_Transmit_DMA(&ST7789_SPI_HAL, buff, buff_size);
			}

			while(HAL_SPI_GetState(&ST7789_SPI_HAL) != HAL_SPI_STATE_READY){};


	#endif
	//-----------------------------------------------------

	
	//-- если захотим переделать под CMSIS  ---------------------------------------------
	#ifdef ST7789_SPI_CMSIS	

#ifdef DRAW_PIXEL
		//======  FOR F-SERIES ===========================================================
			
			// Disable SPI	
			//CLEAR_BIT(ST7789_SPI_CMSIS->CR1, SPI_CR1_SPE);	// ST7789_SPI_CMSIS->CR1 &= ~SPI_CR1_SPE;
			// Enable SPI
			if((ST7789_SPI_CMSIS->CR1 & SPI_CR1_SPE) != SPI_CR1_SPE){
				// If disabled, I enable it
				SET_BIT(ST7789_SPI_CMSIS->CR1, SPI_CR1_SPE);	// ST7789_SPI_CMSIS->CR1 |= SPI_CR1_SPE;
			}
			
			while( buff_size ){
				
			// Ждем, пока не освободится буфер передатчика
			// TXE(Transmit buffer empty) – устанавливается когда буфер передачи(регистр SPI_DR) пуст, очищается при загрузке данных
			while( (ST7789_SPI_CMSIS->SR & SPI_SR_TXE) == RESET ){};
					
				// передаем 1 байт информации--------------
				*((__IO uint8_t *)&ST7789_SPI_CMSIS->DR) = *buff++;

				buff_size--;
			}
			
			// TXE(Transmit buffer empty) – устанавливается когда буфер передачи(регистр SPI_DR) пуст, очищается при загрузке данных
			while( (ST7789_SPI_CMSIS->SR & (SPI_SR_TXE | SPI_SR_BSY)) != SPI_SR_TXE ){};
				
			// Ждем, пока не освободится буфер передатчика
			// while((ST7789_SPI_CMSIS->SR&SPI_SR_BSY)){};
				
			// Disable SPI	
			//CLEAR_BIT(ST7789_SPI_CMSIS->CR1, SPI_CR1_SPE);
			
#endif

#ifdef DRAW_PIXEL_DMA
			CLEAR_BIT(SPI3->CR1, SPI_CR1_SPE);
			DMA2_Channel2->CCR &= ~(DMA_CCR_EN);
            DMA2_Channel2->CNDTR = buff_size;
            DMA2_Channel2->CPAR = (uint32_t *)(&(ST7789_SPI_CMSIS->DR));
            DMA2_Channel2->CMAR = buff;
            DMA2_Channel2->CCR |= DMA_CCR_EN;
            uint32_t ccr =             DMA2_Channel2->CCR;
            uint32_t dr =             ST7789_SPI_CMSIS->DR;
            ccr =             DMA2_Channel2->CCR;
			 SET_BIT(SPI3->CR2, SPI_CR2_TXDMAEN);
			 SET_BIT(SPI3->CR1, SPI_CR1_SPE);


#endif
		//================================================================================
		
/*		//======  FOR H-SERIES ===========================================================

			// Disable SPI	
			//CLEAR_BIT(ST7789_SPI_CMSIS->CR1, SPI_CR1_SPE);	// ST7789_SPI_CMSIS->CR1 &= ~SPI_CR1_SPE;
			// Enable SPI
			if((ST7789_SPI_CMSIS->CR1 & SPI_CR1_SPE) != SPI_CR1_SPE){
				// If disabled, I enable it
				SET_BIT(ST7789_SPI_CMSIS->CR1, SPI_CR1_SPE);	// ST7789_SPI_CMSIS->CR1 |= SPI_CR1_SPE;
			}

			SET_BIT(ST7789_SPI_CMSIS->CR1, SPI_CR1_CSTART);	// ST7789_SPI_CMSIS->CR1 |= SPI_CR1_CSTART;
			
			// ждем пока SPI будет свободна------------
			//while (!(ST7789_SPI_CMSIS->SR & SPI_SR_TXP)){};		
			
			while( buff_size ){
		
				// передаем 1 байт информации--------------
				*((__IO uint8_t *)&ST7789_SPI_CMSIS->TXDR )  = *buff++;
				
				// Ждать завершения передачи---------------
				while (!( ST7789_SPI_CMSIS -> SR & SPI_SR_TXC )){};

				buff_size--;

			}
			
			// Disable SPI	
			//CLEAR_BIT(ST7789_SPI_CMSIS->CR1, SPI_CR1_SPE);
			
*/		//================================================================================
		
	#endif
	//-----------------------------------------------------------------------------------

}
//==============================================================================

//==============================================================================
// Процедура отправки данных (параметров) в дисплей MASS
//==============================================================================
__inline void ST7789_SendDataMASS(uint8_t* buff, size_t buff_size){

	//-- если захотим переделать под HAL ------------------
	#ifdef ST7789_SPI_HAL

		if( buff_size <= 0xFFFF ){
			HAL_SPI_Transmit(&ST7789_SPI_HAL, buff, buff_size, HAL_MAX_DELAY);
		}
		else{
			while( buff_size > 0xFFFF ){
				HAL_SPI_Transmit(&ST7789_SPI_HAL, buff, 0xFFFF, HAL_MAX_DELAY);
				buff_size-=0xFFFF;
				buff+=0xFFFF;
			}
			HAL_SPI_Transmit(&ST7789_SPI_HAL, buff, buff_size, HAL_MAX_DELAY);
		}

		while(HAL_SPI_GetState(&ST7789_SPI_HAL) != HAL_SPI_STATE_READY){};


	#endif
	//-----------------------------------------------------


	//-- если захотим переделать под CMSIS  ---------------------------------------------
	#ifdef ST7789_SPI_CMSIS

		//======  FOR F-SERIES ===========================================================

			// Disable SPI
			//CLEAR_BIT(ST7789_SPI_CMSIS->CR1, SPI_CR1_SPE);	// ST7789_SPI_CMSIS->CR1 &= ~SPI_CR1_SPE;
			// Enable SPI
			if((ST7789_SPI_CMSIS->CR1 & SPI_CR1_SPE) != SPI_CR1_SPE){
				// If disabled, I enable it
				SET_BIT(ST7789_SPI_CMSIS->CR1, SPI_CR1_SPE);	// ST7789_SPI_CMSIS->CR1 |= SPI_CR1_SPE;
			}

			while( buff_size ){

			// Ждем, пока не освободится буфер передатчика
			// TXE(Transmit buffer empty) – устанавливается когда буфер передачи(регистр SPI_DR) пуст, очищается при загрузке данных
			while( (ST7789_SPI_CMSIS->SR & SPI_SR_TXE) == RESET ){};

				// передаем 1 байт информации--------------
				*((__IO uint8_t *)&ST7789_SPI_CMSIS->DR) = *buff++;

				buff_size--;
			}

			// TXE(Transmit buffer empty) – устанавливается когда буфер передачи(регистр SPI_DR) пуст, очищается при загрузке данных
			while( (ST7789_SPI_CMSIS->SR & (SPI_SR_TXE | SPI_SR_BSY)) != SPI_SR_TXE ){};

			// Ждем, пока не освободится буфер передатчика
			// while((ST7789_SPI_CMSIS->SR&SPI_SR_BSY)){};

			// Disable SPI
			//CLEAR_BIT(ST7789_SPI_CMSIS->CR1, SPI_CR1_SPE);

		//================================================================================

/*		//======  FOR H-SERIES ===========================================================

			// Disable SPI
			//CLEAR_BIT(ST7789_SPI_CMSIS->CR1, SPI_CR1_SPE);	// ST7789_SPI_CMSIS->CR1 &= ~SPI_CR1_SPE;
			// Enable SPI
			if((ST7789_SPI_CMSIS->CR1 & SPI_CR1_SPE) != SPI_CR1_SPE){
				// If disabled, I enable it
				SET_BIT(ST7789_SPI_CMSIS->CR1, SPI_CR1_SPE);	// ST7789_SPI_CMSIS->CR1 |= SPI_CR1_SPE;
			}

			SET_BIT(ST7789_SPI_CMSIS->CR1, SPI_CR1_CSTART);	// ST7789_SPI_CMSIS->CR1 |= SPI_CR1_CSTART;

			// ждем пока SPI будет свободна------------
			//while (!(ST7789_SPI_CMSIS->SR & SPI_SR_TXP)){};

			while( buff_size ){

				// передаем 1 байт информации--------------
				*((__IO uint8_t *)&ST7789_SPI_CMSIS->TXDR )  = *buff++;

				// Ждать завершения передачи---------------
				while (!( ST7789_SPI_CMSIS -> SR & SPI_SR_TXC )){};

				buff_size--;

			}

			// Disable SPI
			//CLEAR_BIT(ST7789_SPI_CMSIS->CR1, SPI_CR1_SPE);

*/		//================================================================================

	#endif
	//-----------------------------------------------------------------------------------

}
//==============================================================================


//==============================================================================
// Процедура включения режима сна
//==============================================================================
void ST7789_SleepModeEnter_u( void ){
	
	ST7789_Select_u();
	
	ST7789_SendCmd_u(ST7789_SLPIN);
	
	ST7789_Unselect_u();
	
	HAL_Delay(250);
}
//==============================================================================


//==============================================================================
// Процедура отключения режима сна
//==============================================================================
void ST7789_SleepModeExit_u( void ){
	
	ST7789_Select_u();
	
	ST7789_SendCmd_u(ST7789_SLPOUT);
	
	ST7789_Unselect_u();
	
	HAL_Delay(250);
}
//==============================================================================


//==============================================================================
// Процедура включения/отключения режима частичного заполнения экрана
//==============================================================================
void ST7789_InversionMode_u(uint8_t Mode){
	
  ST7789_Select_u();
	
  if (Mode){
    ST7789_SendCmd_u(ST7789_INVON);
  }
  else{
    ST7789_SendCmd_u(ST7789_INVOFF);
  }
  
  ST7789_Unselect_u();
}
//==============================================================================


//==============================================================================
// Процедура закрашивает экран цветом color
//==============================================================================
void ST7789_FillScreen_u(uint16_t color){
	
  ST7789_FillRect_u(0, 0,  ST7789_Width, ST7789_Height, color);
}
//==============================================================================


//==============================================================================
// Процедура очистки экрана - закрашивает экран цветом черный
//==============================================================================
void ST7789_Clear_u(void){
	
  ST7789_FillRect_u(0, 0,  ST7789_Width, ST7789_Height, 0);
}
//==============================================================================


//==============================================================================
// Процедура заполнения прямоугольника цветом color
//==============================================================================
void ST7789_FillRect_u(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color){
	
  if ((x >= ST7789_Width) || (y >= ST7789_Height)){
	  return;
  }
  
  if ((x + w) > ST7789_Width){	  
	  w = ST7789_Width - x;
  }
  
  if ((y + h) > ST7789_Height){
	  h = ST7789_Height - y;
  }
  
  ST7789_SetWindow_u(x, y, x + w - 1, y + h - 1);
  
//  for (uint32_t i = 0; i < (h * w); i++){
//	  ST7789_RamWrite(&color, 1);
//  }
		
  ST7789_RamWrite_u(&color, (h * w));
}
//==============================================================================


//==============================================================================
// Процедура установка границ экрана для заполнения
//==============================================================================
void ST7789_SetWindow_u(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1){
	
	ST7789_Select_u();
	
	ST7789_ColumnSet_u(x0, x1);
	ST7789_RowSet_u(y0, y1);
	
	// write to RAM
	ST7789_SendCmd_u(ST7789_RAMWR);
	
	ST7789_Unselect_u();
	
}
//==============================================================================


//==============================================================================
// Процедура записи данных в дисплей
//==============================================================================
void ST7789_RamWrite_u(uint16_t *pBuff, uint32_t Len){
	
  ST7789_Select_u();
	
  if(Len == 1){
  uint8_t buff[2];
  buff[0] = *pBuff >> 8;
  buff[1] = *pBuff & 0xFF;
	
  while (Len--){
//    ST7789_SendData_u( buff[0] );
//    ST7789_SendData_u( buff[1] );
	  ST7789_SendDataMASS_u( buff, 2);
  } 
	
  }

  else{
	  memset(dimForChar, *pBuff, 16*240*2);
	  for(char i=0;i<15;i++){
		  ST7789_SendDataMASS_u( dimForChar, 16*240*2);
		  HAL_Delay(1);
	  }
  }
  ST7789_Unselect_u();



}
//==============================================================================


//==============================================================================
// Процедура установки начального и конечного адресов колонок
//==============================================================================
static void ST7789_ColumnSet_u(uint16_t ColumnStart, uint16_t ColumnEnd){
	
  if (ColumnStart > ColumnEnd){
    return;
  }
  
  if (ColumnEnd > ST7789_Width){
    return;
  }
  
  ColumnStart += ST7789_x_Start;
  ColumnEnd += ST7789_x_Start;
  
  ST7789_SendCmd_u(ST7789_CASET);
  ST7789_SendData_u(ColumnStart >> 8);
  ST7789_SendData_u(ColumnStart & 0xFF);
  ST7789_SendData_u(ColumnEnd >> 8);
  ST7789_SendData_u(ColumnEnd & 0xFF);
  
}
//==============================================================================


//==============================================================================
// Процедура установки начального и конечного адресов строк
//==============================================================================
static void ST7789_RowSet_u(uint16_t RowStart, uint16_t RowEnd){
	
  if (RowStart > RowEnd){
    return;
  }
  
  if (RowEnd > ST7789_Height){
    return;
  }
  
  RowStart += ST7789_y_Start;
  RowEnd += ST7789_y_Start;
 
  ST7789_SendCmd_u(ST7789_RASET);
  ST7789_SendData_u(RowStart >> 8);
  ST7789_SendData_u(RowStart & 0xFF);
  ST7789_SendData_u(RowEnd >> 8);
  ST7789_SendData_u(RowEnd & 0xFF);

}
//==============================================================================


//==============================================================================
// Процедура управления подсветкой (ШИМ)
//==============================================================================
void ST7789_SetBL_u(uint8_t Value){
	
//  if (Value > 100)
//    Value = 100;

//	tmr2_PWM_set(ST77xx_PWM_TMR2_Chan, Value);

}
//==============================================================================


//==============================================================================
// Процедура включения/отключения питания дисплея
//==============================================================================
void ST7789_DisplayPower_u(uint8_t On){
	
  ST7789_Select_u();
	
  if (On){
    ST7789_SendCmd_u(ST7789_DISPON);
  }
  else{
    ST7789_SendCmd_u(ST7789_DISPOFF);
  }
  
  ST7789_Unselect_u();
}
//==============================================================================


//==============================================================================
// Процедура рисования прямоугольника ( пустотелый )
//==============================================================================
void ST7789_DrawRectangle_u(int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color) {
	
  ST7789_DrawLine_u(x1, y1, x1, y2, color);
  ST7789_DrawLine_u(x2, y1, x2, y2, color);
  ST7789_DrawLine_u(x1, y1, x2, y1, color);
  ST7789_DrawLine_u(x1, y2, x2, y2, color);
	
}
//==============================================================================


//==============================================================================
// Процедура вспомогательная для --- Процедура рисования прямоугольника ( заполненый )
//==============================================================================
static void SwapInt16Values_u(int16_t *pValue1, int16_t *pValue2){
	
  int16_t TempValue = *pValue1;
  *pValue1 = *pValue2;
  *pValue2 = TempValue;
}
//==============================================================================


//==============================================================================
// Процедура рисования прямоугольника ( заполненый )
//==============================================================================
void ST7789_DrawRectangleFilled_u(int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t fillcolor) {
	
  if (x1 > x2){
    SwapInt16Values_u(&x1, &x2);
  }
  
  if (y1 > y2){
    SwapInt16Values_u(&y1, &y2);
  }
  
  ST7789_FillRect_u(x1, y1, x2 - x1, y2 - y1, fillcolor);
}
//==============================================================================


//==============================================================================
// Процедура вспомогательная для --- Процедура рисования линии
//==============================================================================
static void ST7789_DrawLine_Slow_u(int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color) {
	
  const int16_t deltaX = abs(x2 - x1);
  const int16_t deltaY = abs(y2 - y1);
  const int16_t signX = x1 < x2 ? 1 : -1;
  const int16_t signY = y1 < y2 ? 1 : -1;

  int16_t error = deltaX - deltaY;

  ST7789_DrawPixel_u(x2, y2, color);

  while (x1 != x2 || y1 != y2) {
	  
    ST7789_DrawPixel_u(x1, y1, color);
    const int16_t error2 = error * 2;
 
    if (error2 > -deltaY) {
		
      error -= deltaY;
      x1 += signX;
    }
    if (error2 < deltaX){
		
      error += deltaX;
      y1 += signY;
    }
  }
}
//==============================================================================


//==============================================================================
// Процедура рисования линии
//==============================================================================
void ST7789_DrawLine_u(int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color) {

  if (x1 == x2){

    if (y1 > y2){
      ST7789_FillRect_u(x1, y2, 1, y1 - y2 + 1, color);
	}
    else{
      ST7789_FillRect_u(x1, y1, 1, y2 - y1 + 1, color);
	}
	
    return;
  }
  
  if (y1 == y2){
    
    if (x1 > x2){
      ST7789_FillRect_u(x2, y1, x1 - x2 + 1, 1, color);
	}
    else{
      ST7789_FillRect_u(x1, y1, x2 - x1 + 1, 1, color);
	}
	
    return;
  }
  
  ST7789_DrawLine_Slow_u(x1, y1, x2, y2, color);
}
//==============================================================================


//==============================================================================
// Процедура рисования треугольника ( пустотелый )
//==============================================================================
void ST7789_DrawTriangle_u(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t x3, uint16_t y3, uint16_t color){
	/* Draw lines */
	ST7789_DrawLine_u(x1, y1, x2, y2, color);
	ST7789_DrawLine_u(x2, y2, x3, y3, color);
	ST7789_DrawLine_u(x3, y3, x1, y1, color);
}
//==============================================================================


//==============================================================================
// Процедура рисования треугольника ( заполненый )
//==============================================================================
void ST7789_DrawFilledTriangle_u(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t x3, uint16_t y3, uint16_t color){
	
	int16_t deltax = 0, deltay = 0, x = 0, y = 0, xinc1 = 0, xinc2 = 0, 
	yinc1 = 0, yinc2 = 0, den = 0, num = 0, numadd = 0, numpixels = 0, 
	curpixel = 0;
	
	deltax = abs(x2 - x1);
	deltay = abs(y2 - y1);
	x = x1;
	y = y1;

	if (x2 >= x1) {
		xinc1 = 1;
		xinc2 = 1;
	} 
	else {
		xinc1 = -1;
		xinc2 = -1;
	}

	if (y2 >= y1) {
		yinc1 = 1;
		yinc2 = 1;
	} 
	else {
		yinc1 = -1;
		yinc2 = -1;
	}

	if (deltax >= deltay){
		xinc1 = 0;
		yinc2 = 0;
		den = deltax;
		num = deltax / 2;
		numadd = deltay;
		numpixels = deltax;
	} 
	else {
		xinc2 = 0;
		yinc1 = 0;
		den = deltay;
		num = deltay / 2;
		numadd = deltax;
		numpixels = deltay;
	}

	for (curpixel = 0; curpixel <= numpixels; curpixel++) {
		ST7789_DrawLine_u(x, y, x3, y3, color);

		num += numadd;
		if (num >= den) {
			num -= den;
			x += xinc1;
			y += yinc1;
		}
		x += xinc2;
		y += yinc2;
	}
}
//==============================================================================


//==============================================================================
// Процедура окрашивает 1 пиксель дисплея
//==============================================================================
void ST7789_DrawPixel_u(int16_t x, int16_t y, uint16_t color){
	
  if ((x < 0) ||(x >= ST7789_Width) || (y < 0) || (y >= ST7789_Height)){
    return;
  }

  ST7789_SetWindow_u(x, y, x, y);
  ST7789_RamWrite_u(&color, 1);
}
//==============================================================================


//==============================================================================
// Процедура рисования круг ( заполненый )
//==============================================================================
void ST7789_DrawCircleFilled_u(int16_t x0, int16_t y0, int16_t radius, uint16_t fillcolor) {
	
  int x = 0;
  int y = radius;
  int delta = 1 - 2 * radius;
  int error = 0;

  while (y >= 0){
	  
    ST7789_DrawLine_u(x0 + x, y0 - y, x0 + x, y0 + y, fillcolor);
    ST7789_DrawLine_u(x0 - x, y0 - y, x0 - x, y0 + y, fillcolor);
    error = 2 * (delta + y) - 1;

    if (delta < 0 && error <= 0) {
		
      ++x;
      delta += 2 * x + 1;
      continue;
    }
	
    error = 2 * (delta - x) - 1;
		
    if (delta > 0 && error > 0) {
		
      --y;
      delta += 1 - 2 * y;
      continue;
    }
	
    ++x;
    delta += 2 * (x - y);
    --y;
  }
}
//==============================================================================


//==============================================================================
// Процедура рисования круг ( пустотелый )
//==============================================================================
void ST7789_DrawCircle_u(int16_t x0, int16_t y0, int16_t radius, uint16_t color) {
	
  int x = 0;
  int y = radius;
  int delta = 1 - 2 * radius;
  int error = 0;

  while (y >= 0){
	  
    ST7789_DrawPixel_u(x0 + x, y0 + y, color);
    ST7789_DrawPixel_u(x0 + x, y0 - y, color);
    ST7789_DrawPixel_u(x0 - x, y0 + y, color);
    ST7789_DrawPixel_u(x0 - x, y0 - y, color);
    error = 2 * (delta + y) - 1;

    if (delta < 0 && error <= 0) {
		
      ++x;
      delta += 2 * x + 1;
      continue;
    }
	
    error = 2 * (delta - x) - 1;
		
    if (delta > 0 && error > 0) {
		
      --y;
      delta += 1 - 2 * y;
      continue;
    }
	
    ++x;
    delta += 2 * (x - y);
    --y;
  }
}
//==============================================================================


//==============================================================================
// Процедура рисования символа ( 1 буква или знак )
//==============================================================================
void ST7789_DrawChar_u(uint16_t x, uint16_t y, uint16_t TextColor, uint16_t BgColor, uint8_t TransparentBg, FontDef_t* Font, uint8_t multiplier, unsigned char ch){
	
	uint32_t i, b, j;
	
	uint32_t X = x, Y = y;
	
	uint8_t xx, yy;
	
	if( multiplier < 1 ){
		multiplier = 1;
	}

	/* Check available space in LCD */
	if (ST7789_Width >= ( x + Font->FontWidth) || ST7789_Height >= ( y + Font->FontHeight)){

	
			/* Go through font */
			for (i = 0; i < Font->FontHeight; i++) {		
				
				if( ch < 127 ){			
					b = Font->data[(ch - 32) * Font->FontHeight + i];
				}
				
				else if( (uint8_t) ch > 191 ){
					// +96 это так как латинские символы и знаки в шрифтах занимают 96 позиций
					// и если в шрифте который содержит сперва латиницу и спец символы и потом 
					// только кирилицу то нужно добавлять 95 если шрифт 
					// содержит только кирилицу то +96 не нужно
					b = Font->data[((ch - 192) + 96) * Font->FontHeight + i];
				}
				
				else if( (uint8_t) ch == 168 ){	// 168 символ по ASCII - Ё
					// 160 эллемент ( символ Ё ) 
					b = Font->data[( 160 ) * Font->FontHeight + i];
				}
				
				else if( (uint8_t) ch == 184 ){	// 184 символ по ASCII - ё
					// 161 эллемент  ( символ ё ) 
					b = Font->data[( 161 ) * Font->FontHeight + i];
				}
				//-------------------------------------------------------------------
				
				//----  Украинская раскладка ----------------------------------------------------
				else if( (uint8_t) ch == 170 ){	// 168 символ по ASCII - Є
					// 162 эллемент ( символ Є )
					b = Font->data[( 162 ) * Font->FontHeight + i];
				}
				else if( (uint8_t) ch == 175 ){	// 184 символ по ASCII - Ї
					// 163 эллемент  ( символ Ї )
					b = Font->data[( 163 ) * Font->FontHeight + i];
				}
				else if( (uint8_t) ch == 178 ){	// 168 символ по ASCII - І
					// 164 эллемент ( символ І )
					b = Font->data[( 164 ) * Font->FontHeight + i];
				}
				else if( (uint8_t) ch == 179 ){	// 184 символ по ASCII - і
					// 165 эллемент  ( символ і )
					b = Font->data[( 165 ) * Font->FontHeight + i];
				}
				else if( (uint8_t) ch == 186 ){	// 184 символ по ASCII - є
					// 166 эллемент  ( символ є )
					b = Font->data[( 166 ) * Font->FontHeight + i];
				}
				else if( (uint8_t) ch == 191 ){	// 168 символ по ASCII - ї
					// 167 эллемент ( символ ї )
					b = Font->data[( 167 ) * Font->FontHeight + i];
				}
				//-----------------------------------------------------------------------------
			

				for (j = 0; j < Font->FontWidth; j++) {
					
					if ((b << j) & 0x8000) {
						
						for (yy = 0; yy < multiplier; yy++){
							for (xx = 0; xx < multiplier; xx++){
#ifdef DRAW_PIXEL
							    ST7789_DrawPixel_u(X+xx, Y+yy, TextColor);
#endif
#ifdef DRAW_PIXEL_DMA
								dimForChar[(Y-y+yy)*(Font->FontWidth)*multiplier+(X-x+xx)] = (TextColor << 8) | (TextColor >> 8);
#endif
							}

						}
						
					} 
					else if( TransparentBg ){
						
						for (yy = 0; yy < multiplier; yy++){
							for (xx = 0; xx < multiplier; xx++){
#ifdef DRAW_PIXEL
							    ST7789_DrawPixel_u(X+xx, Y+yy, BgColor);
#endif
#ifdef DRAW_PIXEL_DMA
								dimForChar[(Y-y+yy)*(Font->FontWidth)*multiplier+(X-x+xx)] = (BgColor << 8) | (BgColor >> 8);
#endif
							}

						}
						
					}
					X = X + multiplier;

				}
				X = x;
				Y = Y + multiplier;
			}
#ifdef DRAW_PIXEL_DMA
			 uint16_t numDim = (Font->FontWidth)*multiplier*(Font->FontHeight)*multiplier*2;

			 ST7789_SetWindow_u(x,y,x+(Font->FontWidth*multiplier - 1),y+(Font->FontHeight*multiplier - 1));
		     ST7789_SendDataMASS_u((uint8_t*)dimForChar, numDim);
		     for(uint16_t i=0; i< numDim/2 ;i++) dimForChar[i]=0;
			// HAL_Delay(1);
#endif
	}
}
//==============================================================================


//==============================================================================
// Процедура рисования строки
//==============================================================================
void ST7789_print_u(uint16_t x, uint16_t y, uint16_t TextColor, uint16_t BgColor, uint8_t TransparentBg, FontDef_t* Font, uint8_t multiplier, char *str){
	
	if( multiplier < 1 ){
		multiplier = 1;
	}
	
	unsigned char buff_char;
	
	uint16_t len = strlen(str);
	
	while (len--) {
		
		//---------------------------------------------------------------------
		// проверка на кириллицу UTF-8, если латиница то пропускаем if
		// Расширенные символы ASCII Win-1251 кириллица (код символа 128-255)
		// проверяем первый байт из двух ( так как UTF-8 ето два байта )
		// если он больше либо равен 0xC0 ( первый байт в кириллеце будет равен 0xD0 либо 0xD1 именно в алфавите )
		if ( (uint8_t)*str >= 0xC0 ){	// код 0xC0 соответствует символу кириллица 'A' по ASCII Win-1251
			
			// проверяем какой именно байт первый 0xD0 либо 0xD1---------------------------------------------
			switch ((uint8_t)*str) {
				case 0xD0: {
					// увеличиваем массив так как нам нужен второй байт
					str++;
					// проверяем второй байт там сам символ
					if ((uint8_t)*str >= 0x90 && (uint8_t)*str <= 0xBF){ buff_char = (*str) + 0x30; }	// байт символов А...Я а...п  делаем здвиг на +48
					else if ((uint8_t)*str == 0x81) { buff_char = 0xA8; break; }		// байт символа Ё ( если нужнф еще символы добавляем тут и в функции DrawChar() )
					else if ((uint8_t)*str == 0x84) { buff_char = 0xAA; break; }		// байт символа Є ( если нужнф еще символы добавляем тут и в функции DrawChar() )
					else if ((uint8_t)*str == 0x86) { buff_char = 0xB2; break; }		// байт символа І ( если нужнф еще символы добавляем тут и в функции DrawChar() )
					else if ((uint8_t)*str == 0x87) { buff_char = 0xAF; break; }		// байт символа Ї ( если нужнф еще символы добавляем тут и в функции DrawChar() )
					break;
				}
				case 0xD1: {
					// увеличиваем массив так как нам нужен второй байт
					str++;
					// проверяем второй байт там сам символ
					if ((uint8_t)*str >= 0x80 && (uint8_t)*str <= 0x8F){ buff_char = (*str) + 0x70; }	// байт символов п...я	елаем здвиг на +112
					else if ((uint8_t)*str == 0x91) { buff_char = 0xB8; break; }		// байт символа ё ( если нужнф еще символы добавляем тут и в функции DrawChar() )
					else if ((uint8_t)*str == 0x94) { buff_char = 0xBA; break; }		// байт символа є ( если нужнф еще символы добавляем тут и в функции DrawChar() )
					else if ((uint8_t)*str == 0x96) { buff_char = 0xB3; break; }		// байт символа і ( если нужнф еще символы добавляем тут и в функции DrawChar() )
					else if ((uint8_t)*str == 0x97) { buff_char = 0xBF; break; }		// байт символа ї ( если нужнф еще символы добавляем тут и в функции DrawChar() )
					break;
				}
			}
			//------------------------------------------------------------------------------------------------
			// уменьшаем еще переменную так как израсходывали 2 байта для кириллицы
			len--;
			
			ST7789_DrawChar_u(x, y, TextColor, BgColor, TransparentBg, Font, multiplier, buff_char);
		}
		//---------------------------------------------------------------------
		else{
			ST7789_DrawChar_u(x, y, TextColor, BgColor, TransparentBg, Font, multiplier, *str);
		}
		
		x = x + (Font->FontWidth * multiplier);
		/* Increase string pointer */
		str++;
	}
}
//==============================================================================


//==============================================================================
// Процедура ротации ( положение ) дисплея
//==============================================================================
// па умолчанию 1 режим ( всего 1, 2, 3, 4 )
void ST7789_rotation_u( uint8_t rotation ){
	
	ST7789_Select_u();
	
	ST7789_SendCmd_u(ST7789_MADCTL);

	// длайвер расчитан на экран 320 х 240 (  максимальный размер )
	// для подгона под любой другой нужно отнимать разницу пикселей

	  switch (rotation) {
		
		case 1:
			//== 1.13" 135 x 240 ST7789 =================================================
			#ifdef ST7789_IS_135X240
				ST7789_SendData_u(ST7789_MADCTL_RGB);
				ST7789_Width = 135;
				ST7789_Height = 240;
				ST7789_x_Start = 52;
				ST7789_y_Start = 40;
				ST7789_FillScreen_u(0);
			#endif
			//==========================================================================
		
			//== 1.3" 240 x 240 ST7789 =================================================
			#ifdef ST7789_IS_240X240
				ST7789_SendData_u(ST7789_MADCTL_RGB);
				ST7789_Width = 240;
				ST7789_Height = 240;
				ST7789_x_Start = 0;
				ST7789_y_Start = 0;
				ST7789_FillScreen_u(0);
			#endif
			//==========================================================================
			
			//== 2" 240 x 320 ST7789 =================================================
			#ifdef ST7789_IS_240X320
				ST7789_SendData_u(ST7789_MADCTL_RGB);
				ST7789_Width = 240;
				ST7789_Height = 320;
				ST7789_x_Start = 0;
				ST7789_y_Start = 0;
				ST7789_FillScreen_u(0);
			#endif
			//==========================================================================
		 break;
		
		case 2:
			//== 1.13" 135 x 240 ST7789 =================================================
			#ifdef ST7789_IS_135X240
				ST7789_SendData_u(ST7789_MADCTL_MX | ST7789_MADCTL_MV | ST7789_MADCTL_RGB);
				ST7789_Width = 240;
				ST7789_Height = 135;
				ST7789_x_Start = 40;
				ST7789_y_Start = 53;
				ST7789_FillScreen_u(0);
			#endif
			//==========================================================================
		
			//== 1.3" 240 x 240 ST7789 =================================================
			#ifdef ST7789_IS_240X240
				ST7789_SendData_u(ST7789_MADCTL_MX | ST7789_MADCTL_MV | ST7789_MADCTL_RGB);
				ST7789_Width = 240;
				ST7789_Height = 240;		
				ST7789_x_Start = 0;
				ST7789_y_Start = 0;
				ST7789_FillScreen_u(0);
			#endif
			//==========================================================================
			
			//== 2" 240 x 320 ST7789 =================================================
			#ifdef ST7789_IS_240X320
				ST7789_SendData_u(ST7789_MADCTL_MX | ST7789_MADCTL_MV | ST7789_MADCTL_RGB);
				ST7789_Width = 320;
				ST7789_Height = 240;		
				ST7789_x_Start = 0;
				ST7789_y_Start = 0;
				ST7789_FillScreen_u(0);
			#endif
			//==========================================================================
		 break;
		
	   case 3:
		   //== 1.13" 135 x 240 ST7789 =================================================
			#ifdef ST7789_IS_135X240
				ST7789_SendData_u(ST7789_MADCTL_MX | ST7789_MADCTL_MY | ST7789_MADCTL_RGB);
				ST7789_Width = 135;
				ST7789_Height = 240;
				ST7789_x_Start = 53;
				ST7789_y_Start = 40;
				ST7789_FillScreen_u(0);
			#endif
			//==========================================================================
	   
			//== 1.3" 240 x 240 ST7789 =================================================
			#ifdef ST7789_IS_240X240
				ST7789_SendData_u(ST7789_MADCTL_MX | ST7789_MADCTL_MY | ST7789_MADCTL_RGB);
				ST7789_Width = 240;
				ST7789_Height = 240;
				ST7789_x_Start = 0;
				ST7789_y_Start = 80;
				ST7789_FillScreen_u(0);
			#endif
			//==========================================================================
	   
			//== 2" 240 x 320 ST7789 =================================================
			#ifdef ST7789_IS_240X320
				ST7789_SendData_u(ST7789_MADCTL_MX | ST7789_MADCTL_MY | ST7789_MADCTL_RGB);
				ST7789_Width = 240;
				ST7789_Height = 320;
				ST7789_x_Start = 0;
				ST7789_y_Start = 0;
				ST7789_FillScreen_u(0);
			#endif
			//==========================================================================
			
		 break;
	   
	   case 4:
		   //== 1.13" 135 x 240 ST7789 =================================================
			#ifdef ST7789_IS_135X240
				ST7789_SendData_u(ST7789_MADCTL_MY | ST7789_MADCTL_MV | ST7789_MADCTL_RGB);
				ST7789_Width = 240;
				ST7789_Height = 135;
				ST7789_x_Start = 40;
				ST7789_y_Start = 52;
				ST7789_FillScreen_u(0);
			#endif
			//==========================================================================
	   
			//== 1.3" 240 x 240 ST7789 =================================================
			#ifdef ST7789_IS_240X240
				ST7789_SendData_u(ST7789_MADCTL_MY | ST7789_MADCTL_MV | ST7789_MADCTL_RGB);
				ST7789_Width = 240;
				ST7789_Height = 240;
				ST7789_x_Start = 80;
				ST7789_y_Start = 0;
				ST7789_FillScreen_u(0);
			#endif
			//==========================================================================
	   
			//== 2" 240 x 320 ST7789 =================================================
			#ifdef ST7789_IS_240X320
				ST7789_SendData_u(ST7789_MADCTL_MY | ST7789_MADCTL_MV | ST7789_MADCTL_RGB);
				ST7789_Width = 320;
				ST7789_Height = 240;
				ST7789_x_Start = 0;
				ST7789_y_Start = 0;
				ST7789_FillScreen_u(0);
			#endif
			//==========================================================================
		 break;
	   
	   default:
		 break;
	  }
	  
	  ST7789_Unselect_u();
}
//==============================================================================


//==============================================================================
// Процедура рисования иконки монохромной
//==============================================================================
void ST7789_DrawBitmap_u(int16_t x, int16_t y, const unsigned char* bitmap, int16_t w, int16_t h, uint16_t color){

    int16_t byteWidth = (w + 7) / 8; 	// Bitmap scanline pad = whole byte
    uint8_t byte = 0;

    for(int16_t j=0; j<h; j++, y++){
		
        for(int16_t i=0; i<w; i++){
			
            if(i & 7){
               byte <<= 1;
            }
            else{
               byte = (*(const unsigned char *)(&bitmap[j * byteWidth + i / 8]));
            }
			
            if(byte & 0x80){
				ST7789_DrawPixel_u(x+i, y, color);
			}
        }
    }
}
//==============================================================================


//==============================================================================
// Процедура рисования прямоугольник с закругленніми краями ( заполненый )
//==============================================================================
void ST7789_DrawFillRoundRect_u(int16_t x, int16_t y, uint16_t width, uint16_t height, int16_t cornerRadius, uint16_t color) {
	
	int16_t max_radius = ((width < height) ? width : height) / 2; // 1/2 minor axis
  if (cornerRadius > max_radius){
    cornerRadius = max_radius;
	}
	
  ST7789_DrawRectangleFilled_u(x + cornerRadius, y, x + cornerRadius + width - 2 * cornerRadius, y + height, color);
  // draw four corners
  ST7789_DrawFillCircleHelper_u(x + width - cornerRadius - 1, y + cornerRadius, cornerRadius, 1, height - 2 * cornerRadius - 1, color);
  ST7789_DrawFillCircleHelper_u(x + cornerRadius, y + cornerRadius, cornerRadius, 2, height - 2 * cornerRadius - 1, color);
}
//==============================================================================

//==============================================================================
// Процедура рисования половины окружности ( правая или левая ) ( заполненый )
//==============================================================================
void ST7789_DrawFillCircleHelper_u(int16_t x0, int16_t y0, int16_t r, uint8_t corners, int16_t delta, uint16_t color) {

  int16_t f = 1 - r;
  int16_t ddF_x = 1;
  int16_t ddF_y = -2 * r;
  int16_t x = 0;
  int16_t y = r;
  int16_t px = x;
  int16_t py = y;

  delta++; // Avoid some +1's in the loop

  while (x < y) {
    if (f >= 0) {
      y--;
      ddF_y += 2;
      f += ddF_y;
    }
    x++;
    ddF_x += 2;
    f += ddF_x;

    if (x < (y + 1)) {
      if (corners & 1){
        ST7789_DrawLine_u(x0 + x, y0 - y, x0 + x, y0 - y - 1 + 2 * y + delta, color);
			}
      if (corners & 2){
        ST7789_DrawLine_u(x0 - x, y0 - y, x0 - x, y0 - y - 1 + 2 * y + delta, color);
			}
    }
    if (y != py) {
      if (corners & 1){
        ST7789_DrawLine_u(x0 + py, y0 - px, x0 + py, y0 - px - 1 + 2 * px + delta, color);
			}
      if (corners & 2){
        ST7789_DrawLine_u(x0 - py, y0 - px, x0 - py, y0 - px - 1 + 2 * px + delta, color);
			}
			py = y;
    }
    px = x;
  }
}
//==============================================================================																		

//==============================================================================
// Процедура рисования четверти окружности (закругление, дуга) ( ширина 1 пиксель)
//==============================================================================
void ST7789_DrawCircleHelper_u(int16_t x0, int16_t y0, int16_t radius, int8_t quadrantMask, uint16_t color)
{
    int16_t f = 1 - radius ;
    int16_t ddF_x = 1;
    int16_t ddF_y = -2 * radius;
    int16_t x = 0;
    int16_t y = radius;

    while (x <= y) {
        if (f >= 0) {
            y--;
            ddF_y += 2;
            f += ddF_y;
        }
				
        x++;
        ddF_x += 2;
        f += ddF_x;

        if (quadrantMask & 0x4) {
            ST7789_DrawPixel_u(x0 + x, y0 + y, color);
            ST7789_DrawPixel_u(x0 + y, y0 + x, color);;
        }
        if (quadrantMask & 0x2) {
			ST7789_DrawPixel_u(x0 + x, y0 - y, color);
            ST7789_DrawPixel_u(x0 + y, y0 - x, color);
        }
        if (quadrantMask & 0x8) {
			ST7789_DrawPixel_u(x0 - y, y0 + x, color);
            ST7789_DrawPixel_u(x0 - x, y0 + y, color);
        }
        if (quadrantMask & 0x1) {
            ST7789_DrawPixel_u(x0 - y, y0 - x, color);
            ST7789_DrawPixel_u(x0 - x, y0 - y, color);
        }
    }
}
//==============================================================================		

//==============================================================================
// Процедура рисования прямоугольник с закругленніми краями ( пустотелый )
//==============================================================================
void ST7789_DrawRoundRect_u(int16_t x, int16_t y, uint16_t width, uint16_t height, int16_t cornerRadius, uint16_t color) {
	
	int16_t max_radius = ((width < height) ? width : height) / 2; // 1/2 minor axis
  if (cornerRadius > max_radius){
    cornerRadius = max_radius;
	}
	
  ST7789_DrawLine_u(x + cornerRadius, y, x + cornerRadius + width -1 - 2 * cornerRadius, y, color);         // Top
  ST7789_DrawLine_u(x + cornerRadius, y + height - 1, x + cornerRadius + width - 1 - 2 * cornerRadius, y + height - 1, color); // Bottom
  ST7789_DrawLine_u(x, y + cornerRadius, x, y + cornerRadius + height - 1 - 2 * cornerRadius, color);         // Left
  ST7789_DrawLine_u(x + width - 1, y + cornerRadius, x + width - 1, y + cornerRadius + height - 1 - 2 * cornerRadius, color); // Right
	
  // draw four corners
	ST7789_DrawCircleHelper_u(x + cornerRadius, y + cornerRadius, cornerRadius, 1, color);
  ST7789_DrawCircleHelper_u(x + width - cornerRadius - 1, y + cornerRadius, cornerRadius, 2, color);
	ST7789_DrawCircleHelper_u(x + width - cornerRadius - 1, y + height - cornerRadius - 1, cornerRadius, 4, color);
  ST7789_DrawCircleHelper_u(x + cornerRadius, y + height - cornerRadius - 1, cornerRadius, 8, color);
}
//==============================================================================










////==============================================================================
//// Процедура вывода буффера кадра на дисплей
////==============================================================================
//// нужно создать сам буфер глобально uint16_t buff_frame[ST7789_WIDTH*ST7789_HEIGHT];
//void ST7789_Update_u(uint16_t color) {
//	
//	for( uint16_t i =0; i < ST7789_Width*ST7789_Height; i ++ ){
//		buff_frame[i] = color;
//	}
//	
//    ST7789_SetWindow_u(0, 0, ST7789_Width, ST7789_Height);
//	
//	ST7789_Select_u();
//	
//    ST7789_SendDataMASS_u((uint8_t*)buff_frame, sizeof(uint16_t)*ST7789_Width*ST7789_Height);
//	
//    ST7789_Unselect_u();
//}
////==============================================================================

//#########################################################################################################################
//#########################################################################################################################

/*

//==============================================================================


//==============================================================================
// Тест поочерёдно выводит на дисплей картинки с SD-флешки
//==============================================================================
void Test_displayImage_u(const char* fname)
{
  FRESULT res;
  
  FIL file;
  res = f_open(&file, fname, FA_READ);
  if (res != FR_OK)
    return;

  unsigned int bytesRead;
  uint8_t header[34];
  res = f_read(&file, header, sizeof(header), &bytesRead);
  if (res != FR_OK) 
  {
    f_close(&file);
    return;
  }

  if ((header[0] != 0x42) || (header[1] != 0x4D))
  {
    f_close(&file);
    return;
  }

  uint32_t imageOffset = header[10] | (header[11] << 8) | (header[12] << 16) | (header[13] << 24);
  uint32_t imageWidth  = header[18] | (header[19] << 8) | (header[20] << 16) | (header[21] << 24);
  uint32_t imageHeight = header[22] | (header[23] << 8) | (header[24] << 16) | (header[25] << 24);
  uint16_t imagePlanes = header[26] | (header[27] << 8);

  uint16_t imageBitsPerPixel = header[28] | (header[29] << 8);
  uint32_t imageCompression  = header[30] | (header[31] << 8) | (header[32] << 16) | (header[33] << 24);

  if((imagePlanes != 1) || (imageBitsPerPixel != 24) || (imageCompression != 0))
  {
    f_close(&file);
    return;
  }

  res = f_lseek(&file, imageOffset);
  if(res != FR_OK)
  {
    f_close(&file);
    return;
  }

  // Подготавливаем буфер строки картинки для вывода
  uint8_t imageRow[(240 * 3 + 3) & ~3];
  uint16_t PixBuff[240];

  for (uint32_t y = 0; y < imageHeight; y++)
  {
    res = f_read(&file, imageRow, (imageWidth * 3 + 3) & ~3, &bytesRead);
    if (res != FR_OK)
    {
      f_close(&file);
      return;
    }
      
    uint32_t rowIdx = 0;
    for (uint32_t x = 0; x < imageWidth; x++)
    {
      uint8_t b = imageRow[rowIdx++];
      uint8_t g = imageRow[rowIdx++];
      uint8_t r = imageRow[rowIdx++];
      PixBuff[x] = RGB565(r, g, b);
    }

    dispcolor_DrawPartXY_u(0, imageHeight - y - 1, imageWidth, 1, PixBuff);
  }

  f_close(&file);
}
//==============================================================================


//==============================================================================
// Тест вывода картинок на дисплей
//==============================================================================
void Test240x240_Images_u(void)
{
  FATFS fatfs;
  DIR DirInfo;
  FILINFO FileInfo;
  FRESULT res;
  
  res = f_mount(&fatfs, "0", 1);
  if (res != FR_OK)
    return;
  
  res = f_chdir("/240x240");
  if (res != FR_OK)
    return;

  res = f_opendir(&DirInfo, "");
  if (res != FR_OK)
    return;
  
  while (1)
  {
    res = f_readdir(&DirInfo, &FileInfo);
    if (res != FR_OK)
      break;
      
    if (FileInfo.fname[0] == 0)
      break;
      
    char *pExt = strstr(FileInfo.fname, ".BMP");
    if (pExt)
    {
      Test_displayImage_u(FileInfo.fname);
      delay_ms(2000);
    }
  }
}
//==============================================================================


//==============================================================================
// Процедура заполнения прямоугольной области из буфера. Порядок заполнения экрана Y - X
//==============================================================================
void ST77xx_DrawPartYX_u(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t *pBuff)
{
  if ((x >= ST77xx_Width) || (y >= ST77xx_Height))
    return;
  
  if ((x + w - 1) >= ST77xx_Width)
    w = ST77xx_Width - x;
  
  if ((y + h - 1) >= ST77xx_Height)
    h = ST77xx_Height - y;

  ST77xx_SetWindow_u(x, y, x + w - 1, y + h - 1);

  for (uint32_t i = 0; i < (h * w); i++)
    ST77xx_RamWrite_u(pBuff++, 1);
}
//==============================================================================


//==============================================================================
// Процедура заполнения прямоугольной области из буфера. Порядок заполнения экрана X - Y
//==============================================================================
void ST77xx_DrawPartXY_u(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t *pBuff)
{
  if ((x >= ST77xx_Width) || (y >= ST77xx_Height))
    return;
  
  if ((x + w - 1) >= ST77xx_Width)
    w = ST77xx_Width - x;
  
  if ((y + h - 1) >= ST77xx_Height)
    h = ST77xx_Height - y;

  for (uint16_t iy = y; iy < y + h; iy++)
  {
    ST77xx_SetWindow_u(x, iy, x + w - 1, iy + 1);
    for (x = w; x > 0; x--)
      ST77xx_RamWrite_u(pBuff++, 1);
  }
}
//==============================================================================

//########################################################################################################

*/



/************************ (C) COPYRIGHT GKP *****END OF FILE****/
