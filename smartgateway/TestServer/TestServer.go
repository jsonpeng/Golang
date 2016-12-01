package testserver

import (
	"fmt"

	"github.com/astaxie/beego"
	"github.com/astaxie/beego/config"

	_ "github.com/portus/SmartGateway/TestServer/routers"
)

func TestServerTask() {

	//load configutrue file
	cnf, err := config.NewConfig("ini", "./TestServer/conf/app.conf")
	if err != nil {
		fmt.Println(err)
	}

	//print config information
	fmt.Println("appname:" + cnf.String("appname") +
		"\nhttpport:" + cnf.String("httpport") +
		"\nrunmode:" + cnf.String("runmode"))

	//set views path and static path
	beego.BConfig.WebConfig.Session.SessionOn = true
	beego.SetViewsPath("HomeServer/views")
	beego.SetStaticPath("/static", "HomeServer/static")
	httpport := cnf.String("httpport")
	beego.Run(":" + httpport)

}
