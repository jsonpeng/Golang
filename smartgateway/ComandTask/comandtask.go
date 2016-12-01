//copyright 2016 Portus(Wuhan). All rights reserved.
//@Author: Lewis
//@Date:   2016/07/27
//@Version:

/*
  This Package implements a set of simple functions.
  If you have and questions, please don't hesitate to connect us.
*/
package commandtask

import (
	//"fmt"
	"net"
	"strconv"
	"strings"

	"github.com/portus/SmartGateway/DebugOut"
)

const listenPort int = 6969

func CommandTaskInit() {
	//to do , add your code

}

//Command Task
func CommandTask() {

	tcpAddr, err := net.ResolveTCPAddr("tcp4", ":"+strconv.Itoa(listenPort))
	tcpListener, err := net.ListenTCP("tcp4", tcpAddr)

	/*check listentcp*/
	if err != nil {
		debugout.DebugOut("commandtask", debugout.DBG_ERROR, "Error listening:", err)
		return
	}
	defer tcpListener.Close()

	for {
		newconn, err := tcpListener.Accept()
		if err != nil {
			debugout.DebugOut("commandtask", debugout.DBG_ERROR, "Error accepting:", err)
		}

		/*check something*/
		if checkUserAndPwd() {
			/*all right. Then handle the request*/
			go handleRequest(newconn)
		}
	}
}

//Check user and password
func checkUserAndPwd() bool {
	//add your code
	return true
}

//Handle Request
func handleRequest(conn net.Conn) {
	defer conn.Close()
	handle := true
	for handle {
		//to do, add you code and logic
		var buf = make([]byte, 256)
		/*set read timeout 100ms*/
		//conn.SetReadDeadline(time.Now().Add(time.Microsecond * 100))
		n, err := conn.Read(buf)
		if err != nil {
			if nerr, ok := err.(net.Error); ok && nerr.Timeout() {
				continue
			}
		}

		if n > 0 {
			/*get the reveive data*/
			temp := buf[0:n]
			debugout.DebugOut("commandtask", debugout.DBG_INFO, "len:", len(temp), "data:", temp)
			/*parse command, and get the command len*/
			cmdlist := strings.Fields(string(temp))
			//cmdlen := len(cmdlist)

			if cmdlist != nil {
				if Func, ok := commandlist[cmdlist[0]]; ok {
					cmdvalue := cmdlist[1:]
					result, err := Func(cmdvalue)
					if err == nil {
						conn.Write([]byte(cmdlist[0] + result + "\r\n"))
					} else {
						conn.Write([]byte("Have Wrong Done"))
					}
				}

			}
		}
	}
}
