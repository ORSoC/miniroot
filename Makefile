KERNEL_SOURCE=../linux
KERNEL_BUILD=$(KERNEL_SOURCE)/build
CROSS_COMPILE=or32-linux-
KERNEL_ARCH=openrisc
UBOOT_ARCH=or1k
export CROSS_COMPILE ARCH

all: initramfs.img

mrproper: src/mrproper clean

.PHONY: modules

ifdef KERNEL_MODULES
KERNELRELEASE = $(shell $(MAKE) -s -C $(KERNEL_BUILD) CROSS_COMPILE=$(CROSS_COMPILE) ARCH=$(KERNEL_ARCH) kernelrelease)
KERNEL_BUILD = $(KERNEL_MODULES)/build
KERNEL_SOURCE = $(KERNEL_MODULES)/source
endif
modules: 
	rm -rf modules
	mkdir -p modules
ifndef KERNEL_MODULES
	$(warning please define KERNEL_MODULES to where the kernel modules are installed)
	sleep 5
else
	$(info kernel version $(KERNELRELEASE))
	mkdir -p modules/lib/modules/$(KERNELRELEASE)
	sh -ec "cd $(KERNEL_MODULES);find * -depth -print0 | cpio -0pdm $(PWD)/modules/lib/modules/$(KERNELRELEASE)"
	find modules/lib/modules/$(KERNELRELEASE) -name '*.ko' | xargs $(CROSS_COMPILE)strip -g
endif

initramfs.gz: initramfs.cpio
	gzip -9 <initramfs.cpio >initramfs.gz

initramfs.lzo: initramfs.cpio
	lzop -9 -o initramfs.lzo initramfs.cpio

initramfs.img: initramfs.gz
	mkimage -T ramdisk -A $(UBOOT_ARCH) -C none -d $< initramfs.img

initramfs.cpio: modules src/install
	( cat initramfs.devnodes ; sh $(KERNEL_SOURCE)/scripts/gen_initramfs_list.sh -u squash -g squash initramfs-bin/ initramfs/ modules/ ) | \
	$(KERNEL_BUILD)/usr/gen_init_cpio - >initramfs.cpio

initramfs.cpio-asis:
	( cat initramfs.devnodes ; sh $(KERNEL_SOURCE)/scripts/gen_initramfs_list.sh -u squash -g squash initramfs-bin/ initramfs/ modules/ ) | \
	$(KERNEL_BUILD)/usr/gen_init_cpio - >initramfs.cpio

clean:
	rm -rf modules initramfs-bin initramfs.cpio initramfs.gz initramfs.lzo initramfs.img

clean: src/clean

src/%:
	$(MAKE) -C src $*

