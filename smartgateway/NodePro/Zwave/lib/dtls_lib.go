package zwave

import "C"

import (
	"encoding/hex"
	"fmt"
	"net"
	"time"
)

func DTLSInit(url, psk string) *DTLSClient {
	ctx := NewDTLSContext()
	if !ctx.SetCipherList("PSK-AES256-CBC-SHA:PSK-AES128-CBC-SHA" /*"PSK-AES256-CCM8:PSK-AES128-CCM8"*/) {
		fmt.Println("impossible to set cipherlist")
		return nil
	}
	fmt.Println("Dial")

	conn, err := net.Dial("udp", url) //UZB IP:Port
	if err != nil {
		fmt.Println("DTLSInit:", err)
		return nil
	}
	fmt.Println(conn.RemoteAddr().String())
	c := NewDTLSClient(ctx, conn)
	if c == nil {
		fmt.Println("NewDTLSClient error")
		return nil
	}

	fmt.Println("SetPSK")
	psk_byte, _ := hex.DecodeString(psk) //PSK
	c.SetPSK("Client_identity", psk_byte)
	return c

}

func DTLSHandlerMSG(c *DTLSClient, zip_frm []byte, timeout int) ([]byte, int) {
	fmt.Printf("-->[%d]: ", len(zip_frm))
	for i := 0; i < len(zip_frm); i++ {
		fmt.Printf("%02x ", zip_frm[i])
	}
	fmt.Printf("%s", "\r\n")

	if _, err := c.Write(zip_frm); err != nil {
		fmt.Println("Write error:", err)
		//return nil, 0
	}

	buff := make([]byte, 1500)
	bufflen := 0
	for newtime := 0; newtime < timeout; newtime += 100 {
		c.SetDeadline(time.Now().Add(time.Microsecond * 100))
		datalen, _ := c.Read(buff)
		if datalen <= 0 {
			//can't receive data
			continue
		} else {
			fmt.Printf("<-[%d]: ", int(datalen))
			for i := 0; i < datalen; i++ {
				fmt.Printf("%02x ", buff[i])
			}
			fmt.Printf("%s", "\r\n")

			//deal with command
			switch buff[0] {
			case COMMAND_CLASS_ZIP:
				{
					//respond, but no command class
					if (buff[2]&0x40 == 0x40) && (datalen <= 7) {
						//to do
						continue
					}
				}
			case COMMAND_CLASS_ZIP_ND:

			default:
				break
			}

			bufflen = datalen
			return buff[:datalen], bufflen
		}
	}

	return nil, 0
}
