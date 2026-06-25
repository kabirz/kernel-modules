// SPDX-License-Identifier: GPL-2.0
/*
 * simple_blk.c — Simple block device driver example.
 * Demonstrates basic block device operations: open, release, read, write.
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/blkdev.h>
#include <linux/hdreg.h>
#include <linux/blk-mq.h>
#include <linux/slab.h>

#define DISK_NAME "simple_blk"
#define DISK_SIZE (1024 * 1024) /* 1MB */

static struct gendisk *gd;
static struct blk_mq_tag_set tag_set;
static u8 *data;

static int simple_blk_open(struct gendisk *disk, blk_mode_t mode)
{
	pr_info("simple_blk: device opened\n");
	return 0;
}

static void simple_blk_release(struct gendisk *disk)
{
	pr_info("simple_blk: device released\n");
}

static blk_status_t simple_queue_rq(struct blk_mq_hw_ctx *hctx,
				    const struct blk_mq_queue_data *bd)
{
	struct request *rq = bd->rq;
	blk_status_t ret = BLK_STS_OK;
	struct bio_vec bvec;
	struct req_iterator iter;
	loff_t pos = blk_rq_pos(rq) << SECTOR_SHIFT;

	rq_for_each_segment(bvec, rq, iter) {
		void *buf = page_address(bvec.bv_page) + bvec.bv_offset;
		size_t len = bvec.bv_len;

		if (rq_data_dir(rq) == READ) {
			memcpy(buf, data + pos, len);
			pr_info("simple_blk: read %zu bytes at offset %lld\n",
				len, pos);
		} else {
			memcpy(data + pos, buf, len);
			pr_info("simple_blk: write %zu bytes at offset %lld\n",
				len, pos);
		}
		pos += len;
	}

	blk_mq_end_request(rq, ret);
	return ret;
}

static const struct block_device_operations simple_fops = {
	.owner = THIS_MODULE,
	.open = simple_blk_open,
	.release = simple_blk_release,
};

static const struct blk_mq_ops simple_mq_ops = {
	.queue_rq = simple_queue_rq,
};

static int __init simple_init(void)
{
	int ret;

	data = kvzalloc(DISK_SIZE, GFP_KERNEL);
	if (!data)
		return -ENOMEM;

	tag_set.ops = &simple_mq_ops;
	tag_set.nr_hw_queues = 1;
	tag_set.queue_depth = 128;
	tag_set.numa_node = NUMA_NO_NODE;
	tag_set.cmd_size = 0;
	tag_set.flags = 0;
	tag_set.driver_data = NULL;

	ret = blk_mq_alloc_tag_set(&tag_set);
	if (ret)
		goto out_free_data;

	gd = blk_mq_alloc_disk(&tag_set, NULL, NULL);
	if (IS_ERR(gd)) {
		ret = PTR_ERR(gd);
		goto out_free_tag;
	}

	gd->major = 0;
	gd->first_minor = 0;
	gd->minors = 1;
	gd->fops = &simple_fops;
	snprintf(gd->disk_name, 32, DISK_NAME);

	set_capacity(gd, DISK_SIZE >> SECTOR_SHIFT);

	ret = add_disk(gd);
	if (ret)
		goto out_free_disk;

	pr_info("simple_blk: block device registered\n");
	return 0;

out_free_disk:
	put_disk(gd);
out_free_tag:
	blk_mq_free_tag_set(&tag_set);
out_free_data:
	kvfree(data);
	return ret;
}

static void __exit simple_exit(void)
{
	del_gendisk(gd);
	put_disk(gd);
	blk_mq_free_tag_set(&tag_set);
	kvfree(data);
	pr_info("simple_blk: block device unregistered\n");
}

module_init(simple_init);
module_exit(simple_exit);

MODULE_AUTHOR("kabirz <jxwazxzhp@126.com>");
MODULE_DESCRIPTION("Simple block device driver example");
MODULE_LICENSE("GPL");
MODULE_VERSION("1.0");