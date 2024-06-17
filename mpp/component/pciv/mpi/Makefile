# $(ROOT)/mpp/code/component/pci/pciv/Makefile
ifeq ($(PARAM_FILE), )
    PARAM_FILE:=../../../Makefile.param
    include $(PARAM_FILE)
endif
PCIV_ROOT := $(PWD)/..
INC_PATH  := -I$(PCIV_ROOT)/pciv/include
INC_PATH  += -I$(PCIV_ROOT)/copy_inc
INC_PATH  += -I$(PCIV_ROOT)/include
ifneq ($(MPP_BUILD), y)
INC_PATH  += -I$(REL_INC)
endif

ifeq ($(OT_GDB),y)
FLAGS += -Wall -c -g
else
FLAGS += -Wall -c -O2
endif

FLAGS += -I. $(INC_PATH)

define maco_cp_file
    if [ ! -z "$(1)" ];then cp -rf $(1) "$(2)"; fi
endef

FLAGS += -fstack-protector-strong -fPIC
LIBS_LD_CFLAGS += -Wl,-z,relro -Wl,-z,noexecstack -Wl,-z,now,-s -fPIC
FLAGS += $(MPP_CFLAGS)
FLAGS += $(LIBS_CFLAGS)
#*************************************************************************
# release header
INC_FILE  := $(wildcard $(PCIV_ROOT)/include/*.h)
MPI_SRC := $(PWD)/ot_mpi_pciv.c
BUILD_DIR = $(TEMP_PATH)/$(subst $(SDK_PATH)/,,$(CURDIR))

all:$(MPP_OBJ)
	@echo -e "\e[0;32;1m--Compiling libpciv... Configs as follow:\e[0;36;1m"
	@echo "---- USER_CROSS=$(CROSS)      KERNEL_CROSS=$(CROSS_COMPILE)"
	@echo ---- OT_ARCH=$(OT_ARCH), ARCH=$(ARCH), OT_CHIP=$(OT_CHIP), CVER=$(CVER), DBG=$(OT_DBG), OT_FPGA=$(OT_FPGA)
	@echo ---- CPU_TYPE=$(CPU_TYPE)
	@echo ---- FLAGS=$(FLAGS)
	@echo ---- KERNEL_ROOT=$(KERNEL_ROOT)
	@echo ---- INC_PATH=$(INC_PATH)
	@echo ---- INC_FILE=$(INC_FILE) $(ADAPT_INC_FILE)
	@@echo -e "\e[0m"
	@mkdir -p $(BUILD_DIR)
	@mkdir -p $(REL_LIB)
	@mkdir -p $(REL_INC) && $(call maco_cp_file,$(INC_FILE),$(REL_INC))
	@$(CC) -o $(BUILD_DIR)/mpi_pciv.o  $(MPI_SRC) $(FLAGS) $(MPI_CFLAGS)
	@$(CC) $(LIBS_LD_CFLAGS) -shared -o $(REL_LIB)/libot_pciv.so $(BUILD_DIR)/mpi_pciv.o
	@$(AR) -rsv $(REL_LIB)/libot_pciv.a $(BUILD_DIR)/mpi_pciv.o

clean:
	@rm -f $(BUILD_DIR)/mpi_pciv.o $(REL_LIB)/libot_pciv.a $(REL_LIB)/libot_pciv.so
	@rm -f $(REL_INC)/ot_mpi_pciv.h $(REL_INC)/ot_common_pciv.h

