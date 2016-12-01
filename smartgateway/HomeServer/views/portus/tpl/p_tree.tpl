

<!DOCTYPE html>
<html>
<head>
	<title>网关主页</title>
	<meta http-equiv="content-type" content="text/html;charset=UTF-8">
	<style type="text/css">@import url('/static/components/dtree/dtree.css');</style>
	<script type="text/javascript">var dtreeIconBasePath = "/static/components/dtree";</script>
	<script type="text/javascript"  src="/static/components/dtree/dtree.js"></script>
</head>
<body>
<script type="text/javascript"> 
var treeMenu = [

    {{if .NotAdd}}
    { level:1,ico:"/static/img/loadgateway3.gif"},
    { level:1,name:" ",ico:"/static/img/arror.gif"},
    {{else if .IsAdded}}
     { level:1,name:" ",ico:"/static/img/icon_cart_prompt.jpg"},
    {{else }}
     { level:1,name:" ",ico:"/static/img/ok.gif"},
    {{end}}
   {{if .IsAdd}}
   {{ range .Gateway}}   
   {{with .}}
   { level:1,name:"{{.Address}}", ico:"/static/img/gateway.png",link:"/gateway?gwid={{.Id}}"},
	{{end}}
	{{end}}
	{{else if .IsAdded}}
	{ level:1,name:" ",ico:"/static/img/gateway.png",link:"/gateway"},
    { level:1,name:":{{.mygateway}}",ico:"/static/img/ip2.png",link:"/gateway"},
	// { level:2, name:"节点列表", ico:"/static/img/icon_default.gif",link:"/gateway"},
	{ level:1, name:" ", ico:"/static/img/gateway.png",link:"/backpackgateway"},
    { level:1,name:":{{.mygateway}}",ico:"/static/img/ip2.png",link:"/backpackgateway"},
	{{else}}
	 { level:1, name:" ", ico:"/static/img/gateway.png",link:"/gateway"},
     { level:1,name:":{{.mygateway}}",ico:"/static/img/ip2.png",link:"/gateway"},
      { level:1, name:" ", ico:"/static/img/gateway.png",link:"/gateway"},
     { level:1,name:":192.168.1.1",ico:"/static/img/ip2.png",link:"/gateway"},
     
	{{end}}


];
</script>
<style>
* {
    background: none repeat scroll 0 0 transparent;
    border: 0 none;
    margin: 0;
    padding: 0;
    vertical-align: baseline;
	font-family:微软雅黑;
	overflow:hidden;
}
#menuControll{
	width:100%;
	position:relative;
	word-wrap:break-word;
	border-bottom:1px solid #065FB9;
	margin:0;
	padding:0 10px;
	font-size:14px;
	height:40px;
	line-height:40px;
	vertical-align:middle;
    background-image: -moz-linear-gradient(top,#EBEBEB, #BFBFBF);
    background-image: -webkit-gradient(linear, left top, left bottom, color-stop(0, #EBEBEB),color-stop(1, 
#BFBFBF));
}
</style>
<div id="menuControll">
网关控制:【<a href="#" onclick="tree.openAll();this.blur();return false;" style="color:#333333;text-decoration:none">展开</a>】
【<a href="#" onclick="tree.closeAll();this.blur();return false;" style="color:#333333;text-decoration:none">折叠</a>】
</div>
<div class="dtree" style="margin:10px;">
<script type="text/javascript"> 
//建立新树
tree = new dTree('tree');
tree.config.target = "MainFrame";
tree.config.useCookies = false;
var selNum = -1;
var link = "";
//根目录
tree.add(0,-1,'网关操作', null, null, null, '', '');
var count = 0;
var pLevelIdArray = new Array();
pLevelIdArray[1] = 0;
var currLevel = 1;
for (var i=0; i<treeMenu.length; i++) {
	var item = treeMenu[i];
	var itemLevel = item.level;
	pLevelIdArray[itemLevel+1] = ++count;
	if (item.link!=null && item.link!="") {
		if (item.ico!=null) {
			tree.add(count, pLevelIdArray[itemLevel], item.name, item.link, null, null, item.ico, item.ico);
		} else {
			tree.add(count, pLevelIdArray[itemLevel], item.name, item.link);
		}
	} else {
		if (item.ico!=null) {
			tree.add(count, pLevelIdArray[itemLevel], item.name, null, null, null, item.ico, item.ico);
		} else {
			tree.add(count, pLevelIdArray[itemLevel], item.name);
		}
	}
	if (item.select) {
		selNum = count;
		link = item.link;
	}
}
document.write(tree);
tree.openAll();
if (selNum != -1) {
	tree.openTo(selNum,true);
	top.document.frames["MainFrame"].location.href=link;
}
</script>
</div>
</body>
</html>