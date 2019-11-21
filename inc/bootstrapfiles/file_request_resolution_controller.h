#ifndef FILE_REQUEST_RESOLUTION_CONTROLLER_H
#define FILE_REQUEST_RESOLUTION_CONTROLLER_H

#include<stdio.h>
#include<unistd.h>

#include<hashmap.h>
#include<dstring.h>

#include<httpobject.h>
#include<strhsh.h>

#define SERC_ROOT_PATH "./serc_root"

int file_request_controller(HttpRequest* hrq, HttpResponse* hrp, int* routing_resolved);

#endif