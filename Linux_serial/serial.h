#ifndef SERIAL_H
#define SERIAL_H

#include <stdint.h>

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

#define WF_SENDING 		0x09

/* ADC parameters */
#define ADC_RES			4096
#define MAXV			3300
#define DIV_MULT		12
#define SAMPLES_NR 		300


void get_waveform(int fd, uint16_t * waveform);
void uart_get16(int fd, uint16_t *ptr);
int open_serial_port(char* portname);
int set_interface_attribs(int fd, int speed);
uint16_t adc_to_mv(uint16_t adc_val, uint16_t mv_max);
uint16_t calc_samp_int(uint16_t tb);
uint16_t adc_arr_to_mv(uint16_t *arr, uint16_t mv_max, uint16_t size);

#endif
