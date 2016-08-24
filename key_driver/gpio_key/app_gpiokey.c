#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <time.h>
#include <fcntl.h>
#include <linux/input.h>

/*
gpio-keys{  
       compatible = "gpio-keys";  
        home {  
                label = "HOME Button";  
                gpios = <&gpio1 22 1>;   //  8 
                linux,code = <102>;  
        };  
        enter {  
                label = "Enter Button";  //3
                gpios = <&gpio1 23 1>;  
                linux,code = <28>;  
        };     
	};
#define KEY_HOME		102
#define KEY_ENTER		28


*/
int main(int argc, char **argv)
{
     int key_state;
     int fd;
     int ret;
     int code;
     struct input_event buf;
     int repeat_param[2];
     fd = open("/dev/input/event0", O_RDONLY);
     if (fd < 0) 
     {
         printf("Open gpio-keys failed.\n");
         return -1;
  }
  else
  {
     printf("Open gpio-keys success.\n");
  }
     repeat_param[0]=500;//ms重复按键第一次间隔
     repeat_param[1]=66;//ms重复按键后续间隔
     ret = ioctl(fd,EVIOCSREP,(int *)repeat_param);//设置重复按键参数
     if(ret != 0)
         {
                   printf("set repeat_param fail!\n");
         }
     else
         {
                printf("set repeat_param ok.\n");
         }

     while(1)
     {
         ret = read(fd,&buf,sizeof(struct input_event));
         if(ret <= 0)
              {
                   printf("read fail!\n");
                   return -1;
              }

       code = buf.code;
       key_state = buf.value;
	   printf("code = %d\n");
       switch(code)
       {
          case KEY_HOME:
              code = 4;
              break;
          case KEY_ENTER:
              code = 8;
              break;
      
          default:
              code = 0;
              break;
       }

       if(code!=0)
          {
           printf("Key_%c state= %d.\n",code,key_state);
         }
     }
     close(fd);
     printf("Key test finished.\n");  
     return 0;
}