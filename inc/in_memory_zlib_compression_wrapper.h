#ifndef IN_MEMORY_ZLIB_COMPRESSION_WRAPPER
#define IN_MEMORY_ZLIB_COMPRESSION_WRAPPER

#include<dstring.h>

#include<zlib.h>

// this file is header to a small compression wrapper function
// It is a wrapper around zlib library,
// make sure zlib is installed
// in the actual sense this wrapper only helps your call to zlib functions stateless
// this function will do everything in memory,
// considering you are using dstring to buffer all of file in memory for compression
// apart from zlib, other libraries may be added in future

typedef enum compression_type compression_type;
enum compression_type
{
	DEFLATE,
	GZIP,
	BROTLI,
	IDENTITY
};

// returns 1, if the compression was successfull
int compress_in_memory(dstring* data, compression_type compr_type);

// returns 1, if the uncompression was successfull
int uncompress_in_memory(dstring* data, compression_type compr_type);

#endif