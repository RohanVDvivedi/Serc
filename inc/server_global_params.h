#ifndef SERVER_GLOBAL_PARAMS_H
#define SERVER_GLOBAL_PARAMS_H

#include<dstring.h>

typedef struct server_global_params server_global_params;
struct server_global_params
{
	dstring root_path;
};

#endif