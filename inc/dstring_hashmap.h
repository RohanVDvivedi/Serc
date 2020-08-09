#ifndef DSTRING_HASHMAP_H
#define DSTRING_HASHMAP_H

#include<dstring_entry.h>
#include<hashmap.h>
#include<queue.h>

// It is a hashmap in disguise, where we can store dstring vs dstring
// it is used for storing parameters and headers for http_request and http_response
// It maintains unique set of dstrings as keys
// insert will check and first remove, if you insert using an identical key (not necessarily the same)

typedef hashmap dmap;

void initialize_dmap(dmap* dmapp, unsigned int size);

dstring* find_equals_in_dmap(dmap* dmapp, dstring* key);

dstring* find_equals_in_dmap_cstr(dmap* dmapp, char* key);

// the key and value dstrings are now pointing to NULL
void insert_in_dmap(dmap* dmapp, dstring* key, dstring* value);

void insert_in_dmap_cstr(dmap* dmapp, char* key, char* value);

int remove_from_dmap(dmap* dmapp, dstring* key);

int remove_from_dmap_cstr(dmap* dmapp, char* key);

void remove_all_from_dmap(dmap* dmapp);

void for_each_in_dmap(dmap* dmapp, void (*operation)(dstring* key, dstring* value, const void* additional_params), const void* additional_params);

void deinitialize_dmap(dmap* dmapp);

#endif