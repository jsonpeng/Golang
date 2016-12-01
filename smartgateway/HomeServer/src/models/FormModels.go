package models

/*
type user struct {
	Name string
}
*/
var Name string

func Inputname(username string) {
	Name = username
}

func GetName() string {
	return Name
}
