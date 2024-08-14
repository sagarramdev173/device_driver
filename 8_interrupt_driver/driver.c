#include <linux/module.h>
#include <linux/init.h>
#include <linux/gpio.h>
#include <linux/interrupt.h>

/* Meta Information */
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Sagar Sagar");
MODULE_DESCRIPTION("A simple LKM for a gpio interrupt");

/** variable contains pin number o interrupt controller to which GPIO 17 is mapped to */
unsigned int irq_number;
int gpio_input  = 528;
int gpio_output = 532;
static int value = 0;
/*
 *
 * @brief Interrupt service routine is called, when interrupt is triggered
 */
static irqreturn_t gpio_irq_handler(int irq, void *dev_id)
{
	printk("gpio_irq: Interrupt was triggered and ISR was called!\n");
  value = (value == 0 ) ? 1 :0;
  gpio_set_value(gpio_output,value);
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

	printk("Done!\n");
	printk("GPIO %d is mapped to IRQ Nr.: %d\n", gpio_input,irq_number);
	return 0;
}

/**
 * @brief This function is called, when the module is removed from the kernel
 */
static void __exit ModuleExit(void) {
	printk("gpio_irq: Unloading module... ");
	free_irq(irq_number, NULL);
  gpio_set_value(gpio_output,0);
	gpio_free(gpio_output);
	gpio_free(gpio_input);

}

module_init(ModuleInit);
module_exit(ModuleExit);
