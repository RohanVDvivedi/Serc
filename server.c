#include<stdio.h>
#include<stdlib.h>

#include<http_server.h>

int main()
{
	//http_server_run(80, 0);
	http_server_run(443, 1);
	return 0;
}