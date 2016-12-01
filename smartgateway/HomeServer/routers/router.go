/**
*copyright 2016 Portus(Wuhan). All rights reserved.
*@Author: PengYun
*@Date:   2016/07/28
*@Version:
*Here is the application of all routing rules,
*regulations system access route of the page
 */
package routers

//The introduction of the outer package to function
import (
	"github.com/astaxie/beego"
	"github.com/portus/SmartGateway/HomeServer/controllers"
)

func init() {
	//Load the routing
	beego.Router("/", &controllers.AdminRegController{})
	beego.Router("/login", &controllers.LoginController{})

	beego.Router("/usertree", &controllers.IndexController{}, "*:Tree")
	beego.Router("/index", &controllers.IndexController{})
	beego.Router("/main", &controllers.IndexController{}, "*:Main")
	beego.Router("/gateway", &controllers.IndexController{}, "*:Gateway")
	beego.Router("/backpack", &controllers.IndexController{}, "*:Backpack")
	beego.Router("/userinfo", &controllers.IndexController{}, "*:Info")
	beego.Router("/reg", &controllers.RegController{})

	//	beego.Router("/admin", &controllers.AdminController{})
	beego.Router("/list", &controllers.AdminController{}, "*:AdminList")

	beego.Router("/info", &controllers.AdminController{}, "*:AdminInfo")
	beego.Router("/add", &controllers.AdminController{}, "*:AdminAdd")

}
