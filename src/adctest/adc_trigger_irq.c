#include <stdio.h>
#include <linux/uio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/select.h>
#include <stdlib.h>
#include <sys/mman.h>

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
	
	/* Trigger DRDYOUT_N IRQ */
	adc_write(ADC_REG_INTERRUPT_ENABLE, ADC_IRQ_ADC_DRDYOUT_N);
	adc_write(ADC_REG_ADC, ADC_ADC_TEST_ENABLE_DRDYOUT_N_IRQ_TEST | ADC_ADC_TEST_DRDYOUT_N_IRQ_TEST);
	adc_write(ADC_REG_ADC, 0);
}
