#ifndef DSTRING_ENTRY_H
#define DSTRING_ENTRY_H

#include<strhsh.h>

typedef struct dentry dentry;
struct dentry
{
	dstring key;

	// cached hashvalue of the key
	unsigned int key_hash_value;

	void* value;
};

dentry* get_dentry(char* key, void* value);

// the key dstring you provide would have null cstring after calling this function
// and the original c string will be owned by the dentry
dentry* get_dentry_transferred(dstring* key, void* value);

int key_compare_dentry(const void* entry1, const void* entry2);

unsigned int key_hash_dentry(const void* entryp);

void delete_dentry(dentry* entryp, void (*value_destroyer)(void* value));

#endif