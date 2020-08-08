#ifndef DISTRIBUTER_H

#define DISTRIBUTER_H

#include<string.h>

#include<http_request.h>
#include<http_response.h>

#include<strhsh.h>

#include<file_request_resolution_controller.h>

void distribute(HttpRequest* hrq, HttpResponse* hrp, file_cache* server_file_cache);

#endif