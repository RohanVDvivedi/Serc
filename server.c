#include<stdio.h>
#include<stdlib.h>

#include<config.h>

#include<http_server.h>

int main()
{
	//http_server_run(80, SERC_ROOT_PATH, 0);
	http_server_run(443, /*NULL*/SERC_ROOT_PATH, 1);
	return 0;
}