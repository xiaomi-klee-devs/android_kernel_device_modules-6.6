# SPDX-License-Identifier: GPL-2.0
# Mediatek UFS extended features makefile
ccflags-y += -I$(DEVICE_MODULES_PATH)/drivers/clk/mediatek/
ccflags-y += -I$(DEVICE_MODULES_PATH)/drivers/misc/mediatek/include/
ccflags-y += -I$(DEVICE_MODULES_PATH)/drivers/misc/mediatek/power_throttling/
ccflags-y += -I$(DEVICE_MODULES_PATH)/drivers/ufs/
ccflags-y += -I$(DEVICE_MODULES_PATH)/drivers/ufs/vendor/
ccflags-y += -I$(DEVICE_MODULES_PATH)/include
ccflags-$(CONFIG_SCSI_UFS_XIAOMI) += -I$(DEVICE_MODULES_PATH)/drivers/ufs/mi_ufs/core/
ccflags-$(CONFIG_SCSI_UFS_XIAOMI) += -I$(DEVICE_MODULES_PATH)/drivers/ufs/mi_ufs/host/
ccflags-$(CONFIG_SCSI_UFS_XIAOMI) += -I$(DEVICE_MODULES_PATH)/drivers/ufs/mi_ufs/include/

ccflags-y += -I$(srctree)/drivers/scsi/
ccflags-y += -I$(srctree)/kernel/
ccflags-y += -I$(srctree)/block/

# must after -I$(DEVICE_MODULES_PATH)/drivers/ufs/
ccflags-y += -I$(srctree)/drivers/ufs/
ccflags-y += -I$(srctree)/drivers/ufs/core/
ccflags-y += -I$(srctree)/drivers/ufs/host/

# UFS KO without iSE
obj-$(CONFIG_DEVICE_MODULES_SCSI_UFS_MEDIATEK)  += ufs-mediatek-mod.o
ifeq ($(CONFIG_UFS_MEDIATEK_MT6893),y)
ufs-mediatek-mod-y				+= ../ufs-mediatek-perf.o ../ufs-mediatek-sysfs.o ../ufs-mediatek-mimic.o ../ufs-mediatek-mbrain.o
else
ufs-mediatek-mod-y                              += ../ufs-mediatek.o ../ufs-mediatek-sysfs.o ../ufs-mediatek-mimic.o ../ufs-mediatek-mbrain.o
endif

ufs-mediatek-mod-$(CONFIG_MTK_BLOCK_IO_TRACER)  += ../ufs-mediatek-btag.o
ufs-mediatek-mod-$(CONFIG_RPMB)                 += ../ufs-mediatek-rpmb.o
ufs-mediatek-mod-$(CONFIG_RPMB)                 += ../ufs-mediatek-ise-dummy.o
ufs-mediatek-mod-$(CONFIG_UFSFEATURE)           += ufsfeature.o ufsfeature-mimic.o
ufs-mediatek-mod-$(CONFIG_UFSSHPB)              += ufsshpb.o
ufs-mediatek-mod-$(CONFIG_UFSTW)                += ufstw.o
ufs-mediatek-mod-$(CONFIG_UFSHID)               += ufshid.o
ufs-mediatek-mod-$(CONFIG_UFSRINGBUF)           += ufsringbuf.o


# UFS KO with iSE enabled
ccflags-y += -I$(srctree)/drivers/ufs/
ccflags-y += -I$(srctree)/drivers/ufs/core/
ccflags-y += -I$(srctree)/drivers/ufs/host/

obj-$(CONFIG_DEVICE_MODULES_SCSI_UFS_MEDIATEK)      += ufs-mediatek-mod-ise.o
ifeq ($(CONFIG_UFS_MEDIATEK_MT6893),y)
ufs-mediatek-mod-ise-y                              += ../ufs-mediatek-perf.o ../ufs-mediatek-sysfs.o ../ufs-mediatek-mimic.o ../ufs-mediatek-mbrain.o
else
ufs-mediatek-mod-ise-y                              += ../ufs-mediatek.o ../ufs-mediatek-sysfs.o ../ufs-mediatek-mimic.o ../ufs-mediatek-mbrain.o
endif

ufs-mediatek-mod-ise-$(CONFIG_MTK_BLOCK_IO_TRACER)  += ../ufs-mediatek-btag.o
ufs-mediatek-mod-ise-$(CONFIG_RPMB)                 += ../ufs-mediatek-rpmb.o
ufs-mediatek-mod-ise-$(CONFIG_RPMB)                 += ../ufs-mediatek-ise.o
ufs-mediatek-mod-ise-$(CONFIG_UFSFEATURE)           += ufsfeature.o ufsfeature-mimic.o
ufs-mediatek-mod-ise-$(CONFIG_UFSSHPB)              += ufsshpb.o
ufs-mediatek-mod-ise-$(CONFIG_UFSTW)                += ufstw.o
ufs-mediatek-mod-ise-$(CONFIG_UFSHID)               += ufshid.o
ufs-mediatek-mod-ise-$(CONFIG_UFSRINGBUF)           += ufsringbuf.o

ifeq ($(CONFIG_SCSI_UFS_XIAOMI),y)
ufs-mediatek-mod-y += \
	../mi_ufs/core/mi_ufshcd.o \
	../mi_ufs/core/mi_ufs-sysfs.o \
	../mi_ufs/core/mi_ufs-mcq.o \
	../mi_ufs/core/mi_ufs-debugfs.o \
	../mi_ufs/core/mi_ufs_bsg.o \
	../mi_ufs/core/mi_ufs-fault-injection.o \
	../mi_ufs/core/mi_ufshcd-crypto.o \
	../mi_ufs/core/mi_ufs-hwmon.o \
	../mi_ufs/host/mi_ufshcd-pltfrm.o

ufs-mediatek-mod-ise-y += \
	../mi_ufs/core/mi_ufshcd.o \
	../mi_ufs/core/mi_ufs-sysfs.o \
	../mi_ufs/core/mi_ufs-mcq.o \
	../mi_ufs/core/mi_ufs-debugfs.o \
	../mi_ufs/core/mi_ufs_bsg.o \
	../mi_ufs/core/mi_ufs-fault-injection.o \
	../mi_ufs/core/mi_ufshcd-crypto.o \
	../mi_ufs/core/mi_ufs-hwmon.o \
	../mi_ufs/host/mi_ufshcd-pltfrm.o
endif

ifeq ($(CONFIG_SCSI_UFS_XIAOMI_FFU),y)
ufs-mediatek-mod-y += ../mi_ufs/feature/ffu/ufsffu.o
ufs-mediatek-mod-ise-y += ../mi_ufs/feature/ffu/ufsffu.o
endif

ifeq ($(CONFIG_SCSI_UFS_XIAOMI_CLD),y)
ufs-mediatek-mod-y += \
	../mi_ufs/feature/cld/ufscld.o \
	../mi_ufs/feature/cld/ufscld_sysfs.o \
	../mi_ufs/feature/cld/hid.o \
	../mi_ufs/feature/cld/kioxia_cld.o \
	../mi_ufs/feature/cld/micron_cld.o \
	../mi_ufs/feature/cld/samsung_cld.o \
	../mi_ufs/feature/cld/skhynixcld.o \
	../mi_ufs/feature/cld/ymtc_cld.o

ufs-mediatek-mod-ise-y += \
	../mi_ufs/feature/cld/ufscld.o \
	../mi_ufs/feature/cld/ufscld_sysfs.o \
	../mi_ufs/feature/cld/hid.o \
	../mi_ufs/feature/cld/kioxia_cld.o \
	../mi_ufs/feature/cld/micron_cld.o \
	../mi_ufs/feature/cld/samsung_cld.o \
	../mi_ufs/feature/cld/skhynixcld.o \
	../mi_ufs/feature/cld/ymtc_cld.o
endif
