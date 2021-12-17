#include<dstring_entry.h>

#include<strhsh.h>

#include<stdlib.h>

dentry* get_dentry(const dstring* key, const dstring* value)
{
	dentry* entryp = malloc(sizeof(dentry));
	init_empty_dstring(&(entryp->key), get_char_count_dstring(key));
	concatenate_dstring(&(entryp->key), key);
	init_empty_dstring(&(entryp->value), get_char_count_dstring(value));
	concatenate_dstring(&(entryp->key), value);
	entryp->key_hash_value = 0;
	return entryp;
}

dentry* get_dentry_cstrings(const char* key, const char* value)
{
	dentry* entryp = malloc(sizeof(dentry));
	init_dstring(&(entryp->key), key, (key == NULL) ? 0 : strlen(key));
	init_dstring(&(entryp->value), value, (value == NULL) ? 0 : strlen(value));
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