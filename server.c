#include<stdio.h>
#include<stdlib.h>

#include<config.h>

#include<http_server.h>

void connection_started(int conn_fd, const void* params)
{
	printf("connection started %s\n", ((char*)params));
}

void connection_finished(int conn_fd, const void* params)
{
	printf("connection finished %s\n", ((char*)params));
}

int main()
{
	//http_server_run(80, SERC_ROOT_PATH, 0, connection_started, connection_finished,"-- Rohan");
	http_server_run(443, /*NULL*/SERC_ROOT_PATH, 1, connection_started, connection_finished, "-- Rohan");
	return 0;
}