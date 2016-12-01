//copyright 2016 Portus(Wuhan). All rights reserved.
//@Author: Lewis
//@Date:   2016/08/20
//@Version:

/*
  This Package implements a set of simple functions.
  If you have and questions, please don't hesitate to connect us.
*/
package systemcfg

import (
	//"fmt"
	"io/ioutil"
	//"os"
	//"strings"
)

//System url file name
const sysUrlFile = "url.conf"

//get the url
func GetSysCfgUrl() string {
	Raw, err := ioutil.ReadFile(sysUrlFile)
	if (err == nil) && (len(Raw) > 0) {
		url := string(Raw)
		return url
	} else {
		url := GetDefaultURL()
		return url
	}
}
