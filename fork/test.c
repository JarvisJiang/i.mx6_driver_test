#include "stdio.h"
#include "sys/types.h"
#include "unistd.h"
#include <sys/syscall.h>
#include <assert.h>

#define gettid() syscall(__NR_gettid)
int main()
{

    pid_t pid1;
    pid_t pid2;
	
	printf("pid1:%d, pid2:%d\n", getppid(), getpid());
    pid1 = fork();
    pid2 = fork();
 
    printf("pid1:%d, pid2:%d\n", pid1, pid2);
}