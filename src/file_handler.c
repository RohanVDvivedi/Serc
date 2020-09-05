#include<file_handler.h>

#include<stdio.h>
#include<stdlib.h>
#include<sys/stat.h>
#include<unistd.h>

file_cache* get_file_cache(char* root_path)
{
	file_cache* fc = malloc(sizeof(file_cache));
	fc->root_path = root_path;
	fc->file_cache_table = get_cashtable(30);
	return fc;
}

int read_file_in_dstring(dstring* append_file_contents, file_cache* fc, dstring* relative_file_path)
{
	// cache hit case ---->>>

	int cache_hit = get_value_cashtable(fc->file_cache_table, relative_file_path, append_file_contents);

	if(cache_hit)
		return 0;

	// cache hit case <<<----

	// cache miss case ---->>>

	dstring file_path;
	init_dstring(&file_path, fc->root_path);
	concatenate_dstring(&file_path, relative_file_path);

	// if the file is not in cache, check if the file even exists
	// if the file does not exist, or if it is a folder in the server root, we return with -1
	if(access(file_path.cstring, R_OK) != 0)
	{
		deinit_dstring(&(file_path));
		return -1;
	}
	struct stat file_stat;
	stat(file_path.cstring, &file_stat);
	if(S_ISDIR(file_stat.st_mode))
	{
		deinit_dstring(&(file_path));
		return -1;
	}

    // buffer to read the new file, for which we missed the cache
    #define FILE_READ_BUFFER_SIZE 1024
    char file_buffer[FILE_READ_BUFFER_SIZE];

    // store the actual file contents which were requested in this dstring
    dstring file_contents;	init_dstring_data(&file_contents, NULL, 0);

    // loop through the file, to put it in cache 
    // open the file
    FILE* file = fopen(file_path.cstring, "rb");
		while(!feof(file))
		{
			long long int read_count = fread(file_buffer, sizeof(char), FILE_READ_BUFFER_SIZE, file);
			if(read_count >= 0)
				appendn_to_dstring(&file_contents, file_buffer, read_count);
		}
	fclose(file);

	// concatenate the file result to the append_file_contents
	// and update the file_cache_tabel
	set_key_value_expiry_cashtable(fc->file_cache_table, relative_file_path, &file_contents, CACHE_EXPIRY_MINS * 60);
	concatenate_dstring(append_file_contents, &file_contents);

	deinit_dstring(&(file_contents));
	deinit_dstring(&(file_path));

	return 0;
	// cache miss case <<<----
}

void delete_file_cache(file_cache* fc)
{
	delete_cashtable(fc->file_cache_table);
	free(fc);
}

void get_extension_from_file_path(dstring* extension_result, dstring* path)
{
	char* path_t = path->cstring;
	for(;path_t < path->cstring + path->bytes_occupied; path_t++)
	{
		if((*path_t) == '.')
			break;
	}
	if((*path_t) == '.')
	{
		path_t++;
		appendn_to_dstring(extension_result, path_t, path->cstring + path->bytes_occupied - path_t);
	}
}