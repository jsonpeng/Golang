//copyright 2016 Portus(Wuhan), All rights reserved.
//@Author: minggao
//@Date  : 2016/07/29
//@upDate:   2016/08/24
//the code maybe will update
//The package can be used to carry out the user's registration.
//The package can be used for the user's registration, delete, update, search.
//import the RegisterUserInfo
package RegisterUserInfo

import (
	"encoding/json"
	"errors"
	"fmt"
	"io/ioutil"
	"os"
	"strconv"
)

var account = 1

//This is a structure that contains three variables.
/*
Including:
*/
/*
username ,password, online
*/
type Registry struct {
	UId      int64  `json:"uid"`
	Username string `json:"username"`
	Password string `json:"password"`
	Online   string `json:"online"`
	Language string `json:"language"`
}

const DefaultStatue = OFFLINE
const (
	ONLINE  = "online"
	OFFLINE = "offonline"
)

//Define a map
type UserStruct struct {
	User map[string]Registry `json:user`
}

const Registername = "user.conf"
const Account = 0

/*  Register Function */
//input : interface{}
//out   :error  return nil:success
func RegisterUser(userinfo interface{}) error {
	var userJoin Registry
	userJoin = userinfo.(Registry)
	f, err := existFile(Registername)
	if err == nil {
		userFile := getFile(f)
		account := len(userFile.User)
		for i := 1; i <= account; i++ {
			Number := "user" + strconv.Itoa(i)
			if userFile.User[Number].Username == userJoin.Username {
				return errors.New("Have been Register")
			}
		}
		userNumber := "user" + strconv.Itoa(account+1)
		userFile.User[userNumber] = userJoin
		os.Remove(f.Name())
		fN, err := os.Create(Registername)
		if err != nil {
			fmt.Println("create file failed")
		}
		Input, err := json.MarshalIndent(userFile, "", "\t\t")
		if err != nil {
			fmt.Println("Error marshalling to JSON:", err)
		}
		ioutil.WriteFile(fN.Name(), Input, 0644)
		if err != nil {
			return err
		}
		return nil
	} else {
		userNumber := "user" + strconv.Itoa(account)
		userFile := getFile(f)
		userFile.User[userNumber] = userJoin
		Input, err := json.MarshalIndent(userFile, "", "\t\t")
		if err != nil {
			return err
		}
		ioutil.WriteFile(f.Name(), Input, 0644)
		if err != nil {
			return err
		}
		return nil
	}
}

/* Seek Function */
//input username
// out :error,interface{},nil:success
func SeekUser(username string) (error, Registry) {
	f, err := existFile(Registername)
	checkErr(err)
	AllUser := getFile(f)
	account = len(AllUser.User)
	for i := 1; i <= account; i++ {
		Number := "user" + strconv.Itoa(i)
		if AllUser.User[Number].Username == username {
			return nil, AllUser.User[Number]
		}
	}
	return errors.New("Have Not Found"), Registry{}
}

/* Delete Function */
//input:username
//out:error return nil:success
func DeleteUser(username string) error {
	f, err := existFile(Registername)
	checkErr(err)
	AllUser := getFile(f)
	account = len(AllUser.User)
	for i := 1; i <= account; i++ {
		Number := "user" + strconv.Itoa(i)
		if AllUser.User[Number].Username == username {
			delete(AllUser.User, Number)
			fmt.Println(i, account)
			for j := i; j < account; j++ {
				fmt.Println("1")
				Number = "user" + strconv.Itoa(j)
				Copy := "user" + strconv.Itoa(j+1)
				AllUser.User[Number] = AllUser.User[Copy]
				fmt.Println()
				delete(AllUser.User, Copy)
			}
			os.Remove(f.Name())
			fN, _ := os.Create(Registername)
			Input, err := json.MarshalIndent(AllUser, "", "\t\t")
			checkErr(err)
			ioutil.WriteFile(fN.Name(), Input, 0644)
			return nil
		}
	}
	return errors.New("Delete Failed")
}

/* Update Function */
//input :username ,interface{}
//out: error return nil:success
func UpdateUser(username string, user Registry) error {
	f, err := existFile(Registername)
	checkErr(err)
	AllUser := getFile(f)
	account = len(AllUser.User)
	for i := 1; i <= account; i++ {
		Number := "user" + strconv.Itoa(i)
		if AllUser.User[Number].Username == username {
			AllUser.User[Number] = user
			os.Remove(f.Name())
			fN, _ := os.Create(Registername)
			Input, err := json.MarshalIndent(AllUser, "", "\t\t")
			checkErr(err)
			ioutil.WriteFile(fN.Name(), Input, 0644)
			return nil
		}
	}
	return errors.New("Update Failed")
}

/* Online Function */
//input:username
//out:bool return true:user is online to cloud
func OnlineUser(username string) string {
	f, err := existFile(Registername)
	checkErr(err)
	AllUser := getFile(f)
	account = len(AllUser.User)
	for i := 1; i <= account; i++ {
		Number := "user" + strconv.Itoa(i)
		if AllUser.User[Number].Username == username {
			fmt.Println("1:", AllUser.User[Number].Online)
			return AllUser.User[Number].Online
		}
	}
	return "user is not exist"
}
func SelectLanguage(username string) string {
	f, err := existFile(Registername)
	checkErr(err)
	AllUser := getFile(f)
	account = len(AllUser.User)
	for i := 1; i <= account; i++ {
		Number := "user" + strconv.Itoa(i)
		if AllUser.User[Number].Username == username {
			return AllUser.User[Number].Language
		}
	}
	return "Failed to Get"
}
func AllUserInfo() (allUser UserStruct) {
	f, err := existFile(Registername)
	checkErr(err)
	return getFile(f)
}
func SeekUserById(uid int64) (username string) {
	f, err := existFile(Registername)
	checkErr(err)
	AllUser := getFile(f)
	account = len(AllUser.User)
	for i := 1; i <= account; i++ {
		Number := "user" + strconv.Itoa(i)
		if AllUser.User[Number].UId == uid {
			return AllUser.User[Number].Username
		}
	}
	return ""
}
