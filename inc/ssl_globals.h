#ifndef SSL_GLOBALS_H
#define SSL_GLOBALS_H

#include<openssl/ssl.h>
#include<openssl/err.h>

#include<config.h>

extern SSL_CTX* gbl_server_ssl_ctx;

void init_gbl_server_ssl_ctx();
void deinit_gbl_server_ssl_ctx();

#endif