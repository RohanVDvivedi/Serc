#include<ssl_globals.h>

SSL_CTX* ssl_context = NULL;

void initialize_ssl_context()
{
	SSL_library_init();
    OpenSSL_add_all_algorithms();
    SSL_load_error_strings();

    if(!(ssl_context = SSL_CTX_new(TLSv1_2_client_method())))
    	exit(-1);
}

void destroy_ssl_context()
{
	SSL_CTX_free(ssl_context);
}