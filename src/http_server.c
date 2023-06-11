#include<http_server.h>

#include<server_global_params.h>
#include<http_connection_handler.h>

#include<server.h>

#include<ssl_ctx_helper.h>

#include<signal.h>

volatile int listen_fd = -1;
static void intHandler(int signum)
{
	printf("Caught signal %d\n", signum);
	if(listen_fd != -1)
    	server_stop(listen_fd);
}

static SSL_CTX* create_server_ssl_ctx(char* SSL_KEYS_CERTS)
{
	int len = strlen(SSL_KEYS_CERTS);

	char* cert_file = malloc(len + 10);
	strcpy(cert_file, SSL_KEYS_CERTS);
	strcat(cert_file, ".crt");

	char* key_file = malloc(len + 10);
	strcpy(key_file, SSL_KEYS_CERTS);
	strcat(key_file, ".key");

	SSL_CTX* ssl_ctx = get_ssl_ctx_for_server(cert_file, key_file);

	free(cert_file);
	free(key_file);

	return ssl_ctx;
}

void http_server_run(uint16_t PORT, char* ROOT_PATH, int SERVE_DIRS, char* SSL_KEYS_CERTS, const void* server_param)
{
	// these values will be constant through out all the connections of this specific server
	server_global_params sgp = {0};
	init_dstring(&(sgp.root_path), ROOT_PATH, strlen(ROOT_PATH));
	sgp.serve_dirs = SERVE_DIRS;
	sgp.server_param = server_param;

	// start the server using https connection handler
	comm_address cgp = new_comm_address_tcp_ipv4(NULL, PORT);

	// for HTTPS server, you also need to create appropriate ssl context
	SSL_CTX* ssl_ctx = NULL;
	if(SSL_KEYS_CERTS != NULL)
	{
		ssl_ctx = create_server_ssl_ctx(SSL_KEYS_CERTS);
		if(ssl_ctx == NULL)
			printf("starting server without SSL TLS security\n");
	}

	signal(SIGINT, intHandler);
	serve_using_stream_handlers(&cgp, &sgp, http_connection_stream_handler, 100, ssl_ctx, &listen_fd);

	if(ssl_ctx != NULL)
		destroy_ssl_ctx(ssl_ctx);

	// de init server global params
	deinit_dstring(&(sgp.root_path));
}
