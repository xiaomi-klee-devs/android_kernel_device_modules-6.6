# SPDX-License-Identifier: GPL-2.0
#
# Makefile for RTC class/drivers.
#

ccflags-$(CONFIG_RTC_DEBUG)	:= -DDEBUG

# common include path
ccflags-y += -I$(DEVICE_MODULES_PATH)/drivers/misc/mediatek/include/

obj-$(CONFIG_RTC_DRV_MT6685)	+= rtc-mt6685.o
