#ifndef FILE_HANDLER_H
#define FILE_HANDLER_H

typedef struct file_cache file_cache;
struct file_cache
{
	// this is the root path to the folder that we are serving using this server
	dstring root_path;
};

// this will create the file cache, for your server
file_cache* new_file_cache(const char* root_path);

// this method returns -1, if the file is absent on th disk, else 0 if no error
// the file if found in file_cache_table, it will be appended in append_file_contents
// else we will go to disk and find the file and append it to append_file_contents, 
// and save it to the cashtable against ite relative path
int read_file_in_dstring(dstring* append_file_contents, file_cache* fc, dstring* relative_file_path);

// it will clear the cache completely and delete the struct then, hence releasing all the resources
void delete_file_cache(file_cache* fc);

// UTILITY FUNCTION

// extension_result is appended with the extension from the file_path, only a utility function
void get_extension_from_file_path(dstring* extension_result, dstring* file_path);

#endif