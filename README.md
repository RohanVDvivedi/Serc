# Serc
 Serc is a http server / c framework for building apis in c. It can also be used for serving static webpages.

### Project Directory Description

 * src for .c c source files
 * inc for .h header files
 * obj for .o intermediate object files
 * lib for .a or .so libraries
 * bin for .out binaries
 * con for .con or .h configuration files
 * pyt for .py python helper scripts
 * share for .temp template file ***that python scripts like route.py will use***

### Serc enables you to
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
* Template application as an illustration to use Serc framework can be found [here](https://github.com/RohanVDvivedi/serc_template_application)

#### Dependencies
##### Certain libraries must be installed before you can start using Serc
* Cutlery
* BoomPar
* ConnMan
* ReaderWriterLock
* JsonParser
* Cashed
* zlib		($ sudo apt install zlib1g-dev)
* openssl	($ sudo apt-get install libssl-dev)

## Setup instructions
*considering that you have installed all the required dependencies mentioned above*
**Download source code :**
 * `git clone https://github.com/RohanVDvivedi/Serc.git`

**Build from source :**
 * `cd Serc`
 * `make clean all`

**Install from the build :**
 * `sudo make install` : This will install the library aswell as the static file server.
 * ***Once you have installed from source, you may discard the build by*** `make clean`

## Instructions for uninstalling library (and the static file server)
**Uninstall :**
 * `cd Serc`
 * `sudo make uninstall`

## Usage instructions
#### usage as (static file) server
 * `serc` or `serc --root=.`	=> this will start a http server hosting all files in your `pwd`
 * `serc --root=./public --port=8080` => this will start a http server on port 8080 to host all your files in ./public directory
 * `serc --port=6900 --root=./public --ssl_cert_keys=./con/server`	=> this will start a https server on port 6900 to host all your files in ./public directory, the public key and certificates will be "./con/server.crt" and  private key will be "./con/server.key" 

#### usage as framework
 * create appropriate routing.con file (as described in con directory)
 * and create appropriate controller file functions as mentioned in routing.con file, with strict declaration signature as : 
   * ```int controller(HttpRequest* hrq, HttpResponse* hrp)```
 * do not forget to include appropriate public api headers as and when needed. this includes
   * `#include<http_request.h>`
   * `#include<http_response.h>`
   * `#include<http_server.h>`
   * `#include<http_client.h>`
 * the routing files need to be compiled to produce a distributer function (*a huge switch case*), which will help the server to know how to route the request to your comroller.
   * route.py <routing_file_1> <routing_file_2> <routing_file_3> ...
 * above step will create distributer.c file in your `pwd`
 * you can you compile your source with this distributer.c
 * don't forget to add "-lserc -lcashed -lm -lconnman -lrwlock -lboompar -lpthread -ljsonpar -lcutlery -lz -lssl -lcrypto" linker flag, while compiling your application

## Third party acknowledgements
 * *gzip and deflate compression, internally supported by [zlib](https://github.com/madler/zlib) checkout their website [here](https://zlib.net/).*
 * *SSL and TLS encryption for https, internally supported by [openssl](https://github.com/openssl/openssl) checkout their website [here](https://www.openssl.org/).*
