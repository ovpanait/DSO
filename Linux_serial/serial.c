#include <unistd.h>
#include "serial.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>

void wf_printf(FILE *plot_file, uint16_t *waveform, uint16_t size, double interval);
void plot_wf(void);

int main(int argc, char *argv[])
{
	char *commands[] = { "plus\n", "minus\n", "sel\n", "ok\n", "wave\n", 0 };
	uint8_t codes[] = { SERIAL_PLUS, SERIAL_MINUS, SERIAL_SEL, SERIAL_SINGLE, SERIAL_SEND_WF };

	uint8_t comm_code;
	uint16_t waveform[300];
	
	int32_t fd = open_serial_port("/dev/ttyUSB0");
	set_interface_attribs(fd, 115200);
	
	char command_buf[128];
	uint8_t wr_code = SERIAL_SEL;
	
	uint8_t ret;
	uint16_t tb_us;

	while(1) {
		uint8_t i = 0;
		
		if(!fgets(command_buf, 128, stdin)) {
			perror("fgets");
			exit(EXIT_FAILURE);
		}

		char **cmd_ptr = commands;
		while(*cmd_ptr != NULL){
			if(!strcmp(*cmd_ptr, command_buf))
				break;
			cmd_ptr++;
			++i;
		}
		
		if(!*cmd_ptr) {
			printf("Invalid command.\n");
			continue;
		}
		wr_code = codes[i];
		do {
			ret = write(fd, &wr_code, 1);
			if(ret == -1) {
				perror("write");
				exit(EXIT_FAILURE);
			}
		
			ret = read(fd, &comm_code, 1);
			if(ret == -1) {
				perror("read");
				exit(EXIT_FAILURE);

			}
		} while(comm_code == RESEND);

		if(comm_code == WF_SENDING){
			FILE *plot_file = fopen("plot.dat", "w");
			if(plot_file == NULL) {
				perror("fopen");
				exit(EXIT_FAILURE);
			}

			uart_get16(fd, &tb_us);
			printf("%d\n",tb_us);
			get_waveform(fd, waveform);
			
			adc_arr_to_mv(waveform, MAXV, SAMPLES_NR);
			/*for(int i = 0; i < 300; i++)
				printf("%d\n", waveform[i]);*/
			wf_printf(plot_file, waveform, SAMPLES_NR, calc_samp_int(tb_us));
			fclose(plot_file);
			plot_wf();
		}
	}
	
}


int open_serial_port(char* portname)
{
	int fd = open(portname, O_RDWR | O_NOCTTY | O_SYNC);
    if (fd < 0) {
	    perror("open");
	    exit(EXIT_FAILURE);
    }
	return fd;
}

int set_interface_attribs(int fd, int speed)
{
	struct termios tty;

	if (tcgetattr(fd, &tty) < 0) {
		perror("tcgetattr");
		exit(0);
	}

	cfsetospeed(&tty, (speed_t)speed);
	cfsetispeed(&tty, (speed_t)speed);

	tty.c_cflag |= (CLOCAL | CREAD);    /* ignore modem controls */
	tty.c_cflag &= ~CSIZE;

	tty.c_cflag |= CS8;         /* 8-bit characters */
	tty.c_cflag &= ~PARENB;     /* no parity bit */
	tty.c_cflag &= ~CSTOPB;     /* only need 1 stop bit */
	/*tty.c_cflag |= CRTSCTS;  */  /* no hardware flowcontrol */

	/* setup for non-canonical mode */
	tty.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL | IXON);
	tty.c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
	tty.c_oflag &= ~OPOST;

	/* fetch bytes as they become available */
	tty.c_cc[VMIN] = 1;
	tty.c_cc[VTIME] = 100;

	if (tcsetattr(fd, TCSANOW, &tty) != 0) {
		perror("tcsetattr");
		exit(0);
	}
	return 0;
}

void uart_get16(int fd, uint16_t *ptr) 
{
	uint8_t res_8;
	int16_t ret;

	ret = read(fd, &res_8, 1);
	if(ret == -1) {
		perror("read");
		exit(EXIT_FAILURE);
	}
	*ptr = res_8;
	
	ret = read(fd, &res_8, 1);
	if(ret == -1) {
		perror("read");
		exit(EXIT_FAILURE);
	}
	*ptr += (res_8 << 8);	
	
}

void get_waveform(int fd, uint16_t * waveform)
{
	for(uint16_t i = 0; i < 300; ++i) {
		uart_get16(fd, waveform + i);
		//printf("%d\n", waveform[i]);
	}
}

double calc_samp_int(uint16_t tb)
{
	return ((double)DIV_MULT * tb) / SAMPLES_NR;
}

uint16_t adc_to_mv(uint16_t adc_val, uint16_t mv_max)
{
	return ((double)mv_max / (ADC_RES)) * adc_val;
}

uint16_t adc_arr_to_mv(uint16_t *arr, uint16_t mv_max, uint16_t size)
{
	for(uint16_t i = 0; i < size; ++i)
		*(arr + i) = adc_to_mv(*(arr + i), mv_max);
}

void wf_printf(FILE *plot_file, uint16_t *waveform, uint16_t size, double interval)
{
	for(uint16_t i = 0; i < size; ++i)
		fprintf(plot_file, "%f\t%d\n", i * interval, waveform[i]);
}

/* Plot waveform using gnuplot script */
void plot_wf(void)
{
	int32_t child_exits;

	char *args[] = { "gnuplot" , "-p", "plot_script", NULL};

	int32_t child_pid = fork();
	switch(child_pid) {
	case -1:
		perror("fork");
		exit(2);
	case 0:
		if(execvp("gnuplot", args) == -1){
			perror("execvp");
			exit(2);
		}
		break;
	default:
		waitpid(child_pid, &child_exits, 0);
		break;
	}
}
