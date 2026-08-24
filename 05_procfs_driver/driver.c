#include "linux/minmax.h"
#include <linux/errno.h>
#include <linux/fs.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Procfs read/write exercise");

#define PROC_NAME "procfs_demo"
#define MESSAGE_SIZE 128

static struct proc_dir_entry *proc_entry;
static char message[MESSAGE_SIZE] = "hello from procfs\n";

static ssize_t procfs_read(struct file *file, char __user *buf, size_t count,
                           loff_t *ppos) {
  /* Return message with simple_read_from_buffer(). */
  if (*ppos < 0)
    return -EINVAL;
  size_t avail = strnlen(message, MESSAGE_SIZE);
  if (*ppos >= avail)
    return 0;
  ssize_t len_to_copy = MIN_T(size_t, count, avail - (size_t)*ppos);
  int ret = copy_to_user(buf, message + *ppos, len_to_copy);
  if (ret != 0) {
    return -EFAULT;
  }
  *ppos += len_to_copy;
  return len_to_copy;
}

static ssize_t procfs_write(struct file *file, const char __user *buf,
                            size_t count, loff_t *ppos) {
  /* Bound input, copy it to message, add a NUL, and return the count. */
  if (count == 0)
    return 0;
  count = min(count, MESSAGE_SIZE - 1);
  int ret = copy_from_user(message, buf, count);
  if (ret != 0) {
    return -EFAULT;
  }
  message[count] = '\0';
  *ppos += count;
  return count;
}

static const struct proc_ops procfs_ops = {
    .proc_read = procfs_read,
    .proc_write = procfs_write,
};

static int __init procfs_demo_init(void) {
  proc_entry = proc_create(PROC_NAME, 0644, NULL, &procfs_ops);
  if (!proc_entry)
    return -ENOMEM;

  pr_info("created /proc/%s\n", PROC_NAME);
  return 0;
}

static void __exit procfs_demo_exit(void) {
  proc_remove(proc_entry);
  pr_info("removed /proc/%s\n", PROC_NAME);
}

module_init(procfs_demo_init);
module_exit(procfs_demo_exit);
