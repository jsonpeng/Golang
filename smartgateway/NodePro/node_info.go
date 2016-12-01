package nodepro

import (
	"fmt"
	"net"

	"github.com/portus/SmartGateway/NodePro/Zwave/lib"
)

//gateway
type GatewayInfo struct {
	IpAddress net.IP `json:"gatewayip"`
}

func GetGatewayList() []GatewayInfo {
	//to add your code
	ipList := zwave.ScanGateway(nil, 10000)
	gatewaylist := make([]GatewayInfo, 0, 10)
	for _, v := range ipList {
		gatewaylist = append(gatewaylist, GatewayInfo{v})
	}
	return gatewaylist
}

//nodeinfo
type NodeInfo struct {
	NodeID    byte   `json:"nodeid"`
	IpAddress net.IP `json:"nodeip"`
}

type DTLSPara struct {
	Ip  string `json:"ip"`
	Psk string `json:"psk"`
}

func GetNodeListInfo(para *DTLSPara) []NodeInfo {
	var address, psk string
	if para == nil { /*use default parameter*/
		address = zwave.GetZIPAddress()
		psk = zwave.GetDTLSPSK()
	} else {
		address = para.Ip
		psk = para.Psk
	}

	fmt.Println("ZIPAddress:", address)
	fmt.Println("DTLSPSK:", psk)
	client := zwave.DTLSInit(address+":41230", psk)
	if client == nil {
		return nil
	}
	nodelist := zwave.GetNodeList(client, 3000)

	nodeinfo := make([]NodeInfo, 0, len(nodelist))
	for _, v := range nodelist {
		node := NodeInfo{}
		node.NodeID = v.Node_id
		node.IpAddress = net.IP(v.Ipv6_addr).To16()
		nodeinfo = append(nodeinfo, node)
	}
	client.Close()

	return nodeinfo
}
