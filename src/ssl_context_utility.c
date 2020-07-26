#include<ssl_context_utility.h>

#include<string.h>
#include<alloca.h>

// for sample programs for building SSL applications use https://aticleworld.com/ssl-server-client-using-openssl-in-c/

void load_certificates(SSL_CTX* server_ssl_ctx, char* SSL_PUBLIC_KEY_AND_CERTIFICATE_FILE, char* SSL_PRIVATE_KEY_FILE)
{
    if(SSL_CTX_use_certificate_file(server_ssl_ctx, SSL_PUBLIC_KEY_AND_CERTIFICATE_FILE, SSL_FILETYPE_PEM) <= 0)
    	exit(-1);

	if(SSL_CTX_use_PrivateKey_file(server_ssl_ctx, SSL_PRIVATE_KEY_FILE, SSL_FILETYPE_PEM) <= 0)
    	exit(-1);

	if(!SSL_CTX_check_private_key(server_ssl_ctx))
    	exit(-1);
}

SSL_CTX* create_gbl_server_ssl_ctx(char* pub_cert_priv_key_file)
{
    SSL_load_error_strings();
	SSL_library_init();

    SSL_CTX* server_ssl_ctx = NULL;

    if(!(server_ssl_ctx = SSL_CTX_new(SSL_SERVER_METHOD())))
    	exit(-1);

    SSL_CTX_set_options(server_ssl_ctx, SSL_OP_CIPHER_SERVER_PREFERENCE);

    char* public_key_and_certificate_file = alloca(strlen(pub_cert_priv_key_file) + 5);
    strcpy(public_key_and_certificate_file, pub_cert_priv_key_file);
    strcat(public_key_and_certificate_file, ".crt");

    char* private_key_file = alloca(strlen(pub_cert_priv_key_file) + 5);
    strcpy(private_key_file, pub_cert_priv_key_file);
    strcat(private_key_file, ".key");

    load_certificates(server_ssl_ctx, public_key_and_certificate_file, private_key_file);

    return server_ssl_ctx;
}

void destroy_gbl_server_ssl_ctx(SSL_CTX* server_ssl_ctx)
{
	SSL_CTX_free(server_ssl_ctx);
}