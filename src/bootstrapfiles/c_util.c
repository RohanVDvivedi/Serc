#include<c_util.h>

void delete_multi_dim_intermediate(void** multi_p, unsigned long long int* dimensions, unsigned long long int dimensions_count, unsigned long long int current_level)
{
	if( current_level < dimensions_count - 1  )
	{
		for(unsigned long long int i = 0;i<dimensions[current_level];i++)
		{
			if( multi_p[i] != NULL )
			{
				delete_multi_dim_intermediate( ((void**)(multi_p[i])) , dimensions, dimensions_count, current_level+1);
				multi_p[i] = NULL;
			}
		}
	}
	free(multi_p);
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