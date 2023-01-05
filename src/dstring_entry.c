#include<dstring_entry.h>

#include<strhsh.h>

#include<stdlib.h>

dentry* new_dentry(const dstring* key, const dstring* value)
{
	dentry* entryp = malloc(sizeof(dentry));
	init_copy_dstring(&(entryp->key), key);
	init_copy_dstring(&(entryp->value), value);
	initialize_rbhnode(&(entryp->embed_node));
	entryp->key_hash_value = 0;
	return entryp;
}

dentry* new_dentry_from_cstrings(const char* key, const char* value)
{
	dentry* entryp = malloc(sizeof(dentry));
	init_dstring(&(entryp->key), key, (key == NULL) ? 0 : strlen(key));
	init_dstring(&(entryp->value), value, (value == NULL) ? 0 : strlen(value));
	initialize_rbhnode(&(entryp->embed_node));
	entryp->key_hash_value = 0;
	return entryp;
}

int key_compare_dentry_CASE_SENSITIVE(const void* entry1, const void* entry2)
{
	return compare_dstring(&(((dentry*)entry1)->key), &(((dentry*)entry2)->key));
}

int key_compare_dentry_CASE_INSENSITIVE(const void* entry1, const void* entry2)
{
	return case_compare_dstring(&(((dentry*)entry1)->key), &(((dentry*)entry2)->key));
}

unsigned int key_hash_dentry(const void* entryp)
{
	if(((dentry*)entryp)->key_hash_value == 0)
		((dentry*)entryp)->key_hash_value = getHashValueDstring(&(((dentry*)entryp)->key));
	return ((dentry*)entryp)->key_hash_value;
}

void delete_dentry(dentry* entryp)
{
	deinit_dstring(&(entryp->key));
	deinit_dstring(&(entryp->value));
	free(entryp);
}