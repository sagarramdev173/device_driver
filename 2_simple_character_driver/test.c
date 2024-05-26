#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define DRIVER_CLASS "Demodriver"
#define DEVICE_PATH "/dev/" DRIVER_CLASS

int main(int argc, char *argv[]) {
  
  int dev = open(DEVICE_PATH, O_RDONLY);  

  if (dev < 0) {
    printf("Device opening failed\n");
    return EXIT_FAILURE;
  }
  printf("Device opened successfully and now will close it\n");
  close(dev);

  return EXIT_SUCCESS;
}
