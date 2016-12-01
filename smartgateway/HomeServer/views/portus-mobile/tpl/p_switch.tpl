<!DOCTYPE html>
<html>
    <meta http-equiv="content-type" content="text/html;charset=UTF-8">
  {{template "../tpl/p_gateway_css.tpl"}}

    <link rel="stylesheet" href="/static/bootstrap-web/css/bootstrap.min.css">
    <!--开始滑动匡布局-->
    <script src="/static/js/jquery-1.7.2.min.js"></script>
    <!--布局结束-->
    <script src="/static/bootstrap-web/js/bootstrap.min.js"></script>
       <script src="/static/bootstrap-web/js/bootstrap.js"></script>


<body>

<form id="formContainer" class="form form-horizontal" method="post" action="#">
<div class="panel-body">


<div class="form-group">
<label class="control-label col-sm-2" for="DisplayName">节点:</label>
<div class="col-sm-4">
<input id="DisplayName" class="form-control" type="text"  placeholder="{{.Address}}">
</div>
</div>

<div class="form-group">
<label class="control-label col-sm-2" for="FromeDate">类型:</label>
<div class="col-sm-4">
<input id="DisplayName" class="form-control" type="text"  placeholder="{{.Category}}">
</div>

</div>

<div class="form-group">
<label class="control-label col-sm-2" for="FromeDate">状态:</label>
<div class="col-sm-4">
<input id="DisplayName" class="form-control" type="text"  placeholder="off">
</div>
</div>

<div class="form-group">
<label class="control-label col-sm-2" for="FromeDate">操作:</label>
<div class="col-sm-4">
<!-- <div class="switch" data-on-label="OK" data-off-label="NO">
    <input type="checkbox" checked />
</div> -->
 <a href="/on">开</a> | <a href="/off">关</a> 
</div>
</div>
</form>
</div>

</body>

</html>