package controllers

import (
	"encoding/json"

	"strconv"
	"time"

	"github.com/portus/SmartGateway/CloudClient"
	"github.com/portus/SmartGateway/DateBase/LoginInfo"
	"github.com/portus/SmartGateway/DateBase/RegisterUserInfo"
	"github.com/portus/SmartGateway/DateBase/SystemCfg"
	"github.com/portus/SmartGateway/NodePro/Simulate"
)

type DataController struct {
	CommonController
}

const (
	REQUEST = "request"
	REPLY   = "reply"
	NOTIFY  = "notify"
)

type MessageInfo struct {
	ErrorNumber int         `json:"error"`
	Action      string      `json:"action"`
	Data        interface{} `json:"data"`
}

/*
func validate(err error) (MessageInfo, error) {
	var info MessageInfo
	if err != nil {
		info.Action = REPLY
		return info, err
	}
	info.Action = REPLY
	return info, nil
}
*/
func initialize() MessageInfo {
	var messageInfo MessageInfo
	messageInfo.Action = REPLY
	return messageInfo
}
func infoJson(messageInfo MessageInfo) []byte {
	data, _ := json.MarshalIndent(messageInfo, "", "\t\t")
	return data
}

/*
func nameNUll(c *DataController, messageInfo MessageInfo) {
	messageInfo.ErrorNumber = USERNAME_NIL
	c.Ctx.WriteString(string(infoJson(messageInfo)))
	return
}
*/
func messageNull(c *DataController, messageInfo MessageInfo, errorNumber int) {
	messageInfo.ErrorNumber = errorNumber
	c.Ctx.WriteString(string(infoJson(messageInfo)))
}

/*
func passWordNUll(c *DataController, messageInfo MessageInfo) {
	messageInfo.ErrorNumber = PASSWORD_NIL
	c.Ctx.WriteString(string(infoJson(messageInfo)))
	return
}
func uidNull(c *DataController, messageInfo MessageInfo) {
	messageInfo.ErrorNumber = NO_UID
	c.Ctx.WriteString(string(infoJson(messageInfo)))
	return
}
func passWordInvalid(c *DataController, messageInfo MessageInfo) {
	messageInfo.ErrorNumber = INVALID_PASSWORD
	c.Ctx.WriteString(string(infoJson(messageInfo)))
	return
}
func userNull(c *DataController, messageInfo MessageInfo) {
	messageInfo.ErrorNumber = NO_USERNAME
	c.Ctx.WriteString(string(infoJson(messageInfo)))
	return
}
func uidInvalid(c *DataController, messageInfo MessageInfo) {
	messageInfo.ErrorNumber = INVALID_UID
	c.Ctx.WriteString(string(infoJson(messageInfo)))
	return
}
func repeatLogin(c *DataController, messageInfo MessageInfo) {
	messageInfo.ErrorNumber = LONGINNED_USER
	c.Ctx.WriteString(string(infoJson(messageInfo)))
	return
}
*/
func login(c *DataController, user RegisterUserInfo.Registry, messageInfo MessageInfo) {
	LoginInfo.Login(LoginInfo.LoginUser{user.Username, user.Password, nil, "192.168.32.1", "2016-8-26"})
	messageInfo.ErrorNumber = SUCCESS
	struid := strconv.Itoa(int(user.UId))
	userUid := make(map[string]string)
	userUid["uid"] = struid
	messageInfo.Data = userUid
	c.Ctx.WriteString(string(infoJson(messageInfo)))
	return
}
func getUserInfo(c *DataController, user RegisterUserInfo.Registry, messageInfo MessageInfo) {
	messageInfo.ErrorNumber = SUCCESS
	type UserData struct {
		Username string `json:"username"`
		Password string `json:"password"`
	}
	userinfo := UserData{
		Username: user.Username,
		Password: user.Password,
	}

	messageInfo.Data = userinfo
	c.Ctx.WriteString(string(infoJson(messageInfo)))
	return
}

func onlineHandle(c *DataController, user RegisterUserInfo.Registry, messageInfo MessageInfo) {
	struid := strconv.Itoa(int(user.UId))
	data := struid + ":" + user.Username + ":" + user.Password
	_, err := cloudclient.DoOnlineMessage(data, 1000)
	if err != nil {
		messageInfo.ErrorNumber = SERVER_NORUNNING
		c.Ctx.WriteString(string(infoJson(messageInfo)))
		return
	}
	user.Online = "online"
	RegisterUserInfo.UpdateUser(user.Username, user)
	messageInfo.ErrorNumber = SUCCESS
	c.Ctx.WriteString(string(infoJson(messageInfo)))
	return

}
func offonlineHandle(c *DataController, user RegisterUserInfo.Registry, messageInfo MessageInfo) {
	struid := strconv.Itoa(int(user.UId))
	data := struid + ":" + user.Username + ":" + user.Password
	_, err := cloudclient.DoOfflineMessage(data, 1000)
	if err != nil {
		messageInfo.ErrorNumber = SERVER_NORUNNING

		c.Ctx.WriteString(string(infoJson(messageInfo)))
		return
	}
	user.Online = "offonline"
	RegisterUserInfo.UpdateUser(user.Username, user)

	messageInfo.ErrorNumber = SUCCESS

	c.Ctx.WriteString(string(infoJson(messageInfo)))
	return
}

/*
func twicePwdNUll(c *DataController, messageInfo MessageInfo) {
	messageInfo.ErrorNumber = SUCCESS
	c.Ctx.WriteString(string(infoJson(messageInfo)))
	return
}
func pwdNoEqeual(c *DataController, messageInfo MessageInfo) {
	messageInfo.ErrorNumber = NOSAME_PASSWORD

	c.Ctx.WriteString(string(infoJson(messageInfo)))
	return
}
func compasswordNULL(c *DataController, messageInfo MessageInfo) {

	messageInfo.ErrorNumber = COMPASSWORD_NULL

	c.Ctx.WriteString(string(infoJson(messageInfo)))
	return
}
*/
func (c *DataController) Login() {
	if c.Ctx.Request.Method == "POST" {
		userName := c.Input().Get("username")
		passWord := c.Input().Get("password")
		messageInfo := initialize()
		if userName == "" {
			messageNull(c, messageInfo, USERNAME_NIL)
			//nameNUll(c, messageInfo)
			return
		}
		if passWord == "" {
			messageNull(c, messageInfo, PASSWORD_NIL)
			//passWordNUll(c, messageInfo)
			return
		}
		err, user := RegisterUserInfo.SeekUser(userName)
		if err != nil {
			messageNull(c, messageInfo, NO_USERNAME)
			//userNull(c, messageInfo)
			return
		} else {
			if user.Password == passWord {
				_, ok := LoginInfo.LoginUserMap[user.Username]
				if ok {
					messageNull(c, messageInfo, LONGINNED_USER)
					//repeatLogin(c, messageInfo)
					return
				} else {
					login(c, user, messageInfo)
					return
				}
			} else {
				messageNull(c, messageInfo, INVALID_PASSWORD)
				//passWordInvalid(c, messageInfo)
				return
			}
		}
	}
}

func (c *DataController) UserInfo() {
	uid := c.Input().Get("uid")
	messageInfo := initialize()
	if uid == "" {
		messageNull(c, messageInfo, NO_UID)
		//uidNull(c, messageInfo)
		return
	}
	struid, _ := strconv.Atoi(uid)
	username := RegisterUserInfo.SeekUserById(int64(struid))
	err, user := RegisterUserInfo.SeekUser(username)
	if err != nil {
		messageNull(c, messageInfo, INVALID_UID)
		//uidInvalid(c, messageInfo)
		return
	}
	if c.Ctx.Request.Method == "GET" {
		getUserInfo(c, user, messageInfo)
		return
	}
	if c.Ctx.Request.Method == "POST" {
		onlineStatus := c.Input().Get("online")
		password := c.Input().Get("password")
		compassword := c.Input().Get("compassword")
		if onlineStatus == "" && password == "" && compassword == "" {
			messageInfo.ErrorNumber = SUCCESS
			c.Ctx.WriteString(string(infoJson(messageInfo)))
			return
		}
		if onlineStatus != "" {
			if password == "" && compassword == "" {
				if onlineStatus != "online" && onlineStatus != "offonline" {
					messageNull(c, messageInfo, INVALID_PARAMETER)
				}
				if onlineStatus == user.Online {
					messageNull(c, messageInfo, SUCCESS)
					//twicePwdNUll(c, messageInfo)
					return
				}
				if onlineStatus == "online" && user.Online != onlineStatus {
					onlineHandle(c, user, messageInfo)
					return
				}
				if onlineStatus == "offonline" && user.Online != onlineStatus {

					offonlineHandle(c, user, messageInfo)
					return
				}
			} else {

				if password == "" && compassword != "" {
					// messageInfo(c,messageInfo,)
					messageNull(c, messageInfo, PASSWORD_NIL)
					return
				}
				if password != "" && compassword == "" {
					messageNull(c, messageInfo, COMPASSWORD_NULL)
					//compasswordNULL(c, messageInfo)
					return
				}
				if password != compassword {
					messageNull(c, messageInfo, NOSAME_PASSWORD)
					//pwdNoEqeual(c, messageInfo)
					return
				}
				if password == compassword {
					if onlineStatus == user.Online {
						if user.Online == "online" {
							user.Password = password
							onlineHandle(c, user, messageInfo)
							return
						}
						RegisterUserInfo.UpdateUser(username, user)

						messageNull(c, messageInfo, SUCCESS)
						return
					}
					if onlineStatus == "online" && user.Online != onlineStatus {
						user.Password = password
						onlineHandle(c, user, messageInfo)
						return
					}
					if onlineStatus == "offonline" && user.Online != onlineStatus {
						user.Password = password
						offonlineHandle(c, user, messageInfo)
						return
					}
				}
			}

		} else {
			if password == "" && compassword != "" {
				messageNull(c, messageInfo, PASSWORD_NIL)
				//passWordNUll(c, messageInfo)
				return

			}
			if password != "" && compassword == "" {
				messageNull(c, messageInfo, COMPASSWORD_NULL)
				//compasswordNULL(c, messageInfo)
				return
			}
			if password != compassword {
				messageNull(c, messageInfo, NOSAME_PASSWORD)
				//pwdNoEqeual(c, messageInfo)
				return
			}
			if password == compassword {

				if user.Online == "online" {
					user.Password = password
					onlineHandle(c, user, messageInfo)
					return
				} else {
					user.Password = password
					RegisterUserInfo.UpdateUser(username, user)

					messageNull(c, messageInfo, SUCCESS)
					return
				}

			}
		}
	}
}

func (c *DataController) Gatewaylist() {
	messageInfo := initialize()
	if c.Ctx.Request.Method == "GET" {
		infoData, err := simulate.GetGatewayList()
		if err != nil {
			messageInfo.ErrorNumber = FAILED

			c.Ctx.WriteString(string(infoJson(messageInfo)))
			return
		}
		messageInfo.ErrorNumber = SUCCESS
		type GateWay struct {
			Id          int64
			Category    string
			GatewayName string
			Address     string
			Protocol    string
		}
		var gatewaylist []GateWay

		for _, info := range infoData {
			var gateway GateWay
			gateway.Address = info.Address
			gateway.Category = info.Category
			gateway.GatewayName = info.GatewayName
			gateway.Id = info.Id
			gateway.Protocol = info.Protocol
			gatewaylist = append(gatewaylist, gateway)
		}

		messageInfo.Data = gatewaylist

		c.Ctx.WriteString(string(infoJson(messageInfo)))
		return
	} else {
		gid := c.Input().Get("gid")
		strgid, _ := strconv.Atoi(gid)
		nodeList, err := simulate.GetNodeList(int64(strgid))

		if err != nil {
			messageInfo.ErrorNumber = FAILED

			c.Ctx.WriteString(string(infoJson(messageInfo)))
			return
		}
		messageInfo.Data = nodeList

		c.Ctx.WriteString(string(infoJson(messageInfo)))
		return

	}

}
func (c *DataController) Deleteinfo() {
	messageInfo := initialize()
	if c.Ctx.Request.Method == "GET" {
		uid := c.Input().Get("uid")
		struid, _ := strconv.Atoi(uid)
		username := RegisterUserInfo.SeekUserById(int64(struid))
		if uid == "" {
			messageNull(c, messageInfo, NO_UID)
			//uidNull(c, messageInfo)
			return
		}
		if username == "" {
			messageNull(c, messageInfo, INVALID_UID)
			return
			//nameNUll(c, messageInfo)
		}
		_, user := RegisterUserInfo.SeekUser(username)
		_, ok := LoginInfo.LoginUserMap[username]
		if ok {
			LoginInfo.Exit(username)
		}
		if user.Online == "online" {
			struid := strconv.Itoa(int(user.UId))
			data := struid + ":" + username + ":" + user.Password
			_, err := cloudclient.DoOfflineMessage(data, 1000)
			if err != nil {
				messageInfo.ErrorNumber = SERVER_NORUNNING
				c.Ctx.WriteString(string(infoJson(messageInfo)))
				return
			}
		}
		RegisterUserInfo.DeleteUser(username)

		messageNull(c, messageInfo, SUCCESS)
		return
	}
}
func (c *DataController) Adduserinfo() {
	messageInfo := initialize()
	if c.Ctx.Request.Method == "POST" {
		username := c.Input().Get("username")
		password := c.Input().Get("password")
		cmpassword := c.Input().Get("compassword")
		onlineStatus := c.Input().Get("online")
		if username == "" {
			messageNull(c, messageInfo, USERNAME_NIL)
			//userNull(c, messageInfo)
			return
		}
		if password == "" && cmpassword != "" {
			messageNull(c, messageInfo, PASSWORD_NIL)
			//passWordNUll(c, messageInfo)
			return

		}
		if password != "" && cmpassword == "" {
			messageNull(c, messageInfo, COMPASSWORD_NULL)
			//compasswordNULL(c, messageInfo)
			return
		}
		if password == "" && cmpassword == "" {
			messageNull(c, messageInfo, PASSWORD_NIL)
			//passWordNUll(c, messageInfo)
			return
		}
		if password != cmpassword {
			messageNull(c, messageInfo, NOSAME_PASSWORD)
			//pwdNoEqeual(c, messageInfo)
			return
		}
		err, _ := RegisterUserInfo.SeekUser(username)
		if err == nil {
			messageNull(c, messageInfo, REGISTER_USER)

			return
		}
		if onlineStatus == "online" {
			newUser := RegisterUserInfo.Registry{time.Now().Unix(), username, password, RegisterUserInfo.DefaultStatue, DefaultLang}
			RegisterUserInfo.RegisterUser(newUser)
			onlineHandle(c, newUser, messageInfo)
			return
		}
		if onlineStatus != "online" && onlineStatus != "" && onlineStatus != "offonline" {
			messageNull(c, messageInfo, INVALID_PARAMETER)
			return
		}
		if onlineStatus == "offonline" || onlineStatus == "" {
			newUser := RegisterUserInfo.Registry{time.Now().Unix(), username, password, RegisterUserInfo.DefaultStatue, DefaultLang}
			RegisterUserInfo.RegisterUser(newUser)
			messageNull(c, messageInfo, SUCCESS)
		}
		return

	}
}

func (c *DataController) UserList() {
	messageInfo := initialize()
	if c.Ctx.Request.Method == "GET" {
		messageInfo.Data = RegisterUserInfo.AllUserInfo()
		messageNull(c, messageInfo, SUCCESS)
		return
	}
}
func (c *DataController) OnlineUser() {
	messageInfo := initialize()
	if c.Ctx.Request.Method == "POST" {

		userGroup := make(map[string]string)
		for _, userinfo := range LoginInfo.LoginUserMap {
			userGroup[userinfo.UserName] = userinfo.PassWord
		}
		messageInfo.Data = userGroup
		messageNull(c, messageInfo, SUCCESS)

		return
	}
}
func (c *DataController) Exit() {
	messageInfo := initialize()
	if c.Ctx.Request.Method == "POST" {
		uid := c.Input().Get("uid")
		userName := c.Input().Get("username")
		struid, _ := strconv.Atoi(uid)
		username := RegisterUserInfo.SeekUserById(int64(struid))
		if uid == "" && userName == "" {
			messageNull(c, messageInfo, NO_UID)
			return
		}

		if username == "" && userName == "" {
			messageNull(c, messageInfo, INVALID_UID)
			return
		}
		if username == "" {
			err, _ := RegisterUserInfo.SeekUser(userName)
			if err != nil {
				messageNull(c, messageInfo, NO_USERNAME)
			} else {
				_, ok := LoginInfo.LoginUserMap[userName]
				if !ok {

					messageNull(c, messageInfo, NO_LOGIN)

					return
				} else {
					LoginInfo.Exit(userName)
					messageNull(c, messageInfo, SUCCESS)
					return
				}
			}
		}
		if userName == "" {
			_, ok := LoginInfo.LoginUserMap[username]
			if !ok {

				messageNull(c, messageInfo, NO_LOGIN)

				return
			} else {
				LoginInfo.Exit(username)
				messageNull(c, messageInfo, SUCCESS)
				return
			}
		}
	}
}
func (c *DataController) FindPwd() {
	messageInfo := initialize()
	if c.Ctx.Request.Method == "POST" {
		gwid := c.Input().Get("gwid")
		username := c.Input().Get("username")
		if gwid == "" {
			messageNull(c, messageInfo, NO_GWID)
			return
		}

		FileGwid, err := systemcfg.GetGatewayID()
		if gwid == FileGwid && username == "" {
			messageNull(c, messageInfo, USERNAME_NIL)
			return
		}
		if err != nil {

			messageNull(c, messageInfo, GWID_NOTFOUND)
		} else {
			if gwid == FileGwid {
				err, user := RegisterUserInfo.SeekUser(username)
				if err != nil {

					messageNull(c, messageInfo, NO_USERNAME)
					return
				} else {
					type StructUser struct {
						Username string `json:"username"`
						Password string `json:"password"`
					}
					userMsg := StructUser{Username: user.Username, Password: user.Password}

					messageInfo.Data = userMsg
					messageNull(c, messageInfo, SUCCESS)

					return
				}

			} else {

				messageNull(c, messageInfo, INVALID_GWID)

				return
			}
		}
	}
}
