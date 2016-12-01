
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
					<a href="">
					<i class="icon-home"></i> 
					<span class="title">{{$uname := .username}}{{$uname}} [["HOMEPAGE"|translate]]</span>
					<span class="selected"></span>
					</a>
				</li>
	
					   {{range .Gateway}}   
                          {{with .}}
<li {{if eq .Id .Id}} {{else}} class="active" {{end}}>
					<a target="iframepage" href="/gateway?gwid={{.Id}}">
					<i class="icon-map-marker"></i> 
					<span class="title">{{.Address}}</span>
					
						</a>	</li>
						{{end}}
						{{end}}
				
			
				  <li class="">
					<a href="/logout">
					<i class="icon-user"></i> 
					<span class="title">[["EXIT"|translate]]</span>
					</a>
				</li>