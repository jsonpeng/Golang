package systemcfg

import "testing"
import "fmt"

//read and write system configuration file
func TestSystemCfgFile(t *testing.T) {
	LoadSysCfg()
	fmt.Println(sysCfgInfo)

	sysCfgInfo["localip"] = "192.168.10.99"
	SaveSysCfg()
	fmt.Println(sysCfgInfo)

}

//Get system configuration
func TestSystemCfgGet(t *testing.T) {
	ip, err := getSystemCfg("localip")
	if err == nil {
		fmt.Println("localip:", ip)
	} else {
		fmt.Println(err)
	}

	port, err := getSystemCfg("localport")
	if err == nil {
		fmt.Println("localport:", port)
	} else {
		fmt.Println(err)
	}

	test, err := getSystemCfg("test")
	if err == nil {
		fmt.Println("test:", test)
	} else {
		fmt.Println(err)
	}
}

//Set system configuration
func TestSystemCfgSet(t *testing.T) {
	setSystemCfg("name", "lewis")
	fmt.Println(sysCfgInfo)

	setSystemCfg("test", "mytest")
	fmt.Println(sysCfgInfo)
}

//Get and set system configuration
func TestSystemCfg(t *testing.T) {
	SetServerIp("127.0.0.1")
	ip, _ := GetServerIp()
	fmt.Println("localip:", ip)
	SetServerPort("80")
	port, _ := GetServerPort()
	fmt.Println("localport:", port)
	SetGatewayName("portus")
	gateway, _ := GetGatewayName()
	fmt.Println("gateway:", gateway)

	url := GetSysCfgUrl()
	fmt.Println("url:", url)

}
