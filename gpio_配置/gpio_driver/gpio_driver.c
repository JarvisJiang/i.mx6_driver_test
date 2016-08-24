/*
mx6ul_14x14
*/
/*
�������� ����gpio��ع������ĵ���
*/
#include <asm/imx-common/iomux-v3.h>
#include <asm/arch/mx6-pins.h>
#include <asm/arch/imx-regs.h>
#include <asm/gpio.h>
/*
IMX_GPIO_NR���� �ĵ���
asm/imx-common/gpio.h
Gpio.h (e:\uboot-imx-imx_v2015.04_3.14.52_1.1.0_ga\arch\arm\include\asm\arch-mx6):#include <asm/imx-common/gpio.h>


#define IMX_GPIO_NR(port, index)		((((port)-1)*32)+((index)&31))
*/
#define IOX_SDI IMX_GPIO_NR(5, 10)
#define IOX_STCP IMX_GPIO_NR(5, 7)
#define IOX_SHCP IMX_GPIO_NR(5, 11)
#define IOX_OE IMX_GPIO_NR(5, 18)
//MX6_PAD_BOOT_MODE0__GPIO5_IO10   mx6-pins.h ->  mx6ul_pins.h
//�������� �ṹ��
//iomux_v3_cfg_t    ->iomux-v3.h
static iomux_v3_cfg_t const iox_pads[] = {
	/* IOX_SDI */
	MX6_PAD_BOOT_MODE0__GPIO5_IO10 | MUX_PAD_CTRL(NO_PAD_CTRL),
	/* IOX_SHCP */
	MX6_PAD_BOOT_MODE1__GPIO5_IO11 | MUX_PAD_CTRL(NO_PAD_CTRL),
	/* IOX_STCP */
	MX6_PAD_SNVS_TAMPER7__GPIO5_IO07 | MUX_PAD_CTRL(NO_PAD_CTRL),
	/* IOX_nOE */
	MX6_PAD_SNVS_TAMPER8__GPIO5_IO08 | MUX_PAD_CTRL(NO_PAD_CTRL),
};

/*
Linux ϵͳ������õ�����ʱ������ͷ�ļ���#include <unistd.h>���У��������� sleep��
usleep��
Linux �ں����õ�����ʱ�����ڡ�#include <linux/delay.h>���У��������� ndelay��
udelay��mdelay��
*/

//�������ú���
//imx_iomux_v3_setup_multiple_pads ->iomux-v3.h
imx_iomux_v3_setup_multiple_pads(iox_pads, ARRAY_SIZE(iox_pads));






		udelay(500);



	udelay(500);



	gpio_direction_output(IOX_STCP, 0);

	/*
	  * shift register will be output to pins
	  */
	gpio_direction_output(IOX_STCP, 1);

	gpio_direction_output(IOX_OE, 1);


