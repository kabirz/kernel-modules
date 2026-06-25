#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/rtc.h>
#include <linux/ioctl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv)
{
	int fd;
	char data[256];

	int retval;
	fd = open("/dev/xmimx", O_RDWR);
	if (fd == -1) {
		perror("error open\n");
		exit(-1);
	}
	printf("open /dev/xmimx ok\n");

	retval = write(fd, "ZhongHuiping", 12);
	if (retval == -1) {
		perror("write error\n");
		exit(-1);
	}

	memset(data, '\0', 256);

	retval = read(fd, data, 12);

	if (retval == -1) {
		perror("read error\n");
		exit(-1);
	}

	data[retval] = '\0';

	printf("read ok:%s\n", data);
	close(fd);
	return 0;
}
