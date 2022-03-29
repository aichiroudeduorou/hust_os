#include "stdio.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "unistd.h"

#define buflen 20

int main(int argc, const char* argv[])
{
    if (argc != 3)
    {
        printf("Missing parameter!\n");
        return 0;
    }

    int f_read = open(argv[1], O_RDONLY);
    if (f_read == -1)
    {
        printf("Open file failed!\n");
        return 0;
    }

    int f_write = open(argv[2], O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    if (f_write == -1)
    {
        printf("Create file failed!\n");
        return 0;
    }

    char buf[buflen];
    int num;
    while ((num = read(f_read, buf, buflen)) > 0)
    {
        write(f_write, buf, num);
    }

    close(f_read);
    close(f_write);
    return 0;
}