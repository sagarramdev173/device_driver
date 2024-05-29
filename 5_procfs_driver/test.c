#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#define DEVICE_PATH "/proc/demo_driver"

int main(int argc, char const *argv[])
{
    int fd;
    char buf[1024];
    fd = open(DEVICE_PATH, O_RDWR);
    if (fd < 0) {
        printf("Device opening failed\n");
        return EXIT_FAILURE;
    }
    printf("Device opened successfully\n");
  
    printf("Now will read the data from device\n");

    int byte_read= read(fd, buf, sizeof(buf));
    printf("buffer length = %d\n", byte_read);
    printf("received from device = %s\n", buf);  

    printf("Now will write the data to device\n");
    write(fd, "hello from user space", sizeof("hello from user space"));

    printf("Now will close the device\n");
    close(fd);
    return 0;
}