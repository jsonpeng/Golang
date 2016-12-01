package models

type UserInfo struct {
	Id       int    `json: "id"`
	UserName string `json:"username"`
	Password string `json:"password"`
	Online   bool   `json:"online"`
}
