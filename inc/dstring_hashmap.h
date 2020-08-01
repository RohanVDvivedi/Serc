#ifndef DSTRING_HASHMAP_H
#define DSTRING_HASHMAP_H

#include<dstring_entry.h>
#include<hashmap.h>
#include<queue.h>

// It is a generic hashmap where we can store dstring vs any void*
// It maintains unique set of dstrings only as keys
// insert will check and first remove, if you insert using an identical key (not necessarily the same)

typedef struct dmap dmap;
struct dmap
{
	hashmap map;						// hashmap form dstring* to void*
	void (*value_destroyer)(void* value);		// destructor for the void* value
};

void initialize_dmap(dmap* dmapp, unsigned int size, void (*value_destroyer)(void* value));

void* find_equals_in_dmap(dmap* dmapp, dstring* key);

void* find_equals_in_dmap_cstr(dmap* dmapp, char* key);

void insert_in_dmap(dmap* dmapp, dstring* key, void* value);

void insert_in_dmap_cstr(dmap* dmapp, char* key, void* value);

int remove_from_dmap(dmap* dmapp, dstring* key);

int remove_from_dmap_cstr(dmap* dmapp, char* key);

void remove_all_from_dmap(dmap* dmapp);

void for_each_in_dmap(dmap* dmapp, void (*operation)(dstring* key, void* value, const void* additional_params), const void* additional_params);

void deinitialize_dmap(dmap* dmapp);

#endif