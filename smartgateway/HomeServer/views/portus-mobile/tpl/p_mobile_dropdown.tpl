<!-- END RESPONSIVE MENU TOGGLER -->                
                <!-- BEGIN TOP NAVIGATION MENU -->            
         <ul class="nav pull-right">
            <li class="dropdown user">
              <a href="#" class="dropdown-toggle" data-toggle="dropdown" role="button" aria-haspopup="true" aria-expanded="false"><span class="glyphicon glyphicon-user"></span> {{$uname := .username}}{{$uname}}</a>
              <ul class="dropdown-menu">
                {{if .IsAdmin}}
     <li><a href="/userinfo?username={{$uname}}"   target="MainFrame" ><span class="glyphicon glyphicon-calendar"></span>个人信息</a></li>
      <li><a href="/info"  target="MainFrame"><span class="glyphicon glyphicon-lock"></span>管理用户信息</a></li>
      <li class="divider"></li>
      <li><a href="/login?exit={{$uname}}"><span class="glyphicon glyphicon-log-in"></span>退出</a></li>
      {{else}}
      <li><a href="/userinfo?username={{$uname}}" target="MainFrame"><span class="glyphicon glyphicon-calendar"></span>个人信息</a></li>
      <li class="divider"></li>
      <li><a href="/login?exit={{$uname}}"><span class="glyphicon glyphicon-log-in"></span>退出</a></li>
      {{end}}
              </ul>
            </li>
          </ul>
                    <!-- END USER LOGIN DROPDOWN -->
           
                 </div>
</div>
</div>
<!-- END HEADER --> 