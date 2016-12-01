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
                        { ele: { type: 'select', name: 'province', title: 'node:', withNull: true, items: [{ text: 'node1', value: 'GuangDong', extendAttr: { id: 1000 } }, { text: 'node2', value: 'HuNan', extendAttr: { id: 2000 } }] } },
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



<!--触控层-->
<button class="btn btn-primary btn-lg" data-toggle="modal" data-target="#myModal">
    SEND
</button>
<!-- 模态框（Modal） -->
<div class="modal fade" id="myModal" tabindex="-1" zindex="1110" role="dialog" aria-labelledby="myModalLabel" aria-hidden="true">
    <div class="modal-dialog modal-lg"  style="width:600px;height:800px;position:absolute;top:50%;left:30%;margin-left:50px;margin-top:40px">
        <div class="modal-content">
            <div class="modal-header">
                <button type="button" class="close" data-dismiss="modal" aria-hidden="true">
                    &times;
                </button>


                <h4 class="modal-title" id="myModalLabel">
                    网关操作个性界面
                </h4>
            </div>
            <div class="modal-body">
                <label class="control-label col-sm-1" for="region">MODE:</label>
<div class="col-sm-3">
<select id="region" class="form-control" name="region" style="width:200px;position:absolute;top:50%;">
<option class="" value="">--selected--</option>
<option value="TH" data-id="1000001001" data-parentid="1000001">SWITCH_ALL_SET</option>
<option value="HZ" data-id="1000001002" data-parentid="1000001">SWITCH_ALL_ADD</option>
<option value="YX" data-id="1000001003" data-parentid="1000001">SWITCH_ALL_DELETE</option>
<option value="BY" data-id="1000001004" data-parentid="1000001">SWITCH_ALL_REPORT</option>

</select>
</div>
            </div>
            <div class="modal-footer">
                <button type="button" class="btn btn-default" data-dismiss="modal">关闭
                </button>
                <button type="button"  id="send1" class="btn btn-primary">
                    SEND
                </button>
   //              <script type="text/javascript">
   //              $("#send1").click(function () {
   //   // //取表格的选中行数据
   //   // var arrselections = $("#tb_departments").bootstrapTable('getSelections');
   //   // if (arrselections.length <= 0) {
   //   //   toastr.warning('请选择有效数据');
   //   //   return;
   //   // }
 
   //   Ewin.confirm({ message: "确认要删除选择的数据吗？" }).on(function (e) {
   //     if (!e) {
   //       return;
   //     }
   //     $.ajax({
   //       type: "post",
   //       url: "/api/DepartmentApi/Delete",
   //       data: { "": JSON.stringify(arrselections) },
   //       success: function (data, status) {
   //         if (status == "success") {
   //           toastr.success('提交数据成功');
   //           $("#tb_departments").bootstrapTable('refresh');
   //         }
   //       },
   //       error: function () {
   //         toastr.error('Error');
   //       },
   //       complete: function () {
 
   //       }
 
   //     });
   //   });
   // });
   //              </script>
            </div>
        </div><!-- /.modal-content -->
    </div><!-- /.modal -->
  