package zwave

import (
	"fmt"
)

const IPV6_ADDR_LEN int = 16

/*
//command class
const (
	COMMAND_CLASS_ZIP_ND                   = 0x58
	COMMAND_CLASS_ZIP                      = 0x23
	COMMAND_CLASS_NETWORK_MANAGEMENT_PROXY = 0x52
)

//command
const (
	COMMAND_ZIP_PACKET = 0x02

	COMMAND_NODE_LIST_GET    = 0x01
	COMMAND_NODE_LIST_REPORT = 0x02

	COMMAND_ZIP_NODE_ADVERTISEMENT    = 0x01
	COMMAND_ZIP_NODE_SOLICITATION     = 0x03
	COMMAND_ZIP_INV_NODE_SOLICITATION = 0x04
)
*/
//
type NodeInfo struct {
	Node_id   byte
	Ipv6_addr []byte
	Homeid    uint32
}

func GetNodeList(c *DTLSClient, timeout int) []NodeInfo {
	nodeinfo := make([]NodeInfo, 0, 10)
	zip_frm := []byte{COMMAND_CLASS_ZIP, COMMAND_ZIP_PACKET, 0x80, 0x50, 0xaa, 0x00, 0x00,
		COMMAND_CLASS_NETWORK_MANAGEMENT_PROXY, COMMAND_NODE_LIST_GET, 0xbb}

	buff, bufflen := DTLSHandlerMSG(c, zip_frm, timeout)
	if bufflen > 0 {
		netbuff := buff[7:bufflen]
		if netbuff[0] == COMMAND_CLASS_NETWORK_MANAGEMENT_PROXY &&
			netbuff[1] == COMMAND_NODE_LIST_REPORT && netbuff[3] == 0x00 {
			nodecode := GetNodeCode(netbuff[5:bufflen])
			fmt.Println("nodecode len:", len(nodecode))
			for _, v := range nodecode {
				zip_frm[0] = COMMAND_CLASS_ZIP_ND
				zip_frm[1] = COMMAND_ZIP_INV_NODE_SOLICITATION
				zip_frm[2] = 0x04
				zip_frm[3] = v

				//get node list
				buff_frm := zip_frm[:4]
				buff, bufflen := DTLSHandlerMSG(c, buff_frm, timeout)
				if bufflen > 0 {
					rpt := NodeInfo{}
					rpt.Node_id = buff[3]
					rpt.Ipv6_addr = make([]byte, 16)
					for i := 0; i < IPV6_ADDR_LEN; i++ {
						rpt.Ipv6_addr[i] = buff[4+i]
					}
					homeid := buff[4+IPV6_ADDR_LEN:]
					rpt.Homeid = (uint32)(homeid[0])<<24 | (uint32)(homeid[1])<<16 | (uint32)(homeid[2])<<8 | (uint32)(homeid[3])
					nodeinfo = append(nodeinfo, rpt)
				}
			}
		}
	}
	return nodeinfo
}

//get node code list
func GetNodeCode(buff []byte) []byte {
	code := make([]byte, 0, 10)
	var idx byte = 1
	for _, v := range buff {
		for i := 0; i < 8; i++ {
			if flag := v & (byte)(1<<(uint)(i)); flag != 0 {
				code = append(code, idx)
			}
			idx++
		}
	}
	return code
}
