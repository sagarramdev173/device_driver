obj-m += driver.o

KERN_DIR := /mnt/kernel


all:
	make -C $(KERN_DIR) M=$(PWD) modules

clean:
	make -C $(KERN_DIR) M=$(PWD) clean
	sudo rmmod $(TARGET)
