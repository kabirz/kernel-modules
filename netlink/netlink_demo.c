// SPDX-License-Identifier: GPL-2.0
/*
 * netlink_demo.c — Netlink communication example.
 * Demonstrates kernel-userspace communication via Netlink.
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/netlink.h>
#include <linux/skbuff.h>
#include <net/sock.h>

#define NETLINK_USER 31
#define MSG_LEN 256

static struct sock *netlink_sock;
static int user_pid;

static void netlink_msg_handler(struct sk_buff *skb)
{
	struct nlmsghdr *nlh;
	char *msg;

	nlh = nlmsg_hdr(skb);
	user_pid = nlh->nlmsg_pid;

	msg = (char *)nlmsg_data(nlh);
	pr_info("netlink_demo: received from user (pid=%d): %s\n",
		user_pid, msg);
}

static int __init simple_init(void)
{
	struct netlink_kernel_cfg cfg = {
		.input = netlink_msg_handler,
	};

	netlink_sock = netlink_kernel_create(&init_net, NETLINK_USER, &cfg);
	if (!netlink_sock) {
		pr_err("netlink_demo: failed to create netlink socket\n");
		return -ENOMEM;
	}

	pr_info("netlink_demo: netlink socket created\n");
	return 0;
}

static void __exit simple_exit(void)
{
	netlink_kernel_release(netlink_sock);
	pr_info("netlink_demo: netlink socket released\n");
}

module_init(simple_init);
module_exit(simple_exit);

MODULE_AUTHOR("kabirz <jxwazxzhp@126.com>");
MODULE_DESCRIPTION("Netlink communication example");
MODULE_LICENSE("GPL");
MODULE_VERSION("1.0");
