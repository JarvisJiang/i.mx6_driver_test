#include "stdio.h"
#include "unistd.h"
#include "sys/types.h"
#include "sys/ioctl.h"
#include "stdlib.h"
#include "termios.h"
#include "sys/stat.h"
#include "fcntl.h"
#include "sys/time.h"
//super_system
#include <errno.h>
#include <string.h>

#define LED_BRIGHTNESS    "/sys/class/leds/led1/brightness"
void itoa(int value, char *str);

int len(char ch[]){
    int i=0;
    while (ch[i]!='\0'){                 
          i++;                   
    }            
    return i;
}



/*瀛楃涓叉彃鍏ュ瓧绗︿覆鍑芥暟*/

int chIns(char ch[],char ch1[],int k)
{
     int i;
     int len_ch=len(ch);
     int len_ch1=len(ch1);
     if (k>len_ch) {
        printf("鎻掑叆鐨勪綅缃ぇ浜庣涓�涓瓧绗︿覆鐨勯暱搴︼紝绋嬪簭閫�鍑?");
        return -1;            
     }
     if(k>199){
	printf("insert size err\n");
	}
     else {
         for (i=len_ch+len_ch1-1;i>=k+len_ch1;i--){

             ch[i]=ch[i-len_ch1];       /*绉诲姩瀛楃灏嗚鎻掑叆鐨勪綅缃┖鍑烘潵*/

         }
         for (i=0;i<len_ch1;i++){
             ch[k+i]=ch1[i];     /*鎻掑叆瀛楃涓*/                       
         }
		 
         ch[len_ch+len_ch1]='\0';  /*璁剧疆瀛楃缁撴*/
         return 1;
     }               
}


//happen with PWM7 (CSI_VSYNC)
#define PWM7_INIT						 "echo 0 > /sys/class/pwm/pwmchip6/export"

#define PWM7_UNINIT						 "echo 0 > /sys/class/pwm/pwmchip6/unexport"

#define PWM7_ENABLE						 "echo 1 >/sys/class/pwm/pwmchip6/pwm0/enable"	
#define PWM7_DISABLE					 "echo 0 >/sys/class/pwm/pwmchip6/pwm0/enable"
#define PWM7_SET_PERIOD(PAR)         	 	 	 "echo PAR > /sys/class/pwm/pwmchip6/pwm0/period"
#define PWM7_SET_DUTY_CYCLE 		 	 "echo 500000 > /sys/class/pwm/pwmchip6/pwm0/duty_cycle"
#define PWM7_GET_ENABLE					 "cat /sys/class/pwm/pwmchip6/pwm0/enable"
#define PWM7_GET_PERIOD					 "cat /sys/class/pwm/pwmchip6/pwm0/period"
#define PWM7_GET_DUTY_CYCLE				 "cat /sys/class/pwm/pwmchip6/pwm0/duty_cycle"




//set csi_data00(gpio04_21) as  dir   //测试成功
#define IO_DIR_INIT                      "echo 0 > /sys/class/leds/led1/brightness"


#define IO_DIR_LOW                      "echo 1 > /sys/class/leds/led1/brightness"  //碌脥碌莽脝艙
#define IO_DIR_HIGH                     "echo 0 > /sys/class/leds/led1/brightness"  //啪脽碌莽脝艙

//set csi_data01(gpio04_22) as  /enable    测试成功
#define IO_EN_INIT                     "echo 0 > /sys/class/leds/led2/brightness"
#define IO_EN_LOW                      "echo 1 > /sys/class/leds/led2/brightness"  //碌脥碌莽脝艙
#define IO_EN_HIGH                     "echo 0 > /sys/class/leds/led2/brightness"  //啪脽碌莽脝艙


#define _50_PER_DUTY_CYCLE   500000


#define _1KHZ_PERIOD         1000000
#define _1HZ_PERIOD         1000000000

#define _10KHZ_PERIOD        1000000 


#define CMD_ENABLE_A4988     49
#define CMD_DISABLE_A4988	 59
#define CMD_DIR_POSITIVE	 69
#define CMD_DIR_NEGATIVE 	 79
#define CMD_UNINIT_A4988 	 89
void set_nstep_persecond(int n)
{
	int temp , speed;
	
	char str_set_period[200]   = "echo  > /sys/class/pwm/pwmchip6/pwm0/period";
	char str_period_value[32]  = "1000000";
    printf("**set_nstep_persecond\n");
	
	
	
	if(0==n)
		return;
	if(n>1500)
	{
		printf("err nstep is too large\n");
		return;
	}
	
	temp = _1HZ_PERIOD/n;
	printf("PWM7_TEMP  = %d \n",temp);
	speed = temp;
	itoa(speed,str_period_value);
	printf("str_period_value =%s\n",str_period_value);
	chIns(str_set_period,str_period_value,5);
	system(str_set_period);
	printf("str_set_period =%s\n",str_set_period);
	printf("PWM7 PERIOD SET OVER\n");
}

/*
1.8  * 200 step  = 1脠艩
*/
/*IO虏脵脳梅*/



static long chardevnode_ioctl( unsigned int cmd, unsigned long arg){
	long speed;
	char str_set_period[200]   = "echo  > /sys/class/pwm/pwmchip6/pwm0/period";
	char str_period_value[32]  = "1000000";
	printf("chardevnode_ioctl is success! cmd is %d,arg is %d \n",cmd,arg);
	speed = arg;
	itoa(speed,str_period_value);
	//printf("str_period_value =%s\n",str_period_value);
	//chIns(str_set_period,str_period_value,5);
	//system(str_set_period);
	/*
	system(PWM7_INIT);//export
	printf("PWM7_INIT \n");
	system("echo 1000000 > /sys/class/pwm/pwmchip6/pwm0/period");
	printf("no macro!!\n");
	system(PWM7_SET_PERIOD(1000000));
	printf("set period 001 \n");
	system(PWM7_SET_PERIOD(_1KHZ_PERIOD));
	printf("set period 002 \n");
	system(PWM7_SET_PERIOD(_10KHZ_PERIOD));	
	printf("set period 003 \n");*/
	switch(cmd)
	{
		case CMD_ENABLE_A4988:
		{
			system(PWM7_INIT);//export
			system(IO_EN_LOW);
			system(PWM7_ENABLE);
		}
		break;
		case CMD_DISABLE_A4988:
		{
			system(IO_EN_HIGH);
			system(PWM7_DISABLE);
		}
		break;
		case CMD_DIR_POSITIVE:
		{
			system(IO_DIR_HIGH);
		}
		break;
		case CMD_DIR_NEGATIVE:
		{
			system(IO_DIR_LOW);
		}
		break;
		case CMD_UNINIT_A4988:
		{
			system(PWM7_DISABLE);
			system(PWM7_UNINIT);
		}
		break;
		default:
		printf("cmd is err please check!!! \n");
		break;
	}
	if(speed == 0)
	{
		system(PWM7_DISABLE);
	}
	else
	{	
		printf("set speed = %d!!!\r\n",speed);
		system(PWM7_ENABLE);
		system(PWM7_DISABLE);
		
		set_nstep_persecond(speed);
		system("echo 500000 > /sys/class/pwm/pwmchip6/pwm0/duty_cycle");//50%

		printf("duty_cycle over!\n");
		system(PWM7_ENABLE);
	}
	
	
	return 0;
}
/*
gpio01_4  --->  pwm3
*/
int super_system(const char * cmd, char *retmsg, int msg_len)
{
        FILE * fp;
        int res = -1;
        if (cmd == NULL || retmsg == NULL || msg_len < 0)
        {
                printf("Err: Fuc:%s system paramer invalid!\n", __func__);
                return 1;
        }
        if ((fp = popen(cmd, "r") ) == NULL)
        {
                perror("popen");
                printf("Err: Fuc:%s popen error: %s\n", __func__, strerror(errno));
                return 2;
        }
        else
        {
                memset(retmsg, 0, msg_len);
                while(fgets(retmsg, msg_len, fp));
                {
                        printf("Fuc: %s fgets buf is %s\n", __func__, retmsg);
                }
                if ( (res = pclose(fp)) == -1)
                {
                        printf("Fuc:%s close popen file pointer fp error!\n", __func__);
                        return 3;
                }
                //drop #012 from system result retmsg.
                retmsg[strlen(retmsg)-1] = '\0';
                return 0;
        }
}
//happen with PWM3 (GPIO01_04)
#define PWM3_INIT						 "echo 0 > /sys/class/pwm/pwmchip2/export"

#define PWM3_ENABLE						 "echo 1 >/sys/class/pwm/pwmchip2/pwm0/enable"	
#define PWM3_DISABLE					 "echo 0 >/sys/class/pwm/pwmchip2/pwm0/enable"
#define PWM3_SET_PERIOD(t_us)         	 "echo t_us > /sys/class/pwm/pwmchip2/pwm0/period"
#define PWM3_SET_DUTY_CYCLE(dc) 		 "echo dc > /sys/class/pwm/pwmchip2/pwm0/duty_cycle"
#define PWM3_GET_ENABLE					 "cat /sys/class/pwm/pwmchip2/pwm0/enable"
#define PWM3_GET_PERIOD					 "cat /sys/class/pwm/pwmchip2/pwm0/period"
#define PWM3_GET_DUTY_CYCLE				 "cat /sys/class/pwm/pwmchip2/pwm0/duty_cycle"






#define  PWM3  3
#define  PWM7  7
void pwm_init(int opt)
{
	
	switch(opt)
	{
		case PWM3:
		if(-1==system(PWM3_INIT))
		{
		printf("err\r\n");
		}	
		break;
		case PWM7:
		if(-1==system(PWM7_INIT))
		{
			printf("err\r\n");
		}
		break;
		default:
		printf("this PWM is not config");
		break;
	}
}
#define PWM_SET_PERIOD 
void pwm_config(int opt,int p_t, int p_dc)
{
	#ifdef PWM_SET_PERIOD
	#undef PWM_SET_PERIOD
	#endif
	switch(opt)
	{
		case PWM3:
		if(-1==system(PWM3_SET_PERIOD(p_t)))
		{
			printf("err\r\n");
		}
		if(-1==system(PWM3_SET_DUTY_CYCLE(p_dc)))
		{
			printf("err\r\n");
		}
		break;
		case PWM7:
		if(-1==system(PWM7_SET_PERIOD(p_t)))
		{
			printf("err\r\n");
		}
//		if(-1==system(PWM7_SET_DUTY_CYCLE(p_dc)))
		{
			printf("err\r\n");
		}
		break;
		default:
		printf("this PWM is not config");
		return;
	}
}
void enable_pwm(int opt)
{
	switch(opt)
	{
		case PWM3:
		if(-1==system(PWM3_ENABLE))
		{
		printf("err\r\n");
		}
		break;
		case PWM7:
		if(-1==system(PWM7_ENABLE))
		{
		printf("err\r\n");
		}
		break;
		default:
		break;
	}

}
void disable_pwm3(int opt)
{
	switch(opt)
	{
		case PWM3:
		if(-1==system(PWM3_DISABLE))
		{
		printf("err\r\n");
		}
		break;
		case PWM7:
		if(-1==system(PWM7_DISABLE))
		{
		printf("err\r\n");
		}
		break;
		default:
		break;
	}
}
/*
#define CMD_SPEED_UP 		0X1
#define CMD_SPEED_DOWN 	0X2
#define CMD_RIGHT 			0X3
#define CMD_LEFT 			0X4
#define CMD_ENABLE_PWM3   	0X5
#define CMD_DISABLE_PWM3   	0X6
void control_stepper(int cmd, int arg1,int arg2)
{
	switch(cmd)
	{
		case CMD_SPEED_UP:
		break;
		case CMD_SPEED_DOWN:
		break;
		case CMD_RIGHT:
		break;
		case CMD_LEFT:
		break;
		case CMD_ENABLE_PWM3:
		break;
		case CMD_DISABLE_PWM3:
		break;
		default:
		break;
	}
}
*/
void itoa(int value, char *str)
{
    if (value < 0) //如果是负数,则str[0]='-',并把value取反(变成正整数)

    {
        str[0] = '-';
        value = 0-value;
    }
    int i,j;
    for(i=1; value > 0; i++,value/=10) //从value[1]开始存放value的数字字符，不过是逆序，等下再反序过来

        str[i] = value%10+'0'; //将数字加上0的ASCII值(即'0')就得到该数字的ASCII值

    for(j=i-1,i=1; j-i>=1; j--,i++) //将数字字符反序存放

    {
        str[i] = str[i]^str[j];
        str[j] = str[i]^str[j];
        str[i] = str[i]^str[j];
    }
    if(str[0] != '-') //如果不是负数，则需要把数字字符下标左移一位，即减1

    {
        for(i=0; str[i+1]!='\0'; i++)
            str[i] = str[i+1];
        str[i] = '\0';
    }
}

int main(int argc,char *argv[])
{
	
	int i,j;
	int ibuf[4];
	char str_set_period[200]   = "echo   > /sys/class/pwm/pwmchip6/pwm0/period";
	char str_period_value[32]  = "1000000";
	char temps[32];
	int temp;
	int fd;
	temp = 789654;
	i = atoi(argv[1]);
	j = atoi(argv[2]);
	itoa(temp,temps);
	printf("temps = %s \r\n",temps);
	//fd=open(LED_BRIGHTNESS, O_WRONLY);
//	chIns(str_set_period,str_period_value,5);
//	system(str_set_period);
	//argv[0] 鲁脤脨貌脙没
	printf("str_set_period = %s\n",str_set_period);
	printf("the Program name is %s\n",argv[0]);
	printf("the argv1 is %s\n",argv[1]);
	printf("the argv2 name is %s\n",argv[2]);
	printf("The command line has %d argument:\n",argc-1);
	
	
	printf("%d,%d\n",i,j);
	chardevnode_ioctl(i,j);
	//close(fd);
	return 0;
}






