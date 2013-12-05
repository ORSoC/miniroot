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
	
	printf("ADC_REG_INTERRUPT: 0x%x\n", adc_read(ADC_REG_INTERRUPT));
	printf("ADC_REG_DRDYOUT_N_CNT: 0x%x\n", adc_read(ADC_REG_DRDYOUT_N_CNT));
	printf("ADC_REG_PPS_CNT: 0x%x\n", adc_read(ADC_REG_PPS_CNT));
	printf("ADC_REG_INTERRUPT_ENABLE: 0x%x\n", adc_read(ADC_REG_INTERRUPT_ENABLE));
	printf("ADC_REG_ADC: 0x%x\n", adc_read(ADC_REG_ADC));
	printf("ADC_REG_GPS: 0x%x\n", adc_read(ADC_REG_GPS));
	printf("ADC_REG_WIFI: 0x%x\n", adc_read(ADC_REG_WIFI));
	printf("ADC_REG_MUX: 0x%x\n", adc_read(ADC_REG_MUX));
}
