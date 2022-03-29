#include <unistd.h>
#include <stdio.h>
#include <errno.h>

int main(int argc, char **argv) {
  printf("Test mycopy\n");
  if (argc != 3) {
    printf("Missing parameters!\n");
    return 0;
  }
  printf("Copy from %s to %s\n", argv[1], argv[2]);
  long ret = syscall(334, argv[1], argv[2]);
  printf("ret: %ld\n", ret);
  printf("errno: %d\n", errno);
  return 0;
}