package cloudclient

import (
	"fmt"
	"os/exec"

	"github.com/portus/SmartGateway/DateBase/SystemCfg"
)

//init function
func init() {

}

//call system command
func PangolinClient() {
	defer func() {
		if r := recover(); r != nil {
			fmt.Println(r)
		}
	}()

	RemoteIP, _ := systemcfg.GetRemoteServerIp()
	LocalIp, _ := systemcfg.GetServerIp()
	/*pangolin script, as client to startup*/
	pangolin := "node ./pangolin client " + "-r " + RemoteIP + ":10000 " + "-l " + LocalIp + ":8360 " + "&"
	/*print the script*/
	fmt.Println("pangolin:", pangolin)
	cmd := exec.Command("/bin/sh", "-c", pangolin) //

	/*run command*/
	err := cmd.Run()
	if err != nil {
		panic(err)
	}
}
