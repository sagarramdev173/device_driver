obj-m += spinlock_demo.o
spinlock_demo-objs := driver.o
driver_name=spinlock_demo

KDIR := /lib/modules/$(shell uname -r)/build

all:
	scl enable gcc-toolset-11 -- make -C $(KDIR) M=$(PWD) modules

clean:
	make -C $(KDIR) M=$(PWD) clean

install: all
	sudo insmod $(driver_name).ko

uninstall:
	sudo rmmod $(driver_name)