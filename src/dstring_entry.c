#include<dstring_entry.h>

dstring_entry* get_dstring_entry(dstring* key, unsigned long long int value_size)
{
	dstring_entry* entryp = malloc(sizeof(dstring_entry));
	entryp->key = key;
	entryp->value = get_dstring("", value_size);
	entryp->key_hash_value;
	return entryp;
}

int key_compare_dstring_entry(const void* entry1, const void* entry2)
{
	return compare_dstring(((dstring_entry*)entry1)->key, ((dstring_entry*)entry2)->key);
}

int key_hash_dstring_entry(const void* entryp)
{
	return getHashValueDstring(((dstring_entry*)entryp)->key);
}

void delete_dstring_entry(dstring_entry* entryp)
{
	delete_dstring(entryp->key);
	delete_dstring(entryp->value);
	free(entryp);
}