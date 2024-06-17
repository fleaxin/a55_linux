include ../Makefile.param

SMP_SRCS := $(wildcard *.c)
TARGET := sample_photo

MPI_LIBS += $(REL_LIB)/libss_snap.a
MPI_LIBS += $(REL_LIB)/libss_photo.a
MPI_LIBS += $(REL_LIB)/libss_dsp.a
MPI_LIBS += $(REL_LIB)/libhdr_ap.a
MPI_LIBS += $(REL_LIB)/libmfnr_ap.a
MPI_LIBS += $(REL_LIB)/libsfnr_ap.a
MPI_LIBS += $(REL_LIB)/libdetail_ap.a

TARGET_PATH := $(PWD)

include $(PWD)/../$(ARM_ARCH)_$(OSTYPE).mak
