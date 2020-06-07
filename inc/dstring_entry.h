#ifndef DSTRING_ENTRY_H
#define DSTRING_ENTRY_H

#include<strhsh.h>

typedef struct dentry dentry;
struct dentry
{
	dstring* key;

	// cached hashvalue of the key
	unsigned long long int key_hash_value;

	void* value;
};

dentry* get_dentry(dstring* key, void* value);

int key_compare_dentry(const void* entry1, const void* entry2);

unsigned long long int key_hash_dentry(const void* entryp);

void delete_dentry(dentry* entryp, void (*value_destroyer)(void* value));

#endif