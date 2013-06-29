#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

//Leading bits
#define DO_CMD   ((0<<1)|(0<<0))
#define DO_BUSY  ((0<<1)|(1<<0))
#define DO_WRITE ((1<<1)|(0<<0))
#define DO_READ  ((1<<1)|(1<<0))

//Instructions
#define CMD_CLEAR	(1<<0)
#define CMD_HOME	(1<<1)
#define CMD_ENTRY_MODE	(1<<2)		/* 1: cursor direction, 0: shift */
#define CMD_ONOFF	(1<<3)		/* 2: display, 1: cursor, 0: blink */
#define CMD_SHIFT	(1<<4)		/* 3: cursor/display, 2: direction */
#define CMD_FUNCTION	(1<<5)|(1<<3)	/* 4: interfacelength, 1,0: font */
#define CMD_CGRAM_ADDR	(1<<6)		/* address */
#define CMD_DDRAM_ADDR	(1<<7)		/* address */

#define BUSY_IS_BUSY	(0x80)
#define BUSY_ADDRESS	(0x7f)


int spi_channel;

//functions
///////////

void init_spi_oled(const char *devname);

void write_oled(int type, uint8_t bits);
void write_instruction(uint8_t bits);

void check_busy_flag(void);
void select_font(int data_byte);
void display_mode(int on, int cursor, int blink);
void clear_display(void);
void entry_mode_set(int decrement, int shift);
void return_home(void);
void set_ddram_address(int addr);
void set_cgram_address(int addr);
void write_data(uint8_t data);
void shift_cursor(int right);
void shift_display(int right);

void write_str(const char* str);

void check_busy_flag3(int bits);

void usage(void)
{
	printf("oled-test device commands...\n"
		"\t-t text	write text\n"
		"\t-b	check busy flag\n"
		"\t-f #	select font\n"
		"\t-m on/off cursor blink	set display mode\n"
		"\t-c	clear\n"
		"\t-g y x	goto\n"
		"\t-a X	Set DDRAM address\n"
		"\t-A X	Set CGRAM address\n"
		"\t-w X	Write byte\n"
		"\t-h	return home\n"
		"\t-C left/right	move cursor left/right\n"
		"\t-S left/right	shift display left/right\n"
	);
	exit(1);
}

int main(int argc, char **argv)
{		
	int check_busy = 0;
	if (argc < 3)
		usage();

	//init spi oled
	init_spi_oled(argv[1]);

	argc-=2;
	argv+=2;

	while (argc >= 1) {
		const char *cmd = argv[0];
		int args = 0;
		if (strcmp(cmd, "-B") == 0) {
			check_busy = 0;
			args = 1;
			check_busy_flag3(atoi(argv[1]));
		}
		if (strcmp(cmd, "-b") == 0)
			check_busy = 1;
		if (check_busy)
			check_busy_flag();
		if (strcmp(cmd, "-f") == 0) {
			select_font(atoi(argv[1]));
			args = 1;
		}
		if (strcmp(cmd, "-m") == 0) {
			display_mode(atoi(argv[1]), atoi(argv[2]), atoi(argv[3]));
			args = 3;
		}
		if (strcmp(cmd, "-c") == 0)
			clear_display();
		if (strcmp(cmd, "-g") == 0) {
			set_ddram_address(atoi(argv[1]) * 0x40 + atoi(argv[2]));
			args = 2;
		}
		if (strcmp(cmd, "-a") == 0) {
			set_ddram_address(strtoul(argv[1], NULL, 0));
			args = 1;
		}
		if (strcmp(cmd, "-A") == 0) {
			set_cgram_address(strtoul(argv[1], NULL, 0));
			args = 1;
		}
		if (strcmp(cmd, "-w") == 0) {
			write_data(strtoul(argv[1], NULL, 0));
			args = 1;
		}
		if (strcmp(cmd, "-h") == 0)
			return_home();
		if (strcmp(cmd, "-C") == 0) {
			shift_cursor(*argv[1] == 'r');
			args = 1;
		}
		if (strcmp(cmd, "-S") == 0) {
			shift_display(*argv[1] == 'r');
			args = 1;
		}
		if (strcmp(cmd, "-t") == 0) {
			write_str(argv[1]);
			args = 1;
		}
		if (*cmd != '-')
			write_str(cmd);
		check_busy = 1;
		argc -= 1 + args;
		argv += 1 + args;
	}
}

//functions
///////////
void init_spi_oled(const char *devname)
{
	spi_channel = open(devname, O_RDWR);
}

void check_busy_flag(void){
	uint32_t command = DO_BUSY << (8+8); /* dummy data + read 8 */
	uint32_t busy_reg;
	
	struct spi_ioc_transfer spi_xfr[] = {
		{
		.tx_buf = (unsigned long)&command,
		.rx_buf = (unsigned long)&busy_reg,
		.len = 4,
		.bits_per_word = 18,
		},
	};
	int err;

	do {
		usleep(600);
		err = ioctl(spi_channel, SPI_IOC_MESSAGE(1), spi_xfr);
		if (err < 0) {
			perror("spi ioctl");
			printf("err=%d\n", err);
			exit(1);
		}
	} while (busy_reg & BUSY_IS_BUSY);
}

void check_busy_flag3(int bits){
	uint32_t command = DO_BUSY << bits - 2;
	uint32_t busy_reg;
	
	struct spi_ioc_transfer spi_xfr[] = {
		{
		.tx_buf = (unsigned long)&command,
		.rx_buf = (unsigned long)&busy_reg,
		.len = 4,
		.bits_per_word = bits,
		},
	};
	int err;

	err = ioctl(spi_channel, SPI_IOC_MESSAGE(1), spi_xfr);
	if (err < 0) {
		perror("spi ioctl");
		printf("err=%d\n", err);
		exit(1);
	}
}

/* Commands */
void select_font(int font){
	write_instruction(CMD_FUNCTION | (1 << 4) | (font << 0));
}


void display_mode(int display, int cursor, int blink)
{
	write_instruction(CMD_ONOFF | (display << 2) | (cursor << 1) | (blink << 0));
}


void clear_display(void){
	write_instruction(CMD_CLEAR);
}

void entry_mode_set(int decrement, int shift)
{ 
	write_instruction(CMD_ENTRY_MODE | (decrement << 1) | (shift << 0));
}

void return_home(void){
	write_instruction(CMD_HOME);
}

void set_ddram_address(int addr){
	write_instruction(CMD_DDRAM_ADDR | addr);
}

void set_cgram_address(int addr){
	write_instruction(CMD_CGRAM_ADDR | addr);
}

void write_data(uint8_t data){
	write_oled(DO_WRITE, data);
}

void shift_cursor(int right)
{
	write_instruction(CMD_SHIFT | (0<<3) | (right << 2));
}

void shift_display(int right)
{
	write_instruction(CMD_SHIFT | (1<<3) | (right << 2));
}

void write_oled(int type, uint8_t bits){
	uint16_t data = type << 8 | bits;
	struct spi_ioc_transfer spi_xfr[] = {
		{
		.tx_buf = (unsigned long)&data,
		.len = 2,
		.bits_per_word = 10,
		},
	};
	int err;

	err = ioctl(spi_channel, SPI_IOC_MESSAGE(1), spi_xfr);
	if (err < 0) {
		perror("spi ioctl");
		printf("err=%d\n", err);
		exit(1);
	}
}

void write_instruction(uint8_t bits)
{
	write_oled(DO_CMD, bits);
}

void write_str(const char* str)
{
	uint8_t cmd = DO_WRITE;
	struct spi_ioc_transfer spi_xfr[] = {
		{
		.tx_buf = (unsigned long)&cmd,
		.len = 1,
		.bits_per_word = 2,
		},
		{
		.tx_buf = (unsigned long)str,
		.len = strlen(str),
		.bits_per_word = 8,
		},
	};
	int err;

	err = ioctl(spi_channel, SPI_IOC_MESSAGE(2), spi_xfr);
	if (err < 0) {
		perror("spi ioctl");
		printf("err=%d\n", err);
		exit(1);
	}
}
