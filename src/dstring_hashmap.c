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
	return find_equals_in_dmap(dmapp, dstring_DUMMY_CSTRING(key));
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
		dstring copy_key; init_dstring(&copy_key, key->cstring, key->bytes_occupied); toLowercase(&copy_key);
		dent = (dentry*) find_equals_in_hashmap(&(dmapp->map), &((dentry){.key = copy_key}));
		deinit_dstring(&copy_key);
	}

	return (dent != NULL) ? (&(dent->value)) : NULL;
}

static void insert_elements_wrapper(const void* dent, const void* new_map)
{
	insert_in_hashmap(((hashmap*)(new_map)), ((dentry*)(dent)));
}
static void rehash_if_necessary(dmap* dmapp)
{
	if(dmapp->map.occupancy == dmapp->map.hashmap_holder.total_size)
	{
		hashmap new_map;
		initialize_hashmap(&(new_map), dmapp->map.hashmap_policy, dmapp->map.hashmap_holder.total_size * 2, dmapp->map.hash_function, dmapp->map.compare, 0);
		for_each_in_hashmap(&(dmapp->map), insert_elements_wrapper, &new_map);
		deinitialize_hashmap(&(dmapp->map));
		dmapp->map = new_map;
	}
}

void insert_unique_in_dmap_cstr(dmap* dmapp, const char* key, const char* value)
{
	insert_unique_in_dmap(dmapp, dstring_DUMMY_CSTRING(key), dstring_DUMMY_CSTRING(value));
}

void insert_unique_in_dmap(dmap* dmapp, const dstring* key, const dstring* value)
{
	if(key == NULL || value == NULL)
		return;
	dstring* val_found = find_equals_in_dmap(dmapp, key);
	if(val_found != NULL)
	{
		deinit_dstring(val_found);
		init_dstring(val_found, value->cstring, value->bytes_occupied);
	}
	else
	{
		rehash_if_necessary(dmapp);
		dentry* dent = get_dentry(key, value);
		if(dmapp->key_type == CASE_INSENSITIVE_KEY_TYPE)
			toLowercase(&(dent->key));
		insert_in_hashmap(&(dmapp->map), dent);
	}
}

void insert_duplicate_in_dmap_cstr(dmap* dmapp, const char* key, const char* value)
{
	insert_duplicate_in_dmap(dmapp, dstring_DUMMY_CSTRING(key), dstring_DUMMY_CSTRING(value));
}

void insert_duplicate_in_dmap(dmap* dmapp, const dstring* key, const dstring* value)
{
	if(key == NULL || value == NULL)
		return;
	rehash_if_necessary(dmapp);
	dentry* dent = get_dentry(key, value);
	if(dmapp->key_type == CASE_INSENSITIVE_KEY_TYPE)
		toLowercase(&(dent->key));
	insert_in_hashmap(&(dmapp->map), dent);
}

int remove_from_dmap_cstr(dmap* dmapp, const char* key)
{
	return remove_from_dmap(dmapp, dstring_DUMMY_CSTRING(key));
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
		dstring copy_key; init_dstring(&copy_key, key->cstring, key->bytes_occupied); toLowercase(&copy_key);
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
	for(unsigned int i = 0; i < dmapp->map.hashmap_holder.total_size;i++)
		set_element(&(dmapp->map.hashmap_holder), NULL, i);
}

void for_each_in_dmap(dmap* dmapp, void (*operation)(const dstring* key, const dstring* value, const void* additional_params), const void* additional_params)
{
	for(unsigned int i = 0; i < dmapp->map.hashmap_holder.total_size; i++)
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