package zwave

/*
//--Note-- : There is cgo comment. DONOT change it

#cgo linux CFLAGS:-I../external/openssl/install/include/openssl  -I../external/openssl/install/include
#cgo linux CFLAGS:-I../external/zware_c_api_v7.02/include 	-I../external/zware_c_api_v7.02/include/zwave
#cgo linux CFLAGS:-I.
#cgo linux CFLAGS:-DOS_LINUX -DZIP_V2 -DUSE_OPENSSL -DCONFIG_DEBUG  -DWKUP_BY_MAILBOX_ACK  -pthread -Wall
#cgo linux LDFLAGS:-L../external/zware_c_api_v7.02/lib -L../external/zware_c_api_v7.02/src -L../external/openssl
#cgo linux LDFLAGS:-lzip_ctl -lzip_api -lm -lpthread -lrt -lssl -lcrypto -ldl


#include <add_node.h>

//--Note-- : There must be no blank lines in between the cgo comment and the import statement
*/
import "C"

func AddNode() {
	C.AddNode()
}
