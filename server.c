#include<stdio.h>
#include<stdlib.h>


// this defines what SSL encryption and SSL certificates folder would be used for serving content
// the below lines mean "./con/server.crt" => file for public key and certificate
// and "./con/server.key" => file for private key
// SSL_PRIVATE_KEY_AND_PUB_CERTIFICATE = "./con/server"

#include<http_server.h>

int main(int argc, char** argv)
{
	if(argc == 1)
		http_server_run(80, ".", NULL);
	else if(argc == 2)
		http_server_run(80, argv[1], NULL);
	else
		http_server_run(443, argv[1], argv[2]);
	return 0;
}