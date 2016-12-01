package simulate

import (
	"encoding/json"
	"errors"
	"fmt"
)

//node struct
type NodeInfo struct {
	Id       int64  `json:"id,int"`
	Category string `json:"category"`
	Nodename string `json:"nodename"`
	Address  string `json:"address"`
	Protocol string `json:"protocol"`
}

//gatway struct
type GateWayInfo struct {
	Id          int64      `json:"id,int"`
	Category    string     `json:"category"`
	GatewayName string     `json:"gatewayName"`
	Address     string     `json:"address"`
	Protocol    string     `json:"protocol"`
	NodeData    []NodeInfo `json:"nodeData"`
}

//test node data
var node1 = []NodeInfo{{0, "node1", "test1", "fe80::19c0:d694:85c1:b81", "SIMULATE"},
	{1, "node2", "test2", "fe80::19c0:d694:85c1:b82", "ZIBGEE"},
	{2, "node3", "test3", "fe80::19c0:d694:85c1:b83", "ZWAVE"},
	{3, "node4", "test4", "fe80::19c0:d694:85c1:b84", "ZWAVE"},
	{4, "node5", "test5", "fe80::19c0:d694:85c1:b85", "ZWAVE"}}

//test node data
var node2 = []NodeInfo{{0, "switch", "switch1", "fe80::19c0:d694:85c1:b81", "SIMULATE"},
	{1, "switch", "switch2", "fe80::19c0:d694:85c1:b81", "SIMULATE"},
	{2, "test", "test3", "fe80::19c0:d694:85c1:b81", "SIMULATE"}}

//test node data
var node3 = []NodeInfo{{0, "switch", "switch1", "fe80::19c0:d694:85c1:b81", "SIMULATE"},
	{1, "switch", "switch2", "fe80::19c0:d694:85c1:b81", "SIMULATE"}}

//gatewaylist
var gatewaylist = []GateWayInfo{{0, "gateway", "gateway1", "192.168.10.99", "SIMULATE", node1},
	{1, "getway", "gateway2", "192.168.10.227", "SIMULATE", node2},
	{2, "getway", "gateway3", "192.168.10.1", "SIMULATE", node3}}

//get gatewaylist
func GetGatewayList() ([]GateWayInfo, error) {
	//to add your code
	return gatewaylist, nil
}

//get gateway
func GetGateway(gid int64) (GateWayInfo, error) {
	return gatewaylist[gid], nil
}

//get nodeinfo Test
func GetNodeListTest(GwAddress string) ([]NodeInfo, error) {
	fmt.Println("GwAddress", GwAddress)
	if GwAddress == "192.168.10.99" {
		return gatewaylist[0].NodeData, nil
	} else if GwAddress == "192.168.10.227" {
		return gatewaylist[1].NodeData, nil
	} else {
		return nil, errors.New("error")
	}
}

//get node list
func GetNodeList(gid int64) ([]NodeInfo, error) {
	return gatewaylist[gid].NodeData, nil
}

func GetNode(gid, nid int64) (NodeInfo, error) {
	//add your code
	return gatewaylist[gid].NodeData[nid], nil
}
func Jsonlist() ([]byte, error) {
	list, _ := GetGatewayList()
	jsonlist, _ := json.MarshalIndent(list, "", "\t\t")
	return jsonlist, nil
}
