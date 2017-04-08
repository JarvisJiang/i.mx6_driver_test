#!/bin/bash	
objs = spidev_test.c  ads1220.c
 
ads1220 : $(objs)
	arm-fsl-linux-gnueabi-gcc -I ./ -o ads1220 $(objs)
 
.PHONY: clean
clean:
	-rm  ads1220

