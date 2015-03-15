#include <stdio.h>
#include <stdlib.h>

#define MEGABYTE (1024*1024)

int main(int argc, char *argv[])
{
    void *myblock = NULL;
    int count = 0;
    int i;
    while (1)
    {
        myblock = (void *) malloc(MEGABYTE);
        if (!myblock)
            break;
        for(i = 0;i < MEGABYTE / 4;i ++)    /// 由于操作系统的某些机制，申请的内存只有在用到的时候才
        {           /// 真正申请内存，所以这里需要写内存操作。
            ((unsigned int*)myblock)[i] = i;
        }
        count++;
        printf("%d \n",count);
    }
    return 0;
}
