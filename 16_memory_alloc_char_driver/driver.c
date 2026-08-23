#include "linux/printk.h"
#include <linux/errno.h>
#include <linux/module.h>
#include <linux/slab.h>

MODULE_LICENSE("GPL");

#define BUFFER_SIZE (1 * 1024 * 1024)

static char *buffer;

static int __init alloc_demo_init(void) {

  /* It also gives memeory but unintalized and may leak info*/
  // buffer = kmalloc(BUFFER_SIZE, GFP_KERNEL);

  /* It gives memeory but intalized */
  buffer = kzalloc(BUFFER_SIZE, GFP_KERNEL);
  if (!buffer)
    return -ENOMEM;

  pr_info("allocated %d bytes\n", BUFFER_SIZE);
  pr_info("First byte = %u\n", (unsigned char)buffer[0]);
  return 0;
}

static void __exit alloc_demo_exit(void) {
  kfree(buffer);
  pr_info("freed the buffer\n");
}

module_init(alloc_demo_init);
module_exit(alloc_demo_exit);
