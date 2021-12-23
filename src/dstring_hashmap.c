#include<dstring_hashmap.h>

#include<stdlib.h>
#include<string.h>

void initialize_dmap(dmap* dmapp, dmap_key_type key_type, unsigned int size)
{
	dmapp->key_type = key_type;
	if(key_type == CASE_SENSITIVE_KEY_TYPE)
		initialize_hashmap(&(dmapp->map), ROBINHOOD_HASHING, size, key_hash_dentry, key_compare_dentry_CASE_SENSITIVE, 0);
	else if(key_type == CASE_INSENSITIVE_KEY_TYPE)
		initialize_hashmap(&(dmapp->map), ROBINHOOD_HASHING, size, key_hash_dentry, key_compare_dentry_CASE_INSENSITIVE, 0);
}

dstring* find_equals_in_dmap_cstr(dmap* dmapp, const char* key)
{
	return find_equals_in_dmap(dmapp, &get_literal_cstring(key));
}

dstring* find_equals_in_dmap(dmap* dmapp, const dstring* key)
{
	if(key == NULL)
		return NULL;

	dentry* dent = NULL;

	if(dmapp->key_type == CASE_SENSITIVE_KEY_TYPE)
		dent = (dentry*) find_equals_in_hashmap(&(dmapp->map), &((dentry){.key = *key}));
	else if(dmapp->key_type == CASE_INSENSITIVE_KEY_TYPE)
	{
		dstring copy_key = get_copy_dstring(key); toLowercase(&copy_key);
		dent = (dentry*) find_equals_in_hashmap(&(dmapp->map), &((dentry){.key = copy_key}));
		deinit_dstring(&copy_key);
	}

	return (dent != NULL) ? (&(dent->value)) : NULL;
}

static void rehash_and_expand_hashmap_if_necessary(dmap* dmapp)
{
	if(get_bucket_count_hashmap(&(dmapp->map)) < (1.5 * get_element_count_hashmap(&(dmapp->map))))
		expand_hashmap(&(dmapp->map), 1.5);
}

void insert_unique_in_dmap_cstr(dmap* dmapp, const char* key, const char* value)
{
	insert_unique_in_dmap(dmapp, &get_literal_cstring(key), &get_literal_cstring(value));
}

void insert_unique_in_dmap(dmap* dmapp, const dstring* key, const dstring* value)
{
	if(key == NULL || value == NULL)
		return;
	dstring* val_found = find_equals_in_dmap(dmapp, key);
	if(val_found != NULL)
	{
		make_dstring_empty(val_found);
		concatenate_dstring(val_found, value);
	}
	else
	{
		rehash_and_expand_hashmap_if_necessary(dmapp);
		dentry* dent = get_dentry(key, value);
		if(dmapp->key_type == CASE_INSENSITIVE_KEY_TYPE)
			toLowercase(&(dent->key));
		insert_in_hashmap(&(dmapp->map), dent);
	}
}

void insert_duplicate_in_dmap_cstr(dmap* dmapp, const char* key, const char* value)
{
	insert_duplicate_in_dmap(dmapp, &get_literal_cstring(key), &get_literal_cstring(value));
}

void insert_duplicate_in_dmap(dmap* dmapp, const dstring* key, const dstring* value)
{
	if(key == NULL || value == NULL)
		return;
	rehash_and_expand_hashmap_if_necessary(dmapp);
	dentry* dent = get_dentry(key, value);
	if(dmapp->key_type == CASE_INSENSITIVE_KEY_TYPE)
		toLowercase(&(dent->key));
	insert_in_hashmap(&(dmapp->map), dent);
}

int remove_from_dmap_cstr(dmap* dmapp, const char* key)
{
	return remove_from_dmap(dmapp, &get_literal_cstring(key));
}

int remove_from_dmap(dmap* dmapp, const dstring* key)
{
	if(key == NULL)
		return 0;

	dentry* dent = NULL;

	if(dmapp->key_type == CASE_SENSITIVE_KEY_TYPE)
		dent = (dentry*) find_equals_in_hashmap(&(dmapp->map), &((dentry){.key = *key}));
	else if(dmapp->key_type == CASE_INSENSITIVE_KEY_TYPE)
	{
		dstring copy_key = get_copy_dstring(key); toLowercase(&copy_key);
		dent = (dentry*) find_equals_in_hashmap(&(dmapp->map), &((dentry){.key = copy_key}));
		deinit_dstring(&copy_key);
	}

	if(dent != NULL && remove_from_hashmap(&(dmapp->map), dent))
	{
		delete_dentry(dent);
		return 1;
	}
	return 0;
}

static void delete_dentry_wrapper(const void* dent, const void* additional_params)
{
	delete_dentry(((dentry*)dent));
}

void remove_all_from_dmap(dmap* dmapp)
{
	for_each_in_hashmap(&(dmapp->map), delete_dentry_wrapper, NULL);
	for(unsigned int i = 0; i < get_bucket_count_hashmap(&(dmapp->map));i++)
		set_element(&(dmapp->map.hashmap_holder), NULL, i);
}

void for_each_in_dmap(dmap* dmapp, void (*operation)(const dstring* key, const dstring* value, const void* additional_params), const void* additional_params)
{
	for(unsigned int i = 0; i < get_bucket_count_hashmap(&(dmapp->map)); i++)
	{
		if(get_element(&(dmapp->map.hashmap_holder), i) != NULL)
		{
			const dentry* dent = get_element(&(dmapp->map.hashmap_holder), i);
			operation(&(dent->key), &(dent->value), additional_params);
		}
	}
}

void deinitialize_dmap(dmap* dmapp)
{
	remove_all_from_dmap(dmapp);
	deinitialize_hashmap(&(dmapp->map));
}