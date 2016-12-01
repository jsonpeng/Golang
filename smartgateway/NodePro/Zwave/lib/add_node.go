package zwave

import (
	"errors"
	"fmt"
)

//mode
const (
	NODE_ADD_ANY  = 0x01
	NODE_ADD_STOP = 0x05
)

func AddZwaveNode(c *DTLSClient, timeout int) error {
	zip_frm := []byte{COMMAND_CLASS_ZIP, COMMAND_ZIP_PACKET, 0x80, 0x50, 0xaa, 0x00, 0x00,
		COMMAND_CLASS_NETWORK_MANAGEMENT_INCLUSION, COMMAND_NODE_ADD, 0xbb, 0x00, NODE_ADD_ANY, 0x20}

	buff, bufflen := DTLSHandlerMSG(c, zip_frm, timeout)
	if bufflen > 0 {
		fmt.Println(bufflen, buff)
		buffdata := buff[7:]
		if buffdata[1] == COMMAND_NODE_ADD_STATUS {
			//to do
			fmt.Println("NewNodeID:", buffdata[5])
			fmt.Println("Add Node Successfull.")
			return nil
		}

	}
	return errors.New("AddNode error")
}
