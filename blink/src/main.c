/*
 * Copyright (c) 2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>

/* 1000 msec = 1 sec */
#define SLEEP_TIME_MS   100


static const struct gpio_dt_spec led_1 = GPIO_DT_SPEC_GET(DT_ALIAS(my_led_1), gpios);
static const struct gpio_dt_spec led_2 = GPIO_DT_SPEC_GET(DT_ALIAS(my_led_2), gpios);
static const struct gpio_dt_spec button = GPIO_DT_SPEC_GET(DT_ALIAS(my_button), gpios);

int main(void)
{
	int ret;
	bool led_state = true;

	if (!gpio_is_ready_dt(&led_1) || !gpio_is_ready_dt(&led_2) ||
	    !gpio_is_ready_dt(&button)) {
		printf("Error: GPIO device not ready\n");
		return 0;
	}

	ret = gpio_pin_configure_dt(&led_1, GPIO_OUTPUT_ACTIVE);
	if (ret != 0) {
		printf("Error %d: failed to configure %s pin %d\n",
		       ret, led_1.port->name, led_1.pin);
		return 0;
	}
	ret = gpio_pin_configure_dt(&led_2, GPIO_OUTPUT_ACTIVE);
	if (ret != 0) {
		printf("Error %d: failed to configure %s pin %d\n",
		       ret, led_2.port->name, led_2.pin);
		return 0;
	}
	ret = gpio_pin_configure_dt(&button, GPIO_INPUT);
	if (ret != 0) {
		printf("Error %d: failed to configure %s pin %d\n",
		       ret, button.port->name, button.pin);
		return 0;
	}


	while (1) {
		int val = gpio_pin_get_dt(&button);

		if (val >= 0) {
			gpio_pin_set_dt(&led_1, val);
			gpio_pin_set_dt(&led_2, !val);
		}
		k_msleep(10);
		printf("Hello World! %s\n", val ? "ON" : "OFF");

	}
	return 0;
}
