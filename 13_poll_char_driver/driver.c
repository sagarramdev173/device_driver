#include "asm-generic/poll.h"
#include "asm-generic/rwonce.h"
#define pr_fmt(fmt) "waitqueue_demo: " fmt

#include <linux/cdev.h>
#include <linux/errno.h>
#include <linux/fs.h>
#include <linux/minmax.h>
#include <linux/module.h>
#include <linux/mutex.h>
#include <linux/poll.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/wait.h>

MODULE_LICENSE("GPL");

#define DEVICE_NAME "waitqueue_demo"
#define BUFFER_SIZE 1024

static dev_t devno;
static struct cdev demo_cdev;
static struct class *demo_class;
static char *buffer;
static DEFINE_MUTEX(buffer_lock);
static wait_queue_head_t read_queue;
static bool data_ready;

static ssize_t demo_read(struct file *, char __user *, size_t, loff_t *);
static ssize_t demo_write(struct file *, const char __user *, size_t, loff_t *);
static __poll_t demo_poll(struct file *, poll_table *);

static const struct file_operations demo_fops = {
    .owner = THIS_MODULE,
    .read = demo_read,
    .write = demo_write,
    .poll = demo_poll,
};

static char *demo_devnode(struct device *dev, umode_t *mode) {
  if (mode)
    *mode = 0666;
  return NULL;
}

static ssize_t demo_read(struct file *file, char __user *user_buf, size_t len,
                         loff_t *off) {
  size_t available, count;
  ssize_t ret;

  if (*off < 0)
    return -EINVAL;

  ret = wait_event_interruptible(read_queue, READ_ONCE(data_ready));
  if (ret)
    return -ERESTARTSYS;

  mutex_lock(&buffer_lock);
  available = strnlen(buffer, BUFFER_SIZE);
  if (*off >= available) {
    ret = 0;
    goto out;
  }

  count = min_t(size_t, len, available - (size_t)*off);
  if (copy_to_user(user_buf, buffer + *off, count)) {
    ret = -EFAULT;
    goto out;
  }

  *off += count;
  ret = count;
  if (*off > available)
    data_ready = false;
out:
  mutex_unlock(&buffer_lock);
  return ret;
}

static ssize_t demo_write(struct file *file, const char __user *user_buf,
                          size_t len, loff_t *off) {
  size_t count = min_t(size_t, len, BUFFER_SIZE - 1);
  ssize_t ret;

  mutex_lock(&buffer_lock);
  if (copy_from_user(buffer, user_buf, count)) {
    ret = -EFAULT;
    goto out;
  }

  buffer[count] = '\0';
  *off = 0;
  data_ready = true;
  ret = count;
out:
  mutex_unlock(&buffer_lock);
  if (ret > 0)
    wake_up_interruptible(&read_queue);
  return ret;
}

static __poll_t demo_poll(struct file *file, poll_table *wait) {
  __poll_t mask = 0;

  poll_wait(file, &read_queue, wait);
  if (READ_ONCE(data_ready))
    mask |= POLLIN | POLLRDNORM;
  return mask;
}

static int __init demo_init(void) {
  int ret;

  init_waitqueue_head(&read_queue);
  data_ready = false;

  ret = alloc_chrdev_region(&devno, 0, 1, DEVICE_NAME);
  if (ret)
    return ret;

  cdev_init(&demo_cdev, &demo_fops);
  ret = cdev_add(&demo_cdev, devno, 1);
  if (ret)
    goto unregister;

  demo_class = class_create(THIS_MODULE, DEVICE_NAME);
  if (IS_ERR(demo_class)) {
    ret = PTR_ERR(demo_class);
    goto del_cdev;
  }
  demo_class->devnode = demo_devnode;

  buffer = kmalloc(BUFFER_SIZE, GFP_KERNEL);
  if (!buffer) {
    ret = -ENOMEM;
    goto destroy_class;
  }
  buffer[0] = '\0';

  if (IS_ERR(device_create(demo_class, NULL, devno, NULL, DEVICE_NAME))) {
    ret = -ENOMEM;
    goto free_buffer;
  }
  return 0;

free_buffer:
  kfree(buffer);
destroy_class:
  class_destroy(demo_class);
del_cdev:
  cdev_del(&demo_cdev);
unregister:
  unregister_chrdev_region(devno, 1);
  return ret;
}

static void __exit demo_exit(void) {
  device_destroy(demo_class, devno);
  kfree(buffer);
  class_destroy(demo_class);
  cdev_del(&demo_cdev);
  unregister_chrdev_region(devno, 1);
}

module_init(demo_init);
module_exit(demo_exit);
