




//for module.
#include <linux/module.h>
#include <linux/init.h>
//for IRQ.
#include <linux/irq.h>
#include <linux/interrupt.h>
//for timer.
#include <linux/timer.h>
#include <linux/jiffies.h>

//for input framework.
#include <linux/input.h>

//for ioremap.
#include <linux/io.h>
//for kzalloc.
#include <linux/slab.h>
//for ulong.
#include <linux/types.h>

//timer add
#include <linux/kernel.h>

/*MKDEV转换设备号数据类型的宏定义*/
#include <linux/kdev_t.h>
/*定义字符设备的结构体*/
#include <linux/cdev.h>

/*包含函数device_create 结构体class等头文件*/
#include <linux/device.h>

/*Linux中申请GPIO的头文件*/
#include <linux/gpio.h>

/*驱动注册的头文件，包含驱动的结构体和注册和卸载的函数*/
#include <linux/platform_device.h>
/*注册杂项设备头文件*/
#include <linux/miscdevice.h>
/*注册设备节点的文件结构体*/
#include <linux/fs.h>
MODULE_LICENSE("Dual BSD/GPL");
/*声明是开源的，没有内核版本限制*/
MODULE_AUTHOR("PD JARVIS");
/*声明作者*/
/*

寄存器配置
gpio5_9
gpio1_9  


*/
#define DRIVER_NAME "int_gpioctl"
#define DEVICE_NAME "int_gpioctl"
#define DEVICE_MINOR_NUM 1
#define DEV_MAJOR 0
#define DEV_MINOR 0
#define REGDEV_SIZE 3000

/*测试不成功*/

int numdev_major = DEV_MAJOR;
int numdev_minor = DEV_MINOR;

/*

description:

添加GPIO5_9     LED2
GPIO1_9    控制 LED3
26.4.3.2 GPIO Write Mode
The programming sequence for driving output signals should be as follows:
1. Configure IOMUX to select GPIO mode (Via IOMUXC), also enable SION if
to read loopback pad value through PSR
2. Configure GPIO direction register to output (GPIO_GDIR[GDIR] set to 1b).
3. Write value to data register (GPIO_DR).


A pseudocode description to drive 4'b0101 on [output3:output0] is as follows:
// SET PADS TO GPIO MODE VIA IOMUX.

write sw_mux_ctl_pad_<output[0-3]>.mux_mode, <GPIO_MUX_MODE>
// Enable loopback so we can capture pad value into PSR in output mode
write sw_mux_ctl_pad_<output[0-3]>.sion, 1
// SET GDIR=1 TO OUTPUT BITS.
write GDIR[31:4,output3_bit,output2_bit, output1_bit, output0_bit,] 32'hxxxxxxxF
// WRITE OUTPUT VALUE=4’b0101 TO DR.
write DR, 32'hxxxxxxx5
// READ OUTPUT VALUE FROM PSR ONLY.
read_cmp PSR, 32'hxxxxxxx5
*/
#define IMX_GPIO_NR(port, index)        ((((port)-1)*32)+((index)&31))
#define led26 IMX_GPIO_NR(5, 9)
#define led74 IMX_GPIO_NR(1, 9)


#define IOMUXC_SW_MUX_CTL_PAD_SNVS_TAMPER9         0x20E0040 //gpio_5_9 
#define IOMUXC_SW_MUX_CTL_PAD_GPIO1_IO09           0x20E0080 //gpio_1_9

/**
 * PAD Control registers.
 * used to control the pad physical features.
 */
#define IOMUXC_SW_PAD_CTL_PAD_GPIO1_IO09                   0x20E030C
#define IOMUXC_SW_PAD_CTL_PAD_SNVS_TAMPER9                 0x20E02CC     







#define INT_GPIO1_INT7  90
#define INT_GPIO1_INT6  91
#define INT_GPIO1_INT5  92
#define INT_GPIO1_INT4  93
#define INT_GPIO1_INT3  94
#define INT_GPIO1_INT2  95
#define INT_GPIO1_INT1  96
#define INT_GPIO1_INT0  97

#define INT_GPIO1_COM_0_15 			98
#define INT_GPIO1_COM_16_31 		99
#define INT_GPIO2_COM_0_15 			100
#define INT_GPIO2_COM_16_31     	101
#define INT_GPIO3_COM_0_15 			102
#define INT_GPIO3_COM_16_31 		103
#define INT_GPIO4_COM_0_15 			104
#define INT_GPIO4_COM_16_31     	105
#define INT_GPIO5_COM_0_15 			106
#define INT_GPIO5_COM_16_31 		107


//io mux
#define IOMUXC_SW_MUX_CTL_PAD_UART2_RX_DATA  0x20E0098
//io pad
#define IOMUXC_SW_PAD_CTL_PAD_UART2_RX_DATA  0x20E0324



#define KEY_INPUT IMX_GPIO_NR(1, 21)  //UART2_RXD  gpio_1_21 key  uart 2 con_ pin8





//register size,it's always 32-bit,4 bytes in 32-bit CPU.
#define REG_SIZE        0x4



#define print_debug(fmt,arg...)     printk(KERN_DEBUG fmt,##arg)
#define print_error(fmt,arg...)        printk(KERN_ERR fmt,##arg)

//GPIO related register define here.
//up/down key,GPIO1[4].
#define GPIO1_DR         0X209C000
#define GPIO1_GDIR       0x209C004
#define GPIO1_PSR        0x209C008
#define GPIO1_IMR        0x209C014
#define GPIO1_EDGE_SEL   0x209C01C
#define GPIO1_ICR2       0x209C010
//left/right key,GPIO5[2 0].

#define GPIO5_DR         0X20AC000
#define GPIO5_GDIR       0x20AC004
#define GPIO5_PSR        0x20AC008
#define GPIO5_IMR        0x20AC014
//led key:GPIO6[31].
//ok key: GPIO6[11].
//return key: GPIO6[14].
#define GPIO6_DR         0X20B0000
#define GPIO6_GDIR       0x20B0004
#define GPIO6_PSR        0x20B0008
#define GPIO6_IMR        0x20B0014




struct imx6_keys_dev {
    //virtual address after mapping.
    volatile ulong *up_down_vir_addr; //GPIO1[4].
    volatile ulong up_down_status;

    volatile ulong *left_right_vir_addr; //GPIO5[20].
    volatile ulong left_right_status;

    volatile ulong *led_ok_return_vir_addr;//GPIO6[11/14/31].
    volatile ulong led_ok_return_status;
    //scan timer.
    struct timer_list scan_timer;

    //input core.
    struct input_dev *input_device;
};
struct timer_list timer;
struct imx6_keys_dev *key_dev;
/**
 * configure the register at initial stage.
 * these registers should only be configured once.
 */
 #if 0
 #define print_reg(fmt,arg...)     printk(KERN_EMERG fmt,##arg)
 #else
  #define print_reg(fmt,arg...)    
 #endif
int imx6_config_reg(ulong phy_addr, int bits_mask, int bits_value) {
    volatile ulong *pbase = NULL;
    volatile int ret;
    int i;
    int reg_bits_num = sizeof(ret)*8;
    pbase = (ulong*) ioremap(phy_addr, REG_SIZE);
    print_reg(KERN_EMERG "phy_addr = %lx bits_mask = %x bits_value=%x\n",phy_addr,bits_mask,bits_value);
    if (!pbase) {
        print_error(KERN_EMERG "error accord when ioremap()!\n");
        return -1;
    }
    
    ret = ioread32(pbase);
    print_reg(KERN_EMERG "ret = %x\n",ret);
    for (i = 0; i <   reg_bits_num; i++) {
        if (bits_mask & (0x1 << i)) {
            ret &= (~(0x1 << i));
        }
    }
    ret |= bits_value;
    print_reg(KERN_EMERG "value = %x pbase = %p\n",ret,pbase);
    iowrite32(ret, pbase);
    ret = ioread32(pbase);
    printk(KERN_EMERG "read = %x\n",ret);
    iounmap(pbase);
    pbase = NULL;
    return 0;
}




/**
 * configure the IOMUX registers to choose the right pin mode.
 #define IOMUXC_SW_MUX_CTL_PAD_GPIO1_IO00           0x20E005C  //gpio_1_0
#define IOMUXC_SW_MUX_CTL_PAD_GPIO1_IO01           0x20E0060  //gpio_1_1	
#define IOMUXC_SW_MUX_CTL_PAD_GPIO1_IO02           0x20E0064  //gpio_1_2
#define IOMUXC_SW_MUX_CTL_PAD_GPIO1_IO03           0x20E0068  //gpio_1_3
#define IOMUXC_SW_MUX_CTL_PAD_GPIO1_IO04           0x20E006C //gpio_1_4
#define IOMUXC_SW_MUX_CTL_PAD_SNVS_TAMPER9         0x20E0040 //gpio_5_9 
#define IOMUXC_SW_MUX_CTL_PAD_GPIO1_IO09           0x20E0080 //gpio_1_9
 */
 



int imx6_config_all_iomuxcs(void) {
    //IOMUXC_SW_MUX_CTL_PAD_SNVS_TAMPER9.[2:0]=101=0x5,GPIO1[4].
    //bits_mask=111=0x7,
    if (imx6_config_reg(IOMUXC_SW_MUX_CTL_PAD_SNVS_TAMPER9, 0x7, 0x5) < 0) {
        return -1;
    }
    
    //IOMUXC_SW_MUX_CTL_PAD_GPIO1_IO09,[2:0]=101=0x5,GPIO6[31].
    //bits_mask=111=0x7.
    if (imx6_config_reg(IOMUXC_SW_MUX_CTL_PAD_GPIO1_IO09, 0x7, 0x5) < 0) {
        return -1;
    }

    

    return 0;
}
/*
#define IOMUXC_SW_PAD_CTL_PAD_GPIO1_IO09                   0x20E030C
#define IOMUXC_SW_PAD_CTL_PAD_SNVS_TAMPER9                 0x20E02CC     
*/
//configure pad's physical feature.
int imx6_config_all_pads(void) {
    //[16]=1,Schmitt trigger input.
    //[15:14]=10,100Kohm pull up.
    //[13]=1,pull enabled.
    //[12]=1,pull/keeper enabled.
    //bits_mask= 1,1111,0000,0000,0000=0x1F000.
    //bits_value=1,1011,0000,0000,0000=0x1B000.

    //IOMUXC_SW_PAD_CTL_PAD_GPIO04.
    if (imx6_config_reg(IOMUXC_SW_PAD_CTL_PAD_GPIO1_IO09, 0x1F000, 0x800) < 0) {
        return -1;
  }
    //IOMUXC_SW_PAD_CTL_PAD_EIM_BCLK.
  if (imx6_config_reg(IOMUXC_SW_PAD_CTL_PAD_SNVS_TAMPER9, 0x1F000, 0x800) < 0) {
      return -1;
   }

    return 0;
}

//configure GPIO registers.
int imx6_config_all_gpios(void) {
    //up/down key,GPIO1[4].
    //[4]=0,direction:input.
    //bits_mask=0001,0000=0x10.
    //bits_value=0000,0000=0x0.
    if (imx6_config_reg(GPIO1_GDIR, 0x200, 0x200) < 0) {
        return -1;
    }
    //[4]=0,disable interrupt.
    if (imx6_config_reg(GPIO1_IMR, 0x200, 0x200) < 0) {
        return -1;
    }

    //left/right key,GPIO5[20].
    //[20]=0,direction,input.
    //bits_mask=1,0000,0000,0000,0000,0000=0x100000.
    //bits_value=0,0000,0000,0000,0000,0000=0x0.
    if (imx6_config_reg(GPIO5_GDIR, 0x200, 0x0) < 0) {
        return -1;
    }
    //[4]=0,disable interrupt.
    if (imx6_config_reg(GPIO5_IMR, 0x200, 0x0) < 0) {
        return -1;
    }


    return 0;
}

//static struct class *myclass;

struct reg_dev
{
    char *data;
    unsigned long size;
    
    struct cdev cdev;
};
struct reg_dev *my_devices;

/*IO操作*/
static long gpio_ioctl(struct file *file, unsigned int cmd, unsigned long arg){
    printk(KERN_EMERG "chardevnode_ioctl is success! cmd is %d,arg is %ld \n",cmd,arg);

    switch(cmd)
    {
        case 1://gpio_1_9
            if(0==arg)
            {
                printk(KERN_EMERG "set 19 0\n");
                imx6_config_reg(GPIO1_DR,0x200, 0x0);
            }
            else
            {
                printk(KERN_EMERG "set 19 1\n");
                imx6_config_reg(GPIO1_DR,0x200, 0x200);
            }
        break;
        case 2://gpio_5_9
            if(0==arg)
            {
                printk(KERN_EMERG "set 59 0\n");
                imx6_config_reg(GPIO5_DR,0x200, 0x200);
            }
            else
            {
                printk(KERN_EMERG "set 59 1\n");
                imx6_config_reg(GPIO5_DR,0x200, 0x0);
            }
        break;
        default:
        printk(KERN_EMERG "err cmd arg \n");
        break;
    }
    return 0;
}




static int gpio_release(struct inode *inode, struct file *file){
    printk(KERN_EMERG "gpio release rere\n");
    return 0;
}

static int gpio_open(struct inode *inode, struct file *file){
    printk(KERN_EMERG "gpio open okok\n");
    return 0;
}

static struct file_operations gpio_ops = {
    .owner = THIS_MODULE,
    .open = gpio_open,
    .release = gpio_release,
    .unlocked_ioctl = gpio_ioctl,
};

static  struct miscdevice gpio_dev = {
    .minor = MISC_DYNAMIC_MINOR,
    .name = DEVICE_NAME,
    .fops = &gpio_ops,
};

/*
IOMUXC_SW_PAD_CTL_PAD_DISP0_DATA05  0x1B0B0
IOMUXC_SW_MUX_CTL_PAD_DISP0_DATA05  0x00000005
GPIOx_GDIR             				0x00000000
GPIOx_ICR2              			0x00200000
GPIOx_IMR               			0x04000000
GPIOx_EDGE_SEL    					0x00000000

*/

//UART2_RXD  gpio_1_21 key  uart 2 con_ pin8
static irqreturn_t key_interrupt_1_21(int irq, void *dev_id) {

        printk("%s(%d)\n", __FUNCTION__, __LINE__);

        return IRQ_HANDLED;
}
int g_num_irq;
static int key_config(void)
{

	int ret;
	
	ret = gpio_request(KEY_INPUT,"INT_IO_1_21");
    if(ret < 0){
        printk(KERN_EMERG "gpio_request led26 59 failed!\n");
        return ret;
     }

	// 复用 
	if (imx6_config_reg(IOMUXC_SW_MUX_CTL_PAD_UART2_RX_DATA, 0x7, 0x5) < 0) {
        return -1;
    }

    //[16]=1,Schmitt trigger input.
    //[15:14]=10,100Kohm pull up.
    //[13]=1,pull enabled.
    //[12]=1,pull/keeper enabled.
    //bits_mask= 1,1111,0000,0000,0000=0x1F000.
    //bits_value=1,1011,0000,0000,0000=0x1B000.

    //IOMUXC_SW_PAD_CTL_PAD_GPIO04.
    if (imx6_config_reg(IOMUXC_SW_PAD_CTL_PAD_UART2_RX_DATA, 0xff0f0, 0x1B0B0) < 0) {
        return -1;
  	}
  	//设置成输入
  	if (imx6_config_reg(GPIO1_GDIR, 0x200000, 0) < 0) {
        return -1;
  	}
  	//设置成下降沿敏感
  	/*
		00 LOW_LEVEL — Interrupt n is low-level sensitive.
		01 HIGH_LEVEL — Interrupt n is high-level sensitive.
		10 RISING_EDGE — Interrupt n is rising-edge sensitive.
		11 FALLING_EDGE — Interrupt n is falling-edge sensitive.
  	*/
  	if (imx6_config_reg(GPIO1_ICR2, 0x200000, 0x300000) < 0) {
        return -1;
  	}

  	if(imx6_config_reg(GPIO1_IMR, 0x200000, 0x200000) < 0) {
        return -1;
  	}
  	if(imx6_config_reg(GPIO1_EDGE_SEL, 0x200000, 0x200000) < 0) {
        return -1;
  	}
  	gpio_free(KEY_INPUT);
  	/*
  	   gpio当作中断口使用

        int gpio_to_irq(unsigned gpio); 

        返回的值即中断编号可以传给request_irq()和free_irq()

        内核通过调用该函数将gpio端口转换为中断，在用户空间也有类似方法
  	*/
    g_num_irq   = 	gpio_to_irq(KEY_INPUT);
    if(g_num_irq < 0)
    {
    	printk(KERN_EMERG "can not request_irq num\n");
    	return -1;
    }
    printk(KERN_EMERG "num_irq =%d\n",g_num_irq);
  	ret = request_irq((unsigned int)g_num_irq, key_interrupt_1_21,
                        (unsigned long)IRQ_TYPE_EDGE_FALLING /*IRQF_TRIGGER_FALLING*/, "INT_GPIO1_COM_16_31", (void *)&gpio_dev);
    if (ret < 0) {

            printk("Request IRQ:%d failed, %d\n", KEY_INPUT, ret);
       
    }
    return 0;
}


#if 0
static int gpio_probe(struct platform_device *pdv){
    int ret;
    
    printk(KERN_EMERG "\tinitialized\n");  
    ret = gpio_request(led26,"LED59");
    if(ret < 0){
        printk(KERN_EMERG "gpio_request led26 failed!\n");
        return ret;
     }
    ret = gpio_request(led74,"LEDS19");
    if(ret < 0){
        printk(KERN_EMERG "gpio_request led74 failed!\n");
        return ret;
     }
    imx6_config_all_iomuxcs();
    imx6_config_all_pads();
    imx6_config_all_gpios();
    misc_register(&gpio_dev);
    printk ("probe initialized!\n");
    return 0;
}

static int gpio_remove(struct platform_device *pdv){
    
    printk(KERN_EMERG "\tremove\n");
    misc_deregister(&gpio_dev);
    return 0;
}

static void gpio_shutdown(struct platform_device *pdv){
    
    ;
}

static int gpio_suspend(struct platform_device *pdv,pm_message_t pmt){
    
    return 0;
}

static int gpio_resume(struct platform_device *pdv){
    
    return 0;
}

struct platform_driver gpio_driver = {
    .probe = gpio_probe,
    .remove = gpio_remove,
    .shutdown = gpio_shutdown,
    .suspend = gpio_suspend,
    .resume = gpio_resume,
    .driver = {
        .name = DRIVER_NAME,
        .owner = THIS_MODULE,
    }
};

#endif
// static int gpio_init(void)
// {
//     int DriverState;
    
//     printk(KERN_EMERG "gpio WORLD enter22!\n");
//     DriverState = platform_driver_register(&gpio_driver);
    
//     printk(KERN_EMERG "\tDriverState is %d\n",DriverState);
//     return 0;
// }


// static void gpio_exit(void)
// {
//     printk(KERN_EMERG "gpio WORLD exit!\n");
    
//     platform_driver_unregister(&gpio_driver);  
// }






// module_init(gpio_init);
// module_exit(gpio_exit);
void timer_function(unsigned long para)
{
    printk("<0>Timer Expired and para is %ld !!\n",para);    
    timer.expires = jiffies + (HZ/10);//5 MS
    add_timer(&timer);
}
int timer_config(void)
{
    init_timer(&timer);
    printk(KERN_EMERG "\nHZ = %d\n",HZ);
    timer.data = 5;
    timer.expires = jiffies + (HZ);//5 MS
    timer.function = timer_function;
    add_timer(&timer);
    
    return 0;
}
void timer_del(void)
{
    del_timer( &timer );
}

#if 1

static int __init dev_init(void)
{
    int ret;

  //  int i;
    /*设置GPIO控制寄存器，GPIO设置为输出模式,默认下灯全灭*/
  
    /*注册混杂型字符设备驱动*/

#if 0    
    printk(KERN_EMERG "\tinitialized\n");  
    ret = gpio_request(led26,"LED59");
    if(ret < 0){
        printk(KERN_EMERG "gpio_request led26 59 failed!\n");
        return ret;
     }
    // ret = gpio_request(led74,"LEDS19");
    // if(ret < 0){
    //     printk(KERN_EMERG "gpio_request led74 19failed!\n");
    //     return ret;
    //  }

    imx6_config_all_iomuxcs();
    imx6_config_all_pads();
    imx6_config_all_gpios();
    misc_register(&gpio_dev);
    printk ("\tinitialized\n");
#endif

    ret = key_config();
    ret = timer_config();
    misc_register(&gpio_dev);
    printk ("\tinitialized\n"); 	
    return ret;
}

static void __exit dev_exit(void)
{
    /*注销混杂型字符设备驱动*/
//    gpio_free(led26);
    free_irq(g_num_irq,(void *)&gpio_dev);
   // gpio_free(led74);
    printk(KERN_EMERG "gpio  dev_exit\n");
    timer_del();
    misc_deregister(&gpio_dev);
}


module_init(dev_init);
module_exit(dev_exit);
#endif

