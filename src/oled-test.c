#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

//function set
#define	DL	0x10
#define FT0	0x1
#define FT1	0x2
//function set

//display on/off
#define	D	0x4
#define C	0x2
#define B	0x1
//display on/off

//entry mode set
#define ID	0x2
#define S	0x1
//entry mode set

//cursor/display shift
#define	SC	0x8
#define	RL	0x4
//cursor/display shift

//Leading bits
#define DO_CMD   ((0<<1)|(0<<0))
#define DO_BUSY  ((0<<0)|(0<<1))
#define DO_WRITE ((1<<1)|(0<<0))
#define DO_READ  ((1<<1)|(0<<1))


int spi_channel;

//functions
///////////

void init_spi_oled(const char *devname);

void write_instruction(int ten_bits);

void check_busy_flag(void);
void function_set(int data_byte);
void display_on_off(int data_byte);
void clear_display(void);
void entry_mode_set(int data_byte);
void return_home(void);
void set_ddram_address(int data_byte);
void write_data(int data_byte);
void cursor_display_shift(int data_byte);

void write_str(int start_address, char* str);

int main(int argc, char **argv)
{		
	//init spi oled
	init_spi_oled(argv[1]);

	//init oled
	function_set(DL|FT1|FT0);
	check_busy_flag();
	
	display_on_off(0x0);
	check_busy_flag();

	clear_display();
	check_busy_flag();

	entry_mode_set(ID);
	check_busy_flag();

	return_home();
	check_busy_flag();

	display_on_off(D);
	check_busy_flag();

	//print
	write_data('H');
	check_busy_flag();

	write_data(0x65);
	check_busy_flag();

	char* str = "llo World!";

	write_str(0x40, str);
}

//functions
///////////
void init_spi_oled(const char *devname)
{
	int arg;
	spi_channel = open(devname, O_RDWR);
}

void check_busy_flag(void){
	char type_busy = DO_BUSY;
	char busy_response[16];
	int is_busy;
	struct spi_ioc_transfer spi_xfr[] = {
		{
		.tx_buf = (unsigned long)&type_busy,
		.rx_buf = (unsigned long)NULL,
		.len = 1,
		.speed_hz = 0,
		.bits_per_word = 2,
		.delay_usecs = 0,
		.cs_change = 0,
		},
		{
		.tx_buf = (unsigned long)NULL,
		.rx_buf = (unsigned long)busy_response,
		.len = 16,
		.speed_hz = 0,
		.bits_per_word = 8,
		.delay_usecs = 0,
		.cs_change = 1,
		},
	};
	int err;

	do {
		err = ioctl(spi_channel, SPI_IOC_MESSAGE(2), spi_xfr);
		if (err) {
			perror("spi ioctl");
			printf("err=%d\n", err);
//			exit(1);
		}
printf("busy data: %02x 
		is_busy = 0;
	} while (is_busy);
	usleep(500);
}

void function_set(int data_byte){
	write_instruction(0x28 | data_byte);
}


void display_on_off(int data_byte){
	write_instruction(0x8 | data_byte);
}


void clear_display(void){
	write_instruction(0x1);
}

void entry_mode_set(int data_byte){
	write_instruction(0x4 | data_byte);
}

void return_home(void){
	write_instruction(0x2);
}

void set_ddram_address(int data_byte){
	write_instruction(0x80 | data_byte);
}

void write_data(int data_byte){
	write_instruction(0x200 | data_byte);
}

void cursor_display_shift(int data_byte){
	write_instruction(0x10 | data_byte);
}

void write_instruction(int ten_bits){
	uint16_t data = ten_bits;
	struct spi_ioc_transfer spi_xfr[] = {
		{
		.tx_buf = (unsigned long)&data,
		.rx_buf = (unsigned long)NULL,
		.len = 2,
		.speed_hz = 0,
		.bits_per_word = 10,
		.delay_usecs = 0,
		.cs_change = 1,
		},
	};
	int err;

	err = ioctl(spi_channel, SPI_IOC_MESSAGE(1), spi_xfr);
	if (err < 0) {
		perror("spi ioctl");
		printf("err=%d\n", err);
//		exit(1);
	}
}

void write_str(int start_address, char* str){
	set_ddram_address(start_address);
	while( *str != '\0' ){
		write_data(*(str++));
		check_busy_flag();	
	}
}
