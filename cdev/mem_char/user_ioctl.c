#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#define MEM_TYPE 'Z'
#define MEM_RESET _IO(MEM_TYPE, 1)
#define MEM_RESIZE _IOW(MEM_TYPE, 2, int)

int main(void)
{
	int fd, ret;

	fd = open("/dev/char2", O_RDWR);
	if (fd == -1) {
		perror("open /dev/char2");
		exit(1);
	}

	ret = ioctl(fd, MEM_RESET);
	if (ret)
		printf("Reset buffer error\n");
	else
		printf("Reset buffer OK\n");

	return 0;
}
