#ifndef DISTRIBUTER_H
#define DISTRIBUTER_H

#include<string.h>

#include<http_request.h>
#include<http_response.h>

#include<strhsh.h>

#include<file_request_resolution_controller.h>

// return 0, if same connection is usable
// return 1, to close the connection
int distribute(http_request* hrq, stream* strm, file_cache* server_file_cache);

#endif