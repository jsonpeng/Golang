<!DOCTYPE html>
<html>
    <meta http-equiv="content-type" content="text/html;charset=UTF-8">
<script src="/static/assets/js/aj.js"></script> 
    {{template "../tpl/p_all_css.tpl"}}
    
<body>

<div id="navi">
    <div id='naviDiv'>
        <span><img src="/static/img/arror.gif" width="7" height="11" border="0" alt=""></span>&nbsp;查看个人信息<span>
      <!--   <span><img src="/static/img/arror.gif" width="7" height="11" border="0" alt=""></span>&nbsp;个人资料添加<span>&nbsp; -->
    </div>
</div>



<div ng-app="myApp" ng-controller="information">
<form id="formContainer" class="form form-horizontal" action="/userinfo" method="post"  name="registerForm" novalidate>
<div class="panel-body">
<div class="form-group">
<label class="control-label col-sm-2" for="DisplayName">用户名:</label>
<div class="col-sm-4">
<input id="DisplayName" class="form-control" type="text" name="username"  readonly="readonly"   value="{{.username}}" placeholder="{{.username}}">  


</div>
</div>


<div class="form-group">
<label class="control-label col-sm-2" for="DisplayName">密码:</label>
<div class="col-sm-4">
<div class="input-group">
<input id="DisplayName" class="form-control" type="text" name="password" value="{{.password}}" placeholder="{{.password}}" ng-model="password" ng-minlength="6" ng-maxlength="16" name="password" placeholder="请输入密码..." required>
                     <p>
                    <span style="color: red" ng-show="registerForm.password.$invalid">
                        <span ng-show="registerForm.password.$error.minlength">*密码长度不小于6</span>
                        <span ng-show="registerForm.password.$error.maxlength">*密码长度不超过16</span>
                    </span>
                </p>
<span class="input-group-btn">
<button class="btn btn-default" type="button">
<i class="glyphicon glyphicon-calendar"></i>
</button>
</span>
</div>
</div>
</div>
{{if .IsAd}}
{{else}}
<div class="form-group">
<label class="control-label col-sm-2" for="FromeDate">上线状态:</label>
<div class="col-sm-4">
<div class="input-group">
<input type="checkbox" name="online">
</div>
</div>
</div>
{{end}}
  <tr>
    <td colspan="2" align="center"><input class="button" type="submit" value="确认"   ng-disabled="registerForm.password.$invalid"></td>
  </tr>
 <script>
    var app = angular.module('myApp', []);
    app.controller('information', function($scope) {

    });
</script>
</div>
</form>

</div>
</body>
</html>



 




 