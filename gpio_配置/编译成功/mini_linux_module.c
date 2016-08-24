#include <linux/init.h>
#include <linux/module.h>
#include <asm/io.h>
#include <arch-mx6/imx-regs.h>
#include "arch-mx6/gpio.h"
#include <device.h>
//#include <linux/platform_device.h>
#include <linux/delay.h>
#include <linux/sched.h>
#ifndef CONFIG_MX6
#define CONFIG_MX6
#endif
#ifndef CONFIG_MX6UL
#define CONFIG_MX6UL
#endif
MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("TOPEET");

/* State of a GPIO, as reported by get_function() */
enum gpio_func_t {
	GPIOF_INPUT = 0,
	GPIOF_OUTPUT,
	GPIOF_UNUSED,		/* Not claimed */
	GPIOF_UNKNOWN,		/* Not known */
	GPIOF_FUNC,		/* Not used as a GPIO */

	GPIOF_COUNT,
};
enum mxc_gpio_direction {
	MXC_GPIO_DIRECTION_IN,
	MXC_GPIO_DIRECTION_OUT,
};

#define GPIO_PER_BANK			32

struct mxc_gpio_plat {
	int bank_index;
	struct gpio_regs *regs;
};

struct mxc_bank_info {
	struct gpio_regs *regs;
};

#if 1
#define GPIO_TO_PORT(n)		(n / 32)

/* GPIO port description */
static unsigned long gpio_ports[] = {
	[0] = GPIO1_BASE_ADDR,
	[1] = GPIO2_BASE_ADDR,
	[2] = GPIO3_BASE_ADDR,
#if defined(CONFIG_MX25) || defined(CONFIG_MX27) || defined(CONFIG_MX51) || \
		defined(CONFIG_MX53) || defined(CONFIG_MX6) || \
		defined(CONFIG_MX7)
	[3] = GPIO4_BASE_ADDR,
#endif
#if defined(CONFIG_MX27) || defined(CONFIG_MX53) || defined(CONFIG_MX6) || \
		defined(CONFIG_MX7)
	[4] = GPIO5_BASE_ADDR,
#if !defined(CONFIG_MX6UL)
	[5] = GPIO6_BASE_ADDR,
#endif
#endif
#if defined(CONFIG_MX53) || defined(CONFIG_MX6) || defined(CONFIG_MX7)
#if !defined(CONFIG_MX6UL)
	[6] = GPIO7_BASE_ADDR,
#endif
#endif
};

#ifdef CONFIG_MXC_RDC
static unsigned int gpio_rdc[] = {
	RDC_PER_GPIO1,
	RDC_PER_GPIO2,
	RDC_PER_GPIO3,	
	RDC_PER_GPIO4,
	RDC_PER_GPIO5,
	RDC_PER_GPIO6,
	RDC_PER_GPIO7,
};

#define RDC_CHECK(x) imx_rdc_check_permission(gpio_rdc[x])
#define RDC_SPINLOCK_UP(x) imx_rdc_sema_lock(gpio_rdc[x])
#define RDC_SPINLOCK_DOWN(x) imx_rdc_sema_unlock(gpio_rdc[x])
#else
#define RDC_CHECK(x) 0
#define RDC_SPINLOCK_UP(x)
#define RDC_SPINLOCK_DOWN(x)
#endif


static int mxc_gpio_direction(unsigned int gpio,
	enum mxc_gpio_direction direction)
{
	unsigned int port = GPIO_TO_PORT(gpio);
	struct gpio_regs *regs;
	u32 l;

	if (port >= ARRAY_SIZE(gpio_ports))
		return -1;

	if (RDC_CHECK(port))
		return -1;

	RDC_SPINLOCK_UP(port);

	gpio &= 0x1f;

	regs = (struct gpio_regs *)gpio_ports[port];

	l = readl(&regs->gpio_dir);

	switch (direction) {
	case MXC_GPIO_DIRECTION_OUT:
		l |= 1 << gpio;
		break;
	case MXC_GPIO_DIRECTION_IN:
		l &= ~(1 << gpio);
	}
	writel(l, &regs->gpio_dir);

	RDC_SPINLOCK_DOWN(port);

	return 0;
}

int gpio_set_value(unsigned gpio, int value)
{
	unsigned int port = GPIO_TO_PORT(gpio);
	struct gpio_regs *regs;
	u32 l;

	if (port >= ARRAY_SIZE(gpio_ports))
		return -1;

	if (RDC_CHECK(port))
		return -1;

	RDC_SPINLOCK_UP(port);

	gpio &= 0x1f;

	regs = (struct gpio_regs *)gpio_ports[port];

	l = readl(&regs->gpio_dr);
	if (value)
		l |= 1 << gpio;
	else
		l &= ~(1 << gpio);
	writel(l, &regs->gpio_dr);

	RDC_SPINLOCK_DOWN(port);

	return 0;
}

int gpio_get_value(unsigned gpio)
{
	unsigned int port = GPIO_TO_PORT(gpio);
	struct gpio_regs *regs;
	u32 val;

	if (port >= ARRAY_SIZE(gpio_ports))
		return -1;

	if (RDC_CHECK(port))
		return -1;

	RDC_SPINLOCK_UP(port);

	gpio &= 0x1f;

	regs = (struct gpio_regs *)gpio_ports[port];

	val = (readl(&regs->gpio_psr) >> gpio) & 0x01;

	RDC_SPINLOCK_DOWN(port);

	return val;
}

int gpio_request(unsigned gpio, const char *label)
{
	unsigned int port = GPIO_TO_PORT(gpio);
	if (port >= ARRAY_SIZE(gpio_ports))
		return -1;

	if (RDC_CHECK(port))
		return -1;

	return 0;
}

int gpio_free(unsigned gpio)
{
	return 0;
}

int gpio_direction_input(unsigned gpio)
{
	return mxc_gpio_direction(gpio, MXC_GPIO_DIRECTION_IN);
}

int gpio_direction_output(unsigned gpio, int value)
{
	int ret = gpio_set_value(gpio, value);

	if (ret < 0)
		return ret;

	return mxc_gpio_direction(gpio, MXC_GPIO_DIRECTION_OUT);
}
#endif


static int mxc_gpio_is_output(struct gpio_regs *regs, int offset)
{
	u32 val;

	val = readl(&regs->gpio_dir);

	return val & (1 << offset) ? 1 : 0;
}


static void mxc_gpio_bank_direction(struct gpio_regs *regs, int offset,
				    enum mxc_gpio_direction direction)
{
	u32 l;

	l = readl(&regs->gpio_dir);

	switch (direction) {
	case MXC_GPIO_DIRECTION_OUT:
		l |= 1 << offset;
		break;
	case MXC_GPIO_DIRECTION_IN:
		l &= ~(1 << offset);
	}
	writel(l, &regs->gpio_dir);
}

static void mxc_gpio_bank_set_value(struct gpio_regs *regs, int offset,
				    int value)
{
	u32 l;

	l = readl(&regs->gpio_dr);
	if (value)
		l |= 1 << offset;
	else
		l &= ~(1 << offset);
	writel(l, &regs->gpio_dr);
}

static int mxc_gpio_bank_get_value(struct gpio_regs *regs, int offset)
{
	return (readl(&regs->gpio_psr) >> offset) & 0x01;
}
#if 0

/* set GPIO pin 'gpio' as an input */
static int mxc_gpio_direction_input(struct udevice *dev, unsigned offset)
{
	struct mxc_bank_info *bank = (dev);

	/* Configure GPIO direction as input. */
	mxc_gpio_bank_direction(bank->regs, offset, MXC_GPIO_DIRECTION_IN);

	return 0;
}
#endif
void *dev_get_mypriv(struct udevice *dev)
{
	if (!dev) {

		return NULL;
	}

	return dev->priv;
}

/* set GPIO pin 'gpio' as an output, with polarity 'value' */
static int mxc_gpio_direction_output(struct udevice *dev, unsigned offset,
				       int value)
{
	struct mxc_bank_info *bank = dev_get_mypriv(dev);

	/* Configure GPIO output value. */
	mxc_gpio_bank_set_value(bank->regs, offset, value);

	/* Configure GPIO direction as output. */
	mxc_gpio_bank_direction(bank->regs, offset, MXC_GPIO_DIRECTION_OUT);

	return 0;
}

/* read GPIO IN value of pin 'gpio' */
static int mxc_gpio_get_value(struct udevice *dev, unsigned offset)
{
	struct mxc_bank_info *bank = dev->priv;

	return mxc_gpio_bank_get_value(bank->regs, offset);
}

/* write GPIO OUT value to pin 'gpio' */
static int mxc_gpio_set_value(struct udevice *dev, unsigned offset,
				 int value)
{
	struct mxc_bank_info *bank = dev->priv;

	mxc_gpio_bank_set_value(bank->regs, offset, value);

	return 0;
}

static int mxc_gpio_get_function(struct udevice *dev, unsigned offset)
{
	struct mxc_bank_info *bank = dev->priv;

	/* GPIOF_FUNC is not implemented yet */
	if (mxc_gpio_is_output(bank->regs, offset))
		return 1;//GPIOF_OUTPUT
	else
		return 0;//GPIOF_INPUT
}

/*
left 40 --- goio_01_08 --- lcd_blt
#define	MX6UL_PAD_GPIO1_IO08__GPIO1_IO08                         	0x007C 0x0308 0x0000 5 0

right 5 --- gpio3.IO[1] --- lcd_de
#define	MX6UL_PAD_LCD_ENABLE__LCDIF_ENABLE                       	0x0108 0x0394 0x0000 0 0


right 56 -- SNVS_TAMPER3||pio5.IO[3]-- led1

#define	MX6UL_PAD_SNVS_TAMPER3__GPIO5_IO03				0x0028 0x02B4 0x0000 5 0



right 44 -- SNVS_TAMPER9||pio5.IO[9]---led2

#define	MX6UL_PAD_SNVS_TAMPER9__GPIO5_IO09				0x0040 0x02CC 0x0000 5 0



right 38 -- goio_01_09	-- led3
#define	MX6UL_PAD_GPIO1_IO09__GPIO1_IO09                         	0x0080 0x030C 0x0000 5 0

right 26 -- gpio_01_05  -- led4

#define	MX6UL_PAD_GPIO1_IO05__GPIO1_IO05                         	0x0070 0x02FC 0x0000 5 0

*/

/*
IMX_GPIO_NR(5, 9)
*/

void lcd_ctl_init(void)
{
	int i = 0;

	gpio_direction_output(IMX_GPIO_NR(1,8),0);
	//led 2
	gpio_direction_output(IMX_GPIO_NR(5,9),0);
	//led 3
	gpio_direction_output(IMX_GPIO_NR(1,9),0);
	while(1)
	{
	    i++;
	    if(i>5)
	    {
		printk("lcd_ctl__high\r\n");		
		gpio_direction_output(IMX_GPIO_NR(5,9),1);
		//led 3
		gpio_direction_output(IMX_GPIO_NR(1,9),1);	
		mdelay(500);
		gpio_direction_output(IMX_GPIO_NR(5,9),0);
		//led 3
		gpio_direction_output(IMX_GPIO_NR(1,9),0);
		printk("lcd_ctl__low\r\n");
		mdelay(500);
		break;
	    }
	}

}
void lcd_ctl_exit(void)
{

}
module_init(lcd_ctl_init);
module_exit(lcd_ctl_exit);
#if 0
static const struct dm_gpio_ops gpio_mxc_ops = {
	.direction_input	= mxc_gpio_direction_input,
	.direction_output	= mxc_gpio_direction_output,
	.get_value		= mxc_gpio_get_value,
	.set_value		= mxc_gpio_set_value,
	.get_function		= mxc_gpio_get_function
};


static int mxc_gpio_probe(struct udevice *dev)
{
	struct mxc_bank_info *bank = dev_get_priv(dev);
	struct mxc_gpio_plat *plat = dev_get_platdata(dev);
	struct gpio_dev_priv *uc_priv = dev->uclass_priv;
	int banknum;
	char name[18], *str;

	banknum = plat->bank_index;
	sprintf(name, "GPIO%d_", banknum + 1);
	str = strdup(name);
	if (!str)
		return -ENOMEM;
	uc_priv->bank_name = str;
	uc_priv->gpio_count = GPIO_PER_BANK;
	bank->regs = plat->regs;

	return 0;
}

static int mxc_gpio_bind(struct udevice *dev)
{
	struct mxc_gpio_plat *plat = dev->platdata;
	fdt_addr_t addr;

	/*
	 * If platdata already exsits, directly return.
	 * Actually only when DT is not supported, platdata
	 * is statically initialized in U_BOOT_DEVICES.Here
	 * will return.
	 */
	if (plat)
		return 0;

	addr = dev_get_addr(dev);
	if (addr == FDT_ADDR_T_NONE)
		return -ENODEV;

	/*
	 * TODO:
	 * When every board is converted to driver model and DT is supported,
	 * this can be done by auto-alloc feature, but not using calloc
	 * to alloc memory for platdata.
	 */
	plat = calloc(1, sizeof(*plat));
	if (!plat)
		return -ENOMEM;

	plat->regs = (struct gpio_regs *)addr;
	plat->bank_index = dev->req_seq;
	dev->platdata = plat;

	return 0;
}

static const struct udevice_id mxc_gpio_ids[] = {
	{ .compatible = "fsl,imx35-gpio" },
	{ }
};

U_BOOT_DRIVER(gpio_mxc) = {
	.name	= "gpio_mxc",
	.id	= UCLASS_GPIO,
	.ops	= &gpio_mxc_ops,
	.probe	= mxc_gpio_probe,
	.priv_auto_alloc_size = sizeof(struct mxc_bank_info),
	.of_match = mxc_gpio_ids,
	.bind	= mxc_gpio_bind,
};
#endif
