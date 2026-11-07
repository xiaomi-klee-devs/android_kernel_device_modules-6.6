/*
 * Copyright (C) 2022-2023, Xiaomi, Inc.
 * All Rights Reserved.
 *
 * Reconstructed multi-rail power + DT parsing, verified against
 * xiaomi_spi_tee.ko strings (fp_3v3_vreg, fp_1v8_vreg, xiaomi,gpio_pwr,
 * pwr_gpio, vreg1v8, vreg3v3, mPwrGpio, mRegulator).
 */

#include "fp_driver.h"
#include <linux/pinctrl/consumer.h>
#include <linux/delay.h>

/* -------------------------------------------------------------------- */
/* fingerprint chip hardware configuration				*/
/* -------------------------------------------------------------------- */
#define DTS_NETLINK_NUM			"netlink-event"
#define DTS_IRQ_GPIO			"xiaomi,gpio_irq"
#define DTS_POWER_GPIO			"xiaomi,gpio_pwr"
#define DTS_VREG_3V3			"fp_3v3_vreg"
#define DTS_VREG_1V8			"fp_1v8_vreg"
#define DTS_POWER_TYPE			"power-type"
#define DTS_PINCTL_RESET_HIGH		"reset_high"
#define DTS_PINCTL_RESET_LOW		"reset_low"

/* power-type values */
#define FP_POWER_TYPE_REGULATOR	1
#define FP_POWER_TYPE_GPIO	2

int fp_parse_dts(struct fp_device *fp_dev)
{
	int ret;
	struct device_node *node = fp_dev->driver_device->dev.of_node;

	FUNC_ENTRY();
	if (node) {
		fp_dev->irq_gpio = of_get_named_gpio(node, DTS_IRQ_GPIO, 0);
		pr_debug("irq_gpio:%d\n", fp_dev->irq_gpio);
		if (!gpio_is_valid(fp_dev->irq_gpio)) {
			pr_debug("IRQ GPIO is invalid.\n");
			return -EPERM;
		}
		fp_dev->irq_num = gpio_to_irq(fp_dev->irq_gpio);
		of_property_read_u32(node, DTS_NETLINK_NUM, &fp_dev->netlink_num);
	} else {
		pr_debug("device node is null\n");
		return -EPERM;
	}

	if (fp_dev->driver_device) {
		fp_dev->pinctrl = pinctrl_get(&fp_dev->driver_device->dev);
		if (IS_ERR(fp_dev->pinctrl)) {
			ret = PTR_ERR(fp_dev->pinctrl);
			pr_debug("can't find fingerprint pinctrl\n");
			return ret;
		}
		fp_dev->pins_reset_high =
			pinctrl_lookup_state(fp_dev->pinctrl, DTS_PINCTL_RESET_HIGH);
		if (IS_ERR(fp_dev->pins_reset_high)) {
			ret = PTR_ERR(fp_dev->pins_reset_high);
			pr_debug("can't find pinctrl reset_high\n");
			return ret;
		}
		fp_dev->pins_reset_low =
			pinctrl_lookup_state(fp_dev->pinctrl, DTS_PINCTL_RESET_LOW);
		if (IS_ERR(fp_dev->pins_reset_low)) {
			ret = PTR_ERR(fp_dev->pins_reset_low);
			pr_debug("can't find pinctrl reset_low\n");
			return ret;
		} else {
			pinctrl_select_state(fp_dev->pinctrl, fp_dev->pins_reset_low);
		}

		fp_dev->pins_spiio_spi_mode =
			pinctrl_lookup_state(fp_dev->pinctrl, "spiio_spi_mode");
		if (IS_ERR(fp_dev->pins_spiio_spi_mode)) {
			ret = PTR_ERR(fp_dev->pins_spiio_spi_mode);
			pr_debug("%s can't find fingerprint pinctrl spiio_spi_mode\n",
				__func__);
		} else {
			pinctrl_select_state(fp_dev->pinctrl, fp_dev->pins_spiio_spi_mode);
		}

		fp_dev->pins_spiio_gpio_mode =
			pinctrl_lookup_state(fp_dev->pinctrl, "spiio_gpio_mode");
		if (IS_ERR(fp_dev->pins_spiio_gpio_mode)) {
			ret = PTR_ERR(fp_dev->pins_spiio_gpio_mode);
			pr_debug("%s can't find fingerprint spiio_gpio_mode\n",
				__func__);
		}

		fp_dev->pins_eint_default =
			pinctrl_lookup_state(fp_dev->pinctrl, "eint_default");
		if (IS_ERR(fp_dev->pins_eint_default)) {
			ret = PTR_ERR(fp_dev->pins_eint_default);
			pr_debug("%s can't find fingerprint pinctrl pins_eint_default\n",
				__func__);
			return ret;
		}

		fp_dev->pins_eint_pulldown =
			pinctrl_lookup_state(fp_dev->pinctrl, "eint_pulldown");
		if (IS_ERR(fp_dev->pins_eint_pulldown)) {
			ret = PTR_ERR(fp_dev->pins_eint_pulldown);
			pr_debug("%s can't find fingerprint pinctrl pins_eint_pulldown\n",
				__func__);
			return ret;
		} else {
			pinctrl_select_state(fp_dev->pinctrl, fp_dev->pins_eint_pulldown);
		}
		pr_debug("get pinctrl success!\n");
	} else {
		pr_debug("platform device is null\n");
		return -EPERM;
	}

	FUNC_EXIT();
	return 0;
}

/* Must run before fp_parse_dts per stock fp_probe order. */
int fp_power_config(struct fp_device *fp_dev)
{
	struct device *dev = &fp_dev->driver_device->dev;
	int ret;

	FUNC_ENTRY();

	ret = of_property_read_u32(dev->of_node, DTS_POWER_TYPE,
				   &fp_dev->power_type);
	if (ret)
		fp_dev->power_type = FP_POWER_TYPE_REGULATOR;

	if (fp_dev->power_type == FP_POWER_TYPE_REGULATOR) {
		fp_dev->vreg_3v3 = devm_regulator_get_optional(dev, DTS_VREG_3V3);
		if (IS_ERR(fp_dev->vreg_3v3)) {
			ret = PTR_ERR(fp_dev->vreg_3v3);
			if (ret == -EPROBE_DEFER)
				return ret;
			fp_dev->vreg_3v3 = NULL;
		}

		fp_dev->vreg_1v8 = devm_regulator_get_optional(dev, DTS_VREG_1V8);
		if (IS_ERR(fp_dev->vreg_1v8)) {
			ret = PTR_ERR(fp_dev->vreg_1v8);
			if (ret == -EPROBE_DEFER)
				return ret;
			fp_dev->vreg_1v8 = NULL;
		}
	}

	fp_dev->power_gpio = of_get_named_gpio(dev->of_node, DTS_POWER_GPIO, 0);
	if (!gpio_is_valid(fp_dev->power_gpio))
		fp_dev->power_gpio = -EINVAL;

	FUNC_EXIT();
	return 0;
}

int fp_set_power(struct fp_device *fp_dev, const char *name, bool enable)
{
	struct regulator *regulator = NULL;
	int ret;

	FUNC_ENTRY();

	if (!strcmp(name, "vreg3v3")) {
		regulator = fp_dev->vreg_3v3;
	} else if (!strcmp(name, "vreg1v8")) {
		regulator = fp_dev->vreg_1v8;
	} else if (!strcmp(name, "pwr_gpio")) {
		if (!gpio_is_valid(fp_dev->power_gpio))
			return -ENODEV;
		ret = gpio_direction_output(fp_dev->power_gpio, enable ? 1 : 0);
		usleep_range(1000, 1100);
		return ret;
	} else {
		pr_err("%s is unknown power name\n", name);
		return -EINVAL;
	}

	if (!regulator)
		return -ENODEV;

	if (enable) {
		ret = regulator_enable(regulator);
		if (!ret)
			usleep_range(1000, 1100);
	} else {
		ret = regulator_disable(regulator);
	}

	return ret;
}

void fp_power_on(struct fp_device *fp_dev)
{
	int status = 0;

	FUNC_ENTRY();
	if (fp_dev->device_available == 1) {
		pr_err("have already powered on\n");
	} else {
		status = fp_set_power(fp_dev, "vreg3v3", true);
		pr_debug("power on vreg3v3 ret %d\n", status);
		fp_dev->device_available = 1;
	}
}

void fp_power_off(struct fp_device *fp_dev)
{
	int status = 0;

	FUNC_ENTRY();
	if (fp_dev->device_available == 0) {
		pr_err("has already powered off\n");
	} else {
		status = fp_set_power(fp_dev, "vreg3v3", false);
		pr_debug("power off vreg3v3 ret %d\n", status);
		fp_dev->device_available = 0;
	}
}

/* delay ms after reset */
void fp_hw_reset(struct fp_device *fp_dev, u8 delay_ms)
{
	pinctrl_select_state(fp_dev->pinctrl, fp_dev->pins_reset_low);
	mdelay(5);
	pinctrl_select_state(fp_dev->pinctrl, fp_dev->pins_reset_high);
	mdelay(delay_ms);
}

void fp_hw_reset_delay(struct fp_device *fp_dev, u32 low_ms, u32 high_ms)
{
	pinctrl_select_state(fp_dev->pinctrl, fp_dev->pins_reset_low);
	if (low_ms)
		msleep(low_ms);
	pinctrl_select_state(fp_dev->pinctrl, fp_dev->pins_reset_high);
	if (high_ms)
		msleep(high_ms);
}

void fp_enable_irq(struct fp_device *fp_dev)
{
	if (1 == fp_dev->irq_enabled) {
		pr_debug("irq already enabled\n");
	} else {
		enable_irq(fp_dev->irq_num);
		fp_dev->irq_enabled = 1;
		pr_debug("enable irq!\n");
	}
}

void fp_disable_irq(struct fp_device *fp_dev)
{
	if (0 == fp_dev->irq_enabled) {
		pr_debug("irq already disabled\n");
	} else {
		disable_irq(fp_dev->irq_num);
		fp_dev->irq_enabled = 0;
		pr_debug("disable irq!\n");
	}
}

void fp_kernel_key_input(struct fp_device *fp_dev, struct fp_key *fp_key)
{
	uint32_t key_input = 0;

	if (FP_KEY_HOME == fp_key->key) {
		key_input = FP_KEY_INPUT_HOME;
	} else if (FP_KEY_POWER == fp_key->key) {
		key_input = FP_KEY_INPUT_POWER;
	} else if (FP_KEY_CAMERA == fp_key->key) {
		key_input = FP_KEY_INPUT_CAMERA;
	} else {
		key_input = fp_key->key;
	}

	pr_debug("received key event[%d], key=%d, value=%d\n",
		 key_input, fp_key->key, fp_key->value);

	if ((FP_KEY_POWER == fp_key->key || FP_KEY_CAMERA == fp_key->key)
		&& (fp_key->value == 1)) {
		input_report_key(fp_dev->input, key_input, 1);
		input_sync(fp_dev->input);
		input_report_key(fp_dev->input, key_input, 0);
		input_sync(fp_dev->input);
	}

	if (FP_KEY_HOME == fp_key->key) {
		input_report_key(fp_dev->input, key_input, fp_key->value);
		input_sync(fp_dev->input);
	}
}
