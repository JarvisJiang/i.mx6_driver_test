#include <stdio.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#define DRIVER_NAME "gpioctl"
main(int argc, char** argv){
	int fd;
	char *hello_node = "/dev/gpioctl";
	printf("the total of cmd: %d\n",argc);
	printf("app name = %s\n",argv[0]);
	printf("first  argv = %s\n",argv[1]);
	printf("second argv = %s\n",argv[2]);
/*O_RDWR只读打开,O_NDELAY非阻塞方式*/	
	if((fd = open(hello_node,O_RDWR|O_NDELAY))<0){
		printf("APP open %s failed",hello_node);
	}
	else{
		printf("/dev/ioctl\n");
		ioctl(fd,atoi(argv[1]),atoi(argv[2]));
		}
	
	close(fd);
}