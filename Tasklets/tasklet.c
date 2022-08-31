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

#define static_tasklet 0
#define Dynamic_tasklet 0
#define IRQ_NO 11

#if static_tasklet == 1
    void tasklet_fn(unsigned long data){
        printk(KERN_INFO "i'm in tasklet function with value =%ld\n",data);
    }    
    DECLARE_TASKLET(tasklet,tasklet_fn,1);
    
#endif

#if Dynamic_tasklet ==1
    void tasklet_fn(unsigned long data){
        printk(KERN_INFO "i'm in tasklet function with value =%ld\n",data);
    }

    /* Tasklet by Dynamic Method */
    struct tasklet_struct *tasklet=NULL;

#endif
dev_t my_dev=0;
static struct cdev my_cdev;
static struct class *my_class;



static irqreturn_t irq_handler(int irq,void *dev_id){
    printk(KERN_INFO "i'm in irq handler by device \n");
    
    #if static_tasklet == 1
        tasklet_schedule(&tasklet); 
    #endif
    #if Dynamic_tasklet == 1
        tasklet_schedule(tasklet); 
    #endif
    return IRQ_HANDLED;
}
static int demo_open(struct inode *inode, struct file *file){
    printk(KERN_INFO "Device has been opened...\n");
    return 0;
}

static int demo_release(struct inode *inode, struct file *file){
    printk(KERN_INFO "Device has been released...\n");
    return 0;
}

static ssize_t demo_write(struct file *filp,const char __user *buf, size_t len, loff_t *off){
    printk(KERN_INFO "Device write has been started...\n");
    return 0;
}

static ssize_t demo_read(struct file *filp,char __user *buf, size_t len, loff_t *off){
    printk(KERN_INFO "Device read has been started...\n");
    asm("int $0x3B");  // Corresponding to irq 11 (0x20(first external vector) + 0x10(irq 10 vlaue) + 11)
    return 0;
}

//file operations registeration
static struct file_operations fops =
{
        .owner          = THIS_MODULE,
        .read           = demo_read,
        .write          = demo_write,
        .open           = demo_open,
        .release        = demo_release,
};

static int __init driver_start(void){
    if(alloc_chrdev_region(&my_dev,0,1,"demo_device")<0){
        printk(KERN_INFO "Cannont allocate device number\n");
    }
    printk(KERN_INFO "Major = %d Minor = %d \n",MAJOR(my_dev), MINOR(my_dev));

    cdev_init(&my_cdev,&fops);

    if(cdev_add(&my_cdev,my_dev,1)<0){
        printk(KERN_INFO "adding device info to cdev failed\n");
        goto r_region;
    }
    if((my_class =(class_create(THIS_MODULE,"demo_device")))== NULL){
        printk(KERN_INFO "creating device class failed\n");
        goto r_cdev;
    }

    if (device_create(my_class,NULL,my_dev,NULL,"demo_device") ==NULL){
        printk(KERN_INFO "device creation failed\n");
        goto r_class;
    }
    if(request_irq(IRQ_NO,irq_handler,IRQF_SHARED,"demo_device",(void*)(irq_handler))){
        printk(KERN_INFO "irq handler registration failed\n");
        goto r_device;
    }
    #if Dynamic_tasklet == 1
        tasklet=kmalloc(sizeof(struct tasklet_struct),GFP_KERNEL);
        if(tasklet==NULL){
            printk(KERN_INFO "tasklet memory allocation failed\n");
            goto irq;
        }
        tasklet_init(tasklet,tasklet_fn,1);
    #endif
    printk(KERN_INFO "deivce registered successfully\n");
    return 0;
irq:
    free_irq(IRQ_NO,(void*)(irq_handler));
r_device:
    device_destroy(my_class,my_dev);
r_class:
    class_destroy(my_class);
r_cdev:
    cdev_del(&my_cdev);
r_region:
    unregister_chrdev_region(my_dev,1);

    return -1;

}
static void __exit driver_end(void){

    free_irq(IRQ_NO,(void *)(irq_handler));
    device_destroy(my_class,my_dev);
    class_destroy(my_class);
    cdev_del(&my_cdev);
    unregister_chrdev_region(my_dev,1);
    printk(KERN_INFO "i'm done removing your device\n");
}

module_init(driver_start);
module_exit(driver_end);
MODULE_LICENSE("GPL");