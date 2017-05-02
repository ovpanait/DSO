#include "stm32f10x.h"

#include "scope.h"
#include "Screen.h"
#include "Board.h"
#include "stdlib.h"

struct scope dso_scope;
struct waveform wave;

void scope_init(void)
{
	dso_scope.done_sampling = 0;
	dso_scope.timebase = 1000;
}

void waveform_init(void) 
{
	U16 i;
	for(i = 0; i< WD_WIDTH / 2; ++i)
		wave.samples[i] = 0;

	wave.midpoint = WD_MIDY;
}

void waveform_display(void)
{
	U8 buf[5];
	U16 xpos = 10;
	U16 i = 0, current_ypos = 0, prev_ypos = 0;
	S16 diff = 0;
	U16 midpoint = wave.midpoint;
	ClrScreen();

	/* Display first sample */
	prev_ypos = GET_SAMPLE(wave.samples[i++]);
	FillRect(xpos++, midpoint - prev_ypos, 2, 2, WF_CL);

	/* Display the rest */
	for(; i < SAMPLES_NR; ++i, ++xpos) {
		current_ypos = GET_SAMPLE(wave.samples[i]);
		diff = current_ypos - prev_ypos;
		if(diff > 1 && diff <= WD_HEIGHT / 2 )
			FillRect(xpos, midpoint - current_ypos, 2, 2 + diff, WF_CL);
		else if((diff < -1 && diff >= -(WD_HEIGHT / 2)))
			FillRect(xpos, midpoint - prev_ypos, 2, 2 - diff, WF_CL);
		else
			FillRect(xpos, midpoint - current_ypos, 2, 2, WF_CL);
		prev_ypos = current_ypos;
	}
	PutsGenic(5, 5, (U8 *)itoa(dso_scope.timebase, buf, 10), clWhite, clBlack, &ASC8X16);
}

/* void waveform_test()
{
	U16 i =0;
	for(;i < 40; ++i) 
		wave->samples[i] = i;
	for(;i; --i)
		wave->samples[40 + i] = i;
	
}
*/

/*
void waveform_sampling(void)
{
	U16 i;
	U16 read_res;
	U16 current_sample_val;

	char buf[16];
	
	for(i = 0; i < SAMPLES_NR; ++i) {
		read_res = ADC_Poll(ADC1, 4); 
		
		itoa(read_res, buf, 10);
		uputs(buf, USART1);
		UartPutc('\n', USART1);
		
		current_sample_val = wave->samples[i].val;
		if(current_sample_val == read_res)
			wave->samples[i].exist = 1;
 		
		wave->samples[i].new_val = GET_SAMPLE(read_res);
	}
}
*/

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
  ADC_RegularChannelConfig(ADC1, ADC_Channel_4, 1, ADC_SampleTime_13Cycles5);
 
  //ADC_ITConfig(ADC1, ADC_IT_EOC , ENABLE);
  //ADC_ExternalTrigConvCmd(ADC1, ENABLE);
  ADC_Cmd(ADC1, ENABLE);
  ADC_DMACmd(ADC1, ENABLE);
  /* Enable ADC1 */
  //ADC_Cmd(ADC1, ENABLE);
 
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

void TIM3_Configuration(void)
{
  TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
 
  /* Time base configuration */
  TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
  TIM_TimeBaseStructure.TIM_Period = ( TIM2_FREQ_HZ / 256461) - 1;
  TIM_TimeBaseStructure.TIM_Prescaler = 0;
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

  /* TIM3 TRGO selection */
  TIM_SelectOutputTrigger(TIM3, TIM_TRGOSource_Update); // ADC_ExternalTrigConv_T3_TRGO 
	//TIM_ITConfig (TIM3, TIM_IT_Update, ENABLE);
 
  /* TIM3 enable counter */
  TIM_Cmd(TIM3, ENABLE);
}
 
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
	
  /*NVIC_InitStructure.NVIC_IRQChannel = ADC1_2_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);*/
}
 
/**************************************************************************************/
 

void start_sampling(void) {
 
  	TIM3_Configuration();
	
	/* TIM4_Init(); */
	
	ADCs_Init();

	DMA_Configuration();

	NVIC_Configuration();
 
  	
 
  	
}

/*
	*
	*	ADC1 - DMA1 END
	*
*/
