/*

 因为我们知道，SIGKILL和SIGSTOP这两个信号是不能够定制自己的信号处理函数的，当然也不能block，原因很简单，OS或者说root才是final boss，必须有稳定终结进程的办法。假如所有的信号，进程都能ignore，OS如何终结进程？
这个signal_receiver会等待所有的信号，接收到某信号后，该信号的捕捉到的次数++，SIGINT会终结进程，进程退出前，会打印信号的捕捉统计。
如果进程有参数，表示sleep时间，signal_receiver会先屏蔽所有信号（当然，SIGKILL和SIGSTOP并不能被真正屏蔽）。然后sleep 一段时间后，取消信号屏蔽。我们可以想象，在信号屏蔽期间，我们收到的信号，都会在kernel记录下来，但是并不能delivery，这种信号称之挂起信号。如果在sleep期间或者说信号屏蔽期间，我收到SIGUSR1 这个信号1次和10000次，对内核来说，都是没差别的，因为后面的9999次都会被ignore掉。SIGUSR1属于不可靠信号，位图表示有没有挂起信号，有的话，直接ignore，没有的话，则记录在kernel。
然后我们看下，signal_sender： 

manu@manu-hacks:~/code/c/self/signal$ cat signal_sender.c


http://blog.chinaunix.net/uid-24774106-id-4064447.html
*/
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <signal.h>
#include <string.h>
#include <errno.h>

void usage()
{
	fprintf(stderr,"USAGE:\n");
	fprintf(stderr,"--------------------------------\n");
	fprintf(stderr,"signal_sender pid signo times\n");
}

int main(int argc,char* argv[])
{
	pid_t pid = -1 ;
	int signo = -1;
	int times = -1;
	int i ;


	if(argc < 4 )
	{
		usage();
		return -1;
	}
	
	pid = atol(argv[1]);
	signo = atoi(argv[2]);
	times = atoi(argv[3]);

	if(pid <= 0 || times < 0 || signo <1 ||signo >=64 ||signo == 32 || signo ==33)
	{
		usage();
		return -1;
	}

	printf("pid = %ld,signo = %d,times = %d\n",pid,signo,times);

	for( i = 0 ; i < times ; i++)
	{
		if(kill(pid,signo) == -1)
		{
			fprintf(stderr, "send signo(%d) to pid(%ld) failed,reason(%s)\n",signo,pid,strerror(errno));
			return -2;
		}
	}
	fprintf(stdout,"done\n");
	return 0;

}

