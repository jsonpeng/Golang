//copyright 2016 Portus(Wuhan). All rights reserved.
//@Author: Lewis
//@Date:   2016/08/11
//@Version:

/*
  This Package implements a set of simple functions.
  If you have and questions, please don't hesitate to connect us.
*/
package cloudclient

import (
	"errors"
	"fmt"
	"net"
	//"strconv"
	"strings"
	"sync"
	"time"

	"github.com/portus/SmartGateway/CmdQueue"
	"github.com/portus/SmartGateway/DateBase/SystemCfg"
)

/*gateway status*/
const (
	gw_unknown = "unknown" //
	gw_ready   = "ready"
	gw_online  = "online"
	gw_offline = "offline"
	gw_exit    = "gwexit"
)

/*geteway client struct*/
type client struct {
	mutex  sync.Mutex
	status string            /*gateway statue, value is online or offline*/
	sync   chan string       /*sync chan*/
	sendQ  *cmdqueue.MyQueue /*send command*/
	recvQ  *cmdqueue.MyQueue /*recv command*/
	conn   *net.UDPConn      /*comminucation handle*/
}

//gateway client information
var gwClient client = client{status: gw_exit}

//get gate way status
func GetGatewayStatus() string {
	if gwClient.conn == nil {
		return gw_exit
	}

	return gwClient.status

}

//set gateway status
func SetGatewayStatus(status string) {
	gwClient.status = status
}

//set gate way exit
func SetGatewayExit() {
	if gwClient.conn != nil {
		gwClient.conn.Close()
		gwClient.conn = nil
		gwClient.status = gw_exit
	}
}

//Insert Request Data
func InsertClientSendQ(data interface{}) error {
	return gwClient.sendQ.Insert(data)
}

//PickOut Request Data
func PickOutClientSendQ() (interface{}, error) {
	data, err := gwClient.sendQ.Pickout()
	if err != nil {
		return nil, err
	} else {
		return data, nil
	}

}

//Insert Reply Data
func InsertClientRecvQ(data interface{}) error {
	return gwClient.recvQ.Insert(data)
}

//PickOut Reply Data
func PickOutClientRecvQ() (interface{}, error) {
	data, err := gwClient.recvQ.Pickout()
	if err != nil {
		return nil, err
	} else {
		return data, nil
	}
}

//cloudclinet init
func CloudClientInit() {
	//allow gateway to online
	if allow, _ := systemcfg.GetGatewayAllow(); allow == "off" {
		fmt.Println("gateway doesn't allow to online")
		return
	}

	/*server listen port*/
	remotePort, _ := systemcfg.GetRemoteServerPort()
	remoteAddr, _ := systemcfg.GetRemoteServerIp()

	fmt.Println("remmote:", remoteAddr+":"+remotePort)
	//udp address
	udpAddr, err := net.ResolveUDPAddr("udp4", remoteAddr+":"+remotePort)
	if err != nil {
		fmt.Println("ResolveUDPAddr Error:", err)
		return
	}

	udpConn, err := net.DialUDP("udp4", nil, udpAddr)
	if err != nil {
		fmt.Println("Dial Error:", err)
		return
	}

	//get the socket
	gwClient.conn = udpConn
	gwClient.status = gw_unknown
	gwClient.sync = make(chan string, 0)
	//init the Queue, and set the queue's capacity
	gwClient.sendQ = cmdqueue.NewMyQueueBySize(32)
	gwClient.recvQ = cmdqueue.NewMyQueueBySize(32)

}

//Cloud Client Task
func CloudClientTask() {
	if gwClient.conn == nil {
		fmt.Println("Get the socket error")
		return
	}
	fmt.Println("CloudClient Task Start...")

	defer gwClient.conn.Close()

	//Start pangolin
	go PangolinClient()
	//Start beat task
	go beatMessageTask()

	//loop
	var buff = make([]byte, 64)
	for {
		//gateway offline
		if gwClient.status == gw_exit {
			if gwClient.conn == nil {
				//exit the task
				return
			}
			//wail util timeout, do it every 15 seconds
			sleepTimer := time.NewTimer(time.Second * 15)
			<-sleepTimer.C
		}

		/*get the requestdata and send*/
		data, err := PickOutClientSendQ()
		if err != nil {
			//block 100ms
			time.Sleep(time.Microsecond * 100)
			continue
		}
		event := data.(RequestCmd).Event
		requestdata := data.(RequestCmd).Data
		timeout := data.(RequestCmd).Timeout

		fmt.Println("RequestData:", requestdata)
		gwClient.conn.Write([]byte(requestdata))

		/*receive reply data*/
		for timeout >= 0 {
			gwClient.conn.SetReadDeadline(time.Now().Add(time.Millisecond * 100))
			n, _, err := gwClient.conn.ReadFromUDP(buff)
			if err != nil {
				timeout -= 100
				if nerr, ok := err.(net.Error); ok && nerr.Timeout() {
					//fmt.Println(err)
					if timeout > 0 {
						/*block 100 microsecond*/
						time.Sleep(time.Microsecond * 100)
						continue
					}
				}
			}

			/*if recvive the date, then deal with it*/
			if n > 0 {
				/*clear timeout*/
				/*parse Message*/
				temp := buff[0:n]
				cmdreply := parseMessage(temp, n)

				/*handle repley message*/
				fmt.Println(event, cmdreply)
				if event == cmdreply[0] { //check e
					err := handleMessage(event, cmdreply)
					if err != nil { //
						gwClient.sync <- "FAIL"
					} else {
						gwClient.sync <- event
					}
				}
				break
			}
		} //end for timeout

		/*deal with timeout*/
		if timeout >= 0 {
			/*clear timeout*/
			timeout = 0
		} else { //Timeout
			gwClient.sync <- "TIMEOUT"
		}
	}
}

//DoCloudClient Message
func DoCloudClientMessage(request RequestCmd) (ReplyCmd, error) {

	gwClient.mutex.Lock()
	defer gwClient.mutex.Unlock()

	err := InsertClientSendQ(request)
	if err != nil {
		//can't instert data to SendQ
		return ReplyCmd{}, errors.New("SendDataError")
	}

	fmt.Println(request)
	sync, ok := <-gwClient.sync
	switch sync {
	case "TIMEOUT":
		fmt.Println(sync, ok)
		return ReplyCmd{}, errors.New("GetDataTimeOut")
	case "UNKNOWN":
		return ReplyCmd{}, errors.New("GetDataError")
	default: /*pick out data from RecvQ*/
		data, err := PickOutClientRecvQ()
		if err == nil {
			//get the data,then return it
			fmt.Println(sync, ok, data)
			return data.(ReplyCmd), nil
		}
	}

	return ReplyCmd{}, errors.New("GetDataError")
}

//send beat message task
func beatMessageTask() {
	const TIMEOUTMAX int = 6
	var timeoutCnt int = 0

	for {
		switch gwClient.status {
		case gw_unknown: //unknown
			_, err1 := DoRegisterMessage()
			_, err2 := DoOnlineDefaultMessage()
			if (err1 == nil) && (err2 == nil) {
				gwClient.status = gw_ready
			} else {
				timeoutCnt++
				if timeoutCnt > TIMEOUTMAX {
					gwClient.status = gw_offline
					timeoutCnt = 0
				}
			}
		case gw_ready: //ready
			_, err3 := DoURLMessage()
			if err3 == nil {
				gwClient.status = gw_online
			}
		case gw_online: //online
			data, err := DoBeatMessage()
			if err != nil {
				timeoutCnt++
				if timeoutCnt > TIMEOUTMAX {
					gwClient.status = gw_offline
					timeoutCnt = 0
				}
			} else {
				fmt.Println(data)
				gwClient.status = gw_online
				/*clear timeout count*/
				timeoutCnt = 0
			}
		case gw_offline:
			{
				//to do
				fmt.Println("gateway offline")
				SetGatewayStatus(gw_offline)
				timeoutCnt++
				if timeoutCnt > TIMEOUTMAX {
					CloudClientInit()
					timeoutCnt = 0
				}
			}
		case gw_exit:
			{
				fmt.Println("gateway exit")
				if gwClient.conn == nil {
					return
				}
			}
		}

		//fmt.Println(gwClient.status)
		//wail util timeout, do it every 10 seconds
		sleepTimer := time.NewTimer(time.Second * 10)
		<-sleepTimer.C
	}

}

//parse message, covert []byte to []string
func parseMessage(buff []byte, len int) []string {
	analMsg := make([]string, 0)
	strNow := ""
	for i := 0; i < len; i++ {
		if string(buff[i:i+1]) == ":" {
			analMsg = append(analMsg, strNow)
			strNow = ""
		} else {
			strNow += string(buff[i : i+1])
		}
	}
	analMsg = append(analMsg, strNow)
	return analMsg
}

//handle message
func handleMessage(event string, cmdreply []string) error {
	switch event {
	case GWID_EVENT:
		//to add your code
		data, err := doRegisterMessageReply(cmdreply)
		if err == nil {
			err = InsertClientRecvQ(data)
		}
		return err
	case URL_EVENT:
		data, err := doURLMessageReply(cmdreply)
		if err == nil {
			err = InsertClientRecvQ(data)
		}
		return err
	case BEAT_EVENT: //deal with beat
		data, err := doBeatMessageReply(cmdreply)
		if err == nil {
			err = InsertClientRecvQ(data)
		}
		return err
	case ONLINE_EVENT:
		//to add your code
		data, err := doOnlineMessageReply(cmdreply)
		if err == nil {
			err = InsertClientRecvQ(data)
		}
		return err
	case OFFLINE_EVENT:
		//to add your code
		data, err := doOfflineMessageReply(cmdreply)
		if err == nil {
			err = InsertClientRecvQ(data)
		}
		return err
	case EXIT_EVENT:
		//to add your code
		data, err := doOfflineMessageReply(cmdreply)
		if err == nil {
			//gateway exit
			SetGatewayExit()
			err = InsertClientRecvQ(data)
		}
		return err
	}
	return errors.New("HandleMessage Error")
}

//deal with the reply data, the return the result
func doRegisterMessageReply(data []string) (ReplyCmd, error) {
	str := strings.Join(data, ":")
	if data[1] == "OK" {
		return ReplyCmd{str}, nil
	} else {
		return ReplyCmd{}, errors.New("Register Error")
	}

}

//deal with the reply data, the return the result
func doURLMessageReply(data []string) (ReplyCmd, error) {
	str := strings.Join(data, ":")
	if data[1] == "OK" {
		return ReplyCmd{str}, nil
	} else {
		return ReplyCmd{}, errors.New("URL Error")
	}
}

//deal with the reply data, the return the result
func doBeatMessageReply(data []string) (ReplyCmd, error) {
	str := strings.Join(data, ":")
	if data[1] == "OK" {
		return ReplyCmd{str}, nil
	} else {
		return ReplyCmd{}, errors.New("Beat Error")
	}
}

//deal with the reply data, the return the result
func doOnlineMessageReply(data []string) (ReplyCmd, error) {
	str := strings.Join(data, ":")
	if data[1] == "OK" {
		return ReplyCmd{str}, nil
	} else {
		return ReplyCmd{}, errors.New("Online Error")
	}
}

//deal with the reply data, the return the result
func doOfflineMessageReply(data []string) (ReplyCmd, error) {
	str := strings.Join(data, ":")
	if data[1] == "OK" {
		return ReplyCmd{str}, nil
	} else {
		return ReplyCmd{}, errors.New("Offline Error")
	}
}

//deal with the reply data, the return the result
func doExitMessageReply(data []string) (ReplyCmd, error) {
	str := strings.Join(data, ":")
	if data[1] == "OK" {
		return ReplyCmd{str}, nil
	} else {
		return ReplyCmd{}, errors.New("Offline Error")
	}
}
