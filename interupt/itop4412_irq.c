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