ifeq ($(EXTDRV_PARAM_FILE), )
    SUB_DIR_DEEP=../
    EXTDRV_PARAM_FILE:=../../Makefile.param
    include $(EXTDRV_PARAM_FILE)
endif
ifeq ($(CONFIG_OT_MOTIONFUSION_SUPPORT), y)
# Sensor Type: ICM20690
SENSOR_TYPE ?= ICM20690

# Transfer Type: I2C SPI
export TRANSFER_TYPE := SPI

# ICM20690 proc info function : y or n
export ICM20690_PROC = y

SRCS = ./main/motionsensor.c

INC := -I$(REL_INC)
INC += -I$(OSAL_ROOT)/include
INC += -I$(CBB_ROOT)/include

INC += -I$(CBB_ROOT)/include/adapt
INC += -I$(CBB_ROOT)/base/include/adapt
INC += -I$(CBB_ROOT)/sys/include/adapt

#debug
INC += -I$(CBB_ROOT)/vi/ext_inc
ifeq ($(OSTYPE), liteos)
INC += -I$(LITEOSTOPDIR)/drivers/gpio/include
endif
ifeq ($(MPP_BUILD),y)
    INC += -I$(VENDOR_ROOT)/$(OT_ARCH)/motionsensor/motionsensor_chip/main/
    ##INC += -I$(VENDOR_ROOT)/$(OT_ARCH)/motionsensor/motionsensor_chip/proc/
    INC += -I$(VENDOR_ROOT)/$(OT_ARCH)/motionsensor/motionsensor_chip/pub/
    INC += -I$(VENDOR_ROOT)/$(OT_ARCH)/motionsensor/motionsensor_mng/include/
    INC += -I$(VENDOR_ROOT)/$(OT_ARCH)/motionsensor/include/
    INC += -I$(VENDOR_ROOT)/$(OT_ARCH)/motionsensor/ext_inc
    INC += -I$(VENDOR_ROOT)/$(OT_ARCH)/motionsensor/motionsensor_spi/
else
    INC += -I$(VENDOR_ROOT)/motionsensor/motionsensor_chip/main/
    ##INC += -I$(VENDOR_ROOT)/motionsensor/motionsensor_chip/proc/
    INC += -I$(VENDOR_ROOT)/motionsensor/motionsensor_chip/pub/
    INC += -I$(VENDOR_ROOT)/motionsensor/motionsensor_mng/include/
    INC += -I$(VENDOR_ROOT)/motionsensor/include/
    INC += -I$(VENDOR_ROOT)/motionsensor/ext_inc
    INC += -I$(VENDOR_ROOT)/motionsensor/motionsensor_spi/
endif

ifeq ($(CONFIG_OT_PROC_SHOW_SUPPORT),y)
ifeq ($(ICM20690_PROC),y)
	ccflags-y += -DICM20690_PARAM_PROC
	SRCS += ./proc/motionsensor_chip_proc.c
ifeq ($(MPP_BUILD),y)
	INC += -I$(VENDOR_ROOT)/$(OT_ARCH)/motionsensor/motionsensor_chip/proc/
else
	INC += -I$(VENDOR_ROOT)/motionsensor/motionsensor_chip/proc/
endif
endif
endif

ifeq ($(SENSOR_TYPE), ICM20690)
    SRCS += ./sensor_dev/icm20690/icm20690.c
	SRCS += ./sensor_dev/icm20690/motionsensor_gpio.c
ifeq ($(MPP_BUILD),y)
    INC += -I$(VENDOR_ROOT)/$(OT_ARCH)/motionsensor/motionsensor_chip/sensor_dev/icm20690/
else
    INC += -I$(VENDOR_ROOT)/motionsensor/motionsensor_chip/sensor_dev/icm20690/
endif


endif

ifeq ($(TRANSFER_TYPE), I2C)
    SRCS += ./transfer_type/i2c/i2c_dev.c
	ccflags-y += -DTRANSFER_I2C
ifeq ($(MPP_BUILD),y)
    INC += -I$(VENDOR_ROOT)/$(OT_ARCH)/motionsensor/motionsensor_chip/transfer_type/i2c/
else
    INC += -I$(VENDOR_ROOT)/motionsensor/motionsensor_chip/transfer_type/i2c/
endif
    INC += -I$(TRANSFER_DIR)/i2c
endif

ifeq ($(TRANSFER_TYPE), SPI)
ifeq ($(CONFIG_LITEOS_OS),y)
    SRCS += ./transfer_type/spi/spi_dev.c
endif
	ccflags-y += -DTRANSFER_SPI
ifeq ($(MPP_BUILD),y)
    INC += -I$(VENDOR_ROOT)/$(OT_ARCH)/motionsensor/motionsensor_chip/transfer_type/spi/
else
    INC += -I$(VENDOR_ROOT)/motionsensor/motionsensor_chip/transfer_type/spi/
endif
    INC += -I$(TRANSFER_DIR)/spi
endif

EXTDRV_CFLAGS += $(INC)

#*************************************************************************
TARGET := motionsensor_chip

#*************************************************************************
# compile linux or liteos
include $(PWD)/../../Make.$(OSTYPE)

#CONFIG_OT_MOTIONFUSION_SUPPORT endif
else
all:
	@echo "motionsensor module not compile"
clean:
	@echo "motionsensor module don't need clean"
endif
