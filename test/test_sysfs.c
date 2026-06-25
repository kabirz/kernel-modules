/*
 * test_sysfs.c — Test program for sysfs_demo module.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

#define SYSFS_DIR "/sys/kernel/demo_sysfs"
#define SYSFS_VALUE SYSFS_DIR "/value"
#define SYSFS_TEXT SYSFS_DIR "/text"

static ssize_t sysfs_read(const char *path, char *buf, size_t size)
{
	int fd = open(path, O_RDONLY);
	if (fd < 0) {
		perror(path);
		return -1;
	}
	ssize_t ret = read(fd, buf, size - 1);
	if (ret > 0)
		buf[ret] = '\0';
	close(fd);
	return ret;
}

static ssize_t sysfs_write(const char *path, const char *value)
{
	int fd = open(path, O_WRONLY);
	if (fd < 0) {
		perror(path);
		return -1;
	}
	ssize_t ret = write(fd, value, strlen(value));
	close(fd);
	return ret;
}

int main(void)
{
	char buf[256];

	printf("=== Sysfs Demo Test ===\n\n");

	printf("--- Value Attribute ---\n");
	if (sysfs_read(SYSFS_VALUE, buf, sizeof(buf)) >= 0)
		printf("Read value: %s", buf);
	sysfs_write(SYSFS_VALUE, "42");
	if (sysfs_read(SYSFS_VALUE, buf, sizeof(buf)) >= 0)
		printf("After write: %s", buf);

	printf("\n--- Text Attribute ---\n");
	if (sysfs_read(SYSFS_TEXT, buf, sizeof(buf)) >= 0)
		printf("Read text: %s", buf);
	sysfs_write(SYSFS_TEXT, "Hello from userspace!");
	if (sysfs_read(SYSFS_TEXT, buf, sizeof(buf)) >= 0)
		printf("After write: %s", buf);

	printf("\n=== Test Complete ===\n");
	return 0;
}
