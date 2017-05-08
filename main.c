
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
extern __IO U16 timebase_vals[];

int main (void)
{
	U8 btns_flags;
	U16 timebase;

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
	
	timebase_display(timebase_vals[dso_scope.tb_i]);
 	dso_scope.done_sampling = 1;
	
	/* Main loop */
	while(1) {
		while(!dso_scope.done_sampling)
			;
		/*char buf[16];
		itoa(ADC_GetConversionValue(ADC1), buf, 10);
		uputs(buf, USART1);
		*/

		/* Read buttons */
		btns_flags = read_btns();
		if(BitTest(btns_flags, (1 << TB_FLAG_BIT))) {
			dso_scope.tb_i = (dso_scope.tb_i + 1) % TIMEBASE_NR;
			dso_scope.timebase = timebase = timebase_vals[dso_scope.tb_i];
		} else 
			timebase = 0;
		
		fill_display_buf();
		sampling_enable();

		waveform_display();
		timebase_display(timebase);
		/* Update peak-to-peak voltage */
		voltage_display(PPV_OFFSETX, PPV_OFFSETY, "Vpp:", (wave.max - wave.min + NOISE_MARGIN), TEXT_CL, BG_CL); 
		voltage_display(MAXV_OFFSETX, MAXV_OFFSETY, "Vmax:", (wave.max + NOISE_MARGIN), TEXT_CL, BG_CL);
		freq_display(wave.frequency);
		//for(int i = 0 ; i < ; i++)
			Delay(55000);
		dso_scope.done_displaying = 1;
	}
	UartPutc('\n', USART1);
}	

