#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <errno.h>


static int sig_cnt[NSIG];
static volatile sig_atomic_t get_SIGINT = 0;

void handler(int signo)
{
	if(signo == SIGINT)
		get_SIGINT = 1;
	else
		sig_cnt[signo]++;
}

int main(int argc,char* argv[])
{
	int i = 0;
	sigset_t blockall_mask ;
	sigset_t pending_mask ;
	sigset_t empty_mask ;
	printf("%s:PID is %ld\n",argv[0],getpid());

	
	for(i = 1; i < NSIG; i++)
	{
		if(i == SIGKILL || i == SIGSTOP)
			continue;

		if(signal(i,&handler) == SIG_ERR)
		{
			fprintf(stderr,"signal for signo(%d) failed (%s)\n",i,strerror(errno));
//            return -1;
		}
	}

	if(argc > 1)
	{
		int sleep_time = atoi(argv[1]);
		sigfillset(&blockall_mask);

		if(sigprocmask(SIG_SETMASK,&blockall_mask,NULL) == -1)
		{
			fprintf(stderr,"setprocmask to block all signal failed(%s)\n",strerror(errno));
			return -2;
		}

		printf("I will sleep %d second\n",sleep_time);

		sleep(sleep_time);
		if(sigpending(&pending_mask) == -1)
		{
			fprintf(stderr,"sigpending failed(%s)\n",strerror(errno));
			return -2;
		}

		for(i = 1 ; i < NSIG ; i++)
		{
			if(sigismember(&pending_mask,i))
			printf("signo(%d) :%s\n",i,strsignal(i));
		}

		sigemptyset(&empty_mask);
		if(sigprocmask(SIG_SETMASK,&empty_mask,NULL) == -1)
		{
			fprintf(stderr,"setprocmask to release all signal failed(%s)\n",strerror(errno));
			return -3;
		}
		
	}

	while(!get_SIGINT)
		continue ; //why not use pause ? I will explain later

	for(i = 1; i < NSIG ; i++)
	{
		if(sig_cnt[i] != 0 )
		{
			printf("%s:signal %d caught %d time%s\n",
					argv[0],i,sig_cnt[i],(sig_cnt[i] >1)?"s":"");
		}
	}

	return 0;

}