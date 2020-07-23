#include<ssl_globals.h>

// for sample programs for building SSL applications use https://aticleworld.com/ssl-server-client-using-openssl-in-c/

void load_certificates(SSL_CTX* server_ssl_ctx)
{
    if(SSL_CTX_use_certificate_file(server_ssl_ctx, SSL_PUB_CERTIFICATE, SSL_FILETYPE_PEM) <= 0)
    	exit(-1);

	if(SSL_CTX_use_PrivateKey_file(server_ssl_ctx, SSL_PRIVATE_KEY, SSL_FILETYPE_PEM) <= 0)
    	exit(-1);

	if(!SSL_CTX_check_private_key(server_ssl_ctx))
    	exit(-1);
}

SSL_CTX* create_gbl_server_ssl_ctx()
{
	SSL_library_init();
    OpenSSL_add_all_algorithms();
    SSL_load_error_strings();

    SSL_CTX* server_ssl_ctx = NULL

    if(!(server_ssl_ctx = SSL_CTX_new(SSL_SERVER_METHOD())))
    	exit(-1);

    SSL_CTX_set_options(server_ssl_ctx, SSL_OP_CIPHER_SERVER_PREFERENCE);

    load_certificates(server_ssl_ctx);

    return server_ssl_ctx;
}

void destroy_gbl_server_ssl_ctx(SSL_CTX* server_ssl_ctx)
{
	SSL_CTX_free(server_ssl_ctx);
}