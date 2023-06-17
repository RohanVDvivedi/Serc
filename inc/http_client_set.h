#ifndef HTTP_CLIENT_SET_H
#define HTTP_CLIENT_SET_H

#include<client_set.h>

// below function creates a client set for a http client

// if port == NULL, then port = "80" when ssl_ctx = NULL, and port = "443" when ssl_ctx != NULL
client_set* new_http_s_client_set(const char* hostname, const char* port, SSL_CTX* ssl_ctx, unsigned int max_clients);

#endif