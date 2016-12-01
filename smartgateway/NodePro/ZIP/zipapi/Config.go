package zipapi

/*
//--Note-- : There is cgo comment. DONOT change it

#cgo linux CFLAGS:-I../external-lib/openssl/install/include/openssl  -I../external-lib/openssl/install/include
#cgo linux CFLAGS:-I../external-lib/zware_c_api_v7.02/include 	-I../external-lib/zware_c_api_v7.02/include/zwave
#cgo linux CFLAGS:-I.
#cgo linux CFLAGS:-DOS_LINUX -DZIP_V2 -DUSE_OPENSSL -DCONFIG_DEBUG  -DWKUP_BY_MAILBOX_ACK  -pthread -Wall
#cgo linux LDFLAGS:-L../external-lib/zware_c_api_v7.02/lib -L../external-lib/zware_c_api_v7.02/src -L../external-lib/openssl/install/lib
#cgo linux LDFLAGS:-lzip_ctl -lzip_api -lm -lpthread -lrt -lssl -lcrypto -ldl



#include <Appl.h>

//--Note-- : There must be no blank lines in between the cgo comment and the import statement
*/
import "C"

import (
	"encoding/json"
	"errors"
	"fmt"
	"io/ioutil"
	"net"
	"os"
	"unsafe"
)

type ZipJson struct {
	ZIP []ZIPType
}

type ZIPType struct {
	ZipLanPort  uint16
	ZipRouterIP string
	DTLSPSK     string
}

//type JsonStruct struct {
//}

var (
	Zip ZipJson
	//	JsonPaser JsonStruct
	fileload bool = false
)

func UpdateConfigFile(file string, zip ZipJson) error {
	filejson, err := json.MarshalIndent(zip, "", "\t")
	if err != nil {
		fmt.Println("Marshal Json data error:", err)
		return err
	}
	err1 := ioutil.WriteFile(file, filejson, os.ModeAppend.Perm())
	if err1 != nil {
		fmt.Println("Write data to file error:", err)
		return err1
	}
	return err
}

func LoadConfigFile(filename string, v interface{}) error {
	file, err := ioutil.ReadFile(filename)
	if err != nil {
		fmt.Println("Read file error:", err)
		return err
	}
	filejson := []byte(file)
	err = json.Unmarshal(filejson, v)
	if err != nil {
		fmt.Println("Json Unmarshal error:", err)
		return err
	}
	return nil
}

func ConfigParamList(file string) (num uint16, err error) {
	if fileload == true {
		err = LoadConfigFile(file, &Zip)
	}
	//	UpdateConfigFile(file, Zip)
	return uint16(len(Zip.ZIP)), err
}

// Get index host_port,router,psk from filename
func ConfigParamGet(filename string, index uint16) (host_port uint16, router string, psk string, err error) {
	num, err := ConfigParamList(filename)
	if err != nil {
		fmt.Println("ConfigParamGet errors	 ", err)
	}
	if index < num {
		host_port = Zip.ZIP[index].ZipLanPort
		router = Zip.ZIP[index].ZipRouterIP
		psk = Zip.ZIP[index].DTLSPSK
	} else {

		err = errors.New("inputted index out of range")
		fmt.Println(err)
	}
	return host_port, router, psk, err
}

func ConfigParamNew(file string, ip []net.IPAddr) error {
	var temp ZIPType = ZIPType{0, "0", "123456789012345678901234567890AA"}
	Zip.ZIP = make([]ZIPType, 0, 1000) //clean Zip buffer data

	fileload = true
	for i := 0; i < len(ip); i++ {
		temp.ZipRouterIP = ip[i].String()
		Zip.ZIP = append(Zip.ZIP, temp)
	}
	return UpdateConfigFile(file, Zip)
}

//export CConfigParamGet
func CConfigParamGet(file *C.char, port *C.uint16_t, addr, psk *C.char) int {
	file_str := C.GoString(file)
	host_port, router, psk1, err := ConfigParamGet(file_str, 0)
	if err != nil {
		fmt.Println("CConfigParamGet errors:", err)
		return -1
	}
	*port = C.uint16_t(host_port)

	addr2 := C.CString(router)
	psk2 := C.CString(psk1)
	C.memcpy(unsafe.Pointer(addr), unsafe.Pointer(addr2), C.size_t(len(router)))
	C.memcpy(unsafe.Pointer(psk), unsafe.Pointer(psk2), C.size_t(len(psk1)))
	return 0
}
