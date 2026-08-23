TARGET=driver
obj-m += $(TARGET).o

KDIR = /lib/modules/$(shell uname -r)/build


all:
	make -C $(KDIR) M=$(shell pwd) modules
clean:	
	make -C $(KDIR) M=$(shell pwd) clean
load:all
	sudo insmod $(TARGET).ko
remove:clean
	sudo rmmod $(TARGET).ko
