#ifndef SCOPE_H
#define SCOPE_H

#include "stm32f10x.h"

#include "Common.h"
#include "Screen.h"

#define SAMPLES_NR		300
#define BLK_MV			1000					/* Milivolts in one block */
#define ADC_TOTAL		4096					/* 12 bit ADC */
#define MAX_VAL_MV		3300					/* Max measurable value */
#define MV_PIXEL		(BLK_MV / BLK_PX)			/* Milivolts per pixel */
#define ADC_VAL_DEL		((MV_PIXEL * ADC_TOTAL) / MAX_VAL_MV)	/* ADC value delimiter */
#define GET_SAMPLE(x)		( (x) / ADC_VAL_DEL) 

struct sample {
	U16 val;
	U16 new_val;
	U8 exist;
};

struct waveform {
	__IO U16 samples[SAMPLES_NR];
	U8 midpoint;
	U16 frequency;
	U16 peak_to_peak;
	U16 max;
	U16 min;
	U16 pp_v;
};

struct scope {
	U16 timebase;
	U16 timebase_ch;
	__IO U8 done_sampling;
};

void waveform_display(void);
void waveform_init(void);
void waveform_test(void);
void waveform_sampling(void);
void scope_init(void);
void ADCs_Init(void);
void DMA_Configuration(void);
void TIM3_Configuration(void);
void NVIC_Configuration(void);
void start_sampling(void);

#endif
