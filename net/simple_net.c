// SPDX-License-Identifier: GPL-2.0
/*
 * simple_net.c — Simple network device driver example.
 * Demonstrates basic net_device registration and ndo operations.
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>
#include <linux/if_arp.h>
#include <linux/ip.h>
#include <linux/inet.h>

static struct net_device *simple_dev;

static netdev_tx_t simple_xmit(struct sk_buff *skb, struct net_device *dev)
{
	pr_info("simple_net: TX packet, len=%d\n", skb->len);
	dev_kfree_skb_any(skb);
	return NETDEV_TX_OK;
}

static int simple_ndo_open(struct net_device *dev)
{
	pr_info("simple_net: device opened\n");
	netif_start_queue(dev);
	return 0;
}

static int simple_ndo_stop(struct net_device *dev)
{
	pr_info("simple_net: device stopped\n");
	netif_stop_queue(dev);
	return 0;
}

static const struct net_device_ops simple_net_ops = {
	.ndo_open = simple_ndo_open,
	.ndo_stop = simple_ndo_stop,
	.ndo_start_xmit = simple_xmit,
	.ndo_validate_addr = eth_validate_addr,
	.ndo_set_mac_address = eth_mac_addr,
};

static int __init simple_net_init(void)
{
	int ret;

	simple_dev = alloc_etherdev(0);
	if (!simple_dev)
		return -ENOMEM;

	simple_dev->netdev_ops = &simple_net_ops;
	simple_dev->type = ARPHRD_ETHER;
	simple_dev->mtu = 1500;
	simple_dev->tx_queue_len = 100;
	simple_dev->flags = IFF_BROADCAST | IFF_MULTICAST;

	eth_hw_addr_set(simple_dev, "\x00\x11\x22\x33\x44\x55");

	ret = register_netdev(simple_dev);
	if (ret) {
		free_netdev(simple_dev);
		return ret;
	}

	pr_info("simple_net: device registered\n");
	return 0;
}

static void __exit simple_net_exit(void)
{
	unregister_netdev(simple_dev);
	free_netdev(simple_dev);
	pr_info("simple_net: device unregistered\n");
}

module_init(simple_net_init);
module_exit(simple_net_exit);

MODULE_AUTHOR("kabirz <jxwazxzhp@126.com>");
MODULE_DESCRIPTION("Simple network device driver example");
MODULE_LICENSE("GPL");
MODULE_VERSION("1.0");
