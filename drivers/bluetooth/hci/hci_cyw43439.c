/*
 * Copyright (c) 2026 Beechwoods Software
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/mfd/cyw43439.h>
#include <zephyr/bluetooth/hci.h>
#include <zephyr/bluetooth/hci_raw.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(bt_cyw43439, CONFIG_BT_HCI_DRIVER_LOG_LEVEL);

#define DT_DRV_COMPAT infineon_cyw43439_bt

struct bt_cyw43439_data {
	struct k_work rx_work;
	struct k_fifo rx_queue;
};

struct bt_cyw43439_config {
	const struct device *mfd_dev;
};

static int bt_cyw43439_send(const struct device *dev, struct net_buf *buf)
{
	const struct bt_cyw43439_config *config = dev->config;
	int ret;
	
	LOG_DBG("Sending HCI packet, type: %u, len: %u", bt_buf_get_type(buf), buf->len);
	
	/* TODO: Send via MFD to CYW43439 */
	ret = mfd_cyw43439_write(config->mfd_dev, 0, buf->data, buf->len);
	
	net_buf_unref(buf);
	return ret;
}

static int bt_cyw43439_open(const struct device *dev)
{
	LOG_INF("Opening CYW43439 Bluetooth");
	
	/* TODO: Initialize BT firmware on CYW43439 */
	
	return 0;
}

static const struct bt_hci_driver_api bt_cyw43439_api = {
	.open = bt_cyw43439_open,
	.send = bt_cyw43439_send,
};

static int bt_cyw43439_init(const struct device *dev)
{
	const struct bt_cyw43439_config *config = dev->config;
	struct bt_cyw43439_data *data = dev->data;
	
	if (!device_is_ready(config->mfd_dev)) {
		LOG_ERR("MFD device not ready");
		return -ENODEV;
	}
	
	k_fifo_init(&data->rx_queue);
	
	LOG_INF("CYW43439 Bluetooth HCI initialized");
	return 0;
}

#define BT_CYW43439_DEFINE(inst)						\
	static struct bt_cyw43439_data bt_cyw43439_data_##inst;			\
										\
	static const struct bt_cyw43439_config bt_cyw43439_config_##inst = {	\
		.mfd_dev = DEVICE_DT_GET(DT_INST_PARENT(inst)),			\
	};									\
										\
	DEVICE_DT_INST_DEFINE(inst,						\
			      bt_cyw43439_init,					\
			      NULL,						\
			      &bt_cyw43439_data_##inst,				\
			      &bt_cyw43439_config_##inst,			\
			      POST_KERNEL,					\
			      CONFIG_BT_HCI_INIT_PRIORITY,			\
			      &bt_cyw43439_api);

DT_INST_FOREACH_STATUS_OKAY(BT_CYW43439_DEFINE)
