#include<http_server.h>
#include<http_https_connection_handler.h>

volatile int listen_fd = -1;
void intHandler(int dummy)
{
	if(listen_fd != -1)
	{
    	server_stop(listen_fd);
    }
}

void http_server_run(uint16_t PORT)
{
	// initialize the content cache for serving the files
	file_content_cache* fcc = get_file_content_cache();
	make_global_file_content_cache(fcc);

	// start the server using http connection handler
	connection_group cgp = get_connection_group_tcp_ipv4("127.0.0.1", PORT);
	serve(&cgp, http_connection_handler, 100, &listen_fd);

	// delete the file cache
	delete_file_content_cache(fcc);
}

void https_server_run(uint16_t PORT, char* ssl_cert_path)
{
	// initialize the content cache for serving the files
	file_content_cache* fcc = get_file_content_cache();
	make_global_file_content_cache(fcc);

	// start the server using https connection handler
	connection_group cgp = get_connection_group_tcp_ipv4("127.0.0.1", PORT);
	serve(&cgp, https_connection_handler, 100, &listen_fd);

	// delete the file cache
	delete_file_content_cache(fcc);
}
