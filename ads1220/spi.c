

unsigned char spi_send_byte(unsigned char byte)
{
	int ret;
	unsigned char tx[1] = {0};
	unsigned char rx[1] = 0;
	tx[0]  = byte; 
	struct spi_ioc_transfer tr = {
		.tx_buf = (unsigned long)tx,
		.rx_buf = (unsigned long)rx,
		.len = ARRAY_SIZE(tx),
		.delay_usecs = delay,
		.speed_hz = speed,
		.bits_per_word = bits,
	};

	ret = ioctl(fd, SPI_IOC_MESSAGE(1), &tr);
	if (ret < 1)
		pabort("can't send spi message");

	for (ret = 0; ret < ARRAY_SIZE(tx); ret++) {
		if (!(ret % 6))
			puts("");
		printf("recive send byte %.2X ", rx[ret]);
	}
	puts("");	
}

unsigned char spi_send_buf(unsigned char *buf, int size)
{
	int ret;
	unsigned char tx[32] = {0};
	unsigned char rx[32] = 0;
	int i;
	if(32 < size)
	{
		printf("err: too large!\r\n");
		return 0;
	}
	for(i = 0; i<size; i++)
	{
		tx[i] = buf[i];
	} 
	struct spi_ioc_transfer tr = {
		.tx_buf = (unsigned long)tx,
		.rx_buf = (unsigned long)rx,
		.len = ARRAY_SIZE(tx),
		.delay_usecs = delay,
		.speed_hz = speed,
		.bits_per_word = bits,
	};

	ret = ioctl(fd, SPI_IOC_MESSAGE(1), &tr);
	if (ret < 1)
		pabort("can't send spi message");	
	for (ret = 0; ret < ARRAY_SIZE(tx); ret++) {
		if (!(ret % 6))
			puts("");
		printf("recive send byte %.2X ", rx[ret]);
		buf[0] = rx[0];
	}
	puts("");	
}

unsigned char spi_receive_byte(unsigned char *buf)
{
	int ret;
	unsigned char tx[1] = {0};
	unsigned char rx[1] = 0;
	tx[0]  = 0; 
	struct spi_ioc_transfer tr = {
		.tx_buf = (unsigned long)tx,
		.rx_buf = (unsigned long)rx,
		.len = ARRAY_SIZE(tx),
		.delay_usecs = delay,
		.speed_hz = speed,
		.bits_per_word = bits,
	};

	ret = ioctl(fd, SPI_IOC_MESSAGE(1), &tr);
	if (ret < 1)
		pabort("can't send spi message");

	for (ret = 0; ret < ARRAY_SIZE(tx); ret++) {
		if (!(ret % 6))
			puts("");
		printf("recive send byte %.2X ", rx[ret]);
		buf[0] = rx[0];
	}
	puts("");	
}


unsigned char spi_recive_buf(unsigned char *buf, int size)
{
	int ret;
	unsigned char tx[32] = {0};
	unsigned char rx[32] = 0;
	if(32<size)
	{
		printf("err: size too large\n");
		return 0;
	}
	for(i = 0; i < 32; i+)
	tx[i]  = o; 
	struct spi_ioc_transfer tr = {
		.tx_buf = (unsigned long)tx,
		.rx_buf = (unsigned long)rx,
		.len = ARRAY_SIZE(tx),
		.delay_usecs = delay,
		.speed_hz = speed,
		.bits_per_word = bits,
	};

	ret = ioctl(fd, SPI_IOC_MESSAGE(1), &tr);
	if (ret < 1)
		pabort("can't send spi message");

	for (ret = 0; ret < size; ret++) {
		if (!(ret % 6))
			puts("");
		buf[ret] = rx[ret]);
		printf("recive send byte %.2X ", rx[ret]);
	}
	puts("");	
}
