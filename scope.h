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

#define MINUS_BTN_PIN		GPIO_Pin_13
#define MINUS_BTN_BIT		1

#define SEL_BTN_PIN		GPIO_Pin_12
#define SEL_BTN_BIT		2

#define OK_BTN_BIT		3
#define OK_BTN_PIN		GPIO_Pin_15

#define TB_BIT			4
#define LCURSOR_BIT		5
#define RCURSOR_BIT		6

#define SINGLES_BIT		7
#define ANALYZING_BIT		8
#define SS_STARTED_BIT		9
#define SS_CAPTURED_BIT		10

#define SEL_NR			4

typedef enum {
	l_cursor = 0,
	r_cursor,
	tb,
	mode
} selected;

/* Timebase */
#define TIMEBASE_NR		8 /* Number of existing timebases */

/* Frequency */
#define GET_FREQ(freq_cnt)	(1000000.0 / (((U32)dso_scope.timebase * 12000 / SAMPLES_NR) * (freq_cnt)))
#define FREQ_DELAY		15

/* USART Flags */
#define ACK			0x01
#define NEED_ACK		0x02
#define	RESEND			0x03
#define RX_DONE 		0x04
#define RX_WAITING		0x05

/* USART Commands */
#define SERIAL_SEL		0x04
#define SERIAL_PLUS		0x05
#define SERIAL_MINUS		0x06
#define SERIAL_SINGLE		0x07
#define SERIAL_SEND_WF		0x08
#define WF_SENDING		0x09

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
	/* USART1 receive buffer */
	__IO U16 RX_command;
	__IO U8 RX_flag;

	/* Real-time/Trigger mode */
	__IO U8 rt_mode;
	__IO U16 rt_timer ;

	/* Timebase */
	__IO S8 tb_i;
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
	__IO U8 debounced;
	__IO U16 btns_flags;
	__IO U8 btn_selected;
};

/* Display functions */
void waveform_display(void);
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
void read_btns(void);
void btns_update(void);
U8 check_btn(GPIO_TypeDef* GPIOx, U16 GPIO_pin, U8 state);

/* USART1 */
void USART1_set_flags(void);

#endif
