package main

import (
	"fmt"
	"net"

	"github.com/portus/SmartGateway/NodePro/Zwave/lib"
)

func main() {
	ctx := zwave.NewDTLSContext()
	if !ctx.SetCipherList("PSK-AES256-CBC-SHA:PSK-AES128-CBC-SHA" /*"PSK-AES256-CCM8:PSK-AES128-CCM8"*/) {
		panic("impossible to set cipherlist")
	}
	conn, err := net.Dial("udp", "localhost:5684")
	if err != nil {
		panic(err)
	}
	c := zwave.NewDTLSClient(ctx, conn)
	c.SetPSK("Client_identity", []byte("secretPSK"))
	if _, err = c.Write([]byte("Yeah!\n")); err != nil {
		panic(err)
	}

	buff := make([]byte, 1500)
	c.Read(buff)

	fmt.Println("Rcvd:", string(buff))

	c.Close()
}
