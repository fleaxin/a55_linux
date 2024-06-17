include $(PWD)/../Makefile.param

CFLAGS += -I$(PWD)/include
CFLAGS += -O3
CFLAGS += -L$(REL_LIB)/npu/stub -lascendcl
INC_FLAGS += -I$(REL_INC)/npu

SMP_SRCS := $(wildcard ./*.c)
SMP_SRCS += $(wildcard ./sample_svp_npu/*.c)
SMP_SRCS += $(wildcard $(PWD)/../common/*.c)
TARGET := sample_npu_main
TARGET_PATH := $(PWD)

# compile linux or LITEOS
include $(PWD)/../../$(ARM_ARCH)_$(OSTYPE).mak
