# sysfs in kernel

Sysfs is a virtual filesystem exported by the kernel, similar to /proc. The files in Sysfs contain information about devices and drivers. Some files in Sysfs are even writable, for configuration and control of devices attached to the system. Sysfs is always mounted on /sys.

The directories in Sysfs contain the hierarchy of devices, as they are attached to the computer.

Sysfs is the commonly used method to **export system information from the kernel space to the user space for specific devices.**


The heart of the sysfs model is the **kernel object. Kobject** is the glue that binds the sysfs and the kernel, which is represented by `struct kobject` and defined in `<linux/kobject.h>`.

## Kernel objects

A struct kobject represents a kernel object, maybe a device or so, such as the things that show up as a directory in the sysfs filesystem.

Kobjects are usually embedded in other structures.

It is defined as,

``` C
#define KOBJ_NAME_LEN 20 
struct kobject {
 char *k_name;
 char name[KOBJ_NAME_LEN];
 struct kref kref;
 struct list_head entry;
 struct kobject *parent;
 struct kset *kset;
 struct kobj_type *ktype;
 struct dentry *dentry;
};
```
Some of the important fields are:

struct kobject  
|– `name` (Name of the kobject. Current kobject is created with this name in sysfs.)  
|– `parent` (This is kobject’s parent. When we create a directory in sysfs for the current kobject, it will create under this parent directory)  
|– `ktype` (the type associated with a kobject)  
|– `kset` (a group of kobjects all of which are embedded in structures of the same type)  
|– `sd` (points to a sysfs_dirent structure that represents this kobject in sysfs.)  
|– `kref` (provides reference counting)  

It is the glue that holds much of the device model and its sysfs interface together.

So kobject is used to create kobject directory in /sys. This is enough. We will not go deep into the kobjects.

## Create a directory in /sys

We can use this function (kobject_create_and_add) to create a directory.  
``` C
struct kobject * kobject_create_and_add ( const char * name, struct kobject * parent);
```

Where,  
`name` – the name for the kobject  
`parent` – the parent kobject of this kobject, if any.

Example

``` C
struct kobject *kobj_ref;
/*Creating a directory in /sys/kernel/ */
kobj_ref = kobject_create_and_add("etx_sysfs",kernel_kobj); //sys/kernel/etx_sysfs
/*Freeing Kobj*/
kobject_put(kobj_ref);
```

## Create Sysfs file

Using the above function we will create a directory in /sys. 
Now we need to create a sysfs file, which is used to interact user space with kernel space through sysfs. So we can create the sysfs file using sysfs attributes.

Attributes are represented as regular files in sysfs with one value per file.

Create attribute:  
`Kobj_attribute` is defined as,

``` C
struct kobj_attribute {
 struct attribute attr;
 ssize_t (*show)(struct kobject *kobj, struct kobj_attribute *attr, char *buf);
 ssize_t (*store)(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count);
};
```

## Remove Sysfs file and kernel obejct
``` C
void sysfs_remove_file ( struct kobject *  kobj, const struct attribute * attr);
```
Where,  
kobj – object we’re creating for.  
attr – attribute descriptor.
```C
//This should be called from exit function
kobject_put(kobj_ref);
```