/*
 * Copyright 2004-2009 Freescale Semiconductor, Inc. All rights reserved.
 */

/*
 * The code contained herein is licensed under the GNU General Public
 * License. You may obtain a copy of the GNU General Public License
 * Version 2 or later at the following locations:
 *
 * http://www.opensource.org/licenses/gpl-license.html
 * http://www.gnu.org/copyleft/gpl.html
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#define LOOPBACK        0x8000
/******************************************
 信号处理函数，设备wait_flag=FASLE
 ******************************************************/
 int uart_file1;
void signal_handler_IO(int status)
{
	int i;
	char buf[6];
	printf("received SIGIO signale.\n");
	read(uart_file1, buf, 5);
	for(i = 0 ; i < 5; i++)
	{
		printf("buf[%d] =%x\n",i,buf[i]);
	}
	

}
////////////////////////////////////////////////////////////////////////////////
/**
*@brief   设置串口数据位，停止位和效验位
*@param  fd     类型  int  打开的串口文件句柄
*@param  databits 类型  int 数据位   取值 为 7 或者8
*@param  stopbits 类型  int 停止位   取值为 1 或者2
*@param  parity  类型  int  效验类型 取值为N,E,O,,S
*/
int set_Parity(int fd,int databits,int stopbits,int parity)
{ 
	struct termios options; 
	if  ( tcgetattr( fd,&options)  !=  0) { 
		perror("SetupSerial 1");     
		return(FALSE);  
	}
	options.c_cflag &= ~CSIZE; 
	switch (databits) /*设置数据位数*/
	{   
	case 7:		
		options.c_cflag |= CS7; 
		break;
	case 8:     
		options.c_cflag |= CS8;
		break;   
	default:    
		fprintf(stderr,"Unsupported data size\n"); return (FALSE);  
	}
	switch (parity) 
	{   
		case 'n':
		case 'N':    
			options.c_cflag &= ~PARENB;   /* Clear parity enable */
			options.c_iflag &= ~INPCK;     /* Enable parity checking */ 
			break;  
		case 'o':   
		case 'O':     
			options.c_cflag |= (PARODD | PARENB); /* 设置为奇效验*/  
			options.c_iflag |= INPCK;             /* Disnable parity checking */ 
			break;  
		case 'e':  
		case 'E':   
			options.c_cflag |= PARENB;     /* Enable parity */    
			options.c_cflag &= ~PARODD;   /* 转换为偶效验*/     
			options.c_iflag |= INPCK;       /* Disnable parity checking */
			break;
		case 'S': 
		case 's':  /*as no parity*/   
			options.c_cflag &= ~PARENB;
			options.c_cflag &= ~CSTOPB;break;  
		default:   
			fprintf(stderr,"Unsupported parity\n");    
			return (FALSE);  
		}  
	/* 设置停止位*/  
	switch (stopbits)
	{   
		case 1:    
			options.c_cflag &= ~CSTOPB;  
			break;  
		case 2:    
			options.c_cflag |= CSTOPB;  
		   break;
		default:    
			 fprintf(stderr,"Unsupported stop bits\n");  
			 return (FALSE); 
	} 
	/* Set input parity option */ 
	if (parity != 'n')   
		options.c_iflag |= INPCK; 
	tcflush(fd,TCIFLUSH);
	options.c_cc[VTIME] = 150; /* 设置超时15 seconds*/   
	options.c_cc[VMIN] = 0; /* Update the options and do it NOW */
	if (tcsetattr(fd,TCSANOW,&options) != 0)   
	{ 
		perror("SetupSerial 3");   
		return (FALSE);  
	} 
	options.c_lflag  &= ~(ICANON | ECHO | ECHOE | ISIG);  /*Input*/
	options.c_oflag  &= ~OPOST;   /*Output*/
	return (TRUE);  
}
int main(int argc, char **argv)
{
        
        unsigned int line_val;
        char buf[5];
        struct termios mxc, old;
        int retval;
		
		struct sigaction saio; /*definition of signal axtion */
		
        printf("Test: MXC UART!\n");
        printf("Usage: mxc_uart_test <UART device name, opens UART2 if no dev name is specified>\n");

        if (argc == 1) {
                /* No Args, open UART 2 */
                if ((uart_file1 = open("/dev/ttymxc1", O_RDWR)) == -1) {
                        printf("Error opening UART 2\n");
                        exit(1);
                } else {
                        printf("Test: UART 2 opened\n");
                }
        } else {
                /* Open the specified UART device */
                if ((uart_file1 = open(*++argv, O_RDWR)) == -1) {
                        printf("Error opening %s\n", *argv);
                        exit(1);
                } else {
                        printf("%s opened\n", *argv);
                }
        }
		
		/* install the signal handle before making the device asynchronous*/
		saio.sa_handler = signal_handler_IO;
		sigemptyset(&saio.sa_mask);
		//saio.sa_mask = 0; 必须用sigemptyset函数初始话act结构的sa_mask成员

		saio.sa_flags = 0;
		saio.sa_restorer = NULL;
		sigaction(SIGIO,&saio,NULL);

		/* allow the process to recevie SIGIO*/
		fcntl(uart_file1,F_SETOWN,getpid());
		/* Make the file descriptor asynchronous*/
		fcntl(uart_file1,F_SETFL,FASYNC);
		

        tcgetattr(uart_file1, &old);
        mxc = old;
        mxc.c_lflag &= ~(ICANON | ECHO | ISIG);
        retval = tcsetattr(uart_file1, TCSANOW, &mxc);
		cfsetispeed(&Opt, B115200); 
        printf("Attributes set\n");

        line_val = LOOPBACK;
       // ioctl(uart_file1, TIOCMSET, &line_val);
        printf("Test: IOCTL Set\n");

        write(uart_file1, "Test\0", 5);
        printf("Data Written= Test\n");
        while(1)
		{
			read(uart_file1, buf, 5);
			printf("Data Read back= %s\n", buf);
			sleep(2);
		}
        
        ioctl(uart_file1, TIOCMBIC, &line_val);

        retval = tcsetattr(uart_file1, TCSAFLUSH, &old);

        close(uart_file1);
        return 0;
}
