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
};

#endif