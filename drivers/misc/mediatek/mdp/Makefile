#
# Copyright (C) 2019 MediaTek Inc.
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License version 2 as
# published by the Free Software Foundation.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details.
#

#==========================================================================
# MDP Common Headers
#==========================================================================

ccflags-y += -I$(DEVICE_MODULES_PATH)/drivers/misc/mediatek/smi/ \
             -I$(DEVICE_MODULES_PATH)/drivers/misc/mediatek/mmdvfs/ \
             -I$(DEVICE_MODULES_PATH)/drivers/misc/mediatek/mtk-interconnect/ \
             -I$(DEVICE_MODULES_PATH)/drivers/misc/mediatek/include/ \
             -I$(DEVICE_MODULES_PATH)/drivers/misc/mediatek/mmp/ \
             -I$(DEVICE_MODULES_PATH)/drivers/iommu \
             -I$(DEVICE_MODULES_PATH)/drivers/misc/mediatek/iommu/ \
             -I$(DEVICE_MODULES_PATH)/drivers/iommu/arm/arm-smmu-v3 \
             -I$(DEVICE_MODULES_PATH)/drivers/gpu/drm/mediatek/dpc

ccflags-y += -I$(DEVICE_MODULES_PATH)/drivers/misc/mediatek/cmdq/mailbox/
ccflags-y += -I$(DEVICE_MODULES_PATH)/drivers/dma-buf/heaps

ifneq ($(filter y m, $(CONFIG_MTK_PMQOS)),)
ccflags-y += -I$(DEVICE_MODULES_PATH)/drivers/soc/mediatek/
ccflags-y += -I$(DEVICE_MODULES_PATH)/drivers/misc/mediatek/helio-dvfsrc/
endif
ifneq ($(filter y m, $(CONFIG_MTK_MME_SUPPORT)),)
ccflags-y += -I$(DEVICE_MODULES_PATH)/drivers/misc/mediatek/mme/
endif

ifneq ($(filter mt6771,$(CONFIG_MTK_LEGACY_PLATFORM)),)
ccflags-y += -DMT8788_MDP
endif

ifeq ($(strip $(CONFIG_MTK_PQ_COLOR_MODE)),"MDP")
ccflags-y += -DCMDQ_MDP_COLOR
endif

# Enable MET
ccflags-y += -DMET_USER_EVENT_SUPPORT

# Enable AEE
ccflags-y += -DCMDQ_AEE_READY

# iommu callback support
ifeq ($(strip $(CONFIG_DEVICE_MODULES_MTK_IOMMU)),y)
#ccflags-y += -DMDP_IOMMU_DEBUG
endif

# dump mdp command
#ccflags-y += -DMDP_COMMAND_SIMULATE

#==========================================================================
# MDP Secure path for SVP/HDCP/SecCam
#==========================================================================

ifneq ($(filter y m, $(or $(CONFIG_MTK_TEE_GP_SUPPORT), $(CONFIG_HAVE_MTK_ENABLE_GENIEZONE))),)
ifneq ($(filter y m, $(CONFIG_MTK_MDP_MTEE_SUPPORT)),)
ccflags-y += -DCMDQ_SECURE_PATH_SUPPORT
ccflags-y += -I$(DEVICE_MODULES_PATH)/drivers/dma-buf/heaps
endif # !CONFIG_MTK_MDP_MTEE_SUPPORT
endif # !SECURE_PATH_SUPPORET

# platform dependent definition
ifneq ($(strip $(CONFIG_MTK_GMO_RAM_OPTIMIZE)),y)
ccflags-y += -DCMDQ_LARGE_MAX_FIRSTERROR_BUFFER
endif # !CONFIG_MTK_GMO_RAM_OPTIMIZE

#==========================================================================
# MDP driver module
#==========================================================================

obj-$(CONFIG_MTK_MDP_DUMMY) += mdp_drv_dummy.o
ifeq ($(CONFIG_MTK_MDP), y)
else
ifneq ($(wildcard $(srctree)/$(src)/mdp_mt6886.c),)
obj-$(CONFIG_MTK_MDP) += mdp_drv_mt6886.o
endif
ifneq ($(wildcard $(srctree)/$(src)/mdp_mt6897.c),)
obj-$(CONFIG_MTK_MDP) += mdp_drv_mt6897.o
endif
ifneq ($(wildcard $(srctree)/$(src)/mdp_mt6983.c),)
obj-$(CONFIG_MTK_MDP) += mdp_drv_mt6983.o
endif
ifneq ($(wildcard $(srctree)/$(src)/mdp_mt6985.c),)
obj-$(CONFIG_MTK_MDP) += mdp_drv_mt6985.o
endif
ifneq ($(wildcard $(srctree)/$(src)/mdp_mt6989.c),)
obj-$(CONFIG_MTK_MDP) += mdp_drv_mt6989.o
endif
ifneq ($(wildcard $(srctree)/$(src)/mdp_mt6771.c),)
obj-$(CONFIG_MTK_MDP) += mdp_drv_mt6771.o
endif
endif

mdp_drv_mt6771-objs := mdp_mt6771.o\
	mdp_driver.o \
	mdp_ioctl_ex.o \
	mdp_common.o \
	cmdq_record.o \
	cmdq_virtual.o \
	cmdq_device.o \
	mdp_event_common.o \
	cmdq_subsys_common.o \
	cmdq_helper_ext.o \
	cmdq_mmp.o \
	mdp_fence.o \
	mdp_dpc.o


mdp_drv_mt6886-objs := mdp_mt6886.o\
	mdp_driver.o \
	mdp_ioctl_ex.o \
	mdp_common.o \
	cmdq_record.o \
	cmdq_virtual.o \
	cmdq_device.o \
	mdp_event_common.o \
	cmdq_subsys_common.o \
	cmdq_helper_ext.o \
	cmdq_mmp.o \
	mdp_fence.o \
	mdp_dpc.o


mdp_drv_mt6897-objs := mdp_mt6897.o\
	mdp_driver.o \
	mdp_ioctl_ex.o \
	mdp_common.o \
	cmdq_record.o \
	cmdq_virtual.o \
	cmdq_device.o \
	mdp_event_common.o \
	cmdq_subsys_common.o \
	cmdq_helper_ext.o \
	cmdq_mmp.o \
	mdp_fence.o \
	mdp_dpc.o

mdp_drv_mt6983-objs := mdp_mt6983.o\
	mdp_driver.o \
	mdp_ioctl_ex.o \
	mdp_common.o \
	cmdq_record.o \
	cmdq_virtual.o \
	cmdq_device.o \
	mdp_event_common.o \
	cmdq_subsys_common.o \
	cmdq_helper_ext.o \
	cmdq_mmp.o \
	mdp_fence.o \
	mdp_dpc.o

mdp_drv_mt6985-objs := mdp_mt6985.o\
	mdp_driver.o \
	mdp_ioctl_ex.o \
	mdp_common.o \
	cmdq_record.o \
	cmdq_virtual.o \
	cmdq_device.o \
	mdp_event_common.o \
	cmdq_subsys_common.o \
	cmdq_helper_ext.o \
	cmdq_mmp.o \
	mdp_fence.o \
	mdp_dpc.o

mdp_drv_mt6989-objs := mdp_mt6989.o\
	mdp_driver.o \
	mdp_ioctl_ex.o \
	mdp_common.o \
	cmdq_record.o \
	cmdq_virtual.o \
	cmdq_device.o \
	mdp_event_common.o \
	cmdq_subsys_common.o \
	cmdq_helper_ext.o \
	cmdq_mmp.o \
	mdp_fence.o \
	mdp_dpc.o









#obj-y += cmdq_prof.o
#obj-y += cmdq_test.o

#ifeq ($(strip $(CONFIG_MMPROFILE)),y)
#obj-y += cmdq_mmp.o
#endif

