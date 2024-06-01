#define DRIVER_PREFIX "demo_driver: " 
#define pr_fmt(fmt) DRIVER_PREFIX  fmt


#include<linux/module.h>
#include<linux/kernel.h>
#include<linux/init.h>
#include<linux/fs.h>
#include<linux/uaccess.h>              //copy_to/from_user()
#include<linux/sysfs.h> 
#include<linux/kobject.h> 

// preprocessor to make work easy
#define INIT_FUNC static int __init
#define EXIT_FUNC static void __exit
#define SUCCESS 0
#define FAILURE -1

/************************************ Sysfs functions****************************/

int my_sysfs = 10;
static ssize_t read_sysfs(struct kobject *kobj, struct kobj_attribute *attr, char *buf){
    pr_info("read sysfs\n");
    return sprintf(buf, "%d\n", my_sysfs);
}

static ssize_t write_sysfs(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count){
    int res;
    pr_info("write sysfs\n");
    res = kstrtoint(buf, 10, &my_sysfs);
    if (res < 0) {
        pr_info("Invalid input\n");
        return res;
    }
    return count;
}   

static struct kobj_attribute demo_mysysfs_attr = __ATTR(my_sysfs, 0664, read_sysfs, write_sysfs);

static struct kobject *kobj;

INIT_FUNC demo_driver_init(void){
    pr_info("device has been initialized\n");
    
    kobj = kobject_create_and_add("demo_driver", kernel_kobj);    
    if(!kobj){
        pr_info("kobject creation failed\n");
        return FAILURE;
    }

    if(sysfs_create_file(kobj, &demo_mysysfs_attr.attr)){
      pr_info("sysfs creation failed\n");  
      goto remove_kobj;
    }

    pr_info("device is ready\n");
    return SUCCESS;

remove_kobj:
    kobject_put(kobj);
    return FAILURE;
}

EXIT_FUNC demo_driver_exit(void){
    pr_info("device is being removed\n");
    sysfs_remove_file(kobj, &demo_mysysfs_attr.attr);
    kobject_put(kobj);
    pr_info("device has been removed\n");
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Sagar Ramdev");
MODULE_DESCRIPTION("A simple sysfs attribute");
module_init(demo_driver_init);
module_exit(demo_driver_exit);

