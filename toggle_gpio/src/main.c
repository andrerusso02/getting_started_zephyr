/*
 * Copyright (c) 2016 Intel Corporation
 * Copyright (c) 2020 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file Sample app to demonstrate PWM.
 */

#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>

/* Get the pin from device tree */
#define MY_OUT_NODE DT_NODELABEL(my_out)

static const struct gpio_dt_spec my_out = GPIO_DT_SPEC_GET(MY_OUT_NODE, gpios);

int main(void)
{
    int ret;

    if (!device_is_ready(my_out.port)) {
        printk("Error: GPIO device not ready\n");
        return;
    }

    /* Configure the pin as output */
    ret = gpio_pin_configure_dt(&my_out, GPIO_OUTPUT_INACTIVE);
    if (ret < 0) {
        printk("Error configuring pin\n");
        return;
    }

    while (1) {
        gpio_pin_set_dt(&my_out, 0);
        k_sleep(K_USEC(400));
        gpio_pin_set_dt(&my_out, 1);
        k_sleep(K_USEC(1000));
    }
	return 0;
}
