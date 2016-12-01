package zwave

import (
	"fmt"
	"net"
	//"strings"
	"time"
)

//
//
func ScanGateway(localAddr *net.UDPAddr, timeout int) []net.IP {
	ipList := make([]net.IP, 0, 10)
	zip_frm := []byte{COMMAND_CLASS_ZIP, COMMAND_ZIP_PACKET, 0x00, 0x40, 0xaa, 0x00, 0x00,
		COMMAND_CLASS_NETWORK_MANAGEMENT_PROXY, COMMAND_NODE_INFO_CACHED_GET, 0xbb, 0x00, 0x00}

	socket, err := net.ListenUDP("udp4", localAddr)
	if err != nil {
		fmt.Println("dail error!", err)
		return nil
	}
	defer socket.Close()

	// send data
	senddata := zip_frm
	fmt.Println(senddata)
	_, err = socket.WriteToUDP(senddata, &net.UDPAddr{
		IP:   net.IPv4(255, 255, 255, 255),
		Port: 4123,
	})
	if err != nil {
		fmt.Println("write error!", err)
		return nil
	}

	// receive data
	data := make([]byte, 128)
	newtime := 0
	for ; newtime <= timeout; newtime += 5000 {
		socket.SetDeadline(time.Now().Add(time.Second * 5))

		read, remoteAddr, err := socket.ReadFromUDP(data)
		if err != nil {
			continue
		}

		if read > 0 {
			fmt.Println(read, data[:read])
			if data[0] == COMMAND_CLASS_ZIP {
				if data[9] == zip_frm[9] {
					fmt.Println("ip = ", remoteAddr.IP)
					fmt.Println("port = ", remoteAddr.Port)

					ipList = append(ipList, remoteAddr.IP)
				}
			}
		}
	}

	return ipList
}
