TARGET = simple_character_driver

obj-m += $(TARGET).o

KDIR = /lib/modules/$(shell uname -r)/build

all:
	make -C $(KDIR) M=$(PWD) modules

clean:
	make -C $(KDIR) M=$(PWD) clean
	rm -f test

install:
	insmod $(TARGET).ko

remove:
	rmmod $(TARGET)

test:
	gcc -o test test.c
