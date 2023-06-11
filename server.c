#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include<http_server.h>

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
	int serve_dirs = 0;
	char* ssl_keys_certs_prefix = NULL;

	int port = -1;

	if(argc > 5)
		return -1;

	// read command line options 
	// --port=<port_no>
	// --root=<root_folder>
	// --ssl_cert_keys=<path_to_public_n_private_ssl_keys_without_extension>
	for(int i = 1; i < argc; i++)
	{
		if(starts_with("--port=", argv[i]))
			sscanf(argv[i], "--port=%d", &port);
		else if(starts_with("--root=", argv[i]))
			root_path = argv[i] + strlen("--root=");
		else if(0 == strcmp("--serve-dirs", argv[i]))
			serve_dirs = 1;
		else if(starts_with("--ssl_cert_keys=", argv[i]))
			ssl_keys_certs_prefix = argv[i] + strlen("--ssl_cert_keys=");
	}

	// select default port, if not specified in commandline options
	// 80 for http and 443 for https
	if(port == -1)
	{
		if(ssl_keys_certs_prefix == NULL)	// we are starting a http server
			port = 80;
		else
			port = 443;
	}

	// debug statements, confirming the user command line options
	printf("serving files at path:%s http%s on port %d\n", root_path, ((ssl_keys_certs_prefix==NULL)?"":"s"), port);
	if(ssl_keys_certs_prefix != NULL)
		printf("ssl certficate (and public key) found at %s.crt and private key %s.key\n", ssl_keys_certs_prefix, ssl_keys_certs_prefix);

	// start the server
	http_server_run(port, root_path, serve_dirs, ssl_keys_certs_prefix, NULL);

	return 0;
}