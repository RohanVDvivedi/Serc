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

dstring* find_equals_in_dmap_cstr(dmap* dmapp, char* key)
{
	dstring dummy_key; init_dstring_slize(&dummy_key, key, strlen(key));
	return find_equals_in_dmap(dmapp, &dummy_key);
}

dstring* find_equals_in_dmap(dmap* dmapp, dstring* key)
{
	if(key == NULL)
		return NULL;
	dentry* dent = (dentry*) find_equals_in_hashmap(&(dmapp->map), &((dentry){.key = *key}));
	return (dent != NULL) ? (&(dent->value)) : NULL;
}

static void insert_elements_wrapper(const void* dent, const void* new_map)
{
	insert_in_hashmap(((hashmap*)(new_map)), ((dentry*)(dent)));
}

static void rehash_if_necessary(dmap* dmapp)
{
	if(dmapp->occupancy == dmapp->hashmap_holder.total_size)
	{
		hashmap new_map;
		initialize_hashmap(&(new_map), dmapp->hashmap_policy, dmapp->hashmap_holder.total_size * 2, dmapp->hash_function, dmapp->compare, 0);
		for_each_in_hashmap(dmapp, insert_elements_wrapper, &new_map);
		deinitialize_hashmap(dmapp);
		(*dmapp) = new_map;
	}
}

void insert_unique_in_dmap_cstr(dmap* dmapp, char* key, char* value)
{
	dstring dummy_key; init_dstring_slize(&dummy_key, key, strlen(key));
	dstring dummy_val; init_dstring_slize(&dummy_val, value, strlen(value));
	insert_unique_in_dmap(dmapp, &dummy_key, &dummy_val);
}

void insert_unique_in_dmap(dmap* dmapp, dstring* key, dstring* value)
{
	if(key == NULL || value == NULL)
		return;
	dentry* dent = (dentry*) find_equals_in_hashmap(dmapp, &((dentry){.key = *key}));
	if(dent != NULL)
	{
		deinit_dstring(&(dent->value));
		init_dstring_data(&(dent->value), value->cstring, value->bytes_occupied);
	}
	else
	{
		rehash_if_necessary(dmapp);
		insert_in_hashmap(dmapp, get_dentry(key, value));
	}
}

void insert_duplicate_in_dmap_cstr(dmap* dmapp, char* key, char* value)
{
	dstring dummy_key; init_dstring_slize(&dummy_key, key, strlen(key));
	dstring dummy_val; init_dstring_slize(&dummy_val, value, strlen(value));
	insert_duplicate_in_dmap(dmapp, &dummy_key, &dummy_val);
}

void insert_duplicate_in_dmap(dmap* dmapp, dstring* key, dstring* value)
{
	if(key == NULL || value == NULL)
		return;
	rehash_if_necessary(dmapp);
	insert_in_hashmap(dmapp, get_dentry(key, value));
}

int remove_from_dmap_cstr(dmap* dmapp, char* key)
{
	dstring dummy_key; init_dstring_slize(&dummy_key, key, strlen(key));
	return remove_from_dmap(dmapp, &dummy_key);
}

int remove_from_dmap(dmap* dmapp, dstring* key)
{
	if(key == NULL)
		return 0;
	dentry* dent = (dentry*) find_equals_in_hashmap(dmapp, &((dentry){.key = *key}));
	if(dent != NULL && remove_from_hashmap(dmapp, dent))
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
	for_each_in_hashmap(dmapp, delete_dentry_wrapper, NULL);
	for(unsigned int i = 0; i < dmapp->hashmap_holder.total_size;i++)
		set_element(&(dmapp->hashmap_holder), NULL, i);
}

void for_each_in_dmap(dmap* dmapp, void (*operation)(const dstring* key, const dstring* value, const void* additional_params), const void* additional_params)
{
	for(unsigned int i = 0; i < dmapp->hashmap_holder.total_size; i++)
	{
		if(get_element(&(dmapp->hashmap_holder), i) != NULL)
		{
			const dentry* dent = get_element(&(dmapp->hashmap_holder), i);
			operation(&(dent->key), &(dent->value), additional_params);
		}
	}
}

void deinitialize_dmap(dmap* dmapp)
{
	remove_all_from_dmap(dmapp);
	deinitialize_hashmap(dmapp);
}