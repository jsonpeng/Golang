<!DOCTYPE html>
<!--[if IE 8]> <html lang="en" class="ie8"> <![endif]-->
<!--[if IE 9]> <html lang="en" class="ie9"> <![endif]-->
<!--[if !IE]><!--> <html lang="en"> <!--<![endif]-->
<!-- BEGIN HEAD -->
<head>
  <meta charset="utf-8" />
  <title>PortusSystem</title>
<!--    <meta content="width=device-width, initial-scale=1.0" name="viewport" /> -->
   <meta content="width=device-width, initial-scale=1.0, user-scalable=no" name="viewport" />
  <meta http-equiv="X-UA-Compatible" content="IE=edge,chrome=1">
    <meta name="HandheldFriendly" content="true">
  <meta content="" name="description" />
  <meta content="" name="author" />
  <!--封装好的字体
    <link rel="stylesheet" type="text/css" href="/static/assets/css/font.css" /> -->
<script src="/static/assets/js/jquery-1.8.3.min.js"></script>
 <link rel="stylesheet" type="text/css" href="/static/css/animate.css">
  <link href="/static/assets/bootstrap/css/bootstrap.min.css" rel="stylesheet" />
  <link href="/static/assets/bootstrap/css/bootstrap.css" rel="stylesheet" />
  <link href="/static/assets/css/metro.css" rel="stylesheet" />
  <link href="/static/assets/font-awesome/css/font-awesome.css" rel="stylesheet" />
  <link href="/static/assets/css/style.css" rel="stylesheet" />
  <link href="/static/assets/css/style_responsive.css" rel="stylesheet" />
  <link href="/static/assets/css/style_default.css" rel="stylesheet" id="style_color" />
  <link rel="stylesheet" type="text/css" href="/static/assets/uniform/css/uniform.default.css" />
 
  <link rel="shortcut icon" href="favicon.ico" />
    <!-- BEGIN JAVASCRIPTS  -->
  
  <script src="/static/assets/bootstrap/js/bootstrap.min.js"></script> 

<script src="/static/assets/js/aj.js"></script> 
<script src="/static/js/aj-route.js"></script>
<script src="/static/layer-2.4/layer.js"></script>
<script src="/static/js/angular-cookies.js"></script>
<script src="/static/js/angular-translate.min.js"></script>
<script src="/static/js/storage-cookie.js"></script>
<script src="/static/js/angular-translate-loader-static-files.js"></script>
<script src="/static/js/aj-animate.js"></script>

<script>
var app = angular.module('portushome', ['pascalprecht.translate','ngCookies','ngRoute','ngAnimate'])
app.config(['$interpolateProvider', function($interpolateProvider) {
  $interpolateProvider.startSymbol('[[');
  $interpolateProvider.endSymbol(']]');
}]);
app.config(function($translateProvider) {  
    $translateProvider.useStaticFilesLoader({  
        prefix: './static/languge/',  
        suffix: '.json'  
    }); 
  //$translateProvider.determinePreferredLanguage();
  $translateProvider.preferredLanguage('en');
  $translateProvider.useCookieStorage();
  
});
  // configure our routes
  app.config(function($routeProvider) {
    $routeProvider

      // route for the home page
      .when('/', {
        templateUrl : '/static/pages/login.html',
        controller  : 'loginController'
      })

      // route for the about page
      .when('/reg', {
        templateUrl : '/static/pages/reg.html',
        controller  : 'regController'
      })
      .when('/getpwd',{
       templateUrl : '/static/pages/find.html',
        controller  : 'findController'

      })

      // route for the contact page
      .when('/forget', {
        templateUrl : '/static/pages/forget.html',
        controller  : 'forgetController'
      });
  });
    // create the controller and inject Angular's $scope
  app.controller('loginController', function($scope) {
      $scope.pageClass = 'page-home';
  });

  app.controller('regController', function($scope) {
      $scope.pageClass = 'page-about';
  });

  app.controller('forgetController', function($scope) {
     $scope.pageClass = 'page-contact';
  });

  app.controller('findController', function($scope) {
   
  });
app.controller('TranslateController', function($translate, $scope) {
  $scope.changeLanguage = function(langKey) {
    $translate.use(langKey);
  }
});
app.controller('registerCtrl',['$scope','$http',function($scope,$http){
     $scope.asave= function(){
          //var data = 'username=admin&password=13456'
         $http({
             method: 'POST',
             url: '/',
             data: $.param($scope.formData), // pass in data as strings
             headers: {'Content-Type':'application/x-www-form-urlencoded; charset=UTF-8'}  
         }).success(function (data) {
           if(data.errno==0x00){
              layer.msg('注册成功', {
               offset: 0,
               shift: 6
});
            location.href="/";
          }else if(data.errno==0x01){
                      layer.msg('注册失败，请重新注册', {
               offset: 0,
               shift: 6
});
           
          }
          });
 
     };
 }]);
 app.controller('login',['$scope','$http',function($scope,$http){
     $scope.asave= function(){
          //var data = 'username=admin&password=13456'
         $http({
             method: 'POST',
             url: '/data/login',
             data: $.param($scope.formData), // pass in data as strings
             headers: {'Content-Type':'application/x-www-form-urlencoded; charset=UTF-8'}  
         }).success(function (data) {
           if(data.error==0x12){
            layer.alert('用户名不存在', {icon: 5});
          }else if(data.error==0x14){
             layer.alert('密码不正确', {icon: 2});
          }else if(data.error==0x17){
             layer.msg('用户已登录', {
               offset: 0,
               shift: 6
});
           }else if(data.error==0x00){  
         ////加载层-风格4
       layer.msg('登录中', {icon: 16});
        //此处演示关闭
        setTimeout(function(){
       layer.closeAll('loading');
            }, 3000);
             window.location.href = "/index?uid="+data.data.uid;
             //eval("obj."+data)
             //alert(data.data.uid);
           }
          });
     };
 }]);
app.controller('forgetpassword',['$scope','$http',function($scope,$http) {
    $scope.asave= function(){
         $http({
             method: 'POST',
             url:'/data/findpwd',
             data: $.param($scope.formData), // pass in data as strings
             headers: {'Content-Type':'application/x-www-form-urlencoded; charset=UTF-8'}  
         }).success(function (data) {
    if(data.error==0x00){
layer.alert('您的密码为'+data.data.password, {icon: 1});
location.href="/#/"
    }else if (data.error==0x10){
         layer.alert('用户名为空', {icon: 5});
    }else if(data.error==0x42){
        layer.alert('无效的gwid', {icon: 2});
  }
});
};
    }]);
// app.controller('getpassword', ['$scope','$http',function($scope,$http) {
//   $scope.asave=function(){
//  $http({
//              method: 'POST',
//              url:'/data/findpwd',
//              data: $.param($scope.formData), // pass in data as strings
//              headers: {'Content-Type':'application/x-www-form-urlencoded; charset=UTF-8'}  
//          }).success(function (data) {
//     if(data.error==0x00){
//      //配置一个透明的询问框
// layer.msg('您的网关为{{.gwid}},对应的密码为{{.pwd}}', {
//   time: 30000, //30s
//   btn: [ '知道了', '谢谢']
// });
//     }else if(data.error==0x42){
//         layer.alert('无效的gwid', {icon: 2});
//   }
// });
// };
//     }]);
</script>
 <!-- END JAVASCRIPTS -->
   <!--ADD aj.js-->

<!-- END HEAD -->
{{if .IsHome}}
<style>
.video-container{
position:relative;
height:0;
padding-top:20px;
padding-bottom:93%;
overflow:hidden;
}
.video-container embed, .video-container iframe{
position:absolute;
top:0;
left:0;
width:100%;
height:100%;
}
.iframe-container{
padding-bottom:56%;
}
</style>
<script src="/static/js/pym.js"></script>
</head>

{{else}}
</head>
<!-- BEGIN BODY -->
<body class="login" ng-app="portushome" ng-controller="TranslateController">
<div class="nav pull-left"  ng-controller="TranslateController">
                    <div class="btn-group">
                        <button type="button" class="btn btn-xs btn-default btn-md dropdown-toggle" data-toggle="dropdown">LANGUGES<i class="caret"></i></button>
                        <ul class="dropdown-menu">
                          <li><a  ng-click="changeLanguage('en')" ng-controller="TranslateController" class="lang-changed"><img src="/static/img/en.png">English</a></li>
                          <li><a   ng-click="changeLanguage('ch')" ng-controller="TranslateController" class="lang-changed"><img src="/static/img/zhcn.png">Chinese</a></li>
                        </ul>
                    </div>
                </div>
  <!-- BEGIN LOGO -->

  <!-- END LOGO -->
  {{end}}