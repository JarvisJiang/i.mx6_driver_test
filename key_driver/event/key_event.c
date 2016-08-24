#include <stdio.h>
#include <linux/input.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#define DEV_PATH "/dev/input/event2"   //difference is possible
/*
KEY_HOME  102
KEY_ENTER  28
*/
/*
struct input_event {
	struct timeval time;
	__u16 type;
	__u16 code;
	__s32 value;
};
*/
int main()
{
	int keys_fd;
	char ret[2];
	struct input_event t;
	keys_fd=open(DEV_PATH, O_RDONLY);
	printf("det even\n");
	if(keys_fd <= 0)
	{
		printf("open /dev/input/event2 device error!\n");
		return -1;
	}
	while(1)
	{

		if(read(keys_fd, &t, sizeof(t)) == sizeof(t))
		{
			printf("key %d %s\n", t.code, (t.value) ? "Pressed" : "Released");
			if(t.type==KEY_HOME)
				if(t.value==0 || t.value==1)
				{
					printf("key %d %s\n", t.code, (t.value) ? "Pressed" : "Released");
					if(t.code == KEY_ESC)
						break;
				}
			if(t.type==KEY_ENTER)
				if(t.value==0 || t.value==1)
				{
					printf("key %d %s\n", t.code, (t.value) ? "Pressed" : "Released");
					if(t.code == KEY_ESC)
						break;
				}
		}

		if(t.type==KEY_ENTER)
				if(t.value==0 || t.value==1)
				{
					printf("key enter: %d %s\n", t.code, (t.value) ? "Pressed" : "Released");
					if(t.code == KEY_ESC)
						break;
				}
		
	}
	close(keys_fd);
	return 0;
}


