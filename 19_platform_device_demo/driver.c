#include "linux/mod_devicetable.h"
#include <linux/err.h>
#include <linux/module.h>
#include <linux/platform_device.h>

#define DEV_NAME "demo-platform"

static struct platform_device *demo_device;

static int demo_probe(struct platform_device *pdev)
{	
	pr_info("Device probed\n");
	return 0;
}

static void demo_remove(struct platform_device *pdev)
{
	pr_info("Device removed\n");
}

static struct platform_driver demo_driver = {
	.probe = demo_probe,
	.remove = demo_remove,
	.driver = {
		.name = DEV_NAME,
	},
};

static int __init demo_init(void)
{
	int ret;
	pr_info("Module intilaizing\n");
	ret = platform_driver_register(&demo_driver);
	if (ret)
		return ret;

	/* TODO: create DEV_NAME with platform_device_register_simple(). */
	platform_device_register_simple(DEV_NAME,PLATFORM_DEVICES_ID_NONE , NULL, 0)
	return 0;
}

static void __exit demo_exit(void)
{
	/* TODO: unregister demo_device before unregistering demo_driver. */
	platform_device_re
}

module_init(demo_init);
module_exit(demo_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Platform device/driver matching exercise");
