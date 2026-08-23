#define DRIVER_PREFIX "demo_driver: "
#define pr_fmt(fmt) DRIVER_PREFIX  fmt

#include <linux/module.h>
#include <linux/fs.h>
#include <linux/kernel.h>
#include <linux/cdev.h>
#include <linux/slab.h>     //kmalloc
#include <linux/uaccess.h>  //copy_to/from_user
#include <linux/wait.h>       // wait queue    
#include <linux/kthread.h>    // for creating kernel thread>

MODULE_LICENSE("GPL");

// #define STATIC  0
#define DYNAMIC 1

#define SUCCESS 0
#define FAILURE -1
#define DEV_NAME "demo_driver"

#define MEMSIZE 1024

uint32_t read_count =0;
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

#ifdef STATIC
// statically declaring wait queue
DECLARE_WAIT_QUEUE_HEAD(my_demo_wait_queue_head);
pr_info("Static wait queue created\n");
#endif 

#ifdef DYNAMIC
// dynamically declaring wait queue
wait_queue_head_t my_demo_wait_queue_head;
#endif 
// flag for wait queue
int wait_queue_flag =0;

// for creating kernel thread
static struct task_struct *wait_thread;

// thread wait function
static int wait_function(void *data){
    while (1)
    {
        pr_info("wait thread is sleeping\n");
        wait_event_interruptible(my_demo_wait_queue_head,wait_queue_flag != 0);
        if(wait_queue_flag == 2){
            pr_info("wait thread is woken up by exit function\n");
            break;
        }
        pr_info("Event came from read function = %d\n",++read_count);
        wait_queue_flag = 0;
    }
    do_exit(SUCCESS);
    return SUCCESS;
}
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
    pr_info("device read has been started\n");
    wait_queue_flag = 1;
    wake_up_interruptible(&my_demo_wait_queue_head);
    pr_info("device read has been completed\n");
    return 0;
}

static ssize_t demo_write(struct file *filp, const char __user *buf, size_t len, loff_t *off){  
    pr_info("device write has been started\n");
   
    pr_info("device write has been completed\n");
    return 0;
}

// For device class setting up  the mode so that normal user can access device node directly
static char * demo_device_devnode(struct device *dev, umode_t *mode){
    if(mode)
        *mode = 0666;
    pr_info("Device node has been created\n");
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
    demo_cdev.owner = THIS_MODULE;
    demo_cdev.ops = &fops;
    
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

    #ifdef DYNAMIC
    init_waitqueue_head(&my_demo_wait_queue_head);
    pr_info("Dynamic wait queue created\n");
    #endif 
    // creating kernel thread
    wait_thread = kthread_create(wait_function,NULL,"wait_thread");
    if(wait_thread){
        pr_info("kthread created\n");
        wake_up_process(wait_thread);
    }else{
        pr_info("kthread creation failed\n");
        goto undo_device;
    }   

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
    wait_queue_flag = 2;    
    wake_up_interruptible(&my_demo_wait_queue_head);
    if(wait_thread){
        kthread_stop(wait_thread);    
        pr_info("kthread stopped\n");
    }
    device_destroy(demo_class, mydev);
    class_destroy(demo_class);
    cdev_del(&demo_cdev);
    unregister_chrdev_region(mydev, 1);
    pr_info("driver is removed\n");
}   


module_init(demo_driver_init);
module_exit(demo_driver_exit);