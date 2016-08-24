#include <stdio.h>  
#include <signal.h>  
#include "sys/time.h"
#include <string.h>  
#include <stdlib.h>  
#include <unistd.h>  
#include <time.h>
#define CLOCKID CLOCK_REALTIME  
  
void timer_thread(union sigval v)  
{  
	printf("timer_thread function! %d\n", v.sival_int);  
}  
  
int main()  
{  
	// XXX int timer_create(clockid_t clockid, struct sigevent *evp, timer_t *timerid);  
	// clockid--ֵ��CLOCK_REALTIME,CLOCK_MONOTONIC��CLOCK_PROCESS_CPUTIME_ID,CLOCK_THREAD_CPUTIME_ID  
	// evp--��Ż���ֵ�ĵ�ַ,�ṹ��Ա˵���˶�ʱ�����ڵ�֪ͨ��ʽ�ʹ���ʽ��  
	// timerid--��ʱ����ʶ��  
	timer_t timerid;  
	struct sigevent evp;  
	memset(&evp, 0, sizeof(struct sigevent));       //�����ʼ��  
  
	evp.sigev_value.sival_int = 111;            //Ҳ�Ǳ�ʶ��ʱ���ģ����timerid��ʲô���𣿻ص��������Ի��  
	evp.sigev_notify = SIGEV_THREAD;            //�߳�֪ͨ�ķ�ʽ����פ���߳�  
	evp.sigev_notify_function = timer_thread;       //�̺߳�����ַ  
  
	if (timer_create(CLOCKID, &evp, &timerid) == -1)  
	{  
		perror("fail to timer_create");  
		exit(-1);  
	}  
  
	// XXX int timer_settime(timer_t timerid, int flags, const struct itimerspec *new_value,struct itimerspec *old_value);  
	// timerid--��ʱ����ʶ  
	// flags--0��ʾ���ʱ�䣬1��ʾ����ʱ��  
	// new_value--��ʱ�����³�ʼֵ�ͼ�����������it  
	// old_value--ȡֵͨ��Ϊ0�������ĸ�������ΪNULL,����ΪNULL���򷵻ض�ʱ����ǰһ��ֵ  
	  
	//��һ�μ��it.it_value��ô��,�Ժ�ÿ�ζ���it.it_interval��ô��,����˵it.it_value��0��ʱ���װ��it.it_interval��ֵ  
	struct itimerspec it;  
	it.it_interval.tv_sec = 1;  
	it.it_interval.tv_nsec = 0;  
	it.it_value.tv_sec = 1;  
	it.it_value.tv_nsec = 0;  
  
	if (timer_settime(timerid, 0, &it, NULL) == -1)  
	{  
		perror("fail to timer_settime");  
		exit(-1);  
	}  
  
	pause();  
  
	return 0;  
}  
/* 
 * int timer_gettime(timer_t timerid, struct itimerspec *curr_value); 
 * ��ȡtimeridָ���Ķ�ʱ����ֵ������curr_value 
 * 
 */  