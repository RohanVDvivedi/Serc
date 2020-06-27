#ifndef CONFIG_H
#define CONFIG_H

#define SERC_ROOT_PATH "./public"

#define DEFAULT_SERVER_RESPONSE_COMPRESSION GZIP
// possible values are DEFLATE, GZIP, BROTLI

// this defines what SSL encryption and SSL certificates folder would be used for serving content
#define SSL_CERTIFICATION_AND_KEYS	"./con/cert_and_keys.pem"
#define SSL_SERVER_METHOD			TLSv1_2_server_method 

// for more options to SSL_SERVER_METHOD, use openSSL documentation

#endif