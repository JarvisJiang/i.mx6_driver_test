#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <sys/time.h>

#include <sys/unistd.h>
int sec;

struct timeval start, end;

void sigroutine(int signo){
    
    
    switch (signo){
        case SIGALRM:
            printf("one one-- SIGALRM /n");
            signal(SIGALRM, sigroutine);
            gettimeofday( &end, NULL );
            printf("end   : %d.%d\n", end.tv_sec, end.tv_usec);
            break;
        case SIGVTALRM:
            printf("UUUUUU -- SIGVTALRM /n");
            gettimeofday( &end, NULL );
            printf("end   : %d.%d\n", end.tv_sec, end.tv_usec);
            signal(SIGVTALRM, sigroutine);
            break;
    }
    return;
}
int main()
{
    struct itimerval value, ovalue, value2;          //(1)
    sec = 5;
    gettimeofday( &start, NULL );
    printf("start : %d.%d\n", start.tv_sec, start.tv_usec);
    
    printf("process id is %d/n", getpid());

    signal(SIGALRM, sigroutine);
    signal(SIGVTALRM, sigroutine);
    value.it_value.tv_sec = 1;
    value.it_value.tv_usec = 0;
    value.it_interval.tv_sec = 1;
    value.it_interval.tv_usec = 0;
   setitimer(ITIMER_REAL, &value, &ovalue);    //(2)
    value2.it_value.tv_sec = 0;
    value2.it_value.tv_usec = 200000;
    value2.it_interval.tv_sec = 0;
    value2.it_interval.tv_usec = 200000;
    setitimer(ITIMER_VIRTUAL, &value2, &ovalue);
    for(;;)
        ;
}