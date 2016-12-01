package zipapi

/*
//--Note-- : There is cgo comment. DONOT change it

#cgo linux CFLAGS:-I../external-lib/openssl/install/include/openssl  -I../external-lib/openssl/install/include
#cgo linux CFLAGS:-I../external-lib/zware_c_api_v7.02/include 	-I../external-lib/zware_c_api_v7.02/include/zwave
#cgo linux CFLAGS:-I.
#cgo linux CFLAGS:-DOS_LINUX -DZIP_V2 -DUSE_OPENSSL -DCONFIG_DEBUG  -DWKUP_BY_MAILBOX_ACK  -pthread -Wall
#cgo linux LDFLAGS:-L../external-lib/zware_c_api_v7.02/lib -L../external-lib/zware_c_api_v7.02/src -L../external-lib/openssl/install/lib
#cgo linux LDFLAGS:-L/home/eric/GoPack/src/github.com/portus/SmartGateway/NodePro/ZIP/zipapi
#cgo linux LDFLAGS:-lzip_ctl -lzip_api -lm -lpthread -lrt -lssl -lcrypto -ldl

#include <Appl.h>

//--Note-- : There must be no blank lines in between the cgo comment and the import statement
*/
import "C"

import (
	"fmt"
	"unsafe"
)

//Note: following keyword "export" and export function name MUSTBE RESERVED

/*
hl_nw_node_cb - Callback function to notify node is added, deleted, or updated
@param[in]	user	    The high-level api context
@param[in]	noded	Node
@param[in]	mode	    The node status
@return
*/

//export GoNwNodeCB
func GoNwNodeCB(user unsafe.Pointer, noded C.zwnoded_p, mode C.int) {
	fmt.Println("Go callback function \"GoNwNodeCB\" be called")
	switch mode {
	case C.ZWNET_NODE_ADDED:
		fmt.Printf("\nNode:%d added\n", noded.nodeid)

		break

	case C.ZWNET_NODE_REMOVED:
		fmt.Printf("\nNode:%d removed\n", noded.nodeid)

		break
	case C.ZWNET_NODE_UPDATED:
		fmt.Printf("\nNode:%d updated\n", noded.nodeid)

		break
	case C.ZWNET_NODE_STATUS_ALIVE:
		fmt.Printf("\nNode:%d alive\n", noded.nodeid)

		break
	case C.ZWNET_NODE_STATUS_DOWN:
		fmt.Printf("\nNode:%d down or sleep\n", noded.nodeid)

		break

	default:
		fmt.Printf("Node mode unknown\n")
		break
	}
}

/*
hl_nw_notify_cb - Callback function to notify the status of current operation
@param[in]	user	The high-level api context
@param[in]	op		Network operation ZWNET_OP_XXX
@param[in]	sts		The status of current operation
@return
*/

//export GoNwNotifyCB
func GoNwNotifyCB(user unsafe.Pointer, op uint8, sts uint16) {
	fmt.Println("Go callback function \"GoNwNotifyCB \" be called")
	var hl_appl *C.hl_appl_ctx_t
	hl_appl = (*C.hl_appl_ctx_t)(user)

	//Check whether the status is progress status of discovering each detailed node information
	if (sts & C.OP_GET_NI_TOTAL_NODE_MASK) != 0 {
		var total_nodes uint16
		var cmplt_nodes uint16

		total_nodes = (sts & C.OP_GET_NI_TOTAL_NODE_MASK) >> 8
		cmplt_nodes = sts & C.OP_GET_NI_NODE_CMPLT_MASK
		fmt.Printf("Get node info [%d / %d] completed\n", cmplt_nodes, total_nodes)
		return
	}

	switch op {
	case C.ZWNET_OP_INITIALIZE:
		fmt.Printf("\nInitialization status:%d\n", uint(sts))
		if sts == C.OP_DONE {
			fmt.Printf("\nInitialization completed\n")

			var nw_desp C.zwnetd_p
			nw_desp = C.zwnet_get_desc(hl_appl.zwnet)
			fmt.Printf("network id:0x%08X, Z/IP controller id:%d\n", nw_desp.id, nw_desp.ctl_id)
			hl_appl.init_status = 1
			// fmt.Printf("\n(1) Add node\n(x) Exit\n");
			// fmt.Printf("Select your choice:\n");
			fmt.Printf("\nInitialization status: DONE\n")

		} else {
			fmt.Printf("Initialization failed\n")
		}
		break

	case C.ZWNET_OP_ADD_NODE:
		fmt.Printf("Add node status:%u\n", uint(sts))
		if sts == C.OP_DONE { //Clear add node DSK callback control & status
			hl_appl.sec2_cb_enter = 0
			hl_appl.sec2_cb_exit = 1

			hl_appl.add_status = C.ADD_NODE_STS_DONE
		} else if sts == C.OP_FAILED { //Clear add node DSK callback control & status
			hl_appl.sec2_cb_enter = 0
			hl_appl.sec2_cb_exit = 1

			hl_appl.add_status = C.ADD_NODE_STS_UNKNOWN
		}

		if hl_appl.add_status != C.ADD_NODE_STS_PROGRESS {
			// fmt.Printf("\n(1) Add node\n(x) Exit\n");
			// fmt.Printf("Select your choice:\n");
		}
		break

	case C.ZWNET_OP_RM_NODE:
		fmt.Printf("Remove node status:%u\n", uint(sts))
		if sts == C.OP_DONE {
			hl_appl.rm_status = C.RM_NODE_STS_DONE
		} else if sts == C.OP_FAILED {
			hl_appl.rm_status = C.RM_NODE_STS_UNKNOWN
		}

		if hl_appl.rm_status != C.RM_NODE_STS_PROGRESS {
			// fmt.Printf("\n(1) Remove node\n(x) Exit\n");
			// fmt.Printf("Select your choice:\n");
		}
		break

	case C.ZWNET_OP_RESET:
		fmt.Printf("Reset status:%u\n", uint(sts))
		if sts == C.OP_DONE {
			hl_appl.rst_status = C.RESET_NW_STS_DONE
		} else if sts == C.OP_FAILED {
			hl_appl.rst_status = C.RESET_NW_STS_UNKNOWN
		}

		if hl_appl.rst_status != C.RESET_NW_STS_PROGRESS {
			fmt.Printf("Press 'x' to exit ...\n")
		}
		break
	default:
		fmt.Printf("hl_nw_notify_cb op:%u, status:%u\n", uint(op), uint(sts))
	}
}

/**
hl_nw_tx_cb - Callback function to notify application transmit data status
@param[in]	user	    The high-level api context
param[in]	tx_sts	    Transmit status ZWNET_TX_xx
@return
*/

//export GoNwTxCB
func GoNwTxCB(user unsafe.Pointer, tx_sts uint8) {
	var tx_cmplt_sts []string = []string{"ok", "timeout: no ACK received", "system error", "destination host needs long response time", "frame failed to reach destination host"}

	fmt.Println("Go callback function \" GoNwTxCB \" be called")
	fmt.Println(len(tx_cmplt_sts))
	if tx_sts == C.TRANSMIT_COMPLETE_OK {
		fmt.Printf("Higher level appl send data completed successfully\n")
	} else {
		if tx_sts < uint8(len(tx_cmplt_sts)) {
			fmt.Printf("Higher level appl send data completed with error:%s\n", tx_cmplt_sts[tx_sts])
		} else {
			fmt.Printf("Higher level appl send data completed with error:unknown\n")
		}
		//        fmt.Printf("Higher level appl send data completed with error:%s\n",
		//               (tx_sts < sizeof(tx_cmplt_sts)/sizeof(char *))? tx_cmplt_sts[tx_sts]  : "unknown");
	}
}

/*                               Gateway Discovery                            */
/**
gw_discvr_cb - gateway discovery callback
@param[in]	gw_addr	    Gateway addresses
@param[in]	gw_addr_cnt Number of gateway addresses returned in gw_addr
@param[in]	ipv4        Flag to indicate the addr parameter is IPv4 or IPv6. 1=IPv4; 0=IPv6
@param[in]	usr_param   User defined parameter used in callback function
@param[in]	rpt_num     Report number that this callback is delivering the gateway addresses report; start from 1
@param[in]	total_rpt   Total reports that will be delivered by callbacks. Each callback delivers one report.
@param[in]	gw_name	    Gateway names corresponding to the gw_ip.  If NULL, it means gateway name information is unavailable.
*/
////export GoNwTxCB
//func GoGwDiscvrCB(gw_addr *uint8,  gw_addr_cnt uint8,  ipv4 bool, usr_param unsafe.Pointer,  rpt_num int,  total_rpt int, gw_name **C.char){

////void gw_discvr_cb(uint8_t *gw_addr, uint8_t gw_addr_cnt, int ipv4, void *usr_param, int rpt_num, int total_rpt, char **gw_name){

//	var hl_appl *C.hl_appl_ctx_t
//	hl_appl = (*C.hl_appl_ctx_t)(usr_param)

//var tmp_buf *uint8
//var tmp_gw_name **C.char
//var gw_addr_buf_sz, i,result,one_ip_addr_len int
//var addr_str [80]C.char

//temp_ip_slice :=make([]uint8,0,100)
//temp_name_slice :=make([]string,0,100)

//    fmt.Printf("\nReceived report:%d/%d with gw count:%u\n", rpt_num, total_rpt, gw_addr_cnt);

//    if (total_rpt == 0) {
//        printf("The system has no valid IP, please configure it.\n");
//        return;
//    }
//if ipv4{
//	one_ip_addr_len = 4
//}else{
//	one_ip_addr_len = 16
//}

//    if (gw_addr_cnt > 0){
//        //Calculate IP address buffer size for storing new found gateways from this report
//        gw_addr_buf_sz = (gw_addr_cnt * one_ip_addr_len);

//        if (hl_appl.gw_addr_buf != nil){
//		//Expand buffer to store new found gateways from this report
//            tmp_buf = C.realloc(hl_appl.gw_addr_buf, (hl_appl->gw_addr_cnt * one_ip_addr_len) + gw_addr_buf_sz);
//        }
//        else{
//		//Allocate buffer to store new found gateways
//            tmp_buf = malloc(gw_addr_buf_sz);
//        }

//        if (gw_name){
//		//Gateway names are available
//            if (hl_appl->gw_name){
//			//Expand buffer to store new found gateways from this report
//                tmp_gw_name = (char **)realloc(hl_appl->gw_name, (hl_appl->gw_addr_cnt + gw_addr_cnt)*sizeof(char *));
//            }
//            else{
//			//Allocate buffer to store new found gateways
//                tmp_gw_name = (char **)malloc(gw_addr_cnt * sizeof(char *));
//            }
//            if (tmp_gw_name){
//			//Save gateway names
//                hl_appl->gw_name = tmp_gw_name;
//                for (i=0; i<gw_addr_cnt; i++){
//                    tmp_gw_name[hl_appl->gw_addr_cnt + i] = strdup(gw_name[i]);
//                }
//            }
//        }

//        if (tmp_buf){
//		//Save gateway IP addresses
//            hl_appl->gw_addr_buf = tmp_buf;
//            memcpy(hl_appl->gw_addr_buf + (hl_appl->gw_addr_cnt * one_ip_addr_len), gw_addr, gw_addr_buf_sz);
//            hl_appl->gw_addr_cnt += gw_addr_cnt;
//        }
//    }

//    if (hl_appl.gw_addr_cnt > 0){
//        //Display gateway ip addresses
//        fmt.Printf("\n---Gateways found---\n");
//        for (i=0; i<hl_appl.gw_addr_cnt; i++){
//            result = zwnet_ip_ntoa(hl_appl->gw_addr_buf + (i * one_ip_addr_len), addr_str, 80, ipv4);
//            if (result == 0){
//                printf("(%d) %s [%s]\n", i, addr_str, (hl_appl->gw_name)? hl_appl->gw_name[i] : "unknown");
//            }
//        }
//    }
//}
