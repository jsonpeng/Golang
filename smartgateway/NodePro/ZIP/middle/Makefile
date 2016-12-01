################################################################################
# Makefile to make sample application
################################################################################

CC=gcc
CFLAGS=
LDFLAGS=

RM := rm -rf

LIBS := -lzip_ctl -lzip_api -lm

ifneq (,$(findstring DOS_MAC_X,$(CFLAGS)))
	LDFLAGS = -arch i386
	LIBS += -lpthread
else ifneq (,$(findstring DOS_ANDROID,$(CFLAGS)))
	LDFLAGS =
else
	LIBS += -lpthread -lrt
endif

ifneq (,$(findstring CONFIG_DEBUG,$(CFLAGS)))
	COMPILE_OPTION = -g -O0
else
	COMPILE_OPTION = -O2
endif


SRC_OBJS = add_node.o

LIB_FILES = \
../../src/libzip_ctl.a \
../../lib/libzip_api.a

LIB_PATH = -L"../../lib" -L"../../src"

SRC_HEADERS = \
../../include/zip_platform.h ../../include/zip_util.h \
../../include/zip_transport.h ../../include/zip_error.h \
../../include/zip_platform.h ../../include/zip_util.h \
../../include/zip_frame.h ../../include/zip_transport.h \
../../include/zip_tls_server.h \
../../include/zip_session.h ../../include/zip_frame.h \
../../include/zip_application.h ../../include/zip_session.h \
../../include/zip_api.h ../../include/zwave/ZW_transport_api.h \
../../include/zwave/ZW_classcmd.h

INCLUDE_PATH =

ifneq (,$(findstring DZIP_V2,$(CFLAGS)))
	LIBS += -lssl -lcrypto -ldl
	LIB_FILES += ../../../openssl/install/lib/libssl.a ../../../openssl/install/lib/libcrypto.a
	LIB_PATH += -L"../../../openssl/install/lib"
	INCLUDE_PATH += -I../../../openssl/install/include/openssl  -I../../../openssl/install/include
endif

TARGET_BIN = add_node


# All Target
all: $(TARGET_BIN)

# Compile c source file
%.o: %.c $(SRC_HEADERS)
	@echo 'Compiling file: $<'
	$(CC) $(COMPILE_OPTION) $(INCLUDE_PATH) -Wall -DOS_LINUX $(CFLAGS) -c -o"$@" "$<"
	@echo 'Finished compiling: $<'
	@echo ' '

# Tool invocations
$(TARGET_BIN): $(SRC_OBJS) $(LIB_FILES)
	@echo 'Building target: $@'
	$(CC) $(LIB_PATH) $(LDFLAGS) -o $(TARGET_BIN) $(SRC_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '


# Other Targets
clean:
	-$(RM) *.o
	-$(RM) $(TARGET_BIN)
	-@echo ' '

.PHONY: all clean
