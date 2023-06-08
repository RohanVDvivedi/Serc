#ifndef HTTP_HTTPS_CONNECTION_HANDLER_H
#define HTTP_HTTPS_CONNECTION_HANDLER_H

#include<stream.h>

void http_connection_stream_handler(stream* strm, void* server_specific_params);

#endif