################################################################################
# Makefile to make zipctl library
################################################################################

CC=gcc
AR=ar
CFLAGS=

RM := rm -rf

LIB_OBJS = \
zip_api_base.o \
zip_api_intf.o \
zip_api_network.o \
zip_api_sm.o \
zip_api_util.o \
zip_poll.o \
zip_set_poll.o \
zip_dev_poll.o \
zip_wkup_poll.o \
cjson.o \
zip_dev_cfg.o \
zip_network_save.o

LIB_HEADERS = \
 ../include/zip_error.h ../include/zip_platform.h \
 ../include/zip_util.h ../include/zip_transport.h \
 ../include/zip_error.h ../include/zip_platform.h \
 ../include/zip_util.h ../include/zip_frame.h \
 ../include/zip_transport.h ../include/zip_session.h \
 ../include/zip_tls_server.h \
 ../include/zip_frame.h ../include/zip_application.h \
 ../include/zip_session.h ../include/zip_api_pte.h \
 ../include/zip_api.h ../include/zip_api_util.h \
 ../include/zip_poll.h ../include/zip_set_poll.h \
 ../include/zip_dev_poll.h ../include/zip_wkup_poll.h \
 ../include/zwave/ZW_controller_api.h \
 ../include/cjson.h \
 ../include/zip_network_save.h \
 ../include/zwave/ZW_transport_api.h ../include/zwave/ZW_classcmd.h

ifneq (,$(findstring CONFIG_DEBUG,$(CFLAGS)))
	COMPILE_OPTION = -g -O0
else
	COMPILE_OPTION = -O2
endif

INCLUDE_PATH =
ifneq (,$(findstring DUSE_OPENSSL,$(CFLAGS)))
INCLUDE_PATH += -I../../openssl/install/include/openssl  -I../../openssl/install/include
endif

# All Target
all: libzip_ctl.a

# Compile c source file
%.o: %.c $(LIB_HEADERS)
	@echo 'Compiling file: $<'
	$(CC) $(COMPILE_OPTION) $(INCLUDE_PATH) -Wall -DOS_LINUX $(CFLAGS) -c -o"$@" "$<"
	@echo 'Finished compiling: $<'
	@echo ' '

# Tool invocations
libzip_ctl.a: $(LIB_OBJS)
	@echo 'Building target: $@'
	$(AR) -r "libzip_ctl.a" $(LIB_OBJS)
	@echo 'Finished building target: $@'
	@echo ' '


# Other Targets
clean:
	-$(RM) *.o
	-$(RM) libzip_ctl.a
	-@echo ' '

.PHONY: all clean
