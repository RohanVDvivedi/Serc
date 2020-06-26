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

### serc enables you to
1. Route to your function for a http request for given http path and http method combination, as it is specified in the routing json file (in config directory).
2. Host static content from SERC_ROOT_PATH, The SERC_ROOT_PATH can be set in config.h header in ./con folder.

### The framework logic is 
* /src/*.c are the source files
* /inc/*.h are their corresponding headers
* /con/*.h are the configuration header file used by the server
* /con/*.con are config files used for routing (.json files) or for internal server configuration
* /pyt/*.py are the several python sources which help you with building the switch case logic for your routings from the routing.con files
* /pyt/*.temp are the several templates for building the switch case routing logic (source and header files)(controller.h and distributer.c files)

#### Template application
* Template application as an illustration to use serc framework can be found [here](https://github.com/RohanVDvivedi/serc_template_application)

#### Dependencies
##### Certain libraries must be installed before you can start using serc
* cutlery
* BoomPar
* ConnMan
* ReaderWriterLock
* JsonParser
* zlib		($ sudo apt install zlib1g-dev)
* openssl	($ sudo apt-get install libssl-dev)

### Third party acknowledgements
gzip and deflate compression, internally supported by [zlib](https://github.com/madler/zlib) checkout their website [here](https://zlib.net/).
SSL and TLS encryption for https, internally supported by [openssl](https://github.com/openssl/openssl) checkout their website [here](https://www.openssl.org/).