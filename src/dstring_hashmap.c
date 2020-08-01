#include<dstring_hashmap.h>

void initialize_dmap(dmap* dmapp, unsigned int size, void (*value_destroyer)(void* value))
{
	initialize_hashmap(&(dmapp->map), ROBINHOOD_HASHING, size, key_hash_dentry, key_compare_dentry, 0);
	dmapp->value_destroyer = value_destroyer;
}

void* find_equals_in_dmap_cstr(dmap* dmapp, char* key)
{
	if(key == NULL)
		return NULL;
	dentry* dent = (dentry*) find_equals_in_hashmap(&(dmapp->map), &((dentry){.key = {.cstring = key}}));
	return (dent != NULL) ? dent->value : NULL;
}

void* find_equals_in_dmap(dmap* dmapp, dstring* key)
{
	return find_equals_in_dmap_cstr(dmapp, key->cstring);
}

static void insert_elements_wrapper(const void* dent, const void* new_map)
{
	insert_in_hashmap(((hashmap*)(new_map)), ((dentry*)(dent)));
}

static void rehash_if_necessary(dmap* dmapp)
{
	if(dmapp->map.occupancy == dmapp->map.total_bucket_count)
	{
		hashmap new_map;
		initialize_hashmap(&(new_map), ROBINHOOD_HASHING, dmapp->map.total_bucket_count * 2, key_hash_dentry, key_compare_dentry, 0);

		for_each_in_hashmap(&(dmapp->map), insert_elements_wrapper, &new_map);

		deinitialize_hashmap(&(dmapp->map));

		dmapp->map = new_map;
	}
}

void insert_in_dmap_cstr(dmap* dmapp, char* key, void* value)
{
	if(key == NULL)
		return;
	dentry* dent = (dentry*) find_equals_in_hashmap(&(dmapp->map), &((dentry){.key = {.cstring = key}}));
	if(dent != NULL)
		dent->value = value;
	else
	{
		rehash_if_necessary(dmapp);
		insert_in_hashmap(&(dmapp->map), get_dentry(key, value));
	}
}

void insert_in_dmap(dmap* dmapp, dstring* key, void* value)
{
	if(key == NULL)
		return;
	dentry* dent = (dentry*) find_equals_in_hashmap(&(dmapp->map), &((dentry){.key = {.cstring = key->cstring}}));
	if(dent != NULL)
		dent->value = value;
	else
	{
		rehash_if_necessary(dmapp);
		insert_in_hashmap(&(dmapp->map), get_dentry_transferred(key, value));
	}
}

int remove_from_dmap(dmap* dmapp, dstring* key)
{
	if(key == NULL)
		return 0;
	dentry* dent = (dentry*) find_equals_in_hashmap(&(dmapp->map), &((dentry){.key = ((dstring){.cstring = key->cstring})}));
	int removed = 0;
	if(dent != NULL)
		removed = remove_from_hashmap(&(dmapp->map), dent);
	if(removed)
		delete_dentry(dent, dmapp->value_destroyer);
	return removed;
}

static void delete_dentry_wrapper(const void* dent, const void* value_destroyer)
{
	delete_dentry(((dentry*)dent), (void (*)(void*))value_destroyer);
}

void remove_all_from_dmap(dmap* dmapp)
{
	for_each_in_hashmap(&(dmapp->map), delete_dentry_wrapper, dmapp->value_destroyer);
	for(unsigned int i = 0; i < dmapp->map.total_bucket_count;i++)
		dmapp->map.holder[i] = NULL;
}

static void queue_elements_wrapper(const void* dentry, const void* additional_params)
{
	push_queue(((queue*)additional_params), dentry);
}

void for_each_in_dmap(dmap* dmapp, void (*operation)(dstring* key, void* value, const void* additional_params), const void* additional_params)
{
	queue elements_to_delete;
	initialize_queue(&elements_to_delete, dmapp->map.occupancy);
	for_each_in_hashmap(&(dmapp->map), queue_elements_wrapper, &elements_to_delete);
	while(!isQueueEmpty(&elements_to_delete))
	{
		dentry* dent = (dentry*) get_top_queue(&elements_to_delete);
		pop_queue(&elements_to_delete);
		operation(&(dent->key), dent->value, additional_params);
	}
	deinitialize_queue(&elements_to_delete);
}

void deinitialize_dmap(dmap* dmapp)
{
	remove_all_from_dmap(dmapp);
	deinitialize_hashmap(&(dmapp->map));
}