obj-m := flydigi_vader3.o
flydigi_vader3-objs := hid-flydigi-vader3.o

KDIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

all:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

install:
	$(MAKE) -C hid-vader3
	sudo ./uninstall.sh
	sudo ./install.sh
	sudo modprobe hid-vader3

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean

mr-clean:
	sudo ./uninstall.sh
	$(MAKE) -C $(KDIR) M=$(PWD) clean
