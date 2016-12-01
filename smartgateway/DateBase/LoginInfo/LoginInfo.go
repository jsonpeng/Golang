package LoginInfo

import (
	"errors"

	"github.com/gorilla/websocket"
)

type LoginUser struct {
	UserName string `json:"username"`
	PassWord string `json:"password"`
	Ws       *websocket.Conn
	Ip       string `json:"ip"`
	Time     string `json:"time"`
}

var AllUserIM []LoginUser

//var LoginUserMap map[string]LoginUser
var LoginUserMap = make(map[string]LoginUser, 20)

func Login(loginUser interface{}) error {
	joinUser := loginUser.(LoginUser)
	//LoginUserMap = make(map[string]LoginUser)
	AllUserIM = append(AllUserIM, joinUser)
	for userNowLength, userInfo := range AllUserIM {
		LoginUserMap[userInfo.UserName] = AllUserIM[userNowLength]
	}
	return nil
}
func Exit(userName string) error {
	_, ok := LoginUserMap[userName]
	if ok {
		for userNowLength, userInfo := range AllUserIM {
			if userInfo.UserName == userName {
				AllUserIM = append(AllUserIM[:userNowLength], AllUserIM[userNowLength+1:]...)
			}

		}
		delete(LoginUserMap, userName)
		return nil
	} else {
		return errors.New("Exit Failed")
	}
}
func AllLoginUser() []LoginUser {
	return AllUserIM
}
func ProvingUser(userName string, passWord string) error {
	_, ok := LoginUserMap[userName]
	if ok {
		if LoginUserMap[userName].PassWord == passWord {
			return nil
		}
		return errors.New("Password Wrong")
	} else {
		return errors.New("UserName Is Not Exist")
	}

}
