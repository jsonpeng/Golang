/**
*copyright 2016 Portus(Wuhan). All rights reserved.
*@Author: PengYun
*@Date:   2016/08/02
*@Update: 2016/08/09
*
*This package is the controller, is mainly used to
*responsible for scheduling and load the view,
*in which also need to introduce the API from the database to
*complete the application of the system as a whole
 */
package controllers

//The introduction of the outer package to function
import (
	"github.com/astaxie/beego"
	"github.com/astaxie/beego/context"
)

type LoginController struct {
	CommonController
}

// Body returns the raw request body data as bytes.

func (c *LoginController) Get() {

	isExit := c.Input().Get("exit") == "true"
	if isExit {
		c.Ctx.SetCookie("name", "", -1, "/")
		c.Ctx.SetCookie("pwd", "", -1, "/")
		c.Redirect("/login", 301)
		return
	}

	c.TplName = c.GetTemplatetype() + "/html/p_login.html"

}

//Define a global variable

// @Title Post
// @Description User Login
// @Param   body      UserName: , Password: , Ip: , Time:
// @Success 200 301
// @Failure 403 body is empty
// @router /user/login [post]
func (c *LoginController) Post() {

	name := c.Input().Get("username")
	pwd := c.Input().Get("password")
	autologin := c.Input().Get("autologin") == "on"

	if name == "pengyun" && pwd == "123456" && name != "admin" && len(name) > 5 && len(pwd) > 5 {

		maxAge := 0
		if autologin == true {
			maxAge = 1<<31 - 1

		}
		c.Ctx.SetCookie("name", name, maxAge, "/")
		c.Ctx.SetCookie("pwd", pwd, maxAge, "/")
		c.Redirect("/index?username="+name, 301)
		//c.Ctx.Output.Body([]byte("<script>alert('欢迎您**用户');location.href='/index?username=+name';</script>"))

		//		return
	} else if name == "admin" && len(pwd) > 5 {
		c.Redirect("/index?username="+name, 301)
		//		c.Ctx.Output.Body([]byte("<script>alert('欢迎您管理员,');location.href='/admin';</script>"))
		//		//c.Redirect("/admin/index", 301)
	} else {

		c.Ctx.Output.Body([]byte("<script>alert('账号或密码错误，点击重新输入');location.href='/login';</script>"))

	}

}

//slove cookie
func checkAccount(ctx *context.Context) bool {
	ck, err := ctx.Request.Cookie("name")
	if err != nil {
		return false
	}
	name := ck.Value
	ck, err = ctx.Request.Cookie("pwd")
	if err != nil {
		return false
	}
	pwd := ck.Value

	return beego.AppConfig.String("username") == name &&
		beego.AppConfig.String("password") == pwd && name != "admin" && len(name) > 5 && len(pwd) > 5

}
