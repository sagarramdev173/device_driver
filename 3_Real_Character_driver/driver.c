#define DRIVER_PREFIX "demo_driver: "
#define pr_fmt(fmt) DRIVER_PREFIX  fmt

#include <linux/module.h>
#include <linux/fs.h>
#include <linux/kernel.h>
#include <linux/cdev.h>
#include <linux/slab.h>     //kmalloc
#include <linux/uaccess.h>  //copy_to/from_user


MODULE_LICENSE("GPL");

#define SUCCESS 0
#define FAILURE -1
#define DEV_NAME "demo_driver"

#define MEMSIZE 1024

// For device number
dev_t mydev = 0;

// For file operation structure
static struct  cdev demo_cdev;

//For device class
static struct class *demo_class;

//For kernel allocated memory 
char *kernel_buffer;

// Function prototype for file operation
static int demo_open(struct inode *inode, struct file *file);
static int demo_release(struct inode *inode, struct file *file);
static ssize_t demo_read(struct file *filp, char __user *buf, size_t len, loff_t *off);
static ssize_t demo_write(struct file *filp, const char __user *buf, size_t len, loff_t *off);

//For file operation
static struct file_operations fops = {
    .owner = THIS_MODULE,
    .read = demo_read,
    .write = demo_write,
    .open = demo_open,
    .release = demo_release
};

static int demo_open(struct inode *inode, struct file *file){
    pr_info("device has been opened\n");
    return SUCCESS;
}

static int demo_release(struct inode *inode, struct file *file){
    pr_info("device has been released\n");
    return SUCCESS;
}

static ssize_t demo_read(struct file *filp, char __user *buf, size_t len, loff_t *off){
    unsigned long ret = 0;
    pr_info("device read has been started\n");
    ret = copy_to_user(buf, kernel_buffer, MEMSIZE);
    if(ret != SUCCESS){
        pr_info("copy to user failed\n");
        return FAILURE;
    }    
    pr_info("device read has been completed\n");
    return MEMSIZE;
}

static ssize_t demo_write(struct file *filp, const char __user *buf, size_t len, loff_t *off){
    unsigned long ret = 0;
    pr_info("device write has been started\n");
    ret = copy_from_user(kernel_buffer, buf, MEMSIZE);
    if(ret != SUCCESS){
        pr_info("copy from user failed\n");
        return FAILURE;
    }
    pr_info("data from user is %s\n", kernel_buffer);
    pr_info("device write has been completed\n");
    return MEMSIZE;
}

// For device class setting up  the mode so that normal user can access device node directly
static char * demo_device_devnode(struct device *dev, umode_t *mode){
    if(mode)
        *mode = 0666;
    pr_info("device node has been created\n");
    return NULL;
}

static int __init demo_driver_init(void){
    pr_info("starting up your driver\n");

    // allocating the device number
    if (alloc_chrdev_region(&mydev, 0, 1, DEV_NAME) < SUCCESS){
        pr_info("cannot allocate device number\n");
        return FAILURE;
    }

    pr_info("major = %d minor = %d\n", MAJOR(mydev), MINOR(mydev));

    // initializing the cdev structure
    cdev_init(&demo_cdev, &fops);
    
    // associating the cdev to the device
    if (cdev_add(&demo_cdev, mydev, 1) < SUCCESS){
        pr_info("cannot add the device to the system\n");
        goto undo_chrdev;
    }

    pr_info("device added\n");

    // creating device class
    demo_class = class_create(THIS_MODULE, DEV_NAME);
    if(demo_class == NULL){
        pr_info("class creation failed\n");
        goto undo_cdev;
    }

    //setting up device node for normal user
    demo_class -> devnode = demo_device_devnode;

    pr_info("class created\n");

    // creating device file
    if(device_create(demo_class, NULL, mydev, NULL, DEV_NAME) == NULL){
        pr_info("device creation failed\n");
        goto undo_class;
    }
    pr_info("device created\n");

    //creating physical memory 
    kernel_buffer= kmalloc(MEMSIZE, GFP_KERNEL);
    if(kernel_buffer == NULL){
        pr_info("memory allocation failed\n");
        goto undo_device;
    }

    strncpy(kernel_buffer,"Hello world\n",MEMSIZE);

    pr_info("your driver is ready\n");


    return SUCCESS;
undo_device:
    device_destroy(demo_class, mydev);
undo_class:
    class_destroy(demo_class);
undo_cdev:
    cdev_del(&demo_cdev);
undo_chrdev:
    unregister_chrdev_region(mydev, 1);
    return FAILURE;

}

static void __exit demo_driver_exit(void){
    pr_info("cleaning up your driver\n");
    kfree(kernel_buffer);
    device_destroy(demo_class, mydev);
    class_destroy(demo_class);
    cdev_del(&demo_cdev);
    unregister_chrdev_region(mydev, 1);
    pr_info("driver is removed\n");
}   


module_init(demo_driver_init);
module_exit(demo_driver_exit);