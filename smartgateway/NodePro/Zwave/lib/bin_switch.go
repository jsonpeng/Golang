package zwave

import (
	"errors"
	"fmt"
)

///
func GetBinarySwitchStatus(c *DTLSClient, timeout int) (byte, error) {
	zip_frm := []byte{COMMAND_CLASS_ZIP, COMMAND_ZIP_PACKET, 0x80, 0x40, 0xaa, 0x00, 0x00,
		COMMAND_CLASS_SWITCH_BINARY, SWITCH_BINARY_GET}

	buff, bufflen := DTLSHandlerMSG(c, zip_frm, timeout)
	if bufflen > 7 {
		netbuff := buff[7:bufflen]
		if netbuff[0] == COMMAND_CLASS_SWITCH_BINARY {
			//to do
			fmt.Println("Get Status:", netbuff[2])
			return netbuff[2], nil
		}
	}

	return 0x00, errors.New("Get BinarySwitch Status error")
}

////
func TurnOnBinarySwitch(c *DTLSClient, timeout int) error {
	zip_frm := []byte{COMMAND_CLASS_ZIP, COMMAND_ZIP_PACKET, 0x80, 0x40, 0xaa, 0x00, 0x00,
		COMMAND_CLASS_SWITCH_BINARY, SWITCH_BINARY_SET, 0xff}

	buff, bufflen := DTLSHandlerMSG(c, zip_frm, timeout)
	if bufflen > 7 {
		//to do
		netbuff := buff[7:bufflen]
		if netbuff[0] == COMMAND_CLASS_SWITCH_BINARY {

			return nil
		}
	}
	return errors.New("Turn On BinarySwitch error")
}

////
func TurnOffBinarySwitch(c *DTLSClient, timeout int) error {
	zip_frm := []byte{COMMAND_CLASS_ZIP, COMMAND_ZIP_PACKET, 0x80, 0x40, 0xaa, 0x00, 0x00,
		COMMAND_CLASS_SWITCH_BINARY, SWITCH_BINARY_SET, 0x00}

	buff, bufflen := DTLSHandlerMSG(c, zip_frm, timeout)
	if bufflen > 7 {
		netbuff := buff[7:bufflen]
		if netbuff[0] == COMMAND_CLASS_SWITCH_BINARY {
			//to do
			return nil
		}
	}
	return errors.New("Turn Off BinarySwitch error")
}
