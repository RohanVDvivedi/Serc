#ifndef COMBINED_H
#define COMBINED_H

#include<type_support.h>

#include<array_json.h>
//#include<classname_json.h>
#include<MyObject_json.h>
#include<MyObjectSub_json.h>

char* toJson(void* data,Type_Support type);

void* fromJson(json_node* json,Type_Support type);

void* get(Type_Support type);

void del(void* data,Type_Support type);

int is_combined(Type_Support type);

void initialize_attributes(void* data,Type_Support type);

void delete_attributes(void* data,Type_Support type);

void* get_array(unsigned long long int n,Type_Support type);

void del_array(void* data,unsigned long long int n,Type_Support type);

#endif