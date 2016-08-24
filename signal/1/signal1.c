#include"stdio.h"
#include"signal.h"
#include"sys/types.h"
#include"unistd.h"




void SigHandler(int iSignNum)
{
    printf("signal:%d\n",iSignNum);
}

int main()
{
    signal(SIGALRM,SigHandler);
    alarm(5);
    printf("before pause \n");
    pause();
    printf("after pause \n");
    return 0;
} 
#if 0
void SigRoutine(int iSignNum)
{
    switch(iSignNum)
      {
            case 1:
               printf("capture SIGHUP signal, signal number is %d\n",iSignNum);
               break;
            case 2:
               printf("capture SIGINT signal, signal number is %d\n",iSignNum);
               break;
            case 3:
               printf("capture SIGINT signal, signal number is %d\n",iSignNum);
               break;
      }
}
int main()
{
    printf("process ID is %d\n",getpid());
    if(signal(SIGHUP,SigRoutine) == SIG_ERR)
      {
          printf("coundn't register signal handler for SIGHUP");
      }
    if(signal(SIGINT,SigRoutine) == SIG_ERR)
      {
          printf("coundn't register signal handler for SIGINT");
      }
    if(signal(SIGQUIT,SigRoutine) == SIG_ERR)
      {
          printf("coundn't register signal handler for SIGQUIT");
      }
    while(1)
       sleep(1);
    return 0;
} 

#endif