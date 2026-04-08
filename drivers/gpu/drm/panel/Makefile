# SPDX-License-Identifier: GPL-2.0
ccflags-y += -I$(DEVICE_MODULES_PATH)/drivers/misc/mediatek/include/mt-plat

ifneq ($(filter y m, $(CONFIG_MTK_MME_SUPPORT)),)
ccflags-y += -I$(DEVICE_MODULES_PATH)/drivers/misc/mediatek/mme
endif

obj-$(CONFIG_DRM_PANEL_NT37801_MSYNC2_RTE) += panel-nt37801-msync2-rte.o
obj-$(CONFIG_DRM_PANEL_ALPHA_SAMSUNG_S6E3HAE_CMD_120HZ) += panel-alpha-samsung-s6e3hae-fhd-dphy-cmd-120hz-r4831a.o
obj-$(CONFIG_DRM_PANEL_P10_36_02_0A_DSC_VDO) += panel-p10-36-02-0a-dsc-vdo.o
obj-$(CONFIG_DRM_PANEL_P10_42_02_0B_DSC_VDO) += panel-p10-42-02-0b-dsc-vdo.o
obj-$(CONFIG_DRM_PANEL_TD4160_HKC_HDP_VDO) += panel-ocp2131-i2c.o
obj-$(CONFIG_DRM_PANEL_TD4160_HKC_HDP_VDO) += panel-td4160-hkc-hdp-dsi-vdo-120hz.o
obj-$(CONFIG_DRM_PANEL_TD4160_BOE_HDP_VDO) += panel-ocp2131-i2c.o
obj-$(CONFIG_DRM_PANEL_TD4160_BOE_HDP_VDO) += panel-td4160-boe-hdp-dsi-vdo-120hz.o

ifneq ($(wildcard $(srctree)/$(src)/k6985v1_64),)
obj-$(CONFIG_DRM_TD2204_WQHD_AMB678ZY01_S6E3HC3_CMD) += k6985v1_64/td2204-wqhd-amb678zy01-s6e3hc3-cmd.o
endif
ifneq ($(wildcard $(srctree)/$(src)/k6985v1_64_alpha),)
obj-$(CONFIG_DRM_PANEL_NT37705_ALPHA_CMD) += k6985v1_64_alpha/panel-nt37705-alpha-cmd.o
endif
ifneq ($(wildcard $(srctree)/$(src)/k6989v1_64_alpha),)
obj-$(CONFIG_DRM_PANEL_ILI7838E_ALPHA_CMD) += k6989v1_64_alpha/panel-ili7838e-alpha-cmd.o
obj-$(CONFIG_DRM_PANEL_ILI7838E_DV2_ALPHA_CMD) += k6989v1_64_alpha/panel-ili7838e-dv2-alpha-cmd.o
obj-$(CONFIG_DRM_PANEL_ILI7838E_DV2_SPR_CMD) += k6989v1_64_alpha/panel-ili7838e-dv2-spr-cmd.o
endif
ifneq ($(wildcard $(srctree)/$(src)/k6991v1_64_alpha),)
obj-$(CONFIG_DRM_PANEL_RM692H5_ALPHA_CMD_SPR) += k6991v1_64_alpha/panel-rm692h5-alpha-cmd-spr.o
obj-$(CONFIG_DRM_PANEL_NT37707_BOE_FHDPLUS_CMD) += k6991v1_64_alpha/panel-nt37707-boe-fhdplus-cmd.o
obj-$(CONFIG_DRM_PANEL_RM692K0_VISIONOX_FHDPLUS_CMD) += k6991v1_64_alpha/panel-rm692k0-visionox-fhdplus-cmd.o
obj-$(CONFIG_DRM_PANEL_NT37707_BOE_FHDPLUS_CMD_LTPS) += k6991v1_64_alpha/panel-nt37707-boe-fhdplus-cmd-ltps.o
obj-$(CONFIG_DRM_PANEL_RM692K0_VSN_FHDPLUS_CMD_LTPS) += k6991v1_64_alpha/panel-rm692k0-vsn-fhdplus-cmd-ltps.o
obj-$(CONFIG_DRM_PANEL_NT37707_BOE_FHDPLUS_CMD_LTPO) += k6991v1_64_alpha/panel-nt37707-boe-fhdplus-cmd-ltpo.o
obj-$(CONFIG_DRM_PANEL_RM692J0_TM_FHDPLUS_CMD_LTPO) += k6991v1_64_alpha/panel-rm692j0-tm-fhdplus-cmd-ltpo.o
obj-$(CONFIG_DRM_PANEL_NT37706A_BOE_FHDPLUS_VDO) += k6991v1_64_alpha/panel-nt37706a-boe-fhdplus-vdo.o
endif
obj-$(CONFIG_DRM_PANEL_SERDES_DP) += panel-serdes-dp.o





ccflags-y += -I$(DEVICE_MODULES_PATH)/drivers/misc/mediatek/smi/			\
             -I$(DEVICE_MODULES_PATH)/drivers/misc/mediatek/mmp/			\
             -I$(DEVICE_MODULES_PATH)/drivers/misc/mediatek/mmdvfs/			\
             -I$(DEVICE_MODULES_PATH)/drivers/iommu/				\
             -I$(DEVICE_MODULES_PATH)/drivers/devfreq/				\
             -I$(DEVICE_MODULES_PATH)/drivers/gpu/drm/				\
             -I$(DEVICE_MODULES_PATH)/drivers/misc/mediatek/include/mt-plat/ \
             -I$(DEVICE_MODULES_PATH)/drivers/misc/mediatek/cmdq/mailbox/ \
             -I$(DEVICE_MODULES_PATH)/drivers/misc/mediatek/iommu/ \
             -I$(DEVICE_MODULES_PATH)/drivers/misc/mediatek/mtk-interconnect/ \
             -I$(DEVICE_MODULES_PATH)/drivers/misc/mediatek/slbc/ \
             -I$(DEVICE_MODULES_PATH)/drivers/misc/mediatek/scp/include/ \
             -I$(DEVICE_MODULES_PATH)/drivers/gpu/drm/mediatek/mediatek_v2/mi_disp/ \
             -I$(DEVICE_MODULES_PATH)/drivers/gpu/drm/mediatek/mediatek_v2/ \
			 -I$(DEVICE_MODULES_PATH)/drivers/gpu/drm/mediatek/dpc/ \

ccflags-$(CONFIG_DRM_MEDIATEK_PANEL) += -I$(DEVICE_MODULES_PATH)/drivers/leds/

ifeq (${CONFIG_VIS_DISPLAY_DALI},m)
ccflags-y += -I$(DEVICE_MODULES_PATH)/drivers/gpu/drm/mediatek/mediatek_v2/driver_novatek/dali/
ccflags-y += -DCONFIG_VIS_DISPLAY_DALI
endif
