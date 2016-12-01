/**
*copyright 2016 Portus(Wuhan). All rights reserved.
*@Author: PengYun
*@Date:   2016/07/28
*@Update: 2016/08/09
*Single entry, here is the file homeserver under
*all the procedure and function method to open it
 */
package homeserver

//The introduction of the outer package to function
import (
	"fmt"

	"github.com/astaxie/beego"

	_ "github.com/portus/smartGateway/HomeServer/routers"
	//"github.com/portus/smartgateway/HomeServer/i18n"
)

func Init() {
	//i18n.SetMessage("en-US", "conf/locale_en-US.ini")
	//i18n.SetMessage("zh-CN", "conf/locale_zh-CN.ini")
	//	beego.SetStaticPath("HomeServer/static", "static")
	beego.SetViewsPath("Homeserver/views")
	beego.SetStaticPath("/static", "Homeserver/static")

	beego.BConfig.WebConfig.Session.SessionAutoSetCookie = false // true don't start
	beego.BConfig.Listen.EnableHTTPS = true                      //start https
	beego.BConfig.Listen.HTTPSPort = 10443
	beego.BConfig.WebConfig.Session.SessionGCMaxLifetime = 3600 //3600s
	beego.BConfig.Listen.HTTPSKeyFile = "conf/ssl.key"
	//beego.AddFuncMap("i18n", i18n.Tr)
	beego.BConfig.Log.AccessLogs = true
	beego.BConfig.Log.FileLineNum = true
	beego.BConfig.Log.Outputs = map[string]string{"console": ""}

}
func HomeServerTask() {
	Init()
	fmt.Println("HomeServer Task Start...")
	beego.Run(":8350")

}
