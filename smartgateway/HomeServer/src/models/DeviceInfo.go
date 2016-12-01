package models

//"encoding/json"

type DeviceInfo struct {
	SystemType string `json:"systemType"` //ios or android or web
	SystemVer  string `json:"systemVersion"`
	AppVer     string `json:"appVersion"`
}
