# Describe the include path, using the full path
ifeq ($(PARAM_FILE), )
    PARAM_FILE:=../../../../Makefile.param
    include $(PARAM_FILE)
endif

# Enum the C files needed to be compiled, using the relative path
SRCS :=
SRCS += pciv_firmware.c
SRCS += pciv_pic_queue.c
SRCS += pciv_firmware_comm.c
ifeq ($(CONFIG_OT_PROC_SHOW_SUPPORT), y)
    SRCS += pciv_firmware_proc.c
endif
SRCS_O := $(SRCS)
#*************************************************************************
# local header
PCIV_ROOT := $(PWD)/../..

MKP_CFLAGS := -I$(PCIV_ROOT)/pciv_fmw/include

#*************************************************************************
TARGET := $(KO_PREFIX)_pciv_fmw

#*************************************************************************
# compile linux or liteos
include $(PWD)/../../Makefile.$(OSTYPE)
