//copyright 2016 Portus(Wuhan). All rights reserved.
//@Author: Lewis
//@Date:   2016/09/07
//@Version:

/*
  This Package implements a set of simple functions.
  If you have and questions, please don't hesitate to connect us.
*/
package controllers

import (
	"fmt"
	"strconv"
	"strings"

	"github.com/astaxie/beego"
	"github.com/astaxie/beego/session"
	"github.com/beego/i18n"

	"github.com/portus/SmartGateway/DateBase/LoginInfo"
)

//Administrator
const Administrator = "admin"

//default language
const DefaultLang = Chinese

//language list
const (
	Chinese = "zh-CN"
	English = "en-US"
	//to add your language
)

//error list
const (
	SUCCESS = 0x00 //成功
	FAILED  = 0x01 //失败

	//LOCAL
	COMPASSWORD_NULL      = 0x09 //验证密码为空
	USERNAME_NIL          = 0x10 //用户名为空
	PASSWORD_NIL          = 0x11 //密码为空
	NO_USERNAME           = 0x12 //用户名不存在
	INVALID_USERNAME      = 0x13 //用户名非法
	INVALID_PASSWORD      = 0x14 //密码不正确
	NOSAME_PASSWORD       = 0x15 //两次密码不一致
	REGISTER_USER         = 0x16 //用户已经注册
	LONGINNED_USER        = 0x17 //用户已登录
	ERROR_USERINFORMATION = 0X18 //清除了Session
	NO_UID                = 0x19 //uid为空
	INVALID_UID           = 0x20 //uid不存在
	//SERVER
	SERVER_NORUNNING = 0x21 //未启动服务器
	SERVER_NOFOUND   = 0x22 //未发现服务器
	SERVER_ISBUSY    = 0x23 //服务器忙
	ACCESS_TIMEOUT   = 0x24 //访问超时
	NO_LOGIN         = 0x25 //用户未登录
	//OTHER
	INVALID_PARAMETER = 0x30 //参数非法
	ACCESS_OUTRANGE   = 0x31 //超出范围

	//to add
	CONTACT_ADMIN    = 0x40 //联系管理员
	NOTFOUND_CONFILE = 0x41 //未找到配置文件
	INVALID_GWID     = 0x42 //无效的gwid
	GWID_NOTFOUND    = 0x43 //未找到gwid
	NO_GWID          = 0x44 // gwid为空
)

//command controller
type CommonController struct {
	i18n.Locale
	beego.Controller

	//user information
	login bool
	uname string
}

//global session
var globalSessions *session.Manager

//init session
func init() {

	cf := &session.ManagerConfig{CookieName: "gosessionid", Gclifetime: 3600}
	globalSessions, _ = session.NewManager("memory", cf)
	go globalSessions.GC()
}

//prepare
func (c *CommonController) Prepare() {
	/*
		ck, err := c.Ctx.Request.Cookie("username")
		if err != nil {
			c.login = false
			return
		}
		username := ck.Value

		ck, err = c.Ctx.Request.Cookie("password")
		if err != nil {
			c.login = false
			return
		}
		password := ck.Value
	*/
	sess, _ := globalSessions.SessionStart(c.Ctx.ResponseWriter, c.Ctx.Request)
	defer sess.SessionRelease(c.Ctx.ResponseWriter)
	sess_user := sess.Get("username")
	sess_pwd := sess.Get("password")
	if sess_user == nil || sess_pwd == nil {
		c.login = false
		return
	}

	username := sess_user.(string)
	password := sess_pwd.(string)
	fmt.Println(username, ":", password)
	value, ok := LoginInfo.LoginUserMap[username]
	if ok {
		if value.PassWord == password {
			c.login = true
			c.uname = username
			sess.Set("username", username)
			sess.Set("password", password)
			lang := c.GetString("lang")
			fmt.Println(lang)
			if lang == "zh-CN" {
				c.Lang = lang
			} else {
				c.Lang = "en-US"
			}
			c.Data["Lang"] = c.Lang
		}
	} else {
		c.login = false
	}
}

//GetTplName
func (c *CommonController) GetTplName(filename string) string {
	str := strings.Split(filename, ".")
	if str[1] == "html" {
		return "portus-mobile/html/p_" + filename
	} else if str[1] == "tpl" {
		return "portus-mobile/tpl/p_" + filename
	} else {
		return ""
	}
}

//report error
func (c *CommonController) ReportError(errno int) {
	errno2str := strconv.Itoa(errno)
	c.Data["json"] = &map[string]interface{}{"status": true, "errno": errno2str}
	c.ServeJSON()
}
