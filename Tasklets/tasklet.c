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

dev_t my_dev=0;


static struct cdev my_cdev;
static struct class *my_class;

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
    

}
static void __exit driver_end(void){

}

module_init(driver_start);
module_exit(driver_end);
MODULE_LICENSE("GPL");