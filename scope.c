#include "stm32f10x.h"

#include "scope.h"
#include "Screen.h"
#include "Board.h"
#include "stdlib.h"
#include "Common.h"
#include "string.h"

struct scope dso_scope;
struct waveform wave;

__IO U16 timebase_vals[] = { 10, 20, 50, 100, 200, 500, 1000, 5000 };
__IO U16 timebase_pres[] = { 116, 115, 144, 288, 576, 1440, 2880, 7220 }; /* Timer prescaler 72Mhz */

/* Global peripheral initializers */
__IO TIM_TimeBaseInitTypeDef TIM3_struct;
__IO DMA_InitTypeDef DMA_struct;

/* Static variables */
static U8 buf[8];

void scope_init(void)
{
	dso_scope.tb_i = 4;
	dso_scope.done_sampling = 0;
	dso_scope.done_displaying = 0;
	dso_scope.timebase = timebase_vals[dso_scope.tb_i];
	
	/* Real time mode */
	dso_scope.rt_mode = 1;
	dso_scope.rt_timer = (U16)12 * dso_scope.timebase;

	/* Averaging function */
	dso_scope.avg_flag = 1;
	dso_scope.avg_total = 32;

	/* Trig lvl */
	dso_scope.trig_lvl_adc = 2000;
	dso_scope.test_timer = dso_scope.timebase / 2;
	dso_scope.prev_cal_samp = ADC_MAX;
	
	/* Buttons */
	dso_scope.debounced = 0;
	BitSet(dso_scope.btns_flags, (1 << LCURSOR_BIT));
	BitSet(dso_scope.btns_flags, (1 << RCURSOR_BIT));
	BitSet(dso_scope.btns_flags, (1 << TB_BIT));
	dso_scope.btn_selected = tb;
}

void waveform_init(void) 
{
	U16 i;
	for(i = 0; i< SAMPLES_NR; ++i) {
		wave.display_buf[i] = 0;
		wave.avg_buf[i] = 0;
		wave.tmp_buf[i] = 0;
	}

	wave.midpoint = WD_MIDY;
	wave.min = WD_HEIGHT - WD_OFFSETY;
	wave.max = WD_OFFSETY;
}

void waveform_display(void)
{
	U16 prev_val, current_val;
	U16 wave_per = 1; /* The first trigger is the one that started the samping */
	U16 freq_cnt = 0;

	double freq_avg = 0;

	/* Clear current waveform in one go */
	FillRect(WD_OFFSETX, wave.midpoint - GET_SAMPLE(wave.max) - 10, WD_WIDTH, GET_SAMPLE((wave.max - wave.min)) + 3 + 20, BG_CL);

	grid_display();
	U16 xpos = 10;
	U16 i = 0, current_ypos = 0, prev_ypos = 0;
	S16 diff = 0;
	U16 midpoint = wave.midpoint;

	wave.max = wave.display_buf[0];
	wave.min = wave.display_buf[0];

	/* Display first sample */
	prev_val = wave.display_buf[i];
	prev_ypos = GET_SAMPLE(wave.display_buf[i++]);
	FillRect(xpos++, midpoint - prev_ypos, 2, 2, WF_CL);

	/* Display the rest */
	for(; i < SAMPLES_NR; ++i, ++xpos) {
		current_val = wave.display_buf[i];
		current_ypos = GET_SAMPLE(current_val);
		
		/* Update min and max */
		if(current_val > wave.max)
			wave.max = current_val;
		else if(current_val < wave.min)
			wave.min = current_val;	

		/* Update frequency counter */
		if(i > 25){	
			if(!(prev_val < dso_scope.trig_lvl_adc - NOISE_MARGIN && current_val > (dso_scope.trig_lvl_adc + NOISE_MARGIN)))
				++wave_per;
			else {
				if(freq_cnt >= 1)
				{	if(freq_avg == 0)
						freq_avg = GET_FREQ(wave_per);
					else 
						freq_avg = (freq_avg + GET_FREQ(wave_per)) / 2;
				}
				wave_per = 1;
				++freq_cnt;
			}
		}
		
		/* Display sample accordingly */
		diff = current_ypos - prev_ypos;
		if(diff > 1 && diff <= WD_HEIGHT / 2 )
			FillRect(xpos, midpoint - current_ypos, 2, 2 + diff, WF_CL);
		else if((diff < -1 && diff >= -(WD_HEIGHT / 2)))
			FillRect(xpos, midpoint - prev_ypos, 2, 2 - diff, WF_CL);
		else
			FillRect(xpos, midpoint - current_ypos, 2, 2, WF_CL);
		/* Update */
		prev_ypos = current_ypos;
		prev_val = current_val;

	}

	/* Update frequency */
	if(freq_cnt > 1)
		wave.frequency = freq_avg;
	else 
		wave.frequency = 0;
}
	
/* *
   * Buttons config 
   *
*/

/*void plus_btn_iconfig()
{
	//select EXTI line0
	EXTI_InitStructure.EXTI_Line = EXTI_Line14;
	//select interrupt mode
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	//generate interrupt on rising edge
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	//enable EXTI line
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	//send values to registers
	EXTI_Init(&EXTI_InitStructure);

}*/

/* Read buttons state */
U8 check_btn(GPIO_TypeDef* GPIOx, U16 GPIO_pin, U8 state){
	if(GPIO_ReadInputDataBit(GPIOx, GPIO_pin) == state) {
		U8 cnt = 0;
		for(U8 i = 0; i < DEBOUNCE_TOTAL; ++i)
			if(GPIO_ReadInputDataBit(GPIOx, GPIO_pin) == state)
				++cnt;
		if(cnt >= DEBOUNCE_LIM)
			return 1;
	}
	return 0;
}

#define CHECK_BTN(btn_flag_bit, btn_pin) \
	 	if(BitTest(dso_scope.debounced, (1 << (btn_flag_bit))) && !check_btn(BTN_PORT, (btn_pin), RESET)) \
			BitClr(dso_scope.debounced, (1 << (btn_flag_bit))); \
		else if(!BitTest(dso_scope.debounced, (1 << (btn_flag_bit))) && check_btn(BTN_PORT, (btn_pin), RESET)){ \
			BitSet(dso_scope.debounced, (1 << (btn_flag_bit))); \
			BitSet(dso_scope.btns_flags, (1 << (btn_flag_bit))); \
		}
void btns_update(void)
{
	/* If SEL button was pressed */
	if(BitTest(dso_scope.btns_flags, (1 << SEL_BTN_BIT))) {
		dso_scope.btn_selected = ( dso_scope.btn_selected + 1 ) % 3;
		BitClr(dso_scope.btns_flags, (1 << SEL_BTN_BIT));
		BitSet(dso_scope.btns_flags, (1 << TB_BIT));
	}

	/* If PLUS button was pressed */
	if(BitTest(dso_scope.btns_flags, (1 << PLUS_BTN_BIT))) {
		switch(dso_scope.btn_selected) {
			case tb:
				/* Increase timebase */
				BitSet(dso_scope.btns_flags, (1 << TB_BIT));
				dso_scope.tb_i = (dso_scope.tb_i + 1) % TIMEBASE_NR;
				dso_scope.timebase = timebase_vals[dso_scope.tb_i];
				break;
			case l_cursor:
				/* Move waveform upwards */
				BitSet(dso_scope.btns_flags, (1 << LCURSOR_BIT));
				wave.midpoint -= 10;
				break;
			case r_cursor:
				/* Increase trigger level */
				BitSet(dso_scope.btns_flags, (1 << RCURSOR_BIT));
				dso_scope.trig_lvl_adc += 100;
				break;
		} 
		BitClr(dso_scope.btns_flags, (1 << PLUS_BTN_BIT));
	}
	
	/* If MINUS button was pressed */
	if(BitTest(dso_scope.btns_flags, (1 << MINUS_BTN_BIT))) {
		switch(dso_scope.btn_selected) {
			case tb:
				/* Decrease timebase */
				BitSet(dso_scope.btns_flags, (1 << TB_BIT));
				if(!dso_scope.tb_i)
					dso_scope.tb_i = TIMEBASE_NR;
				--dso_scope.tb_i;

				dso_scope.timebase = timebase_vals[dso_scope.tb_i];
				break;
			case l_cursor:
				BitSet(dso_scope.btns_flags, (1 << LCURSOR_BIT));
				/* Move waveform downwards */
				wave.midpoint += 10;
				break;
			case r_cursor:
				/* Decrease trigger lvl */
				BitSet(dso_scope.btns_flags, (1 << RCURSOR_BIT));
				dso_scope.trig_lvl_adc -= 100;
				break;
		}
		BitClr(dso_scope.btns_flags, (1 << MINUS_BTN_BIT));
	}
}

void read_btns(void) {

	CHECK_BTN(PLUS_BTN_BIT, PLUS_BTN_PIN)
	CHECK_BTN(MINUS_BTN_BIT, MINUS_BTN_PIN)
	CHECK_BTN(SEL_BTN_BIT, SEL_BTN_PIN)

	btns_update();
}

/*
	*
	*	CONFIG ADC1 and DMA1 FOR SAMPLING
	*
*/

/*  ADC config */

void ADCs_Init(void)
{
	ADC_InitTypeDef ADC_InitStructure;
	 
	ADC_DeInit(ADC1);
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
	ADC_InitStructure.ADC_ScanConvMode = DISABLE; // 1 Channel
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE; // Conversions Triggered 
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T3_TRGO;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_NbrOfChannel = 1;
	ADC_Init(ADC1, &ADC_InitStructure);
	 
	// ADC1 regular channel 4 configuration 
	ADC_RegularChannelConfig(ADC1, ADC_Channel_4, 1, ADC_SampleTime_1Cycles5);
	 
	/* Enable end of conversion interrupt */
	ADC_ITConfig(ADC1, ADC_IT_EOC , ENABLE);
	ADC_Cmd(ADC1, ENABLE);
	ADC_DMACmd(ADC1, ENABLE);
	 
	/* Enable ADC1 reset calibration register */   
	ADC_ResetCalibration(ADC1);

	/* Check the end of ADC1 reset calibration register */
	while(ADC_GetResetCalibrationStatus(ADC1));

	/* Start ADC1 calibration */
	ADC_StartCalibration(ADC1);

	/* Check the end of ADC1 calibration */
	while(ADC_GetCalibrationStatus(ADC1));

	ADC_SoftwareStartConvCmd(ADC1, ENABLE);
}

#define ADC1_DR_Address    ((uint32_t)0x4001244C)

void DMA_Configuration(void)
{
	  DMA_DeInit(DMA1_Channel1);
	  DMA_struct.DMA_PeripheralBaseAddr = ADC1_DR_Address;
	  DMA_struct.DMA_MemoryBaseAddr = wave.tmp_buf;
	  DMA_struct.DMA_DIR = DMA_DIR_PeripheralSRC;
	  DMA_struct.DMA_BufferSize = SAMPLES_NR;
	  DMA_struct.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	  DMA_struct.DMA_MemoryInc = DMA_MemoryInc_Enable;
	  DMA_struct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	  DMA_struct.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	  DMA_struct.DMA_Mode = DMA_Mode_Circular;
	  DMA_struct.DMA_Priority = DMA_Priority_High;
	  DMA_struct.DMA_M2M = DMA_M2M_Disable;
	  DMA_Init(DMA1_Channel1, &DMA_struct);  
	 
	  /* Enable DMA Stream Transfer Complete interrupt */
	  DMA_ITConfig(DMA1_Channel1, DMA_IT_TC, ENABLE);

}

#define TIM2_FREQ_HZ	36000000
#define TIM2_FREQ_MHZ	36

void TIM3_Configuration(void)
{
	  /* Timer 3 */
	/* Time base configuration */
	TIM_TimeBaseStructInit(&TIM3_struct);
	TIM3_struct.TIM_Period = 50;
	TIM3_struct.TIM_Prescaler = 0;
	TIM3_struct.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM3, &TIM3_struct);

	/* TIM3 TRGO selection */
	TIM_SelectOutputTrigger(TIM3, TIM_TRGOSource_Update); // ADC_ExternalTrigConv_T3_TRGO
}

/* Used for waveform stabilization */

/*void TIM4_Configuration(void)
{
	  TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	 
	  /* Time base configuration */
	  /*TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
	  TIM_TimeBaseStructure.TIM_Prescaler = TIM2_FREQ_MHZ - 1; /* 1 Mhz timer clock  */ 
	  /*TIM_TimeBaseStructure.TIM_Period = (12 * timebase_vals[dso_scope.tb_i]) - 1; /* microseconds */
	  /*TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	  TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);

	  /* TIM3 TRGO selection */
	  //TIM_SelectOutputTrigger(TIM3, TIM_TRGOSource_Update); // ADC_ExternalTrigConv_T3_TRGO 
	  /*TIM_ITConfig (TIM4, TIM_IT_Update, ENABLE);
	 
}*/
 
/**************************************************************************************/
 
void NVIC_Configuration(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	/* Enable the DMA Stream IRQ Channel */
	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	// Enable the TIM3 Interrupt 
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure); 

	/* Enable TIM4 interrupt */
	/*NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);*/ 
	
	/* Analog to digital converter interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = ADC1_2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

}
 
/**************************************************************************************/
 

void sampling_config(void) 
{
	TIM3_Configuration();
	
	ADCs_Init();

	DMA_Configuration();

	//TIM4_Configuration();

	NVIC_Configuration();

	/* TIM4 enable counter */
  	//TIM_Cmd(TIM4, ENABLE);
}

/*
	*
	*	ADC1 - DMA1 END
	*
*/

void sampling_enable(void)
{
	dso_scope.done_sampling = 0;
	dso_scope.done_displaying = 0;
	dso_scope.avg_total = 32;
	dso_scope.prev_cal_samp = ADC_MAX;
	dso_scope.test_timer = dso_scope.timebase / 2;	
	
	/* 10us and 20us special handling */
	/* Sample only half the total amount */
	if(dso_scope.timebase == 20) {
		DMA_struct.DMA_BufferSize = SAMPLES_NR / 2;
		DMA_Init(DMA1_Channel1, &DMA_struct);
	} else if(dso_scope.timebase == 10) {
		DMA_struct.DMA_BufferSize = SAMPLES_NR / 4;
		DMA_Init(DMA1_Channel1, &DMA_struct);
	}
	else {
		DMA_struct.DMA_BufferSize = SAMPLES_NR;
		DMA_Init(DMA1_Channel1, &DMA_struct);
	}
	TIM3_struct.TIM_Period = 72;
	TIM_TimeBaseInit(TIM3, &TIM3_struct);
	/* TIM3 TRGO selection */
	TIM_SelectOutputTrigger(TIM3, TIM_TRGOSource_Update); // ADC_ExternalTrigConv_T3_TRGO
	TIM_Cmd(TIM3, ENABLE);

	/* Enable ADC interrupts */
	ADC_ITConfig(ADC1, ADC_IT_EOC , ENABLE);
}

void fill_display_buf(void)
{
	for(int i = 0; i < SAMPLES_NR; ++i) 
		wave.display_buf[i] = wave.avg_buf[i];
}

void get_digits(U32 n, U8 *dig_buf)
{
	while(n != 0){
		*dig_buf = '0' + n % 10;
		n /= 10;
		++dig_buf;
	}
	*dig_buf = '\0';
}

