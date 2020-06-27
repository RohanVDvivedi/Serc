#include<ssl_globals.h>

SSL_CTX* gbl_server_ssl_ctx = NULL;

void init_gbl_server_ssl_ctx()
{
	SSL_library_init();
    OpenSSL_add_all_algorithms();
    SSL_load_error_strings();

    if(!(gbl_server_ssl_ctx = SSL_CTX_new(SSL_SERVER_METHOD())))
    	exit(-1);

    SSL_CTX_set_options(gbl_server_ssl_ctx, SSL_OP_CIPHER_SERVER_PREFERENCE);
}

void deinit_gbl_server_ssl_ctx()
{
	SSL_CTX_free(gbl_server_ssl_ctx);
}