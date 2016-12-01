################################################################################
# Makefile to make libraries
################################################################################

TARGET_PLATFORM = LINUX
TARGET_PRODUCT = ZIP_CLIENT
CFLAG :=

ifeq ($(TARGET_PRODUCT),PORTAL_OPENSSL)
	CFLAG += -DTCP_PORTAL -DUSE_OPENSSL
endif

ifeq ($(TARGET_PLATFORM),OPENWRT)
	MAKE_PARAM_CC = 'CC=mips-openwrt-linux-gcc'
	MAKE_PARAM_AR = 'AR=mips-openwrt-linux-ar'
endif

ifeq ($(TARGET_PLATFORM),BEAGLEBONE)
	MAKE_PARAM_CC = 'CC=arm-linux-gnueabihf-gcc'
	MAKE_PARAM_AR = 'AR=arm-linux-gnueabihf-ar'
endif

ifeq ($(TARGET_PLATFORM),OSX)
	CFLAG += -DOS_MAC_X -arch i386
endif

ifeq ($(TARGET_PLATFORM),SIGMA)
	MAKE_PARAM_CC = 'CC=mipsel-linux-gcc'
	MAKE_PARAM_AR = 'AR=mipsel-linux-ar'
	CFLAG += -DSUPPORT_SIMPLE_AV_CONTROL -fPIC
endif

ifeq ($(TARGET_PLATFORM),VANTAGE)
        MAKE_PARAM_CC = 'CC=arm-none-linux-gnueabi-gcc'
        MAKE_PARAM_AR = 'AR=arm-none-linux-gnueabi-ar'
	CFLAG += -DSUPPORT_SIMPLE_AV_CONTROL -fPIC
endif

ifeq ($(TARGET_PLATFORM),LG1152)
	MAKE_PARAM_CC = 'CC=arm-lg1152-linux-gnueabi-gcc'
	MAKE_PARAM_AR = 'AR=arm-lg1152-linux-gnueabi-ar'
	CFLAG += -DSUPPORT_SIMPLE_AV_CONTROL -fPIC
endif

ifeq ($(TARGET_PLATFORM),ANDROID)
	MAKE_PARAM_CC = 'CC=arm-linux-androideabi-gcc'
	MAKE_PARAM_AR = 'AR=arm-linux-androideabi-ar'
	CFLAG += -DOS_ANDROID
endif

ifeq ($(TARGET_PLATFORM),LINUX_ZIP2)
    ifeq ($(SINGLE_CLIENT),1)
	CFLAG += -DZIP_V2 -DUSE_OPENSSL -pthread
    else
	CFLAG += -DWKUP_BY_MAILBOX_ACK -DZIP_V2 -DUSE_OPENSSL -pthread
    endif
endif

ifeq ($(TARGET_PLATFORM),VANTAGE_ZIP2)
        MAKE_PARAM_CC = 'CC=arm-none-linux-gnueabi-gcc'
        MAKE_PARAM_AR = 'AR=arm-none-linux-gnueabi-ar'
	CFLAG += -DSUPPORT_SIMPLE_AV_CONTROL -fPIC
	CFLAG += -DZIP_V2 -DUSE_OPENSSL -pthread
endif

ifeq ($(TARGET_PLATFORM),SIGMA_ZIP2)
	MAKE_PARAM_CC = 'CC=mipsel-linux-gcc'
	MAKE_PARAM_AR = 'AR=mipsel-linux-ar'
	CFLAG += -DSUPPORT_SIMPLE_AV_CONTROL -fPIC
	CFLAG += -DZIP_V2 -DUSE_OPENSSL -pthread
endif

ifeq ($(TARGET_PLATFORM),ANDROID_ZIP2)
	MAKE_PARAM_CC = 'CC=arm-linux-androideabi-gcc'
	MAKE_PARAM_AR = 'AR=arm-linux-androideabi-ar'
	CFLAG += -DOS_ANDROID
	CFLAG += -DZIP_V2 -DUSE_OPENSSL -pthread
endif

ifeq ($(TARGET_PLATFORM),OPENWRT_ZIP2)
	MAKE_PARAM_CC = 'CC=mips-openwrt-linux-gcc'
	MAKE_PARAM_AR = 'AR=mips-openwrt-linux-ar'
	CFLAG += -DZIP_V2 -DUSE_OPENSSL -pthread
endif

ifeq ($(TARGET_PLATFORM),BEAGLEBONE_ZIP2)
	MAKE_PARAM_CC = 'CC=arm-linux-gnueabihf-gcc'
	MAKE_PARAM_AR = 'AR=arm-linux-gnueabihf-ar'
    ifeq ($(SINGLE_CLIENT),1)
	CFLAG += -DZIP_V2 -DUSE_OPENSSL -pthread
    else
	CFLAG += -DWKUP_BY_MAILBOX_ACK -DZIP_V2 -DUSE_OPENSSL -pthread
    endif
endif

ifeq ($(TARGET_PLATFORM),DLINK_ZIP2)
	MAKE_PARAM_CC = 'CC=mipsel-linux-gcc'
	MAKE_PARAM_AR = 'AR=mipsel-linux-ar'
	CFLAG += -fPIC
	CFLAG += -DZIP_V2 -DUSE_OPENSSL -pthread
endif

ifeq ($(TARGET_PLATFORM),OSX_ZIP2)
	CFLAG += -DOS_MAC_X -arch i386 -DZIP_V2 -DUSE_OPENSSL -pthread
endif

ifeq ($(DEBUG),1)
	CFLAG += -DCONFIG_DEBUG
endif


MAKE_PARAM_CFLAG = 'CFLAGS= $(CFLAG)'

# All Target
all::

all \
clean ::
	@cd lib && ${MAKE} ${MAKE_PARAM_CC} ${MAKE_PARAM_CFLAG} ${MAKE_PARAM_AR} $@
	@cd src && ${MAKE} ${MAKE_PARAM_CC} ${MAKE_PARAM_CFLAG} ${MAKE_PARAM_AR} $@
	@cd demos &&		$(MAKE) -C add_node ${MAKE_PARAM_CC} ${MAKE_PARAM_CFLAG} ${MAKE_PARAM_AR} $@
	@cd demos && 		$(MAKE) -C basic ${MAKE_PARAM_CC} ${MAKE_PARAM_CFLAG} ${MAKE_PARAM_AR} $@
	@cd demos && 		$(MAKE) -C bin_switch ${MAKE_PARAM_CC} ${MAKE_PARAM_CFLAG} ${MAKE_PARAM_AR} $@
	@cd demos && 		$(MAKE) -C gw_discovery ${MAKE_PARAM_CC} ${MAKE_PARAM_CFLAG} ${MAKE_PARAM_AR} $@
	@cd demos && 		$(MAKE) -C nw_reset ${MAKE_PARAM_CC} ${MAKE_PARAM_CFLAG} ${MAKE_PARAM_AR} $@
	@cd demos && 		$(MAKE) -C rm_node ${MAKE_PARAM_CC} ${MAKE_PARAM_CFLAG} ${MAKE_PARAM_AR} $@
.PHONY: all clean
