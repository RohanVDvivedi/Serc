#include<file_handler.h>

// this is where we will cache the file that we will serve,
// we read from the server root, to send in response will be sent 
hashmap* file_cache = NULL;
rwlock* file_cache_rwlock = NULL;

typedef struct file_cache_component file_cache_component;
struct file_cache_component
{
	dstring* file_content;
	rwlock* file_content_rwlock;
};

void init_file_cache_if_empty()
{
	if(file_cache == NULL)
	{
		file_cache = get_hashmap(30, (unsigned long long int (*)(const void*))getHashValueDstring, (int (*)(const void*, const void*))compare_dstring, ELEMENTS_AS_RED_BLACK_BST);
		file_cache_rwlock = get_rwlock();
	}
}

int read_file_in_dstring(dstring* file_contents_result, dstring* file_path)
{
	init_file_cache_if_empty();

	// find the file from the cache, once the file has been requested from the server it gets cached, in page cache
	read_lock(file_cache_rwlock);
	file_cache_component* file_content_from_cache = (file_cache_component*)(find_value_from_hash(file_cache, file_path));
	read_unlock(file_cache_rwlock);

	// if we cache miss, we need to go to disk, we go to disk only if we confirm that the file is present
	if(file_content_from_cache == NULL && access(file_path->cstring, R_OK) != -1)
	{
		// insert the cache entry, so we do not miss the cache next time
		file_content_from_cache = (file_cache_component*)malloc(sizeof(file_cache_component));
		file_content_from_cache->file_content = get_dstring("", 10);
		file_content_from_cache->file_content_rwlock = get_rwlock();

		// write newly built component to cache
		write_lock(file_cache_rwlock);
		insert_entry_in_hash(file_cache, get_dstring(file_path->cstring ,10), file_content_from_cache);
		write_unlock(file_cache_rwlock);
    	
    	// open the file
    	FILE* file = fopen(file_path->cstring, "rb");

    	// buffer to read the new file, for which we missed the cache
    	#define FILE_READ_BUFFER_SIZE 4000
    	char file_buffer[FILE_READ_BUFFER_SIZE];

		while(!feof(file))
		{
			long long int read_count = fread(file_buffer, sizeof(char), FILE_READ_BUFFER_SIZE, file);
			if(read_count >= 0)
			{
				write_lock(file_content_from_cache->file_content_rwlock);
				appendn_to_dstring(file_content_from_cache->file_content, file_buffer, read_count);
				write_unlock(file_content_from_cache->file_content_rwlock);
			}
		}

		fclose(file);
	}

	// if cache had been updated, we read from the cache variable
	if(file_content_from_cache != NULL)
	{
		read_lock(file_content_from_cache->file_content_rwlock);
		concatenate_dstring(file_contents_result, file_content_from_cache->file_content);
		read_unlock(file_content_from_cache->file_content_rwlock);
		return 0;
	}
	return -1;
}

void get_extension_from_file_path(dstring* extension_result, dstring* path)
{
	char* path_t = path->cstring;
	int in_extension = 0;
	while((*(path_t)) != '\0')
	{
		if(in_extension == 1)
		{
			char temp[2] = "X";
			temp[0] = (*(path_t));
			append_to_dstring(extension_result, temp);
		}
		else if((*(path_t)) == '.')
		{
			in_extension = 1;
		}
		path_t++;
	}
}