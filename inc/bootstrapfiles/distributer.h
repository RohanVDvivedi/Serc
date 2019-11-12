#ifndef DISTRIBUTER_H

#define DISTRIBUTER_H

#include<string.h>

#include<httpobject.h>
#include<strhsh.h>

#include<controller.h>
#include<file_request_resolution_controller.h>

void distribute(HttpRequest* hrq,HttpResponse* hrp);

#endif