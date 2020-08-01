#include<dstring_entry.h>

dentry* get_dentry(char* key, void* value)
{
	dentry* entryp = malloc(sizeof(dentry));
	init_dstring(&(entryp->key), key, 0);
	entryp->value = value;
	entryp->key_hash_value = 0;
	return entryp;
}

dentry* get_dentry_transferred(dstring* key, void* value)
{
	dentry* entryp = malloc(sizeof(dentry));
	entryp->key = *key;
	key->cstring = NULL;
	entryp->value = value;
	entryp->key_hash_value = 0;
	return entryp;
}

int key_compare_dentry(const void* entry1, const void* entry2)
{
	return compare_dstring(&(((dentry*)entry1)->key), &(((dentry*)entry2)->key));
}

unsigned int key_hash_dentry(const void* entryp)
{
	if(((dentry*)entryp)->key_hash_value == 0)
		((dentry*)entryp)->key_hash_value = getHashValueDstring(&(((dentry*)entryp)->key));
	return ((dentry*)entryp)->key_hash_value;
}

void delete_dentry(dentry* entryp, void (*value_destroyer)(void* value))
{
	deinit_dstring(&(entryp->key));
	value_destroyer(entryp->value);
	free(entryp);
}