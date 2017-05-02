
#include "stm32f10x.h"
#include "stm32f10x_conf.h"

#include "Common.h"
#include "Board.h"
#include "Screen.h"
#include "Command.h"
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

	 tmp1 = clBlack;
	 PutsGenic(24, 180, (U8 *)"Ovidiu Panait", clWhite, tmp1, &ASC8X16);
	
	ClrScreen();
	display_grid();

	waveform_init();
	scope_init();
	
	char *test = "Done";
	uputs(test, USART1);

	/* Configure Interrupt controller */
	NVIC_Configuration();
	start_sampling();
	uputs("Done initializing.\n", USART1);
	DMA_Cmd(DMA1_Channel1, ENABLE);

	while(1) {
		/*for(int i = 0; i < 100; ++i)
			Delay(65000);*/
		while(!dso_scope.done_sampling)
			;
		/*char buf[16];
		itoa(ADC_GetConversionValue(ADC1), buf, 10);
		uputs(buf, USART1);
		*/
		dso_scope.done_sampling = 0;
		waveform_display();
		/* DMA1_Channel1 enable */
  		DMA_Cmd(DMA1_Channel1, ENABLE);
		Delay(65000);
		Delay(65000);
	}
}	

