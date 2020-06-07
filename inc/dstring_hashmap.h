#ifndef DSTRING_HASHMAP_H
#define DSTRING_HASHMAP_H

#include<dstring_entry.h>
#include<hashmap.h>
#include<queue.h>

// It is a generic hashmap where we can store dstring vs any void*
// It maintains unique set of dstrings only as keys

typedef struct dmap dmap;
struct dmap
{
	hashmap map;						// hashmap form dstring* to void*
	void (*value_destroyer)(void* value);		// destructor for the void* value
};

dmap* get_dmap(unsigned long long int size, void (*value_destroyer)(void* value));

void* find_equals_in_dmap(dmap* dmapp, dstring* key);

void* find_equals_in_dmap_cstr(dmap* dmapp, char* key);

int insert_in_dmap(dmap* dmapp, dstring* key, void* value);

int insert_in_dmap_cstr(dmap* dmapp, char* key, void* value);

int remove_from_dmap(dmap* dmapp, dstring* key);

void remove_all_from_dmap(dmap* dmapp);

void for_each_in_dmap(dmap* dmapp, void (*operation)(dstring* key, void* value, const void* additional_params), const void* additional_params);

void delete_dmap(dmap* dmapp);

#endif