#include<dstring_hashmap.h>

dmap* get_dmap(unsigned long long int size)
{
	hashmap* dmap = malloc(sizeof(hashmap));
	initialize_hashmap(dmap, ROBINHOOD_HASHING, size, key_hash_dentry, key_compare_dentry, 0);
	return dmap;
}

void* find_equals_in_dmap(dmap* dmapp, dstring* key)
{
	dentry temp_dentry;
	temp_dentry.key = key;
	dentry* dent = (dentry*) find_equals_in_hashmap(dmapp, &temp_dentry);
	return (dent != NULL) ? dent->value : NULL;
}

int insert_in_dmap(dmap* dmapp, dstring* key, void* value)
{
	return insert_in_hashmap(dmapp, get_dentry(key, value));
}

int insert_in_dmap_cstr(dmap* dmapp, char* key, void* value)
{
	return insert_in_dmap(dmapp, get_dstring(key, 10), value);
}

int remove_from_dmap(dmap* dmapp, dstring* key, void (*value_destroyer)(void*))
{
	dentry temp_dentry;
	temp_dentry.key = key;
	dentry* dent = (dentry*) find_equals_in_hashmap(dmapp, &temp_dentry);
	int removed = remove_from_hashmap(dmapp, dent);
	if(removed)
	{
		delete_dentry(dent, value_destroyer);
	}
	return removed;
}

static void queue_elements_wrapper(const void* dentry, const void* additional_params)
{
	push_queue(((queue*)additional_params), dentry);
}

void remove_all_from_dmap(dmap* dmapp, void (*value_destroyer)(void* value))
{
	queue elements_to_delete;
	initialize_queue(&elements_to_delete, dmapp->occupancy);
	for_each_in_hashmap(dmapp, queue_elements_wrapper, &elements_to_delete);
	while(!isQueueEmpty(&elements_to_delete))
	{
		dentry* dent = (dentry*) get_top_queue(&elements_to_delete);
		pop_queue(&elements_to_delete);

		// remove entry from hashmap
		remove_from_hashmap(dmapp, dent);
		// delete entry
		delete_dentry(dent, value_destroyer);
	}
	deinitialize_queue(&elements_to_delete);
}

void for_each_in_dmap(dmap* dmapp, void (*operation)(dstring* key, void* value, const void* additional_params), const void* additional_params)
{
	queue elements_to_delete;
	initialize_queue(&elements_to_delete, dmapp->occupancy);
	for_each_in_hashmap(dmapp, queue_elements_wrapper, &elements_to_delete);
	while(!isQueueEmpty(&elements_to_delete))
	{
		dentry* dent = (dentry*) get_top_queue(&elements_to_delete);
		pop_queue(&elements_to_delete);
		operation(dent->key, dent->value, additional_params);
	}
	deinitialize_queue(&elements_to_delete);
}

void delete_dmap(dmap* dmapp, void (*value_destroyer)(void* value))
{
	remove_all_from_dmap(dmapp, value_destroyer);
	deinitialize_hashmap(dmapp);
	free(dmapp);
}