# IOCTL in Linux

IOCTL is referred to as Input and Output Control, which is used to talk to device drivers. This system call is available in most driver categories. The major use of this is in case of handling some specific operations of a device for which the kernel does not have a system call by default.

# Create IOCTL Command in the Driver

``` C
#define "ioctl name" __IOX("magic number","command number","argument type")
```

where IOX can be :

`IO`  : an ioctl with no parameters.

`IOW` : an ioctl with write parameters (copy_from_user)

`IOR` : an ioctl with read parameters (copy_to_user)

`IOWR`: an ioctl with both write and read parameters

The Magic Number is a unique number or character that will differentiate our set of ioctl calls from the other ioctl calls. some times the major number for the device is used here.
Command Number is the number that is assigned to the ioctl. This is used to differentiate the commands from one another.
The last is the type of data.

# Linux IOCTL Example

``` C
#include <linux/ioctl.h>

#define WR_VALUE _IOW('a','a',int32_t*)
#define RD_VALUE _IOR('a','b',int32_t*)
```

## Write IOCTL Function in the Driver

``` C
static long demo_ioctl(struct file *file, unsigned int cmd, unsigned long arg);
```
