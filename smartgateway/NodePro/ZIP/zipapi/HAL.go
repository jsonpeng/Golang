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
	"fmt"
	"time"
)

/************************************************
HALInit - initialization HAL
Load configure file, hardware initialization

@param[in] none
@return[out] error

nil on success; otherwise negative string
*************************************************/

func HALInit() error {
	// if ipv4 {
	// 	C.ApplCtx.use_ipv4 = 1
	// } else {
	// 	C.ApplCtx.use_ipv4 = 0
	// }
	// return NetworkInit(&C.ApplCtx)
	return nil
}

/************************************************
HALDeInit - deinitial HAL

@param[in] none
@return[out] error

nil on success; otherwise negative string
*************************************************/

func HALDeInit() error {
	// if C.ApplCtx.init_status == 1 {
	//
	// }
	// return NetworkExit(&C.ApplCtx)
	return nil
}

/************************************************
HALNetworkInit - initialization all kinds of network
network types refer to configure file
scan gateway, connect gateway
@param[in] none
@return[out] error

nil on success; otherwise negative string
*************************************************/

func HALNetworkInit(ipv4 bool) error {
	if ipv4 {
		C.ApplCtx.use_ipv4 = 1
	} else {
		C.ApplCtx.use_ipv4 = 0
	}
	return NetworkInit(&C.ApplCtx)
}

/************************************************
HALNetworkExit - clean up network
network types refer to configure file
@param[in] none
@return[out] error

nil on success; otherwise negative string

*************************************************/

func HALNetworkExit(ipv4 bool) error {
	return NetworkExit(&C.ApplCtx)
}

/************************************************
HALGetGatewayList - list gateway
@param[in] none
@return[out] error

nil on success; otherwise negative string

*************************************************/

//func HALGetGatewayList( bool) list []net.IPAddr {
//	return
//}
/************************************************
HALGetGatewayList - list gateway
@param[in] none
@return[out] error

nil on success; otherwise negative string

*************************************************/

func HALSelectAGateway(index int16, password string) {
	//	return
}

/************************************************
HALGetNodeList - get node list, list all node in network

@param[in] none
@return[out] error
nil on success; otherwise negative string

@return[out] json
json string, format followed

{
	"node_number":number,
	"node_001":{
		"index":number,
		"attribute":"light",
		"ip_type":4,
		"ip":"192.168.1.100"
	}
}


*************************************************/

func HALGetNodeList() (error, json string) {
	return "hello", "hello"
}

/************************************************
HALAddNode - add a node to network

After call this function, gateway will attempt to
add a node new node into the network for
the next 15 sec.


After add node success, you can get node list again

@param[in] none
@return[out] error
nil on success; otherwise negative string


*************************************************/

func HALAddNode() error {
	return nil
}

/************************************************
HALRemoveNode - Remove a node from network

After call this function, gateway will attempt to
remove a node new node from the network within 15sec.

after remove a node success, you can get node list again

@param[in] none
@return[out] error
nil on success; otherwise negative string


*************************************************/

func HALRemoveNode() error {
	return nil

}

/************************************************
HALSetNodeValue - Set a node value

@param[in] none
@return[out] error
nil on success; otherwise negative string


*************************************************/

func HALSetNodeValue(node_index int, value int) error {
	return nil

}

/************************************************
HALGetNodeValue - Get a node value

@param[in] none
@return[out] error
nil on success; otherwise negative string


*************************************************/

func HALGetNodeValue(node_index int) (error, value *int) {
	return nil, nil

}

/************************************************
HAL TEST - HAL TEST

@param[in] none
@return[out] error
nil on success; otherwise negative string


*************************************************/

/**
basic_intf_get - Search for the first basic interface
@param[in]  net        network handle
@param[out] basic_if   first basic interface found
@return  0 on success; otherwise negative number
*/
//func Afunc(net *C.struct__zwnet, basic_if *C.zwifd_t) int16 {

func Afunc(net C.zwnet_p, basic_if *C.zwifd_t) int16 {
	var result int16
	var node C.zwnoded_t
	var ep C.zwepd_t
	var intf C.zwifd_t

	//Get first node (controller node)
	result = int16(C.zwnet_get_node(net, &node))
	fmt.Println("0 C.zwnet_get_node(net, &node) | node:", node)
	fmt.Println("0 C.zwnet_get_node(net, &node) | result:", result)

	if result != 0 {
		return result
	}
	for {
		if C.zwnode_get_next(&node, &node) != 0 {
			break
		}
		fmt.Println("1 C.zwnode_get_next(&node, &node) | node:", node)
		if C.zwnode_get_ep(&node, &ep) == 0 { //get first endpoint of the node
			fmt.Println("2 C.zwnode_get_ep(&node, &ep) | ep:", ep)
			// do
			for {
				if C.zwep_get_if(&ep, &intf) == 0 { //get first interface of the endpoint
					fmt.Println("3 C.zwep_get_if(&ep, &intf) | intf:", intf)
					// do
					for {
						if intf.cls == C.COMMAND_CLASS_SWITCH_BINARY { //Found
							*basic_if = intf
							fmt.Println("6 intf.cls == C.COMMAND_CLASS_SWITCH_BINARY | intf:", intf)
							fmt.Println("intf.nodeid is ", intf.nodeid)
							return 0
							//							break
						}

						//Free interface data
						if intf.data_cnt > 0 {
							C.free(intf.data)
						}
						if C.zwif_get_next(&intf, &intf) != 0 {
							fmt.Println("5 C.zwif_get_next(&intf, &intf) | intf:", intf)
							break
						}
						fmt.Println("4 C.zwif_get_next(&intf, &intf) | intf:", intf)
						time.Sleep(200 * time.Microsecond)

					} //while (!zwif_get_next(&intf, &intf)); //get next interface
				}
				if C.zwep_get_next(&ep, &ep) == 0 {
					fmt.Println("7 C.zwep_get_next(&ep, &ep) | ep:", ep)
					break
				}
				fmt.Println("8 C.zwep_get_next(&ep, &ep) | ep:", ep)
			} //while (!zwep_get_next(&ep, &ep)); //get next endpoint

		}
	}
	return 0
}

const Timeout = 20

//export HALTest
func HALTest() {
	fmt.Println("C.ApplCtx.init_status=", C.ApplCtx.init_status, "  Wait for network initialized...")
	//	if C.ApplCtx.init_status != 1 {
	//		ticktick := time.Now()
	//		fmt.Println(time.Now(), "Wait for [", Timeout, "] seconds")
	//		//		time.Sleep(Timeout * time.Second)
	//		for {
	//			if ticktick.After(time.Time(Timeout * time.Second)) {
	//				break
	//			}
	//		}
	//	}
	//	if C.ApplCtx.init_status != 1 {
	//		fmt.Println("C.ApplCtx.init_status=", C.ApplCtx.init_status)
	//		fmt.Println("Timeup, stop waiting network, return")
	//		return
	//	}

	//	for { //firstly Initialize network
	//		if C.ApplCtx.init_status == 1 {
	//			break
	//		}
	//	}

	fmt.Println("C.ApplCtx.init_status=", C.ApplCtx.init_status)
	fmt.Println("hello aaaa")
	var basic_if C.zwifd_t

	Afunc(C.ApplCtx.zwnet, &basic_if)

	fmt.Println("hello kkkk")

}
