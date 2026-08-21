#define pr_fmt(fmt) "spinlock_demo: " fmt

#include <linux/init.h>
#include <linux/jiffies.h>
#include <linux/module.h>
#include <linux/spinlock.h>
#include <linux/timer.h>
#include <linux/workqueue.h>

MODULE_LICENSE("GPL");

/* One number shared by the timer callback and the workqueue callback. */
static unsigned long shared_count;
static DEFINE_SPINLOCK(count_lock);
static struct timer_list demo_timer;
static struct work_struct demo_work;

/* Timer callbacks run in atomic context: they must not sleep. */
static void demo_timer_fn(struct timer_list *timer) {
  unsigned long flags;

  /* Disable local interrupts while this CPU holds the shared lock. */
  spin_lock_irqsave(&count_lock, flags);
  shared_count++;
  spin_unlock_irqrestore(&count_lock, flags);

  /* Move the printable, process-context work out of the timer callback. */
  schedule_work(&demo_work);
  mod_timer(&demo_timer, jiffies + HZ);
}

/* Workqueue callbacks run in process context and may sleep if needed. */
static void demo_work_fn(struct work_struct *work) {
  unsigned long flags;
  unsigned long snapshot;

  /* Use the same lock because the timer callback can touch shared_count. */
  spin_lock_irqsave(&count_lock, flags);
  snapshot = shared_count;
  spin_unlock_irqrestore(&count_lock, flags);

  pr_info("process context read count=%lu\n", snapshot);
}

static int __init demo_init(void) {
  INIT_WORK(&demo_work, demo_work_fn);
  timer_setup(&demo_timer, demo_timer_fn, 0);
  mod_timer(&demo_timer, jiffies + HZ);
  pr_info("loaded; timer updates once per second\n");
  return 0;
}

static void __exit demo_exit(void) {
  /* Stop a callback that may be running before unloading its code. */
  del_timer_sync(&demo_timer);
  /* Wait for already queued work before unloading its callback. */
  cancel_work_sync(&demo_work);
  pr_info("unloaded safely\n");
}

module_init(demo_init);
module_exit(demo_exit);
