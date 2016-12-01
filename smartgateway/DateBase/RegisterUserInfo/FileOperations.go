//copyright 2016 Portus(Wuhan), All rights reserved.
//@Author: minggao
//@Date  : 2016/07/29
//@upDate:   2016/08/24
//the code maybe will update
package RegisterUserInfo

import (
	"encoding/json"
	"errors"
	"fmt"
	"io/ioutil"
	"os"
)

func checkErr(err error) {
	if err != nil {
		fmt.Println(err)
	}
}
func existFile(filename string) (*os.File, error) {
	fO, err := os.OpenFile(filename, os.O_RDWR, os.ModePerm)
	defer fO.Close()
	if err != nil && os.IsNotExist(err) {
		fN, err := os.Create(filename)
		checkErr(err)
		return fN, errors.New("New Created")
	} else {
		fmt.Println("file is exist")
		return fO, nil
	}
}
func getFile(f *os.File) (userStruct UserStruct) {
	var userFile UserStruct
	userFile.User = make(map[string]Registry)
	Getfile, _ := ioutil.ReadFile(f.Name())
	f.Close()
	json.Unmarshal(Getfile, &userFile)
	return userFile
}
