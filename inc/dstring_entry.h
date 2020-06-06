#ifndef DSTRING_ENTRY_H
#define DSTRING_ENTRY_H

#include<strhsh.h>

typedef struct dstring_entry dstring_entry;
struct dstring_entry
{
	dstring* key;
	dstring* value;

	// cached hashvalue of the key
	unsigned long long int key_hash_value;
};

dstring_entry* get_dstring_entry(dstring* key, unsigned long long int value_size);

int key_compare_dstring_entry(const void* entry1, const void* entry2);

int key_hash_dstring_entry(const void* entryp);

void delete_dstring_entry(dstring_entry* entryp);

#endif