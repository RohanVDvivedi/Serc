#ifndef FILE_REQUEST_RESOLUTION_CONTROLLER_H
#define FILE_REQUEST_RESOLUTION_CONTROLLER_H

#include<http_request.h>

#include<server_global_params.h>

int file_request_controller(http_request_head* hrq, stream* strm, server_global_params* sgp);

#endif