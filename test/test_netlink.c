/*
 * test_netlink.c — Test program for netlink_demo module.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <linux/netlink.h>

#define NETLINK_USER 31
#define MAX_PAYLOAD 1024

int main(void)
{
	int sockfd;
	struct sockaddr_nl src_addr, dest_addr;
	struct nlmsghdr *nlh;
	struct iovec iov;
	struct msghdr msg;

	printf("=== Netlink Demo Test ===\n\n");

	sockfd = socket(AF_NETLINK, SOCK_RAW, NETLINK_USER);
	if (sockfd < 0) {
		perror("socket");
		return 1;
	}

	memset(&src_addr, 0, sizeof(src_addr));
	src_addr.nl_family = AF_NETLINK;
	src_addr.nl_pid = getpid();

	if (bind(sockfd, (struct sockaddr *)&src_addr, sizeof(src_addr)) < 0) {
		perror("bind");
		close(sockfd);
		return 1;
	}

	memset(&dest_addr, 0, sizeof(dest_addr));
	dest_addr.nl_family = AF_NETLINK;
	dest_addr.nl_pid = 0;

	nlh = (struct nlmsghdr *)malloc(NLMSG_SPACE(MAX_PAYLOAD));
	memset(nlh, 0, NLMSG_SPACE(MAX_PAYLOAD));
	nlh->nlmsg_len = NLMSG_SPACE(MAX_PAYLOAD);
	nlh->nlmsg_pid = getpid();
	nlh->nlmsg_type = NLMSG_DONE;
	strcpy(NLMSG_DATA(nlh), "Hello from userspace!");

	iov.iov_base = nlh;
	iov.iov_len = nlh->nlmsg_len;

	memset(&msg, 0, sizeof(msg));
	msg.msg_name = &dest_addr;
	msg.msg_namelen = sizeof(dest_addr);
	msg.msg_iov = &iov;
	msg.msg_iovlen = 1;

	printf("Sending message to kernel...\n");
	if (sendmsg(sockfd, &msg, 0) < 0) {
		perror("sendmsg");
		free(nlh);
		close(sockfd);
		return 1;
	}
	printf("Message sent\n");

	/* Wait for reply */
	struct timeval tv = { .tv_sec = 2, .tv_usec = 0 };
	setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

	memset(nlh, 0, NLMSG_SPACE(MAX_PAYLOAD));
	if (recvmsg(sockfd, &msg, 0) > 0)
		printf("Received: \"%s\"\n", (char *)NLMSG_DATA(nlh));
	else
		printf("No reply (timeout)\n");

	free(nlh);
	close(sockfd);
	printf("\n=== Test Complete ===\n");
	return 0;
}
