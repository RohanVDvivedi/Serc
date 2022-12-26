#ifndef SERVER_GLOBAL_PARAMS_H
#define SERVER_GLOBAL_PARAMS_H

#include<file_handler.h>
#include<ssl_context_utility.h>

typedef struct server_global_params server_global_params;
struct server_global_params
{
	// below parameter helps in serving static files
	file_cache* server_file_cache;
};

#endif