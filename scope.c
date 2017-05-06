#include "stm32f10x.h"

#include "scope.h"
#include "Screen.h"
#include "Board.h"
#include "stdlib.h"
#include "Common.h"

struct scope dso_scope;
struct waveform wave;

__IO U16 timebase_vals[] = { 10, 20, 50, 100, 200, 500, 1000, 5000 };
__IO U16 timebase_pres[] = { 28, 57, 144, 288, 576, 1440, 2880, 7220 }; /* Timer prescaler 72Mhz */

void scope_init(void)
{
	dso_scope.done_sampling = 0;
	dso_scope.start_sampling = 1;
	dso_scope.tb_i = 4;
	dso_scope.timebase_flag = 1;

	/* Trig lvl */
	dso_scope.trig_lvl_adc = 2047;
	dso_scope.prev_cal_samp = ADC_MAX;
	
	/* Buttons */
	dso_scope.debounced = 0;
}

void waveform_init(void) 
{
	U16 i;
	for(i = 0; i< SAMPLES_NR; ++i)
		wave.samples[i] = 0;

	wave.midpoint = WD_MIDY;
	wave.min = WD_HEIGHT - WD_OFFSETY;
	wave.max = WD_OFFSETY;
}

void waveform_display(void)
{
	/* Clear current waveform in one go */
	FillRect(WD_OFFSETX, wave.max, WD_WIDTH, wave.min - wave.max + 2, BG_CL);

	display_grid();
	U16 xpos = 10;
	U8 buf[5];	
	U16 i = 0, current_ypos = 0, prev_ypos = 0;
	S16 diff = 0;
	U16 midpoint = wave.midpoint;

	/* Display first sample */
	prev_ypos = GET_SAMPLE(wave.samples[i++]);
	FillRect(xpos++, midpoint - prev_ypos, 2, 2, WF_CL);

	/* Max and min */
	wave.max = midpoint - prev_ypos;
	wave.min = midpoint - prev_ypos;

	/* Display the rest */
	for(; i < SAMPLES_NR; ++i, ++xpos) {
		current_ypos = GET_SAMPLE(wave.samples[i]);
		
		/* Update min and max */
		if(midpoint - current_ypos < wave.max)
			wave.max = midpoint - current_ypos;
		if(midpoint - current_ypos > wave.min)
			wave.min = midpoint - current_ypos;

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
	
	/* Update timebase */
	if(dso_scope.timebase_flag) {
		clr_blk(WD_OFFSETX, 2, 6 * 8, 12);
		if(timebase_vals[dso_scope.tb_i] >= 1000){
			PutsGenic(WD_OFFSETX + 8, 2, (U8 *)" ms", clGreen, clBlack, &ASC8X16);
			PutsGenic(WD_OFFSETX, 2, (U8 *)itoa(timebase_vals[dso_scope.tb_i] / 1000, buf, 10), clGreen, clBlack, &ASC8X16);
		} else {
			PutsGenic(WD_OFFSETX + 17, 2, (U8 *)" us", clGreen, clBlack, &ASC8X16);
			PutsGenic(WD_OFFSETX, 2, (U8 *)itoa(timebase_vals[dso_scope.tb_i], buf, 10), clGreen, clBlack, &ASC8X16);	
		}
		dso_scope.timebase_flag = 0;
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

void read_btns(void) {
	if(BitTest(dso_scope.debounced, (1 << PLUS_BTN_BIT)) && !check_btn(GPIOB, GPIO_Pin_14, RESET)) 
		BitClr(dso_scope.debounced, (1 << PLUS_BTN_BIT));
	else if(!BitTest(dso_scope.debounced, (1 << PLUS_BTN_BIT)) && check_btn(GPIOB, GPIO_Pin_14, RESET)){
		BitSet(dso_scope.debounced, (1 << PLUS_BTN_BIT));
		dso_scope.tb_i = (dso_scope.tb_i + 1) % TIMEBASE_NR;
		dso_scope.timebase_flag = 1;
	}
	
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
	//ADC_ExternalTrigConvCmd(ADC1, ENABLE);
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
	  DMA_InitTypeDef DMA_InitStructure;

	  DMA_DeInit(DMA1_Channel1);
	  DMA_InitStructure.DMA_PeripheralBaseAddr = ADC1_DR_Address;
	  DMA_InitStructure.DMA_MemoryBaseAddr = wave.samples;
	  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
	  DMA_InitStructure.DMA_BufferSize = SAMPLES_NR;
	  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
	  DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	  DMA_Init(DMA1_Channel1, &DMA_InitStructure);  
	 
	  /* Enable DMA Stream Transfer Complete interrupt */
	  DMA_ITConfig(DMA1_Channel1, DMA_IT_TC, ENABLE);

}

#define TIM2_FREQ_HZ	36000000
#define TIM2_FREQ_MHZ	36

void TIM3_Configuration(void)
{
	  TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;

	  /* Time base configuration */
	  TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
	  TIM_TimeBaseStructure.TIM_Period = timebase_pres[dso_scope.tb_i] - 1;
	  TIM_TimeBaseStructure.TIM_Prescaler = 0;
	  //TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	  TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

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
 

void sampling_config(void) {
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

void sampling_enable(void) {
	dso_scope.done_sampling = 0;
	dso_scope.prev_cal_samp = ADC_MAX;	
	/* Increase timer frequency */
	TIM_SetCounter(TIM3, 40);
	/* Enable ADC timer */
	TIM_Cmd(TIM3, ENABLE);
	/* Enable ADC interrupts */
	ADC_ITConfig(ADC1, ADC_IT_EOC , ENABLE);
}
