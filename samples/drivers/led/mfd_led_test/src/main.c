/*
 * MFD LED Test for Pico W / Pico 2 W
 * Copyright (c) 2026 Beechwoods Software
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/drivers/led.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(mfd_led_test, LOG_LEVEL_INF);

int main(void)
{
	const struct device *led;
	int ret;
	
	printk("\n=== MFD LED Test for Pico W/2W ===\n");
	
	led = DEVICE_DT_GET_ANY(infineon_cyw43439_led);
	if (!led) {
		printk("ERROR: No CYW43439 LED device found\n");
		printk("Make sure you're using *_mfd board variant\n");
		return -ENODEV;
	}
	
	if (!device_is_ready(led)) {
		printk("ERROR: LED device not ready\n");
		return -ENODEV;
	}
	
	printk("LED device: %s\n", led->name);
	printk("Starting blink test...\n\n");
	
	while (1) {
		ret = led_on(led, 0);
		if (ret < 0) {
			printk("LED ON failed: %d\n", ret);
		} else {
			printk("LED: ON\n");
		}
		k_sleep(K_MSEC(500));
		
		ret = led_off(led, 0);
		if (ret < 0) {
			printk("LED OFF failed: %d\n", ret);
		} else {
			printk("LED: OFF\n");
		}
		k_sleep(K_MSEC(500));
	}
	
	return 0;
}
