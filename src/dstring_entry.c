#include<dstring_entry.h>

#include<strhsh.h>

#include<stdlib.h>

dentry* get_dentry(dstring* key, dstring* value)
{
	dentry* entryp = malloc(sizeof(dentry));
	init_dstring_data(&(entryp->key), key->cstring, value->bytes_occupied);
	init_dstring_data(&(entryp->value), value->cstring, value->bytes_occupied);
	entryp->key_hash_value = 0;
	return entryp;
}

dentry* get_dentry_cstrings(char* key, char* value)
{
	dentry* entryp = malloc(sizeof(dentry));
	init_dstring_data(&(entryp->key), key, (key == NULL) ? 0 : strlen(key));
	init_dstring_data(&(entryp->value), value, (value == NULL) ? 0 : strlen(value));
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