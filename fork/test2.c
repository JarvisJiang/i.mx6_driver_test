#include <unistd.h>  
#include <stdio.h>   




/* 
 *  fork_test.c 
 *  version 3 
 *  Created on: 2010-5-29 
 *      Author: wangth 
 */  
#include <unistd.h>  
#include <stdio.h>  


int main(void)  
{  
   int i=0;  
   pid_t fpid = fork();
   #if 0
   for(i=0;i<1;i++){  
       pid_t fpid=fork();  
       if(fpid==0)  
           printf("son\n");  
       else  
           printf("father\n");  
   }
#endif   
   return 0;  
  
} 
#if 0
int main ()   
{   
    pid_t fpid; //fpid表示fork函数返回的值  
    int count=0;  
    fpid=fork();   
    if (fpid < 0)   
        printf("error in fork!");   
    else if (fpid == 0) {  
        printf("i am the child process, my process id is %d/n",getpid());   
        printf("I AM SON/n");//对某些人来说中文看着更直白。  
        count++;  
    }  
    else {  
        printf("i am the parent process, my process id is %d/n",getpid());   
        printf("I AM FATHER/n");  
        count++;  
    }  
    printf("统计结果是: %d/n",count);  
    return 0;  
} 
#endif