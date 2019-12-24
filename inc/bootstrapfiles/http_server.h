#ifndef SERVE_H
#define SERVE_H

#include<stdio.h>
#include<stdlib.h>

#include<dstring.h>
#include<server.h>

#include<string.h>
#include<http_object.h>
#include<distributer.h>

#define buffersize 100

void http_server_run(uint16_t PORT);

#endif