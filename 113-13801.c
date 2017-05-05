
#include "stm32f10x.h"
#include "stm32f10x_conf.h"

#include "Common.h"
#include "Board.h"
#include "Screen.h"
#include "Eeprom.h"
#include "scope.h"
#include "stdlib.h"

extern __IO struct waveform wave;
extern __IO struct scope dso_scope;

int main (void)
{
 	U16 tmp1, tmp2;
 
	Clock_Init();
	 
	Port_Init();

	/* Unlock the Flash Program Erase controller */
	FLASH_Unlock();

	/* EEPROM Init */
	EE_Init();

	TFT_Init_Ili9341();
	 
	/* Init USART1 */
	USART1_Init();

	/* TEST */
	U8 buf[16];
	U8 test_nr = 0;
	/*while(1) {
		ClrScreen();
		itoa(test_nr++, buf, 10);
		PutsGenic(24, 180, buf, clWhite, clBlack, &ASC8X16);
		for(int i = 0; i < 10; i++)
			Delay(21500);
	}*/
	
	ClrScreen();
	//display_grid();

	waveform_init();
	scope_init();
	
	char *test = "Done";
	uputs(test, USART1);

	sampling_config();
	uputs("Done initializing.\n", USART1);
	sampling_enable();
	
	while(1) {
		while(!dso_scope.done_sampling)
			;
		/*char buf[16];
		itoa(ADC_GetConversionValue(ADC1), buf, 10);
		uputs(buf, USART1);
		*/
		/* Reset flags and display waveform */
		waveform_display();
		//read_btns();
		Delay(65000);
		sampling_enable();
		
	}
	
}	

