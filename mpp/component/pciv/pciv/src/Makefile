# Describe the include path, using the full path
ifeq ($(PARAM_FILE), )
    PARAM_FILE:=../../../../Makefile.param
    include $(PARAM_FILE)
endif

PCIV_ROOT := $(PWD)/../..
KBUILD_EXTRA_SYMBOLS += "$(PCIV_ROOT)/pciv_fmw/src/Module.symvers"
KBUILD_EXTRA_SYMBOLS += "$(KERNEL_ROOT)/../../../components/pcie_mcc/pcie_mcc/Module.symvers"

# Enum the C files needed to be compiled, using the relative path
SRCS :=
SRCS += pciv.c
SRCS += pciv_drvadp.c
SRCS += pciv_ext.c
ifeq ($(CONFIG_OT_PROC_SHOW_SUPPORT), y)
    SRCS += pciv_proc.c
endif

SRCS_O := $(SRCS)
#*************************************************************************
# local header
MKP_CFLAGS := -I$(PCIV_ROOT)/pciv/include

#*************************************************************************
TARGET := $(KO_PREFIX)_pciv

#*************************************************************************
# compile linux or liteos
include $(PWD)/../../Makefile.$(OSTYPE)
