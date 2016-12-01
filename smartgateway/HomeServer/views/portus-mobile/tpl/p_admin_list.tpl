
<!DOCTYPE html>
<html>
    <meta http-equiv="content-type" content="text/html;charset=UTF-8">
{{template "../tpl/p_all_css.tpl"}}
<body>

<div id="navi">
    <div id='naviDiv'>
    
        <span><img src="/static/img/arror.gif" width="7" height="11" border="0" alt=""></span>&nbsp;管理员信息<span>
      <!--   <span><img src="/static/img/arror.gif" width="7" height="11" border="0" alt=""></span>&nbsp;个人资料添加<span>&nbsp; -->
    </div>
</div>
<div id="tips">
</div>

<form id="formContainer" class="form form-horizontal" action="#" method="post">
<div class="panel-body">


<div class="form-group">
<label class="control-label col-sm-2" for="DisplayName">用户名:</label>
<div class="col-sm-4">
<input id="DisplayName" class="form-control" type="text" name="username"  readonly="readonly" placeholder="admin">
</div>
</div>

<div class="form-group">
<label class="control-label col-sm-2" for="FromeDate">密码:</label>
<div class="col-sm-4">
<div class="input-group">
<input id="FromeDate" class="date-picker form-control" type="password" name="password" placeholder="{.password}">
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
<input  type="checkbox" name="online">
</div>
</div>
</div>

  <tr>
    <td colspan="2" align="center"><input class="button" type="submit" value="确认"></td>
  </tr>
  </div>
</form>

</body>
</html>



