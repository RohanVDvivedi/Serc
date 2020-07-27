#include<stdio.h>
#include<stdlib.h>


// this defines what SSL encryption and SSL certificates folder would be used for serving content
// the below lines mean "./con/server.crt" => file for public key and certificate
// and "./con/server.key" => file for private key
// SSL_PRIVATE_KEY_AND_PUB_CERTIFICATE = "./con/server"

#include<http_server.h>

#include<string.h>

int starts_with(const char *pre, const char *str)
{
    size_t lenpre = strlen(pre),
           lenstr = strlen(str);
    return lenstr < lenpre ? 0 : (memcmp(pre, str, lenpre) == 0);
}

int main(int argc, char** argv)
{
	// default paths to use
	char* root_path = ".";
	char* ssl_keys_certs_prefix = NULL;

	int port = -1;

	if(argc > 4)
		return -1;

	for(int i = 1; i < argc; i++)
	{
		if(starts_with("--port=", argv[i]))
			sscanf(argv[i], "--port=%d", &port);
		else if(starts_with("--root=", argv[i]))
			root_path = argv[i] + strlen("--root=");
		else if(starts_with("--ssl_cert_keys=", argv[i]))
			ssl_keys_certs_prefix = argv[i] + strlen("--ssl_cert_keys=");
	}

	if(port == -1)
	{
		if(ssl_keys_certs_prefix == NULL)	// we are starting a http server
			port = 80;
		else
			port = 443;
	}

	printf("serving files at path:%s http%s on port %d\n", root_path, ((ssl_keys_certs_prefix==NULL)?"":"s"), port);

	if(ssl_keys_certs_prefix != NULL)
		printf("ssl certficate (and public key) found at %s.crt and private key %s.key\n", ssl_keys_certs_prefix, ssl_keys_certs_prefix);

	http_server_run(port, root_path, ssl_keys_certs_prefix);

	return 0;
}