#ifndef SSL_GLOBALS_H
#define SSL_GLOBALS_H

#include<openssl/ssl.h>

#define SSL_SERVER_METHOD TLSv1_2_server_method
#define SSL_CLIENT_METHOD TLSv1_2_client_method

SSL_CTX* create_gbl_server_ssl_ctx(char* pub_cert_priv_key_file);
void destroy_gbl_server_ssl_ctx(SSL_CTX* server_ssl_ctx);

#endif