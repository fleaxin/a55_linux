ifeq ($(PARAM_FILE), )
     PARAM_FILE := ../Makefile.param
     include $(PARAM_FILE)
endif

ifeq ($(OT_ARCH), $(filter $(OT_ARCH), ss928v100 ss626v100))
     LIBS += $(REL_LIB)/libss_fisheye_calibrate.a
     SRC := $(wildcard *.c)
else
     ALLSRC := $(wildcard *.c)
     SRC := $(filter-out fisheye_calibration.c, $(ALLSRC))
endif

ifeq ($(CONFIG_OT_AVS_SUPPORT), )
     SRC := $(filter-out avs_chn_dump.c avs_src_dump.c, $(SRC))
endif

ifeq ($(CONFIG_OT_VI_ALL_SUPPORT), y)
     SRC := $(filter-out vi_dump.c, $(SRC))
else ifeq ($(CONFIG_OT_VI_XVR_SUPPORT), y)
     SRC := $(filter-out vi_pipe_dump.c vi_chn_dump.c, $(SRC))
else
     SRC := $(filter-out vi_pipe_dump.c vi_chn_dump.c vi_dump.c, $(SRC))
endif

ifeq ($(CONFIG_OT_VO_WBC), )
    SRC := $(filter-out vo_wbc_dump.c, $(SRC))
endif

ifeq ($(CONFIG_MIPI_TX), )
    SRC := $(filter-out mipitx_read.c mipitx_write.c, $(SRC))
endif


ifeq ($(CONFIG_OT_MCF_SUPPORT), )
	SRC := $(filter-out mcf_chn_dump.c mcf_grp_dump.c, $(SRC))
endif

ifeq ($(CONFIG_OT_MCF_SUPPORT), y)
	LIBS += $(REL_LIB)/libss_mcf.a
endif

OBJ := $(SRC:%.c=%.o)

INC_FLAGS := -I$(REL_INC)
INC_FLAGS += -I$(REL_INC)
CFLAGS += -Wall -g -lpthread $(INC_FLAGS)
CFLAGS += $(LIBS_CFLAGS)
CFLAGS += $(LIBS_EXEC_CFLAGS)
CFLAGS += -ldl

LIBS += $(REL_LIB)/libss_mpi.a
ifeq ($(CONFIG_OT_SECUREC_SUPPORT),y)
LIBS += $(REL_LIB)/libsecurec.a
else
LIBS += -lsecurec
endif

ifeq ($(CONFIG_OT_ISP_SUPPORT), y)
LIBS += $(REL_LIB)/libss_ae.a
LIBS += $(REL_LIB)/libss_isp.a
LIBS += $(REL_LIB)/libot_isp.a
LIBS += $(REL_LIB)/libss_awb.a
LIBS += $(REL_LIB)/libss_dehaze.a
LIBS += $(REL_LIB)/libss_extend_stats.a
LIBS += $(REL_LIB)/libss_drc.a
LIBS += $(REL_LIB)/libss_ldci.a
LIBS += $(REL_LIB)/libss_crb.a
LIBS += $(REL_LIB)/libss_bnr.a
LIBS += $(REL_LIB)/libss_calcflicker.a
LIBS += $(REL_LIB)/libss_ir_auto.a
LIBS += $(REL_LIB)/libss_acs.a
endif

LIBS += $(REL_LIB)/libss_voice_engine.a
LIBS += $(REL_LIB)/libss_upvqe.a
LIBS += $(REL_LIB)/libss_dnvqe.a

LIBS += $(LIBS_LD_CFLAGS)
CFLAGS += $(LIBS_CFLAGS)

CFLAGS += -lm

include $(PWD)/Make.$(OSTYPE)
