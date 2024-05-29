#define DRIVER_PREFIX "procfs_demo_driver: "
#define pr_fmt(fmt) DRIVER_PREFIX  fmt

#include <linux/module.h>
#include <linux/fs.h>
#include <linux/kernel.h>
#include <linux/cdev.h>
#include <linux/slab.h>     //kmalloc
#include <linux/uaccess.h>  //copy_to/from_user
#include <linux/ioctl.h>    // for ioctl 
#include <linux/proc_fs.h>

MODULE_LICENSE("GPL");

#define SUCCESS 0
#define FAILURE -1
#define MESSAGE_SIZE 128

#define PROC_NAME "demo_driver"

//procfs file operations structure
static struct proc_dir_entry *parent = NULL;

static char kernel_buffer[MESSAGE_SIZE] = "hello from kernel space\n";

static int open_demo_proc(struct inode *inode, struct file *file) {
    pr_info("device has been opened\n");
    return SUCCESS;
}

static int close_demo_proc(struct inode *inode, struct file *file) {
    pr_info("device has been closed\n");
    return SUCCESS;
}


static ssize_t read_demo_proc(struct file *filp, char __user *buf, size_t len, loff_t *off) {
    int buf_len=0;  
    buf_len= strlen(kernel_buffer);
    pr_info("device read has been started\n"); 
    
    if(*off >= buf_len){
        pr_info("greater buffer length\n");
        return 0;
    }
    
    if(len > buf_len - *off){
        pr_info("adjusting buffer length\n");
        len = buf_len - *off;
        // return -EINVAL;
    }

    if (copy_to_user(buf,kernel_buffer + *off , len) != SUCCESS) {
        pr_info("copy to user failed\n");
        return -EFAULT;
    }
    *off += len;
    pr_info("device read has been completed\n");    
    return len;
}

static ssize_t write_demo_proc(struct file *filp, const char __user *buf, size_t len, loff_t *off) {
    pr_info("device write has been started\n");
    if(len > MESSAGE_SIZE -1){
        return -EINVAL;
    }
    if (copy_from_user(kernel_buffer, buf, len) != SUCCESS) {
        pr_info("copy from user failed\n");
        return FAILURE;
    }
    kernel_buffer[len] = '\0';
    pr_info("data from user is %s\n", kernel_buffer);
    pr_info("device write has been completed\n");
    return len;
}

// procfs operation structure
static struct file_operations proc_ops = {
    .open = open_demo_proc,
    .read = read_demo_proc,
    .write = write_demo_proc,
    // .lseek = NULL,
    .release = close_demo_proc
};

static int __init demo_driver_init(void) {
    pr_info("starting up your driver\n");
    proc_create(PROC_NAME, 0666, parent, &proc_ops);
    pr_info("driver is inserted\n");
    return SUCCESS;
}

static void __exit demo_driver_exit(void){
    pr_info("cleaning up your driver\n");
    remove_proc_entry(PROC_NAME, parent); 
    pr_info("driver is removed\n");
}

module_init(demo_driver_init);
module_exit(demo_driver_exit);