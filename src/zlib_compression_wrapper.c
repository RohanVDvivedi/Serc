#include<zlib_compression_wrapper.h>

int compress_in_memory(dstring* uncompressedData, compression_type compr_type)
{
	// initialize zstream, databuffer
	z_stream strm;
	strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;

    int err = Z_OK;

    // initialize the zlib's strm resource handle
    switch(compr_type)
    {
    	case DEFLATE :
    	{
    		err = deflateInit2(&strm, Z_BEST_COMPRESSION, Z_DEFLATED, 15, 9, Z_DEFAULT_STRATEGY);
    		break;
    	}
    	case GZIP :
    	{
    		err = deflateInit2(&strm, Z_BEST_COMPRESSION, Z_DEFLATED, 31, 9, Z_DEFAULT_STRATEGY);
    		break;
    	}
    	case BROTLI :
    	{
    		return 0;
    	}
        case IDENTITY :
        {
            return 1;
        }
    }

    if(err != Z_OK)
    {
        deflateEnd(&strm);
        return 0;
    }

	// from dstring internals
	strm.next_in = (Bytef*)(uncompressedData->cstring);
	strm.avail_in = uncompressedData->bytes_occupied;

	// make buffer dstring for compressed data ready
	dstring* compressedData = get_dstring_data(NULL, 0);
    expand_dstring(compressedData, uncompressedData->bytes_occupied);
	strm.avail_out = compressedData->bytes_allocated;
	strm.next_out = (Bytef *)(compressedData->cstring);

	// compress
	err = deflate(&strm, Z_FINISH);
    if(err != Z_OK && err != Z_STREAM_END)
    {
        deflateEnd(&strm);
        delete_dstring(compressedData);
        return 0;
    }

	// bring the compressedData to appropriate dstring format
	compressedData->bytes_occupied = strm.total_out;

	// release resources from zlib
    deflateEnd(&strm);

    // place the compressedData in the same buffer as for the uncompressedData
    // and delete the resources for the compressedData dstring buffer
    make_dstring_empty(uncompressedData);
    concatenate_dstring(uncompressedData, compressedData);
    delete_dstring(compressedData);

    return 1;
}

int uncompress_in_memory(dstring* compressedData, compression_type compr_type)
{
    // initialize zstream, databuffer
    z_stream strm;
    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;

    int err = Z_OK;

    // initialize the zlib's strm resource handle
    switch(compr_type)
    {
        case DEFLATE :
        {
            err = inflateInit2(&strm, 15);
            break;
        }
        case GZIP :
        {
            err = inflateInit2(&strm, 31);
            break;
        }
        case BROTLI :
        {
            return 0;
        }
        case IDENTITY :
        {
            return 1;
        }
    }

    if(err != Z_OK)
    {
        inflateEnd(&strm);
        return 0;
    }

    // make buffer dstring for uncompressed data ready, and expand it to hold atleast as much data as compressedData size
    dstring* uncompressedData = get_dstring_data(NULL, 0);
    expand_dstring(uncompressedData, compressedData->bytes_occupied);

    // from dstring internals
    strm.next_in = (Bytef*)(compressedData->cstring);
    strm.avail_in = compressedData->bytes_occupied;

    // make buffer dstring for uncompressed data ready
    strm.avail_out = uncompressedData->bytes_allocated;
    strm.next_out = (Bytef *)(uncompressedData->cstring);

    while(strm.total_in < compressedData->bytes_occupied && (err == Z_OK || err == Z_BUF_ERROR) && (err != Z_STREAM_END))
    {
        // compress
        err = inflate(&strm, Z_FINISH);

        // bring the uncompressedData to appropriate dstring format
        uncompressedData->bytes_occupied = strm.total_out;

        // from dstring internals
        strm.next_in = (Bytef*)(compressedData->cstring + strm.total_in);
        strm.avail_in = compressedData->bytes_occupied - strm.total_in;

        if(uncompressedData->bytes_allocated <= uncompressedData->bytes_occupied)
            expand_dstring(uncompressedData, uncompressedData->bytes_allocated);

        // make buffer dstring for uncompressed data ready
        strm.avail_out = uncompressedData->bytes_allocated - uncompressedData->bytes_occupied;
        strm.next_out = (Bytef *)(uncompressedData->cstring + uncompressedData->bytes_occupied);
    }

    if(err != Z_OK && err != Z_STREAM_END)
    {
        inflateEnd(&strm);
        delete_dstring(uncompressedData);
        return 0;
    }

    // bring the uncompressedData to appropriate dstring format
    uncompressedData->bytes_occupied = strm.total_out;

    // release resources from zlib
    inflateEnd(&strm);

    // place the uncompressedData in the same buffer as for the compressedData
    // and delete the resources for the uncompressedData dstring buffer
    make_dstring_empty(compressedData);
    concatenate_dstring(compressedData, uncompressedData);
    delete_dstring(uncompressedData);

    return 1;
}
