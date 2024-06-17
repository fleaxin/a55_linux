
include ../Makefile.param

INC_FLAGS +=-I./include
INC_FLAGS +=-I./../scene
INC_FLAGS +=-I./tools/configaccess/include
#INC_FLAGS +=-I./tools/iniparser/include
INC_FLAGS +=-I$(OPEN_SOURCE_PATH)/iniparser/out
INC_FLAGS +=-I./../common
INC_FLAGS +=-I$(REL_DIR)/include

SMP_SRCS += $(wildcard ./tools/configaccess/src/*.c)
SMP_SRCS += $(wildcard ./src/core/*.c)
SMP_SRCS += $(wildcard ./src/sample/*.c)

INC_FLAGS +=-I$(OPEN_SOURCE_PATH)/iniparser/out
INC_FLAGS +=-I./include
INC_FLAGS +=-I$(REL_DIR)/include

TARGET := sample_scene

TARGET_PATH := $(PWD)
INIPARSER_LIB := $(OPEN_SOURCE_PATH)/iniparser/out/libiniparser.a


SCENE_AUTO_MCF_SUPPORT ?= n
SCENE_AUTO_HNR_SUPPORT ?= n

ifeq ($(SCENE_AUTO_MCF_SUPPORT), y)
    CFLAGS += -DMCF_SCENE_AUTO_USED
    INIPARSER_LIB += $(REL_LIB)/libss_mcf.a
    INIPARSER_LIB += $(REL_LIB)/libss_mcfcalibrate.a
    INIPARSER_LIB += $(REL_LIB)/libss_mcf_vi.a
endif

ifeq ($(SCENE_AUTO_HNR_SUPPORT), y)
    CFLAGS += -DHNR_SCENE_AUTO_USED
    MPI_LIBS += -L$(REL_LIB) -lss_hnr
    INIPARSER_LIB += $(REL_LIB)/libss_pqp.a
endif


CFLAGS += $(INC_FLAGS)



# compile linux or LITEOS
include $(PWD)/../$(ARM_ARCH)_$(OSTYPE).mak
