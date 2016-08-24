#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <ads1220.h>
#include <math.h>


//-----------------------------------------------------------------//
//	��    �ܣ�ADS1220 д����
//	��ڲ���: /
//	���ڲ���: /
//	ȫ�ֱ���: /
//	��    ע: ��ADS1220�е�ַΪregaddr�ļĴ���д��һ���ֽ�databyte
//-----------------------------------------------------------------//

void ADS1220WREG(unsigned char regaddr,unsigned char databyte)
{
    ADS_CS_LOW();
//	while(ADS_DRDY);//��ADS1220_DRDYΪ��ʱ����д�Ĵ���
	//��Ĵ���д�����ݵ�ַ
    spi_send_byte(ADS1220_WREG_CMD | (regaddr & 0xF));
    //д�����ݵĸ���n-1
    spi_send_byte(0);
    //��regaddr��ַָ��ļĴ���д������databyte
  	usleep(5);
    spi_send_byte(databyte);
	ADS_CS_HIGH();
}

//-----------------------------------------------------------------//
//	��    �ܣ�ADS1220 ���Ĵ�������
//	��ڲ���: /
//	���ڲ���: /
//	ȫ�ֱ���: /
//	��    ע: ��ADS1220�е�ַΪregaddr�ļĴ�������һ���ֽ�databyte
//-----------------------------------------------------------------//

unsigned char ADS1220RREG(unsigned char regaddr)
{
    //��ADS1220�е�ַΪregaddr�ļĴ�������һ���ֽ�
    unsigned char r=0, len =0;
    ADS_CS_LOW();
//	usleep(10);
//	while(ADS_DRDY);//��ADS1220_DRDYΪ��ʱ����д�Ĵ���
	//д���ַ
	
    spi_send_byte(ADS1220_RREG_CMD | ((regaddr<<2|(len&0x0)) & 0xF));
    //д���ȡ���ݵĸ���n-1
   // spi_send_byte(0);
	  usleep(10);  //min=50*(1/fCLKIN)=50*(1/7.68MHZ)=6500ns;max=whatever
    //����regaddr��ַָ��ļĴ���������
    r=spi_send_byte(0);
	  ADS_CS_HIGH();

    return r;//��������
}

//-----------------------------------------------------------------//
//	��    �ܣ�ADS1220��ʼ���ӳ���
//	��ڲ���: /
//	���ڲ���: /
//	ȫ�ֱ���: /
//	��    ע: /
//-----------------------------------------------------------------//
void ADS1220_Init(void)
{ 

	unsigned char config[4];
	//config[0] = ADS1220_MUX_AIN0_AIN1 + ADS1220_GAIN_16 + ADS1220_USE_PGA;
	config[0] = ADS1220_MUX_AIN1_AIN0 + ADS1220_GAIN_8 + ADS1220_USE_PGA;
	config[1] = ADS1220_DATA_RATE_20SPS + ADS1220_OP_MODE_NORMAL + ADS1220_CONVERSION_CONTINUOUS + ADS1220_TEMP_SENSOR_OFF + ADS1220_BURN_OUT_CURRENT_OFF;

	config[2] = ADS1220_FIR_50_60 + ADS1220_VREF_EXT_REF0_PINS + ADS1220_LOW_SIDE_POWER_OPEN + ADS1220_IDAC_CURRENT_500_UA;
	config[3] = ADS1220_IDAC1_AIN2 + ADS1220_IDAC2_AIN3 + ADS1220_DRDY_ON_DRDY_ONLY;
	printf("*****************write******************\n");
	printf("config[0] = %x\r\n",config[0]);
	printf("config[1] = %x\r\n",config[1]);
	printf("config[2] = %x\r\n",config[2]);
	printf("config[3] = %x\r\n",config[3]);
	ADS_CS_LOW();
	RESET_HIGH();
	usleep(1000);//

	spi_send_byte(ADS1220_RESET_CMD);
	usleep(10);
	//while(ADS_DRDY);                                //��ADS1220_DRDYΪ��ʱ����д�Ĵ���
	spi_send_byte(ADS1220_START_CMD);                 //ͬ������
	spi_send_byte(ADS1220_START_CMD);               //ͬ������
	
//	while(ADS_DRDY);
	spi_send_byte(ADS1220_WREG_CMD | 3);//����д��4���Ĵ���
	spi_send_byte(config[0]);
	spi_send_byte(config[1]);                             //��λ��ǰ��ʹ���ڲ�У׼����ʹ�û���
	spi_send_byte(config[2]);//�˿�����A2Ϊ����A3λ��
	spi_send_byte(config[3]);                   //�Ŵ�������
	//spi_send_byte(ADS1220_DRATE_2_5SPS);             //�ɼ��ٶ�����
	usleep(100);
	
//	while(ADS_DRDY);                                //��ADS1220_DRDYΪ��ʱ����д�Ĵ���
//	spi_send_byte(ADS1220_CMD_SELFCAL);              //ƫ�ƺ������Զ�У׼
	
	
    ADS_CS_HIGH();
	usleep(100);
	
	ADS1220RREG(0);
	printf("*****************read******************\n");
	printf("read config[0] = %x\r\n",ADS1220RREG(0));
	printf("read config[0] = %x\r\n",ADS1220RREG(1));
	printf("read config[0] = %x\r\n",ADS1220RREG(2));
	printf("read config[0] = %x\r\n",ADS1220RREG(3));
 
}


//-----------------------------------------------------------------//
//	��    �ܣ�
//	��ڲ���: /
//	���ڲ���: /
//	ȫ�ֱ���: /
//	��    ע:
//-----------------------------------------------------------------//

unsigned long ADS1220ReadData(void)  
{
    unsigned char i=0;
    unsigned long sum=0;
	unsigned long r=0;
	ADS_CS_LOW();

	spi_send_byte(ADS1220_START_CMD);
	spi_send_byte(ADS1220_START_CMD);
	spi_send_byte(ADS1220_RDATA_CMD);

	//	while(ADS_DRDY);               //��ADS1220_DRDYΪ��ʱ����д�Ĵ��� 	
		usleep(1000);                  //min=50*(1/fCLKIN)=50*(1/7.68MHZ)=6500ns;max=whatever
			for(i=0;i<3;i++)
			{
				sum = sum << 8;
			r = spi_send_byte(0);  
				sum |= r;
			}
		//	spi_send_byte(ADS1220_POWERDOWN_CMD);
		ADS_CS_HIGH();		 
    return sum;
}


#define DS1220_REFER_RESISTOR   1651
#define DS1220_ADC1_FCURRENT				(0.0005) 
#define DS1220_USER_PGA				(8) 
#define DS1220_REFER_VOLTAGE    (DS1220_REFER_RESISTOR*((float)(DS1220_ADC1_FCURRENT*2)))   //  500UA *2 

#define DS1220_FSR              ((float)(DS1220_REFER_VOLTAGE/DS1220_USER_PGA))
	
#define DS1220_MAX_RESISTOR    	((float)(DS1220_FSR/DS1220_ADC1_FCURRENT))
float DS1220ResConvertToTem(short code)
{
	float rtnTemp;
	float rate;
	float resi;
	float voltage;
	if(code<0)
	{
		printf("code < 0 \r\n");
		return 0;
	}
	rate 		= (float)code/32768;	
	voltage = rate*DS1220_FSR;
	resi    = voltage/DS1220_ADC1_FCURRENT;
	#if 0
	printf("MAX resistor: %f\r\n", DS1220_MAX_RESISTOR);
	printf("ds1220 rate: = %f\r\n",rate);
	printf("ds1220 voltage: = %f\r\n",voltage);
	printf("ds1220 resi: = %f\r\n",resi);
	#endif
	
//	rtnTemp = CalculateTemperatureAlg(resi);
	printf("ds1220 rtnTemp: = %f\r\n",rtnTemp);
	return rtnTemp;	
}






float DS1220_Temperature_checking(void)
{
	float ret = 0.00;
	long  temp_result;
	
	temp_result = ADS1220ReadData();
	temp_result = temp_result>>8;	 
	ret = DS1220ResConvertToTem(temp_result); 	
	usleep(20);

	return ret;	
}

void DS1220_TEST(void)
{

	long ads1200Data;

		ads1200Data = ADS1220ReadData();
		printf("orig:%d\r\n", ads1200Data);
		ads1200Data = ads1200Data>>8;
		printf("data: %d\r\n", (short)ads1200Data);
		DS1220ResConvertToTem(ads1200Data);//

		usleep(20);
	
}


/*
pt100 newton algrioth

*/
#define     A       3.9083e-3 
#define     B       -5.775e-7
#define     C       -4.183e-12



float CalculateTemperatureAlg(float fR)
{
    // TODO:  ??????????????

    float  fT , fT0;
    short   i ;
  //  fR = 18.52;
    fT0 = (fR / 100 - 1) / A ;
    if(fR >= 18.52 && fR < 100) //-200?- 0?
    {
        for(i = 0 ; i < 50 ; i ++)
        {
            fT = fT0 + (fR - 100*(1 + A*fT0 + B*fT0*fT0 - 100*C*fT0*fT0*fT0 + C*fT0*fT0*fT0*fT0)) / 
                (100 * (A + 2*B*fT0 - 300*C*fT0*fT0 + 4*C*fT0*fT0*fT0)) ;
            if(fabs(fT - fT0) < 0.001)
                break ;
            else
                fT0 = fT ;
        }
        return fT;
       // l_strT.Format(_T("%.3f") , fT);
    }
    else if(fR >= 100 && fR <= 390.481)  //0?- 850?
    {
        for(i = 0 ; i < 50 ; i ++)
        {
            fT = fT0 + (fR - 100*(1 + A*fT0 + B*fT0*fT0)) / (100*(A + 2*B*fT0)) ;
            if(fabs(fT - fT0) < 0.001)
                break ;
            else
                fT0 = fT ;
        }
        return fT;
       // l_strT.Format(_T("%.3f") , fT);
    }
    else
      {
      	//printf("resitor overlimit\r\n");
				return 0;
	  }

}
