#include<stdio.h>
#include<stdlib.h>

#include<serve.h>
#include<file_handler.h>

int main()
{
	init_file_content_cache();
	server_run(6900);
	return 0;
}