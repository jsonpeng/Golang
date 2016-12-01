
<!DOCTYPE html>
<html>
    <meta http-equiv="content-type" content="text/html;charset=UTF-8">
    <link rel="stylesheet" href="/static/bootstrap-web/css/bootstrap.min.css">
    <script src="/static/js/jquery-1.7.2.min.js"></script>
    <script src="/static/bootstrap-web/js/bootstrap.min.js"></script>
   
    {{template "../tpl/p_all_css.tpl"}}
<body>


<div class="panel panel-info">
  <div class="panel-heading"> 节点控制</div>
 <td> 
 <!-- 按钮触发模态框 -->
<button class="btn btn-primary " data-toggle="modal" data-target="#myModal" >
 <span class="glyphicon glyphicon-resize-horizontal">连接</span>
</button>
<!-- 模态框（Modal） -->
<div class="modal fade" id="myModal" tabindex="-1" role="dialog" aria-labelledby="myModalLabel" aria-hidden="true">
  <div class="modal-dialog" style="width:600px;height:600px;position:absolute;top:50%;left:30%;margin-left:50px;margin-top:-120px">
    <div class="modal-content">
      <div class="modal-header">
        <button type="button" class="close" data-dismiss="modal" aria-hidden="true">
          &times;
        </button>
        <h4 class="modal-title" id="myModalLabel">
         连接设备
        </h4>
      </div>
      <div class="modal-body">
       请选择您连接的设备
      </div>
      <div class="modal-footer">
        <button type="button" class="btn btn-default" data-dismiss="modal">关闭
        </button>
        <button type="button" class="btn btn-primary">
         连接
        </button>
      </div>
    </div><!-- /.modal-content -->
  </div><!-- /.modal -->
</div> 

<!--第二个模态框-->
<button class="btn btn-primary " data-toggle="modal" data-target="#myModal" >
 <span class="glyphicon glyphicon-plus">添加</span>
</button>
<!-- 模态框（Modal） -->
<div class="modal fade" id="myModal" tabindex="-1" role="dialog" aria-labelledby="myModalLabel" aria-hidden="true">
  <div class="modal-dialog" style="width:600px;height:600px;position:absolute;top:50%;left:30%;margin-left:50px;margin-top:-120px">
    <div class="modal-content">
      <div class="modal-header">
        <button type="button" class="close" data-dismiss="modal" aria-hidden="true">
          &times;
        </button>
        <h4 class="modal-title" id="myModalLabel">
         添加设备
        </h4>
      </div>
      <div class="modal-body">
       请选择您添加的设备
      </div>
      <div class="modal-footer">
        <button type="button" class="btn btn-default" data-dismiss="modal">关闭
        </button>
        <button type="button" class="btn btn-primary">
         添加
        </button>
      </div>
    </div><!-- /.modal-content -->
  </div><!-- /.modal -->
</div><!--结束第二个模态框按钮-->
<!--第三个模态框-->
<button class="btn btn-primary " data-toggle="modal" data-target="#myModal" >
<span class="glyphicon glyphicon-remove">删除</span>
</button>
<!-- 模态框（Modal） -->
<div class="modal fade" id="myModal" tabindex="-1" role="dialog" aria-labelledby="myModalLabel" aria-hidden="true">
  <div class="modal-dialog" style="width:600px;height:600px;position:absolute;top:50%;left:30%;margin-left:50px;margin-top:-120px">
    <div class="modal-content">
      <div class="modal-header">
        <button type="button" class="close" data-dismiss="modal" aria-hidden="true">
          &times;
        </button>
        <h4 class="modal-title" id="myModalLabel">
         删除设备
        </h4>
      </div>
      <div class="modal-body">
       请选择您删除的设备
      </div>
      <div class="modal-footer">
        <button type="button" class="btn btn-default" data-dismiss="modal">关闭
        </button>
        <button type="button" class="btn btn-primary">
         删除
        </button>
      </div>
    </div><!-- /.modal-content -->
  </div><!-- /.modal -->
</div><!--结束第三个模态框按钮-->
<!--第四个模态框-->
<button class="btn btn-primary " data-toggle="modal" data-target="#myModal" >
<span class="glyphicon glyphicon-refresh">重置</span></td>
</button>
<!-- 模态框（Modal） -->
<div class="modal fade" id="myModal" tabindex="-1" role="dialog" aria-labelledby="myModalLabel" aria-hidden="true">
  <div class="modal-dialog" style="width:600px;height:600px;position:absolute;top:50%;left:30%;margin-left:50px;margin-top:-120px">
    <div class="modal-content">
      <div class="modal-header">
        <button type="button" class="close" data-dismiss="modal" aria-hidden="true">
          &times;
        </button>
        <h4 class="modal-title" id="myModalLabel">
         重置设备
        </h4>
      </div>
      <div class="modal-body">
       请选择您重置的设备
      </div>
      <div class="modal-footer">
        <button type="button" class="btn btn-default" data-dismiss="modal">关闭
        </button>
        <button type="button" class="btn btn-primary">
         重置
        </button>
      </div>
    </div><!-- /.modal-content -->
  </div><!-- /.modal -->
</div><!--结束第二个模态框按钮-->
 </td>
<!-- <span class="glyphicon glyphicon-resize-horizontal">连接</span>|<span class="glyphicon glyphicon-plus">添加</span>| 
                  <span class="glyphicon glyphicon-remove">删除</span>| <span class="glyphicon glyphicon-refresh">重置</span></td> -->
<table class="table  table-hover">

   <thead>
      <tr>
       
        <th>node</th>
        <th>class</th>
        <th>category</th>
        <th>cmd</th>
      </tr>
   </thead>
   <tbody>

   

         

      </tr>
 
   </tbody>
   </table>
</div>
<!-- </div> -->



</div>
</body>
</html>
