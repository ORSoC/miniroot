CC=$(CROSS_COMPILE)gcc
CFLAGS=-g -Wall -W -Werror -std=gnu99 $(DEFINES)
BUSYBOX_VERSION=1.21.0
MINISNMP_VERSION=1.2b
DROPBEAR_VERSION=2013.58
I2CTOOLS_VERSION=3.1.0
SPITEST_VERSION=1.0
FSNAME=initramfs-bin
RELEASEDIR=../../images/
BOARD=geopebble
export CROSS_COMPILE

PROGRAMS= busybox
all: $(PROGRAMS)

clean: 
	rm -f $(PROGRAMS) *.o

.PHONY: all clean

####
# Busybox
##

.PHONY: busybox 

busybox: busybox-$(BUSYBOX_VERSION)/busybox

busybox-$(BUSYBOX_VERSION)/busybox:	busybox-$(BUSYBOX_VERSION)/.config 
	$(MAKE) -C busybox-$(BUSYBOX_VERSION)

busybox-$(BUSYBOX_VERSION)/.config: busybox.config busybox-$(BUSYBOX_VERSION)/.unpacked
	cp busybox.config busybox-$(BUSYBOX_VERSION)/.config

busybox-$(BUSYBOX_VERSION)/.unpacked: busybox-$(BUSYBOX_VERSION).tar.bz2 patches/busybox/*
	tar jxf busybox-$(BUSYBOX_VERSION).tar.bz2
	for patch in patches/busybox/*.patch; do \
		if [ -f "$patch" ]; then \
			echo "$$patch"; \
			patch -p1 -d busybox-$(BUSYBOX_VERSION) < $$patch; \
		fi; \
	done
	touch busybox-$(BUSYBOX_VERSION)/.unpacked

clean: clean_busybox
clean_busybox:
	rm -rf busybox-$(BUSYBOX_VERSION)

busybox-$(BUSYBOX_VERSION).tar.bz2:
	wget http://www.busybox.net/downloads/busybox-$(BUSYBOX_VERSION).tar.bz2

install: busybox_install
busybox_install: busybox-$(BUSYBOX_VERSION)/busybox
	$(MAKE) -C busybox-$(BUSYBOX_VERSION)  CONFIG_PREFIX=$(shell pwd)/../$(FSNAME)/ install

####
# mini_snmpd
##

.PHONY: mini_snmpd

mini_snmpd: mini_snmpd/mini_snmpd

mini_snmpd/mini_snmpd: mini_snmpd/.unpacked
	$(MAKE) -C mini_snmpd OFLAGS="-O2 -D__IPV4__ -static" CC=$(CC)

mini_snmpd/.unpacked: mini_snmpd-$(MINISNMP_VERSION).tar.gz patches/mini_snmpd/*
	tar zxf mini_snmpd-$(MINISNMP_VERSION).tar.gz
	for patch in patches/mini_snmpd/*.patch; do \
		if [ -f "$patch" ]; then \
			echo "$$patch"; \
			patch -d mini_snmpd < $$patch; \
		fi; \
	done
	touch mini_snmpd/.unpacked

clean: clean_mini_snmpd
clean_mini_snmpd:
	rm -rf mini_snmpd

mini_snmpd-$(MINISNMP_VERSION).tar.gz:
	wget http://members.aon.at/linuxfreak/linux/mini_snmpd-$(MINISNMP_VERSION).tar.gz

####
# dropbear (incomplete)
##

dropbear: dropbear-$(DROPBEAR_VERSION)/.unpacked

dropbear-$(DROPBEAR_VERSION)/.unpacked: dropbear-$(DROPBEAR_VERSION).tar.bz2
	tar jxf dropbear-$(DROPBEAR_VERSION).tar.bz2
	touch dropbear-$(DROPBEAR_VERSION)/.unpacked

clean: clean_dropbear
clean_dropbear:
	rm -rf dropbear-$(DROPBEAR_VERSION)

dropbear-$(DROPBEAR_VERSION).tar.bz2:
	wget https://matt.ucc.asn.au/dropbear/releases/dropbear-$(DROPBEAR_VERSION).tar.bz2:
	

####
# i2c-tools
##

.PHONY: i2c-tools 

i2c-tools: i2c-tools-$(I2CTOOLS_VERSION)/all

i2c-tools-$(I2CTOOLS_VERSION)/all: i2c-tools-$(I2CTOOLS_VERSION)/.unpacked
	$(MAKE) -C i2c-tools-$(I2CTOOLS_VERSION) CC=$(CC) 'CFLAGS=$(CFLAGS)' 'LDFLAGS=-static' prefix=/ all

i2c-tools-$(I2CTOOLS_VERSION)/.unpacked: i2c-tools-$(I2CTOOLS_VERSION).tar.bz2 patches/i2c-tools/*
	tar jxf i2c-tools-$(I2CTOOLS_VERSION).tar.bz2
	for patch in patches/i2c-tools/*.patch; do \
		if [ -f "$patch" ]; then \
			echo "$$patch"; \
			patch -p1 -d i2c-tools-$(I2CTOOLS_VERSION) < $$patch; \
		fi; \
	done
	touch i2c-tools-$(I2CTOOLS_VERSION)/.unpacked

clean: clean_i2c_tools
clean_i2c_tools:
	rm -rf i2c-tools-$(I2CTOOLS_VERSION)

i2c-tools-$(I2CTOOLS_VERSION).tar.bz2:
	wget http://dl.lm-sensors.org/i2c-tools/releases/i2c-tools-$(I2CTOOLS_VERSION).tar.bz2

install: i2c-tools_install
i2c-tools_install: i2c-tools-$(I2CTOOLS_VERSION)/all
	$(MAKE) -C i2c-tools-$(I2CTOOLS_VERSION)  CC=$(CC) 'CFLAGS=$(CFLAGS)' 'LDFLAGS=-static' DESTDIR=$(shell pwd)/../$(FSNAME)/ prefix=/ install
	$(CROSS_COMPILE)strip $(shell pwd)/../$(FSNAME)/sbin/i2cset
	$(CROSS_COMPILE)strip $(shell pwd)/../$(FSNAME)/sbin/i2cget
	$(CROSS_COMPILE)strip $(shell pwd)/../$(FSNAME)/sbin/i2cdetect
	$(CROSS_COMPILE)strip $(shell pwd)/../$(FSNAME)/sbin/i2cdump
	rm -f $(shell pwd)/../$(FSNAME)/share/man/man8/i2c*
	rmdir $(shell pwd)/../$(FSNAME)/share/man/man8
	rmdir $(shell pwd)/../$(FSNAME)/share/man
	rmdir $(shell pwd)/../$(FSNAME)/share
	rm -f $(shell pwd)/../$(FSNAME)/include/linux/i2c-dev.h
	rmdir $(shell pwd)/../$(FSNAME)/include/linux
	rmdir $(shell pwd)/../$(FSNAME)/include
	rm -f $(shell pwd)/../$(FSNAME)/bin/decode-dimms
	rm -f $(shell pwd)/../$(FSNAME)/bin/decode-vaio
	rm -f $(shell pwd)/../$(FSNAME)/bin/decode-edid
	rm -f $(shell pwd)/../$(FSNAME)/bin/ddcmon

####
# oled-test
##

oled-test: oled-test.c
	$(CC) $(CFLAGS) -o $@ $<

install: oled-test_install
oled-test_install: oled-test ../$(FSNAME)/bin
	$(CROSS_COMPILE)strip oled-test -o $(shell pwd)/../$(FSNAME)/bin/oled-test

clean: clean_oled-test
clean_oled-test:
	rm -f oled-test

####
# spi-test
##

.PHONY: spi-test

spi-test:	spi-test-$(SPITEST_VERSION)/.unpacked
	$(MAKE) -C spi-test-$(SPITEST_VERSION)

spi-test-$(SPITEST_VERSION)/.unpacked: spi-test-$(SPITEST_VERSION).tar.gz
	tar zxf spi-test-$(SPITEST_VERSION).tar.gz
	touch spi-test-$(SPITEST_VERSION)/.unpacked

clean: clean_spi-test
clean_spi-test:
	rm -rf spi-test-$(SPITEST_VERSION)

spi-test-$(SPITEST_VERSION).tar.gz:
	wget -O spi-test-$(SPITEST_VERSION).tar.gz https://github.com/ORSoC/spi-test/archive/$(SPITEST_VERSION).tar.gz

install: spi-test_install
spi-test_install: spi-test ../$(FSNAME)/bin
	$(CROSS_COMPILE)strip spi-test-$(SPITEST_VERSION)/spi-test -o $(shell pwd)/../$(FSNAME)/bin/spi-test


#####################################################################

.PHONY: adctest
adctest:
	$(MAKE) CC=$(CC) -C adctest

clean: clean_adctest
clean_adctest:
	$(MAKE) CC=$(CC) -C adctest clean

install: adctest_install
adctest_install: 
	$(MAKE) CC=$(CC) CROSS_COMPILE=$(CROSS_COMPILE) DESTDIR=$(shell pwd)/../$(FSNAME)/ prefix=/ -C adctest install


#####################################################################

install: ../$(FSNAME)

installfs:
	$(MAKE) FSNAME=installfs do_installfs

# Never call this directly. Use installfs above
do_installfs: $(PROGRAMS) busybox_install ../$(FSNAME)
	mkdir -p ../$(FSNAME)/install
	tar -C ../$(FSNAME)/install -zxvf $(RELEASEDIR)/$(BOARD).img

INITRAMFS_DIRS=../$(FSNAME)/mnt ../$(FSNAME)/proc ../$(FSNAME)/sys ../$(FSNAME)/bin ../$(FSNAME)/sbin ../$(FSNAME)/tmp

../$(FSNAME): $(INITRAMFS_DIRS) ../$(FSNAME)/usr ../$(FSNAME)/init

$(INITRAMFS_DIRS):
	mkdir -p $@

../$(FSNAME)/usr:
	ln -s . $@

../$(FSNAME)/init:
	ln -s sbin/init $@

cleaninstallfs:
	$(MAKE) FSNAME=installfs cleaninitramfs

cleaninitramfs:	
	find ../$(FSNAME) -type l -print | xargs rm -f
	rm -f ../$(FSNAME)/bin/busybox
	rm -f ../$(FSNAME)/install/mtd*
	rm -f ../$(FSNAME)/install/run*
	rmdir ../$(FSNAME)/* 2>/dev/null || true


realclean mrproper: cleaninitramfs cleaninstallfs clean
	rm -f *.tar.*
