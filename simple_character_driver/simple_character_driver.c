#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/module.h>

#define SUCCESS 0;
#define FAILURE -1;
#define DRIVER_NAME "Demodriver"
#define DRIVER_CLASS "Mydummyclass"

MODULE_LICENSE("GPL");

dev_t mydev = 0;
static struct cdev mydev_cdev;
static struct class *my_class;

static int my_device_open(struct inode *device_file, struct file *instance) {

  printk("my device has been opened\n");
  
  return SUCCESS;
}

static int my_device_close(struct inode *device_file, struct file *instance) {

  printk("closing %s\n", DRIVER_NAME);
  
  return SUCCESS;
}
static struct file_operations fops = {

    .owner = THIS_MODULE, 
    .open = my_device_open,
    .release = my_device_close
  };

static int __init my_init(void) {
  
  int ret = SUCCESS;

  printk("Creating the my demo device\n");

  ret = alloc_chrdev_region(&mydev, 0, 1, "mydev");
  if (ret < 0) {
    printk("Cannot create device number\n");
    return FAILURE;
  }
  printk("my device major=%d and minor %d number\n", MAJOR(mydev),
         MINOR(mydev));

  // initializing the cdev struture with our file operations
  cdev_init(&mydev_cdev, &fops);

  // assigning my device with the cdev structure
  ret = cdev_add(&mydev_cdev, mydev, 1);
  if (ret < 0) {
    printk("Registering chrdev region failed ");
    goto undo_chrdev;
  }

  // creating device class which will come up in /sys/class/
  my_class = class_create(THIS_MODULE, DRIVER_CLASS);
  if (my_class == NULL) {
    printk("class creation failed\n");
    goto undo_chrdev;
  }

  printk("mydummyclass has been create in /sys/class ,please check\n");

  if (device_create(my_class, NULL, mydev, NULL, DRIVER_NAME) == NULL) {
    printk("Device creation in class failed\n");
    goto remove_class;
  }
  return SUCCESS;

// error handlings
remove_class:
  class_destroy(my_class);
undo_chrdev:
  unregister_chrdev_region(mydev, 1);
  return FAILURE;
}

static void __exit my_exit(void) {

  device_destroy(my_class,mydev);
  class_destroy(my_class);
  cdev_del(&mydev_cdev);
  unregister_chrdev_region(mydev, 1);
  printk("unregistered the dev numbers\n");
}

module_init(my_init);
module_exit(my_exit);
