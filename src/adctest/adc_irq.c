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
	
	while(1) {
		int n;
		fd_set in;
		uint32_t irq_cnt;
		uint32_t irq_sources;
		uint32_t enable = 1;
		FD_ZERO(&in);
		FD_SET(adc_fd, &in);
		
		printf("Waiting for interrupt\n");

		n = select(adc_fd, &in, NULL, NULL, NULL);
		if (n < 0) {
			perror("select:");
			exit(1);
		}

		/* Check which IRQ sources have triggered */
		irq_sources = adc_read(ADC_REG_INTERRUPT);
		n = read(adc_fd, &irq_cnt, sizeof(irq_cnt));
		if (n == sizeof(irq_cnt)) {
			printf("Got %d interrupts, sources %x\n", irq_cnt, irq_sources);
		}

		/* Clear IRQ sources. */
		/* Note: In real application this should only clear the IRQ
		   source that the application handles
		 */
		adc_write(ADC_REG_INTERRUPT, irq_sources);

		/* Enable IRQ again */
		write(adc_fd, &enable, sizeof(enable));
	}
}
