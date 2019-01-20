#include<c_util.h>

void print_tabs(unsigned long long int n )
{
	for(unsigned long long int i = 0 ; i < n ; i++ )
	{
		printf("\t");
	}
}

void primitive_toJson(char* target, void* data, Type_Support type)
{

}

void delete_multi_dim_intermediate(void** multi_p, unsigned long long int* dimensions, unsigned long long int dimensions_count, unsigned long long int current_level)
{
	if( multi_p != NULL )
	{
		if( current_level < dimensions_count - 1  )
		{
			for(unsigned long long int i = 0;i<dimensions[current_level];i++)
			{
				delete_multi_dim_intermediate( ((void**)(multi_p[i])) , dimensions, dimensions_count, current_level+1);
				multi_p[i] = NULL;
			}
		}
		free(multi_p);
	}
}

void delete_multi_dim(void* multi_p, unsigned long long int* dimensions, unsigned long long int dimensions_count)
{
	delete_multi_dim_intermediate( ((void**)(multi_p)), dimensions, dimensions_count, 0);
}

void* alloc_multi_dim_intermediate(unsigned long long int* dimensions, unsigned long long int dimensions_count, unsigned long long int current_level, size_t element_size)
{
	void** result;
	if( current_level < dimensions_count - 1  )
	{
		result = malloc( sizeof(void*) * dimensions[current_level] );
		for(unsigned long long int i = 0;i<dimensions[current_level];i++)
		{
			result[i] = alloc_multi_dim_intermediate(dimensions, dimensions_count, current_level+1, element_size);
		}

	}
	else
	{
		result = malloc( element_size * dimensions[current_level] );
	}
	return (void*)result;
}

void* alloc_multi_dim(unsigned long long int* dimensions, unsigned long long int dimensions_count, size_t element_size)
{
	return alloc_multi_dim_intermediate(dimensions, dimensions_count, 0, element_size);;
}

void apply_multi_dim_intermediate(void** multi_p,unsigned long long int* dimensions, unsigned long long int dimensions_count, unsigned long long int current_level, void (*apply)(void *) , size_t element_size)
{
	if( multi_p != NULL )
	{
		if( current_level < dimensions_count - 1 )
		{
			for(unsigned long long int i = 0;i<dimensions[current_level];i++)
			{
				apply_multi_dim_intermediate( ((void**)(multi_p[i])) , dimensions, dimensions_count, current_level + 1, apply, element_size);
			}
		}
		else
		{
			for(unsigned long long int i = 0;i<dimensions[current_level];i++)
			{
				(*apply)( (void*) ( ((char*)multi_p) + (i * element_size) ) );
			}
		}
	}
}

void apply_multi_dim(void* multi_p,unsigned long long int* dimensions, unsigned long long int dimensions_count, size_t element_size,void (*apply)(void *))
{
	apply_multi_dim_intermediate( ((void**)(multi_p)) , dimensions, dimensions_count, 0, apply, element_size);
}

void toJson_multi_dim_intermediate( JsonString* resultJS,void** multi_p,unsigned long long int* dimensions, unsigned long long int dimensions_count, unsigned long long int current_level, Type_Support type , size_t element_size)
{
	if( multi_p != NULL )
	{
		if( current_level < dimensions_count - 1 )
		{
			for(unsigned long long int i = 0;i<dimensions[current_level];i++)
			{
				toJson_multi_dim_intermediate( resultJS, ((void**)(multi_p[i])) , dimensions, dimensions_count, current_level + 1, type, element_size);
			}
		}
		else
		{
			for(unsigned long long int i = 0;i<dimensions[current_level];i++)
			{
				if( !is_combined(type) )
				{
					char elemental_result[10];
					primitive_toJson(elemental_result, ( (void*) ( ((char*)multi_p) + (i * element_size) ) ), type);
				}
				else
				{
					char* elemental_result = toJson( ( (void*) ( ((char*)multi_p) + (i * element_size) ) ) , type );
					if(elemental_result != NULL)
					{
						free(elemental_result);
					}
				}
			}
		}
	}
}

char* toJson_multi_dim(void* multi_p,unsigned long long int* dimensions, unsigned long long int dimensions_count, size_t element_size, Type_Support type )
{
	JsonString* JS = getJsonString( "" );
	toJson_multi_dim_intermediate( JS, ((void**)(multi_p)) , dimensions, dimensions_count, 0, type, element_size);
	char* result = JS->string;
	free(JS);
	return result;
}

