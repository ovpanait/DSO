#include "stm32f10x.h"

#include "scope.h"
#include "Screen.h"
#include "Board.h"
#include "stdlib.h"
#include "Common.h"
#include "stdio.h"

struct scope dso_scope;
struct waveform wave;

__IO U16 timebase_vals[] = { 10, 20, 50, 100, 200, 500, 1000, 5000 };
__IO U16 timebase_pres[] = { 116, 115, 144, 288, 576, 1440, 2880, 7220 }; /* Timer prescaler 72Mhz */

/* Global peripheral initializers */
__IO TIM_TimeBaseInitTypeDef TIM3_struct;
__IO DMA_InitTypeDef DMA_struct;

/* Static variables */
static U8 buf[5];

void scope_init(void)
{
	dso_scope.done_sampling = 0;
	dso_scope.done_displaying = 0;
	dso_scope.tb_i = 4;
	dso_scope.timebase = timebase_vals[dso_scope.tb_i];
	
	/* Averaging function */
	dso_scope.avg_flag = 1;
	dso_scope.avg_total = 32;

	/* Trig lvl */
	dso_scope.trig_lvl_adc = 1000;
	dso_scope.prev_cal_samp = ADC_MAX;
	
	/* Buttons */
	dso_scope.debounced = 0;
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
	U8 buf[16];
	/* Clear current waveform in one go */
	FillRect(WD_OFFSETX, wave.midpoint - GET_SAMPLE(wave.max), WD_WIDTH, GET_SAMPLE((wave.max - wave.min)) + 3, BG_CL);

	display_grid();
	U16 xpos = 10;
	U16 i = 0, current_ypos = 0, prev_ypos = 0;
	S16 diff = 0;
	U16 midpoint = wave.midpoint;

	wave.max = wave.display_buf[0];
	wave.min = wave.display_buf[0];

	/* Display first sample */
	prev_ypos = GET_SAMPLE(wave.display_buf[i++]);
	FillRect(xpos++, midpoint - prev_ypos, 1, 2, WF_CL);

	/* Display the rest */
	for(; i < SAMPLES_NR; ++i, ++xpos) {
		current_ypos = GET_SAMPLE(wave.display_buf[i]);
		
		/* Update min and max */
		if(wave.display_buf[i] > wave.max)
			wave.max = wave.display_buf[i];
		else if(wave.display_buf[i] < wave.min)
			wave.min = wave.display_buf[i];
		
		/* Display sample accordingly */
		diff = current_ypos - prev_ypos;
		if(diff > 1 && diff <= WD_HEIGHT / 2 )
			FillRect(xpos, midpoint - current_ypos, 2, 2 + diff, WF_CL);
		else if((diff < -1 && diff >= -(WD_HEIGHT / 2)))
			FillRect(xpos, midpoint - prev_ypos, 2, 2 - diff, WF_CL);
		else
			FillRect(xpos, midpoint - current_ypos, 2, 2, WF_CL);
		prev_ypos = current_ypos;
	}

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

U8 read_btns(void) {

	U8 btns_flags = 0;

	if(BitTest(dso_scope.debounced, (1 << PLUS_BTN_BIT)) && !check_btn(GPIOB, GPIO_Pin_14, RESET)) 
		BitClr(dso_scope.debounced, (1 << PLUS_BTN_BIT));
	else if(!BitTest(dso_scope.debounced, (1 << PLUS_BTN_BIT)) && check_btn(GPIOB, GPIO_Pin_14, RESET)){
		BitSet(dso_scope.debounced, (1 << PLUS_BTN_BIT));
		BitSet(btns_flags, (1 << TB_FLAG_BIT));
	}

	return btns_flags;
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

	TIM3_struct.TIM_Period = 50;
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

void timebase_display(U16 timebase)
{
	if(!timebase)
		return;
	
	clr_blk(WD_OFFSETX, 2, 5 * 8, 12);
	if(timebase >= 1000){
		PutsGenic(WD_OFFSETX + 8, 2, (U8 *)" ms", clGreen, clBlack, &ASC8X16);
		PutsGenic(WD_OFFSETX, 2, (U8 *)itoa(timebase / 1000, buf, 10), clGreen, clBlack, &ASC8X16);
	} else {
		PutsGenic(WD_OFFSETX + 17, 2, (U8 *)" us", clGreen, clBlack, &ASC8X16);
		PutsGenic(WD_OFFSETX, 2, (U8 *)itoa(timebase, buf, 10), clGreen, clBlack, &ASC8X16);	
	}
}
		
void ppv_display(void)
{
	U16 ppv = ((wave.max - wave.min + NOISE_MARGIN) * 0.8);

	itoa(ppv / 1000, buf, 10);
	/* Print volts */
	PutsGenic(PPV_OFFSETX, 2, (U8 *) "Vpp: ", clWhite, clBlack, &ASC8X16);
	PutcGenic(PPV_OFFSETX + PPV_SIZE, 2, buf[0], clWhite, clBlack, &ASC8X16);

	if(ppv > 1000)
		ppv %= 1000;
	ppv /= 10;
	/* Print decimal point and fractional part */
	itoa(ppv, buf, 10);
	PutcGenic(PPV_OFFSETX + PPV_SIZE + 8, 2, '.', clWhite, clBlack, &ASC8X16);
	PutsGenic(PPV_OFFSETX + PPV_SIZE + 16, 2, buf, clWhite, clBlack, &ASC8X16);
	PutcGenic(PPV_OFFSETX + PPV_SIZE + 32, 2, 'V', clWhite, clBlack, &ASC8X16);
}
