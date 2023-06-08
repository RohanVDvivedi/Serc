#ifndef SERVE_H
#define SERVE_H

#include<stdint.h>

// if SSL_KEYS_CERTS != NULL, then your network stacck will be SSL/HTTP/TCP/IP
// else your network stack will be HTTP/TCP/IP
void http_server_run(uint16_t PORT, char* ROOT_PATH, char* SSL_KEYS_CERTS);

// if ROOT_PATH == NULL => the server would not serve any static files
// if SSL_KEYS_CERTS == NULL => the server would serve content on http, instead of https

#endif