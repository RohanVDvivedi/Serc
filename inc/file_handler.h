#ifndef FILE_HANDLER_H
#define FILE_HANDLER_H

#include<stdio.h>
#include<unistd.h>

#include<hashmap.h>
#include<dstring.h>

#include<rwlock.h>

#include<strhsh.h>

#define SERC_ROOT_PATH "./serc_root"

typedef struct file_content_cache file_content_cache;
struct file_content_cache
{
	hashmap* file_content_cache_hashmap;
	rwlock* file_content_cache_hashmap_rwlock;
};

// this will initialize the file cache
// you must call this function before using 
// the file_handlers read_file_in_dstring function
file_content_cache* get_file_content_cache();

// this method returns -1, if the file is absent, else 0 if no error
// we go to the disk, read file, cache it, and return you the contents aswell, so next time we can serve you faster
int read_file_in_dstring(dstring* file_contents_result, file_content_cache* fcc_p, dstring* file_path);

// this function can be used to clear all the contents of the cache,
// this does not destroy the content cache, hence the object can be reused to cache more other files
void clear_file_content_cache(file_content_cache* fcc_p);

// it will clear the cache completely and delete the struct then, hence releasing all the resources
void delete_file_content_cache(file_content_cache* fcc_p);

// extension_result is appended with the extension from the file_path, only a utility function
void get_extension_from_file_path(dstring* extension_result, dstring* file_path);

#endif