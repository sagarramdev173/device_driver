#define DRIVER_PREFIX "demo_driver: " 
#define pr_fmt(fmt) DRIVER_PREFIX  fmt

#include <linux/module.h>
#include <linux/init.h>
#include <linux/gpio.h>
#include <linux/interrupt.h>

/* Meta Information */
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Sagar Sagar");
MODULE_DESCRIPTION("A simple LKM for a gpio workqueue which automatically turns off LED after 5 secs");

/** variable contains pin number o interrupt controller to which GPIO 17 is mapped to */
unsigned int irq_number;
int gpio_input  = 528;
int gpio_output = 532;
static int value = 0;

/* work queue data  */
static struct delayed_work my_delayed_wq;
static struct work_struct my_wq;

/*
 * @brief work queue function 
 *
 */
void my_wq_function(struct work_struct * work)
{
	pr_info("executing workqueue function\n");
  value = (value == 0 ) ? 1 :0;
  gpio_set_value(gpio_output,value);
}
/*
 * @brief Interrupt service routine is called, when interrupt is triggered
 *
 */
static irqreturn_t gpio_irq_handler(int irq, void *dev_id)
{
	printk("gpio_irq: Interrupt was triggered and ISR was called!\n");
	schedule_work(&my_wq);
	schedule_delayed_work(&my_delayed_wq, msecs_to_jiffies(5000));
	return IRQ_HANDLED;
}

/**
 * @brief This function is called, when the module is loaded into the kernel
 */
static int __init ModuleInit(void) {
	printk("qpio_irq: Loading module... ");

	/* Setup the gpio */
	if(gpio_request(gpio_input, "rpi-gpio-16")) {
		printk("Error!\nCan not allocate GPIO %d\n", gpio_input);
		return -1;
	}

	if(gpio_request(gpio_output, "rpi-gpio-20")) {
		printk("Error!\nCan not allocate GPIO %d\n", gpio_input);
		return -1;
	}
	/* Set GPIO 17 direction */
	if(gpio_direction_input(gpio_input)) {
		printk("Error!\nCan not set GPIO 17 to input!\n");
		gpio_free(gpio_input);
		return -1;
	}

	if(gpio_direction_output(gpio_output, 0)) {
		printk("Error!\nCan not set GPIO 20 to input!\n");
		gpio_free(gpio_output);
		gpio_free(gpio_input);
		return -1;
	}
	/* Setup the interrupt */
	irq_number = gpio_to_irq(gpio_input);

	if(request_irq(irq_number, gpio_irq_handler, IRQF_TRIGGER_RISING, "my_gpio_irq", NULL) != 0){
		printk("Error!\nCan not request interrupt nr.: %d\n", irq_number);
		gpio_free(gpio_input);
		return -1;
	}
	 /* creating dynamic work queue   */
	INIT_DELAYED_WORK(&my_delayed_wq, my_wq_function);
	INIT_WORK(&my_wq, my_wq_function);

	printk("Done!\n");
	printk("GPIO %d is mapped to IRQ Nr.: %d\n", gpio_input,irq_number);
	return 0;
}

/**
 * @brief This function is called, when the module is removed from the kernel
 */
static void __exit ModuleExit(void) {
	printk("gpio_irq: Unloading module...\n ");
	cancel_delayed_work_sync(&my_delayed_wq);
	free_irq(irq_number, NULL);
  gpio_set_value(gpio_output,0);
	gpio_free(gpio_output);
	gpio_free(gpio_input);

}

module_init(ModuleInit);
module_exit(ModuleExit);
