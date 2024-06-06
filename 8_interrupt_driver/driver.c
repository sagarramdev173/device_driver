// TODO: This driver is currently not useful it crashes need to fix 

#define DRIVER_PREFIX "demo_driver: " 
#define pr_fmt(fmt) DRIVER_PREFIX  fmt

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include<linux/slab.h>   
#include<linux/uaccess.h>
#include<linux/sysfs.h> 
#include<linux/kobject.h>
#include <linux/interrupt.h>
#include <asm/io.h>
#include <linux/workqueue.h>
#include <linux/interrupt.h>
#include <asm/hw_irq.h>


// preprocessor to make work easy
#define INIT_FUNC static int __init
#define EXIT_FUNC static void __exit
#define SUCCESS 0;
#define FAILURE -1;

// information required for device 
dev_t dev=0;
static struct cdev demo_cdev;
static struct class *dev_class;

// driver file operations
static int my_open(struct inode *inode, struct file *file){
    pr_info("device has been opened\n");
    return SUCCESS
}

static int my_release(struct inode *inode, struct file *file){
    pr_info("device has been released\n");
    return SUCCESS
}

static ssize_t my_read(struct file *filp, char __user *buf, size_t len, loff_t *off){
    struct irq_desc *desc ;
    pr_info("device read has been started\n");
    /*
        So if we want to raise an interrupt IRQ11, programmatically we have to add 11 to a vector of IRQ0.
        0x20 + 0x10 + 11 = 0x3B (59 in Decimal).
        Hence, executing “asm("int $0x3B")“, will raise interrupt IRQ 11.
    */
    // desc  = irq_to_desc(11);
    // if(desc == NULL){
    //     pr_info("IRQ not found\n");
    //     return FAILURE
    // }
    // __this_cpu_write(vector_irq[59],desc);
    // asm("int $0x3B");
    asm("int $0x50");
    return SUCCESS
}
static ssize_t my_write(struct file *filp, const char __user *buf, size_t len, loff_t *off){
    pr_info("device write has been started\n");
    return SUCCESS
}

// file operation structure
static struct file_operations fops = {
    .owner = THIS_MODULE,
    .open = my_open,
    .release = my_release,
    .read = my_read,
    .write = my_write
};
// Interrupt Handler
# define MY_IRQ_NO 12
static irqreturn_t demo_irq_handler(int irq, void *dev_id){
    pr_info("Shared IRQ: Interrupt Occurred");
    return IRQ_HANDLED;
}
static int irq_requested = 0; // Flag to track IRQ allocation

INIT_FUNC driver_start(void){
    pr_info("device is being inserted\n");

    // allocating the major number
    if(alloc_chrdev_region(&dev,0,1,"demo_device")<0){
        pr_info("Cannot allocate major number\n");
        return FAILURE
    }
    pr_info("major =%d mindor =%d", MAJOR(dev),MINOR(dev));
    
    //creating cdev structure
    cdev_init(&demo_cdev,&fops);

    // adding cdev to the system
    if(cdev_add(&demo_cdev,dev,1)<0){
        pr_info("cannot add the device to the system\n");
        goto r_class;
    }

    // creating device class
    dev_class= class_create(THIS_MODULE,"demo_class");
    if(dev_class == NULL){
        pr_info("class creation failed\n");
        goto r_class;
    }

    //setting up device node for normal user
    if(device_create(dev_class,NULL,dev,NULL,"demo_device") == NULL){
        pr_info("cannot create device file\n");
        goto r_device;
    }
   
    if(request_irq(MY_IRQ_NO, demo_irq_handler, IRQF_PROBE_SHARED, "demo_driver", NULL)){
        pr_info("cannot register IRQ \n");
        goto free_my_irq;
    }
    irq_requested = 1; // Mark IRQ as requested successfully
    pr_info("device has been inserted \n");
    return SUCCESS
free_my_irq:
    free_irq(MY_IRQ_NO, NULL);  
r_device:
    class_destroy(dev_class);
r_class:
    unregister_chrdev_region(dev,1);
    cdev_del(&demo_cdev);
    return FAILURE
}


EXIT_FUNC driver_end(void){
    pr_info("device is being removed\n");
     if (irq_requested) {
        free_irq(MY_IRQ_NO, &demo_cdev);
        irq_requested = 0;
    }
    device_destroy(dev_class,dev);
    class_destroy(dev_class);
    unregister_chrdev_region(dev,1);
    cdev_del(&demo_cdev);   
    pr_info("device has been removed\n");
}

module_init(driver_start);
module_exit(driver_end);
MODULE_LICENSE("GPL");