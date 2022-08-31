#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include<linux/slab.h>                 //kmalloc()
#include<linux/uaccess.h>              //copy_to/from_user()
#include<linux/sysfs.h> 
#include<linux/kobject.h> 
#include <linux/interrupt.h>
#include <asm/io.h>
#include <linux/workqueue.h>            // Required for workqueues

//flags to handle the different type of queues
#define static_queue 0
#define dynamic_queue 0
#define own_queue 0

#define IRQ_NO 11

dev_t dev=0;
static struct cdev demo_cdev;
static struct class *dev_class;

void workqueue_fn(struct work_struct *work){
    printk(KERN_INFO "i'm doing your queued work\n");
    return;
}

#if static_queue == 1
/*Creating work by Static Method */
DECLARE_WORK(myworkqueue,workqueue_fn);
#endif

#if dynamic_queue == 1
/* Work structure */
static struct work_struct myworkqueue;
#endif

#if own_queue == 1
    static struct workqueue_struct *own_workqueue;
    static DECLARE_WORK(mywork,workqueue_fn);
#endif

static irqreturn_t irq_handler(int irq,void *dev_id){
    printk(KERN_INFO "i'm handling the irq\n");
    
    #if static_queue == 1 || dynamic_queue == 1
        schedule_work(&myworkqueue);
    #endif
    #if own_queue == 1
        queue_work(own_workqueue,&mywork);
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
    if(alloc_chrdev_region(&dev,0,1,"demo_device")<0){
        printk(KERN_INFO "Cannot allocate major number\n");
        return -1;
    }
    printk(KERN_INFO "major =%d mindor =%d", MAJOR(dev),MINOR(dev));
    
    cdev_init(&demo_cdev,&fops);

    if(cdev_add(&demo_cdev,dev,1)<0){
        printk(KERN_INFO "cannot add the device to the system\n");
    }

    if ( (dev_class= class_create(THIS_MODULE,"demo_class") ) ==NULL){
        printk(KERN_INFO "cannot create class\n");        
    }

    if (device_create(dev_class,NULL,dev,NULL,"demo_device") == NULL){
        printk(KERN_INFO "cannot create device file\n");
    }

    if(request_irq(IRQ_NO, irq_handler, IRQF_SHARED, "demo_device",(void*)irq_handler)){
        printk(KERN_INFO "cannot regirster irq \n");
    }

    #if own_queue == 1
    /*Creating workqueue */
    own_workqueue = create_workqueue("own_wq");
    #endif

    #if dynamic_queue == 1
    /*Creating work by Dynamic Method */
        INIT_WORK(&myworkqueue,workqueue_fn);
    #endif

    printk(KERN_INFO " everything went fine \n");
    return 0;

}
static void __exit driver_end(void){
    free_irq(IRQ_NO,(void*)irq_handler);
    device_destroy(dev_class,dev);
    class_destroy(dev_class);
    cdev_del(&demo_cdev);
    unregister_chrdev_region(dev,1);
    printk(KERN_INFO "Device rmeoved successfully\n");
}

module_init(driver_start);
module_exit(driver_end);
MODULE_LICENSE("GPL");