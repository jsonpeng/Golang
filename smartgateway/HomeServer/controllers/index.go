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

import (
	"github.com/astaxie/beego/context"
	"github.com/portus/SmartGateWay/nodeclient"
)

//The introduction of the outer package to function

type IndexController struct {
	CommonController
}

// @Title  Get0.
// @Description User's Index
// @Param   body
// @Success 200
// @Failure 403 body is empty
// @router /index [Get]
func (c *IndexController) Get() {
	IsAdd := c.Input().Get("add") == "true"
	if IsAdd {
		c.Data["Add"] = true
	} else {
		c.Data["Add"] = false
	}

	c.Data["user"] = c.Input().Get("username")
	c.Data["IsLogin"] = checkAccount(c.Ctx)

	c.Data["IsAdded"] = checkAdd(c.Ctx)
	c.Data["IsHome"] = true
	c.Data["users"] = "pengyun"

	username := c.Input().Get("username")
	c.Data["IsAdmin"] = username == "admin"

	c.TplName = c.GetTemplatetype() + "/html/p_index.html"

}

//func (c *IndexController) Post() {
//	velifyname := c.Input().Get("velify")
//}

// @Title  Tree
// @Description User's tree
// @Param   body
// @Success 200
// @Failure 403 body is empty
// @router /usertree [Get]
func (c *IndexController) Tree() {
	c.Data["mygateway"] = "127.0.0.1"
	c.Data["IsAdded"] = checkAdd(c.Ctx)
	c.TplName = c.GetTemplatetype() + "/tpl/p_tree.tpl"
}

// @Title  mainpage
// @Description User's main page
// @Param   body
// @Success 200
// @Failure 403 body is empty
// @router /main [Get]
func (c *IndexController) Main() {
	gateway, _ := nodeclient.GetGatewayDiscovery()
	c.Data["gateway"] = gateway
	//fmt.Println(gateway)
	c.TplName = c.GetTemplatetype() + "/tpl/p_main.tpl"
}

// @Title  mainpage
// @Description User's main page
// @Param   body
// @Success 200
// @Failure 403 body is empty
// @router /gateway
func (c *IndexController) Gateway() {
	c.TplName = c.GetTemplatetype() + "/html/p_gateway.html"
	node := c.Input().Get("node")
	switch node {
	case "node1":
		c.Data["IsNode1"] = true
	case "node2":
		c.Data["IsNode2"] = true
	case "test":
		c.Data["IsTest"] = true
	case "add":
		c.Data["IsTest"] = true
		c.Data["GatewayAdd"] = true
	}

}

// @Title  mainpage
// @Description User's main page
// @Param   body
// @Success 200
// @Failure 403 body is empty
// @router /backpack
func (c *IndexController) Backpack() {

	c.TplName = c.GetTemplatetype() + "/html/p_backpack.html"

}

//slove cookie
func checkAdd(ctx *context.Context) bool {
	ck, err := ctx.Request.Cookie("velify")
	if err != nil {
		return false
	}
	velifyname := ck.Value

	return velifyname != "admin"

}

// @Title  mainpage
// @Description User's main page
// @Param   body
// @Success 200
// @Failure 403 body is empty
// @router /userinfo [Get]
func (c *IndexController) Info() {
	c.Data["IsLogin"] = checkAccount(c.Ctx)
	c.Data["IsHome"] = true

	c.TplName = c.GetTemplatetype() + "/tpl/p_information.tpl"
}
