# Procfs in Linux

(Process Filesystem) which is a filesystem in memory. Many processes store information about themselves on this virtual filesystem. ProcFS also stores other system information.

It can act as a bridge connecting the user space and the kernel space. Userspace programs can use proc files to read the information exported by the kernel. Every entry in the proc file system provides some information from the kernel.

The entry “meminfo”  gives the details of the memory being used in the system.
To read the data in this entry just run

``` bash
cat /proc/meminfo
```

## Creating procfs directory

To create a procfs entry use `proc_create()`, this functions are defined in the file `linux/proc_fs.h`.

``` C
proc_create(PROC_NAME, 0666, parent, &proc_ops);
```

Where,  
`name`: The name of the proc entry  
`mode`: The access mode for proc entry  
`parent`: The name of the parent directory under /proc. If NULL is passed as a parent, the /proc directory will be set as a parent.  
`proc_fops`: The structure in which the file operations for the proc entry will be created.

## cleaning up procfs entry

To clean up the procfs entry

``` C
 remove_proc_entry(PROC_NAME, parent); 
```

Where,  
`name`: The name of the proc entry  
`parent`: The name of the parent directory under /proc. If NULL is passed as a parent, the /proc directory will be set as a parent.

## Procfs Operations

The structure in which the file operations for the proc entry will be created.  
`open`: The open function will be called when the entry is opened.
`read`: The read function will be called when the entry is read.

``` C
static struct file_operations proc_ops = {
    .proc_open = proc_open,
    .proc_read = proc_read
};
```

from kernel v5.10 onwards this structure is replaced by `proc_fops`.

## Advantages of proc_ops over traditional device operations(device_fops)

`Visibility:` Files in /proc are easily accessible from user space, allowing users and administrators to check system and device status without needing special tools.

`No Device File Required:` For some operations, creating a device file might not be necessary. Using /proc, developers can expose information without the need for creating device nodes.

`On-Demand Information:` The /proc filesystem is virtual and can provide dynamic, up-to-date information. For example, reading from a file in /proc can trigger real-time data collection and reporting.