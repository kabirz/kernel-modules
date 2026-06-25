// SPDX-License-Identifier: GPL-2.0
/*
 * simple_gpio.c — Simple GPIO driver example.
 * Demonstrates basic GPIO operations using the GPIO subsystem.
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/gpio.h>
#include <linux/gpio/consumer.h>
#include <linux/platform_device.h>
#include <linux/of.h>
#include <linux/delay.h>

struct simple_gpio {
	struct gpio_desc *led;
	struct gpio_desc *btn;
};

static int simple_gpio_probe(struct platform_device *pdev)
{
	struct simple_gpio *gpio;

	gpio = devm_kzalloc(&pdev->dev, sizeof(*gpio), GFP_KERNEL);
	if (!gpio)
		return -ENOMEM;

	gpio->led = devm_gpiod_get(&pdev->dev, "led", GPIOD_OUT_LOW);
	if (IS_ERR(gpio->led))
		return dev_err_probe(&pdev->dev, PTR_ERR(gpio->led),
				     "Failed to get LED GPIO\n");

	gpio->btn = devm_gpiod_get(&pdev->dev, "btn", GPIOD_IN);
	if (IS_ERR(gpio->btn))
		return dev_err_probe(&pdev->dev, PTR_ERR(gpio->btn),
				     "Failed to get button GPIO\n");

	platform_set_drvdata(pdev, gpio);

	gpiod_set_value(gpio->led, 1);
	msleep(1000);
	gpiod_set_value(gpio->led, 0);

	dev_info(&pdev->dev, "GPIO device probed\n");
	return 0;
}

static void simple_gpio_remove(struct platform_device *pdev)
{
	struct simple_gpio *gpio = platform_get_drvdata(pdev);

	gpiod_set_value(gpio->led, 0);
	dev_info(&pdev->dev, "GPIO device removed\n");
}

static const struct of_device_id simple_gpio_of_match[] = {
	{ .compatible = "simple,gpio-demo", },
	{ /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, simple_gpio_of_match);

static struct platform_driver simple_gpio_driver = {
	.driver = {
		.name = "simple_gpio",
		.of_match_table = simple_gpio_of_match,
	},
	.probe = simple_gpio_probe,
	.remove = simple_gpio_remove,
};

static int __init simple_gpio_init(void)
{
	return platform_driver_register(&simple_gpio_driver);
}

static void __exit simple_gpio_exit(void)
{
	platform_driver_unregister(&simple_gpio_driver);
}

module_init(simple_gpio_init);
module_exit(simple_gpio_exit);

MODULE_AUTHOR("kabirz <jxwazxzhp@126.com>");
MODULE_DESCRIPTION("Simple GPIO driver example");
MODULE_LICENSE("GPL");
MODULE_VERSION("1.0");
