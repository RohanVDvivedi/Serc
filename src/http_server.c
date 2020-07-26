#include<http_server.h>

#include<signal.h>

volatile int listen_fd = -1;
static void intHandler(int signum)
{
	printf("Caught signal %d\n", signum);
	if(listen_fd != -1)
    	server_stop(listen_fd);
}

void http_server_run(uint16_t PORT, char* ROOT_PATH, char* SSL_KEYS_CERTS)
{
	// these values will be constant through out all the connections of this specific server
	server_global_params sgp = {0};

	// initialize the content cache for serving the files
	sgp.files_cached = ((ROOT_PATH != NULL) ? get_file_content_cache(ROOT_PATH) : NULL);

	// start the server using https connection handler
	connection_group cgp = get_connection_group_tcp_ipv4("127.0.0.1", PORT);

	// for HTTPS server, you also need to create appropriate ssl context
	if(SSL_KEYS_CERTS != NULL)
		sgp.server_ssl_ctx = create_gbl_server_ssl_ctx(SSL_KEYS_CERTS);

	signal(SIGINT, intHandler);
	serve(&cgp, &sgp, http_connection_handler, 100, &listen_fd);

	if(sgp.server_ssl_ctx != NULL)
		destroy_gbl_server_ssl_ctx(sgp.server_ssl_ctx);

	// delete the file cache
	if(sgp.files_cached != NULL)
		delete_file_content_cache(sgp.files_cached);
}
