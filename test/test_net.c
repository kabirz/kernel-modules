/*
 * test_net.c — Test program for simple_net module.
 */

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <net/if.h>

#define DEVICE_NAME "simple_net"

int main(void)
{
	int sockfd;
	struct ifreq ifr;

	printf("=== Network Device Test ===\n\n");

	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if (sockfd < 0) {
		perror("socket");
		return 1;
	}

	memset(&ifr, 0, sizeof(ifr));
	strncpy(ifr.ifr_name, DEVICE_NAME, IFNAMSIZ - 1);

	/* Check interface exists */
	if (ioctl(sockfd, SIOCGIFINDEX, &ifr) < 0) {
		printf("Interface '%s' not found\n", DEVICE_NAME);
		printf("Make sure module is loaded: sudo insmod net/simple_net.ko\n");
		close(sockfd);
		return 1;
	}
	printf("Interface '%s' found (index=%d)\n", DEVICE_NAME,
	       ifr.ifr_ifindex);

	/* Get MAC */
	if (ioctl(sockfd, SIOCGIFHWADDR, &ifr) == 0) {
		unsigned char *mac = (unsigned char *)ifr.ifr_hwaddr.sa_data;
		printf("MAC: %02x:%02x:%02x:%02x:%02x:%02x\n", mac[0], mac[1],
		       mac[2], mac[3], mac[4], mac[5]);
	}

	/* Bring up */
	if (ioctl(sockfd, SIOCGIFFLAGS, &ifr) == 0) {
		ifr.ifr_flags |= IFF_UP | IFF_RUNNING;
		ioctl(sockfd, SIOCSIFFLAGS, &ifr);
		printf("Interface set UP\n");
	}

	close(sockfd);
	printf("\n=== Test Complete ===\n");
	return 0;
}
