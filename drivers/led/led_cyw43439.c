/*
 * Copyright (c) 2026 Beechwoods Software
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/drivers/led.h>
#include <zephyr/drivers/mfd/cyw43439.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(led_cyw43439, CONFIG_LED_LOG_LEVEL);

#define DT_DRV_COMPAT infineon_cyw43439_led

struct led_cyw43439_config {
	const struct device *mfd_dev;
	uint8_t gpio_pin;
};

static int led_cyw43439_on(const struct device *dev, uint32_t led)
{
	const struct led_cyw43439_config *config = dev->config;
	
	if (led != 0) {
		return -EINVAL;
	}
	
	return mfd_cyw43439_gpio_set(config->mfd_dev, config->gpio_pin, true);
}

static int led_cyw43439_off(const struct device *dev, uint32_t led)
{
	const struct led_cyw43439_config *config = dev->config;
	
	if (led != 0) {
		return -EINVAL;
	}
	
	return mfd_cyw43439_gpio_set(config->mfd_dev, config->gpio_pin, false);
}

static int led_cyw43439_init(const struct device *dev)
{
	const struct led_cyw43439_config *config = dev->config;
	
	if (!device_is_ready(config->mfd_dev)) {
		LOG_ERR("MFD device not ready");
		return -ENODEV;
	}
	
	LOG_INF("CYW43439 LED initialized");
	return 0;
}

static const struct led_driver_api led_cyw43439_api = {
	.on = led_cyw43439_on,
	.off = led_cyw43439_off,
};

#define LED_CYW43439_DEFINE(inst)						\
	static const struct led_cyw43439_config led_cyw43439_config_##inst = {	\
		.mfd_dev = DEVICE_DT_GET(DT_INST_PARENT(inst)),			\
		.gpio_pin = DT_INST_PROP(inst, gpios),				\
	};									\
										\
	DEVICE_DT_INST_DEFINE(inst,						\
			      led_cyw43439_init,				\
			      NULL,						\
			      NULL,						\
			      &led_cyw43439_config_##inst,			\
			      POST_KERNEL,					\
			      CONFIG_LED_INIT_PRIORITY,				\
			      &led_cyw43439_api);

DT_INST_FOREACH_STATUS_OKAY(LED_CYW43439_DEFINE)
