This is a c server side framework for rest api, and can also be used for serving static webpages.

basic description

src for .c c source files
inc for .h header files
obj for .o object files
bin for .out project run file
con for .con configuration file
shl for .sh build scripts that you may require
pyt for .py python helper scripts
log for .lg log text files

-> It is a frame work that helps host rest apis, and static files over HTTP/TCP/IP protocol

Enables you to
1. attach your your function to a http path and http method combination, as specified in the routing json file, This file is picked up from ./con folder
2. add files to SERC_ROOT_PATH to host them as static content, to set SERC_ROOT_PATH use config.h header in ./con folder

All the framework logic is 
	/src/*.c are the source files
	/inc/*.h are their corresponding headers
	/con/*.h are the configuration header file used by the server
	/con/*.con are config files used for routing
	/pyt/*.py are the several python sources which help you with building the switch case logic for your routings from the routing.con files
	/pyt/*.temp are the several templates for building the switch case routing logic (source and header files)(controller.h and distributer.c files)