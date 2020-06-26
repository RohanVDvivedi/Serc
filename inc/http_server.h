#ifndef SERVE_H
#define SERVE_H

#include<server.h>

// if you want to start multiple servers, in the same application
// you must start all of them in the same thread
void http_server_run(uint16_t PORT);
void https_server_run(uint16_t PORT, char* ssl_cert_path);

#endif