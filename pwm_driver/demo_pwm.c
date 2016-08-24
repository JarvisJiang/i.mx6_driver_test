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
//happen with PWM7 (CSI_VSYNC)
#define PWM7_INIT						 "echo 0 > /sys/class/pwm/pwmchip6/export"

#define PWM7_UNINIT						 "echo 0 > /sys/class/pwm/pwmchip6/export"

#define PWM7_ENABLE						 "echo 1 >/sys/class/pwm/pwmchip6/pwm0/enable"	
#define PWM7_DISABLE					 "echo 0 >/sys/class/pwm/pwmchip6/pwm0/enable"
#define PWM7_SET_PERIOD(t_us)         	 "echo t_us > /sys/class/pwm/pwmchip6/pwm0/period"
#define PWM7_SET_DUTY_CYCLE(dc) 		 "echo dc > /sys/class/pwm/pwmchip6/pwm0/duty_cycle"
#define PWM7_GET_ENABLE					 "cat /sys/class/pwm/pwmchip6/pwm0/enable"
#define PWM7_GET_PERIOD					 "cat /sys/class/pwm/pwmchip6/pwm0/period"
#define PWM7_GET_DUTY_CYCLE				 "cat /sys/class/pwm/pwmchip6/pwm0/duty_cycle"


//set csi_data00(gpio04_21) as  dir  
#define IO_DIR_INIT                      "echo 0 > /sys/class/leds/led2/brightness"


#define IO_DIR_LOW                      "echo 1 > /sys/class/leds/led2/brightness"  //低电平
#define IO_DIR_HIGH                     "echo 0 > /sys/class/leds/led2/brightness"  //高电平

//set csi_data01(gpio04_22) as  /enable  
#define IO_EN_INIT                     "echo 0 > /sys/class/leds/led3/brightness"
#define IO_EN_LOW                      "echo 1 > /sys/class/leds/led3/brightness"  //低电平
#define IO_EN_HIGH                     "echo 0 > /sys/class/leds/led3/brightness"  //高电平


#define _50_PER_DUTY_CYCLE   500000
#define _1KHZ_PERIOD         1000000
#define _1HZ_PERIOD         1000000000

#define CMD_ENABLE_A4988     49
#define CMD_DISABLE_A4988	 59
#define CMD_DIR_POSITIVE	 69
#define CMD_DIR_NEGATIVE 	 79

void set_nstep_persecond(int n)
{
	int temp;
	
	if(0==n)
		return;
	
	temp = _1HZ_PERIOD/n;
	system(PWM7_SET_PERIOD(temp));
}

/*
1.8  * 200 step  = 1圈
*/
/*IO操作*/
static long chardevnode_ioctl(struct file *file, unsigned int cmd, unsigned long arg){
	printf("chardevnode_ioctl is success! cmd is %d,arg is %d \n",cmd,arg);
	long speed;
	arg = speed;
	switch(cmd)
	{
		case CMD_ENABLE_A4988:
		{
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
		
		system(PWM7_ENABLE);
		system(PWM7_DISABLE);
		set_nstep_persecond(speed);
		PWM7_SET_DUTY_CYCLE(_50_PER_DUTY_CYCLE);
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


//happen with PWM7 (CSI_VSYNC)   control steper 
#define PWM7_INIT						 "echo 0 > /sys/class/pwm/pwmchip6/export"

#define PWM7_ENABLE						 "echo 1 >/sys/class/pwm/pwmchip6/pwm0/enable"	
#define PWM7_DISABLE					 "echo 0 >/sys/class/pwm/pwmchip6/pwm0/enable"
#define PWM7_SET_PERIOD(t_us)         	 "echo t_us > /sys/class/pwm/pwmchip6/pwm0/period"
#define PWM7_SET_DUTY_CYCLE(dc) 		 "echo dc > /sys/class/pwm/pwmchip6/pwm0/duty_cycle"
#define PWM7_GET_ENABLE					 "cat /sys/class/pwm/pwmchip6/pwm0/enable"
#define PWM7_GET_PERIOD					 "cat /sys/class/pwm/pwmchip6/pwm0/period"
#define PWM7_GET_DUTY_CYCLE				 "cat /sys/class/pwm/pwmchip6/pwm0/duty_cycle"





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
		if(-1==system(PWM7_SET_DUTY_CYCLE(p_dc)))
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
int main(int argc,char *argv[])
{
	
	int i,j;
	int ibuf[4];
	i = atoi(argv[1]);
	j = atoi(argv[2]);
	
	//argv[0] 程序名
	printf("the Program name is %s\n",argv[0]);
	
	printf("The command line has %d argument:\n",argc-1);
	
	
	printf("%d,%d\n",i,j);
	
	return 0;
}






