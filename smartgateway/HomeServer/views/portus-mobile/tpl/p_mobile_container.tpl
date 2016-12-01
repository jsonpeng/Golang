
  <!-- BEGIN CONTAINER -->
  <div class="page-container row-fluid">
    <!-- BEGIN SIDEBAR -->
    <div class="page-sidebar nav-collapse collapse">
      <!-- BEGIN SIDEBAR MENU -->         
      <ul>
        <li>
          <!-- BEGIN SIDEBAR TOGGLER BUTTON -->
          <div class="sidebar-toggler hidden-phone"></div>
          <!-- BEGIN SIDEBAR TOGGLER BUTTON -->
        </li>
        <li>
          <!-- BEGIN RESPONSIVE QUICK SEARCH FORM -->
          <form class="sidebar-search">
            <div class="input-box">
              <a href="javascript:;" class="remove"></a>
              <input type="text" placeholder="Search..." />       
              <input type="button" class="submit" value=" " />
            </div>
          </form>
          <!-- END RESPONSIVE QUICK SEARCH FORM -->
        </li>
        <li {{if .IsHome}} class="active" {{end}}>
          <a href="/index">
          <i class="icon-home"></i> 
          <span class="title">主页</span>
          <span class="selected"></span>
          </a>
        </li>
  <li {{if .IsGateway}} class="active" {{end}}>
          <a href="/gateway">
          <i class="icon-map-marker"></i> 
          <span class="title">网关控制</span>
          </a>
        </li>
          <li class="">
          <a href="/user/login">
          <i class="icon-user"></i> 
          <span class="title">Exit</span>
          </a>
        </li>