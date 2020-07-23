#ifndef SERVER_GLOBAL_PARAMS_H
#define SERVER_GLOBAL_PARAMS_H

#include<file_handler.h>
#include<ssl_context_utility.h>

typedef struct server_global_params server_global_params;
struct server_global_params
{
	// if non NULL, this is will be handled as https connection, else an http connection
	SSL_CTX* server_ssl_ctx;

	// this is the file content cache for the server
	// this is where the server will cache the files that it will read from the disk, once it serves them for the first time
	file_content_cache* files_cached;

	// the each connection will live on its own thread during its life time
	// hence you may use pthread_key get and set attribites to pass params between connection start and end
	// connection_started_callback will be called before receiving the first request
	// connection_finished_callback will be called after sending the last response
	void (*connection_started_callback)();
	void (*connection_finished_callback)();
};

#endif