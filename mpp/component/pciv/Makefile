#*******************************************************************************
#The rule of make Linux kernel module
#*******************************************************************************

ifeq ($(PARAM_FILE), )
    PARAM_FILE:=../../Makefile.param
    include $(PARAM_FILE)
endif

ifeq ($(CONFIG_OT_PCIV_SUPPORT), y)
COPY_INC_PATH=$(PWD)/copy_inc

BASE_HEADER            := mkp_ioctl.h dev_ext.h mod_ext.h vb_ext.h mm_ext.h proc_ext.h
VGS_EXT_HEADER         := vgs_ext.h
SYS_EXT_HEADER         := sys_ext.h
RGN_EXT_HEADER         := region_ext.h
VPSS_EXT_HEADER        := vpss_ext.h

MCC_USRDEV_HEADER := ot_mcc_usrdev.h

SYS_COMM_HEADER        := ot_common_sys.h
VB_COMM_HEADER         := ot_common_vb.h
VGS_COMM_HEADER        := ot_common_vgs.h
VPSS_COMM_HEADER       := ot_common_vpss.h
VO_COMM_HEADER         := ot_common_vo.h
RGN_COMM_HEADER        := ot_common_region.h

CBB_INC_HEADER         := autoconf.h ot_common_video.h ot_common.h ot_debug.h ot_errno.h ot_math.h ot_type.h
OSAL_INC_HEADER        := ot_osal.h osal_list.h osal_mmz.h osal_ioctl.h
DEFINES_HEADER         := ot_defines.h
SECUREC_HEADER         := securec.h securectype.h

CLEAN_COPY_FILE        := $(CBB_INC_HEADER) ot_common_sys.h ot_common_vb.h ot_common_vgs.h ot_common_vpss.h ot_common_vo.h ot_common_region.h $(OSAL_INC_HEADER) $(DEFINES_HEADER) $(SECUREC_HEADER)

CBB_PATH    := $(MPP_PATH)/cbb
OSAL_PATH   := $(SDK_PATH)/osal
RGN_PATH    := $(CBB_PATH)/region
SYS_PATH    := $(CBB_PATH)/sys
BASE_PATH   := $(CBB_PATH)/base
VO_PATH     := $(CBB_PATH)/vo
VPSS_PATH   := $(CBB_PATH)/vpss
VGS_PATH    := $(CBB_PATH)/vgs
SECUREC_PATH:= $(MPP_PATH)/component/securec
DEFINES_SUB_PATH := $(BASE_PATH)/arch/$(OT_ARCH)/include/$(OT_SUBARCH)
DEFINES_PATH := $(BASE_PATH)/arch/$(OT_ARCH)/include
DEFINES_PATH := $(shell if [ -d "$(DEFINES_SUB_PATH)" ]; then echo "$(DEFINES_SUB_PATH)"; else echo "$(DEFINES_PATH)"; fi)
#*******************************************************************************
.PHONY: all prepare build mpi pciv_fmw pciv clean mpi_clean pciv_fmw_clean pciv_clean
all:build
prepare:
ifeq ($(MPP_BUILD), y)
	@-mkdir -p copy_inc;

	@-pushd $(BASE_PATH)/ext_inc > /dev/null;       cp $(BASE_HEADER)      $(COPY_INC_PATH); popd > /dev/null
	@-pushd $(VGS_PATH)/ext_inc > /dev/null;        cp $(VGS_EXT_HEADER)   $(COPY_INC_PATH); popd > /dev/null
	@-pushd $(SYS_PATH)/ext_inc > /dev/null;        cp $(SYS_EXT_HEADER)   $(COPY_INC_PATH); popd > /dev/null
	@-pushd $(RGN_PATH)/ext_inc > /dev/null;        cp $(RGN_EXT_HEADER)   $(COPY_INC_PATH); popd > /dev/null
	@-pushd $(VPSS_PATH)/ext_inc > /dev/null;       cp $(VPSS_EXT_HEADER)  $(COPY_INC_PATH); popd > /dev/null
	@-pushd $(SYS_PATH)/include > /dev/null;        cp $(SYS_COMM_HEADER)  $(COPY_INC_PATH); popd > /dev/null
	@-pushd $(BASE_PATH)/include > /dev/null;       cp $(VB_COMM_HEADER)   $(COPY_INC_PATH); popd > /dev/null
	@-pushd $(VO_PATH)/include > /dev/null;         cp $(VO_COMM_HEADER)   $(COPY_INC_PATH); popd > /dev/null
	@-pushd $(VGS_PATH)/include > /dev/null;        cp $(VGS_COMM_HEADER)  $(COPY_INC_PATH); popd > /dev/null
	@-pushd $(VPSS_PATH)/include > /dev/null;       cp $(VPSS_COMM_HEADER) $(COPY_INC_PATH); popd > /dev/null
	@-pushd $(RGN_PATH)/include > /dev/null;        cp $(RGN_COMM_HEADER)  $(COPY_INC_PATH); popd > /dev/null
	@-pushd $(CBB_PATH)/include > /dev/null;        cp $(CBB_INC_HEADER)   $(COPY_INC_PATH); popd > /dev/null
	@-pushd $(SECUREC_PATH)/include > /dev/null;    cp $(SECUREC_HEADER)   $(COPY_INC_PATH); popd > /dev/null
	@-pushd $(OSAL_PATH)/include > /dev/null;       cp $(OSAL_INC_HEADER)  $(COPY_INC_PATH); popd > /dev/null
	@-pushd $(DEFINES_PATH) > /dev/null; cp $(DEFINES_HEADER)   $(COPY_INC_PATH); popd > /dev/null
	@-echo "Prepare include file success"
endif
build:mpi pciv_fmw pciv
	@mkdir -p $(REL_INC)
	@-pushd $(PWD)/ext_inc > /dev/null; cp $(MCC_USRDEV_HEADER) $(REL_INC); popd > /dev/null
	@echo "Build pciv all success"
mpi:prepare
	@cd $(PWD)/mpi          && make
	@echo "Compile mpi success"
pciv_fmw:prepare
	@cd $(PWD)/pciv_fmw/src && make
	@echo "Compile pciv_fmw success"
pciv:prepare
	@cd $(PWD)/pciv/src     && make
	@echo "Compile pciv success"

clean:mpi_clean pciv_fmw_clean pciv_clean
ifeq ($(MPP_BUILD), y)
	@mkdir -p $(COPY_INC_PATH)
	@cd $(COPY_INC_PATH) && rm $(CLEAN_COPY_FILE) -f
endif
	@rm $(REL_INC)/$(MCC_USRDEV_HEADER) -f
	@echo "Clean pciv all success"
mpi_clean:
	@cd $(PWD)/mpi          && make clean
pciv_fmw_clean:
	@cd $(PWD)/pciv_fmw/src && make clean
pciv_clean:
	@cd $(PWD)/pciv/src     && make clean
else
all:
	@echo "pciv module not compile"
clean:
	@echo "pciv module don't need clean"
endif
