#define DRIVER_PREFIX "demo_driver: "
#define pr_fmt(fmt) DRIVER_PREFIX fmt

#include "linux/printk.h"
#include "linux/types.h"
#include <linux/cdev.h>
#include <linux/errno.h>
#include <linux/fs.h>
#include <linux/kernel.h>
#include <linux/minmax.h>
#include <linux/module.h>
#include <linux/slab.h> //kmalloc
#include <linux/string.h>
#include <linux/uaccess.h> //copy_to/from_user

MODULE_LICENSE("GPL");

#define SUCCESS 0
#define FAILURE -1
#define DEV_NAME "demo_driver"

#define MEMSIZE 1024

// For device number
dev_t mydev = 0;

// For file operation structure
static struct cdev demo_cdev;

// For device class
static struct class *demo_class;

// For kernel allocated memory
char *kernel_buffer;

// Function prototype for file operation
static int demo_open(struct inode *inode, struct file *file);
static int demo_release(struct inode *inode, struct file *file);
static ssize_t demo_read(struct file *filp, char __user *buf, size_t len,
                         loff_t *off);
static ssize_t demo_write(struct file *filp, const char __user *buf, size_t len,
                          loff_t *off);

// For file operation
static struct file_operations fops = {.owner = THIS_MODULE,
                                      .read = demo_read,
                                      .write = demo_write,
                                      .open = demo_open,
                                      .release = demo_release};

static int demo_open(struct inode *inode, struct file *file) {
  pr_info("device has been opened\n");
  return SUCCESS;
}

static int demo_release(struct inode *inode, struct file *file) {
  pr_info("device has been released\n");
  return SUCCESS;
}

/* VFS calls this when userspace reads from /dev/demo_driver. */
static ssize_t demo_read(struct file *filp, char __user *buf, size_t len,
                         loff_t *off) {
  /* Reject an invalid file position before using it as an array index. */
  if (*off < 0)
    return -EINVAL;

  /* Store the number of valid message bytes in kernel_buffer. */
  size_t available;
  /* Store the number of bytes this one read will return. */
  size_t count;

  /* Find the message length without reading past the fixed-size buffer. */
  available = strnlen(kernel_buffer, MEMSIZE);

  /* Return EOF so callers such as cat know that the message is finished. */
  if (*off >= available)
    return 0;

  /* Copy no more than userspace requested or than remains in the message. */
  count = min_t(size_t, len, available - (size_t)*off);

  /* Copy from kernel memory at the current position into the user buffer. */
  if (copy_to_user(buf, kernel_buffer + *off, count)) {
    /* A nonzero result means some bytes could not be copied to userspace. */
    return -EFAULT;
  }
  /* Record the successful transfer without logging the user-visible payload. */
  pr_info("Successfully read %zu bytes\n", count);
  /* Advance this open file's cursor only after the copy succeeded. */
  *off += count;
  /* Tell userspace exactly how many bytes it received. */
  return count;
}

/* VFS calls this when userspace writes to /dev/demo_driver. */
static ssize_t demo_write(struct file *filp, const char __user *buf, size_t len,
                          loff_t *off) {
  /* Reserve one byte for a terminating NUL because this demo stores text. */
  size_t count = min_t(size_t, len, MEMSIZE - 1);

  /* Copy only the bounded user input into the kernel-owned message buffer. */
  if (copy_from_user(kernel_buffer, buf, count))
    /* A nonzero result means some bytes could not be read from userspace. */
    return -EFAULT;

  /* Make the bounded data safe to use as a C string in the read path. */
  kernel_buffer[count] = '\0';
  /* A write replaces the whole message, so the next read starts at byte zero. */
  *off = 0;
  /* Log metadata only; never print potentially sensitive user payload data. */
  pr_info("wrote %zu bytes\n", count);
  /* Report the number of input bytes accepted by the driver. */
  return count;
}

// For device class setting up  the mode so that normal user can access device
// node directly
static char *demo_device_devnode(struct device *dev, umode_t *mode) {
  if (mode)
    *mode = 0666;
  pr_info("device node has been created\n");
  return NULL;
}

static int __init demo_driver_init(void) {
  pr_info("starting up your driver\n");

  // allocating the device number
  if (alloc_chrdev_region(&mydev, 0, 1, DEV_NAME) < SUCCESS) {
    pr_info("cannot allocate device number\n");
    return FAILURE;
  }

  pr_info("major = %d minor = %d\n", MAJOR(mydev), MINOR(mydev));

  // initializing the cdev structure
  cdev_init(&demo_cdev, &fops);

  // associating the cdev to the device
  if (cdev_add(&demo_cdev, mydev, 1) < SUCCESS) {
    pr_info("cannot add the device to the system\n");
    goto undo_chrdev;
  }

  pr_info("device added\n");

  // creating device class
  demo_class = class_create(THIS_MODULE, DEV_NAME);
  if (demo_class == NULL) {
    pr_info("class creation failed\n");
    goto undo_cdev;
  }

  // setting up device node for normal user
  demo_class->devnode = demo_device_devnode;

  pr_info("class created\n");

  // creating device file
  if (device_create(demo_class, NULL, mydev, NULL, DEV_NAME) == NULL) {
    pr_info("device creation failed\n");
    goto undo_class;
  }
  pr_info("device created\n");

  // creating physical memory
  kernel_buffer = kmalloc(MEMSIZE, GFP_KERNEL);
  if (kernel_buffer == NULL) {
    pr_info("memory allocation failed\n");
    goto undo_device;
  }

  strncpy(kernel_buffer, "Hello world\n", MEMSIZE);

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

static void __exit demo_driver_exit(void) {
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
