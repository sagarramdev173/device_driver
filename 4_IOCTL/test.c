#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>

#define WR_VALUE _IOW('a','a',int32_t*)
#define RD_VALUE _IOR('a','b',int32_t*) 

int main(int argc, char const *argv[])
{
    int fd;
    int value = 0;
    fd = open("/dev/demo_driver", O_RDWR);
    if (fd < 0) {
        printf("Device opening failed\n");
        return EXIT_FAILURE;
    }
    printf("Device opened successfully\n");
    printf("Enter the value\n");
    scanf("%d", &value);    
    ioctl(fd, WR_VALUE, &value);

    printf("Now will read the value from device\n");
    ioctl(fd, RD_VALUE, &value);
    printf("The value is %d received from device\n", value);

    printf("Now will close the device\n");
    close(fd);
    return 0;
}
