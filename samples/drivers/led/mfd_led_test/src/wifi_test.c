/*
 * MFD WiFi Test
 * Copyright (c) 2026
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/net/wifi_mgmt.h>
#include <zephyr/net/net_if.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(mfd_wifi_test, LOG_LEVEL_DBG);

int main(void)
{
	struct net_if *iface;

	LOG_INF("=== MFD WiFi Test ===");

	/* Get WiFi interface */
	iface = net_if_get_first_wifi();
	if (!iface) {
		LOG_ERR("No WiFi interface found");
		return -1;
	}

	LOG_INF("WiFi interface found: %p", iface);
	LOG_INF("WiFi test complete - interface detected");

	return 0;
}
