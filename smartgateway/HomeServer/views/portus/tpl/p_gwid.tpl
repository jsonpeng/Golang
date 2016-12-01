<!DOCTYPE html>
<html>
<head>
	<meta charset="utf-8"> 
	<title>请输入GWID</title>
   <link rel="stylesheet" href="/static/bootstrap-web/css/bootstrap.min.css">
    <!--开始滑动匡布局-->
    <script src="/static/js/jquery-2.1.1.min.js"></script>
    <!--布局结束-->
    <script src="/static/bootstrap-web/js/bootstrap.min.js"></script>
       <script src="/static/bootstrap-web/js/bootstrap.js"></script>

   
    {{template "../tpl/p_all_css.tpl"}}
	
</head>
<body>


<button class="btn btn-primary btn-lg" data-toggle="modal" data-target="#myModal">
	请输入
</button>
<!-- 模态框（Modal） -->
<div class="modal fade" id="myModal" tabindex="-1" role="dialog" aria-labelledby="myModalLabel" aria-hidden="true">
	<div class="modal-dialog" style="width:600px;height:600px;position:absolute;top:50%;left:30%;margin-left:50px;margin-top:-120px"
>
		<div class="modal-content">
			<div class="modal-header">
				<button type="button" class="close" data-dismiss="modal" 
						aria-hidden="true">×
				</button>
				<h4 class="modal-title" id="myModalLabel">
				    请输入GWID
				</h4>
			</div>
			<div class="modal-body">
				<form id="formContainer" class="form form-horizontal" action="/getpassword" method="post">
<div class="panel-body">
<div class="form-group">
<label class="control-label col-sm-2" for="DisplayName">Gwid:</label>
<div class="col-sm-4">
<input id="DisplayName" class="form-control" type="text" name="gwid"  placeholder="请输入GWID">  


</div>
			</div>

			<div class="form-group">
<label class="control-label col-sm-2" for="DisplayName">password:</label>
<div class="col-sm-4">
<input id="DisplayName" class="form-control" type="text" name="password" value="{{.pwd}}" placeholder="{{.pwd}}">  


</div>
			</div>



			<div class="form-group">
<div class="col-sm-4">
  <input type="submit" value="确认" class="btn btn-primary">  
</div>
			</div>

			</form>
			<div class="modal-footer">
				<button type="button" class="btn btn-default" 
						data-dismiss="modal">关闭
				</button>
			<!-- 	<button type="button" class="btn btn-primary" >
					确认
				</button> -->
			</div>
		</div><!-- /.modal-content -->
	</div><!-- /.modal-dialog -->
</div><!-- /.modal -->
<script>
$(function () { $('#myModal').modal({
	keyboard: true
})});
</script>

</body>
</html>