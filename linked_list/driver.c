#define DRIVER_PREFIX "demo_driver: " 
#define pr_fmt(fmt) DRIVER_PREFIX  fmt

#include "list.h"


// preprocessor to make work easy
#define INIT_FUNC static int __init
#define EXIT_FUNC static void __exit
#define SUCCESS 0;
#define FAILURE -1;

demo_driver_list *temp  = NULL;



static inline void* create_demo_drive_node(int data){
    demo_driver_list *demo_node = kmalloc(sizeof(demo_driver_list),GFP_KERNEL); 
    if(!demo_node){
        pr_err("failed to allocate memory\n");
        return NULL;
    }   
    INIT_NODE(&demo_node->list);
    demo_node->data = data;
    return (void *)demo_node;    
}

static inline void remove_demo_node(demo_driver_list *demo_node){
    list_my_del(&demo_node->list);    
    kfree(demo_node);
}

INIT_FUNC driver_start(void){    
    int data = 50;
    temp = create_demo_drive_node(data);
    if(temp == NULL){
        pr_err("failed to allocate memory\n");
        return FAILURE
    }
    pr_info("Data=%d\n",temp->data);
    
    pr_info("device has been inserted \n");
    return SUCCESS
}


EXIT_FUNC driver_end(void){
pr_info("device is being removed\n");
if(temp){
    remove_demo_node(temp);
    temp=NULL;    
}
pr_info("device has been removed\n");
}

module_init(driver_start);
module_exit(driver_end);
MODULE_LICENSE("GPL");