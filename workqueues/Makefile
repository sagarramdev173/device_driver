TARGET=queue
obj-m += $(TARGET).o

KDIR = /lib/modules/$(shell uname -r)/build


all:
	make -C $(KDIR) M=$(shell pwd) modules
clean:	
	make -C $(KDIR) M=$(shell pwd) clean
load:all
	insmod $(TARGET).ko
remove:clean
	rmmod $(TARGET).ko