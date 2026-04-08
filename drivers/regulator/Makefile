# SPDX-License-Identifier: GPL-2.0
#
# Makefile for regulator drivers.
ccflags-y += -I$(srctree)/drivers/regulator
CFLAGS_mtk-dvfsrc-regulator.o := -I$(DEVICE_MODULES_PATH)/drivers/regulator

obj-$(CONFIG_DEVICE_MODULES_REGULATOR_MT6315) += mt6315-regulator.o
obj-$(CONFIG_DEVICE_MODULES_REGULATOR_MT6316) += mt6316-regulator.o
obj-$(CONFIG_DEVICE_MODULES_REGULATOR_MT6357) += mt6357-regulator.o
obj-$(CONFIG_DEVICE_MODULES_REGULATOR_MT6358) += mt6358-regulator.o
obj-$(CONFIG_REGULATOR_MT6362) += mt6362-regulator.o
obj-$(CONFIG_REGULATOR_MT6363) += mt6363-regulator.o
obj-$(CONFIG_REGULATOR_MT6368) += mt6368-regulator.o
obj-$(CONFIG_REGULATOR_MT6681) += mt6681-regulator.o
obj-$(CONFIG_DEVICE_MODULES_REGULATOR_MTK_DVFSRC) += mtk-dvfsrc-regulator.o
obj-$(CONFIG_DEVICE_MODULES_REGULATOR_RT4803)	+= rt4803.o
obj-$(CONFIG_REGULATOR_RT5133)  += rt5133-regulator.o
obj-$(CONFIG_DEVICE_MODULES_REGULATOR_RT6160)	+= rt6160-regulator.o
obj-$(CONFIG_REGULATOR_SGM38121)+= sgm38121.o
obj-m += dio8015.o

ccflags-$(CONFIG_REGULATOR_DEBUG) += -DDEBUG
