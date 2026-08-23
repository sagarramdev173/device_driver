TARGET = hello_world
obj-m +=$(TARGET).o

KDIR= /lib/modules/$(shell uname -r)/build


all:
	make -C $(KDIR) M=$(PWD) modules

clean:
	make -C $(KDIR) M=$(PWD) clean

install:
	insmod $(TARGET).ko

remove:
	rmmod $(TARGET)
