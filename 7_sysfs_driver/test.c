#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#define SIZE sizeof(int)
int main(int argc, char const *argv[])
{
    int fd;
    char choice;
    char read_buf[20];
    int write_buf;
    fd = open("/sys/kernel/demo_driver/my_sysfs", O_RDONLY);
    if (fd < 0) {
        printf("Device opening failed\n");
        return EXIT_FAILURE;
    }
    printf("Device opened successfully\n");

    while(1){
        printf("****Please Enter the Option******\n");
        printf("        1. Read               \n");        
        printf("        2. Exit                 \n");
        printf("*********************************\n");
        scanf(" %c", &choice);
        printf("Your Option = %c\n", choice);
        switch (choice)
        {
            case '1':
                lseek(fd,0,SEEK_SET);
                read(fd, &read_buf, sizeof(int));
                printf("Read from device: %s\n", read_buf);
                break;            
            case '2':     
                printf("Closing the device\n");
                close(fd);
                exit(1);
            default:
                printf("Wrong Choice\n");
                break;
        }
    } 
    return 0;
}
