// GwDiscovery.go
package zipapi

/*
//--Note-- : There is cgo comment. DONOT change it

#cgo linux CFLAGS:-I../external/openssl/install/include/openssl  -I../external/openssl/install/include
#cgo linux CFLAGS:-I../external/zware_c_api_v7.02/include 	-I../external/zware_c_api_v7.02/include/zwave
#cgo linux CFLAGS:-I.
#cgo linux CFLAGS:-DOS_LINUX -DZIP_V2 -DUSE_OPENSSL -DCONFIG_DEBUG  -DWKUP_BY_MAILBOX_ACK  -pthread -Wall
#cgo linux LDFLAGS:-L../external/zware_c_api_v7.02/lib -L../external/zware_c_api_v7.02/src -L../external/openssl/install/lib
#cgo linux LDFLAGS:-L/home/eric/GoPack/src/github.com/portus/SmartGateway/NodePro/ZIP/zipapi
#cgo linux LDFLAGS:-lzip_ctl -lzip_api -lm -lpthread -lrt -lssl -lcrypto -ldl


#include <Appl.h>

//--Note-- : There must be no blank lines in between the cgo comment and the import statement
*/
import "C"

import (
	"errors"
	"fmt"
	"net"
	"time"
	"unsafe"
)

func GwDiscovery(ipv4 bool) (C.struct_hl_appl_ctx, []net.IPAddr, error) {
	var ipDataString [1600]uint8
	var p *uint8 = &ipDataString[0]
	var iplen int
	var gw_discvr_ctx unsafe.Pointer

	appl_ctx := C.struct_hl_appl_ctx{}

	var ipslice []net.IPAddr
	if ipv4 {
		//Init user-application
		appl_ctx.use_ipv4 = 1 //Use IPv4
		iplen = net.IPv4len
		fmt.Println("Scanning for Z/IP gateway with IPv4 ...")
	} else {
		//Init user-application
		appl_ctx.use_ipv4 = 0 //Use IPv6
		iplen = net.IPv6len
		fmt.Println("Scanning for Z/IP gateway with IPv6 ...")
	}

	//Start gateway discovery
	gw_discvr_ctx = C.zwnet_gw_discvr_start((C.util_gw_discvr_cb_t)(unsafe.Pointer(C.gw_discvr_cb)), unsafe.Pointer(&appl_ctx), appl_ctx.use_ipv4, 0)
	//gw_discvr_ctx = C.zwnet_gw_discvr_start(C.gw_discvr_cb, unsafe.Pointer(&appl_ctx), appl_ctx.use_ipv4, 0)
	if gw_discvr_ctx == nil {
		fmt.Println("gw_discvr_ctx is nil")
		return appl_ctx, ipslice, errors.New("Discovery contex build fail")
	}

	fmt.Println(time.Now(), "Wait for 7 seconds")
	time.Sleep(20 * time.Second)
	fmt.Println(time.Now())
	fmt.Println("Timeup, stop gw disr")

	//Stop gateway discovery
	C.zwnet_gw_discvr_stop(gw_discvr_ctx)

	if appl_ctx.gw_addr_cnt == 0 {
		return appl_ctx, ipslice, errors.New("Timeout, no gateway found")
	}
	//save the ip data
	C.get_addr_buf(appl_ctx.gw_addr_buf, (C.uint16_t)(appl_ctx.gw_addr_cnt*(C.uint8_t)(iplen)), (*C.uint8_t)(p))

	for i := 0; i < (int)((C.uint16_t)(appl_ctx.gw_addr_cnt*(C.uint8_t)(iplen))); i = i + iplen {
		temp := ipDataString[i : i+iplen]
		ip := net.IP(temp)
		ipaddr := net.IPAddr{}
		ipaddr.IP = ip
		ipslice = append(ipslice, ipaddr)
	}
	ipslice = RmDup(ipslice)
	err := ConfigParamNew("app.cfg", ipslice)
	fmt.Println("Error: ", err)
	return appl_ctx, ipslice, nil
}

// remove the duplicate ipaddress
func RmDup(list []net.IPAddr) []net.IPAddr {
	var x []net.IPAddr = []net.IPAddr{}
	for _, v := range list {
		if len(x) == 0 {
			x = append(x, v)
		} else {
			for k, v1 := range x {
				if v.String() == v1.String() {
					break
				}
				if k == len(x)-1 {
					x = append(x, v)
				}
			}
		}
	}
	return x
}
