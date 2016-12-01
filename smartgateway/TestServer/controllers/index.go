package controllers

import (
	"fmt"
	"strconv"
	"time"

	"github.com/portus/SmartGateway/CloudClient"
	"github.com/portus/SmartGateway/DateBase/LoginInfo"
	"github.com/portus/SmartGateway/DateBase/RegisterUserInfo"
	"github.com/portus/SmartGateway/DateBase/SystemCfg"
	"github.com/portus/SmartGateway/NodeClient"
	"github.com/portus/SmartGateway/NodePro/Simulate"
)

type IndexController struct {
	CommonController
}

var usernameReturn string

func (c *IndexController) Get() {
	//	info := c.GetSession("portus")
	//	if info == nil {
	//		c.Redirect("/login", 301)
	//	}
	fmt.Println(c.Input())
	uid := c.Input().Get("uid")
	myuid, _ := strconv.Atoi(uid)
	username := RegisterUserInfo.SeekUserById(int64(myuid))
	c.Data["username"] = username
	c.Data["IsHome"] = true
	cmd := nodeclient.RequestInfo{nodeclient.REQUEST, nodeclient.GWDISCOVERY, ""}
	gateway, _ := nodeclient.DealNodeClientMessage(cmd)
	c.Data["Gateway"] = gateway
	c.Data["IsAdmin"] = username == Administrator
	c.TplName = c.GetTplName("index.html")
	//c.TplName = "portus/html/p_index.html"
}
func (c *IndexController) Tree() {
	c.TplName = c.GetTplName("tree.tpl")
	//c.TplName = "portus/tpl/p_tree.tpl"
	cmd := nodeclient.RequestInfo{nodeclient.REQUEST, nodeclient.GWDISCOVERY, ""}
	gateway, err := nodeclient.DealNodeClientMessage(cmd)
	if err != nil {
		fmt.Println(err)
	}
	c.Data["IsAdd"] = true
	c.Data["Gateway"] = gateway
}
func (c *IndexController) Main() {
	c.TplName = c.GetTplName("main.tpl")
	//c.TplName = "portus/tpl/p_main.tpl"
}

func (c *IndexController) Gateway() {
	Gwid, _ := c.GetInt64("gwid")
	fmt.Println("Gwid :", Gwid)
	NodeInfo, _ := simulate.GetNodeList(Gwid)
	c.Data["Gwid"] = Gwid
	c.Data["Node"] = NodeInfo
	c.TplName = c.GetTplName("gateway.html")
	//c.TplName = "portus/html/p_gateway.html"

	//添加的测试代码
	category := c.Input().Get("category")
	switch category {
	case "node1":
		c.Data["IsCategory1"] = true
	case "node2":
		c.Data["IsCategory2"] = true
	case "node3":
		c.Data["IsCategory3"] = true
	case "node4":
		c.Data["IsCategory4"] = true
	case "test":
		c.Data["IsTest"] = true
	case "add":
		c.Data["IsAdd"] = true
	}

}
func (c *IndexController) GwEdit() {
	fmt.Println("GwEdit")
	Gwid, _ := c.GetInt64("Gwid")
	Nid, _ := c.GetInt64("Nid")
	fmt.Println("Gwid:", Gwid, "Nid:", Nid)
	NodeInfo, _ := simulate.GetNode(Gwid, Nid)
	fmt.Println("category:", NodeInfo.Category)

	switch NodeInfo.Category {
	case "test":
		c.Data["Address"] = NodeInfo.Address
		c.TplName = c.GetTplName("test.tpl")
		//c.TplName = "portus/tpl/p_test.tpl"
	case "switch":
		c.Data["Nid"] = Nid
		c.Data["Address"] = NodeInfo.Address
		c.Data["Category"] = NodeInfo.Category
		c.TplName = c.GetTplName("switch.tpl")
		//c.TplName = "portus/tpl/p_switch.tpl"
	default:
		c.TplName = c.GetTplName("gatewayswitch.tpl")
		//c.TplName = "portus/tpl/p_gatewayswitch.tpl"
	}

}

func (c *IndexController) GwManage() {
	if c.Ctx.Request.Method == "GET" {
		gwip, _ := systemcfg.GetRemoteServerIp()
		c.Data["gwip"] = gwip

		gwStatus := cloudclient.GetGatewayStatus() == "online"
		fmt.Println("GetGateStatus:", gwStatus)
		c.Data["Ischecked"] = gwStatus
		c.TplName = c.GetTplName("gateway_manage.tpl")
		//c.TplName = "portus/tpl/p_gateway_manage.tpl"
	} else {
		gwid := c.Input().Get("gwip")
		if c.Input().Get("gwonline") != "on" {
			_, err := cloudclient.DoGwidExit(gwid, 100)
			if err != nil {
				c.ReportError(SERVER_NORUNNING)
			}
			systemcfg.SetGatewayAllow("off")
			c.Redirect("/info", 301)

		} else {
			cloudclient.CloudClientInit()
			go cloudclient.CloudClientTask()
			systemcfg.SetGatewayAllow("on")
			c.Redirect("/info", 301)
		}
	}
}
func (c *IndexController) Backpack() {
	c.TplName = c.GetTplName("backpackgateway.html")
	//c.TplName = "portus/html/p_backpackgateway.html"
}

func (c *IndexController) Info() {

	if c.Ctx.Request.Method == "GET" {
		username := c.Input().Get("username")
		c.Data["username"] = username
		fmt.Println(c.Ctx.Request.Method)

		fmt.Println("INFO", c.Input())
		//get userinformation
		err, user := RegisterUserInfo.SeekUser(username)
		if err != nil {
			c.ReportError(NO_USERNAME)
		}
		c.Data["password"] = user.Password
		//加入判断管理员判断验证
		if username == "admin" {
			c.Data["IsAd"] = true
		} else {
			if user.Online == RegisterUserInfo.ONLINE {
				c.Data["Ischecked"] = true
			} else {
				c.Data["Ischecked"] = false
			}
		}
		c.TplName = c.GetTplName("information.tpl")
		return
		//c.TplName = "portus/tpl/p_information.tpl"
	} else {
		fmt.Println("INFO2", c.Input().Get("username"))
		username := c.Input().Get("username")
		c.Data["username"] = username
		c.Data["password"] = c.Input().Get("password")
		onlineStatus := c.Input().Get("online") == "on"
		fmt.Println("ONLINEStatus:", onlineStatus)
		onlineStatue := RegisterUserInfo.DefaultStatue
		if username == Administrator {
			c.Data["IsAd"] = true
			_, userMsg := RegisterUserInfo.SeekUser(username)
			userMsg.Password = c.Input().Get("password")
			RegisterUserInfo.UpdateUser(username, userMsg)
			c.TplName = c.GetTplName("information.tpl")
			//c.TplName = "portus/tpl/p_information.tpl"
			return
		}
		if onlineStatus {
			_, user := RegisterUserInfo.SeekUser(username)
			struid := strconv.Itoa(int(user.UId))
			data := struid + ":" + username + ":" + c.Input().Get("password")
			_, err := cloudclient.DoOnlineMessage(data, 1000)
			fmt.Println(err)
			if err == nil {
				onlineStatue = RegisterUserInfo.ONLINE
			} else {
				c.ReportError(SERVER_NORUNNING)
				return
			}
		} else {
			_, userMsg := RegisterUserInfo.SeekUser(username)
			struid := strconv.Itoa(int(userMsg.UId))
			if userMsg.Online == RegisterUserInfo.ONLINE {
				data := struid + ":" + username + ":" + c.Input().Get("password")
				_, err := cloudclient.DoOfflineMessage(data, 1000)
				if err == nil {
					onlineStatue = RegisterUserInfo.DefaultStatue
				} else {
					c.ReportError(SERVER_NORUNNING)
					return
				}
			} else {
				_, userMsg := RegisterUserInfo.SeekUser(username)
				userMsg.Password = c.Input().Get("password")
				userMsg.Online = RegisterUserInfo.DefaultStatue
				RegisterUserInfo.UpdateUser(username, userMsg)
				if userMsg.Online == RegisterUserInfo.ONLINE {
					c.Data["Ischecked"] = true
				} else {
					c.Data["Ischecked"] = false
				}
				c.Redirect("/info", 301)
				//c.TplName = "portus/tpl/p_information.tpl"
				return
			}
		}
		_, userMsg := RegisterUserInfo.SeekUser(username)
		userMsg.Password = c.Input().Get("password")
		userMsg.Online = onlineStatue
		RegisterUserInfo.UpdateUser(username, userMsg)
		if userMsg.Online == RegisterUserInfo.ONLINE {
			c.Data["Ischecked"] = true
		} else {
			c.Data["Ischecked"] = false
		}
		c.Redirect("/info", 301)
		//c.TplName = "portus/tpl/p_information.tpl"
		return
	}
}

func (c *IndexController) AdminAlluser() {
	c.TplName = c.GetTplName("admin_alluser.html")
	//c.TplName = "portus/html/p_admin_alluser.html"
	return
}

func (c *IndexController) AdminInfo() {
	deleteLable := c.Input().Get("delete")
	var adminUserMsg RegisterUserInfo.Registry
	type InfoMsg struct {
		User       RegisterUserInfo.Registry
		UserStatue string
	}

	var notAdminInfo []InfoMsg
	var allUserInfo []InfoMsg
	var adminInfo InfoMsg
	if deleteLable == "" {
		userInfo := RegisterUserInfo.AllUserInfo()
		for _, userMsg := range userInfo.User {
			if userMsg.Username == Administrator {
				onlineStatue := "online"
				adminUserMsg = RegisterUserInfo.Registry{userMsg.UId, userMsg.Username, userMsg.Password, userMsg.Online, userMsg.Language}
				adminInfo = InfoMsg{adminUserMsg, onlineStatue}
				continue
			}
			onlineStatue := "offline"
			userId := userMsg.UId
			userName := userMsg.Username
			_, ok := LoginInfo.LoginUserMap[userName]
			if ok {
				onlineStatue = "online"
			}
			passWord := userMsg.Password
			language := userMsg.Language
			onLine := userMsg.Online
			personMsg := RegisterUserInfo.Registry{userId, userName, passWord, onLine, language}
			newUserInfo := InfoMsg{personMsg, onlineStatue}
			notAdminInfo = append(notAdminInfo, newUserInfo)

		}
		allUserInfo = append(allUserInfo, adminInfo)
		for _, personInfo := range notAdminInfo {
			allUserInfo = append(allUserInfo, personInfo)
		}
		fmt.Println(allUserInfo)
		c.Data["InfoMsg"] = allUserInfo
		c.TplName = c.GetTplName("admin_userinfo.tpl")
		return
		//c.TplName = "portus/tpl/p_admin_userinfo.tpl"
	} else {
		if deleteLable == Administrator {
			userInfo := RegisterUserInfo.AllUserInfo()
			for _, userMsg := range userInfo.User {
				if userMsg.Username == Administrator {
					onlineStatue := "online"
					adminUserMsg = RegisterUserInfo.Registry{userMsg.UId, userMsg.Username, userMsg.Password, userMsg.Online, userMsg.Language}
					adminInfo = InfoMsg{adminUserMsg, onlineStatue}
					continue
				}
				userId := userMsg.UId
				onlineStatue := "offline"
				userName := userMsg.Username
				_, ok := LoginInfo.LoginUserMap[userName]
				if ok {
					onlineStatue = "online"

				}
				passWord := userMsg.Password
				language := userMsg.Language
				onLine := userMsg.Online
				personMsg := RegisterUserInfo.Registry{userId, userName, passWord, onLine, language}
				newUserInfo := InfoMsg{personMsg, onlineStatue}
				notAdminInfo = append(notAdminInfo, newUserInfo)
			}
			allUserInfo = append(allUserInfo, adminInfo)
			for _, personInfo := range notAdminInfo {
				allUserInfo = append(allUserInfo, personInfo)
			}
			fmt.Println(allUserInfo)
			c.Data["InfoMsg"] = allUserInfo
			c.TplName = c.GetTplName("admin_userinfo.tpl")
			return
			//c.TplName = "portus/tpl/p_admin_userinfo.tpl"

		} else {
			onlineState := RegisterUserInfo.OnlineUser(deleteLable)
			//fmt.Println("用户当前的状态:", onlineState, onlineState == RegisterUserInfo.ONLINE)
			if onlineState == RegisterUserInfo.ONLINE {
				_, deleteUser := RegisterUserInfo.SeekUser(deleteLable)
				deletePass := deleteUser.Password
				data := string(deleteUser.UId) + ":" + deleteLable + ":" + deletePass
				_, err := cloudclient.DoOfflineMessage(data, 1000)
				if err != nil {
					RegisterUserInfo.DeleteUser(deleteLable)
					c.ReportError(SERVER_NORUNNING)
					return
				}
			}
			RegisterUserInfo.DeleteUser(deleteLable)
			userInfo := RegisterUserInfo.AllUserInfo()
			for _, userMsg := range userInfo.User {
				if userMsg.Username == Administrator {
					onlineStatue := "online"
					adminUserMsg = RegisterUserInfo.Registry{userMsg.UId, userMsg.Username, userMsg.Password, userMsg.Online, userMsg.Language}
					adminInfo = InfoMsg{adminUserMsg, onlineStatue}
					continue
				}
				userId := userMsg.UId
				onlineStatue := "offline"
				userName := userMsg.Username
				_, ok := LoginInfo.LoginUserMap[userName]
				if ok {
					onlineStatue = "online"
				}
				passWord := userMsg.Password
				language := userMsg.Language
				onLine := userMsg.Online
				personMsg := RegisterUserInfo.Registry{userId, userName, passWord, onLine, language}
				newUserInfo := InfoMsg{personMsg, onlineStatue}
				notAdminInfo = append(notAdminInfo, newUserInfo)
			}
			allUserInfo = append(allUserInfo, adminInfo)
			for _, personInfo := range notAdminInfo {
				allUserInfo = append(allUserInfo, personInfo)
			}
			fmt.Println(allUserInfo)
			c.Data["InfoMsg"] = allUserInfo
			c.TplName = c.GetTplName("admin_userinfo.tpl")
			return
			//c.TplName = "portus/tpl/p_admin_userinfo.tpl"
		}
	}
}
func (c *IndexController) AdminAdd() {
	if c.Ctx.Request.Method == "GET" {
		c.TplName = c.GetTplName("add.tpl")
		return
		//c.TplName = "portus/tpl/p_add.tpl"
	} else {
		fmt.Println(c.Input())
		newUserName := c.Input().Get("username")
		if newUserName == "" {
			c.ReportError(USERNAME_NIL)
			return
		}
		if err, _ := RegisterUserInfo.SeekUser(newUserName); err == nil {
			c.ReportError(REGISTER_USER)
			return
		} else {
			newPassWord := c.Input().Get("password")
			comfimPassWord := c.Input().Get("pas")
			if newPassWord == "" || comfimPassWord == "" {
				c.ReportError(PASSWORD_NIL)
				return
			}
			if newPassWord != comfimPassWord {
				c.ReportError(NOSAME_PASSWORD)
				return
			} else {
				if c.Input().Get("online") == "on" {
					onlineState := RegisterUserInfo.ONLINE
					uid := time.Now().Unix()
					struid := strconv.Itoa(int(uid))
					data := struid + ":" + newUserName + ":" + newPassWord
					Msg, err := cloudclient.DoOnlineMessage(data, 1000)
					fmt.Println("AddOnline:", Msg, err)
					if err != nil {
						c.ReportError(SERVER_NORUNNING)
						return
					} else {
						newUser := RegisterUserInfo.Registry{uid, newUserName, newPassWord, onlineState, DefaultLang}
						RegisterUserInfo.RegisterUser(newUser)
						c.ReportError(SUCCESS)
						return
					}
				} else {
					newUser := RegisterUserInfo.Registry{time.Now().Unix(), newUserName, newPassWord, RegisterUserInfo.DefaultStatue, DefaultLang}
					RegisterUserInfo.RegisterUser(newUser)
					c.ReportError(SUCCESS)
					return
				}
			}
		}
	}
}
func (c *IndexController) GateWayList() {
	data, _ := simulate.Jsonlist()
	fmt.Println(string(data))
	c.Ctx.WriteString(string(data))
}
func (c *IndexController) Logout() {
	c.DelSession("portus")
	c.Redirect("/", 302)
}
