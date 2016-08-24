#define _GNU_SOURCE

#include<stdio.h>
#include<pthread.h>
#include<stdlib.h>
#include<sys/poll.h>
#include<unistd.h>
#include<time.h>
#include<linux/types.h>

#define NR_THREADS 10

#define COUNT 5000

int __thread count = 0;
int *pcount[NR_THREADS] = {NULL};
int finalcount = 0;


pthread_spinlock_t g_spinlock;

void inc_count()
{
	count++;
}

int read_count()
{
	int t;
	int sum ;

	pthread_spin_lock(&g_spinlock);
	sum = finalcount;
	for(t = 0;t<NR_THREADS;t++)
	{    
		 if(pcount[t] != NULL)
			sum += *pcount[t];
	}
	pthread_spin_unlock(&g_spinlock);
	return sum;
}


typedef struct
{
   int nr;
   pthread_t tid;
}Thread;

Thread Threads[NR_THREADS];

void count_register_thread(int idx)
{
	pthread_spin_lock(&g_spinlock);
	pcount[idx] = &count;
	pthread_spin_unlock(&g_spinlock);
}

void count_unregister_thread(int idx)
{
	pthread_spin_lock(&g_spinlock);
	finalcount += count;
	pcount[idx] = NULL;
	pthread_spin_unlock(&g_spinlock);
}

void *thr_function(void* arg)
{
   int nr = *(int*)arg;
   fprintf(stderr,"the %d thread init successful\n",nr);
   struct timespec delay = {0};
   delay.tv_sec = 0;
   delay.tv_nsec = 1000000;
   struct timeval tv_begin,tv_end;
   __u64 interval = 0;
	
   count_register_thread(nr);

   int i,j;
   
   gettimeofday(&tv_begin,NULL);
   for(i = 0;i<COUNT;i++)
   {
	 inc_count();
	 nanosleep(&delay,NULL);
   }
   gettimeofday(&tv_end,NULL);

   interval = (tv_end.tv_sec -tv_begin.tv_sec)*1000000               + (tv_end.tv_usec-tv_begin.tv_usec);
   fprintf(stderr,"the thread %d cost %llu us\n",                        nr,interval);
   count_unregister_thread(nr);
   fprintf(stderr,"the %d thread will exit\n",nr);
   return NULL;
}

int main()
{
	int i;

	pthread_spin_init(&g_spinlock,0);


	for(i= 0;i<NR_THREADS;i++)
	{
	   Threads[i].nr = i;
	   if(pthread_create(&Threads[i].tid,NULL,                                  thr_function,&Threads[i].nr))
	   {
			fprintf(stderr,"error happened when create                             thread %d\n",i);
			 return 0;
	   }
	}

	for(i= 0;i<10;i++)
	{

		poll(NULL,0,500);
		fprintf(stderr,"MASTER PROC: the finalcount is                         %d now \n",read_count());
	}

	for(i = 0;i<NR_THREADS;i++)
	{
		if(pthread_join(Threads[i].tid,NULL))
		{
			  fprintf(stderr,"error happened when join                               the thread %d\n",i);
			  return ;
		}
	}

	fprintf(stderr,"MASTER PROC: at last finalcount is                     %d\n　",finalcount);
	return 0;
}