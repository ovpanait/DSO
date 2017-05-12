#include <unistd.h>
#include "serial.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>

int open_serial_port(char* portname){
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

int main(int argc, char *argv[])
{
	char *commands[] = { "plus\n", "minus\n", "sel\n", "ok\n", 0 };
	uint8_t codes[] = { SERIAL_PLUS, SERIAL_MINUS, SERIAL_SEL, SERIAL_SINGLE };
	uint8_t comm_code;
	
	int32_t fd = open_serial_port("/dev/ttyUSB0");
	set_interface_attribs(fd, 115200);
	
	char command_buf[128];
	uint8_t wr_code = SERIAL_SEL;
	
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
			uint8_t ret = write(fd, &wr_code, 1);
			if(ret == -1) {
				perror("write");
				exit(EXIT_FAILURE);
			}
		
			ret = read(fd, &comm_code, 1);
			if(ret == -1) {
				perror("read");
				exit(EXIT_FAILURE);

			}
			sleep(1);
		} while(comm_code == RESEND);	
	}
	
}
