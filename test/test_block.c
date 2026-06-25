/*
 * test_block.c — Test program for simple_blk module.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#define DEVICE_PATH "/dev/simple_blk"

static void test_write(int fd, const char *data, off_t offset)
{
	ssize_t ret = pwrite(fd, data, strlen(data), offset);
	if (ret < 0)
		perror("write");
	else
		printf("Write %zd bytes at offset %ld\n", ret, offset);
}

static void test_read(int fd, char *buf, size_t size, off_t offset)
{
	ssize_t ret = pread(fd, buf, size, offset);
	if (ret < 0)
		perror("read");
	else
		printf("Read  %zd bytes: \"%s\"\n", ret, buf);
}

int main(void)
{
	int fd;
	char buf[512];

	printf("=== Block Device Test ===\n");

	fd = open(DEVICE_PATH, O_RDWR);
	if (fd < 0) {
		perror("open");
		return 1;
	}

	test_write(fd, "Hello Block Device!", 0);
	test_read(fd, buf, sizeof(buf), 0);

	test_write(fd, "Second write at offset 512", 512);
	test_read(fd, buf, sizeof(buf), 512);

	close(fd);
	printf("=== Test Complete ===\n");
	return 0;
}
