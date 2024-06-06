#define DRIVER_PREFIX "demo_driver: " 
#define pr_fmt(fmt) DRIVER_PREFIX  fmt

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include<linux/slab.h>   
#include<linux/uaccess.h>
#include<linux/sysfs.h> 
#include<linux/kobject.h>
#include <linux/interrupt.h>
#include <asm/io.h>
#include <linux/workqueue.h>

#include "list.h"


// preprocessor to make work easy
#define INIT_FUNC static int __init
#define EXIT_FUNC static void __exit
#define SUCCESS 0;
#define FAILURE -1;

struct my_list *demo_list;
static insert_nodes(struct my_list *demo_list,int no_of_nodes){
    struct my_list *temp = NULL;

    for(int i=0;i<no_of_nodes;i++){
        temp = kmalloc(sizeof(struct my_list),GFP_KERNEL);
        INIT_LIST_HEAD(&temp->list);
        temp->data = i;
        LIST_ADD_TAIL(demo_list,temp);
    }
}
INIT_FUNC driver_start(void){    
    demo_list = kmalloc(sizeof(struct my_list),GFP_KERNEL);
    INIT_LIST_HEAD(&demo_list->list);
    demo_list->data = 10;
    insert_nodes(demo_list,10);
    
    // pr_info("data %d\tnext = %p\tprev = %p\n",demo_list->data,demo_list->list.next,demo_list->list.prev);
    pr_info("device has been inserted \n");
    return SUCCESS
}


EXIT_FUNC driver_end(void){
pr_info("device is being removed\n");
kfree(demo_list);
pr_info("device has been removed\n");
}

module_init(driver_start);
module_exit(driver_end);
MODULE_LICENSE("GPL");