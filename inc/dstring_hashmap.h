#ifndef DSTRING_HASHMAP_H
#define DSTRING_HASHMAP_H

#include<dstring_entry.h>
#include<hashmap.h>

// It is a hashmap in disguise, where we can store dstring vs dstring
// CASE_SENSITIVE_KEY_TYPE will compare keys such that 'A' is not equal to 'a' in the dstring
// while CASE_INSENSITIVE_KEY_TYPE will compare keys such that 'a' and 'A' are the same characters

// it is designed for storing parameters and headers for http_request and http_response

// It maintains unique set of dstrings as keys if you only use insert_unique and remove functionality
// insert_unique will check before making  an insert
// and it would update an existing entry if an entry with the same key exists

// while if you use with insert_duplicate and remove
// it will insert in dmap without making any checks, insert duplicate will insert even if an entry
// with duplicate key exists

typedef enum dmap_key_type dmap_key_type;
enum dmap_key_type
{
	CASE_SENSITIVE_KEY_TYPE,
	CASE_INSENSITIVE_KEY_TYPE
};

typedef hashmap dmap;

void initialize_dmap(dmap* dmapp, dmap_key_type key_type, unsigned int size);

dstring* find_equals_in_dmap(dmap* dmapp, dstring* key);
dstring* find_equals_in_dmap_cstr(dmap* dmapp, char* key);

// inserts will fail with 0, if the key already exists in dmap 
void insert_unique_in_dmap(dmap* dmapp, dstring* key, dstring* value);
void insert_unique_in_dmap_cstr(dmap* dmapp, char* key, char* value);

void insert_duplicate_in_dmap(dmap* dmapp, dstring* key, dstring* value);
void insert_duplicate_in_dmap_cstr(dmap* dmapp, char* key, char* value);

int remove_from_dmap(dmap* dmapp, dstring* key);
int remove_from_dmap_cstr(dmap* dmapp, char* key);

void remove_all_from_dmap(dmap* dmapp);

void for_each_in_dmap(dmap* dmapp, void (*operation)(dstring* key, dstring* value, const void* additional_params), const void* additional_params);

void deinitialize_dmap(dmap* dmapp);

#endif