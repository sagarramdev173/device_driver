obj-m += driver.o

KDIR := /mnt/kernel

all:
	make -C $(KDIR) M=$(PWD) modules

clean:
	make -C $(KDIR) M=$(PWD) clean
