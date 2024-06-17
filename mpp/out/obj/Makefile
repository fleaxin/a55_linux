#*******************************************************************************
#The rule of make Linux kernel module
#*******************************************************************************
ifeq ($(PARAM_FILE), )
    PARAM_FILE:=../../../mpp/Makefile.param
    include $(PARAM_FILE)
endif

INIT_ROOT = $(REL_DIR)/init

EXTRA_CFLAGS += $(MPP_CFLAGS)
EXTRA_CFLAGS += -I$(REL_INC)
EXTRA_CFLAGS += -I$(INIT_ROOT)/include
EXTRA_CFLAGS += -I$(INIT_ROOT)/include/npu
EXTRA_CFLAGS += -I$(INIT_ROOT)/include/svp_npu
EXTRA_CFLAGS += -I$(OSAL_ROOT)/include
EXTRA_CFLAGS += -I$(REL_DIR)/../component/gfbg/drv/include

$(KO_PREFIX)_acodec-objs =  mod_acodec.o ../init/src/acodec_init.o
$(KO_PREFIX)_adec-objs =  mod_adec.o ../init/src/adec_init.o
$(KO_PREFIX)_aenc-objs  = mod_aenc.o  ../init/src/aenc_init.o
$(KO_PREFIX)_ai-objs  = mod_ai.o  ../init/src/ai_init.o
$(KO_PREFIX)_aio-objs  = mod_aio.o  ../init/src/aio_init.o
$(KO_PREFIX)_ao-objs  = mod_ao.o  ../init/src/ao_init.o
$(KO_PREFIX)_avs-objs  = mod_avs.o  ../init/src/avs_init.o
$(KO_PREFIX)_base-objs = mod_base.o  ../init/src/base_init.o
$(KO_PREFIX)_chnl-objs  = mod_chnl.o  ../init/src/chnl_init.o
$(KO_PREFIX)_dis-objs  = mod_dis.o  ../init/src/dis_init.o
$(KO_PREFIX)_dpu_match-objs  = mod_dpu_match.o  ../init/src/dpu_match_init.o
$(KO_PREFIX)_dpu_rect-objs  = mod_dpu_rect.o  ../init/src/dpu_rect_init.o
$(KO_PREFIX)_gdc-objs  = mod_gdc.o  ../init/src/gdc_init.o
$(KO_PREFIX)_gyrodis-objs  = mod_gyrodis.o  ../init/src/gyrodis_init.o
$(KO_PREFIX)_h264e-objs  = mod_h264e.o  ../init/src/h264e_init.o
$(KO_PREFIX)_h265e-objs = mod_h265e.o  ../init/src/h265e_init.o
$(KO_PREFIX)_hdmi-objs  = mod_hdmi.o   ../init/src/hdmi_init.o
$(KO_PREFIX)_isp-objs  = mod_isp.o   ../init/src/isp_init.o
$(KO_PREFIX)_ive-objs  = mod_ive.o    ../init/src/ive_init.o
$(KO_PREFIX)_jpegd-objs = mod_jpegd.o  ../init/src/jpegd_init.o
$(KO_PREFIX)_jpege-objs  = mod_jpege.o  ../init/src/jpege_init.o
$(KO_PREFIX)_mau-objs  = mod_mau.o   ../init/src/mau_init.o
$(KO_PREFIX)_mcf-objs  = mod_mcf.o   ../init/src/mcf_init.o
$(KO_PREFIX)_rc-objs  = mod_rc.o   ../init/src/rc_init.o
$(KO_PREFIX)_rgn-objs = mod_rgn.o  ../init/src/rgn_init.o
$(KO_PREFIX)_sys-objs = mod_sys.o    ../init/src/sys_init.o
$(KO_PREFIX)_tde-objs  = mod_tde.o   ../init/src/tde_init.o
$(KO_PREFIX)_vdec-objs = mod_vdec.o  ../init/src/vdec_init.o
$(KO_PREFIX)_vedu-objs  = mod_vedu.o  ../init/src/vedu_init.o
$(KO_PREFIX)_venc-objs = mod_venc.o  ../init/src/venc_init.o
$(KO_PREFIX)_vfmw-objs  = mod_vfmw.o  ../init/src/vfmw_init.o
$(KO_PREFIX)_vgs-objs = mod_vgs.o   ../init/src/vgs_init.o
$(KO_PREFIX)_vi-objs  = mod_vi.o  ../init/src/vi_init.o
$(KO_PREFIX)_vo-objs = mod_vo.o  ../init/src/vo_init.o
$(KO_PREFIX)_vpp-objs = mod_vpp.o  ../init/src/vpp_init.o
$(KO_PREFIX)_vpss-objs  = mod_vpss.o  ../init/src/vpss_init.o
$(KO_PREFIX)_motionfusion-objs  = mod_motionfusion.o  ../init/src/motionfusion_init.o
$(KO_PREFIX)_npu_device-objs  = ./npu/mod_npu_device.o  ../init/src/npu/npudev_init.o
$(KO_PREFIX)_npu_aicpu-objs  = ./npu/mod_npu_aicpu.o
$(KO_PREFIX)_npu_dfx-objs  = ./npu/mod_npu_dfx.o
$(KO_PREFIX)_npu_tsfw-objs  = ./npu/mod_npu_tsfw.o
$(KO_PREFIX)_svp_npu-objs  = ./svp_npu/mod_svp_npu.o  ../init/src/svp_npu/svp_npu_init.o
$(KO_PREFIX)_dsp-objs  = mod_dsp.o    ../init/src/dsp_init.o
$(KO_PREFIX)_photo-objs  = mod_photo.o    ../init/src/photo_init.o
$(KO_PREFIX)_uvc-objs = mod_uvc.o  ../init/src/uvc_init.o


obj-m += $(KO_PREFIX)_acodec.o
obj-m += $(KO_PREFIX)_adec.o
obj-m += $(KO_PREFIX)_aenc.o
obj-m += $(KO_PREFIX)_ai.o
obj-m += $(KO_PREFIX)_aio.o
obj-m += $(KO_PREFIX)_ao.o
obj-m += $(KO_PREFIX)_avs.o
obj-m += $(KO_PREFIX)_base.o
obj-m += $(KO_PREFIX)_chnl.o
obj-m += $(KO_PREFIX)_dis.o
obj-m += $(KO_PREFIX)_dpu_match.o
obj-m += $(KO_PREFIX)_dpu_rect.o
obj-m += $(KO_PREFIX)_gdc.o
obj-m += $(KO_PREFIX)_gyrodis.o
obj-m += $(KO_PREFIX)_h264e.o
obj-m += $(KO_PREFIX)_h265e.o
obj-m += $(KO_PREFIX)_hdmi.o
obj-m += $(KO_PREFIX)_isp.o
obj-m += $(KO_PREFIX)_ive.o
obj-m += $(KO_PREFIX)_jpegd.o
obj-m += $(KO_PREFIX)_jpege.o
obj-m += $(KO_PREFIX)_mau.o
obj-m += $(KO_PREFIX)_mcf.o
obj-m += $(KO_PREFIX)_rc.o
obj-m += $(KO_PREFIX)_rgn.o
obj-m += $(KO_PREFIX)_sys.o
obj-m += $(KO_PREFIX)_tde.o
obj-m += $(KO_PREFIX)_vdec.o
obj-m += $(KO_PREFIX)_vedu.o
obj-m += $(KO_PREFIX)_venc.o
obj-m += $(KO_PREFIX)_vfmw.o
obj-m += $(KO_PREFIX)_vgs.o
obj-m += $(KO_PREFIX)_vi.o
obj-m += $(KO_PREFIX)_vo.o
obj-m += $(KO_PREFIX)_vpp.o
obj-m += $(KO_PREFIX)_vpss.o
obj-m += $(KO_PREFIX)_motionfusion.o
obj-m += $(KO_PREFIX)_npu_device.o
obj-m += $(KO_PREFIX)_npu_aicpu.o
obj-m += $(KO_PREFIX)_npu_dfx.o
obj-m += $(KO_PREFIX)_npu_tsfw.o
obj-m += $(KO_PREFIX)_svp_npu.o
obj-m += $(KO_PREFIX)_dsp.o
obj-m += $(KO_PREFIX)_photo.o
obj-m += $(KO_PREFIX)_uvc.o


default:
	@echo -e "\e[0;32;1m--Compiling '$(TARGET)'... Configs as follow:\e[0;36;1m"
	@echo ---- CROSS=$(CROSS) REL_INC=$(REL_INC)
	@echo ---- OT_SUBARCH=$(OT_SUBARCH), CVER=$(CVER), DBG=$(OT_DBG), OT_FPGA=$(OT_FPGA)
	@echo ---- CPU_TYPE=$(CPU_TYPE) REL_KO=$(REL_KO)
	@echo ---- EXTRA_CFLAGS=$(EXTRA_CFLAGS)
	@echo ---- SDK_PATH=$(SDK_PATH) , PARAM_FILE=$(PARAM_FILE)
	@echo ---- KERNEL_ROOT=$(KERNEL_ROOT)
	@echo ---- ARCH_ROOT=$(ARCH_ROOT), ARCH_HAL=$(ARCH_HAL)
	@echo ---- obj-m=$(obj-m)
	@@echo -e "\e[0m"
	@cd $(SDK_PATH)/mpp/component/securec; make -j
	@cd $(SDK_PATH)/osal/$(OSTYPE); make -j
	@cd $(SDK_PATH)/mpp/cbb/isp; make -j
	@cd $(SDK_PATH)/mpp/component/gfbg; make -j
	@cd $(SDK_PATH)/mpp/component/pciv; make -j
	@cd $(SDK_PATH)/mpp/component/security_subsys; make -j
	@cd $(SDK_PATH)/interdrv; make -j
	@cd $(SDK_PATH)/vendor; make -j
	@$(MAKE) -C $(KERNEL_ROOT) M=$(PWD) V=10 modules -j 24
	@$(CROSS)strip --strip-unneeded *.ko
	@mkdir -p $(REL_KO)/npu && mkdir -p $(REL_KO)/svp_npu
	@cp -rf *.ko $(REL_KO) && mv $(REL_KO)/ot_npu* $(REL_KO)/npu && mv $(REL_KO)/ot_svp_npu* $(REL_KO)/svp_npu
	@rm $(REL_INC)/ot_mpi*
	@rm $(REL_LIB)/libot_*
	@cp $(SDK_PATH)/mpp/cbb/isp/user/lib/libot_isp.* $(REL_LIB)

clean:
	@rm -f *.ko
	@rm .tmp_versions  -rf
	@rm Module.symvers -f
	@rm modules.order  -f
	@rm .*.cmd  -f
	@rm *.mod.c  -f
	@rm *.mod.o  -f
	@rm $(KO_PREFIX)_*.o  -f
	@rm ../init/src/*.o  -f
	@rm ../init/src/.*.cmd  -f
	@rm ../init/src/npu/*.o  -f
	@rm ../init/src/npu/.*.cmd  -f
	@cd $(SDK_PATH)/mpp/component/securec; make clean
	@cd $(SDK_PATH)/osal/$(OSTYPE); make clean
	@cd $(SDK_PATH)/mpp/cbb/isp; make clean
	@cd $(SDK_PATH)/mpp/component/gfbg; make clean
	@cd $(SDK_PATH)/mpp/component/pciv; make clean
	@cd $(SDK_PATH)/mpp/component/security_subsys; make clean
	@cd $(SDK_PATH)/interdrv; make clean
	@cd $(SDK_PATH)/vendor; make clean
