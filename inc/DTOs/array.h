#define object int

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

void deleteArray(array* array)
{
	for(int i=0;i<array->size;i++)
	{
		free(array->objectList[i]);
	}
	free(array->objectList);
	free(array);
}