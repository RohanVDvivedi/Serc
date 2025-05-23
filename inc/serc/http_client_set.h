#ifndef HTTP_CLIENT_SET_H
#define HTTP_CLIENT_SET_H

#include<connman/client_set.h>

// below function creates a client set for a http client

// uri must contain "http://" or "https://" a host name and optionally a port
client_set* new_http_s_client_set(const dstring* uri_dstr, SSL_CTX* ssl_ctx, uint64_t timeout_in_milliseconds, unsigned int max_clients);

void delete_http_s_client_set(client_set* cls);

#endif