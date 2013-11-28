#include <stdio.h>
#include <linux/uio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/select.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>
#include <sys/ioctl.h>
#include <unistd.h>


#include "adc.h"

char *adc_base;
int adcctrl_fd;
int adcspi_fd;

uint32_t adc_read(int reg)
{
	return *(uint32_t *)(adc_base + reg);
}

void adc_write(int reg, uint32_t value)
{
	*(uint32_t *)(adc_base + reg) = value;
}

void set_muxes(int setting)
{
	adc_write(ADC_REG_MUX, setting);
}

void pga_setup(int pga, int input_vos_trim, int gain)
{
	static const char dev[4][16] = {
		"/dev/spidev0.0",
		"/dev/spidev0.1",
		"/dev/spidev0.2",
		"/dev/spidev0.3"
	};

	int pga_fd = open(dev[pga], O_RDWR);
	if (pga_fd < 0) {
		perror("Failed to open pga handle");
		exit(1);
	}

	uint8_t reg_vos = (input_vos_trim << 1) | 0;
	uint8_t reg_gain = (gain << 1) | 1;
	struct spi_ioc_transfer tr[2] = {
		{
		.tx_buf = (uint64_t)&reg_vos,
		.len = 1,
		.cs_change = 1,
		},
		{
		.tx_buf = (uint64_t)&reg_gain,
		.len = 1,
		.cs_change = 1,
		}
	};

	int ret = ioctl(pga_fd, SPI_IOC_MESSAGE(2), &tr);
	if (ret < 0) {
		perror("can't send PGA SPI message");
		exit(1);
	}
	close(pga_fd);
}

void pga_setup_all(int input_vos_trim, int gain)
{
	int i;
	for (i = 0; i < 4; i++)
		pga_setup(0, input_vos_trim, gain);
}

void open_adcctrl(void)
{
	adcctrl_fd = open("/dev/uio0", O_RDWR, O_NONBLOCK);
	if (adcctrl_fd < 0) {
		perror("Failed to open uio handle");
		exit(1);
	}

	adc_base = mmap(NULL, 0x400, PROT_READ|PROT_WRITE, MAP_SHARED, adcctrl_fd, 0);
	if (adc_base == MAP_FAILED) {
		perror("Failed to mmap uio handle");
		exit(1);
	}
}

static void open_adcspi(void)
{
	adcspi_fd = open("/dev/spidev1.0", O_RDWR);
	if (adcspi_fd < 0) {
		perror("Failed to open ADC SPI handle");
		exit(1);
	}
}

void write_configuration_register_adc(uint8_t value)
{
	uint8_t address = (0 << 7) | 0x60;
	struct spi_ioc_transfer tr[2] = {
		{
		.tx_buf = (uint64_t)&address,
		.len = 1,
		},
		{
		.tx_buf = (uint64_t)&value,
		.len = 1,
		}
	};

	int ret = ioctl(adcspi_fd, SPI_IOC_MESSAGE(2), &tr);
	if (ret < 0) {
		perror("can't send ADC SPI message");
		exit(1);
	}
}

uint8_t read_configuration_register_adc(void)
{
	uint8_t address = (1 << 7) | 0x60;
	uint8_t value;
	struct spi_ioc_transfer tr[2] = {
		{
		.tx_buf = (uint64_t)&address,
		.len = 1,
		},
		{
		.rx_buf = (uint64_t)&value,
		.len = 1,
		}
	};

	int ret = ioctl(adcspi_fd, SPI_IOC_MESSAGE(2), &tr);
	if (ret < 0) {
		perror("can't send ADC SPI message");
		exit(1);
	}
	return value;
}

void write_data_rate_control_register_adc(uint16_t value)
{
	uint8_t address = (0 << 7) | 0x50;
	struct spi_ioc_transfer tr[2] = {
		{
		.tx_buf = (uint64_t)&address,
		.len = 1,
		},
		{
		.tx_buf = (uint64_t)&value,
		.len = 1,
		.bits_per_word = 16,
		}
	};

	int ret = ioctl(adcspi_fd, SPI_IOC_MESSAGE(2), &tr);
	if (ret < 0) {
		perror("can't send ADC SPI message");
		exit(1);
	}
}

uint16_t read_data_rate_control_register_adc(void)
{
	uint8_t address = (1 << 7) | 0x50;
	uint16_t value;
	struct spi_ioc_transfer tr[2] = {
		{
		.tx_buf = (uint64_t)&address,
		.len = 1,
		},
		{
		.rx_buf = (uint64_t)&value,
		.len = 1,
		.bits_per_word = 16,
		}
	};

	int ret = ioctl(adcspi_fd, SPI_IOC_MESSAGE(2), &tr);
	if (ret < 0) {
		perror("can't send ADC SPI message");
		exit(1);
	}
	return value;
}

void write_sampling_instant_control_register_adc(uint32_t value)
{
	uint8_t address = (0 << 7) | 0x40;
	struct spi_ioc_transfer tr[2] = {
		{
		.tx_buf = (uint64_t)&address,
		.len = 1,
		},
		{
		.tx_buf = (uint64_t)&value,
		.len = 1,
		.bits_per_word = 16,
		}
	};

	int ret = ioctl(adcspi_fd, SPI_IOC_MESSAGE(2), &tr);
	if (ret < 0) {
		perror("can't send ADC SPI message");
		exit(1);
	}
}

uint32_t read_sampling_instant_control_register_adc(void)
{
	uint8_t address = (0 << 7) | 0x40;
	uint32_t value;
	struct spi_ioc_transfer tr[2] = {
		{
		.tx_buf = (uint64_t)&address,
		.len = 1,
		},
		{
		.rx_buf = (uint64_t)&value,
		.len = 1,
		.bits_per_word = 16,
		}
	};

	int ret = ioctl(adcspi_fd, SPI_IOC_MESSAGE(2), &tr);
	if (ret < 0) {
		perror("can't send ADC SPI message");
		exit(1);
	}
	return value;
}

void read_data_register_adc(uint32_t result[4])
{
	uint8_t address = (0 << 7) | 0x70;
	char buf[4 * 3];
	struct spi_ioc_transfer tr[2] = {
		{
		.tx_buf = (uint64_t)&address,
		.len = 1,
		},
		{
		.rx_buf = (uint64_t)&buf,
		.len = 4 * 3, /* 96 bits */
		}
	};

	int ret = ioctl(adcspi_fd, SPI_IOC_MESSAGE(2), &tr);
	if (ret < 0) {
		perror("can't send ADC SPI message");
		exit(1);
	}
	/* Reformat result as 4 signed uint32_t words */
	int i;
	char *p = buf;
	for (i = 0; i < 4; i++, p += 3) {
		result[i] = (p[0] << 16) | ((uint8_t)p[1] << 8) | ((uint8_t)p[2]);
	}
}

void dummy_read_data_register_adc(void)
{
	uint32_t result[4];
	read_data_register_adc(result);
}

void read_sample_block_adc(void *data, size_t len)
{
	struct spi_ioc_transfer tr = {
		.rx_buf = (uint64_t)data,
		.len = len,
		.bits_per_word = 192, // CH0, CH1, CH2, CH3, #PPS, #DRDYOUT * 32
	};

	int ret = ioctl(adcspi_fd, SPI_IOC_MESSAGE(1), &tr);
	if (ret < 0) {
		perror("can't send ADC SPI message");
		exit(1);
	}
}

int main(int argc __attribute__((unused)), char **argv __attribute__((unused)))
{
	open_adcctrl();

	char buffer[10*6*4];

fprintf(stderr, "set_muxes\n");
	set_muxes(0);

	//0mV,  MEAS = 0, VOS = 0
	//1V/V, MEAS = 0, GAIN = 0
fprintf(stderr, "pga_setup\n");
	pga_setup_all(0, 0);

fprintf(stderr, "open_adcspi\n");
	open_adcspi();

fprintf(stderr, "write_conf\n");
	write_configuration_register_adc(1 << 6);                //set bit 6, RST

fprintf(stderr, "write_conf\n");
	write_configuration_register_adc(1 << 5);                //set bit 5, EN24BIT, clear bit 6 RST

fprintf(stderr, "write_si\n");
	write_sampling_instant_control_register_adc(0x0);       //0x0, all channels sample at the same time

fprintf(stderr, "write_rate\n");
	write_data_rate_control_register_adc(0x201a);           //20MHz/(( 128 *384)+( 26 *32))=~400.128Hz, FSAMPC = 001, FSAMPF = 11010

fprintf(stderr, "read_data\n");
	dummy_read_data_register_adc();

fprintf(stderr, "adc block\n");
	read_sample_block_adc(buffer, sizeof(buffer));

	write(2, buffer, sizeof(buffer));

	return 0;
}
