/*
 * test_input.c — Test program for simple_input module.
 */

#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <linux/input.h>

int main(void)
{
	DIR *dir;
	struct dirent *entry;
	char path[512], name[256];
	int fd = -1;

	printf("=== Input Device Test ===\n\n");

	/* Find Simple Input device */
	dir = opendir("/dev/input");
	if (!dir) {
		perror("opendir");
		return 1;
	}

	while ((entry = readdir(dir)) != NULL) {
		if (strncmp(entry->d_name, "event", 5) != 0)
			continue;
		snprintf(path, sizeof(path), "/dev/input/%s", entry->d_name);
		fd = open(path, O_RDONLY | O_NONBLOCK);
		if (fd < 0)
			continue;
		if (ioctl(fd, EVIOCGNAME(sizeof(name)), name) >= 0 &&
		    strstr(name, "Simple Input")) {
			printf("Found: %s (%s)\n", path, name);
			break;
		}
		close(fd);
		fd = -1;
	}
	closedir(dir);

	if (fd < 0) {
		printf("Device not found. Load module: sudo insmod input/simple_input.ko\n");
		return 1;
	}

	/* Read events for 3 seconds */
	printf("\nListening for events (3s)...\n");
	struct timeval tv = { .tv_sec = 3, .tv_usec = 0 };
	setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

	struct input_event ev;
	while (read(fd, &ev, sizeof(ev)) == sizeof(ev)) {
		printf("Event: type=%d code=%d value=%d\n", ev.type, ev.code,
		       ev.value);
	}

	close(fd);
	printf("\n=== Test Complete ===\n");
	return 0;
}
