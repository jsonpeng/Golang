package debugout

import "testing"
import "fmt"

func TestDebugOutInfo(t *testing.T) {
	//test 1
	fmt.Println("1*****************************")
	DebugOut("cloudclient", DBG_INFO, "DEG_INFO")

	//test 2
	fmt.Println("2*****************************")
	SetDebugLevel("cloudclient", DBG_ENABLE)
	DebugOut("cloudclient", DBG_INFO, "DEG_INFO")

	//test 3
	fmt.Println("3*****************************")
	SetDebugLevel("cloudclient", DBG_ERROR)
	DebugOut("cloudclient", DBG_INFO, "DEG_INFO")

}

func TestDebugOutError(t *testing.T) {
	//test 1
	fmt.Println("1*****************************")
	DebugOut("cloudclient", DBG_ERROR, "DEG_ERROR", "For Test")

	fmt.Println("2*****************************")
	SetDebugLevel("cloudclient", DBG_DISABLE)
	DebugOut("cloudclient", DBG_ERROR, "DEG_ERROR", "For Test")
}
