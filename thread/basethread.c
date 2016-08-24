#include <stdlib.h>

#include <stdio.h>

#include <pthread.h>

#include <time.h>



int myRand(void* cnt)

 {

         int min = 60;

          int max = 100;

          int randCnt = *((int *)cnt);

         int i = 0;

          pthread_t thread_id = pthread_self();



         

          srand((unsigned int)time(NULL));

         for(; i < randCnt; i ++){

                 

                 printf("thread_id = %d rand()%02d = %d\n",

                                  thread_id, i, min + rand() % (max - min));

                  sleep(1);

         }

         //return 11;

         pthread_exit((void*)11);

 }



 int main(int argc, char* argv[])

 {

          pthread_t tid;

         void* result;

          int reqRandCnt = 10;



         if(pthread_create(&tid, NULL, (void *)myRand, (void *)&reqRandCnt) == 0){

                  printf("myRand thread create OK!\n");

                 //pthread_detach(tid);

          }
		printf("11111111111\n");
         if(pthread_join(tid, &result) == 0){

                 printf("thread tid = %d, result = %d\n", tid, (int)result);

          }

         return 0;

         //pthread_exit((void*)22);

 }