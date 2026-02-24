/*
 * Copyright (c) 2026 Beechwoods Software
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef ZEPHYR_INCLUDE_DRIVERS_MFD_CYW43439_H_
#define ZEPHYR_INCLUDE_DRIVERS_MFD_CYW43439_H_

#include <zephyr/device.h>

int mfd_cyw43439_read(const struct device *dev, uint32_t addr, uint8_t *data, size_t len);
int mfd_cyw43439_write(const struct device *dev, uint32_t addr, const uint8_t *data, size_t len);
int mfd_cyw43439_gpio_set(const struct device *dev, uint8_t pin, bool value);
int mfd_cyw43439_gpio_get(const struct device *dev, uint8_t pin, bool *value);

#endif
