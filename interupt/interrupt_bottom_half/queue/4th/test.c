#include <linux/module.h>
#include <linux/init.h>

#include <linux/interrupt.h>
#include <linux/workqueue.h>

#define DEBUG_SWITCH    1
#if DEBUG_SWITCH
	#define P_DEBUG(fmt, args...)   printk("<1>" "<kernel>[%s]"fmt, __FUNCTION__, ##args)
#else
	#define P_DEBUG(fmt, args...)   printk("<7>" "<kernel>[%s]"fmt, __FUNCTION__, ##args)
#endif

struct workqueue_struct *xiaobai_wq;	//定义工作队列
struct work_struct xiaobai_work;		//定义work结构体

void xiaobai_func(struct work_struct *work)
{
	printk("hello xiaobai!\n");
}

irqreturn_t irq_handler(int irqno, void *dev_id)	//中断处理函数
{
	printk("key down\n");
	queue_work(xiaobai_wq ,&xiaobai_work);
	return IRQ_HANDLED;
}
static int __init test_init(void)	//模块初始化函数
{
	int ret;

	/*work*/
	xiaobai_wq = create_workqueue("xiaobai");
	INIT_WORK(&xiaobai_work, xiaobai_func);

	ret = request_irq(IRQ_EINT1, irq_handler, 
			IRQF_TRIGGER_FALLING, "key INT_EINT1", NULL);
	if(ret){
		P_DEBUG("request irq failed!\n");
		return ret;
	}

	printk("hello irq\n");
	return 0;
}

static void __exit test_exit(void)		//模块卸载函数
{
	flush_workqueue(xiaobai_wq);
	destroy_workqueue(xiaobai_wq);
	free_irq(IRQ_EINT1, NULL);
	printk("good bye irq\n");
}

module_init(test_init);
module_exit(test_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("xoao bai");
MODULE_VERSION("v0.1");
