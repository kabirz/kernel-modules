#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
	const char msg[] = "Hello cdev\n";
	int fd;
	ssize_t size;
	char buf[1024];

	fd = open(argv[1], O_RDWR);
	if (fd < 0) {
		perror("open() error\n");
		exit(1);
	}

	size = write(fd, msg, strlen(msg));

	size = read(fd, buf, sizeof(buf) - 1);
	if (size > 0)
		buf[size] = '\0';

	ioctl(fd, 1, NULL);

	printf("%s\n", buf);

	close(fd);

	return 0;
}
