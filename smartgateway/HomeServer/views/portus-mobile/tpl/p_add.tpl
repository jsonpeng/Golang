
<!DOCTYPE html>
<html>
    <meta http-equiv="content-type" content="text/html;charset=UTF-8">
  
     <script src="/static/js/aj.js"></script>
    {{template "../tpl/p_all_css.tpl"}}
  

<body>


<div ng-app="myApp" ng-controller="addcontroller">
<form id="formContainer" class="form form-horizontal" method="post" action="/add"  name="registerForm" novalidate>
<div class="panel-body">
<div class="form-group">
<label class="control-label col-sm-2" for="DisplayName">用户名:</label>
<div class="col-sm-4">
<input id="DisplayName" class="form-control" type="text" ng-model="username" ng-minlength="5" ng-maxlength="10" name="username" id="username"  placeholder="请输入用户名" required>
    <p>
                    <span style="color: red" ng-show="registerForm.username.$invalid">
                        <span ng-show="registerForm.username.$error.minlength">*用户名长度不小于5</span>
                        <span ng-show="registerForm.username.$error.maxlength">*用户名长度不超过10</span>
                        <span>
                    </span>
                </p>
</div>
</div>

<div class="form-group">
<label class="control-label col-sm-2" for="FromeDate">密码:</label>
<div class="col-sm-4">
<div class="input-group">
<input id="FromeDate" class="date-picker form-control" type="password"  ng-model="password" ng-minlength="6" ng-maxlength="16" name="password" id="password" required placeholder="请输入密码">
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

<div class="form-group">
<label class="control-label col-sm-2" for="FromeDate">确认密码:</label>
<div class="col-sm-4">
<div class="input-group">
<input id="FromeDate" class="date-picker form-control" type="password"  ng-model="pas" name="pas" id="pas"  required placeholder="请再输一遍密码">
                     <p>
                    <span style="color: red" ng-show="registerForm.password.$valid">
                        <span ng-show="pas!=password">*两次密码输入不一致</span>
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

<div class="form-group">
<label class="control-label col-sm-2" for="FromeDate">上线状态:</label>
<div class="col-sm-4">
<div class="input-group">
<input type="checkbox" name="online">

</div>
</div>
</div>
 <tr>
    <td colspan="2" align="center"><input class="button" type="submit" value="确认"  ng-disabled="registerForm.username.$invalid  || registerForm.password.$invalid || pas != password"></td>
  </tr>

</div>
</form>
</div>
</body>
</html>

<script>
    var app = angular.module('myApp', []);
    app.controller('addcontroller', function($scope) {
    });
</script>