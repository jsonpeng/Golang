##  Portus System

基于beego，jquery ui ,bootstrap的管理系统系统
##使用教程

###简介
整个Portus System 本身就是一套基于beego开发得后台系统。


它有自己得模版及样式。目前默认是我自己先做出来的一套，第二套是基于jquery easyi和Bootstrap的模板。



##具体需要实现的功能

1. 用户管理员的登录注册
2. 用户信息的管理
3. 用户家庭网关及节点的管理
4. 用户家庭电网用电情况的监控


####用户管理员的登录注册
 用户用户首次访问会进入到用户主页，如果之前未登录将会显示部分用户主页的信息，并且用户必须正确登录才可成功进入我们的系统

####用户信息的管理 家庭网关及节点的管理（目前只有界面，功能还未实现）

1. 节点管理
2. 用户管理
3. 分组管理
4. 角色管理


####用户管理
用户管理就是指登录后台得用户，默认有一个超级用户admin，这个用户可以操作所有得节点，有最高权限。

新建一个用户，填写资料以后，这个用户可以登录后台，但是无任何操作权限。需要关联角色才有角色全权限。


####节点管理
节点是后台操作得关键。它分3层结构

第一层是package。

第二层是controller。

第三层是method.

它对应了go中得这三个概念。
当然，你也可以自定义你自己得概念，只要url符合权限规范就可以。

需要用户手动添加节点，后期会添加自动生成节点功能。


####角色管理
角色是关联用户与节点得中间桥梁，一个用户可以有多个角色，一个角色也可以有多个节点。

这就能够很精确得分配用户权限，从而更灵活得控制权限安全。


####分组管理
分组管理是为了再项目功能越来越复杂所衍生得一个功能，它再节点上又分出了一层，每个分组下都可以挂靠一部分
业务上相关类型的节点。

但是它不体现再url上，仅仅是逻辑上得分组。


####用户家庭电网用电情况的监控
 生成图表 加载信息让用户一目了然






