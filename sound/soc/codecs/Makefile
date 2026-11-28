# SPDX-License-Identifier: GPL-2.0

ifneq ($(CONFIG_MTK_TINYSYS_SCP_SUPPORT),)
ccflags-y += -I$(DEVICE_MODULES_PATH)/drivers/misc/mediatek/scp/include
endif

ifneq ($(CONFIG_DEVICE_MODULES_SND_SOC_MEDIATEK),)
ccflags-y += -I$(DEVICE_MODULES_PATH)/sound/soc/mediatek/common
endif

snd-soc-mt6351-objs := mt6351.o
snd-soc-mt6358-objs := mt6358.o
snd-soc-mt6660-objs := mt6660.o
snd-soc-mt6359-accdet-objs := mt6359-accdet.o
snd-soc-mt6368-objs := mt6368.o
snd-soc-hdmi-codec-objs := hdmi-codec.o
snd-soc-rt5509-objs += rt5509.o rt5509-calib.o
snd-soc-mt6366-objs := mt6358.o
snd-soc-cs40l26-objs := cs40l26.o

obj-$(CONFIG_DEVICE_MODULES_SND_SOC_MT6351)	+= snd-soc-mt6351.o
obj-$(CONFIG_DEVICE_MODULES_SND_SOC_MT6358)	+= snd-soc-mt6358.o
obj-$(CONFIG_DEVICE_MODULES_SND_SOC_MT6359_ACCDET) += mt6359-accdet.o
obj-$(CONFIG_DEVICE_MODULES_SND_SOC_MT6660)	+= snd-soc-mt6660.o
obj-$(CONFIG_SND_SOC_MT6368)	+= snd-soc-mt6368.o
obj-$(CONFIG_SND_SOC_MT6368_ACCDET) += mt6368-accdet.o
obj-$(CONFIG_SND_SOC_TYPEC_ANALOG_ACC) += typec/
obj-$(CONFIG_DEVICE_MODULES_SND_SOC_HDMI_CODEC)	+= snd-soc-hdmi-codec.o
obj-$(CONFIG_SND_SOC_RT5512)	+= richtek/
obj-$(CONFIG_SND_SOC_TFA9874)	+= tfa98xx/
obj-$(CONFIG_SND_SOC_RT5509) 	+= snd-soc-rt5509.o
obj-$(CONFIG_SND_SOC_AK7709)	+= ak7709/
obj-$(CONFIG_SND_SOC_HFDA80X)	+= hfda80x/
obj-$(CONFIG_DEVICE_MODULES_SND_SOC_MT6366)	+= snd-soc-mt6366.o
obj-$(CONFIG_SND_SOC_FS16XX)    += fs18xx/
obj-$(CONFIG_SND_SMARTPA_AW882XX) += pa_combine/
obj-$(CONFIG_SND_SOC_CS40L26)   += snd-soc-cs40l26.o
obj-$(CONFIG_SND_SOC_FRSM)      += frsm_i2cd/
