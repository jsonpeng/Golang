/**
*copyright 2016 Portus(Wuhan). All rights reserved.
*@Author: PengYun
*@Date:   2016/08/02
*@Update: 2016/08/17
*This package is the controller, is mainly used to
*responsible for scheduling and load the view,
*in which also need to introduce the API from the database to
*complete the application of the system as a whole
 */

package controllers

type AdminRegController struct {
	CommonController
}

func (c *AdminRegController) Get() {

	c.TplName = c.GetTemplatetype() + "/html/p_admin_reg.html"

}

// @Title Post
// @Description Admin registered
// @Param   body      pass: , confirm_pass:
// @Success 200
// @Failure 403 body is empty
// @router /admin/reg [post]
func (c *AdminRegController) Post() {

	//Obtain information on the form
	pas := c.Input().Get("pass")
	pwd := c.Input().Get("confirm_pass")

	if pas == pwd && len(pas) > 5 && len(pwd) > 5 {

		c.Ctx.Output.Body([]byte("<script>alert('注册成功！点击跳转到登录界面');location.href='/login';</script>"))

	} else {
		c.Ctx.Output.Body([]byte("<script>alert('注册失败！点击请重新输入');location.href='/';</script>"))

	}

}
