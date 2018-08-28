#ifndef SERVE_H
#define SERVE_H

#include<string.h>
#include<sys/socket.h>
#include<httpobject.h>
#include<distributer.h>
#include<logger.h>

void serve(int fd);

#endif