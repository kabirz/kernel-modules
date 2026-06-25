#include <assert.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

static int random_number(int min, int max)
{
	int dev_random_fd;
	char *next_random_byte;
	int bytes_to_read;
	unsigned random_value;

	assert(max > min);

	dev_random_fd = open("/dev/random", O_RDONLY);
	assert(dev_random_fd >= 0);

	next_random_byte = (char *)&random_value;
	bytes_to_read = sizeof(random_value);

	do {
		int bytes_read;
		bytes_read =
			read(dev_random_fd, next_random_byte, bytes_to_read);
		bytes_to_read -= bytes_read;
		next_random_byte += bytes_read;
	} while (bytes_to_read > 0);

	close(dev_random_fd);

	return min + (random_value % (max - min + 1));
}

int main(void)
{
	printf("%d\n", random_number(13, 85));
	printf("%d\n", random_number(13, 85));
	printf("%d\n", random_number(13, 85));
	return 0;
}
