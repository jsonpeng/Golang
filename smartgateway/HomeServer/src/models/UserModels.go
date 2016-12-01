package models

import (
	"errors"
	"log"
	"time"

	//"github.com/astaxie/beego"
	"github.com/astaxie/beego/orm"
	"github.com/astaxie/beego/validation"
	_ "github.com/go-sql-driver/mysql"
	"github.com/portus/SmartGateWay/HomeServer/src/lib"
)

/*
type p_admin struct {
	Id                       int    //`orm:"column(id);pk"`
	p_admin_password         string //`orm:"column(admin_password);"`
	p_admin_createtime       string //`orm:"column(admin_createtime);"`
	p_admin_updatetime       string //`orm:"column(admin_updatetime);"`
	p_admin_topiccount       string //`orm:"column(admin_topiccount);"`
	p_admin_topiclastadminid string //`orm:"column(admin_topiclastadminid);"`
}

type p_user struct {
	Id                      int64  `orm:"column(id);pk"`
	P__userName             string `orm:"column(user_username);"`
	P_passWord              string `orm:"column(user_password);"`
	P_user_createTime       string `orm:"column(user_createtime);"`
	P_user_updateTime       string `orm:"column(user_updatetime);"`
	P_user_tel              string `orm:"column(user_tel);"`
	P_User_email            string `orm:"column(user_email);"`
	P_user_topicCount       string `orm:"column(user_topocCount);"`
	P_user_topicLastAdminID string `orm:"column(user_topicLastAdminID);"`
}
*/
//用户表
type User struct {
	Id            int64
	Username      string    `orm:"unique;size(32)" form:"Username"  valid:"Required;MaxSize(20);MinSize(6)"`
	Password      string    `orm:"size(32)" form:"Password" valid:"Required;MaxSize(20);MinSize(6)"`
	Repassword    string    `orm:"-" form:"Repassword" valid:"Required"`
	Nickname      string    `orm:"unique;size(32)" form:"Nickname" valid:"Required;MaxSize(20);MinSize(2)"`
	Email         string    `orm:"size(32)" form:"Email" valid:"Email"`
	Remark        string    `orm:"null;size(200)" form:"Remark" valid:"MaxSize(200)"`
	Status        int       `orm:"default(2)" form:"Status" valid:"Range(1,2)"`
	Lastlogintime time.Time `orm:"null;type(datetime)" form:"-"`
	Createtime    time.Time `orm:"type(datetime);auto_now_add" `
	Role          []*Role   `orm:"rel(m2m)"`
}

//节点表
type Node struct {
	Id     int64
	Title  string  `orm:"size(100)" form:"Title"  valid:"Required"`
	Name   string  `orm:"size(100)" form:"Name"  valid:"Required"`
	Level  int     `orm:"default(1)" form:"Level"  valid:"Required"`
	Pid    int64   `form:"Pid"  valid:"Required"`
	Remark string  `orm:"null;size(200)" form:"Remark" valid:"MaxSize(200)"`
	Status int     `orm:"default(2)" form:"Status" valid:"Range(1,2)"`
	Group  *Group  `orm:"rel(fk)"`
	Role   []*Role `orm:"rel(m2m)"`
}

//
//角色表
type Role struct {
	Id     int64
	Title  string  `orm:"size(100)" form:"Title"  valid:"Required"`
	Name   string  `orm:"size(100)" form:"Name"  valid:"Required"`
	Remark string  `orm:"null;size(200)" form:"Remark" valid:"MaxSize(200)"`
	Status int     `orm:"default(2)" form:"Status" valid:"Range(1,2)"`
	Node   []*Node `orm:"reverse(many)"`
	User   []*User `orm:"reverse(many)"`
}

//分组表
type Group struct {
	Id     int64
	Name   string  `orm:"size(100)" form:"Name"  valid:"Required"`
	Title  string  `orm:"size(100)" form:"Title"  valid:"Required"`
	Status int     `orm:"default(2)" form:"Status" valid:"Range(1,2)"`
	Sort   int     `orm:"default(1)" form:"Sort" valid:"Numeric"`
	Nodes  []*Node `orm:"reverse(many)"`
}

func RegisterDB() {
	//注册 model
	orm.RegisterModel(new(User), new(Node), new(Role), new(Group))
	//注册驱动
	orm.RegisterDriver("mysql", orm.DRMySQL)
	//注册默认数据库
	orm.RegisterDataBase("default", "mysql", "root:123456@tcp(localhost:3306)/default?charset=utf8")
}

func (u *User) Valid(v *validation.Validation) {
	if u.Password != u.Repassword {
		v.SetError("Repassword", "两次输入的密码不一样")
	}
}

//验证用户信息
func checkUser(u *User) (err error) {
	valid := validation.Validation{}
	b, _ := valid.Valid(&u)
	if !b {
		for _, err := range valid.Errors {
			log.Println(err.Key, err.Message)
			return errors.New(err.Message)
		}
	}
	return nil
}

/************************************************************/

//get user list
func Getuserlist(page int64, page_size int64, sort string) (users []orm.Params, count int64) {
	o := orm.NewOrm()
	user := new(User)
	qs := o.QueryTable(user)
	var offset int64
	if page <= 1 {
		offset = 0
	} else {
		offset = (page - 1) * page_size
	}
	qs.Limit(page_size, offset).OrderBy(sort).Values(&users)
	count, _ = qs.Count()
	return users, count
}

//添加用户
func AddUser(u *User) (int64, error) {
	if err := checkUser(u); err != nil {
		return 0, err
	}
	o := orm.NewOrm()
	user := new(User)
	user.Username = u.Username
	user.Password = lib.Strtomd5(u.Password)
	user.Nickname = u.Nickname
	user.Email = u.Email
	user.Remark = u.Remark
	user.Status = u.Status

	id, err := o.Insert(user)
	return id, err
}

//更新用户
func UpdateUser(u *User) (int64, error) {
	if err := checkUser(u); err != nil {
		return 0, err
	}
	o := orm.NewOrm()
	user := make(orm.Params)
	if len(u.Username) > 0 {
		user["Username"] = u.Username
	}
	if len(u.Nickname) > 0 {
		user["Nickname"] = u.Nickname
	}
	if len(u.Email) > 0 {
		user["Email"] = u.Email
	}
	if len(u.Remark) > 0 {
		user["Remark"] = u.Remark
	}
	if len(u.Password) > 0 {
		user["Password"] = lib.Strtomd5(u.Password)
	}
	if u.Status != 0 {
		user["Status"] = u.Status
	}
	if len(user) == 0 {
		return 0, errors.New("update field is empty")
	}
	var table User
	num, err := o.QueryTable(table).Filter("Id", u.Id).Update(user)
	return num, err
}

func DelUserById(Id int64) (int64, error) {
	o := orm.NewOrm()
	status, err := o.Delete(&User{Id: Id})
	return status, err
}

func GetUserByUsername(username string) (user User) {
	user = User{Username: username}
	o := orm.NewOrm()
	o.Read(&user, "Username")
	return user
}

/*func (u *User) TableName() string {
	return beego.AppConfig.String("portus_user_table")
}


import (
	"database/sql"
	"fmt"
	"log"
	"time"
	//"os"
	//"path"
	//"github.com/Unknwon/com"
	"github.com/astaxie/beego/orm"
	_ "github.com/go-sql-driver/mysql"
)



const (
	_DB_NAME      = "data/Homeserver.db"
	_MYSQL_DRIVER = "mysql"
)

type p_admin struct {
	Id                       int64
	P_admin_password         string
	P_admin_update           time.Time `orm:"index"`
	P_admin_create           time.Time `orm:"index"`
	P_admin_topiccount       int64
	P_admin_topiclastadminid int64
}

type p_user struct {
	Id                      int64
	P_user_username         string
	P_user_password         string
	P_user_online           bool
	P_user_tel              int64
	P_User_email            string
	P_user_create           time.Time `orm:"index"`
	P_user_update           time.Time `orm:"index"`
	P_user_topicCount       int64
	P_user_topicLastAdminID int64
}

func RegisterDB() {

	//	if !com.IsExist(_DB_NAME) {
	//		os.MkdirAll(path.Dir(_DB_NAME), os.ModePerm)
	//		os.Create(_DB_NAME)
	//	}
	//注册 model
	orm.RegisterModel(new(p_admin), new(p_user))
	//注册驱动
	orm.RegisterDriver("mysql", orm.DRMySQL)
	//注册默认数据库
	orm.RegisterDataBase("default", "mysql", "root:123456@/default?charset=utf8")
}

func checkErr(err error) {
	if err != nil {
		fmt.Println(err.Error())
	}
}
func LookUp_admin(password string, db *sql.DB) bool {
	query := "select p_admin_password from p_admin where p_admin_password=" + "'" + password + "'"
	var v string
	err := db.QueryRow(query).Scan(&v)
	checkErr(err)
	if v == password {
		return false
	} else {
		return true
	}
}

func Insert_admin(data interface{}) bool {
	orm.Debug = true
	orm.RunSyncdb("p_admin", false, true)
	var admin p_admin
	admin = data.(p_admin)
	db, err := sql.Open("mysql", "root:123456@tcp(localhost:3306)/default")
	if err != nil {
		fmt.Println("打开数据库失败")
		return false
	}
	err = db.Ping()
	if err != nil {
		fmt.Println("连接数据库失败")
		return false
	}
	checkErr(err)
	defer db.Close()
	result := LookUp_admin(admin.P_admin_password, db)
	if result == true {
		fmt.Println(admin)
		stmt, err := db.Prepare("INSERT INTO p_admin(Id,P_admin_password,P_admin_create,P_admin_update,P_admin_topiccount,P_admin_topicLastadminiD) VALUES(?,?,?,?)")
		defer stmt.Close()
		if err != nil {
			log.Println(err)
			return false
		}
		stmt.Exec(admin.Id, admin.P_admin_password, admin.P_admin_create, admin.P_admin_update, admin.P_admin_topiccount, admin.P_admin_topiclastadminid)
		return true
	} else {
		stmt, _ := db.Prepare("update p_admin set P_admin_psssword =? where p_admin_update=?")
		strtime := time.Now().Format("2006-01-02 15:04:05")
		stmt.Exec(strtime, admin.P_admin_password)
		return true
	}
}
*/
