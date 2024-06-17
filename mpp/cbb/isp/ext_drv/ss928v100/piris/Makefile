


ifeq ($(EXTDRV_PARAM_FILE), )
    EXTDRV_PARAM_FILE:=../Makefile.param
    include $(EXTDRV_PARAM_FILE)
endif


SRCS = piris.c


ifeq ($(MPP_BUILD),y)
	INC := -I$(OSAL_ROOT)/$(OSTYPE)/kernel/ot_media
else
	INC := -I$(OSAL_ROOT)/source/kernel/ot_media
endif
INC += -I$(ISP_ROOT)/ext_inc
INC += -I$(ISP_ROOT)/kernel/mkp/include
INC += -I$(ISP_ROOT)/kernel/arch/$(OT_ARCH)/include
INC += -I$(ISP_ROOT)/user/firmware/arch/$(OT_ARCH)/include
INC += -I$(ISP_ROOT)/user/firmware/include
INC += -I$(DRV_ROOT)/extdrv/$(EXTDRVVER)/piris/arch/$(ARCH_DIR)/
INC += -I$(OSAL_ROOT)/include

EXTDRV_CFLAGS += $(INC)
EXTDRV_CFLAGS += $(COMM_INC)




#*************************************************************************
TARGET := ot_piris

#*************************************************************************
# compile linux or liteos
include $(PWD)/../Make.$(OSTYPE)
