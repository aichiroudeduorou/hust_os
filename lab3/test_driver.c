#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#define MEM_CLEAR 1
#define SEEK_SET 0
#define SEEK_CUR 1
void print_info(void) {
  printf("Info: \n");
  printf("\t read <readnum>\n");
  printf("\t write <string>\n");
  printf("\t lseek <offset> 0-set/1-cur\n");
  printf("\t ioctl\n");
  printf("\t exit\n");
}

int main(int argc, char **argv) {
  printf("Test device driver\n");
  int fd, start, num;
  fd = open("/dev/mydriver", O_RDWR);
  if (fd < 0) {
    printf("Open mydriver error!\n");
    return 0;
  }
  char op[10];
  while(1){
    printf("Input operation: ");
    scanf("%s", op);
    if (strcmp(op, "read") == 0) {
      char buf[1024];
      memset(buf, 0, 1024);
      // printf("Input start pos: ");
      // scanf("%d", &start);
      printf("Input read num: ");
      scanf("%d", &num);
      // lseek(fd, start, SEEK_SET);
      read(fd, buf, num);
      printf("Read: %s\n", buf);
    } else if (strcmp(op, "write") == 0) {
      char buf[1024];
      memset(buf, 0, 1024);
      // printf("Input start pos: ");
      // scanf("%d", &start);
      printf("Input string: ");
      scanf("%s", buf);
      // lseek(fd, start, SEEK_CUR);
      write(fd, buf, strlen(buf));
      printf("Write success!\n");
    } else if (strcmp(op, "ioctl") == 0) {
      ioctl(fd, MEM_CLEAR);
    } else if(strcmp(op,"lseek") == 0){
      printf("Input offset: ");
      scanf("%d", &start);
      printf("Input whence: ");
      scanf("%d", &num);
      lseek(fd, start, num);
      printf("Current pos is %d\n", lseek(fd, 0, SEEK_CUR));
    } else if (strcmp(op, "exit") == 0) {
      break;
    } else {
      print_info();
    }
  }

  close(fd);
  return 0;
}