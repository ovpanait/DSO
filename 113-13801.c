
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
	Clock_Init();
	 
	Port_Init();

	/* Unlock the Flash Program Erase controller */
	FLASH_Unlock();

	/* EEPROM Init */
	EE_Init();

	/* Init display */
	TFT_Init_Ili9341();
	 
	/* Init USART1 */
	USART1_Init();
	
	clr_screen();

	/* Initialization */
	waveform_init();
	scope_init();
	
	char *test = "Initialization done.\n";
	uputs(test, USART1);

	sampling_config();
	uputs("Configured sampling\n", USART1);
	sampling_enable();
	
	/* Main loop */
	while(1) {
		while(!dso_scope.done_sampling)
			;
		/*char buf[16];
		itoa(ADC_GetConversionValue(ADC1), buf, 10);
		uputs(buf, USART1);
		*/

		/* Display waveform */
		waveform_display();
		/* Read buttons */
		read_btns();
		/* Delay to minimize flickering */
		Delay(65000);
		/* Start looking for trigger */
		sampling_enable();	
	}
}	

