/**
*copyright 2016 Portus(Wuhan). All rights reserved.
*@Author: PengYun
*@Date:   2016/08/02
*@Update: 2016/08/17
*
*This package is the controller, is mainly used to
*responsible for scheduling and load the view,
*in which also need to introduce the API from the database to
*complete the application of the system as a whole
 */
package controllers

type RegController struct {
	CommonController
}

func (c *RegController) Get() {

	c.TplName = c.GetTemplatetype() + "/html/p_reg.html"

}

//Define a global variable

// @Title Post
// @Description User registered
// @Param   body      username: , pass: , confirm_pass: , onlie:
// @Success 200 301
// @Failure 403 body is empty
// @router /user/reg [post]
func (c *RegController) Post() {

	//Obtain information on the form
	name := c.Input().Get("username")
	pas := c.Input().Get("pass")
	pwd := c.Input().Get("confirm_pass")
	//online := c.Input().Get("online") == "on"

	if pas == pwd && len(name) > 5 && len(pas) > 5 && len(pwd) > 5 {

		c.Ctx.Output.Body([]byte("<script>alert('注册成功！点击跳转到登录界面');location.href='/login';</script>"))
	} else {
		c.Ctx.Output.Body([]byte("<script>alert('用户名已存在或输入错误！请重新输入');location.href='/reg';</script>"))
	}
}
