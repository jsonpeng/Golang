<!DOCTYPE html>
<html>
    <meta http-equiv="content-type" content="text/html;charset=UTF-8">
  

    <link rel="stylesheet" href="/static/bootstrap-web/css/bootstrap.min.css">
    <!--开始滑动匡布局-->
    <script src="/static/js/jquery-2.1.1.min.js"></script>
    <!--布局结束-->
    <script src="/static/bootstrap-web/js/bootstrap.min.js"></script>
       <script src="/static/bootstrap-web/js/bootstrap.js"></script>

   
    {{template "../tpl/p_all_css.tpl"}}

<body>

 <div class="panel panel-default">
            <div class="panel-heading">
                <label>网关测试</label>
            </div>
            <div class="panel-body">
            <form id="formContainer" class="form form-horizontal" action="#">
</form>
            </div>
        </div>

 <script>
            $(function () {
               var eles = [
                  [
                        { ele: { type: 'select', name: 'province', title: 'node:', withNull: true, items: [{ text: {{.Address}}, value: 'GuangDong', extendAttr: { id: 1000 } }, { text: 'node2', value: 'HuNan', extendAttr: { id: 2000 } }] } },
                        { ele: { type: 'select', name: 'city', title: 'class:', withNull: true, items: [{ "text": "SWITCH_ALL", "value": "GuangZhou", "extendAttr": { "id": "1000001", "parentId": "1000" } }, { "text": "SWITCH_ONE", "value": "HuaDu", "extendAttr": { "id": "1000002", "parentId": "1000" } }, { "text": "node2_SWITCH_ONE", "value": "ShaoYang", "extendAttr": { "id": "2000001", "parentId": "2000" } }, { "text": "node2_SWITCH_TWO", "value": "ChangSha", "extendAttr": { "id": "2000002", "parentId": "2000" } }] } },
                        { ele: { type: 'select', name: 'region', title: 'cmd:', withNull: true, items: [{ "text": "SWITCH_ALL_SET", "value": "TH", "extendAttr": { "id": "1000001001", "parentId": "1000001" } }, { "text": "SWITCH_ALL_SET", "value": "HZ", "extendAttr": { "id": "1000001002", "parentId": "1000001" } }, { "text": "SWITCH_ALL_SET", "value": "YX", "extendAttr": { "id": "1000001003", "parentId": "1000001" } }, { "text": "SWITCH_ALL_SET", "value": "BY", "extendAttr": { "id": "1000001004", "parentId": "1000001" } }, { "text": "SWITCH_ALL_SET", "value": "HD", "extendAttr": { "id": "1000002001", "parentId": "1000002" } }, { "text": "SWITCH_ONE_SET", "value": "aa", "extendAttr": { "id": "2000001001", "parentId": "2000001" } }, { "text": "SWITCH_ALL_SET", "value": "wc", "extendAttr": { "id": "2000002001", "parentId": "2000002" } }, { "text": "SWITCH_ALL_SET", "value": "yh", "extendAttr": { "id": "2000002002", "parentId": "2000002" } }] } },
                  ]
            ];
            var bsForm = new BSForm({ eles: eles,autoLayout: true }).Render('formContainer', function (sf) {
                //编辑页面的绑定
                sf.InitFormData({
                    province: 'GuangDong',
                    city: 'GuangZhou',
                    region:'TH'
                });
                //必须先赋值再生成插件
                global.Fn.CascadeSelect({ targets: ['province', 'city', 'region'], primaryKey: 'data-id', relativeKey: 'data-parentId' });
            });
            });
        </script>


<HR>

 <!-- 按钮触发模态框 -->
<button class="btn btn-primary " data-toggle="modal" data-target="#myModal" >
 <span class="glyphicon glyphicon-saved" >send</span>
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
         测试model
        </h4>
      </div>
      <div class="modal-body">
       <form id="formContainer" class="form form-horizontal" action="#">
<div class="form-group">
<label class="control-label col-sm-1" for="province">MODE:</label>
<div class="col-sm-3">
<select id="province" class="form-control" name="province">
<option value="">--请选择--</option>
<option value="GuangDong" data-id="1000">SWITCH_ALL_SEND</option>
<option value="HuNan" data-id="2000">SWITCH_ALL_SET</option>
</select>
</div>
</div>
</form>
</div>
      </div>
      <div class="modal-footer">
        <button type="button" class="btn btn-default" data-dismiss="modal">
         <span class="glyphicon glyphicon-remove" >关闭</span>
        </button>
        <button type="button" class="btn btn-primary">
          <span class="glyphicon glyphicon-saved" >send</span>
        </button>
      </div>
    </div><!-- /.modal-content -->
  </div><!-- /.modal -->
</div> 
  
  </body>

  </html>