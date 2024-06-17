ifeq ($(EXTDRV_PARAM_FILE), )
    EXTDRV_PARAM_FILE:=../Makefile.param
    include $(EXTDRV_PARAM_FILE)
endif

ifeq ($(MPP_BUILD), y)
    BASE_MKP_HEADER := mod_ext.h
    SYS_EXT_HEADER := sys_ext.h
    DEV_EXT_HEADER := dev_ext.h
    INC_DIR=$(MPP_PATH)/../vendor/$(OT_ARCH)/motionsensor/ext_inc
endif

.PHONY: default clean

ifeq ($(CONFIG_OT_MOTIONFUSION_SUPPORT), y)

default:
ifeq ($(MPP_BUILD), y)
	@rm -f $(INC_DIR)/$(BASE_MKP_HEADER)
	@rm -f $(INC_DIR)/$(SYS_EXT_HEADER)
	@rm -f $(INC_DIR)/$(DEV_EXT_HEADER)
	@cp $(MPP_PATH)/cbb/base/ext_inc/$(BASE_MKP_HEADER) $(INC_DIR)
	@cp $(MPP_PATH)/cbb/sys/ext_inc/$(SYS_EXT_HEADER) $(INC_DIR)
	@cp $(MPP_PATH)/cbb/base/ext_inc/$(DEV_EXT_HEADER) $(INC_DIR)
endif
	@echo -e "\e[0;32;1m--Compiling 'motionsensor_spi'...\e[0;36;1m"
	@@echo -e "\e[0m"
	@cd ./motionsensor_spi && make;
	@echo -e "\e[0;32;1m--Compiling 'motionsensor_mng'...\e[0;36;1m"
	@@echo -e "\e[0m"
	@cd ./motionsensor_mng && make;
	@echo -e "\e[0;32;1m--Compiling 'motionsensor_chip'...\e[0;36;1m"
	@@echo -e "\e[0m"
	@cd ./motionsensor_chip && make;
	@mkdir -p $(REL_INC) && cp -rf include/ot_common_motionsensor.h $(REL_INC)
	@mkdir -p $(REL_INC) && cp -rf include/ot_motionsensor_chip_cmd.h $(REL_INC)

clean:
ifeq ($(MPP_BUILD), y)
	@rm -f $(INC_DIR)/$(BASE_MKP_HEADER)
	@rm -f $(INC_DIR)/$(SYS_EXT_HEADER)
	@rm -f $(INC_DIR)/$(DEV_EXT_HEADER)
endif
	@cd ./motionsensor_spi && make clean;
	@cd ./motionsensor_mng && make clean;
	@cd ./motionsensor_chip && make clean;
	@rm -f $(REL_INC)/ot_common_motionsensor.h
	@rm -f $(REL_INC)/ot_motionsensor_chip_cmd.h

#CONFIG_OT_MOTIONFUSION_SUPPORT endif
else
all:
	@echo "motionsensor module not compile"
clean:
	@echo "motionsensor module don't need clean"
endif
