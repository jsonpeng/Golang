// GwDiscovery.go
package zipapi

/*
//--Note-- : There is cgo comment. DONOT change it

#cgo linux CFLAGS:-I../external-lib/openssl/install/include/openssl  -I../external-lib/openssl/install/include
#cgo linux CFLAGS:-I../external-lib/zware_c_api_v7.02/include 	-I../external-lib/zware_c_api_v7.02/include/zwave
#cgo linux CFLAGS:-I.
#cgo linux CFLAGS:-DOS_LINUX -DZIP_V2 -DUSE_OPENSSL -DCONFIG_DEBUG  -DWKUP_BY_MAILBOX_ACK  -pthread -Wall
#cgo linux LDFLAGS:-L../external-lib/zware_c_api_v7.02/lib -L../external-lib/zware_c_api_v7.02/src -L../external-lib/openssl/install/lib
#cgo linux LDFLAGS:-lzip_ctl -lzip_api -lm -lpthread -lrt -lssl -lcrypto -ldl


#include <Appl.h>

//--Note-- : There must be no blank lines in between the cgo comment and the import statement
*/
import "C"

import (
	"errors"
	"fmt"
)

/*
func nw_init(hl_appl *C.struct_hl_appl_ctx) int {
	var (
		//		zip_gw_addr_str                                      [100]string
		//		host_port                                            uint16 ///< Host listening port
		//		psk_str                                              [384]string
		dtls_psk            []byte ///< DTLS pre-shared key   MAX_DTLS_PSK
		UnknowIpv4, psk_len int
		zip_gw_ip_str       string
	)
	host_port, zip_gw_addr_str, psk_str, err := ConfigParamGet("config.json", 2)
	if err != nil {
		fmt.Println(err)
	}
	psk_len = len(psk_str)
	if psk_len > 0 {
		if psk_len > C.MAX_DTLS_PSK {
			fmt.Println("PSK string length is too long\n")
			return C.ZW_ERR_VALUE
		}
		if psk_len%2 == 0 {
			fmt.Println("PSK string length should be even\n")
			return C.ZW_ERR_VALUE
		}
		//Convert ASCII hexstring to binary string
		dtls_psk, err = HexstringToBin(psk_str, psk_len)
		if err != nil {
			fmt.Println("PSK string is not hex string")
			return C.ZW_ERR_VALUE
		}
	}
	zip_gw_addr := []rune(zip_gw_addr_str)
	zip_gw_ip := []rune(zip_gw_ip_str)
	//Convert IPv4 / IPv6 address string to numeric equivalent
	ret := C.zwnet_ip_aton((*C.char)((unsafe.Pointer)(&zip_gw_addr)), (*C.uint8_t)((unsafe.Pointer)(&zip_gw_ip)), &hl_appl.use_ipv4)

	if ret != 0 {
		fmt.Printf("Invalid Z/IP router IP address:%s\n", zip_gw_addr_str)
		return C.ZW_ERR_IP_ADDR
	}

	fmt.Printf("Z/IP router IP address:%s\n", zip_gw_addr_str)

	//Initialize library
	ret = lib_init(hl_appl, host_port, zip_gw_ip, int(hl_appl.use_ipv4), "./cfg/zwave_device_rec.txt",
		dtls_psk, psk_len/2, nil)

	if ret < 0 {
		fmt.Println("lib_init with error: %d\n", ret)
	}
	return ret

}
func lib_init(hl_appl *C.struct_hl_appl_ctx, host_port uint16, zip_router_ip net.IP, use_ipv4 int, dev_cfg_file_name string,
	dtls_psk []byte, dtls_psk_len uint8, pref_dir string) {

	var result int
	var zw_init C.zwnet_init_t = C.zwnet_init_t{0}

	zw_init.user = hl_appl //high-level application context
	zw_init.node = GoNwNodeCB
	zw_init.notify = GoNwNotifyCB
	zw_init.appl_tx = GoNwTxCB
	zw_init.pref_dir = pref_dir
	zw_init.print_txt_fn = nil
	zw_init.net_info_dir = nil
	zw_init.host_port = host_port
	zw_init.use_ipv4 = use_ipv4
	if use_ipv4 > 0 {
		C.memcpy(zw_init.zip_router, zip_router_ip, net.IPv4len)
	} else {
		C.memcpy(zw_init.zip_router, zip_router_ip, net.IPv6len)
	}

	zw_init.dev_cfg_file = dev_cfg_file_name
	zw_init.dev_cfg_usr = nil
	zw_init.dtls_psk_len = dtls_psk_len
	if dtls_psk_len {
		C.memcpy(zw_init.dtls_psk, dtls_psk, dtls_psk_len)
	}
	//Unhandled command handler
	zw_init.unhandled_cmd = nil

	//Init ZW network
	result = C.zwnet_init(&zw_init, &hl_appl.zwnet)

	if result != 0 {
		fmt.Printf("zwnet_init with error:%d\n", result)

		//Display device configuration file error
		if zw_init.err_loc.dev_ent {
			fmt.Printf("Parsing device configuration file error loc:\n")
			fmt.Printf("Device entry number:%u\n", zw_init.err_loc.dev_ent)
			if zw_init.err_loc.ep_ent {
				fmt.Printf("Endpoint entry number:%u\n", zw_init.err_loc.ep_ent)
			}

			if zw_init.err_loc.if_ent {
				fmt.Printf("Interface entry number:%u\n", zw_init.err_loc.if_ent)
			}
		}
		return result
	}

	return 0
}
*/
func HexstringToBin(str string, psk_len int) ([]byte, error) {
	bytes := make([]byte, len(str))
	for i := 0; i < psk_len; i++ {
		switch {
		case str[i] >= '0' && str[i] <= '9':
			bytes[i] = str[i] - '0'
		case str[i] >= 'a' && str[i] <= 'z':
			bytes[i] = str[i] - 'a' + 10
		case str[i] >= 'A' && str[i] <= 'Z':
			bytes[i] = str[i] - 'A' + 10
		default:
			return nil, errors.New(fmt.Sprintf("invalid hex character: %c", str[i]))
		}
	}
	return bytes, nil
}

/////////////////////////////////////////////////////////////////
func NetworkInit(ctx *C.struct_hl_appl_ctx) error {
	if ctx == nil {
		return errors.New("Point is NULL")
	}
	if C.nw_init(ctx) != 0 {
		return errors.New("Network Init fail")
	}
	fmt.Println("Network start initialization")
	return nil
}

func NetworkExit(ctx *C.struct_hl_appl_ctx) error {
	if ctx == nil {
		return errors.New("Point is NULL")
	}
	C.zwnet_exit(ctx.zwnet)

	fmt.Println("Exit and clean up")
	return nil
}
func NetworkExt() error {

	return NetworkExit(&C.ApplCtx)
}
func NetworkAbort(ctx *C.struct_hl_appl_ctx) error {
	if ctx == nil {
		return errors.New("Point is NULL")
	}
	result := C.zwnet_abort(ctx.zwnet)

	if result == 0 {
		fmt.Printf("Reset network operation aborted.\n")
		ctx.rst_status = C.RESET_NW_STS_UNKNOWN
		ctx.rm_status = C.RM_NODE_STS_UNKNOWN
		ctx.add_status = C.ADD_NODE_STS_UNKNOWN

	} else {
		fmt.Printf("Reset network operation can't be aborted, error:%d\n", result)
	}
	return nil
}

/**
zwnet_abort - abort current action/transmission
@param[in]	net		network
@return		ZW_ERR_XXX
*/
func NetworkReset(ctx *C.struct_hl_appl_ctx) error {
	if ctx == nil {
		return errors.New("Point is NULL")
	}
	result := C.zwnet_reset(ctx.zwnet)

	if result == 0 {
		fmt.Printf("Reset network in progress, please wait for status ...\n")
		ctx.rst_status = C.RESET_NW_STS_PROGRESS
	} else {
		fmt.Printf("Reset network with error:%d\n", result)
	}
	return nil
}

func NwInit() error {
	//	C.ApplCtx.use_ipv4 = 1
	//	return NetworkInit(&C.ApplCtx)
	ret := C.nwInit()
	fmt.Println("network init...: ", ret)
	ret2 := fmt.Sprintln("network init...: ", ret)
	return errors.New(ret2)
}

func NwStatus() error {
	if C.ApplCtx.init_status == 0 {
		return errors.New("Network Initialing...")
	} else {
		return nil
	}
}
