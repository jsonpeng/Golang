package testserver

import (
	"net"
	"os"
	"testing"
)
import "fmt"

func GetLocalIP() string {
	addrs, err := net.InterfaceAddrs()
	if err != nil {
		return ""
	}
	for _, address := range addrs {
		// check the address type and if it is not a loopback the display it
		if ipnet, ok := address.(*net.IPNet); ok && !ipnet.IP.IsLoopback() {
			if ipnet.IP.To4() != nil {
				return ipnet.IP.String()
			}
		}
	}
	return ""
}

func TestMySlice(t *testing.T) {
	fmt.Println("Test Server Test....")
	hostname, _ := os.Hostname()
	fmt.Println(hostname)

	addrs, err := net.LookupHost(hostname)
	if err != nil {
		fmt.Printf("Oops: %v\n", err)
		return
	}

	for _, a := range addrs {
		fmt.Println(a)
	}

	fmt.Println("IPAddr:=", GetLocalIP())
}
