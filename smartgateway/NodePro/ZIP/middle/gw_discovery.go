package zwave

/*
//--Note-- : There is cgo comment. DONOT change it

#cgo linux CFLAGS:-I../../external/openssl/include/openssl  -I../../external/openssl/include
#cgo linux CFLAGS:-I../../external/zware_c_api_v7.02/include 	-I../../external/zware_c_api_v7.02/include/zwave
#cgo linux CFLAGS:-I.
#cgo linux CFLAGS:-DOS_LINUX -DZIP_V2 -DUSE_OPENSSL -DCONFIG_DEBUG  -DWKUP_BY_MAILBOX_ACK  -pthread -Wall
#cgo linux LDFLAGS:-L../../external/zware_c_api_v7.02/lib -L../../external/zware_c_api_v7.02/src -L../../external/openssl/lib
#cgo linux LDFLAGS:-lzip_ctl -lzip_api -lm -lpthread -lrt -lssl -lcrypto -ldl


#include <gw_discovery.h>

//--Note-- : There must be no blank lines in between the cgo comment and the import statement
*/
import "C"
import (
	"fmt"
	"unsafe"
)

func Discovery_gateway() {
	C.gw_discovery()
}

//export discvr_cb
/*
func discvr_cb(gw_addr *C.uint8_t, gw_addr_cnt C.uint8_t, ipv4 C.int, usr_param unsafe.Pointer,
	rpt_num C.int, total_rpt C.int, gw_name **C.char) {

	hl_appl := (*C.hl_appl_ctx_t)(usr_param)
	if total_rpt == 0 {
		fmt.Println("can't get the gateway")
		return
	}

	//one_ip_addr_len := 4

	var tmp_buf []C.uint8_t
	if gw_addr_cnt > 0 {
		if hl_appl.gw_addr_buf == nil {
			tmp_buf = make([]C.uint8_t, 0, 128)
			hl_appl.gw_addr_buf = (*C.uint8_t)(unsafe.Pointer(&tmp_buf))
		}
		//tmp_buf = append(tmp_buf, *gw_addr)
		b := (*C.char)(unsafe.Pointer(gw_addr))
		//for i := 0; i < one_ip_addr_len; i++ {
		//	fmt.Println(b[i])
		//}
		fmt.Println(b)

	}

	if len(tmp_buf) > 0 {
		fmt.Println(tmp_buf)
	}



}
*/

//export discvr_cb
func discvr_cb(gw_addr *C.uint8_t, buflen, cnt C.int) {
	fmt.Println("count:", cnt)
	d := C.GoBytes((unsafe.Pointer)(gw_addr), buflen)
	fmt.Println(d)

}
