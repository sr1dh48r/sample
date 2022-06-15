/*
 * Copyright (c) 2012-2014 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr.h>
#include <sys/printk.h>
#include "common/defs.h"

void main(void)
{
	int i = 0;
	int key_mgmt = 0;
	printk("Hello World! %s\n", CONFIG_BOARD);

	sleep(10);

	zephyr_supp_add_network();
	zephyr_supp_set_ssid(0, "\"NORDIC-GUEST\"");
	zephyr_supp_set_psk(0, "\"BillionBluetooth\"");
	key_mgmt = WPA_KEY_MGMT_NONE|WPA_KEY_MGMT_PSK|WPA_KEY_MGMT_WPA_NONE;
	zephyr_supp_set_key_mgmt(0, key_mgmt);
	zephyr_supp_enable_network(0);	
}
