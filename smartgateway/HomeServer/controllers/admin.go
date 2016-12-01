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

import (
	"github.com/astaxie/beego/context"
)

type AdminController struct {
	CommonController
}

//// @Title  Get
//// @Description Admin's Index
//// @Param   body
//// @Success 200
//// @Failure 403 body is empty
//// @router /admin [Get]
//func (c *AdminController) Get() {
//	//Through the  template engine load the view

//	c.Data["IsLogin"] = checkAccount(c.Ctx)
//	c.TplName = c.GetTemplatetype() + "/html/p_admin_index.html"

//}

// @Title
// @Description Admin's Index
// @Param   body
// @Success 200
// @Failure 403 body is empty
// @router /list
func (c *AdminController) AdminList() {
	c.Data["IsLogin"] = checkAccount(c.Ctx)
	c.Data["IsAdded"] = checkAdd(c.Ctx)
	c.Data["IsHome"] = true
	c.Data["users"] = "admin"

	c.TplName = c.GetTemplatetype() + "/tpl/p_admin_list.tpl"
}

//slove cookie
func checkAdmin(ctx *context.Context) bool {
	ck, err := ctx.Request.Cookie("velifyAdmin")
	if err != nil {
		return false
	}
	velifyadmin := ck.Value

	return len(velifyadmin) > 5 && velifyadmin != "admin"

}

// @Title
// @Description Admin's Index
// @Param   body
// @Success 200
// @Failure 403 body is empty
// @router /info
func (c *AdminController) AdminInfo() {
	c.Data["IsLogin"] = checkAccount(c.Ctx)
	c.Data["IsAdded"] = checkAdd(c.Ctx)
	c.Data["IsHome"] = true
	c.Data["users"] = "admin"

	c.TplName = c.GetTemplatetype() + "/tpl/p_admin_userinfo.tpl"
}

// @Title
// @Description Admin's Index
// @Param   body
// @Success 200
// @Failure 403 body is empty
// @router /add
func (c *AdminController) AdminAdd() {
	c.Data["IsLogin"] = checkAccount(c.Ctx)
	c.Data["IsAdded"] = checkAdd(c.Ctx)
	c.Data["IsHome"] = true
	c.Data["users"] = "admin"

	c.TplName = c.GetTemplatetype() + "/tpl/p_add.tpl"
}
