//copyright 2016 Portus(Wuhan). All rights reserved.
//@Author: Lewis
//@Date:   2016/08/04
//@Version:

/*
  This Package implements a set of simple functions.
  If you have and questions, please don't hesitate to connect us.
*/
package nodeclient

import (
	"fmt"
	//"net"
	"errors"
	"sync"
	"time"

	"github.com/portus/SmartGateway/CmdQueue"
)

type client struct {
	mutex   sync.Mutex
	sendUiQ *cmdqueue.MyQueue /*send webui command, higher authority*/
	sendGwQ *cmdqueue.MyQueue /*send gateway command, lower authority*/
	recvUiQ *cmdqueue.MyQueue /*recv webui command, higher authority*/
	recvGwQ *cmdqueue.MyQueue /*recv gateway command, lower authority*/
	syncCh  chan string       /*sync channel*/
}

var nodeClient client = client{}

//Node client init
func NodeClientInit() {
	//to add your code
	//init the Queue, and set the queue's capacity
	nodeClient.sendUiQ = cmdqueue.NewMyQueueBySize(16)
	nodeClient.sendGwQ = cmdqueue.NewMyQueueBySize(16)
	nodeClient.recvUiQ = cmdqueue.NewMyQueueBySize(16)
	nodeClient.recvGwQ = cmdqueue.NewMyQueueBySize(16)
	nodeClient.syncCh = make(chan string)
}

//Node client task
func NodeClientTask() {
	fmt.Println("NodeClientTask start....")

	//creat gateway task
	creatGatewayTask()

	//loop
	for {
		source := false //default: data source come form gw
		data, err := nodeClient.sendUiQ.Pickout()
		if err != nil {
			if data, err = nodeClient.sendGwQ.Pickout(); err != nil {
				/*can't get the command from the queuem, then block 200ms*/
				time.Sleep(time.Microsecond * 200)
				continue
			}
		} else {
			source = true //data source come form ui
		}

		/*deal with the message*/
		if result, err := DealNodeClientMessage(data); err != nil {
			//to add your code
			nodeClient.syncCh <- "FAIL"
		} else {
			if source {
				nodeClient.recvUiQ.Insert(result)
				nodeClient.syncCh <- "OK"
			} else {
				nodeClient.recvGwQ.Insert(result)
				nodeClient.syncCh <- "OK"
			}

		}

	}
}

//Do node client webui message
func DoNodeClientUiMessage(e interface{}) (interface{}, error) {
	nodeClient.mutex.Lock()
	defer nodeClient.mutex.Unlock()
	//insert data to queue
	if err := nodeClient.sendUiQ.Insert(e); err != nil {
		return nil, errors.New("SendDataError")
	}

	sync, _ := <-nodeClient.syncCh
	if sync == "OK" {
		if data, err := nodeClient.recvUiQ.Pickout(); err == nil {
			return data, nil
		}
	}

	return nil, errors.New("GetDataError")

}

//Do node client Gateway message
func DoNodeClientGwMessage(e interface{}) (interface{}, error) {
	nodeClient.mutex.Lock()
	defer nodeClient.mutex.Unlock()
	//insert data to queue
	if err := nodeClient.sendGwQ.Insert(e); err != nil {
		return nil, errors.New("SendDataError")
	}

	sync, _ := <-nodeClient.syncCh
	if sync == "OK" {
		//pickout data from queue
		if data, err := nodeClient.recvGwQ.Pickout(); err == nil {
			//do add your code
			return data, nil
		}
	}

	//FAIL
	return nil, errors.New("GetDataError")
}

//Gateway task, get energy meter data
func EnergyMeterDataTask() {
	for {
		fmt.Println("EnergyMeter Task is running...")
		for i := 0; i < 1; i++ {
			GetEnergyMeterData()
		}
		sleepTimer := time.NewTimer(time.Second * 200)
		<-sleepTimer.C

	}
}

//Gateway task, get Temperature
func TemperatureDataTask() {
	for {
		fmt.Println("Temperature Task is running...")
		for i := 0; i < 1; i++ {
			GetTemperatureData()
		}

		sleepTimer := time.NewTimer(time.Second * 100)
		<-sleepTimer.C

	}
}

//Gateway task, get humidity
func HumidityDataTask() {
	for {
		fmt.Println("Humidity Task is running...")
		for i := 0; i < 1; i++ {
			GetHumidityData()
		}

		sleepTimer := time.NewTimer(time.Second * 60)
		<-sleepTimer.C
	}
}

//Creat gateway Task, and check the task to work
func creatGatewayTask() {
	//add your task
	go EnergyMeterDataTask()
	go TemperatureDataTask()
	go HumidityDataTask()

}
