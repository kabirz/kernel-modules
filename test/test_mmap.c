/*
 * test_mmap.c — Test program for mmap_demo module.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>

#define DEVICE_PATH "/dev/mmap_demo"
#define BUFFER_SIZE (4096 * 2)

int main(void)
{
	int fd;
	char buf[BUFFER_SIZE];
	char *mmap_buf;

	printf("=== mmap Demo Test ===\n");

	fd = open(DEVICE_PATH, O_RDWR);
	if (fd < 0) {
		perror("open");
		return 1;
	}

	printf("Device opened\n");

	/* Test read */
	printf("\n--- Read Test ---\n");
	if (pread(fd, buf, sizeof(buf), 0) > 0)
		printf("Read: \"%s\"\n", buf);

	/* Test write */
	printf("\n--- Write Test ---\n");
	const char *msg = "Updated via write()";
	if (pwrite(fd, msg, strlen(msg), 0) < 0)
		perror("pwrite");
	if (pread(fd, buf, sizeof(buf), 0) > 0)
		printf("After write: \"%s\"\n", buf);

	/* Test mmap */
	printf("\n--- mmap Test ---\n");
	mmap_buf = mmap(NULL, BUFFER_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED,
			fd, 0);
	if (mmap_buf == MAP_FAILED) {
		perror("mmap");
		close(fd);
		return 1;
	}

	printf("mmap original: \"%s\"\n", mmap_buf);
	strcpy(mmap_buf, "Updated via mmap()");
	printf("mmap modified: \"%s\"\n", mmap_buf);

	/* Verify via read */
	if (pread(fd, buf, sizeof(buf), 0) > 0)
		printf("Verify via read: \"%s\"\n", buf);

	munmap(mmap_buf, BUFFER_SIZE);
	close(fd);
	printf("\n=== Test Complete ===\n");
	return 0;
}
