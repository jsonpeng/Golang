//copyright 2016 Portus(Wuhan). All rights reserved.
//@Author: Lewis
//@Date:   2016/07/27
//@Version:

/*
  This Package implements a set of simple functions.
  If you have and questions, please don't hesitate to connect us.
*/
package main

import (
	//add your package
	"github.com/portus/SmartGateway/CloudClient"
	"github.com/portus/SmartGateway/ComandTask"
	//"github.com/portus/SmartGateway/HomeServer"

	"github.com/portus/SmartGateway/NodeClient"
	"github.com/portus/SmartGateway/TestServer"
)

func main() {
	/*Init something, and golang can auto-call the function named "init",
	  before the main function is running. But I prefer to call them
	  in the main function.
	*/

	cloudclient.CloudClientInit()
	commandtask.CommandTaskInit()
	nodeclient.NodeClientInit()

	/*create goroutine, we can create gorouine to deal with things in each module.
	  you can add your code here. According the portus system, we at least need four goroutines.
	  1) cloud client task, it can send from for receive to data to the cloud.
	  2) user command  task, it can accept user command and process it.
	  3) home web server task, it has some ui and can display by browser.
	  4) node client task , it can communicate with the nodes that can contral the devices.
	*/
	go cloudclient.CloudClientTask()
	go commandtask.CommandTask()
	//go homeserver.HomeServerTask()
	go nodeclient.NodeClientTask()
	go testserver.TestServerTask()
	select {}
}
