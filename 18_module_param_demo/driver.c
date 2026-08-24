#include "linux/moduleparam.h"
#include "linux/printk.h"
#include <linux/kernel.h>
#include <linux/module.h>

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Module parameter parsing exercise");

/* Declare a static int named repeat with a default value of 1. */
static int repeat = 1;

static int repeat_set(const char *value, const struct kernel_param *kp) {
  int ret = param_set_int(value, kp);
  if (ret)
    return ret;

  pr_info("repeat changed to %d\n", *(int *)kp->arg);
  return 0;
}

static int get_repeat(char *buffer, const struct kernel_param *kp) {
  pr_alert("paramter get call trigered");
  return param_get_int(buffer, kp);
}
static const struct kernel_param_ops repeat_ops = {
    .set = repeat_set,
    .get = get_repeat,
};

module_param_cb(repeat, &repeat_ops, &repeat, 0644);
MODULE_PARM_DESC(repeat, "Number reported whenever repeat is updated");

static int __init param_demo_init(void) {
  pr_info("Repeat value initlaized %d\n", repeat);
  pr_info("Driver initlazed successfully\n");
  return 0;
}

static void __exit param_demo_exit(void) {
  pr_info("module_param demo unloaded\n");
}

module_init(param_demo_init);
module_exit(param_demo_exit);
