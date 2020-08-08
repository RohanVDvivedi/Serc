#ifndef FILE_REQUEST_RESOLUTION_CONTROLLER_H
#define FILE_REQUEST_RESOLUTION_CONTROLLER_H

#include<hashmap.h>
#include<dstring.h>

#include<file_handler.h>

#include<http_request.h>
#include<http_response.h>

int file_request_controller(HttpRequest* hrq, HttpResponse* hrp, file_cache* server_file_cache, int* routing_resolved);

#endif