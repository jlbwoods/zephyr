/*
 * Copyright (c) 2026 
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @brief MFD-aware SPI transfer for AIROC WiFi driver
 * 
 * This file overrides whd_bus_spi_transfer() to use the MFD parent
 * instead of direct SPI access when CONFIG_MFD_CYW43439 is enabled.
 */

#include <zephyr/logging/log.h>
#include <zephyr/drivers/mfd/cyw43439.h>
#include <whd_types.h>

LOG_MODULE_DECLARE(infineon_airoc_wifi, CONFIG_WIFI_LOG_LEVEL);

/* Forward declaration from WHD */
typedef struct whd_driver *whd_driver_t;

/**
 * Override whd_bus_spi_transfer to use MFD parent
 */
whd_result_t whd_bus_spi_transfer(whd_driver_t whd_driver,
				  const uint8_t *tx, size_t tx_length,
				  uint8_t *rx, size_t rx_length,
				  uint8_t write_fill)
{
	const struct device *mfd_dev = DEVICE_DT_GET(DT_NODELABEL(cyw43439));
	int ret;

	if (!device_is_ready(mfd_dev)) {
		LOG_ERR("MFD parent not ready");
		return WHD_WLAN_SDIO_ERROR;
	}

	/* Handle NULL tx case - WHD reuses rx buffer for command */
	if (tx == NULL && tx_length > 0 && rx_length >= tx_length) {
		tx = rx;
	}

	/* Send command/data */
	if (tx_length > 0) {
		ret = mfd_cyw43439_write(mfd_dev, 0, tx, tx_length);
		if (ret < 0) {
			LOG_ERR("MFD write failed: %d", ret);
			return WHD_WLAN_SDIO_ERROR;
		}
	}

	/* Receive response */
	if (rx != NULL && rx_length > tx_length) {
		ret = mfd_cyw43439_read(mfd_dev, 0, rx + tx_length, 
					rx_length - tx_length);
		if (ret < 0) {
			LOG_ERR("MFD read failed: %d", ret);
			return WHD_WLAN_SDIO_ERROR;
		}
	}

	return WHD_SUCCESS;
}
