#ifndef SSL_GLOBALS_H
#define SSL_GLOBALS_H

#include<openssl/ssl.h>
#include<openssl/err.h>

#include<config.h>

SSL_CTX* create_gbl_server_ssl_ctx();
void destroy_gbl_server_ssl_ctx(SSL_CTX* server_ssl_ctx);

#endif