#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
  int dev = open("/dev/mydummydriver", O_RDONLY);

  if (dev < 0) {
    printf("Device opening failed\n");
    return EXIT_FAILURE;
  }
  printf("Device opened succesfull and now will close it\n");
  close(dev);
  return EXIT_SUCCESS;
}
