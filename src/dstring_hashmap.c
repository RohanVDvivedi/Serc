#include<dstring_hashmap.h>

void initialize_dmap(dmap* dmapp, unsigned int size)
{
	initialize_hashmap(dmapp, ROBINHOOD_HASHING, size, key_hash_dentry, key_compare_dentry, 0);
}

dstring* find_equals_in_dmap_cstr(dmap* dmapp, char* key)
{
	if(key == NULL)
		return NULL;
	dentry* dent = (dentry*) find_equals_in_hashmap(dmapp, &((dentry){.key = {.cstring = key}}));
	return (dent != NULL) ? (&(dent->value)) : NULL;
}

dstring* find_equals_in_dmap(dmap* dmapp, dstring* key)
{
	return find_equals_in_dmap_cstr(dmapp, key->cstring);
}

static void insert_elements_wrapper(const void* dent, const void* new_map)
{
	insert_in_hashmap(((hashmap*)(new_map)), ((dentry*)(dent)));
}

static void rehash_if_necessary(dmap* dmapp)
{
	if(dmapp->occupancy == dmapp->total_bucket_count)
	{
		hashmap new_map;
		initialize_hashmap(&(new_map), ROBINHOOD_HASHING, dmapp->total_bucket_count * 2, key_hash_dentry, key_compare_dentry, 0);
		for_each_in_hashmap(dmapp, insert_elements_wrapper, &new_map);
		deinitialize_hashmap(dmapp);
		(*dmapp) = new_map;
	}
}

void insert_in_dmap_cstr(dmap* dmapp, char* key, char* value)
{
	if(key == NULL)
		return;
	if(value == NULL)
		value = "";
	dentry* dent = (dentry*) find_equals_in_hashmap(dmapp, &((dentry){.key = {.cstring = key}}));
	if(dent != NULL)
	{
		deinit_dstring(&(dent->value));
		init_dstring(&(dent->value), value, 0);
	}
	else
	{
		rehash_if_necessary(dmapp);
		insert_in_hashmap(dmapp, get_dentry(key, value));
	}
}

void insert_in_dmap(dmap* dmapp, dstring* key, dstring* value)
{
	if(key == NULL)
		return;
	dentry* dent = (dentry*) find_equals_in_hashmap(dmapp, &((dentry){.key = {.cstring = key->cstring}}));
	if(dent != NULL)
	{
		if(value == NULL || value->cstring == NULL)
			make_dstring_empty(&(dent->value));
		else
			dent->value = *value;
	}
	else
	{
		rehash_if_necessary(dmapp);
		insert_in_hashmap(dmapp, get_dentry_transferred(key, value));
	}
}

int remove_from_dmap_cstr(dmap* dmapp, char* key)
{
	if(key == NULL)
		return 0;
	dentry* dent = (dentry*) find_equals_in_hashmap(dmapp, &((dentry){.key = ((dstring){.cstring = key})}));
	if(dent != NULL)
		return remove_from_hashmap(dmapp, dent);
	return 0;
}

int remove_from_dmap(dmap* dmapp, dstring* key)
{
	if(key == NULL)
		return 0;
	return remove_from_dmap_cstr(dmapp, key->cstring);
}

static void delete_dentry_wrapper(const void* dent, const void* additional_params)
{
	delete_dentry(((dentry*)dent));
}

void remove_all_from_dmap(dmap* dmapp)
{
	for_each_in_hashmap(dmapp, delete_dentry_wrapper, NULL);
	for(unsigned int i = 0; i < dmapp->total_bucket_count;i++)
		dmapp->holder[i] = NULL;
}

static void queue_elements_wrapper(const void* dentry, const void* additional_params)
{
	push_queue(((queue*)additional_params), dentry);
}

void for_each_in_dmap(dmap* dmapp, void (*operation)(dstring* key, dstring* value, const void* additional_params), const void* additional_params)
{
	queue elements_to_delete;
	initialize_queue(&elements_to_delete, dmapp->occupancy);
	for_each_in_hashmap(dmapp, queue_elements_wrapper, &elements_to_delete);
	while(!isQueueEmpty(&elements_to_delete))
	{
		dentry* dent = (dentry*) get_top_queue(&elements_to_delete);
		pop_queue(&elements_to_delete);
		operation(&(dent->key), &(dent->value), additional_params);
	}
	deinitialize_queue(&elements_to_delete);
}

void deinitialize_dmap(dmap* dmapp)
{
	remove_all_from_dmap(dmapp);
	deinitialize_hashmap(dmapp);
}