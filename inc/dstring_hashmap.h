#ifndef DSTRING_HASHMAP_H
#define DSTRING_HASHMAP_H

#include<dstring_entry.h>
#include<hashmap.h>
#include<queue.h>

// It is a generic hashmap where we can store dstring vs any void*

typedef hashmap dmap;

dmap* get_dmap(unsigned long long int size);

void* find_equals_in_dmap(dmap* dmapp, dstring* key);

int insert_in_dmap(dmap* dmapp, dstring* key, void* value);

int remove_from_dmap(dmap* dmapp, dstring* key);

void for_each_in_dmap(dmap* dmapp, void (*operation)(dstring* key, void* value, const void* additional_params), const void* additional_params);

void delete_dmap(dmap* dmapp, void (*value_destroyer)(void* value));

#endif