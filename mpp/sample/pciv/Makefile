
include ../Makefile.param

MPI_LIBS +=$(REL_LIB)/libss_pciv.a
MPI_LIBS += $(REL_LIB)/libss_hdmi.a

# target source
SRC_HOST  := sample_pciv_host.c pciv_trans.c pciv_msg.c
OBJS_HOST  := $(SRC_HOST:%.c=%.o)
PCIV_HOST := sample_pciv_host
SRC_SLAVE  := sample_pciv_slave.c pciv_trans.c pciv_msg.c
OBJS_SLAVE  := $(SRC_SLAVE:%.c=%.o)
PCIV_SLAVE := sample_pciv_slave
# CFLAGS += -Wall -g
# CFLAGS += -lpthread -lm
.PHONY : clean all host slave
CFLAGS += $(COMM_INC)

all: host slave

host: $(PCIV_HOST)
$(PCIV_HOST): $(OBJS_HOST) $(COMM_OBJ)
	@$(CC) $(CFLAGS) $(LIBS_LD_CFLAGS) -o $@ $^ -Wl,--start-group $(MPI_LIBS) $(SENSOR_LIBS) $(AUDIO_LIBA) $(REL_LIB)/libsecurec.a -Wl,--end-group

slave: $(PCIV_SLAVE)
$(PCIV_SLAVE): $(OBJS_SLAVE) $(COMM_OBJ)
	@$(CC) $(CFLAGS) $(LIBS_LD_CFLAGS) -o $@ $^ -Wl,--start-group $(MPI_LIBS) $(SENSOR_LIBS) $(AUDIO_LIBA) $(REL_LIB)/libsecurec.a -Wl,--end-group

clean:
	@rm sample_pciv_host sample_pciv_slave *.o -fr

cleanstream:
	@rm -f *.h264
	@rm -f *.h265
	@rm -f *.jpg
	@rm -f *.mjp
	@rm -f *.mp4

