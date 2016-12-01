//copyright 2016 Portus(Wuhan). All rights reserved.
//@Author: Lewis
//@Date:   2016/08/05
//@Version:

/*
  This Package implements a set of simple functions.
  If you have and questions, please don't hesitate to connect us.
*/
package systemcfg

import (
	"errors"
	"fmt"
	"os"
	"strings"
)

//init
func init() {
	LoadSysCfg()
}

//get system configuration item
func getSystemCfg(key string) (string, error) {
	for k, v := range sysCfgInfo {
		//fmt.Println(k, v)
		if strings.EqualFold(key, k) {
			return v, nil
		}

	}

	fmt.Println("can't get the data")
	return "", errors.New("GetSystemCfg Error")
}

//set system configuration item
func setSystemCfg(key, value string) error {
	for k, _ := range sysCfgInfo {
		//fmt.Println(k, v)
		if strings.EqualFold(key, k) {
			sysCfgInfo[k] = value
			SaveSysCfg()
			return nil
		}
	}

	fmt.Println("can't set the data")
	return errors.New("SetSystemCfg Error")
}

//get local server ip
func GetServerIp() (string, error) {
	//return getSystemCfg("localip")
	return os.Hostname()

}

//set local server ip
func SetServerIp(ip string) error {
	return setSystemCfg("localip", ip)
}

//get local server port
func GetServerPort() (string, error) {
	return getSystemCfg("localport")
}

//set local server port
func SetServerPort(port string) error {
	return setSystemCfg("localport", port)
}

//get gateway allow
func GetGatewayAllow() (string, error) {
	return getSystemCfg("gateway")
}

//set gateway allow
func SetGatewayAllow(allow string) error {
	return setSystemCfg("gateway", allow)
}

func GetGatewayID() (string, error) {
	return getSystemCfg("gwid")
}

//get user name
func GetUserName() (string, error) {
	return getSystemCfg("name")
}

//get user password
func GetUserPassword() (string, error) {
	return getSystemCfg("passwd")
}

//get default URL
func GetDefaultURL() string {
	ip, _ := GetServerIp()
	port, _ := GetServerPort()
	url := "http://" + ip + ":" + port
	return url
}

//get Remote server ip
func GetRemoteServerIp() (string, error) {
	return getSystemCfg("serverip")
}

//get Remote server port
func GetRemoteServerPort() (string, error) {
	return getSystemCfg("serverport")
}

func GetWebsocketURL() (string, error) {
	ip, _ := GetServerIp()
	port, _ := GetServerPort()

	return "ws://" + ip + ":" + port, nil
}
