/**
*copyright 2016 Portus(Wuhan). All rights reserved.
*@Author: PengYun
*@Date:   2016/08/02
*@Update: 2016/08/17
*This package is the main controller, is mainly used to
*responsible for scheduling and load the view,
*in which also need to introduce the API from the database to
*complete the application of the system as a whole
 */
package controllers

import (
	"github.com/astaxie/beego"
)

type CommonController struct {
	beego.Controller
	Templatetype string //ui template type
}

func (c *CommonController) Rsjson(status bool, str string) {
	c.Data["json"] = &map[string]interface{}{"status": status, "info": str}
	c.ServeJSON()

}

func (c *CommonController) GetTemplatetype() string {
	templatetype := beego.AppConfig.String("template_type")
	if templatetype == "" {
		templatetype = "portus"
	}
	return templatetype
}
