package zwave

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
	"ZipRouterIP": "127.0.0.1",
	"ZipLanPort":  "0",
	"DTLSPSK":     "123456789012345678901234567890AA",
}

//System configuration file name
const sysCfgFile = "app.cfg"

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

func GetZIPAddress() string {
	return sysCfgInfo["ZipRouterIP"]
}

func GetDTLSPSK() string {
	return sysCfgInfo["DTLSPSK"]
}
