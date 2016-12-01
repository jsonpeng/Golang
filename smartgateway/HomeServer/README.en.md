# # Portus System


Based on beego, jquery UI, the bootstrap system of management system
Use # # tutorial


Introduction of # # #
The whole Portus System based on beego development itself is a set of backend systems.




It has its own template and the style.The current default is my own to make a first, second is based on the jquery easyi and Bootstrap template.






# # need to implement specific functions


1. The registered user administrator login
2. The management of user information
3. User management of family gateway and the nodes
4. User monitoring of family power grid electricity supply




# # # # registered user administrator login
User users first visit will be to enter home page, if not before login part will show the user's home page information, and the user must be correct login successfully into our system


# # # # for the management of user information family gateway and the nodes (currently only interface, the function is not yet implemented)


1. The node management
2. User management
3. The group management
4. The role of management




# # # # user management
Login user management is refers to the background to the user, the default is a super admin user, the user can operate on all nodes, has the highest authority.


Create a new user, fill in the information later, the user can log in the background, but without any operation permissions.Need to associate roles have a character all permissions.




# # # # node management
Key nodes is a background operation.It is three layer structure


The first layer is a package.


The second layer is the controller.


The third layer is a method.


It corresponds to the go in these three concepts.
Of course, you can also customize your own concept, as long as the url can conform to the permissions norms.


Requires the user to manually add node, the late add automatically generated node function.




# # # # role management
Role is associated with the user and the node to middle bridge, a user can have multiple roles, a character also can have multiple nodes.


This can be very precise assign user rights, thus more flexible to control access security.




# # # # grouping management
Group management is to project in order to function more and more complex derivative of a function, it nodes and gives off a layer, each group can be under the affiliated part
Business related types of nodes.


But it doesn't reflect on the url again, is simply the logical grouping.




# # # # users family grid electricity condition monitoring
To generate the chart Loading information allows users to be clear at a glance