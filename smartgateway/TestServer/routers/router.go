package routers

import (
	"github.com/astaxie/beego"
	"github.com/portus/SmartGateway/TestServer/controllers"
)

func init() {

	beego.Router("/", &controllers.HomeController{})

	beego.Router("/gwmanage", &controllers.IndexController{}, "*:GwManage")

	beego.Router("/index", &controllers.IndexController{})
	beego.Router("/usertree", &controllers.IndexController{}, "*:Tree")
	beego.Router("/main", &controllers.IndexController{}, "*:Main")
	beego.Router("/gateway", &controllers.IndexController{}, "*:Gateway")
	beego.Router("/backpackgateway", &controllers.IndexController{}, "*:Backpack")
	beego.Router("/userinfo", &controllers.IndexController{}, "*:Info")
	beego.Router("/info", &controllers.IndexController{}, "*:AdminInfo")
	beego.Router("/add", &controllers.IndexController{}, "*:AdminAdd")
	beego.Router("/gwedit", &controllers.IndexController{}, "*:GwEdit")
	beego.Router("/logout", &controllers.IndexController{}, "*:Logout")
	beego.Router("/ws", &controllers.WebSocketController{})
}
