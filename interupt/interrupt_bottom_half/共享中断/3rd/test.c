#include <linux/module.h>
#include <linux/init.h>

#include <linux/interrupt.h>

#define DEBUG_SWITCH    1
#if DEBUG_SWITCH
	#define P_DEBUG(fmt, args...)   printk("<1>" "<kernel>[%s]"fmt, __FUNCTION__, ##args)
#else
	#define P_DEBUG(fmt, args...)   printk("<7>" "<kernel>[%s]"fmt, __FUNCTION__, ##args)
#endif

irqreturn_t irq_handler(int irqno, void *dev_id)	//中断处理函数
{
	printk("key down, dev_id[%d]\n", *(int *)dev_id);
	return IRQ_HANDLED;
}

int id = 321;

static int __init test_init(void)	//模块初始化函数
{
	int ret;

	/*注册中断处理函数：
	 * IRQ_EINT1:中断号，定义在"include/mach/irqs.h"中
	 * irq_handler：中断处理函数
	 * IRQ_TIRGGER_FALLING：中断类型标记，下降沿触发中断
	 * ker_INT_EINT1：中断的名字，显示在/proc/interrupts等文件中
	 * NULL；现在我不使用dev_id，所以这里不传参数
	 */
	ret = request_irq(IRQ_EINT1, irq_handler, 
			IRQF_TRIGGER_FALLING | IRQF_SHARED, "key INT_EINT1", &id);
	if(ret){
		P_DEBUG("request irq failed!\n");
		return ret;
	}
	printk("hello irq\n");
	return 0;
}

static void __exit test_exit(void)		//模块卸载函数
{
	free_irq(IRQ_EINT1, &id);
	printk("good bye irq\n");
}

module_init(test_init);
module_exit(test_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("xoao bai");
MODULE_VERSION("v0.1");
