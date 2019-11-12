#ifndef FILE_REQUEST_RESOLUTION_CONTROLLER_H
#define FILE_REQUEST_RESOLUTION_CONTROLLER_H

#include<stdio.h>
#include<unistd.h>

#include<httpobject.h>

#define SERC_ROOT_PATH "./serc_root"

int file_request_controller(HttpRequest* hrq, HttpResponse* hrp, int* routing_resolved);

#endif