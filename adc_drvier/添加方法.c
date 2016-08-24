reg_sd1_vmmc: regulator@1 {

  compatible = "regulator-fixed";

  regulator-name = "VSD_3V3";

  regulator-min-microvolt = <3300000>;

  regulator-max-microvolt = <3300000>;

//gpio = <&gpio1 9 GPIO_ACTIVE_HIGH>;

  //enable-active-high;

  };

 

 

  reg_vref_3v3: regulator@2 {

  compatible = "regulator-fixed";

  regulator-name = "vref-3v3";

  regulator-min-microvolt = <3300000>;

  regulator-max-microvolt = <3300000>;

  };

 

&adc1 {

  pinctrl-names = "default";

  pinctrl-0 = <&pinctrl_adc1>;

  vref-supply = <&reg_vref_3v3>;

  status = "okay";

};

pinctrl_hog_1: hoggrp-1 {

  fsl,pins = <

  MX6UL_PAD_LCD_RESET__WDOG1_WDOG_ANY    0x30b0

  MX6UL_PAD_UART1_RTS_B__GPIO1_IO19 0x17059 /* SD1 CD */

  MX6UL_PAD_GPIO1_IO05__USDHC1_VSELECT 0x17059 /* SD1 VSELECT */

  //MX6UL_PAD_GPIO1_IO09__GPIO1_IO09        0x17059 /* SD1 RESET */

  >;

  };

 

pinctrl_adc1: adc1grp {

  fsl,pins = <

  MX6UL_PAD_GPIO1_IO09__GPIO1_IO09 0xb0//

  >;

  };

imx6ul.dtsi file i modified :

 

adc1: adc@02198000 {

  compatible = "fsl,imx6ul-adc", "fsl,vf610-adc";

  reg = <0x02198000 0x4000>;

  interrupts = <GIC_SPI 100 IRQ_TYPE_LEVEL_HIGH>;

  clocks = <&clks IMX6UL_CLK_ADC1>;

  num-channels = <10>;

  clock-names = "adc";

  status = "disabled";
}
/*
  chanle 9  adc  is gpio1_9   
  cat in_voltage9_raw 
*/
/*
²Ù×÷·½·¨
Linux IIO provides some system interface to get the raw ADC data from the related
input. Users can also set the sample rate in the available sample rate group. The ADC
controllers system interface is located:
/sys/devices/soc0/soc.1/2200000.aips-bus/2280000.adc/iio:device0:
/sys/devices/soc0/soc.1/2200000.aips-bus/2284000.adc/iio:device1:
*/
/*
  cat in_voltage0_raw               //to get raw ADC data
  cat sampling_frequency_available //to get available sample rate group
  cat in_voltage_sampling_frequency // to show current sample rate
  echo value > in_voltage_sampling_frequency //to set the sample rate

*/

