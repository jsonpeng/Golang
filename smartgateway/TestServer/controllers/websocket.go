package controllers

import (
	"encoding/json"
	//"encoding/json"
	"fmt"
	"net/http"
	"strconv"

	"github.com/astaxie/beego"
	"github.com/gorilla/websocket"

	"github.com/portus/SmartGateway/DateBase/LoginInfo"
	"github.com/portus/SmartGateway/DateBase/RegisterUserInfo"
	"github.com/portus/SmartGateway/DateBase/SystemCfg"
	"github.com/portus/SmartGateway/NodeClient"
)

// WebSocketController handles WebSocket requests.
//
type WebSocketController struct {
	CommonController
}

//websocket data struct
type WsData struct {
	uid  string
	ws   *websocket.Conn
	data []byte
}

//
func WelcomeInfo() []byte {
	Msg := nodeclient.RequestInfo{
		Action:  "NOTIFY",
		Command: "Welcome",
		Data:    "",
	}
	welcomeSign, _ := json.Marshal(Msg)
	return welcomeSign
}

func ExitInfo() []byte {
	Msg := nodeclient.RequestInfo{
		Action:  "NOTIFY",
		Command: "Exit",
		Data:    "",
	}
	exitSign, _ := json.Marshal(Msg)
	return exitSign
}

func AddWsOnlineUser(uid string, ws *websocket.Conn) {

	myuid, _ := strconv.Atoi(uid)
	username := RegisterUserInfo.SeekUserById(int64(myuid))
	_, user := RegisterUserInfo.SeekUser(username)
	uws, ok := LoginInfo.LoginUserMap[username]
	if ok {
		oldWs := uws.Ws
		uws.Ws.WriteMessage(websocket.TextMessage, ExitInfo())
		defer oldWs.Close()
		uws.Ws = ws
		fmt.Println("Have Been Login:", "username:", uws.UserName, "ws:", uws.Ws)
		return

	}
	//how to change the ws's information
	LoginInfo.Login(LoginInfo.LoginUser{user.Username, user.Password, ws, "192.168.32.1", "2016-8-26"})
	fmt.Println("The first Login:", "username:", username)
	//to do
	ws.WriteMessage(websocket.TextMessage, WelcomeInfo())
}

func DelWsOnlineUser(uid string) {
	myuid, _ := strconv.Atoi(uid)
	username := RegisterUserInfo.SeekUserById(int64(myuid))
	delete(LoginInfo.LoginUserMap, username)
}

func (c *WebSocketController) Post() {

	var info MessageInfo
	info.Action = REPLY
	info.ErrorNumber = SUCCESS
	wsurl, _ := systemcfg.GetWebsocketURL()
	wsurl = wsurl + "/ws"
	info.Data = wsurl
	data, _ := json.MarshalIndent(info, "", "\t\t")
	c.Ctx.WriteString(string(data))

	//c.Ctx.WriteString(wsurl)
	return
}

func (c *WebSocketController) Get() {
	fmt.Println("welcome to WsTask...")
	uid := c.Input().Get("uid")

	myuid, _ := strconv.Atoi(uid)
	username := RegisterUserInfo.SeekUserById(int64(myuid))
	if username == "" {
		c.ReportError(NO_USERNAME)
		return
	}

	//Upgrade from http request to WebSocket.
	ws, err := websocket.Upgrade(c.Ctx.ResponseWriter, c.Ctx.Request, nil, 1024, 1024)
	if _, ok := err.(websocket.HandshakeError); ok {
		http.Error(c.Ctx.ResponseWriter, "Not a websocket handshake", 400)
		return
	} else if err != nil {
		beego.Error("Cannot setup WebSocket connection:", err)
		return
	}

	// Join ws to online userlist.
	AddWsOnlineUser(uid, ws)
	defer DelWsOnlineUser(uid)

	// Message receive loop.
	for {
		_, p, err := ws.ReadMessage()
		if err != nil {
			return
		}

		data, err := nodeclient.ParseData(p)
		fmt.Println("data:", data)
		if err == nil {
			fmt.Println("Ws ReadMessage:", data)
			reply, _ := nodeclient.DoNodeClientUiMessage(data)
			ws.WriteJSON(reply)
		}
	}

}
