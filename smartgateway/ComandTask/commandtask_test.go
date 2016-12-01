package commandtask

import "testing"
import "fmt"
import "time"
import "github.com/portus/SmartGateway/DateBase/LoginInfo"

func TestCommandTask(t *testing.T) {
	fmt.Println("test begin ...")
	LoginTime := time.Now().Format("2006-01-02 15:04:05")
	TestUser1 := LoginInfo.LoginUser{
		UserName: "everglow2",
		PassWord: "luominggao",
		Ip:       "192.168.80.101",
		Time:     LoginTime,
	}
	LoginInfo.Login(TestUser1)
	TestUser2 := LoginInfo.LoginUser{
		UserName: "everglow3",
		PassWord: "luominggao",
		Ip:       "192.168.80.101",
		Time:     LoginTime,
	}
	LoginInfo.Login(TestUser2)
	TestUser3 := LoginInfo.LoginUser{
		UserName: "everglow8",
		PassWord: "luominggao",
		Ip:       "192.168.80.101",
		Time:     LoginTime,
	}
	LoginInfo.Login(TestUser3)
	for k, v := range LoginInfo.AllLoginUser() {
		fmt.Println(k, v)
	}
	CommandTaskInit()
	CommandTask()
}
