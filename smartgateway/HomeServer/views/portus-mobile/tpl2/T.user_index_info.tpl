            
                <!-- BEGIN TOP NAVIGATION MENU -->                  
              <ul class="nav pull-right"> 
                  

        

                    <!-- BEGIN USER LOGIN DROPDOWN -->
                 <li class="dropdown user">
                        <a href="#" class="dropdown-toggle" data-toggle="dropdown">
                       <i class="icon-user"></i>
                        <span class="username">{{$uname := .username}}{{$uname}}</span>
                        <i class="icon-angle-down"></i>
                        </a>
                        <ul class="dropdown-menu">
                                 {{if .IsAdmin}}
     <li><a href="/userinfo?username={{$uname}}"   target="iframepage" ><span class="icon-user"></span>[["INFO"|translate]]</a></li>
      <li><a href="/info" target="iframepage"><span class="icon-calendar"></span>[["MANAGEINFO"|translate]]</a></li>
      <li class="divider"></li>
  <!--     /login?exit={{$uname}} -->
      <li><a href="/logout" ><span class="icon-key"></span>[["EXIT"|translate]]</a></li>
      {{else}}
      <li><a href="/userinfo?username={{$uname}}" target="iframepage"><span class="icon-user"></span>[["INFO"|translate]]</a></li>
      <li class="divider"></li>
      <li><a href="/logout" ><span class="icon-key"></span>[["EXIT"|translate]]</a></li>
      {{end}}
                          
                        </ul>
                    </li> 
                    <!-- END USER LOGIN DROPDOWN -->
                </ul>

                            </div>
        </div>
        <!-- END TOP NAVIGATION BAR -->
    </div>
    <!-- END HEADER