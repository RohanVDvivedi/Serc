#ifndef SERVE_H
#define SERVE_H

#include<stdio.h>
#include<stdlib.h>

#include<dstring.h>

#include<server.h>

#include<http_request.h>
#include<http_response.h>
#include<distributer.h>

#define buffersize 100

// if you want to start multiple servers, in the same application
// you must start all of them in the same thread
void http_server_run(uint16_t PORT);

#endif