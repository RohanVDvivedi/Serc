#include<http_server.h>

volatile int listen_fd = -1;
void intHandler(int dummy)
{
	if(listen_fd != -1)
	{
    	server_stop(listen_fd);
    }
}

void http_server_run(uint16_t PORT, int OVER_SSL)
{
	// initialize the content cache for serving the files
	file_content_cache* fcc = get_file_content_cache();
	make_global_file_content_cache(fcc);

	// start the server using https connection handler
	connection_group cgp = get_connection_group_tcp_ipv4("127.0.0.1", PORT);

	if(OVER_SSL)
	{
		// for HTTPS server, you also need to create appropriate global ssl context
		// we leave that task to ssl_globals source
		init_gbl_server_ssl_ctx();
		serve(&cgp, NULL, https_connection_handler, 100, &listen_fd);
		deinit_gbl_server_ssl_ctx();
	}
	else
		serve(&cgp, NULL, http_connection_handler, 100, &listen_fd);

	// delete the file cache
	delete_file_content_cache(fcc);
}
