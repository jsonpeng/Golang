//copyright 2016 Portus(Wuhan). All rights reserved.
//@Author: Lewis
//@Date:   2016/08/04
//@Version:

/*
  This Package implements a set of simple functions.
  If you have and questions, please don't hesitate to connect us.
*/
package nodeclient

import (
	"encoding/json"
	"errors"
	"fmt"

	"github.com/portus/SmartGateway/NodePro"
)

const (
	NOTIFY  = "NOTIFY"
	REPLY   = "REPLY"
	REQUEST = "REQUEST"
)

//define command list
const (
	//standard command
	GWDISCOVERY = "discovery"
	NODEADD     = "nodeAdd"
	NODEREMOVE  = "nodeRemove"
	GETNODELIST = "getnodelist"

	//test command
	GETMETREDATA   = "GetMeterData"
	GETTEMPERATURE = "GetTemperature"
	GETHUMIDITY    = "GetHumidity"
)

type RequestInfo struct {
	Action  string      `json:"action"`
	Command string      `json:"cmd"`
	Data    interface{} `json:"data"`
}

type ReplyInfo struct {
	Error  int         `json:"error"`
	Action string      `json:"action"`
	Data   interface{} `json:"data"`
}

func ParseData(data []byte) (RequestInfo, error) {
	info := RequestInfo{}
	fmt.Println("parse:", string(data))
	err := json.Unmarshal(data, &info)

	if err != nil {
		return RequestInfo{}, err
	} else {
		fmt.Println("info:", info)
		return info, nil
	}
}

//Deal node client Message
func DealNodeClientMessage(e interface{}) (interface{}, error) {
	cmd := e.(RequestInfo)
	switch cmd.Command {
	case GETMETREDATA:
		return 10000, nil
	case GETTEMPERATURE:
		return 30, nil
	case GETHUMIDITY:
		return 65, nil

	//standard event
	case GWDISCOVERY:
		/*call function to get gateway, to do*/
		fmt.Println("Command:", cmd.Command)
		gatwayinfo := nodepro.GetGatewayList()
		replyinfo := ReplyInfo{Error: 0, Action: REPLY, Data: gatwayinfo}
		return replyinfo, nil
	case NODEADD:
		/*call function to NODEADD, to do*/
		return nil, nil
	case NODEREMOVE:
		//to do
		return nil, nil
	case GETNODELIST:
		fmt.Println("Command:", cmd.Command)
		dtlspara := nodepro.DTLSPara{}
		if cmd.Data != nil {
			bytes, _ := json.Marshal(cmd.Data)
			json.Unmarshal(bytes, &dtlspara)
		}
		fmt.Println("DTLSPara:", dtlspara)
		nodeinfo := nodepro.GetNodeListInfo(&dtlspara)
		replyinfo := ReplyInfo{Error: 0, Action: REPLY, Data: nodeinfo}
		return replyinfo, nil
	}
	return nil, errors.New("not find the command")
}
