#include<dstring_hashmap.h>

void initialize_dmap(dmap* dmapp, unsigned long long int size, void (*value_destroyer)(void* value))
{
	initialize_hashmap(&(dmapp->map), ROBINHOOD_HASHING, size, key_hash_dentry, key_compare_dentry, 0);
	dmapp->value_destroyer = value_destroyer;
}

void* find_equals_in_dmap(dmap* dmapp, dstring* key)
{
	if(key == NULL)
	{
		return NULL;
	}
	dentry temp_dentry;
	temp_dentry.key = key;
	temp_dentry.key_hash_value = 0;
	dentry* dent = (dentry*) find_equals_in_hashmap(&(dmapp->map), &temp_dentry);
	return (dent != NULL) ? dent->value : NULL;
}

void* find_equals_in_dmap_cstr(dmap* dmapp, char* key)
{
	if(key == NULL)
	{
		return NULL;
	}
	dstring* key_dstr = get_dstring(key, 0);
	void* value = find_equals_in_dmap(dmapp, key_dstr);
	delete_dstring(key_dstr);
	return value;
}

static void queue_elements_wrapper(const void* dentry, const void* additional_params)
{
	push_queue(((queue*)additional_params), dentry);
}

static void rehash_if_necessary(dmap* dmapp)
{
	if(dmapp->map.occupancy == dmapp->map.total_bucket_count)
	{
		hashmap new_map;
		initialize_hashmap(&(new_map), ROBINHOOD_HASHING, dmapp->map.total_bucket_count * 2, key_hash_dentry, key_compare_dentry, 0);

		queue elements_to_delete;
		initialize_queue(&elements_to_delete, dmapp->map.occupancy);

		for_each_in_hashmap(&(dmapp->map), queue_elements_wrapper, &elements_to_delete);

		while(!isQueueEmpty(&elements_to_delete))
		{
			dentry* dent = (dentry*) get_top_queue(&elements_to_delete);
			pop_queue(&elements_to_delete);

			remove_from_hashmap(&(dmapp->map), dent);
			insert_in_hashmap(&(new_map), dent);
		}

		deinitialize_queue(&elements_to_delete);

		deinitialize_hashmap(&(dmapp->map));

		dmapp->map = new_map;
	}
}

int insert_in_dmap(dmap* dmapp, dstring* key, void* value)
{
	if(key == NULL)
	{
		return 0;
	}
	remove_from_dmap(dmapp, key);
	int inserted = 1;
	if(value != NULL)
	{
		rehash_if_necessary(dmapp);
		dentry* dent = get_dentry(key, value);
		inserted = insert_in_hashmap(&(dmapp->map), dent);
		if(!inserted)
		{
			free(dent);
		}
	}
	return inserted;
}

int insert_in_dmap_cstr(dmap* dmapp, char* key, void* value)
{
	dstring* key_dstr = get_dstring(key, 10);
	int inserted = insert_in_dmap(dmapp, key_dstr, value);
	if(!inserted)
	{
		delete_dstring(key_dstr);
	}
	return inserted;
}

int remove_from_dmap(dmap* dmapp, dstring* key)
{
	dentry temp_dentry;
	temp_dentry.key = key;
	temp_dentry.key_hash_value = 0;
	dentry* dent = (dentry*) find_equals_in_hashmap(&(dmapp->map), &temp_dentry);
	int removed = 0;
	if(dent != NULL)
	{
		removed = remove_from_hashmap(&(dmapp->map), dent);
	}
	if(removed)
	{
		delete_dentry(dent, dmapp->value_destroyer);
	}
	return removed;
}

void remove_all_from_dmap(dmap* dmapp)
{
	queue elements_to_delete;
	initialize_queue(&elements_to_delete, dmapp->map.occupancy);
	for_each_in_hashmap(&(dmapp->map), queue_elements_wrapper, &elements_to_delete);
	while(!isQueueEmpty(&elements_to_delete))
	{
		dentry* dent = (dentry*) get_top_queue(&elements_to_delete);
		pop_queue(&elements_to_delete);

		if(dent != NULL)
		{
			// remove entry from hashmap
			remove_from_hashmap(&(dmapp->map), dent);
			// delete entry
			delete_dentry(dent, dmapp->value_destroyer);
		}
	}
	deinitialize_queue(&elements_to_delete);
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
		operation(dent->key, dent->value, additional_params);
	}
	deinitialize_queue(&elements_to_delete);
}

void deinitialize_dmap(dmap* dmapp)
{
	remove_all_from_dmap(dmapp);
	deinitialize_hashmap(&(dmapp->map));
}