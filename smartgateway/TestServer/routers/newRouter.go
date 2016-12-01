package routers

import (
	"github.com/astaxie/beego"
	"github.com/portus/SmartGateway/TestServer/controllers"
)

func init() {
	beego.Router("/data/login", &controllers.DataController{}, "*:Login")
	beego.Router("/data/gatewaylist", &controllers.DataController{}, "*:Gatewaylist")
	beego.Router("/data/userinfo", &controllers.DataController{}, "*:UserInfo")
	beego.Router("/data/deluserinfo", &controllers.DataController{}, "*:Deleteinfo")
	beego.Router("/data/adduserinfo", &controllers.DataController{}, "*:Adduserinfo")
	beego.Router("/data/userlist", &controllers.DataController{}, "*:UserList")
	beego.Router("/data/onlineuserinfo", &controllers.DataController{}, "*:OnlineUser")
	beego.Router("/data/exit", &controllers.DataController{}, "*:Exit")
	beego.Router("/data/findpwd", &controllers.DataController{}, "*:FindPwd")
}
