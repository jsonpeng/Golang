//copyright 2016 Portus(Wuhan), All rights reserved.
//@Author: minggao
//@Date  : 2016/07/29
//@upDate:   2016/08/03
//the code maybe will update
package RegisterUserInfo

import (
	"fmt"
	"io/ioutil"
	"os"
	"strings"
)

/* To determine if the file exists */
//input :filename
//out:File pointer type
func exist_file(filename string) *os.File {
	fO, err := os.OpenFile(filename, os.O_RDWR, os.ModePerm)
	defer fO.Close()
	if err != nil && os.IsNotExist(err) {
		fN, err := os.Create(filename)
		check_err(err)
		return fN
	} else {
		return fO
	}
}

/* Some err judgment */
//input :error
func check_err(err error) {
	if err != nil {
		fmt.Println(err)
	}
}

/* Get the contents of the file */
// input:File pointer type
// out: []string is all username's name,other is all username's information
//because sometime we only need username's name.
func get_file(f *os.File) ([]string, []string) {
	content, err := ioutil.ReadFile(f.Name())
	check_err(err)
	if len(content) == 0 {
		defer f.Close()
		return nil, nil
	}
	defer f.Close()
	analMsg := make([]string, 0)
	strNow := ""
	for i := 0; i < len(content); i++ {
		if string(content[i]) == "\n" {
			analMsg = append(analMsg, strNow)
			strNow = ""
		} else {
			strNow += string(content[i])
		}
	}
	analMsg = append(analMsg, strNow)
	Recive := make([]string, 0)
	for _, slice := range analMsg {
		for size, info := range strings.Split(slice, ",") {
			if size%3 == 0 {
				Recive = append(Recive, info)
			} else {
				continue
			}
		}
	}
	return Recive, analMsg
}
