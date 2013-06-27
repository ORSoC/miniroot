
//tCYS Min 300ns => 3,33...MHz

//SPI MODE 3
//CPOL = 1, CPHA = 1 

#include "cpu-utils.h"

#define OLED_SPI_BASE		0xb2000000

//SPI MASTER SLAVE
//////////////////
#define RX0	0x00
#define RX1	0x04
#define RX2	0x08
#define RX3	0x0c
#define TX0	0x00
#define TX1	0x04
#define TX2	0x08
#define TX3	0x0c
#define CTRL	0x10
#define DIVIDER 0x14
#define SS	0x18
//CTRL
#define ASS	0x2000
#define IE	0x1000		//interrupt enable
#define LSB	0x800
#define TX_NEG	0x400		//mosi changed falling edge
#define RX_NEG	0x200		//miso latched falling edge
#define GO_BSY	0x100
//USER
#define CHAR_LEN_10	0xa
#define CHAR_LEN_20	0x14
#define CHAR_LEN_CLEAR 	0xffffff80
///////////////////
//SPI MASTER SLAVE

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

//functions
///////////

void init_spi_oled(int divider);

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

int main()
{		
	//init spi oled
	init_spi_oled(0x7);	//50MHz/((1+ 0x7 )*2) = 3,125MHz

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

	while(1);
}

//functions
///////////
void init_spi_oled(int divider){
	REG32( OLED_SPI_BASE + CTRL ) = ASS | RX_NEG | CHAR_LEN_20;
	REG32( OLED_SPI_BASE + DIVIDER ) = divider;//16 bit divider, system_freq/((1+divider)*2) = spi_freq
	REG32( OLED_SPI_BASE + SS ) = 0x1;
}

void check_busy_flag(void){
	int valid_data;
	REG32( OLED_SPI_BASE ) &= CHAR_LEN_CLEAR;
	REG32( OLED_SPI_BASE ) |= CHAR_LEN_20;
	do{
		REG32( OLED_SPI_BASE + TX0 ) = 0x40100;
		REG32( OLED_SPI_BASE + CTRL ) |= GO_BSY;
		while( REG32( OLED_SPI_BASE + CTRL ) & GO_BSY );
		valid_data = REG32( OLED_SPI_BASE + RX0 ) & 0x200;
	}
	while( valid_data );
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
	REG32( OLED_SPI_BASE ) &= CHAR_LEN_CLEAR;
	REG32( OLED_SPI_BASE ) |= CHAR_LEN_10;
	REG32( OLED_SPI_BASE + TX0 ) = ten_bits;
	REG32( OLED_SPI_BASE + CTRL ) |= GO_BSY;
	while( REG32( OLED_SPI_BASE + CTRL ) & GO_BSY );
}

void write_str(int start_address, char* str){
	set_ddram_address(start_address);
	while( *str != '\0' ){
		write_data(*(str++));
		check_busy_flag();	
	}
}
