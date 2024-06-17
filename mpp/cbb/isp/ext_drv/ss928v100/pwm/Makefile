
ifeq ($(EXTDRV_PARAM_FILE), )
    EXTDRV_PARAM_FILE:=../Makefile.param
    include $(EXTDRV_PARAM_FILE)
endif



#SRCS = $(wildcard *.c)
SRCS = pwm.c

ifeq ($(MPP_BUILD),y)
	INC := -I$(OSAL_ROOT)/$(OSTYPE)/kernel/ot_media
else
	INC := -I$(OSAL_ROOT)/source/kernel/ot_media
endif
INC += -I$(OSAL_ROOT)/include

INC += -I$(DRV_ROOT)/extdrv/$(EXTDRVVER)/pwm/arch/$(ARCH_DIR)/
EXTDRV_CFLAGS += $(INC)
EXTDRV_CFLAGS += $(COMM_INC)






#*************************************************************************
TARGET := ot_pwm

#*************************************************************************
# compile linux or liteos
include $(PWD)/../Make.$(OSTYPE)
