#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#define SIZE 1024
int main(int argc, char const *argv[])
{
    int fd;
    char choice;
    char buf[SIZE];
    char write_buf[SIZE];
    fd = open("/dev/demo_driver", O_RDWR);
    if (fd < 0) {
        printf("Device opening failed\n");
        return EXIT_FAILURE;
    }
    printf("Device opened successfully\n");

    while(1){
        printf("****Please Enter the Option******\n");
        printf("        1. Read               \n");
        printf("        2. write                 \n");
        printf("        3. Exit                 \n");
        printf("*********************************\n");
        scanf(" %c", &choice);
        printf("Your Option = %c\n", choice);
        switch (choice)
        {
            case '1':
                read(fd, buf, SIZE);
                printf("Read from device: %s\n", buf);
                break;
            case '2':
                printf("start writing data to the kernel driver\n");
                scanf(" %[^\t\n]s", write_buf);
                write(fd, write_buf, strlen(write_buf)+1);
                break; 
            case '3':     
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
