* io：标准IO，系统调用IO（文件IO）。系统调用IO和特定系统有关，而标准IO屏蔽了不同的系统，例如c语言中的io库。优先使用标准IO，移植性好，而且对buffer有加速机制。在linux下，不存在文本流和二进制流的概念，只有流的概念。而windows环境下需要区分不同的操作。

* 标准IO：FILE类型贯穿始终，实际是一个结构体。
    * fopen()
    * fclose()
    * fgetc()
    * fpuc()
    * fgets()
    * fputc()
    * fread()
    * fwrite()
    * printf()
    * scanf()
    * fseek()
    * ftell()
    * rewind()
    * fflush()

* errno已经不再是一个变量，而是一个宏。私有化数据概念。
```c
#include <errno.h>

errno;

gcc -E test.c

```

* FILE* fopen(const char* path, const char*mode): FILE* 指向的区域在哪里
```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

int main() 
{
    FILE* fp = fopen();
    if(fp == NULL) 
    {
        //fprintf(stderr, "fopen() failed! errno = %d\n", errno);
        // perror("fopen()");
        fprintf("fopen() failed : %s", strerror(errno));
        exit(1);
    }
    exit(0);
}
```