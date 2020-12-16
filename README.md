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
 1. Route to your function for a http request for given http path and http method combination.
 2. Host static content from all the files in the root folder.

### The framework logic is segregated as
* /src/*.c are the source files
* /inc/*.h are their corresponding headers
* /con/*.h are the configuration header file used by the server
* /con/*.con are config files used for routing (.json files) or for internal server configuration
* /pyt/*.py are the several python sources which help you with building the switch case logic for your routings from the routing.con files
* /pyt/*.temp are the several templates for building the switch case routing logic (source and header files)(controller.h and distributer.c files)

#### Template application
* Template application as an illustration to use Serc framework can be found [here](https://github.com/RohanVDvivedi/serc_template_application)

## Setup instructions
**Install dependencies :**
 * [Cutlery](https://github.com/RohanVDvivedi/Cutlery)
 * [BoomPar](https://github.com/RohanVDvivedi/BoomPar)
 * [ConnMan](https://github.com/RohanVDvivedi/ConnMan)
 * [ReaderWriterLock](https://github.com/RohanVDvivedi/ReaderWriterLock)
 * [JSONParser](https://github.com/RohanVDvivedi/JSONparser)
 * [Cashed](https://github.com/RohanVDvivedi/Cashed)
 * [zlib](https://github.com/madler/zlib)		  ($ sudo apt install zlib1g-dev)
 * [openssl](https://github.com/openssl/openssl)	($ sudo apt install libssl-dev)

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
 * create appropriate routing.con file (as described in the sample file in the con directory).
 * and create appropriate controller files with functions with strict declaration signature as : 
   * ```int controller(HttpRequest* hrq, HttpResponse* hrp)```
 * Create appropriate json entries, in your routing.con file, to specify routing information.
 * do not forget to include appropriate public api headers as and when needed by your source, which includes
   * `#include<http_request.h>`
   * `#include<http_response.h>`
   * `#include<http_server.h>`
   * `#include<http_client.h>`
 * Now the routing files need to be compiled to produce a distributer function (*a huge switch case*) in a distributer.c file, which will help the server to know how to route the request to your comroller.
   * route.py <routing_file_1.con> <routing_file_2.con> <routing_file_3.con> ...
 * above step will create distributer.c file in your `pwd`
 * Now, you must compile your source with this distributer.c file, simultaneously linking with serc library.
 * **More info**
   * The distribute() function, has a quite similar function signature, to the signature required by the controller functions.
   * The distribute function is already present, and is called, internally in the source code of the library, but there is only declaration and no definition to that function in the library.
   * The route.py script reads your routing configuration file and generates a distributer function required to route to your controller functions, using a huge switch case statement.
   * And so you are required to compile this distributer functions, while linking with serc library, to produce appropriate binary of your application.
 * don't forget to add "-lserc -lcashed -lm -lconnman -lrwlock -lboompar -lpthread -ljsonpar -lcutlery -lz -lssl -lcrypto" linker flag, while compiling your application

## Third party acknowledgements
 * *gzip and deflate compression, internally supported by [zlib](https://github.com/madler/zlib) checkout their website [here](https://zlib.net/).*
 * *SSL and TLS encryption for https, internally supported by [openssl](https://github.com/openssl/openssl) checkout their website [here](https://www.openssl.org/).*
