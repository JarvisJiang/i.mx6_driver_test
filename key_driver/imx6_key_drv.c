/**
 * filename:imx6_key_drv.c
 * description: matrix key driver for imx6.
 * author:shell.albert@gmail.com
 * date: September 1,2015.
 *
 * three gpio keys are connected to imx6 through different GPIO.
 * up/down key: GPIO04/SLEEP_WAKE
 * led key:EIM_BCLK/VOL+
 * left/right key:CSI0_DATA_EN/VOL-
 * ok key:NANDF_CS0
 * return key:NANDF_CS1
 *
 * attention here.
 * the mdev rules must be enabled before use this driver.
 * do the following work to make it works.
 *     /etc/init.d/rcS
 *     #mount partitions
 *     mkdir /dev/pts -p
 *     mount -t devpts devpts /dev/pts
 *     mount -a
 *     echo /sbin/mdev > /proc/sys/kernel/hotplug
 *     mdev -s
 *
 *     /etc/mdev.conf
 *     # system all-writable devices
 *      full            0:0     0666
 *      null            0:0     0666
 *          ptmx            0:0     0666
 *      random          0:0     0666
 *        tty             0:0     0666
 *        zero            0:0     0666


 *        # i2c devices
 *        i2c-0           0:0     0666    =i2c/0
 *        i2c-1           0:0     0666    =i2c/1

 *        # frame buffer devices
 *        fb[0-9]         0:0     0666

 *        # input devices
 *        mice            0:0     0660    =input/
 *        mouse.*         0:0     0660    =input/
 *        event.*         0:0     0660    =input/
 *        ts.*            0:0     0660    =input/
 */

/**
Event: time 1441159728.919925, -------------- Report Sync ------------
Event: time 1441159733.423896, type 4 (Misc), code 4 (ScanCode), value 70052
Event: time 1441159733.423896, type 1 (Key), code 103 (Up), value 1
Event: time 1441159733.423896, -------------- Report Sync ------------
Event: time 1441159733.511935, type 4 (Misc), code 4 (ScanCode), value 70052
Event: time 1441159733.511935, type 1 (Key), code 103 (Up), value 0
Event: time 1441159733.511935, -------------- Report Sync ------------
Event: time 1441159735.431922, type 4 (Misc), code 4 (ScanCode), value 70050
Event: time 1441159735.431922, type 1 (Key), code 105 (Left), value 1
Event: time 1441159735.431922, -------------- Report Sync ------------
Event: time 1441159735.503919, type 4 (Misc), code 4 (ScanCode), value 70050
Event: time 1441159735.503919, type 1 (Key), code 105 (Left), value 0
Event: time 1441159735.503919, -------------- Report Sync ------------
Event: time 1441159739.127894, type 4 (Misc), code 4 (ScanCode), value 700e4
Event: time 1441159739.127894, type 1 (Key), code 97 (RightCtrl), value 1
Event: time 1441159739.127894, -------------- Report Sync ------------
Event: time 1441159739.263894, type 4 (Misc), code 4 (ScanCode), value 700e4
Event: time 1441159739.263894, type 1 (Key), code 97 (RightCtrl), value 0
Event: time 1441159739.263894, -------------- Report Sync ------------
Event: time 1441159742.103834, type 4 (Misc), code 4 (ScanCode), value 700e0
Event: time 1441159742.103834, type 1 (Key), code 29 (LeftControl), value 1
Event: time 1441159742.103834, -------------- Report Sync ------------
Event: time 1441159742.207873, type 4 (Misc), code 4 (ScanCode), value 700e0
Event: time 1441159742.207873, type 1 (Key), code 29 (LeftControl), value 0
Event: time 1441159742.207873, -------------- Report Sync ------------
 */

/**
 * PAD Mux registers.
 * related register define here.
 * I/O are multiplex,so we choose the right mode.
 */
 
#define IOMUXC_SW_MUX_CTL_PAD_GPIO1_IO00           0x20E005C  //gpio_1_0
#define IOMUXC_SW_MUX_CTL_PAD_GPIO1_IO01           0x20E0060  //gpio_1_1	
#define IOMUXC_SW_MUX_CTL_PAD_GPIO1_IO02           0x20E0064  //gpio_1_2
#define IOMUXC_SW_MUX_CTL_PAD_GPIO1_IO03           0x20E0068  //gpio_1_3
#define IOMUXC_SW_MUX_CTL_PAD_GPIO1_IO04           0x20E006C //gpio_1_4


#define IOMUXC_SW_MUX_CTL_PAD_GPIO04                0x20E0238 //GPIO1_IO04
#define IOMUXC_SW_MUX_CTL_PAD_EIM_BCLK                0x20E0158 //GPIO6_IO31
#define IOMUXC_SW_MUX_CTL_PAD_CSI0_DATA_EN            0x20E0260 //GPIO5_IO20
#define IOMUXC_SW_MUX_CTL_PAD_NAND_CS0_B            0x20E02E4 //GPIO6_IO11
#define IOMUXC_SW_MUX_CTL_PAD_NAND_CS1_B            0x20E02E8 //GPIO6_IO14
/**
 * PAD Control registers.
 * used to control the pad physical features.
 */
#define IOMUXC_SW_PAD_CTL_PAD_GPIO1_IO00                   0X20E02E8  
#define IOMUXC_SW_PAD_CTL_PAD_GPIO1_IO01                   0X20E02EC
#define IOMUXC_SW_PAD_CTL_PAD_GPIO1_IO02                   0X20E02F0
#define IOMUXC_SW_PAD_CTL_PAD_GPIO1_IO03                   0X20E02F4
#define IOMUXC_SW_PAD_CTL_PAD_GPIO1_IO04                   0X20E02F8


#define IOMUXC_SW_PAD_CTL_PAD_GPIO04                    0x20E0608
#define IOMUXC_SW_PAD_CTL_PAD_EIM_BCLK                    0x20E046C
#define IOMUXC_SW_PAD_CTL_PAD_CSI0_DATA_EN                0x20E0630
#define IOMUXC_SW_PAD_CTL_PAD_NAND_CS0_B                0x20E06CC
#define IOMUXC_SW_PAD_CTL_PAD_NAND_CS1_B                0x20E06D0

//register size,it's always 32-bit,4 bytes in 32-bit CPU.
#define REG_SIZE        0x4

#define MODULE_NAME        "imx6_key_drv"

#define print_debug(fmt,arg...)     printk(KERN_DEBUG fmt,##arg)
#define print_error(fmt,arg...)        printk(KERN_ERR fmt,##arg)

//GPIO related register define here.
//up/down key,GPIO1[4].
#define GPIO1_GDIR        0x209C004
#define GPIO1_PSR        0x209C008
#define GPIO1_IMR        0x209C014
//left/right key,GPIO5[20].
#define GPIO5_GDIR        0x20AC004
#define GPIO5_PSR        0x20AC008
#define GPIO5_IMR        0x20AC014
//led key:GPIO6[31].
//ok key: GPIO6[11].
//return key: GPIO6[14].
#define GPIO6_GDIR        0x20B0004
#define GPIO6_PSR        0x20B0008
#define GPIO6_IMR        0x20B0014

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
struct imx6_keys_dev *key_dev;
/**
 * configure the register at initial stage.
 * these registers should only be configured once.
 */
int imx6_config_reg(ulong phy_addr, int bits_mask, int bits_value) {
    volatile ulong *pbase = NULL;
    volatile int ret;
    int i;
    int reg_bits_num = sizeof(ret);
    pbase = (ulong*) ioremap(phy_addr, REG_SIZE);
    if (!pbase) {
        print_error("error accord when ioremap()!\n");
        return -1;
    }
    ret = ioread32(pbase);
    for (i = 0; i < reg_bits_num; i++) {
        if (bits_mask & (0x1 << i)) {
            ret &= (~(0x1 << i));
        }
    }
    ret |= bits_value;
    iowrite32(ret, pbase);
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
 */
 
int imx6_config_all_iomuxcs(void) {
    //IOMUXC_SW_MUX_CTL_PAD_GPIO04.[2:0]=101=0x5,GPIO1[4].
    //bits_mask=111=0x7,
    if (imx6_config_reg(IOMUXC_SW_MUX_CTL_PAD_GPIO1_IO00, 0x7, 0x5) < 0) {
        return -1;
    }
    
    //IOMUXC_SW_MUX_CTL_PAD_EIM_BCLK,[2:0]=101=0x5,GPIO6[31].
    //bits_mask=111=0x7.
    if (imx6_config_reg(IOMUXC_SW_MUX_CTL_PAD_GPIO1_IO01, 0x7, 0x5) < 0) {
        return -1;
    }
    
    //IOMUXC_SW_MUX_CTL_PAD_CSI0_DATA_EN,[2:0]=101=0x5,ALT5,GPIO5[20].
    //bits_mask=111=0x7.
    if (imx6_config_reg(IOMUXC_SW_MUX_CTL_PAD_GPIO1_IO02, 0x7, 0x5) < 0) {
        return -1;
    }

    //IOMUXC_SW_MUX_CTL_PAD_NAND_CS0_B,[2:0]=101=0x5,ALT5,GPIO6[11].
    //bits_mask=111=0x7.
    if (imx6_config_reg(IOMUXC_SW_MUX_CTL_PAD_GPIO1_IO03, 0x7, 0x5) < 0) {
        return -1;
    }
    
    //IOMUXC_SW_MUX_CTL_PAD_NAND_CS1_B,[2:0]=101=0x5,ALT5,GPIO6[14].
    //bits_mask=111=0x7.
    if (imx6_config_reg(IOMUXC_SW_MUX_CTL_PAD_GPIO1_IO04, 0x7, 0x5) < 0) {
        return -1;
    }
    return 0;
}

//configure pad's physical feature.
int imx6_config_all_pads(void) {
    //[16]=1,Schmitt trigger input.
    //[15:14]=10,100Kohm pull up.
    //[13]=1,pull enabled.
    //[12]=1,pull/keeper enabled.
    //bits_mask= 1,1111,0000,0000,0000=0x1F000.
    //bits_value=1,1011,0000,0000,0000=0x1B000.

    //IOMUXC_SW_PAD_CTL_PAD_GPIO04.
    if (imx6_config_reg(IOMUXC_SW_PAD_CTL_PAD_GPIO1_IO00, 0x1F000, 0x1B000) < 0) {
        return -1;
    }
    //IOMUXC_SW_PAD_CTL_PAD_EIM_BCLK.
    if (imx6_config_reg(IOMUXC_SW_PAD_CTL_PAD_GPIO1_IO01, 0x1F000, 0x1B000) < 0) {
        return -1;
    }
    //IOMUXC_SW_PAD_CTL_PAD_CSI0_DATA_EN.
    if (imx6_config_reg(IOMUXC_SW_PAD_CTL_PAD_GPIO1_IO02, 0x1F000, 0x1B000) < 0) {
        return -1;
    }
    
    //IOMUXC_SW_PAD_CTL_PAD_NAND_CS0_B.
    if (imx6_config_reg(IOMUXC_SW_PAD_CTL_PAD_GPIO1_IO03, 0x1F000, 0x1B000) < 0) {
        return -1;
    }
    
    //IOMUXC_SW_PAD_CTL_PAD_NAND_CS1_B.
    if (imx6_config_reg(IOMUXC_SW_PAD_CTL_PAD_GPIO1_IO04, 0x1F000, 0x1B000) < 0) {
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
    if (imx6_config_reg(GPIO1_GDIR, 0x10, 0x0) < 0) {
        return -1;
    }
    //[4]=0,disable interrupt.
    if (imx6_config_reg(GPIO1_IMR, 0x10, 0x0) < 0) {
        return -1;
    }

    //left/right key,GPIO5[20].
    //[20]=0,direction,input.
    //bits_mask=1,0000,0000,0000,0000,0000=0x100000.
    //bits_value=0,0000,0000,0000,0000,0000=0x0.
    if (imx6_config_reg(GPIO5_GDIR, 0x100000, 0x0) < 0) {
        return -1;
    }
    //[4]=0,disable interrupt.
    if (imx6_config_reg(GPIO5_IMR, 0x100000, 0x0) < 0) {
        return -1;
    }

    //led key:GPIO6[31].
    //ok key: GPIO6[11].
    //return key: GPIO6[14].
    //[31]=0,direction input.
    //[11]=0,direction input.
    //[14]=0,direction input.
    //bits_mask=1000,0000,0000,0000,0100,1000,0000,0000=0x80004800.
    //bits_value=0000,0000,0000,0000,0000,0000,0000,0000=0x0.
    if (imx6_config_reg(GPIO6_GDIR, 0x80004800, 0x0) < 0) {
        return -1;
    }
    //[31]=0,disable interrupt.
    //[14]=0,disable interrupt.
    //[11]=0,disable interrupt.
    if (imx6_config_reg(GPIO6_IMR, 0x80004800, 0x0) < 0) {
        return -1;
    }

    //do ioremap for virtual address.
    if (!(key_dev->up_down_vir_addr = (ulong*) ioremap(GPIO1_PSR, REG_SIZE))) {
        return -1;
    }
    if (!(key_dev->left_right_vir_addr = (ulong*) ioremap(GPIO5_PSR, REG_SIZE))) {
        return -1;
    }
    if (!(key_dev->led_ok_return_vir_addr = (ulong*) ioremap(GPIO6_PSR, REG_SIZE))) {
        return -1;
    }
    key_dev->up_down_status=1;
    key_dev->left_right_status=1;
    key_dev->led_ok_return_status=1;
    return 0;
}

//timer function.
//used to poll key status.
void key_status_scan_function(unsigned long arg) {

    struct imx6_keys_dev *pkey_dev = (struct imx6_keys_dev*) (arg);
    volatile int ret;

    //the up/down key,GPIO1[4].
    ret = ioread32(pkey_dev->up_down_vir_addr);
    if(!(ret & (0x1 << 4)) && pkey_dev->up_down_status)
    {
        //key was pressed.
        pkey_dev->up_down_status=0;
        input_report_key(pkey_dev->input_device, KEY_UP, 1);
        input_sync(pkey_dev->input_device);
    }else if((ret&(0x1 << 4)) && !pkey_dev->up_down_status)
    {
        //key was released.
        pkey_dev->up_down_status=1;
        input_report_key(pkey_dev->input_device, KEY_UP, 0);
        input_sync(pkey_dev->input_device);
    }

    //the left/right key,GPIO5[20].
    ret = ioread32(pkey_dev->left_right_vir_addr);
    if(!(ret & (0x1 << 20)) && pkey_dev->left_right_status)
    {
        //key was pressed.
        pkey_dev->left_right_status=0;
        input_report_key(pkey_dev->input_device, KEY_LEFT, 1);
        input_sync(pkey_dev->input_device);
    }else if((ret&(0x1<<20)) && !pkey_dev->left_right_status)
    {
        //key was released.
        pkey_dev->left_right_status=1;
        input_report_key(pkey_dev->input_device, KEY_LEFT, 0);
        input_sync(pkey_dev->input_device);
    }

    //led key:GPIO6[31],open or close led.
    //ok key: GPIO6[11],left ctrl.
    //return key: GPIO6[14],left shift.
    ret = ioread32(pkey_dev->led_ok_return_vir_addr);
    if(!(ret & (0x1 << 11)) && (pkey_dev->led_ok_return_status&(0x1<<11)))
    {
        //key was pressed.
        pkey_dev->led_ok_return_status&=~(0x1<<11);
        input_report_key(pkey_dev->input_device, KEY_LEFTCTRL, 1);
        input_sync(pkey_dev->input_device);
    }else if( (ret&(0x1<<11)) && !(pkey_dev->led_ok_return_status&(0x1<<11)))
    {
        //key was released.
        pkey_dev->led_ok_return_status|=(0x1<<11);
        input_report_key(pkey_dev->input_device, KEY_LEFTCTRL, 0);
        input_sync(pkey_dev->input_device);
    }
    /////////////////////////////////////////////////////////////
    ret = ioread32(pkey_dev->led_ok_return_vir_addr);
    if(!(ret & (0x1 << 14)) && (pkey_dev->led_ok_return_status&(0x1<<14)))
    {
        //key was pressed.
        pkey_dev->led_ok_return_status&=~(0x1<<14);
        input_report_key(pkey_dev->input_device, KEY_LEFTSHIFT, 1);
        input_sync(pkey_dev->input_device);
    }else if( (ret&(0x1<<14)) && !(pkey_dev->led_ok_return_status&(0x1<<14)))
    {
        //key was released.
        pkey_dev->led_ok_return_status|=(0x1<<14);
        input_report_key(pkey_dev->input_device, KEY_LEFTSHIFT, 0);
        input_sync(pkey_dev->input_device);
    }
    //////////////////////////////////////////////////////////////
    ret = ioread32(pkey_dev->led_ok_return_vir_addr);
    if(!(ret & (0x1 << 31)) && (pkey_dev->led_ok_return_status&(0x1<<31)))
    {
        //key was pressed to open led.
        pkey_dev->led_ok_return_status&=~(0x1<<31);
    }else if( (ret&(0x1<<31)) && !(pkey_dev->led_ok_return_status&(0x1<<31)))
    {
        //key was released to close led.
        pkey_dev->led_ok_return_status|=(0x1<<31);
    }

#if 1
    //restart the scan routine.
    pkey_dev->scan_timer.expires = jiffies + HZ / 4;    //500ms.
    add_timer(&pkey_dev->scan_timer);
#endif

}

static int __init imx6_key_drv_init(void)
{
    key_dev=(struct imx6_keys_dev *)kzalloc(sizeof(struct imx6_keys_dev),GFP_KERNEL);
    if(!key_dev)
    {
        print_error("kzalloc() failed!\n");goto error_exit_1;
    }
    key_dev->input_device=input_allocate_device();
    if(!key_dev->input_device)
    {
        print_error(
        "allocate input device failed!\n");goto error_exit_2;
    }

#if 1
    if(imx6_config_all_iomuxcs()<0)
    {
        goto  error_exit_3;
        return -1;
    }
    if(imx6_config_all_pads()<0)
    {
        goto  error_exit_3;
        return -1;
    }
    if(imx6_config_all_gpios()<0)
    {
        goto  error_exit_3;
        return -1;
    }
#endif

    //input device name.
    key_dev->input_device->name=MODULE_NAME;

    //key event.
    set_bit(EV_KEY,key_dev->input_device->evbit);
    //key value.
    set_bit(KEY_UP,key_dev->input_device->keybit);
    set_bit(KEY_LEFT,key_dev->input_device->keybit);
    set_bit(KEY_LEFTCTRL,key_dev->input_device->keybit);
    set_bit(KEY_LEFTSHIFT,key_dev->input_device->keybit);
    if(input_register_device(key_dev->input_device))
    {
        print_error("input register device failed!\n");goto error_exit_3;
    }

    //start timer.
    init_timer(&key_dev->scan_timer);key_dev->scan_timer.data=(ulong)key_dev;
    key_dev->scan_timer.function=key_status_scan_function;
    key_dev->scan_timer.expires=jiffies+HZ/2;//500ms.
    add_timer(&key_dev->scan_timer);

    print_debug("imx6_key_drv load successfully!\n");
return 0;
/**
 * exit flow when errors occured.
 */
error_exit_3:
input_free_device(key_dev->input_device);
error_exit_2:
kfree(key_dev);
error_exit_1:
return -1;
}

static void __exit imx6_key_drv_exit(void)
{
    del_timer_sync(&key_dev->scan_timer);input_unregister_device(key_dev->input_device);
    input_free_device(key_dev->input_device);
    kfree(key_dev);
}

module_init(imx6_key_drv_init);
module_exit(imx6_key_drv_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("zhangshaoyan <shell.albert@gmail.com>");
MODULE_DESCRIPTION("Keyboard driver for Freescale iMX6 GPIOs");
MODULE_ALIAS("platform:imx6 key driver");
