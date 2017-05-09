#ifndef SCOPE_H
#define SCOPE_H

#include "stm32f10x.h"

#include "Common.h"
#include "Screen.h"

#define SAMPLES_NR		300
#define BLK_MV			1000					/* Milivolts in one block */
#define ADC_TOTAL		4096					/* 12 bit ADC */
#define ADC_MAX 		4095
#define MAX_VAL_MV		3300					/* Max measurable value */
#define MV_PIXEL		(BLK_MV / BLK_PX)			/* Milivolts per pixel */
#define ADC_VAL_DEL		((MV_PIXEL * ADC_TOTAL) / MAX_VAL_MV)	/* ADC value delimiter */

/* Convert ADC value to pixels */
#define GET_SAMPLE(x)		( (x) / ADC_VAL_DEL) 
#define ADC_MV_PER_DIV		0.8

/* Noise */
#define NOISE_MARGIN		40	/* ~32mv */

/* Debouncing parameters */
#define DEBOUNCE_TOTAL		250
#define DEBOUNCE_LIM		100

/* Buttons */
#define BTN_PORT		GPIOB

#define PLUS_BTN_PIN		GPIO_Pin_14
#define PLUS_BTN_BIT		0

#define MINUS_BTN_PIN		GPIO_Pin_15
#define MINUS_BTN_BIT		1

#define SEL_BTN_PIN		GPIO_Pin_13
#define SEL_BTN_BIT		2

/* Timebase */
#define TIMEBASE_NR		8 /* Number of existing timebases */

/* Frequency */
#define GET_FREQ(freq_cnt)	(1000000.0 / (((U32)dso_scope.timebase * 12000 / SAMPLES_NR) * (freq_cnt)))

struct waveform {
	__IO U16 tmp_buf[SAMPLES_NR];
	__IO U16 avg_buf[SAMPLES_NR];
	__IO U16 display_buf[SAMPLES_NR];
	
	U8 midpoint;
	double frequency;
	U16 max;
	U16 min;
	U16 pp_v;
};

struct scope {
	/* Real-time/Trigger mode */
	__IO U8 rt_mode;
	__IO U16 rt_timer ;

	/* Timebase */
	__IO U8 tb_i;
	__IO U16 timebase;

	/* Interrupt flags */
	__IO U8 done_sampling;
	__IO U8 done_displaying;

	/* Averaging */
	__IO U8 avg_flag;
	__IO U8 avg_total;

	/* ADC Trigger level */
	__IO U16 trig_lvl_adc;
	__IO U16 test_timer;
	U16 prev_cal_samp; 		/* Previous sample */

	/* Buttons */
	U8 debounced;
	U8 btns_flags;
};

/* Display functions */
void waveform_display(void);
void timebase_display(U16 timebase);
void voltage_display(U16 posx, U16 posy, U8 *label, U16 adc_val, U16 text_clr, U16 bg_clr);
void freq_display(double freq);
void get_digits(U32 n, U8 *dig_buf);

/* Initialization */
void scope_init(void);
void waveform_init(void);
void ADCs_Init(void);
void DMA_Configuration(void);
void TIM3_Configuration(void);
void TIM4_Configuration(void);
void NVIC_Configuration(void);

/* Sampling */
void sampling_config(void); /* Configure ADC1, DMA, TIM3 */
void sampling_enable(void);
void fill_display_buf(void);

/* Buttons */
U8 read_btns(void);
#endif
