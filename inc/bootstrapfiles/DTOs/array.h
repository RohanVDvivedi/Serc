#ifdef object

#define join(x, y) x ## _ ## y
#define concat(x,y) join(x, y)

#define array 		 concat(array_,object)

#define getArray     concat(getArray_,object)
#define get          concat(get_,object)
#define add          concat(add_,object)
#define exists       concat(exists_,object)
#define remove       concat(remove_,object)
#define deleteArray  concat(deleteArray_,object)

typedef struct array array;
struct array
{
	unsigned long long int maxSize;
	unsigned long long int size;
	object** objectList;
};

array getArray()
{
	array* newArray = (array*) malloc(sizeof(array));
	newArray->maxsize = 0;
	newArray->size = 0;
	newArray->objectList = null;
}

object* get(array* array, int i)
{
	return array->size > i ? array->objectList[i] : NULL;
}

void add(array* array,object* object)
{
	if(array!=null && object!=null)
	{
		object* newObj = (object*) malloc(sizeof(object));
		*newObj = *object;
		if(array->objectList!=null && array->size < array->maxSize)
		{
			array->objectList[++size] = newObj;
		}
		else
		{
			int newMaxSize = maxSize * 2;
			if(newMaxSize == 0)
			{
				newMaxSize = 3;
			}
			object** newObjList = (object**) malloc(sizeof(object*));
			int i;
			for(i=0;i<size;i++)
			{
				newObjList[i] = array->objectList[i];
			}
			newObjList[i] = newObj;
			array->objectList = newObjList;
			array->newMaxSize = maxSize;
			array->size++; 
		}
	}
}

void remove(array* array,object* object)
{
	if(array!=null && object!=null)
	{
		int state = 0;
		for(int i=0;i<array->size;i++)
		{
			switch(state)
			{
				case 0:
				{
					if( array->objectList[i] == object || (*(array->objectList[i])) == (*object) ) 
					{
						state = 1;
						free(array->objectList[i]);
					}
					break;
				}
				case 1:
				{
					array->objectList[i-1] = array->objectList[i];
					break;
				}
			}
		}
		array->size-=1;
	}
}

int exists(array* array,object* object)
{
	if(array!=null && object!=null)
	{
		for(int i=0;i<array->size;i++)
		{
			if( array->objectList[i] == object || (*(array->objectList[i])) == (*object) ) 
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

void deleteArray(array* array)
{
	for(int i=0;i<array->size;i++)
	{
		free(array->objectList[i]);
	}
	free(array->objectList);
	free(array);
}

#undef object