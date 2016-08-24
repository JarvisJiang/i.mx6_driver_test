


//for input framework.
#include <linux/input.h>

//for ioremap.
#include <linux/io.h>
//for kzalloc.
#include <linux/slab.h>
//for ulong.
#include <linux/types.h>

/*以后写驱动可以讲头文件一股脑的加载代码前面*/
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <mach/gpio.h>
#include <plat/gpio-cfg.h>
#include <linux/miscdevice.h>
#include <linux/platform_device.h>
#include <mach/regs-gpio.h>
#include <asm/io.h>
#include <linux/regulator/consumer.h>
#include <linux/delay.h>

/*中断函数头文件*/
#include <linux/interrupt.h>
#include <linux/irq.h>

#define IRQ_DEBUG
#ifdef IRQ_DEBUG
#define DPRINTK(x...) printk("IRQ_CTL DEBUG:" x)
#else
#define DPRINTK(x...)
#endif

#define DRIVER_NAME "irq_test"


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



#define IOMUXC_SW_MUX_CTL_PAD_UART2_RX_DATA  0x20E0098
#define IOMUXC_SW_PAD_CTL_PAD_UART2_RX_DATA  0x20E0324



#define IMX_GPIO_NR(port, index)        ((((port)-1)*32)+((index)&31))
#define KEY_INPUT IMX_GPIO_NR(1, 21)  //UART2_RXD  gpio_1_21 key  uart 2 con_ pin8



/*
IOMUXC_SW_PAD_CTL_PAD_DISP0_DATA05  0x1B0B0
IOMUXC_SW_MUX_CTL_PAD_DISP0_DATA05  0x00000005
GPIOx_GDIR             				0x00000000
GPIOx_ICR2              			0x00200000
GPIOx_IMR               			0x04000000
GPIOx_EDGE_SEL    					0x00000000

*/


//GPIO related register define here.
//up/down key,GPIO1[4].
#define GPIO1_DR         0X209C000
#define GPIO1_GDIR       0x209C004
#define GPIO1_PSR        0x209C008
#define GPIO1_IMR        0x209C014
//left/right key,GPIO5[20].

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
int imx6_config_reg(ulong phy_addr, int bits_mask, int bits_value) {
    volatile ulong *pbase = NULL;
    volatile int ret;
    int i;
    int reg_bits_num = sizeof(ret)*8;
    pbase = (ulong*) ioremap(phy_addr, REG_SIZE);
    printk(KERN_EMERG "phy_addr = %lx bits_mask = %x bits_value=%x\n",phy_addr,bits_mask,bits_value);
    if (!pbase) {
        print_error(KERN_EMERG "error accord when ioremap()!\n");
        return -1;
    }
    
    ret = ioread32(pbase);
    printk(KERN_EMERG "ret = %x\n",ret);
    for (i = 0; i <   reg_bits_num; i++) {
        if (bits_mask & (0x1 << i)) {
            ret &= (~(0x1 << i));
        }
    }
    ret |= bits_value;
    printk(KERN_EMERG "value = %x pbase = %p\n",ret,pbase);
    iowrite32(ret, pbase);
    ret = ioread32(pbase);
    printk(KERN_EMERG "read = %x\n",ret);
    iounmap(pbase);
    pbase = NULL;
    return 0;
}

static void config_key_init(void)
{

}

static irqreturn_t eint9_interrupt(int irq, void *dev_id) {

        printk("%s(%d)\n", __FUNCTION__, __LINE__);

        return IRQ_HANDLED;
}

static irqreturn_t eint10_interrupt(int irq, void *dev_id) {

        printk("%s(%d)\n", __FUNCTION__, __LINE__);

        return IRQ_HANDLED;
}

static int irq_probe(struct platform_device *pdev)
{
        int ret;
        char *banner = "irq_test Initialize\n";

        printk(banner);



        ret = gpio_request(EXYNOS4_GPX1(1), "EINT9");
        if (ret) {
                printk("%s: request GPIO %d for EINT9 failed, ret = %d\n", DRIVER_NAME,
                        EXYNOS4_GPX1(1), ret);
                return ret;
        }

        s3c_gpio_cfgpin(EXYNOS4_GPX1(1), S3C_GPIO_SFN(0xF));
        s3c_gpio_setpull(EXYNOS4_GPX1(1), S3C_GPIO_PULL_UP);
        gpio_free(EXYNOS4_GPX1(1));

        ret = gpio_request(EXYNOS4_GPX1(2), "EINT10");
        if (ret) {
                printk("%s: request GPIO %d for EINT10 failed, ret = %d\n", DRIVER_NAME,
                        EXYNOS4_GPX1(2), ret);
                return ret;
        }

        s3c_gpio_cfgpin(EXYNOS4_GPX1(2), S3C_GPIO_SFN(0xF));
        s3c_gpio_setpull(EXYNOS4_GPX1(2), S3C_GPIO_PULL_UP);
        gpio_free(EXYNOS4_GPX1(2));

        ret = request_irq(IRQ_EINT(9), eint9_interrupt,
                        IRQ_TYPE_EDGE_FALLING /*IRQF_TRIGGER_FALLING*/, "eint9", pdev);
        if (ret < 0) {
                printk("Request IRQ %d failed, %d\n", IRQ_EINT(9), ret);
                goto exit;
        }

        ret = request_irq(IRQ_EINT(10), eint10_interrupt,
                        IRQ_TYPE_EDGE_FALLING /*IRQF_TRIGGER_FALLING*/, "eint10", pdev);
        if (ret < 0) {
                printk("Request IRQ %d failed, %d\n", IRQ_EINT(10), ret);
                goto exit;
        }

        return 0;

exit:
        return ret;
}

static int irq_remove (struct platform_device *pdev)
{
        free_irq(IRQ_EINT(9),pdev);
		free_irq(IRQ_EINT(10),pdev);
		
		return 0;
}

static int irq_suspend (struct platform_device *pdev, pm_message_t state)
{
        DPRINTK("irq suspend:power off!\n");
        return 0;
}

static int irq_resume (struct platform_device *pdev)
{
        DPRINTK("irq resume:power on!\n");
        return 0;
}

static struct platform_driver irq_driver = {
        .probe = irq_probe,
        .remove = irq_remove,
        .suspend = irq_suspend,
        .resume = irq_resume,
        .driver = {
                .name = DRIVER_NAME,
                .owner = THIS_MODULE,
        },
};

static void __exit irq_test_exit(void)
{
		platform_driver_unregister(&irq_driver);
}

static int __init irq_test_init(void)
{
        return platform_driver_register(&irq_driver);
}

module_init(irq_test_init);
module_exit(irq_test_exit);

MODULE_LICENSE("Dual BSD/GPL");






