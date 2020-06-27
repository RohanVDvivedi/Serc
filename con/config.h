#ifndef CONFIG_H
#define CONFIG_H

#define SERC_ROOT_PATH "./public"

#define DEFAULT_SERVER_RESPONSE_COMPRESSION GZIP
// possible values are DEFLATE, GZIP, BROTLI

// this defines what SSL encryption and SSL certificates folder would be used for serving content
#define SSL_CERTIFICATES_PATH "./con"
#define SSL_SERVER_METHOD	TLSv1_2_server_method 

// for more options to SSL_SERVER_METHOD, use openSSL documentation

#endif