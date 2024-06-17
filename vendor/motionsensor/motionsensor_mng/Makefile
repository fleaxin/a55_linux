ifeq ($(EXTDRV_PARAM_FILE), )
    SUB_DIR_DEEP=../
    EXTDRV_PARAM_FILE:=../../Makefile.param
    include $(EXTDRV_PARAM_FILE)
endif

ifeq ($(CONFIG_OT_MOTIONFUSION_SUPPORT), y)

SRCS += ./src/motionsensor_exe.c
SRCS += ./src/motionsensor_buf.c
ifeq ($(CONFIG_OT_PROC_SHOW_SUPPORT), y)
SRCS += ./src/motionsensor_mng_proc.c
endif

INC := -I$(REL_INC)
INC += -I$(OSAL_ROOT)/include
INC += -I$(CBB_ROOT)/include

ifeq ($(MPP_BUILD),y)
	INC += -I$(VENDOR_ROOT)/$(OT_ARCH)/motionsensor/include/
	INC += -I$(VENDOR_ROOT)/$(OT_ARCH)/motionsensor/motionsensor_mng/include
	INC += -I$(VENDOR_ROOT)/$(OT_ARCH)/motionsensor/ext_inc
else
	INC += -I$(VENDOR_ROOT)/motionsensor/include/
	INC += -I$(VENDOR_ROOT)/motionsensor/motionsensor_mng/include
	INC += -I$(VENDOR_ROOT)/motionsensor/ext_inc
endif

EXTDRV_CFLAGS += $(INC)

#*************************************************************************
TARGET := motionsensor_mng
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
