#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/module.h>

#define success 0;
#define faliure -1;
#define DRIVER_NAME "mydummydriver"
#define DRIVER_CLASS "mydummyclass"

MODULE_LICENSE("GPL");

dev_t mydev = 0;
static struct cdev mydev_cdev;
static struct class *my_class;

static int my_device_open(struct inode *device_file, struct file *instance) {

  printk("my device has been opened\n");
  printk("inode device number =%d\n", device_file->i_sb->s_dev);
  return success;
}

static int my_device_close(struct inode *device_file, struct file *instance) {

  printk("my device has been opened\n");
  printk("inode device number =%d\n", device_file->i_sb->s_dev);
  return success;
}
static struct file_operations fops = {

    .owner = THIS_MODULE, .open = my_device_open, .release = my_device_close};

static int __init my_init(void) {
  int ret = success;

  printk("creating the my demo device\n");

  ret = alloc_chrdev_region(&mydev, 0, 1, "mydev");
  if (ret < 0) {
    printk("Cannot create device number\n");
    return faliure;
  }
  printk("my device major=%d and minor %d number\n", MAJOR(mydev),
         MINOR(mydev));

  // intializing the cdev strcture with our file operations
  cdev_init(&mydev_cdev, &fops);

  // assinging my device with the cdev structure
  ret = cdev_add(&mydev_cdev, mydev, 1);
  if (ret < 0) {
    printk("Registering chrdev region failed ");
    goto undo_chrdev;
  }

  // creating device class whcih will comeup in /sys/class/
  my_class = class_create(THIS_MODULE, DRIVER_CLASS);
  if (my_class == NULL) {
    printk("class creation failed\n");
    goto undo_chrdev;
  }
  printk("mydummyclass has been create in sys/class ,please check\n");

  if (device_create(my_class, NULL, mydev, NULL, DRIVER_NAME) == NULL) {
    printk("Device creation in class failed\n");
    goto remove_class;
  }
  return success;

// error handlings
remove_class:
  class_destroy(my_class);
undo_chrdev:
  unregister_chrdev_region(mydev, 1);
  return faliure;
}
// undo_chrdev : unregister_chrdev_region(mydev, 1);
// return faliure;

static void __exit my_exit(void) {

  class_destroy(my_class);
  cdev_del(&mydev_cdev);
  unregister_chrdev_region(mydev, 1);
  printk("unreistered the dev numbers\n");
}

module_init(my_init);
module_exit(my_exit);
