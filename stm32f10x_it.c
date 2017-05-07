/**
  ******************************************************************************
  * @file    FSMC/SRAM/stm32f10x_it.c 
  * @author  MCD Application Team
  * @version V3.3.0
  * @date    04/16/2010
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @copy
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2010 STMicroelectronics</center></h2>
  */ 

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_it.h"

#include "Common.h"
#include "Board.h"
#include "scope.h"

extern __IO struct scope dso_scope;
extern __IO struct waveform wave;
extern __IO TIM_TimeBaseInitTypeDef TIM3_struct;
extern U16 timebase_pres[];

/** @addtogroup STM32F10x_StdPeriph_Examples
  * @{
  */

/** @addtogroup FSMC_SRAM
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M3 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSV_Handler exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
/*void SysTick_Handler(void)
{
 // Key scan
 if(--TimerKeyScan == 0) {
	TimerKeyScan = 10;	// 10ms
	BitSet(Keypad.Flags, (1 << KF_DoKeyScan));
 	}

 // General timer
 if(GTimer) {
	GTimer--;
	if(GTimer == 0) {
		GTimeout = 1;
		}
 	}
}*/

/******************************************************************************/
/*                 STM32F10x Peripherals Interrupt Handlers                  		*/
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  	*/
/*  available peripheral interrupt handler's name please refer to the startup 	*/
/*  file (startup_stm32f10x_xx.s).                                            			*/
/******************************************************************************/

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval No	ne
  */
/*void PPP_IRQHandler(void)
{
}*/

void DMA1_Channel1_IRQHandler(void)
{
	/* Test on DMA Stream Transfer Complete interrupt */
	if(DMA_GetITStatus(DMA_IT_TC)) {
	/* Clear DMA Transfer Complete interrupt pending bit */
	DMA_ClearITPendingBit(DMA_IT_TC);

	/* Disable DMA Channel while the waveform is displayed */
	DMA_Cmd(DMA1_Channel1, DISABLE);
	TIM_Cmd(TIM3, DISABLE);

	if(dso_scope.timebase == 20) {
		for(U16 i = 0; i < SAMPLES_NR; i += 2)
			if(dso_scope.avg_flag)
				wave.avg_buf[i] = wave.avg_buf[i + 1] = wave.tmp_buf[i / 2];
			else 
				wave.avg_buf[i] = wave.avg_buf[i + 1] = (wave.avg_buf[i] + wave.tmp_buf[i / 2]) / 2;
	} else if(dso_scope.timebase == 10 ) {
		for(U16 i = 0; i < SAMPLES_NR; i += 4)
			if(dso_scope.avg_flag)
				wave.avg_buf[i] = wave.avg_buf[i + 1] = wave.avg_buf[i+2] = wave.avg_buf[i+3] = wave.tmp_buf[i / 4];
			else 
				wave.avg_buf[i] = wave.avg_buf[i + 1] = wave.avg_buf[i+2] = wave.avg_buf[i+3] = (wave.avg_buf[i] + wave.tmp_buf[i / 4]) / 2;
	}
	else {
		for(U16 i = 0; i < SAMPLES_NR; ++i)
			if(dso_scope.avg_flag)
				wave.avg_buf[i] = wave.tmp_buf[i];
			else 
				wave.avg_buf[i] = (wave.avg_buf[i] + wave.tmp_buf[i]) / 2;
	}

	if(dso_scope.avg_flag)
		dso_scope.avg_flag = 0;

	if(dso_scope.done_displaying || !(--dso_scope.avg_total))
		dso_scope.done_sampling = 1;
	else {
		/* Start searching for a new trigger */
		TIM3_struct.TIM_Period = 50;
		TIM_TimeBaseInit(TIM3, &TIM3_struct);
		TIM_SelectOutputTrigger(TIM3, TIM_TRGOSource_Update); // ADC_ExternalTrigConv_T3_TRGO
		TIM_Cmd(TIM3, ENABLE);
		
		/* ADC Interrupts */
		ADC_ITConfig(ADC1, ADC_IT_EOC , ENABLE);
	}

	/* Reset calibration sample */
	dso_scope.prev_cal_samp = ADC_MAX;
  }
}

/*void TIM4_IRQHandler(void)
{
	TIM_ClearITPendingBit (TIM4, TIM_IT_Update);
	if(dso_scope.start_sampling){
		dso_scope.start_sampling = 0;
		/* Start sampling */
  		//TIM_Cmd(TIM3, ENABLE);
		/* DMA1_Channel1 enable */
  		/*DMA_Cmd(DMA1_Channel1, ENABLE);
	}
}*/

void TIM3_IRQHandler(void)
{
	//TIM_ClearITPendingBit (TIM3, TIM_IT_Update);
	//dso_scope.done_sampling = 1;
}

void ADC1_2_IRQHandler(void)
{
	ADC_ClearITPendingBit(ADC1, ADC_IT_EOC);
	if((ADC1->DR > (dso_scope.trig_lvl_adc + NOISE_MARGIN) && 
			dso_scope.prev_cal_samp < dso_scope.trig_lvl_adc )) {
	/* Disable ADC Interrupts - the sampling can start now */
	ADC_ITConfig(ADC1, ADC_IT_EOC , DISABLE);
		
	/* Reload timer with proper parameters */
	TIM_Cmd(TIM3, DISABLE);
	TIM3_struct.TIM_Period = timebase_pres[dso_scope.tb_i] - 1;
	TIM_TimeBaseInit(TIM3, &TIM3_struct);
	TIM_SelectOutputTrigger(TIM3, TIM_TRGOSource_Update); // ADC_ExternalTrigConv_T3_TRGO
	TIM_Cmd(TIM3, ENABLE);
	
	/* Start averaging */
	dso_scope.avg_flag = 1;

	/* Enable DMA on channel 1 (ADC1) */
	DMA_Cmd(DMA1_Channel1, ENABLE);
	}
	dso_scope.prev_cal_samp = ADC1->DR;
		
}

/******************* (C) COPYRIGHT 2010 STMicroelectronics *****END OF FILE****/
