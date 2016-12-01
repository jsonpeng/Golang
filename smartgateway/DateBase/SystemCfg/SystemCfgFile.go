//copyright 2016 Portus(Wuhan). All rights reserved.
//@Author: Lewis
//@Date:   2016/08/08
//@Version:

/*
  This Package implements a set of simple functions.
  If you have and questions, please don't hesitate to connect us.
*/
package systemcfg

import (
	"fmt"
	"io/ioutil"
	"os"
	"strings"
)

//system configuration cache
var sysCfgInfo = make(map[string]string)

//default configuration information
var defaultCfgInfo = map[string]string{
	"serverip":   "127.0.0.1", //remote cloud client
	"serverport": "7788",      //remote cloud client
	"localip":    "127.0.0.1", //local webserver
	"localport":  "8360",      //local webserver
	"gateway":    "on",        //allow gateway online
	"gwid":       "20160818ABCD",
	"name":       "admin",
	"passwd":     "123456",
}

//System configuration file name
const sysCfgFile = "default.conf"

//when system set up, it will call init function.
func init() {
	LoadSysCfg()
}

//load system configuration file
func LoadSysCfg() {
	Raw, err := ioutil.ReadFile(sysCfgFile)
	if err != nil {
		fout, err := os.Create(sysCfgFile)
		defer fout.Close()

		if err != nil {
			fmt.Println("CreateFile Error:", err)
			return
		}
		sysCfgInfo = defaultCfgInfo
		for k, v := range sysCfgInfo {
			fout.WriteString(k + "=" + v + "\r\n")
		}

		return
	} else {
		info := string(Raw)
		items := strings.Split(info, "\r\n")
		for _, ok := range items {
			if ok != "" {
				item := strings.Split(ok, "=")
				sysCfgInfo[item[0]] = item[1]
			}

		}

	}
}

//save system configuration file
func SaveSysCfg() {
	fout, err := os.Create(sysCfgFile)
	defer fout.Close()

	if err != nil {
		fmt.Println("CreateFile Error:", err)
		return
	}
	for k, v := range sysCfgInfo {
		fout.WriteString(k + "=" + v + "\r\n")
	}

	return
}
