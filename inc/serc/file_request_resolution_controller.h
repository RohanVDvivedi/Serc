#ifndef FILE_REQUEST_RESOLUTION_CONTROLLER_H
#define FILE_REQUEST_RESOLUTION_CONTROLLER_H

#include<httpparser/http_request.h>

#include<serc/server_global_params.h>

int file_request_controller(const http_request_head* hrq, stream* strm, server_global_params* sgp, int* routing_resolved);

#endif