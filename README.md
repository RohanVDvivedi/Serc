# serc

### serc is a http server / c framework for building apis in c
### It can also be used for serving static webpages

### Project Directory Description

* src for .c c source files
* inc for .h header files
* obj for .o intermediate object files
* bin for .out or .lib binaries
* con for .con or .h configuration files
* pyt for .py python helper scripts

Enables you to
1. Route to your function for a http request for given http path and http method combination, as it is specified in the routing json file (in config directory).
2. To host static content from SERC_ROOT_PATH, The SERC_ROOT_PATH can be set in config.h header in ./con folder.

### The framework logic is 
* /src/*.c are the source files
* /inc/*.h are their corresponding headers
* /con/*.h are the configuration header file used by the server
* /con/*.con are config files used for routing (.json files) or for internal server configuration
* /pyt/*.py are the several python sources which help you with building the switch case logic for your routings from the routing.con files
* /pyt/*.temp are the several templates for building the switch case routing logic (source and header files)(controller.h and distributer.c files)


serc provides you with gzip and deflate compression, internally supported by [zlib](https://github.com/madler/zlib).
