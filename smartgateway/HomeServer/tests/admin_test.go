/**
*copyright 2016 Portus(Wuhan). All rights reserved.
*@Author: PengYun
*@Date:   2016/07/28
*@Update: 2016/08/17
*Here is the program unit test module,
*used to test the structure of the debugger
 */
package test

//The introduction of the outer package to function
import (
	"net/http"
	"net/http/httptest"
	"path/filepath"
	"runtime"
	"testing"

	_ "github.com/portus/smartgateway/HomeServer/routers"

	"github.com/astaxie/beego"
	. "github.com/smartystreets/goconvey/convey"
)

//loading
func init() {
	_, file, _, _ := runtime.Caller(1)
	apppath, _ := filepath.Abs(filepath.Dir(filepath.Join(file, ".."+string(filepath.Separator))))
	beego.TestBeegoInit(apppath)
}

// TestIndex is a sample to run an endpoint test
func TestReg(t *testing.T) {
	r, _ := http.NewRequest("GET", "/admin/index", nil)
	w := httptest.NewRecorder()
	beego.BeeApp.Handlers.ServeHTTP(w, r)

	beego.Trace("testing", "TestAdmin", "Code[%d]\n%s", w.Code, w.Body.String())

	Convey("Subject: Test Station Endpoint\n", t, func() {
		Convey("Status Code Should Be 200", func() {
			So(w.Code, ShouldEqual, 200)
		})
		Convey("The Result Should Not Be Empty", func() {
			So(w.Body.Len(), ShouldBeGreaterThan, 0)
		})
	})
}
