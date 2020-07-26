#include<stdio.h>
#include<stdlib.h>

#define SERC_ROOT_PATH "./public"

// this defines what SSL encryption and SSL certificates folder would be used for serving content
// the below lines mean "./con/server.crt" => file for public key and certificate
// and "./con/server.key" => file for private key
#define SSL_PRIVATE_KEY_AND_PUB_CERTIFICATE			"./con/server.crt"

#include<http_server.h>

int main()
{
	http_server_run(80, SERC_ROOT_PATH, NULL);
	//http_server_run(443, SERC_ROOT_PATH, SSL_PRIVATE_KEY_AND_PUB_CERTIFICATE);
	return 0;
}