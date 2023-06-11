#ifndef SERVER_GLOBAL_PARAMS_H
#define SERVER_GLOBAL_PARAMS_H

#include<dstring.h>

typedef struct server_global_params server_global_params;
struct server_global_params
{
	// root path of all the static files
	dstring root_path;

	// set this value if you want to serve directories at the root path
	int serve_dirs;

	// this is a common server param that will be passed to all your controllers
	const void* server_param;
};

#endif