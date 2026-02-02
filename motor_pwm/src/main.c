/*
 * Copyright (c) 2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file Sample app to demonstrate PWM-based servomotor control
 */

#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/device.h>
#include <zephyr/drivers/pwm.h>

static const struct pwm_dt_spec servo = PWM_DT_SPEC_GET(DT_NODELABEL(servo));
static const uint32_t min_pulse = DT_PROP(DT_NODELABEL(servo), min_pulse);
static const uint32_t max_pulse = DT_PROP(DT_NODELABEL(servo), max_pulse);


int main(void)
{
	uint32_t pulse_width = PWM_USEC(500);  // nanoseconds

	int ret;

	printk("Servomotor control\n");

	if (!pwm_is_ready_dt(&servo)) {
		printk("Error: PWM device %s is not ready\n", servo.dev->name);
		return 0;
	}
	int period;
	ret = pwm_get_cycles_per_sec(servo.dev, servo.channel, (uint64_t *)&period);
	printk("ret=%d\n", ret);
	printk("PWM period: %d\n", period);
	ret = pwm_set_pulse_dt(&servo, pulse_width);
	if (ret < 0) {
		printk("Error %d: failed to set pulse width\n", ret);
		return 0;
	}

	while (1) {

		k_sleep(K_SECONDS(1));
	}
	return 0;
}
