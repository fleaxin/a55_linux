
include ../Makefile.param

INC_FLAGS +=-I$(OPEN_SOURCE_PATH)/iniparser/out
INC_FLAGS +=-I./include
INC_FLAGS +=-I$(REL_DIR)/include
SMP_SRCS += $(wildcard *.c)
SMP_SRCS += $(wildcard ./src/*.c)
TARGET := $(SMP_SRCS:%.c=%)

TARGET_PATH := $(PWD)
INIPARSER_LIB := $(OPEN_SOURCE_PATH)/iniparser/out/libiniparser.a
CFLAGS += $(INC_FLAGS)

include $(PWD)/../$(ARM_ARCH)_$(OSTYPE).mak
