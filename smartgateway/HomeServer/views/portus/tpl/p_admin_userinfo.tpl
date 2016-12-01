
<!DOCTYPE html>
<html>
    <meta http-equiv="content-type" content="text/html;charset=UTF-8">
    <link rel="stylesheet" href="/static/bootstrap-web/css/bootstrap.min.css">
    <script src="/static/js/jquery-1.7.2.min.js"></script>
    <script src="/static/bootstrap-web/js/bootstrap.min.js"></script>
   
    {{template "../tpl/p_all_css.tpl"}}
<body>


<div id="navi">
    <div id='naviDiv'>
    
        <span><img src="/static/img/arror.gif" width="7" height="11" border="0" alt=""></span>&nbsp;所有用户信息<span>&nbsp;
      <!--   <span><img src="data:image/gif;base64,R0lGODlhBwALAIAAAP8zAP///yH5BAEAAAEALAAAAAAHAAsAAAIPjI8JGbDonomSPnoilDMVADs=" width="7" height="11" border="0" alt=""></span>&nbsp;个人资料添加<span>&nbsp; -->
    </div>
</div>
<div id="tips">
</div>
<!-- <div id="mainContainer"> -->
<div class="panel panel-info">
  <div class="panel-heading"> <a href="/add"><span class="glyphicon glyphicon-plus">添加用户</span> </a>| <a href="/gwmanage"><span class="glyphicon glyphicon-signal">管理网关</span></a></div>
<table class="table  table-hover">
 <!--    <caption><a href="/add"> 添加用户</a></caption> -->
   <thead>
      <tr>
       
        <th>用户名</th>
        <th>密码</th>
        <th>上线权限</th>
        <th>当前状态</th>
      </tr>
   </thead>
   <tbody>
      
   
      {{ range .InfoMsg}}
      {{with .}} 
      <tr>
         <td >{{.User.Username}}</td>
         <td >{{.User.Password}}</td>
         <td >{{.User.Online}} </td>
 
		 <td >{{.UserStatue}}   </td>
          <td><a href="/userinfo?username={{.User.Username}}"><span class="glyphicon glyphicon-move">编辑</span></a> | 
          {{if eq .User.Username "admin"}} 
            <span class="glyphicon glyphicon-remove">删除</span>
          
          {{else}}
     
   <a href="/info?delete={{.User.Username}}">  <span class="glyphicon glyphicon-remove">删除</span></a>
          
          {{end}} 
      </td>

      </tr>
  {{end}}
  {{end}}
   </tbody>
   </table>
</div>
<!-- </div> -->



</div>
</body>
</html>
