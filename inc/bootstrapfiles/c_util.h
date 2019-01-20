#ifndef C_UTIL_H
#define C_UTIL_H

#include<stdlib.h>
#include<stdio.h>
#include<combined.h>

// by the following given functions n dimentional arrays
// of any structure can be initialized and deleted

// deletes n dimentional arrays
// example for n dimentional array of struct type location
// multi_p is of type struct*** where there are n number of asterisk
// the dimensions is an array of unsigned long long int of length n
// tht dimensions_count is same as the dimensions of the multi_p array
// dimensions_count is same as length of array dimensions
// each of the dimensions array element denotes the dimension size of the
// multi_p multidimensional array

void delete_multi_dim(void* multi_p, unsigned long long int* dimensions, unsigned long long int dimensions_count);

void* alloc_multi_dim(unsigned long long int* dimensions, unsigned long long int dimensions_count, size_t element_size);

void apply_multi_dim(void* multi_p,unsigned long long int* dimensions, unsigned long long int dimensions_count, size_t element_size, void (*apply)(void *) );

char* toJson_multi_dim(void* multi_p,unsigned long long int* dimensions, unsigned long long int dimensions_count, size_t element_size, Type_Support type );

// example 
/*
struct localtion
{
	double lat,lon;	
};
lets say we need a 3 diomensional array of location struct dimensions are 3,5,2
hence 
dimensions = [ 3, 5, 2]
dimensions_count = 3
element_size = sizeof(location)
alloc_multi_dim function returns void* which has to be held by the callie in location*** variable
if we received location*** l
l[i][j][k].lat will be the value of latitude of location structure at index i, j, k
0 <= i < 3
0 <= j < 5
0 <= k < 2 are the index ranges as per dimensions array
the same l is to be passed to delete_multi_dim along with required variables to delete the allocated memory
*/

void print_n_tabs(unsigned long long int n );

// target array has to be of size >= 10, we will not be dealing with number not being handled by the c language
void primitive_toJson(char* target, void* data, Type_Support type);

#endif