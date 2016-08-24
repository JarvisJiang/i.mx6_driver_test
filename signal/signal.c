/*

 ��Ϊ����֪����SIGKILL��SIGSTOP�������ź��ǲ��ܹ������Լ����źŴ������ģ���ȻҲ����block��ԭ��ܼ򵥣�OS����˵root����final boss���������ȶ��ս���̵İ취���������е��źţ����̶���ignore��OS����ս���̣�
���signal_receiver��ȴ����е��źţ����յ�ĳ�źź󣬸��źŵĲ�׽���Ĵ���++��SIGINT���ս���̣������˳�ǰ�����ӡ�źŵĲ�׽ͳ�ơ�
��������в�������ʾsleepʱ�䣬signal_receiver�������������źţ���Ȼ��SIGKILL��SIGSTOP�����ܱ��������Σ���Ȼ��sleep һ��ʱ���ȡ���ź����Ρ����ǿ����������ź������ڼ䣬�����յ����źţ�������kernel��¼���������ǲ�����delivery�������źų�֮�����źš������sleep�ڼ����˵�ź������ڼ䣬���յ�SIGUSR1 ����ź�1�κ�10000�Σ����ں���˵������û���ģ���Ϊ�����9999�ζ��ᱻignore����SIGUSR1���ڲ��ɿ��źţ�λͼ��ʾ��û�й����źţ��еĻ���ֱ��ignore��û�еĻ������¼��kernel��
Ȼ�����ǿ��£�signal_sender�� 

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

