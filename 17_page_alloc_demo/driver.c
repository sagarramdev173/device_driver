#include "asm/page_types.h"
#include <linux/gfp.h>
#include <linux/kernel.h>
#include <linux/mm.h>
#include <linux/module.h>
#include <linux/printk.h>
#include <linux/slab.h>

MODULE_LICENSE("GPL");

static struct page *demo_page;
static char *page_buffer;

static int __init page_demo_init(void) {

  demo_page = alloc_page(GFP_KERNEL);
  if (!demo_page) {
    return -ENOMEM;
  }
  /*  obtain the CPU address with page_address(). */
  page_buffer = page_address(demo_page);
  if (!page_buffer) {
    /* free the page if page_address() fails. */
    __free_page(demo_page);
    return -ENOMEM;
  }
  snprintf(page_buffer, PAGE_SIZE, "hello from one kernel page\n");
  pr_info("Alloacted one page (%lu bytes) : %s\n", PAGE_SIZE, page_buffer);
  return 0;
}

static void __exit page_demo_exit(void) {
  /* release the page with the matching page allocator API. */
  __free_page(demo_page);
  pr_info("Released the alloacted page\n");
}

module_init(page_demo_init);
module_exit(page_demo_exit);
