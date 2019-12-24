#include<stdio.h>
#include<stdlib.h>

#include<http_server.h>
#include<file_handler.h>

int main()
{
	http_server_run(6900);
	return 0;
}