//copyright 2016 Portus(Wuhan). All rights reserved.
//@Author: Lewis
//@Date:   2016/08/05
//@Version:

/*
  This Package implements a set of simple functions.
  If you have and questions, please don't hesitate to connect us.
*/
package debugout

import "fmt"

//debug level
const (
	DBG_ENABLE  = 0x00
	DBG_INFO    = 0x01
	DBG_WARN    = 0x02
	DBG_ERROR   = 0x03
	DBG_DISABLE = 0x0F
)

//moudle name and print level
var moudleList = make(map[string]int)

//save log to file
var logEnable = false

//Set debug Level
func SetDebugLevel(module string, level int) {
	moudleList[module] = level
}

//set logenable
func SetLogEnable(flag bool) {
	logEnable = flag
}

//Debug Out, can control the debug-level
func DebugOut(module string, level int, e ...interface{}) {
	v, ok := moudleList[module]
	if !ok {
		//nothing
		return
	}

	if v <= level {
		fmt.Println("["+module+"]", e)

		if logEnable {
			//add your code, save to log
			//to do
		}
	}
}

//print information
func PrintInfo(module string, e ...interface{}) {
	DebugOut(module, DBG_INFO, e)
}

//print error
func PrintError(module string, e ...interface{}) {
	DebugOut(module, DBG_ERROR, e)
}
