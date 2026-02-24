/*
 * Copyright (c) 2026 Beechwoods Software
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/drivers/mfd/cyw43439.h>
#include <zephyr/drivers/spi.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(mfd_cyw43439, CONFIG_MFD_LOG_LEVEL);

#define DT_DRV_COMPAT infineon_cyw43439

struct mfd_cyw43439_config {
	struct spi_dt_spec bus;
	struct gpio_dt_spec reg_on_gpio;
	struct gpio_dt_spec host_wake_gpio;
};

struct mfd_cyw43439_data {
	struct k_mutex lock;
	uint8_t gpio_state;
};

int mfd_cyw43439_read(const struct device *dev, uint32_t addr, uint8_t *data, size_t len)
{
	const struct mfd_cyw43439_config *config = dev->config;
	struct mfd_cyw43439_data *dev_data = dev->data;
	int ret;

	k_mutex_lock(&dev_data->lock, K_FOREVER);
	
	/* Simplified SPI read - actual implementation needs WHD protocol */
	struct spi_buf tx_buf = {.buf = &addr, .len = sizeof(addr)};
	struct spi_buf rx_buf = {.buf = data, .len = len};
	struct spi_buf_set tx = {.buffers = &tx_buf, .count = 1};
	struct spi_buf_set rx = {.buffers = &rx_buf, .count = 1};
	
	ret = spi_transceive_dt(&config->bus, &tx, &rx);
	
	k_mutex_unlock(&dev_data->lock);
	return ret;
}

int mfd_cyw43439_write(const struct device *dev, uint32_t addr, const uint8_t *data, size_t len)
{
	const struct mfd_cyw43439_config *config = dev->config;
	struct mfd_cyw43439_data *dev_data = dev->data;
	int ret;

	k_mutex_lock(&dev_data->lock, K_FOREVER);
	
	/* Simplified SPI write - actual implementation needs WHD protocol */
	struct spi_buf bufs[] = {
		{.buf = &addr, .len = sizeof(addr)},
		{.buf = (void *)data, .len = len}
	};
	struct spi_buf_set tx = {.buffers = bufs, .count = 2};
	
	ret = spi_write_dt(&config->bus, &tx);
	
	k_mutex_unlock(&dev_data->lock);
	return ret;
}

int mfd_cyw43439_gpio_set(const struct device *dev, uint8_t pin, bool value)
{
	struct mfd_cyw43439_data *dev_data = dev->data;
	
	if (pin > 2) {
		return -EINVAL;
	}
	
	k_mutex_lock(&dev_data->lock, K_FOREVER);
	
	if (value) {
		dev_data->gpio_state |= BIT(pin);
	} else {
		dev_data->gpio_state &= ~BIT(pin);
	}
	
	/* TODO: Write to CYW43439 GPIO register via WHD */
	
	k_mutex_unlock(&dev_data->lock);
	return 0;
}

int mfd_cyw43439_gpio_get(const struct device *dev, uint8_t pin, bool *value)
{
	struct mfd_cyw43439_data *dev_data = dev->data;
	
	if (pin > 2 || !value) {
		return -EINVAL;
	}
	
	k_mutex_lock(&dev_data->lock, K_FOREVER);
	*value = (dev_data->gpio_state & BIT(pin)) != 0;
	k_mutex_unlock(&dev_data->lock);
	
	return 0;
}

static int mfd_cyw43439_init(const struct device *dev)
{
	const struct mfd_cyw43439_config *config = dev->config;
	struct mfd_cyw43439_data *data = dev->data;
	int ret;

	LOG_INF("Initializing CYW43439 MFD");

	k_mutex_init(&data->lock);

	if (!spi_is_ready_dt(&config->bus)) {
		LOG_ERR("SPI bus not ready");
		return -ENODEV;
	}
	
	LOG_INF("SPI bus ready");

	/* Configure power control GPIO */
	if (config->reg_on_gpio.port) {
		ret = gpio_pin_configure_dt(&config->reg_on_gpio, GPIO_OUTPUT_INACTIVE);
		if (ret < 0) {
			LOG_ERR("Failed to configure REG_ON GPIO: %d", ret);
			return ret;
		}
		
		/* Power on the chip */
		k_sleep(K_MSEC(10));
		gpio_pin_set_dt(&config->reg_on_gpio, 1);
		k_sleep(K_MSEC(50));
		LOG_INF("Chip powered on");
	}

	/* Configure host wake GPIO */
	if (config->host_wake_gpio.port) {
		ret = gpio_pin_configure_dt(&config->host_wake_gpio, GPIO_INPUT);
		if (ret < 0) {
			LOG_ERR("Failed to configure HOST_WAKE GPIO: %d", ret);
			return ret;
		}
	}

	LOG_INF("CYW43439 MFD initialized successfully");
	return 0;
}

#define MFD_CYW43439_DEFINE(inst)						\
	static struct mfd_cyw43439_data mfd_cyw43439_data_##inst;		\
										\
	static const struct mfd_cyw43439_config mfd_cyw43439_config_##inst = {	\
		.bus = SPI_DT_SPEC_INST_GET(inst,				\
			SPI_OP_MODE_MASTER | SPI_TRANSFER_MSB |			\
			SPI_WORD_SET(8) | SPI_HALF_DUPLEX, 0),			\
		.reg_on_gpio = GPIO_DT_SPEC_INST_GET_OR(inst, wifi_reg_on_gpios, {0}), \
		.host_wake_gpio = GPIO_DT_SPEC_INST_GET_OR(inst, wifi_host_wake_gpios, {0}), \
	};									\
										\
	DEVICE_DT_INST_DEFINE(inst,						\
			      mfd_cyw43439_init,				\
			      NULL,						\
			      &mfd_cyw43439_data_##inst,			\
			      &mfd_cyw43439_config_##inst,			\
			      POST_KERNEL,					\
			      CONFIG_MFD_INIT_PRIORITY,				\
			      NULL);

DT_INST_FOREACH_STATUS_OKAY(MFD_CYW43439_DEFINE)
