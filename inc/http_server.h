#ifndef SERVE_H
#define SERVE_H

#include<server.h>

#include<ssl_globals.h>
#include<http_https_connection_handler.h>

// if OVER_SSL = 1, then your network stacck will be HTTP/SSL/TCP/IP
// else your network stack will be HTTP/TCP/IP
void http_server_run(uint16_t PORT, int OVER_SSL);

#endif