#include<stdio.h>
#include<stdlib.h>

#include<config.h>

#include<http_server.h>

void connection_started(int conn_fd, void* server_specific_params)
{
	printf("connection started\n");
}

void connection_finished(int conn_fd, void* server_specific_params)
{
	printf("connection finished\n");
}

int main()
{
	//http_server_run(80, SERC_ROOT_PATH, 0, connection_started, connection_finished);
	http_server_run(443, SERC_ROOT_PATH, 1, connection_started, connection_finished);
	return 0;
}