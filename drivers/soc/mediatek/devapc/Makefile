# SPDX-License-Identifier: GPL-2.0

ifeq ($(CONFIG_MTK_GCOV_KERNEL),y)
GCOV_PROFILE := y
endif

subdir-ccflags-$(CONFIG_MTK_ENG_BUILD) += -DDBG_ENABLE

obj-$(CONFIG_DEVAPC_ARCH_MULTI) += device-apc-common.o
device-apc-common-$(CONFIG_DEVAPC_ARCH_MULTI) += devapc-mtk-multi-ao.o







ifneq ($(wildcard $(srctree)/$(src)/devapc-mt6873.c),)
obj-$(CONFIG_DEVAPC_MT6873) += device-apc-mt6873.o
device-apc-mt6873-$(CONFIG_DEVAPC_MT6873) += devapc-mt6873.o
endif


ifneq ($(wildcard $(srctree)/$(src)/devapc-mt6879.c),)
obj-$(CONFIG_DEVAPC_MT6879) += device-apc-mt6879.o
device-apc-mt6879-$(CONFIG_DEVAPC_MT6879) += devapc-mt6879.o
endif

ifneq ($(wildcard $(srctree)/$(src)/devapc-mt6886.c),)
obj-$(CONFIG_DEVAPC_MT6886) += device-apc-mt6886.o
device-apc-mt6886-$(CONFIG_DEVAPC_MT6886) += devapc-mt6886.o
endif


ifneq ($(wildcard $(srctree)/$(src)/devapc-mt6895.c),)
obj-$(CONFIG_DEVAPC_MT6895) += device-apc-mt6895.o
device-apc-mt6895-$(CONFIG_DEVAPC_MT6895) += devapc-mt6895.o
endif

ifneq ($(wildcard $(srctree)/$(src)/devapc-mt6897.c),)
obj-$(CONFIG_DEVAPC_MT6897) += device-apc-mt6897.o
device-apc-mt6897-$(CONFIG_DEVAPC_MT6897) += devapc-mt6897.o
endif

ifneq ($(wildcard $(srctree)/$(src)/devapc-mt6899.c),)
obj-$(CONFIG_DEVAPC_MT6899) += device-apc-mt6899.o
device-apc-mt6899-$(CONFIG_DEVAPC_MT6899) += devapc-mt6899.o
endif

ifneq ($(wildcard $(srctree)/$(src)/devapc-mt6983.c),)
obj-$(CONFIG_DEVAPC_MT6983) += device-apc-mt6983.o
device-apc-mt6983-$(CONFIG_DEVAPC_MT6983) += devapc-mt6983.o
endif

ifneq ($(wildcard $(srctree)/$(src)/devapc-mt6985.c),)
obj-$(CONFIG_DEVAPC_MT6985) += device-apc-mt6985.o
device-apc-mt6985-$(CONFIG_DEVAPC_MT6985) += devapc-mt6985.o
endif

ifneq ($(wildcard $(srctree)/$(src)/devapc-mt6989.c),)
obj-$(CONFIG_DEVAPC_MT6989) += device-apc-mt6989.o
device-apc-mt6989-$(CONFIG_DEVAPC_MT6989) += devapc-mt6989.o
endif

