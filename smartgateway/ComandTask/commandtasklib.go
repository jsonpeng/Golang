//copyright 2016 Portus(Wuhan). All rights reserved.
//@Author: Lewis
//@Date:   2016/08/08
//@Version:

/*
  This Package implements a set of simple functions.
  If you have and questions, please don't hesitate to connect us.
*/
package commandtask

import (
	"encoding/json"
	"errors"
	"fmt"

	"github.com/portus/SmartGateway/CloudClient"
	"github.com/portus/SmartGateway/DateBase/LoginInfo"
	"github.com/portus/SmartGateway/DateBase/SystemCfg"
	"github.com/portus/SmartGateway/DebugOut"
	"github.com/portus/SmartGateway/NodeClient"
)

var commandlist = map[string]func([]string) (string, error){
	"getsrvip":     GetServerip,
	"setsrvip":     SetServerip,
	"exit":         ExitServer,
	"setdbglevel":  SetDBGLevel,
	"gwdiscovery":  GwDiscovery,
	"nodeadd":      NodeAdd,
	"online":       UserOnline,
	"offline":      UserOffline,
	"gwexit":       GatewayExit,
	"getalluser":   GetAllUser,
	"getsigleuser": SpecifiedUser,
	"getlist":      GetList,
}

func SetDBGLevel(value []string) (string, error) {
	if value[1] == "ENABLE" {
		debugout.SetDebugLevel(value[0], debugout.DBG_ENABLE)
	} else if value[1] == "DISABLE" {

		debugout.SetDebugLevel(value[0], debugout.DBG_DISABLE)

	}
	return "Ok", nil
}
func GetServerip(value []string) (string, error) {
	if len(value) != 0 {
		return "", errors.New("Have Wrong Done")
	} else {
		ip, _ := systemcfg.GetServerIp()
		debugout.DebugOut("commandtask", debugout.DBG_INFO, "ServerIpAddr:", ip)
		return ip, nil
	}

}
func SetServerip(value []string) (string, error) {
	if len(value) != 1 {
		return "", errors.New("Have Wrong Done")
	} else {
		systemcfg.SetServerIp(value[0])
		return "Ok", nil
	}

}
func ExitServer(value []string) (string, error) {
	if len(value) != 0 {
		return "", errors.New("Have Wrong Done")
	} else {
		return "Ok", nil
	}
}
func GwDiscovery(value []string) (string, error) {
	nodeclient.GetGatewayDiscovery()
	return "Ok", nil
}
func NodeAdd(value []string) (string, error) {
	nodeclient.GetGatewayNodeAdd()
	return "Ok", nil
}

func UserOnline(value []string) (string, error) {
	if len(value) == 2 {
		data := "0:" + value[0] + ":" + value[1]
		_, err := cloudclient.DoOnlineMessage(data, 1000)
		if err == nil {
			fmt.Println("online", " ", data, " ", "successful.")
			return "Ok", nil
		}
	} else {
		fmt.Println(value)
		return "", errors.New("Have Wrong Done")
	}
	return "", errors.New("Have Wrong Done")
}

func UserOffline(value []string) (string, error) {
	fmt.Println(value)
	if len(value) == 2 {
		data := "0:" + value[0] + ":" + value[1]
		_, err := cloudclient.DoOfflineMessage(data, 1000)
		if err == nil {
			fmt.Println("offline", " ", data, " ", "successful.")
			return "Ok", nil
		}
	} else {
		fmt.Println(value)
		return "", errors.New("UserOffline error")
	}

	return "", errors.New("UserOffline error")

}

func GatewayExit(value []string) (string, error) {
	fmt.Println(value)

	_, err := cloudclient.DoCloudClientMessage(cloudclient.RequestCmd{cloudclient.EXIT_EVENT, "GWEXIT:exit", 1000})
	if err == nil {
		fmt.Println("GWEXIT", "successful.")
		return "Ok", nil
	}

	return "", errors.New("UserOffline error")

}

func GetAllUser(value []string) (string, error) {
	if len(value) != 0 {
		return "", errors.New("Request Failed")
	} else {
		getUserMS, err := json.Marshal(LoginInfo.AllLoginUser())
		if err != nil {
			return "", err
		}

		return string(getUserMS), nil
	}
}
func SpecifiedUser(value []string) (string, error) {
	if len(value) != 1 {
		return "", errors.New("Have Wrong Done")
	} else {
		getSimpleUser, err := json.Marshal(LoginInfo.LoginUserMap[value[0]])
		if err != nil {
			return "", errors.New("To Marshal Failed")
		}
		return string(getSimpleUser), nil
	}
}
func GetList(value []string) (string, error) {
	cloudclient.DoCloudClientMessage(cloudclient.RequestCmd{"LIST", "LIST:list", 1000})
	return "Ok", nil
}
