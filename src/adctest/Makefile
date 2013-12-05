CFLAGS=-g
CC=$(CROSS_COMPILE)gcc
PROGRAMS = adc_enable_irq adc_irq adc_trigger_irq adc_sample adc_regs

all: $(PROGRAMS)

clean:
	rm -f *.o $(PROGRAMS)

install: all
	@for program in $(PROGRAMS); do \
		$(CROSS_COMPILE)strip $$program -o $(DESTDIR)/bin/$$program; \
	done

