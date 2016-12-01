package nodeclient

import (
	"fmt"
	"net"
	"testing"
	"time"
)

func TestMySlice(t *testing.T) {
	//test1
	var slice1 []int
	slice1 = make([]int, 1, 5)
	//slice1[0] = 5
	slice1 = append(slice1, 2, 3, 4)
	fmt.Println(slice1)

	//test2
	var slice2 []int = []int{1, 2, 3, 4}
	fmt.Println(slice2)

	//test3
	slice3 := make([]int, 1, 5)
	fmt.Println(slice3)
}

func TestIpv6(t *testing.T) {
	ip := []byte{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xc0, 0xa8, 0x0a, 0x9d}
	fmt.Println(net.IP(ip).To16())
	t := time.Date(2009, time.November, 10, 23, 0, 0, 0, time.UTC)

}

func TestNodeClient(t *testing.T) {
	NodeClientInit()

	NodeClientTask()
}
