################################################################################
# Makefile to make zip api library
################################################################################

CC=gcc
AR=ar
CFLAGS=


RM := rm -rf

LIB_OBJS = \
zip_application.o \
zip_frame.o \
zip_platform.o \
zip_session.o \
zip_transport.o \
zip_util.o


ifneq (,$(findstring DTCP_PORTAL,$(CFLAGS)))
	LIB_OBJS += zip_tls_server.o
endif

ifneq (,$(findstring DOS_ANDROID,$(CFLAGS)))
	LIB_OBJS += zip_netutil.o
endif

# Z/IP version 2
ifneq (,$(findstring DZIP_V2,$(CFLAGS)))
	LIB_OBJS += zip_dtls.o zip_mdns.o
endif

LIB_HEADERS = \
../include/zip_application.h ../include/zip_session.h \
../include/zip_frame.h ../include/zip_transport.h \
../include/zip_tls_server.h \
../include/zip_error.h ../include/zip_platform.h \
../include/zip_util.h ../include/zip_mdns.h ../include/zip_dtls.h ../include/zwave/ZW_transport_api.h \
../include/zwave/ZW_classcmd.h

ifneq (,$(findstring CONFIG_DEBUG,$(CFLAGS)))
	COMPILE_OPTION = -g -O0
else
	COMPILE_OPTION = -O2
endif

ifneq (,$(findstring DUSE_OPENSSL,$(CFLAGS)))
INCLUDE_PATH =  \
-I../../openssl/install/include/openssl  -I../../openssl/install/include
else ifneq (,$(findstring DTCP_PORTAL,$(CFLAGS)))
INCLUDE_PATH =  \
-I../../axTLS/ssl/  \
-I../../axTLS/crypto/ \
-I../../axTLS/config
COMPILE_OPTION += -DCERT_SERIAL_NUM -DSSL_TXRX_LOCK
else ifneq (,$(findstring DZIP_V2,$(CFLAGS)))
INCLUDE_PATH =  \
-I../../cyassl-3.2.0/install/include/
endif


# All Target
all: libzip_api.a

# Compile c source file
%.o: %.c $(LIB_HEADERS)
	@echo 'Compiling file: $<'
	$(CC) $(COMPILE_OPTION) $(INCLUDE_PATH) -Wall -DOS_LINUX $(CFLAGS) -c -o"$@" "$<"
	@echo 'Finished compiling: $<'
	@echo ' '

# Tool invocations
libzip_api.a: $(LIB_OBJS)
	@echo 'Building target: $@'
	$(AR) -r "libzip_api.a" $(LIB_OBJS)
	@echo 'Finished building target: $@'
	@echo ' '


# Other Targets
clean:
	-$(RM) *.o
	-$(RM) libzip_api.a
	-@echo ' '

.PHONY: all clean
