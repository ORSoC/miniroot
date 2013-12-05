#include <stdio.h>
#include <linux/uio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/select.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>

#include "adc.h"

char *adc_base;

uint32_t adc_read(int reg)
{
	return *(uint32_t *)(adc_base + reg);
}

void adc_write(int reg, uint32_t value)
{
	*(uint32_t *)(adc_base + reg) = value;
}

void adc_dump_regs(void)
{
	printf("ADC_REG_INTERRUPT: 0x%x\n", adc_read(ADC_REG_INTERRUPT));
	printf("ADC_REG_DRDYOUT_N_CNT: 0x%x\n", adc_read(ADC_REG_DRDYOUT_N_CNT));
	printf("ADC_REG_PPS_CNT: 0x%x\n", adc_read(ADC_REG_PPS_CNT));
	printf("ADC_REG_INTERRUPT_ENABLE: 0x%x\n", adc_read(ADC_REG_INTERRUPT_ENABLE));
	printf("ADC_REG_ADC: 0x%x\n", adc_read(ADC_REG_ADC));
	printf("ADC_REG_GPS: 0x%x\n", adc_read(ADC_REG_GPS));
	printf("ADC_REG_WIFI: 0x%x\n", adc_read(ADC_REG_WIFI));
	printf("ADC_REG_MUX: 0x%x\n", adc_read(ADC_REG_MUX));
}

int adc_parse_reg(const char *name)
{
	if (isdigit(*name)) {
		return strtol(name, NULL, 0);
	}
	if (strcmp(name, "INTERRUPT") == 0) return ADC_REG_INTERRUPT;
	if (strcmp(name, "DRDYOUT_N_CNT") == 0) return ADC_REG_DRDYOUT_N_CNT;
	if (strcmp(name, "PPS_CNT") == 0) return ADC_REG_PPS_CNT;
	if (strcmp(name, "INTERRUPT_ENABLE") == 0) return ADC_REG_INTERRUPT_ENABLE;
	if (strcmp(name, "ADC") == 0) return ADC_REG_ADC;
	if (strcmp(name, "GPS") == 0) return ADC_REG_GPS;
	if (strcmp(name, "WIFI") == 0) return ADC_REG_WIFI;
	if (strcmp(name, "MUX") == 0) return ADC_REG_MUX;

	fprintf(stderr, "ERROR: Unknown register %s\n", name);
	exit(1);
}

int main(int argc, char **argv)
{
	int adc_fd = open("/dev/uio0", O_RDWR, O_NONBLOCK);
	if (adc_fd < 0) {
		perror("Failed to open uio handle");
		exit(1);
	}

	adc_base = mmap(NULL, 0x400, PROT_READ|PROT_WRITE, MAP_SHARED, adc_fd, 0);
	if (adc_base == MAP_FAILED) {
		perror("Failed to mmap uio handle");
		exit(1);
	}
	
	if (argc == 1) {
		adc_dump_regs();
		exit(0);
	}

	if (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0) {
		printf("Usage: %s [regname [value]]\n", argv[0]);
		exit(0);
	}

	if (argc == 2) {
		printf("0x%x", adc_read(adc_parse_reg(argv[1])));
		exit(0);
	}
	if (argc == 3) {
		adc_write(adc_parse_reg(argv[1]), strtol(argv[2], NULL, 0));
		exit(0);
	}

	fprintf(stderr, "ERROR! See --help for usage\n");
	exit(1);
}
