This project is for a c server side framework for rest api, and static webpages.
attempt to a project similar to apache httpd or tomcat

basic description

src for .c c source files
inc for .h header files
obj for .o object files
bin for .out project run file
con for .con configuration file
shl for .sh build scripts that you may require
pyt for .py python helper scripts
log for .lg log text files

-> It is a frame work that helps host rest api service, and static files over HTTP/TCP/IP protocol

Enables you to
1. attach your your function to a path and httpmethod combination, as specified in the routing json file, This file is picked up from .con folder
2. add files to serc_root to host them as static content

All the framework logic is 
	/src/bootstrapfiles/*.c are the source files
	/inc/bootstrapfiles/*.h are their corresponding headers
	/pyt/*.py are the several python sources which help you with building the switch case logic for your routings from the routing.con files
	/pyt/*.temp are the several templates for the switch case routing logic, source and header files
	/