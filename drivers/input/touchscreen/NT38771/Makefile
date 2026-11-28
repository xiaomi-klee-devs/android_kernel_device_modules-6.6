#
# Makefile for the Novatek NT387xx touchscreen driver.
#

ccflags-$(CONFIG_DRM_MEDIATEK_V2) += -I$(DEVICE_MODULES_PATH)/drivers/gpu/drm/mediatek/mediatek_v2/
obj-$(CONFIG_TOUCHSCREEN_NT38771) += nt38771_touch_klee.o
nt38771_touch_klee-y := \
			nt387xx.o \
			nt387xx_fw_update.o \
			nt387xx_ext_proc.o \
			nt387xx_selftest.o
