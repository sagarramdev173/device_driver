TARGET = driver
TEST = test

obj-m += $(TARGET).o

KERN_DIR = "/lib/modules/$(shell uname -r)/build"


all:
	make -C $(KERN_DIR) M=$(PWD) modules
	gcc -o $(TEST) $(TEST).c

clean:
	make -C $(KERN_DIR) M=$(PWD) clean
	rm -f $(TEST)
	
install:
	sudo insmod $(TARGET).ko

remove:
	sudo rmmod $(TARGET)

