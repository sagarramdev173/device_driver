#include <linux/init.h>
#include <linux/module.h>

MODULE_LICENSE("GPL");

static int __init my_init(void) {
  printk("hello world\n");
  return 0;
}

static void __exit my_exit(void) { 
  printk("bye cruel wolrd\n"); 
  }

module_init(my_init);
module_exit(my_exit);
