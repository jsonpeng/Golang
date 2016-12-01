package controllers

import (
	"fmt"
	"time"

	"github.com/portus/SmartGateway/DateBase/RegisterUserInfo"
)

type HomeController struct {
	CommonController
}

func (c *HomeController) Get() {
	err, _ := RegisterUserInfo.SeekUser(Administrator)
	//can't get the admin information
	if err != nil {
		c.TplName = c.GetTplName("main.html")
	} else {
		c.Data["IsReg"] = true
		c.TplName = c.GetTplName("main.html")
		return
	}

}

func (c *HomeController) Post() {
	userName := Administrator
	passWord := c.Input().Get("pass")
	comPassWord := c.Input().Get("confirm_pass")
	fmt.Println("password:", passWord, "comPassWord:", comPassWord)

	language := DefaultLang
	createUser := RegisterUserInfo.Registry{time.Now().Unix(), userName, passWord, RegisterUserInfo.DefaultStatue, language}
	err := RegisterUserInfo.RegisterUser(createUser)
	if err == nil && passWord == comPassWord {
		c.ReportError(SUCCESS)
		return
	} else {
		c.ReportError(FAILED)
		return
	}
}
