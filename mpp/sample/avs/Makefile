
include ../Makefile.param

SMP_SRCS := $(wildcard *.c)
TARGET := $(SMP_SRCS:%.c=%)

MPI_LIBS += $(REL_LIB)/libss_avslut.a
MPI_LIBS += $(REL_LIB)/libss_pos_query.a
MPI_LIBS += $(REL_LIB)/libss_avsconvert.a

TARGET_PATH := $(PWD)

# compile linux or LITEOS
include $(PWD)/../$(ARM_ARCH)_$(OSTYPE).mak
