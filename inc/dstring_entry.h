#ifndef DSTRING_ENTRY_H
#define DSTRING_ENTRY_H

#include<strhsh.h>

typedef struct dentry dentry;
struct dentry
{
	// cached hashvalue of the key
	unsigned int key_hash_value;

	dstring key;

	dstring value;
};

dentry* get_dentry(char* key, char* value);

// after calling get dentry transferred function,
// the key and value are transferred, and a key and value themselves now hold null csrings
dentry* get_dentry_transferred(dstring* key, dstring* value);

int key_compare_dentry(const void* entry1, const void* entry2);

unsigned int key_hash_dentry(const void* entryp);

void delete_dentry(dentry* entryp);

#endif