/*������ʼ���궨���ͷ�ļ�,�����е�module_init��module_exit�ڴ��ļ���*/
#include <linux/init.h>
/*������ʼ������ģ���ͷ�ļ�,�����е�MODULE_LICENSE�ڴ�ͷ�ļ���*/
#include <linux/module.h>
/*����module_param module_param_array��ͷ�ļ�*/
#include <linux/moduleparam.h>
/*����module_param module_param_array��perm��ͷ�ļ�*/
#include <linux/stat.h>
/*�����ַ��豸����*/
#include <linux/fs.h>
/*MKDEVת���豸���������͵ĺ궨��*/
#include <linux/kdev_t.h>
/*�����ַ��豸�Ľṹ��*/
#include <linux/cdev.h>
/*�����ڴ�ռ亯��ͷ�ļ�*/
#include <linux/slab.h>

/*��������device_create �ṹ��class��ͷ�ļ�*/
#include <linux/device.h>
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

#define DEVICE_NAME "a4988_node"
#define DEVICE_MINOR_NUM 1
#define DEV_MAJOR 0
#define DEV_MINOR 0
#define REGDEV_SIZE 3000

MODULE_LICENSE("Dual BSD/GPL");
/*�����ǿ�Դ�ģ�û���ں˰汾����*/
MODULE_AUTHOR("PD_Jarvis");
/*��������*/




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


#define IO_DIR_LOW                      "echo 1 > /sys/class/leds/led2/brightness"  //�͵�ƽ
#define IO_DIR_HIGH                     "echo 0 > /sys/class/leds/led2/brightness"  //�ߵ�ƽ

//set csi_data01(gpio04_22) as  /enable  
#define IO_EN_INIT                     "echo 0 > /sys/class/leds/led3/brightness"
#define IO_EN_LOW                      "echo 1 > /sys/class/leds/led3/brightness"  //�͵�ƽ
#define IO_EN_HIGH                     "echo 0 > /sys/class/leds/led3/brightness"  //�ߵ�ƽ


#define _50_PER_DUTY_CYCLE   500000
#define _1KHZ_PERIOD         1000000
#define _1HZ_PERIOD         1000000000
int numdev_major = DEV_MAJOR;
int numdev_minor = DEV_MINOR;
/*

*/
/*�������豸��*/
module_param(numdev_major,int,S_IRUSR);
/*������豸��*/
module_param(numdev_minor,int,S_IRUSR);

static struct class *myclass;

struct reg_dev
{
	char *data;
	unsigned long size;
	
	struct cdev cdev;
};
struct reg_dev *my_devices;
/*�򿪲���*/
static int chardevnode_open(struct inode *inode, struct file *file){
	printk(KERN_EMERG "chardevnode_open is success!\n");
	system(PWM7_INIT);
	system(PWM7_DISABLE);
	
	return 0;
}
/*�رղ���*/
static int chardevnode_release(struct inode *inode, struct file *file){
	printk(KERN_EMERG "chardevnode_release is success!\n");
	system(PWM7_UNINIT);
	system(PWM7_DISABLE);
	return 0;
}

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
1.8  * 200 step  = 1Ȧ
*/
/*IO����*/
static long chardevnode_ioctl(struct file *file, unsigned int cmd, unsigned long arg){
	printk(KERN_EMERG "chardevnode_ioctl is success! cmd is %d,arg is %d \n",cmd,arg);
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
		printk(KERN_EMERG "cmd is err please check!!! \n");
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

ssize_t chardevnode_read(struct file *file, char __user *buf, size_t count, loff_t *f_ops){
	return 0;
}

ssize_t chardevnode_write(struct file *file, const char __user *buf, size_t count, loff_t *f_ops){
	return 0;
}

loff_t chardevnode_llseek(struct file *file, loff_t offset, int ence){
	return 0;
}
struct file_operations my_fops = {
	.owner = THIS_MODULE,
	.open = chardevnode_open,
	.release = chardevnode_release,
	.unlocked_ioctl = chardevnode_ioctl,
	.read = chardevnode_read,
	.write = chardevnode_write,
	.llseek = chardevnode_llseek,
};


/*�豸ע�ᵽϵͳ*/
static void reg_init_cdev(struct reg_dev *dev,int index){
	int err;
	int devno = MKDEV(numdev_major,numdev_minor+index);
	
	/*���ݳ�ʼ��*/
	cdev_init(&dev->cdev,&my_fops);
	dev->cdev.owner = THIS_MODULE;
	dev->cdev.ops = &my_fops;
	
	/*ע�ᵽϵͳ*/
	err = cdev_add(&dev->cdev,devno,1);
	if(err){
		printk(KERN_EMERG "cdev_add %d is fail! %d\n",index,err);
	}
	else{
		printk(KERN_EMERG "cdev_add %d is success!\n",numdev_minor+index);
	}
}

static int scdev_init(void)
{
	int ret = 0,i;
	dev_t num_dev;
	
	
	printk(KERN_EMERG "numdev_major is %d!\n",numdev_major);
	printk(KERN_EMERG "numdev_minor is %d!\n",numdev_minor);
	
	if(numdev_major){
		num_dev = MKDEV(numdev_major,numdev_minor);
		ret = register_chrdev_region(num_dev,DEVICE_MINOR_NUM,DEVICE_NAME);
	}
	else{
		/*��̬ע���豸��*/
		ret = alloc_chrdev_region(&num_dev,numdev_minor,DEVICE_MINOR_NUM,DEVICE_NAME);
		/*������豸��*/
		numdev_major = MAJOR(num_dev);
		printk(KERN_EMERG "adev_region req %d !\n",numdev_major);
	}
	if(ret<0){
		printk(KERN_EMERG "register_chrdev_region req %d is failed!\n",numdev_major);		
	}
	myclass = class_create(THIS_MODULE,DEVICE_NAME);
	
	
	my_devices = kmalloc(DEVICE_MINOR_NUM * sizeof(struct reg_dev),GFP_KERNEL);
	if(!my_devices){
		ret = -ENOMEM;
		goto fail;
	}
	memset(my_devices,0,DEVICE_MINOR_NUM * sizeof(struct reg_dev));
	
	/*�豸��ʼ��*/
	for(i=0;i<DEVICE_MINOR_NUM;i++){
		my_devices[i].data = kmalloc(REGDEV_SIZE,GFP_KERNEL);
		memset(my_devices[i].data,0,REGDEV_SIZE);
		/*�豸ע�ᵽϵͳ*/
		reg_init_cdev(&my_devices[i],i);
		
		/*�����豸�ڵ�*/
		device_create(myclass,NULL,MKDEV(numdev_major,numdev_minor+i),NULL,DEVICE_NAME"%d",i);
	}
	
		
	printk(KERN_EMERG "scdev_init!\n");
	/*��ӡ��Ϣ��KERN_EMERG��ʾ������Ϣ*/
	return 0;

fail:
	/*ע���豸��*/
	unregister_chrdev_region(MKDEV(numdev_major,numdev_minor),DEVICE_MINOR_NUM);
	printk(KERN_EMERG "kmalloc is fail!\n");
	
	return ret;
}

static void scdev_exit(void)
{
	int i;
	printk(KERN_EMERG "scdev_exit!\n");
	
	/*��ȥ�ַ��豸*/
	for(i=0;i<DEVICE_MINOR_NUM;i++){
		cdev_del(&(my_devices[i].cdev));
		/*�ݻ��豸�ڵ㺯��d*/
		device_destroy(myclass,MKDEV(numdev_major,numdev_minor+i));
	}
	/*�ͷ��豸class*/
	class_destroy(myclass);
	/*�ͷ��ڴ�*/
	kfree(my_devices);
	
	unregister_chrdev_region(MKDEV(numdev_major,numdev_minor),DEVICE_MINOR_NUM);
}


module_init(scdev_init);
/*��ʼ������*/
module_exit(scdev_exit);
/*ж�غ���*/
