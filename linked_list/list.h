#ifndef __MY_LIST__
#define __MY_LIST__

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

#define LIST_MY_HEAD_INIT(name) { &(name) ,&(name) }

#define LIST_MY_HEAD(name) \
        struct list_my_head name = LIST_MY_HEAD_INIT(name) 

struct list_my_head
{
    /* data */
    struct list_my_head *next;
    struct list_my_head *prev;
};

// intilize the list head 
static inline void INIT_NODE(struct list_my_head *list){

    list->next = list;
    list->prev = list;
}

static inline void __list_my_add(struct list_my_head *new, struct list_my_head *prev, struct list_my_head *next){

    new->next=next;
    new->prev=prev;
    prev->next=new;
    next->prev=new;
}
static inline void list_my_add(struct list_my_head *new, struct list_my_head *head){
    __list_my_add(new,head,head->next);
}

static inline void list_my_add_tail(struct list_my_head *new, struct list_my_head *head){
    __list_my_add(new,head->prev,head);
}

static inline void __list_my_del(struct list_my_head *prev, struct list_my_head *next){
    prev->next=next;
    next->prev=prev;
}

static inline void __list_my_del_entry(struct list_my_head *entry){
    __list_my_del(entry->prev,entry->next);  
}
static inline void list_my_del(struct list_my_head *entry){
    __list_my_del_entry(entry);
    entry->next=NULL;
    entry->prev=NULL;
}

typedef struct {

    // list prev and next pointers
    struct list_my_head list;      //linux list structure
    // list data
    int data;
}demo_driver_list;

static inline void* create_demo_drive_node(int data);

static inline void remove_demo_node(demo_driver_list *demo_node);








#endif // __LIST__