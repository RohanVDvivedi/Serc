#ifdef object

#define join(x, y) x ## y
#define concat(x,y) join(x, y)

#define array 		 concat(array_,object)

#define getArray     concat(getArray_,object)
#define get          concat(get_,object)
#define add          concat(add_,object)
#define add_all      concat(add_all_,object)
#define exists       concat(exists_,object)
#define remove       concat(remove_,object)
#define deleteArray  concat(deleteArray_,object)

typedef struct array array;
struct array
{
	unsigned long long int max_size;
	unsigned long long int size;
	object** objectList;
};

array* getArray()
{
	array* new_array = (array*) malloc(sizeof(array));
	new_array->max_size = 0;
	new_array->size = 0;
	new_array->objectList = NULL;
	return new_array;
}

object* get(array* array_p, unsigned long long int i)
{
	return (array_p!=NULL && array_p->size > i) ? array_p->objectList[i] : NULL;
}

void add(array* array_p,object* object_p)
{
	if(array_p!=NULL && object_p!=NULL)
	{
		object* new_obj = (object*) malloc(sizeof(object));
		*new_obj = *object_p;
		if(array_p->objectList!=NULL && array_p->size < array_p->max_size)
		{
			array_p->objectList[(array_p->size)++] = new_obj;
		}
		else
		{
			unsigned long long int new_max_size = array_p->max_size * 2;
			if(new_max_size == 0)
			{
				new_max_size = 3;
			}
			object** new_objectList = (object**) calloc(new_max_size,sizeof(object*));
			unsigned long long int i = 0;
			for(;i<array_p->size;i++)
			{
				new_objectList[i] = array_p->objectList[i];
			}
			new_objectList[i] = new_obj;
			array_p->objectList = new_objectList;
			array_p->max_size = new_max_size;
			array_p->size++; 
		}
	}
}

void add_all(array* array_p,object* object_p,unsigned long long int n)
{
	for(int i=0;i<n;i++)
	{
		add(array_p,object_p + i);
	}
}

void remove(array* array_p,object* object_p)
{
	if(array_p!=NULL && object_p!=NULL)
	{
		int state = 0;
		for(unsigned long long int i=0;i<array_p->size;i++)
		{
			switch(state)
			{
				case 0:
				{
					if( array_p->objectList[i] == object_p || (*(array_p->objectList[i])) == (*object_p) ) 
					{
						state = 1;
						free(array_p->objectList[i]);
					}
					break;
				}
				case 1:
				{
					array_p->objectList[i-1] = array_p->objectList[i];
					break;
				}
			}
		}
		if(state == 1)
		{
			array_p->objectList[array_p->size-1] = NULL;
			array_p->size-=1;
		}
	}
}

int exists(array* array_p,object* object_p)
{
	if(array_p!=NULL && object_p!=NULL)
	{
		for(unsigned long long int i=0;i<array_p->size;i++)
		{
			if( array_p->objectList[i] == object_p || (*(array_p->objectList[i])) == (*object_p) ) 
			{
				return 1;
			}
		}
		return 0;
	}
	else
	{
		return -1;
	}
}

void deleteArray(array* array_p)
{
	for(unsigned long long int i=0;i<array_p->size;i++)
	{
		free(array_p->objectList[i]);
	}
	free(array_p->objectList);
	free(array_p);
}

#undef object
#endif