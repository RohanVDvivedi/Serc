#include<in_memory_zlib_compression_wrapper.h>

int compress_in_memory(dstring* uncompressedData, compression_type compr_type)
{
	// initialize zstream, databuffer
	z_stream strm;
	strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;

    // initialize the zlib's strm resource handle
	deflateInit(&strm, Z_BEST_COMPRESSION);

	// from dstring internals
	strm.next_in = (Bytef*)(uncompressedData->cstring);
	strm.avail_in = uncompressedData->bytes_occupied - 1;

	// make buffer dstring for compressed data ready
	dstring* compressedData = get_dstring("", compressedData->bytes_occupied);
	strm.avail_out = compressedData->bytes_allocated - 1;
	strm.next_out = (Bytef *)(compressedData->cstring);

	// compress
	deflate(&strm, Z_FINISH);

	// bring the compressedData to appropriate dstring format
	compressedData->bytes_occupied = strm.total_out + 1;
	compressedData->cstring[compressedData->bytes_occupied - 1] = '\0';

	// release resources from zlib
    deflateEnd(&strm);

    // place the compressedData in the same buffer as for the uncompressedData
    // and delete the resources for the compressedData dstring buffer
    make_dstring_empty(uncompressedData);
    concatenate_dstring(uncompressedData, compressedData);
    delete_dstring(compressedData);

    return Z_OK;
}