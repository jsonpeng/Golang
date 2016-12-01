// AddNode.go
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
	//	"bufio"
	"errors"
	"fmt"
	//	"os"
	//	"unsafe"
)

// func AddNode(ctx *C.struct_hl_appl_ctx) error {
func AddNode(ctx *C.struct_hl_appl_ctx) {
	// appl_ctx := C.struct_hl_appl_ctx{}
	// appl_ctx.use_ipv4 = 1 //Use IPv4

	fmt.Println("Add node ...")
	// var choice int
	var result int

	if ctx.init_status == 0 {
		// return errors.New("Network not Initialized")
		fmt.Println("Network not Initialized")
	} else if (ctx.add_status == C.ADD_NODE_STS_UNKNOWN) || (ctx.add_status == C.ADD_NODE_STS_DONE) {
		fmt.Println("Add node ongoing")
		result = int(C.hl_add_node(ctx))
		fmt.Printf("Add node result:%d\n", result)
		//result = zwnet_add(appl_ctx.zwnet, 1, (appl_ctx.sec2_add_node)? &appl_ctx.sec2_add_prm : NULL);
		if ctx.sec2_add_node == 0 {
			if result == 0 {
				fmt.Printf("Add node in progress, please wait for status ...\n")
				ctx.add_status = C.ADD_NODE_STS_PROGRESS

				// return errors.New("Add node in progress, please wait for status ")
			} else {
				fmt.Printf("Add node with error:%d\n", result)
				// return errors.New("Add node with error ")
			}
		}
	} else if ctx.add_status == C.ADD_NODE_STS_PROGRESS {
		// return errors.New("Add node in progress ")
		fmt.Println("Add node in progress")
	}
}

func AddNodeStatus(ctx *C.struct_hl_appl_ctx) error {
	if ctx.add_status == C.ADD_NODE_STS_UNKNOWN {
		return errors.New("ADD_NODE_STS_UNKNOWN")
	} else if ctx.add_status == C.ADD_NODE_STS_PROGRESS {
		return errors.New("ADD_NODE_STS_PROGRESS")
	} else if ctx.add_status == C.ADD_NODE_STS_DONE {
		return nil
	}
	return nil
}

func AddA() {
	C.ApplCtx.use_ipv4 = 1
	fmt.Println("Wait for network initialized...")
	for { //firstly Initialize network
		if C.ApplCtx.init_status == 1 {
			break
		}
	}
	fmt.Println("Network initialized, Add node...")
	AddNode(&C.ApplCtx)
}

// func AddNode(ctx *C.struct_hl_appl_ctx) error {
func RemoveNode(ctx *C.struct_hl_appl_ctx) {

	fmt.Println("Remove node ...")
	var result int

	if ctx.init_status == 0 {
		// return errors.New("Network not Initialized")
		fmt.Println("Network not Initialized")
	} else if (ctx.rm_status == C.RM_NODE_STS_UNKNOWN) || (ctx.rm_status == C.RM_NODE_STS_DONE) {
		fmt.Println("Remove node ongoing")
		result = int(C.zwnet_add(ctx.zwnet, 0, nil))
		fmt.Printf("Remove node result:%d\n", result)

		if result == 0 {
			fmt.Printf("Remove node in progress, please wait for status ...\n")
			ctx.rm_status = C.RM_NODE_STS_PROGRESS

			// return errors.New("Remove node in progress, please wait for status ")
		} else {
			fmt.Printf("Remove node with error:%d\n", result)
			// return errors.New("Remove node with error ")
		}
	} else if ctx.rm_status == C.RM_NODE_STS_PROGRESS {
		// return errors.New("Remove node in progress ")
		fmt.Println("Remove node in progress")
	} else {
		fmt.Println("unknow")
		fmt.Println("ctx.rm_status:", ctx.rm_status, "ctx.init_status:", ctx.init_status)
	}
}
func RemoveA() {
	C.ApplCtx.use_ipv4 = 1

	fmt.Println("C.ApplCtx.init_status=", C.ApplCtx.init_status, "  Wait for network initialized...")
	for { //firstly Initialize network
		if C.ApplCtx.init_status == 1 {
			break
		}
	}
	fmt.Println("Network initialized, Remove node...")
	RemoveNode(&C.ApplCtx)
}

//func GetNode(){

//}
