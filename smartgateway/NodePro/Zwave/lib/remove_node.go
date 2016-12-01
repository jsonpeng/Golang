package zwave

import (
	"errors"
	"fmt"
)

func RemoveZwaveNode(c *DTLSClient, timeout int) error {
	zip_frm := []byte{COMMAND_CLASS_ZIP, COMMAND_ZIP_PACKET, 0x80, 0x50, 0xaa, 0x00, 0x00,
		COMMAND_CLASS_NETWORK_MANAGEMENT_INCLUSION, COMMAND_NODE_REMOVE, 0xbb, 0x00, NODE_ADD_ANY}

	buff, bufflen := DTLSHandlerMSG(c, zip_frm, timeout)
	if bufflen > 0 {
		fmt.Println(bufflen, buff)
		buffdata := buff[7:]
		if buffdata[1] == COMMAND_NODE_REMOVE_STATUS {
			//to do
			fmt.Println("DelNodeID:", buffdata[4])
			fmt.Println("Remove Node Successfull.")
			return nil
		}
	}
	return errors.New("RemoveNode error")
}
