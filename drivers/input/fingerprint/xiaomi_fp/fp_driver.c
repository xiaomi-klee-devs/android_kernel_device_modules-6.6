/*
 * Copyright (C) 2022-2023, Xiaomi, Inc.
 * All Rights Reserved.
 *
 * Reconstructed against the proprietary xiaomi_spi_tee.ko. Structure
 * (global nl_sk/pid, device_list, two cdevs, multi-rail power, mi_disp
 * notifier) and ioctl ABI verified from the ELF symbols/disassembly.
 */

#include "fp_driver.h"
#include <linux/pinctrl/consumer.h>
#include <linux/delay.h>
#include <linux/rtc.h>
#include <linux/time.h>
#include <linux/timekeeping.h>

#define WAKELOCK_HOLD_TIME 2000	/* in ms */
#define FP_UNLOCK_REJECTION_TIMEOUT (WAKELOCK_HOLD_TIME - 500) /*ms*/

extern void spi_enable_fingerprint_clk(void);
extern void spi_disable_fingerprint_clk(void);

static atomic_t clk_ref = ATOMIC_INIT(0);

static void fp_spi_clk_enable(void)
{
	if (atomic_read(&clk_ref) == 0) {
		pr_debug("enable spi clk\n");
		spi_enable_fingerprint_clk();
		atomic_inc(&clk_ref);
	}
}

static void fp_spi_clk_disable(void)
{
	if (atomic_read(&clk_ref) == 1) {
		atomic_dec(&clk_ref);
		pr_debug("disable spi clk\n");
		spi_disable_fingerprint_clk();
	}
}

/*************************************************************/
static LIST_HEAD(device_list);
static DEFINE_MUTEX(device_list_lock);
static struct wakeup_source *fp_wakesrc = NULL;
struct work_struct fp_display_work;
static struct fp_device fp;

static struct fp_key_map maps[] = {
	{EV_KEY, FP_KEY_INPUT_HOME},
	{EV_KEY, FP_KEY_INPUT_MENU},
	{EV_KEY, FP_KEY_INPUT_BACK},
	{EV_KEY, FP_KEY_INPUT_POWER},
};

/* -------------------------------------------------------------------- */
/* display (mi_disp) notifier                                           */
/* -------------------------------------------------------------------- */
static void notification_work(struct work_struct *work)
{
	FUNC_ENTRY();
	/* screen-on/off side effects handled by netlink below */
	FUNC_EXIT();
}

static int fp_fb_notifier_callback(struct notifier_block *self,
				   unsigned long event, void *data)
{
	struct fp_device *fp_dev = container_of(self, struct fp_device,
						notifier);
	struct mi_disp_notifier *evdata = data;
	int blank;

	FUNC_ENTRY();

	if (!evdata || evdata->disp_id != MI_DISPLAY_PRIMARY || !evdata->data)
		return NOTIFY_DONE;

	blank = *(int *)evdata->data;

	if (event == MI_DISP_DPMS_EARLY_EVENT &&
	    blank == MI_DISP_DPMS_POWERDOWN) {
		if (fp_dev->device_available == 1) {
			fp_dev->fb_black = 1;
			fp_dev->wait_finger_down = true;
			if (fp_dev->netlink_enabled)
				fp_netlink_send(fp_dev, FP_NETLINK_SCREEN_OFF);
		}
	} else if (event == MI_DISP_DPMS_EVENT && blank == MI_DISP_DPMS_ON) {
		if (fp_dev->device_available == 1) {
			fp_dev->fb_black = 0;
			if (fp_dev->netlink_enabled)
				fp_netlink_send(fp_dev, FP_NETLINK_SCREEN_ON);
		}
	}

	FUNC_EXIT();
	return NOTIFY_OK;
}

/* -------------------------------------------------------------------- */
/* fingerdown sysfs attribute                                           */
/* -------------------------------------------------------------------- */
static ssize_t get_fingerdown_event(struct device *dev,
				    struct device_attribute *attr, char *buf)
{
	return scnprintf(buf, PAGE_SIZE, "%d\n", fp.wait_finger_down);
}

static ssize_t set_fingerdown_event(struct device *dev,
				    struct device_attribute *attr,
				    const char *buf, size_t count)
{
	unsigned int val;

	if (kstrtouint(buf, 10, &val))
		return -EINVAL;
	val = !!val;
	pr_debug("set fingerdown %u\n", val);
	fp.wait_finger_down = val;
	return count;
}

static DEVICE_ATTR(fingerdown, 0664, get_fingerdown_event,
		   set_fingerdown_event);

static struct attribute *fp_attrs[] = {
	&dev_attr_fingerdown.attr,
	NULL,
};

static struct attribute_group fp_attr_group = {
	.attrs = fp_attrs,
};

/* -------------------------------------------------------------------- */
/* interrupt handler                                                    */
/* -------------------------------------------------------------------- */
static irqreturn_t fp_irq(int irq, void *handle)
{
	struct fp_device *fp_dev = (struct fp_device *)handle;

	FUNC_ENTRY();
	__pm_wakeup_event(fp_wakesrc, WAKELOCK_HOLD_TIME);
	fp_netlink_send(fp_dev, FP_NETLINK_IRQ);
	FUNC_EXIT();
	return IRQ_HANDLED;
}

/* -------------------------------------------------------------------- */
/* input device                                                         */
/* -------------------------------------------------------------------- */
int fp_create_input_device(struct fp_device *fp_dev)
{
	int i;

	FUNC_ENTRY();

	if (fp_dev->input)
		return 0;

	fp_dev->input = input_allocate_device();
	if (!fp_dev->input)
		return -ENOMEM;

	for (i = 0; i < ARRAY_SIZE(maps); i++)
		input_set_capability(fp_dev->input, maps[i].type, maps[i].code);

	fp_dev->input->name = FP_INPUT_NAME;
	fp_dev->input->dev.parent = &fp_dev->driver_device->dev;

	if (input_register_device(fp_dev->input)) {
		input_free_device(fp_dev->input);
		fp_dev->input = NULL;
		return -ENODEV;
	}

	if (sysfs_create_group(&fp_dev->input->dev.kobj, &fp_attr_group))
		pr_debug("Failed to create fingerdown sysfs group\n");

	FUNC_EXIT();
	return 0;
}

void fp_free_input_device(struct fp_device *fp_dev)
{
	FUNC_ENTRY();
	if (fp_dev->input) {
		sysfs_remove_group(&fp_dev->input->dev.kobj, &fp_attr_group);
		input_unregister_device(fp_dev->input);
		fp_dev->input = NULL;
	}
	FUNC_EXIT();
}

/* -------------------------------------------------------------------- */
/* local time printk                                                    */
/* -------------------------------------------------------------------- */
void fp_local_time_printk(void)
{
	struct timespec64 ts;
	struct rtc_time tm;

	ktime_get_real_ts64(&ts);
	rtc_time64_to_tm(ts.tv_sec - (sys_tz.tz_minuteswest * 60), &tm);
	pr_info("%d-%02d-%02d %02d:%02d:%02d\n",
		tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday,
		tm.tm_hour, tm.tm_min, tm.tm_sec);
}

/* -------------------------------------------------------------------- */
/* file operation function                                              */
/* -------------------------------------------------------------------- */

static long fp_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
	struct fp_device *fp_dev = &fp;
	struct fp_key fp_key;
	struct fp_ioc_chip_info info;
	u32 reset_time[2];
	char power_name[FP_POWER_NAME_LEN];
	int retval = 0;
	u8 buf = 0;
	u8 netlink_route = fp_dev->netlink_num;

	FUNC_ENTRY();
	if (_IOC_TYPE(cmd) != FP_IOC_MAGIC)
		return -EINVAL;

	if (_IOC_DIR(cmd) & _IOC_READ)
		retval = !access_ok((void __user *)arg, _IOC_SIZE(cmd));
	if (retval == 0 && _IOC_DIR(cmd) & _IOC_WRITE)
		retval = !access_ok((void __user *)arg, _IOC_SIZE(cmd));
	if (retval)
		return -EINVAL;

	switch (cmd) {
	case FP_IOC_INIT:
		pr_debug("FP_IOC_INIT ======\n");
		if (fp_dev->netlink_num <= 0) {
			pr_err("netlink init fail, check dts config.\n");
			retval = -EFAULT;
			break;
		}
		if (fp_dev->netlink_enabled == 0) {
			retval = fp_netlink_init(fp_dev);
			if (retval != 0)
				break;
		}
		if (copy_to_user((void __user *)arg, &netlink_route, sizeof(u8))) {
			retval = -EFAULT;
			break;
		}
		fp_dev->netlink_enabled = 1;
		break;

	case FP_IOC_EXIT:
		pr_debug("FP_IOC_EXIT ======\n");
		fp_disable_irq(fp_dev);
		if (fp_dev->netlink_enabled)
			fp_netlink_destroy(fp_dev);
		fp_dev->netlink_enabled = 0;
		fp_dev->device_available = 0;
		break;

	case FP_IOC_ENABLE_IRQ:
		pr_debug("FP_IOC_ENABLE_IRQ ======\n");
		fp_enable_irq(fp_dev);
		break;

	case FP_IOC_DISABLE_IRQ:
		pr_debug("FP_IOC_DISABLE_IRQ ======\n");
		fp_disable_irq(fp_dev);
		break;

	case FP_IOC_RESET:
		pr_debug("FP_IOC_RESET ======\n");
		fp_hw_reset(fp_dev, 60);
		break;

	case FP_IOC_RESET_OUT_LOW:
		pr_debug("FP_IOC_RESET_OUT_LOW ======\n");
		if (fp_dev->pins_reset_low)
			pinctrl_select_state(fp_dev->pinctrl,
					     fp_dev->pins_reset_low);
		break;

	case FP_IOC_RESET_TIME_MS:
		pr_debug("FP_IOC_RESET_TIME_MS ======\n");
		if (copy_from_user(reset_time, (u32 __user *)arg,
				   sizeof(reset_time))) {
			retval = -EFAULT;
			break;
		}
		if (!reset_time[0] || !reset_time[1]) {
			pr_debug("low_time or high_time is 0.\n");
			retval = -EINVAL;
			break;
		}
		fp_hw_reset_delay(fp_dev, reset_time[0], reset_time[1]);
		break;

	case FP_IOC_ENABLE_POWER:
		pr_debug("FP_IOC_ENABLE_POWER ======\n");
		if (copy_from_user(power_name, (void __user *)arg,
				   sizeof(power_name))) {
			retval = -EFAULT;
			break;
		}
		power_name[FP_POWER_NAME_LEN - 1] = '\0';
		retval = fp_set_power(fp_dev, power_name, true);
		break;

	case FP_IOC_DISABLE_POWER:
		pr_debug("FP_IOC_DISABLE_POWER ======\n");
		if (copy_from_user(power_name, (void __user *)arg,
				   sizeof(power_name))) {
			retval = -EFAULT;
			break;
		}
		power_name[FP_POWER_NAME_LEN - 1] = '\0';
		retval = fp_set_power(fp_dev, power_name, false);
		break;

	case FP_IOC_ENABLE_SPI_CLK:
		pr_debug("FP_IOC_ENABLE_SPI_CLK ======\n");
		fp_spi_clk_enable();
		break;

	case FP_IOC_DISABLE_SPI_CLK:
		pr_debug("FP_IOC_DISABLE_SPI_CLK ======\n");
		fp_spi_clk_disable();
		break;

	case FP_IOC_INPUT_KEY_EVENT:
		pr_debug("FP_IOC_INPUT_KEY_EVENT ======\n");
		if (copy_from_user(&fp_key, (struct fp_key __user *)arg,
				   sizeof(struct fp_key))) {
			retval = -EFAULT;
			break;
		}
		fp_kernel_key_input(fp_dev, &fp_key);
		break;

	case FP_IOC_ENTER_SLEEP_MODE:
		pr_debug("FP_IOC_ENTER_SLEEP_MODE ======\n");
		break;

	case FP_IOC_GET_FW_INFO:
		pr_debug("FP_IOC_GET_FW_INFO ======\n");
		pr_debug("firmware info 0x%x\n", buf);
		if (copy_to_user((void __user *)arg, &buf, sizeof(u8))) {
			retval = -EFAULT;
		}
		break;

	case FP_IOC_REMOVE:
		pr_debug("FP_IOC_REMOVE ======\n");
		break;

	case FP_IOC_CHIP_INFO:
		pr_debug("FP_IOC_CHIP_INFO ======\n");
		if (copy_from_user(&info, (struct fp_ioc_chip_info __user *)arg,
				   sizeof(struct fp_ioc_chip_info))) {
			retval = -EFAULT;
			break;
		}
		pr_debug("vendor_id 0x%x mode 0x%x operation 0x%x\n",
			 info.vendor_id, info.mode, info.operation);
		break;

	case FP_IOC_REQUEST_RESOURCE:
		pr_debug("FP_IOC_REQUEST_RESOURCE ======\n");
		if (fp_dev->irq_requested) {
			retval = 0;
			break;
		}
		gpio_direction_input(fp_dev->irq_gpio);
		retval = request_threaded_irq(fp_dev->irq_num, NULL, fp_irq,
					      IRQF_TRIGGER_RISING |
					      IRQF_ONESHOT, "xiaomi_fp_irq",
					      fp_dev);
		if (!retval) {
			fp_dev->irq_requested = 1;
			fp_dev->irq_enabled = 1;
			fp_disable_irq(fp_dev);
		}
		break;

	case FP_IOC_RELEASE_RESOURCE:
		pr_debug("FP_IOC_RELEASE_RESOURCE ======\n");
		if (fp_dev->irq_requested) {
			fp_disable_irq(fp_dev);
			free_irq(fp_dev->irq_num, fp_dev);
			fp_dev->irq_requested = 0;
		}
		break;

	case FP_IOC_DEV_INFO:
		pr_debug("FP_IOC_DEV_INFO ======\n");
		if (copy_to_user((void __user *)arg, &buf, sizeof(u8))) {
			pr_debug("FP_IOC_DEV_INFO failed!\n");
			retval = -EFAULT;
		}
		break;

	default:
		pr_debug("fp doesn't support this command(%x)\n", cmd);
		break;
	}

	return retval;
}

#ifdef CONFIG_COMPAT
static long fp_compat_ioctl(struct file *filp, unsigned int cmd,
			    unsigned long arg)
{
	return filp->f_op->unlocked_ioctl(filp, cmd, arg);
}
#endif

/* -------------------------------------------------------------------- */
/* device function							*/
/* -------------------------------------------------------------------- */
static int fp_open(struct inode *inode, struct file *filp)
{
	struct fp_device *fp_dev = NULL;
	int status = -ENXIO;

	FUNC_ENTRY();
	mutex_lock(&device_list_lock);
	list_for_each_entry(fp_dev, &device_list, device_entry) {
		if (fp_dev->devt == inode->i_rdev) {
			pr_debug("Found\n");
			status = 0;
			break;
		}
	}
	mutex_unlock(&device_list_lock);

	if (status == 0) {
		filp->private_data = fp_dev;
		nonseekable_open(inode, filp);
	} else {
		pr_debug("No device for minor %d\n", iminor(inode));
	}
	FUNC_EXIT();
	return status;
}

static int fp_release(struct inode *inode, struct file *filp)
{
	struct fp_device *fp_dev = filp->private_data;
	int status = 0;

	FUNC_ENTRY();
	if (fp_dev && fp_dev->irq_requested) {
		fp_disable_irq(fp_dev);
		free_irq(fp_dev->irq_num, fp_dev);
		fp_dev->irq_requested = 0;
	}
	fp_spi_clk_disable();
	FUNC_EXIT();
	return status;
}

static unsigned int fp_poll(struct file *filp, poll_table *wait)
{
	struct fp_device *fp_dev = &fp;
	unsigned int mask = 0;

	FUNC_ENTRY();
	poll_wait(filp, &fp_dev->wait_queue, wait);
	if (fp_dev->poll_have_data)
		mask = POLLERR | POLLPRI;
	return mask;
}

static ssize_t fp_read(struct file *filp, char __user *buf, size_t size,
		       loff_t *ppos)
{
	int ret = 0;
	struct fp_device *fp_dev = &fp;
	char value = 0;

	FUNC_ENTRY();
	value = (char)gpio_get_value(fp_dev->irq_gpio) + '0';
	if (copy_to_user(buf, &value, 1)) {
		ret = -EFAULT;
		pr_debug("fp_read copy_to_user error\n");
	} else {
		ret = 1;
		fp_dev->poll_have_data = 0;
	}
	return ret;
}

static const struct file_operations fp_fops = {
	.owner = THIS_MODULE,
	.open = fp_open,
	.unlocked_ioctl = fp_ioctl,
#ifdef CONFIG_COMPAT
	.compat_ioctl = fp_compat_ioctl,
#endif
	.release = fp_release,
	.poll = fp_poll,
	.read = fp_read,
};

static int fp_probe(struct platform_device *driver_device)
{
	struct fp_device *fp_dev = &fp;
	int status = -EINVAL, ret = 0;

	FUNC_ENTRY();

	INIT_LIST_HEAD(&fp_dev->device_entry);

	fp_dev->irq_gpio = -EINVAL;
	fp_dev->device_available = 0;
	fp_dev->fb_black = 0;
	fp_dev->wait_finger_down = false;
	fp_dev->netlink_enabled = 0;
	INIT_WORK(&fp_display_work, notification_work);

	pr_debug("Setting fp device configuration==========\n");
	fp_dev->driver_device = driver_device;

	status = fp_power_config(fp_dev);
	if (status)
		goto err_power;

	status = fp_parse_dts(fp_dev);
	if (status)
		goto err_dts;

	fp_power_on(fp_dev);

	if (!IS_ERR(fp_dev->pins_eint_default))
		pinctrl_select_state(fp_dev->pinctrl, fp_dev->pins_eint_default);
	if (IS_ERR(fp_dev->pins_spiio_spi_mode)) {
		ret = PTR_ERR(fp_dev->pins_spiio_spi_mode);
		pr_debug("fingerprint pinctrl spiio_spi_mode NULL\n");
		status = ret;
		goto err_pinctrl;
	}
	pinctrl_select_state(fp_dev->pinctrl, fp_dev->pins_spiio_spi_mode);

	/* create class */
	fp_dev->class = class_create(FP_CLASS_NAME);
	if (IS_ERR(fp_dev->class)) {
		pr_debug("Failed to create class.\n");
		status = -ENODEV;
		goto err_class;
	}

	/* two minors: main (xiaomi-fp) + id (mifp_id) */
	status = alloc_chrdev_region(&fp_dev->devt, 0, 2, FP_DEV_NAME);
	if (status < 0) {
		pr_debug("Failed to alloc devt.\n");
		goto err_devno;
	}

	/* main cdev */
	cdev_init(&fp_dev->cdev, &fp_fops);
	fp_dev->cdev.owner = THIS_MODULE;
	status = cdev_add(&fp_dev->cdev, fp_dev->devt, 1);
	if (status) {
		pr_debug("Failed to add cdev.\n");
		goto err_cdev;
	}

	fp_dev->device = device_create(fp_dev->class, &driver_device->dev,
				       fp_dev->devt, fp_dev, FP_DEV_NAME);
	if (IS_ERR(fp_dev->device)) {
		pr_debug("Failed to create device.\n");
		status = -ENODEV;
		goto err_device;
	}

	/* id cdev (mifp_id) */
	cdev_init(&fp_dev->id_cdev, &fp_fops);
	fp_dev->id_cdev.owner = THIS_MODULE;
	status = cdev_add(&fp_dev->id_cdev, fp_dev->devt + 1, 1);
	if (status) {
		pr_debug("Failed to add fpid cdev.\n");
		goto err_id_cdev;
	}

	fp_dev->id_device = device_create(fp_dev->class, &driver_device->dev,
					  fp_dev->devt + 1, fp_dev, FP_ID_NAME);
	if (IS_ERR(fp_dev->id_device)) {
		pr_debug("fpid device create failed %d\n",
			 (int)PTR_ERR(fp_dev->id_device));
		status = -ENODEV;
		goto err_id_device;
	}

	mutex_lock(&device_list_lock);
	list_add(&fp_dev->device_entry, &device_list);
	mutex_unlock(&device_list_lock);

	status = fp_create_input_device(fp_dev);
	if (status)
		goto err_input;

	fp_dev->notifier.notifier_call = fp_fb_notifier_callback;
	mi_disp_register_client(&fp_dev->notifier);

	init_waitqueue_head(&fp_dev->wait_queue);

	/* netlink interface init */
	status = fp_netlink_init(fp_dev);
	if (status == -1)
		goto err_netlink;
	fp_dev->netlink_enabled = 1;

	fp_wakesrc = wakeup_source_register(&fp_dev->driver_device->dev,
					    "fp_wakesrc");
	irq_set_irq_wake(fp_dev->irq_num, 1);

	pr_debug("fp probe success\n");
	FUNC_EXIT();
	return 0;

err_netlink:
	mi_disp_unregister_client(&fp_dev->notifier);
	fp_free_input_device(fp_dev);
err_input:
	mutex_lock(&device_list_lock);
	list_del(&fp_dev->device_entry);
	mutex_unlock(&device_list_lock);
	device_destroy(fp_dev->class, fp_dev->devt + 1);
err_id_device:
	cdev_del(&fp_dev->id_cdev);
err_id_cdev:
	device_destroy(fp_dev->class, fp_dev->devt);
err_device:
	cdev_del(&fp_dev->cdev);
err_cdev:
	unregister_chrdev_region(fp_dev->devt, 2);
err_devno:
	class_destroy(fp_dev->class);
err_class:
err_pinctrl:
	fp_power_off(fp_dev);
err_dts:
err_power:
	fp_dev->driver_device = NULL;
	fp_dev->device_available = 0;
	pr_debug("fp probe fail\n");
	FUNC_EXIT();
	return status;
}

static int fp_remove(struct platform_device *driver_device)
{
	struct fp_device *fp_dev = &fp;

	FUNC_ENTRY();

	if (fp_wakesrc) {
		wakeup_source_unregister(fp_wakesrc);
		fp_wakesrc = NULL;
	}

	if (fp_dev->irq_requested) {
		free_irq(fp_dev->irq_num, fp_dev);
		fp_dev->irq_requested = 0;
		fp_dev->irq_enabled = 0;
	}

	mi_disp_unregister_client(&fp_dev->notifier);

	fp_netlink_destroy(fp_dev);
	fp_dev->netlink_enabled = 0;

	fp_free_input_device(fp_dev);

	mutex_lock(&device_list_lock);
	list_del(&fp_dev->device_entry);
	mutex_unlock(&device_list_lock);

	device_destroy(fp_dev->class, fp_dev->devt + 1);
	cdev_del(&fp_dev->id_cdev);
	device_destroy(fp_dev->class, fp_dev->devt);
	cdev_del(&fp_dev->cdev);
	unregister_chrdev_region(fp_dev->devt, 2);
	class_destroy(fp_dev->class);

	if (!IS_ERR(fp_dev->pins_spiio_gpio_mode))
		pinctrl_select_state(fp_dev->pinctrl, fp_dev->pins_spiio_gpio_mode);
	if (!IS_ERR(fp_dev->pins_reset_low))
		pinctrl_select_state(fp_dev->pinctrl, fp_dev->pins_reset_low);
	if (!IS_ERR(fp_dev->pins_eint_default))
		pinctrl_select_state(fp_dev->pinctrl, fp_dev->pins_eint_default);

	fp_power_off(fp_dev);
	fp_dev->driver_device = NULL;
	FUNC_EXIT();
	return 0;
}

static const struct of_device_id fp_of_match[] = {
	{.compatible = DRIVER_COMPATIBLE,},
	{},
};
MODULE_DEVICE_TABLE(of, fp_of_match);

static struct platform_driver fp_platform_driver = {
	.driver = {
		.name = FP_DEV_NAME,
		.owner = THIS_MODULE,
		.of_match_table = fp_of_match,
	},
	.probe = fp_probe,
	.remove = fp_remove,
};

static int __init fp_init(void)
{
	int status = 0;

	FUNC_ENTRY();
	status = platform_driver_register(&fp_platform_driver);
	if (status < 0) {
		pr_debug("Failed to register fp driver.\n");
		return -EINVAL;
	}
	FUNC_EXIT();
	return status;
}

module_init(fp_init);

static void __exit fp_exit(void)
{
	FUNC_ENTRY();
	platform_driver_unregister(&fp_platform_driver);
	FUNC_EXIT();
}

module_exit(fp_exit);

MODULE_AUTHOR("xiaomi");
MODULE_DESCRIPTION("Xiaomi Fingerprint driver");
MODULE_LICENSE("GPL");
MODULE_ALIAS("xiaomi-fp");
