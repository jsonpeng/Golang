
<!DOCTYPE html>
<html>
    <meta http-equiv="content-type" content="text/html;charset=UTF-8">
    <link rel="stylesheet" href="/static/bootstrap-web/css/bootstrap.min.css">
    <script src="/static/js/jquery-1.7.2.min.js"></script>
    <script src="/static/bootstrap-web/js/bootstrap.min.js"></script>
   
  
<body>


<!-- <div id="mainContainer"> -->
<div class="panel panel-info">
  <div class="panel-heading"> <a href="/add"><span class="glyphicon glyphicon-plus">添加用户</span> </a>| <a href="/gwmanage"><span class="glyphicon glyphicon-signal">管理网关</span></a></div>
<table class="table  table-hover">
 <!--    <caption><a href="/add"> 添加用户</a></caption> -->
   <thead>
      <tr>
        <th>用户名</th>
        <th>密码</th>
        <th>上线状态</th>
        <th>语言</th>
        <th>操作</th>
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
