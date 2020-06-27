#ifndef SSL_GLOBALS_H
#define SSL_GLOBALS_H

#include<openssl/ssl.h>
#include<openssl/err.h>

extern SSL_CTX* ssl_context;

void initialize_ssl_context();

void destroy_ssl_context();

#endif