package LoginInfo

import (
	"testing"
	"time"
)

func TestLogin(t *testing.T) {
	LoginTime := time.Now().Format("2006-01-02 15:04:05")
	TestUser1 := LoginUser{
		UserName: "everglow2",
		PassWord: "luominggao",
		Ip:       "192.168.80.101",
		Time:     LoginTime,
	}
	Login(TestUser1)
	TestUser2 := LoginUser{
		UserName: "everglow3",
		PassWord: "luominggao",
		Ip:       "192.168.80.101",
		Time:     LoginTime,
	}
	Login(TestUser2)
	TestUser3 := LoginUser{
		UserName: "everglow8",
		PassWord: "luominggao",
		Ws:       nil,
		Ip:       "192.168.80.101",
		Time:     LoginTime,
	}
	Login(TestUser3)
	TestUser6 := LoginUser{
		UserName: "everglow7",
		PassWord: "luominggao",
		Ws:       nil,
		Ip:       "192.168.80.101",
		Time:     LoginTime,
	}
	Login(TestUser6)
	TestUser7 := LoginUser{
		UserName: "everglow0",
		PassWord: "luominggao",
		Ws:       nil,
		Ip:       "192.168.80.101",
		Time:     LoginTime,
	}
	Login(TestUser7)
}
func TestExit(t *testing.T) {
	r := Exit("everglow3")
	t.Log(r)
}
func TestProvingUesr(t *testing.T) {
	r := ProvingUser("everglow0", "luominggao")
	t.Log(r)
}
func TestAllUser(t *testing.T) {
	r := AllLoginUser()
	t.Log(r)
}
