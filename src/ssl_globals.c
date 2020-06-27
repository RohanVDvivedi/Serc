#include<ssl_globals.h>

SSL_CTX* gbl_server_ssl_ctx = NULL;

// for sample programs for building SSL applications use https://aticleworld.com/ssl-server-client-using-openssl-in-c/

void load_certificates()
{
    if(SSL_CTX_use_certificate_file(gbl_server_ssl_ctx, SSL_CERTIFICATION_AND_KEYS, SSL_FILETYPE_PEM) <= 0)
    	exit(-1);

	if(SSL_CTX_use_PrivateKey_file(gbl_server_ssl_ctx, SSL_CERTIFICATION_AND_KEYS, SSL_FILETYPE_PEM) <= 0)
    	exit(-1);

	if(!SSL_CTX_check_private_key(gbl_server_ssl_ctx))
    	exit(-1);
}

void init_gbl_server_ssl_ctx()
{
	SSL_library_init();
    OpenSSL_add_all_algorithms();
    SSL_load_error_strings();

    if(!(gbl_server_ssl_ctx = SSL_CTX_new(SSL_SERVER_METHOD())))
    	exit(-1);

    SSL_CTX_set_options(gbl_server_ssl_ctx, SSL_OP_CIPHER_SERVER_PREFERENCE);

    load_certificates();
}

void deinit_gbl_server_ssl_ctx()
{
	SSL_CTX_free(gbl_server_ssl_ctx);
}