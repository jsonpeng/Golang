package cloudclient

import "testing"
import "fmt"

func TestCloudClientInit(t *testing.T) {
	CloudClientInit()
}

func TestCloudClientRequest(t *testing.T) {
	beat := RequestCmd{BEAT_EVENT, "BEAT:beat", 1000}
	err := InsertClientSendQ(beat)
	if err != nil {
		fmt.Println("Test error")
		return
	}

	data, err := PickOutClientSendQ()
	if err != nil {
		return
	}
	fmt.Println(data)
}

func TestCloudClientReply(t *testing.T) {
	beat := ReplyCmd{"BEAT:OK"}
	err := InsertClientRecvQ(beat)
	if err != nil {
		fmt.Println("Test error")
		return
	}

	data, err := PickOutClientRecvQ()
	if err != nil {
		return
	}
	fmt.Println(data)
}

func TestCloudClientParseMessage(t *testing.T) {
	beat := "BEAT:OK"
	str := parseMessage([]byte(beat), len(beat))
	fmt.Println(str)

	beat1 := "BEAT"
	str1 := parseMessage([]byte(beat1), len(beat1))
	fmt.Println(str1)
}

/*
func TestCloudClientRegister(t *testing.T) {
	b := registerGateway()
	if b == false {
		fmt.Println("register gateway error")
	}
}
*/

func TestCloudClientTask(t *testing.T) {
	CloudClientTask()
}

func BenchmarkMyTest(b *testing.B) {
	fmt.Println("Test")
}
