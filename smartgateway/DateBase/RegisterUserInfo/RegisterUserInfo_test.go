//copyright 2016 Portus(Wuhan), All rights reserved.
//@Author: minggao
//@Date  : 2016/07/29
//@upDate:   2016/08/24
//the code maybe will update
package RegisterUserInfo

import (
	"testing"
	"time"
)

func TestRegisterUser(t *testing.T) {
	a := Registry{
		UId:      time.Now().Unix(),
		Username: "everglow5",
		Password: "123456",
		Online:   DefaultStatue,
		Language: "Chinese",
	}
	r := RegisterUser(a)
	t.Log(DefaultStatue, r)
	b := Registry{
		UId:      time.Now().Unix(),
		Username: "everglow5",
		Password: "123456",
		Online:   DefaultStatue,
		Language: "Chinese",
	}
	r = RegisterUser(b)
	t.Log(DefaultStatue, r)

}
func TestSeekUser(t *testing.T) {
	r1, r2 := SeekUser("everglow5")
	t.Log(r1, r2)
}

func TestDeleteUser(t *testing.T) {
	r := DeleteUser("everglow1")
	t.Log(r)
}
func TestUpateUser(t *testing.T) {
	b := Registry{
		Username: "everglow2",
		Password: "luominggao",
		Online:   DefaultStatue,
		Language: "English",
	}
	r := UpdateUser("everglow2", b)
	t.Log(r)
}

func TestOnlineUser(t *testing.T) {
	r := OnlineUser("everglow5")
	t.Log(r)
}
func TestSelectLanguage(t *testing.T) {
	r := SelectLanguage("everglow3")
	t.Log(r)
}
func TestAllUserInfo(t *testing.T) {
	r := AllUserInfo()
	t.Log(r)
}
func TestSeekUserById(t *testing.T) {
	var r int64
	r = 1473300272
	t.Log(SeekUserById(r))
}
