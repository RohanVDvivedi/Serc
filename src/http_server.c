#include<http_server.h>

volatile int listen_fd = -1;
void intHandler(int dummy)
{
	if(listen_fd != -1)
    	server_stop(listen_fd);
}

void http_server_run(uint16_t PORT, char* ROOT_PATH, int OVER_SSL, void (*connection_started)(int conn_fd, const void* connection_callback_params), void (*connection_finished)(int conn_fd, const void* connection_callback_params), const void* connection_callback_params)
{
	// these values will be constant through out all the connections of this specific server
	server_global_params sgp = {.connection_started_callback = connection_started,
								.connection_finished_callback = connection_finished,
								.connection_callback_params = connection_callback_params};

	// initialize the content cache for serving the files
	sgp.files_cached = ((ROOT_PATH != NULL) ? get_file_content_cache(ROOT_PATH) : NULL);

	// start the server using https connection handler
	connection_group cgp = get_connection_group_tcp_ipv4("127.0.0.1", PORT);

	// for HTTPS server, you also need to create appropriate ssl context
	if(OVER_SSL)
		sgp.server_ssl_ctx = create_gbl_server_ssl_ctx();

	serve(&cgp, &sgp, http_connection_handler, 100, &listen_fd);

	if(sgp.server_ssl_ctx != NULL)
		destroy_gbl_server_ssl_ctx(sgp.server_ssl_ctx);

	// delete the file cache
	if(sgp.files_cached != NULL)
		delete_file_content_cache(sgp.files_cached);
}
