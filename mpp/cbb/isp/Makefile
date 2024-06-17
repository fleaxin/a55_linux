ifeq ($(PARAM_FILE), )
    PARAM_FILE:=../../Makefile.param
    include $(PARAM_FILE)
endif

ifeq ($(CONFIG_OT_ISP_SUPPORT), y)
ISP_INC := ./include
ISP_ARCH_INC := ./include/arch/$(OT_ARCH)/include
3A_INC := ./user/3a/include
PWM_INC := ./ext_drv/$(OT_ARCH)/pwm

.PHONY: clean all show kernel user extdrv

all: show kernel user extdrv

show:
	@echo -e "\e[0;32;1m--Compiling 'isp'... Configs as follow:\e[0;36;1m"
	@echo ---- CROSS=$(CROSS)
	@echo ---- OT_ARCH=$(OT_ARCH), OT_CHIP=$(OT_CHIP), CVER=$(CVER), OT_DBG=$(OT_DBG)
	@echo ---- SDK_PATH=$(SDK_PATH) , PARAM_FILE=$(PARAM_FILE)
	@echo ---- KERNEL_ROOT=$(KERNEL_ROOT)
	@echo -e "\e[0m"
	@mkdir -p $(REL_INC); cp $(ISP_INC)/*.h $(ISP_ARCH_INC)/*.h $(3A_INC)/*.h $(PWM_INC)/pwm.h  $(REL_INC) -f ;
kernel:
	@cd kernel; make

user:
	@cd user; make

extdrv:
	@cd ext_drv/$(OT_ARCH); make

clean:
	@cd kernel; make clean
	@cd user; make clean
	@cd ext_drv/$(OT_ARCH); make clean

else
all:
	@echo "isp module not compile"
clean:
	@echo "isp module don't need clean"
endif
