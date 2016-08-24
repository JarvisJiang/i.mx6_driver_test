#include <stdio.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>


#define CMD_ENABLE_A4988     49
#define CMD_DISABLE_A4988	 59
#define CMD_DIR_POSITIVE	 69
#define CMD_DIR_NEGATIVE 	 79

/*argv[1] is cmd , argv[2] is io_arg*/
int main(int argc , char **argv){
	int fd;
	char *pwmnode = "/dev/a4988_node0";

	/*O_RDWR只读打开,O_NDELAY非阻塞方式*/	
	if((fd = open(pwmnode,O_RDWR|O_NDELAY))<0){
		printf("APP open %s failed!\n",pwmnode);
	}
	else{
		printf("APP open %s success!\n",pwmnode);
			ioctl(fd,atoi(argv[1]),atoi(argv[2]));
			printf("APP ioctl %s ,cmd is %s! io_arg is %s!\n",pwmnode,argv[1],argv[2]);
	}
	
	close(fd);
}
