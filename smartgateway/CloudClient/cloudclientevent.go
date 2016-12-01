//copyright 2016 Portus(Wuhan). All rights reserved.
//@Author: Lewis
//@Date:   2016/08/04
//@Version:

/*
  This Package implements a set of simple functions.
  If you have and questions, please don't hesitate to connect us.
*/
package cloudclient

import (
	"errors"
	"fmt"
	"strconv"

	"github.com/portus/SmartGateway/DateBase/RegisterUserInfo"
	"github.com/portus/SmartGateway/DateBase/SystemCfg"
)

const (
	GWID_EVENT    = "GWID"    //gateway online
	URL_EVENT     = "URL"     //url access
	BEAT_EVENT    = "BEAT"    //beat message
	ONLINE_EVENT  = "ONLINE"  //user online
	OFFLINE_EVENT = "OFFLINE" //user offline
	EXIT_EVENT    = "GWEXIT"  //gateway offline
)

/*request command struct*/
type RequestCmd struct {
	Event   string /*event string*/
	Data    string /*request data string*/
	Timeout int    /*timeout ms, default value 100ms*/
}

/*reply command struct*/
type ReplyCmd struct {
	//to add
	Data string /*reply data string*/
}

//register gateway information
func DoRegisterMessage() (ReplyCmd, error) {
	for i := 0; i < 1; i++ {
		buf, _ := systemcfg.GetGatewayID()
		reg := RequestCmd{GWID_EVENT, "GWID:" + buf, 1000}
		data, err := DoCloudClientMessage(reg)
		if err == nil {
			fmt.Println(data)
			return data, err
		}
	}
	return ReplyCmd{}, errors.New("DoRegisterMessage Error")
}

//url message
func DoURLMessage() (ReplyCmd, error) {
	for i := 0; i < 1; i++ {
		buf := systemcfg.GetSysCfgUrl()
		url := RequestCmd{URL_EVENT, "URL:" + buf, 1000}
		data, err := DoCloudClientMessage(url)
		if err == nil {
			return data, err
		}
	}
	return ReplyCmd{}, errors.New("DoURLMessage Error")

}

//Beat message
func DoBeatMessage() (ReplyCmd, error) {
	for i := 0; i < 1; i++ {
		beat := RequestCmd{BEAT_EVENT, "BEAT:online", 1000}
		data, err := DoCloudClientMessage(beat)
		if err == nil {
			return data, err
		}
	}

	return ReplyCmd{}, errors.New("DoBeatMessage Error")

}
func DoGwidExit(gwid string, timeout int) (ReplyCmd, error) {
	requestCmd := RequestCmd{}
	requestCmd.Event = EXIT_EVENT
	requestCmd.Data = "GWEXIT:" + gwid
	requestCmd.Timeout = timeout
	replycmd, err := DoCloudClientMessage(requestCmd)
	return replycmd, err
}

//Online Message
func DoOnlineMessage(data string, timeout int) (ReplyCmd, error) {
	//data structure
	requestCmd := RequestCmd{}
	requestCmd.Event = ONLINE_EVENT
	requestCmd.Data = "ONLINE:" + data
	requestCmd.Timeout = timeout

	replycmd, err := DoCloudClientMessage(requestCmd)
	return replycmd, err
}

//Online Message
func DoOfflineMessage(data string, timeout int) (ReplyCmd, error) {
	//data structure
	requestCmd := RequestCmd{}
	requestCmd.Event = OFFLINE_EVENT
	requestCmd.Data = "OFFLINE:" + data
	requestCmd.Timeout = timeout

	replycmd, err := DoCloudClientMessage(requestCmd)
	return replycmd, err
}

//default online user, for test
func DoOnlineDefaultMessage() (ReplyCmd, error) {
	name, _ := systemcfg.GetUserName()
	//passwd, _ := systemcfg.GetUserPassword()
	ok, user := RegisterUserInfo.SeekUser(name)
	struid := strconv.Itoa(int(user.UId))
	fmt.Println(struid + ":" + user.Username + ":" + user.Password)
	if ok == nil {
		for i := 0; i < 1; i++ {
			buf := struid + ":" + user.Username + ":" + user.Password
			data, err := DoOnlineMessage(buf, 1000)
			if err == nil {
				return data, err
			}
		}
	}
	return ReplyCmd{}, errors.New("DoOnlineDefaultMessage Error")
}
