#ifndef DISTRIBUTER_H
#define DISTRIBUTER_H

#include<httpparser/http_request.h>

#include<serc/server_global_params.h>

// return 0, if same connection is usable
// return 1, to close the connection
int distribute(const http_request_head* hrq, stream* strm, server_global_params* sgp);

#endif