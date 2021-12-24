#include<zlib_compression_wrapper.h>

int compress_in_memory(dstring* uncompressedData, compression_type compr_type)
{
    // empty and null check
    if(uncompressedData == NULL || is_empty_dstring(uncompressedData))
        return 0;

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
	strm.next_in = (Bytef*)(get_byte_array_dstring(uncompressedData));
	strm.avail_in = get_char_count_dstring(uncompressedData);

	// make buffer dstring for compressed data ready
	dstring compressedData;
    init_empty_dstring(&compressedData, get_char_count_dstring(uncompressedData));
	strm.avail_out = get_capacity_dstring(&compressedData);
	strm.next_out = (Bytef *)(get_byte_array_dstring(&compressedData));

	// compress
	err = deflate(&strm, Z_FINISH);
    if(err != Z_OK && err != Z_STREAM_END)
    {
        deflateEnd(&strm);
        deinit_dstring(&compressedData);
        return 0;
    }

	// bring the compressedData to appropriate dstring format
	increment_char_count_dstring(&compressedData, strm.total_out);

	// release resources from zlib
    deflateEnd(&strm);

    // place the compressedData in the same buffer as for the uncompressedData
    // and delete the resources for the compressedData dstring buffer
    deinit_dstring(uncompressedData);
    (*(uncompressedData)) = compressedData;

    return 1;
}

int uncompress_in_memory(dstring* compressedData, compression_type compr_type)
{
    // empty and null check
    if(compressedData == NULL || is_empty_dstring(compressedData))
        return 0;

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
    dstring uncompressedData;
    init_empty_dstring(&uncompressedData, get_char_count_dstring(compressedData));

    // from dstring internals
    strm.next_in = (Bytef*)(get_byte_array_dstring(compressedData));
    strm.avail_in = get_char_count_dstring(compressedData);

    // make buffer dstring for uncompressed data ready
    strm.avail_out = get_capacity_dstring(&uncompressedData);
    strm.next_out = (Bytef *)(get_byte_array_dstring(&uncompressedData));

    while(strm.total_in < get_char_count_dstring(compressedData) && (err == Z_OK || err == Z_BUF_ERROR) && (err != Z_STREAM_END))
    {
        // compress
        err = inflate(&strm, Z_FINISH);

        // bring the uncompressedData to appropriate dstring format
        increment_char_count_dstring(&uncompressedData, strm.total_out);

        // from dstring internals
        strm.next_in = (Bytef*)(get_byte_array_dstring(compressedData) + strm.total_in);
        strm.avail_in = get_char_count_dstring(compressedData) - strm.total_in;

        if(get_capacity_dstring(&uncompressedData) <= get_char_count_dstring(&uncompressedData))
            expand_dstring(&uncompressedData, get_capacity_dstring(&uncompressedData));

        // make buffer dstring for uncompressed data ready
        strm.avail_out = get_unused_capacity_dstring(&uncompressedData);
        strm.next_out = (Bytef *)(get_byte_array_dstring(&uncompressedData) + get_char_count_dstring(&uncompressedData));
    }

    if(err != Z_OK && err != Z_STREAM_END)
    {
        inflateEnd(&strm);
        deinit_dstring(&uncompressedData);
        return 0;
    }

    // bring the uncompressedData to appropriate dstring format
    uncompressedData.bytes_occupied = strm.total_out;

    // release resources from zlib
    inflateEnd(&strm);

    // place the uncompressedData in the same buffer as for the compressedData
    // and delete the resources for the uncompressedData dstring buffer
    deinit_dstring(compressedData);
    (*(compressedData)) = uncompressedData;

    return 1;
}
