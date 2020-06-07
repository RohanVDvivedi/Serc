#include<dstring_entry.h>

dentry* get_dstring_entry(dstring* key, void* value)
{
	dentry* entryp = malloc(sizeof(dentry));
	entryp->key = key;
	entryp->value = value;
	entryp->key_hash_value = 0;
	return entryp;
}

int key_compare_dstring_entry(const void* entry1, const void* entry2)
{
	return compare_dstring(((dentry*)entry1)->key, ((dentry*)entry2)->key);
}

unsigned long long int key_hash_dstring_entry(const void* entryp)
{
	if(((dentry*)entryp)->key_hash_value == 0)
	{
		((dentry*)entryp)->key_hash_value = getHashValueDstring(((dentry*)entryp)->key);
	}
	return ((dentry*)entryp)->key_hash_value;
}

void delete_dstring_entry(dentry* entryp, void (*value_destroyer)(void* value))
{
	delete_dstring(entryp->key);
	value_destroyer(entryp->value);
	free(entryp);
}