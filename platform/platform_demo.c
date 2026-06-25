// SPDX-License-Identifier: GPL-2.0
/*
 * platform_demo.c — Platform device driver example.
 * Demonstrates platform device registration and driver binding.
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/io.h>
#include <linux/of.h>

#define DEVICE_NAME "simple_platform"

static void __iomem *reg_base;

static int platform_demo_probe(struct platform_device *pdev)
{
	struct resource *res;

	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (!res)
		return dev_err_probe(&pdev->dev, -ENODEV,
				     "Failed to get resource\n");

	reg_base = devm_ioremap_resource(&pdev->dev, res);
	if (IS_ERR(reg_base))
		return PTR_ERR(reg_base);

	dev_info(&pdev->dev, "Platform device probed, base=%pR\n", res);
	return 0;
}

static void platform_demo_remove(struct platform_device *pdev)
{
	dev_info(&pdev->dev, "Platform device removed\n");
}

static const struct of_device_id platform_demo_of_match[] = {
	{
		.compatible = "simple,platform-demo",
	},
	{ /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, platform_demo_of_match);

static struct platform_driver platform_demo_driver = {
	.driver = {
		.name = DEVICE_NAME,
		.of_match_table = platform_demo_of_match,
	},
	.probe = platform_demo_probe,
	.remove = platform_demo_remove,
};

static int __init platform_demo_init(void)
{
	return platform_driver_register(&platform_demo_driver);
}

static void __exit platform_demo_exit(void)
{
	platform_driver_unregister(&platform_demo_driver);
}

module_init(platform_demo_init);
module_exit(platform_demo_exit);

MODULE_AUTHOR("kabirz <jxwazxzhp@126.com>");
MODULE_DESCRIPTION("Platform device driver example");
MODULE_LICENSE("GPL");
MODULE_VERSION("1.0");
