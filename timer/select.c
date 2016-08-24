#include <sys/time.h>  
#include <sys/select.h>  
#include <time.h>  
#include <stdio.h>  

#include "unistd.h"
#include "sys/types.h"
#include "sys/ioctl.h"
#include "stdlib.h"
#include "termios.h"
#include "sys/stat.h"
#include "fcntl.h"
#include "sys/time.h"
 
 
#define LED_BRIGHTNESS    "/sys/class/leds/led1/brightness"
#define LED1_ON       "echo 0 > /sys/class/leds/led1/brightness"
#define LED2_ON       "echo 0 > /sys/class/leds/led2/brightness"
#define LED1_OFF      "echo 1 > /sys/class/leds/led1/brightness"
#define LED2_OFF       "echo 1 > /sys/class/leds/led2/brightness"
/*seconds: the seconds; mseconds: the micro seconds*/  
void setTimer(int seconds, int mseconds)  
{  
		static int flag = 0;
        struct timeval temp;  
		
        temp.tv_sec = seconds;  
        temp.tv_usec = mseconds;  
		system(LED1_OFF);
        select(0, NULL, NULL, NULL, &temp);  
      //  printf("timer\n");  
		if(flag == 0)
		{
		
			system(LED2_ON);
			
			flag = 1;
		}
		else
		{
		
			system(LED2_OFF);
			//system(LED2_ON);
			flag = 0;
		}
        return ;  
}  
  
int main()  
{  
        int i;  
  
        for(i = 0 ; i < 1; i++)  
                setTimer(0, 500);  
  
        return 0;  
} 


