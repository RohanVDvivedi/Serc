#include<jsonp.h>

json_node* get_new_json_node()
{
	json_node* node = (json_node*) calloc(1,sizeof(json_node));
	node->children = NULL;
	node->child = NULL;
	node->parent = NULL;
	node->start_index = NULL;
	node->end_index = NULL;
	node->child_count = 0;
	node->child_container_size = 0;
	node->type = NULL_JSON;
	node->is_key = 0;
	node->string_hash = 0;
	return node;
}

// return 0 is success and return -1 is failure
int add_child(json_node* parent,json_node* child);

// return 0, -1 means error
int pop(char* stack,int* stack_count,int stack_size)
{
	if( (*stack_count) <= 0 || (*stack_count) > stack_size )
	{
		return -1;
	}
	(*stack_count)--;
	return 0;
}

char top(char* stack,int* stack_count,int stack_size)
{
	if( (*stack_count) <= 0 || (*stack_count) > stack_size )
	{
		return '\0';
	}
	return stack[(*stack_count)-1];
}

// return 0, -1 means error
int push(char* stack,int* stack_count,int stack_size,char push_char)
{
	if( (*stack_count) < 0 || (*stack_count) >= stack_size )
	{
		return -1;
	}
	stack[(*stack_count)++] = push_char;
	return 0;
}

json_node* get_non_key_parent_node(json_node* node);

int is_white_space(char c);

json_node* json_parse(char* json,json_error* error)
{
	char* string_base = json;

	json_node* root_node = NULL;
	json_node* node = root_node;

	int stack_size = 0;
	int stack_count = 0;

	char* temp = json;
	while( (*temp) != '\0' )
	{
		if( (*temp)!='{' || (*temp)!='[' || (*temp)!=']' || (*temp)!='}' || (*temp)!='\'' || (*temp)!='\"' || (*temp)!=':' )
		{
			stack_size++;
		}
		temp++;
	}
	char* stack = (char*) malloc(sizeof(char)*stack_size);

	int skip_char = 0;

	while((*json)!='\0')
	{
		if( is_white_space((*json)) )
		{
			json++;
			continue;
		}
		if( (top(stack,&stack_count,stack_size) == '\"' && (*json) != '\"' ) || ( top(stack,&stack_count,stack_size) == '\'' && (*json) != '\'' ) || skip_char == 1)
		{
			skip_char = 0;
			goto DEFAULT_CASE;
		}
		switch( (*json) )
		{
			case '[' :
			{
				push(stack,&stack_count,stack_size,(*json));
				json_node* new_node = get_new_json_node();
				new_node->type = ARRAY_JSON;
				new_node->start_index = json;
				add_child(node,new_node);
				node = new_node;
				if(root_node == NULL)
				{
					root_node = node;
				}
				break;
			}
			case ']' :
			{
				pop(stack,&stack_count,stack_size);
				if(node->type == NULL_JSON)
				{
					node = node->parent;
				}
				// at this point the node points to current ARRAY_JSON node
				node->end_index = json;
				// then we go to its parent
				node = get_non_key_parent_node(node);
				break;
			}
			case '{' :
			{
				push(stack,&stack_count,stack_size,(*json));
				json_node* new_node = get_new_json_node();
				new_node->type = OBJECT_JSON;
				new_node->start_index = json;
				add_child(node,new_node);
				node = new_node;
				if(root_node == NULL)
				{
					root_node = node;
				}
				break;
			}
			case '}' :
			{
				pop(stack,&stack_count,stack_size);
				if(node->type == NULL_JSON)
				{
					node = get_non_key_parent_node(node);
				}
				// by now the node points to the current OBJECT_JSON node
				node->end_index = json;
				// by here the node is now at the corresponding OBJECT_JSON type node whose all children we all jsu saw
				// so now we throw in some code that tests if the last element of the object is a key value
				if( node->type == OBJECT_JSON && node->children[node->child_count-1]->is_key == 0 )
				{
					(*error) = OBJECT_ELEMENT_HAS_TO_BE_KEY_VALUE;
					goto END;
				}
				// then we just go to non key parent of the node
				node = get_non_key_parent_node(node);
				break;
			}
			case ':' :
			{
				if(node->type == OBJECT_JSON)
				{
					// pick up the last node and we know that this thing is a key
					node = node->children[(node->child_count)-1];
					node->is_key = 1;
					// since it is key it has to be a STRING_JSON type
					// we reach here when it is ARRAY_JSON or OBJECT_JSON
					if(node->type != STRING_JSON)
					{
						(*error) = KEY_HAS_TO_BE_STRING;
						goto END;
					}
				}
				else if(node->type == ARRAY_JSON)
				{
					(*error) = ARRAY_ELEMENT_CAN_NOT_BE_KEY_VALUE;
					goto END;
				}
				// below condition evaluates when key is non string
				// we reach here when it is NULL, TRUE, FALSE, or a number etc
				else if( node->type == NULL_JSON)
				{
					node->end_index = json - 1;
					// since it is key it has to be a STRING_JSON type
					if(node->type != STRING_JSON)
					{
						(*error) = KEY_HAS_TO_BE_STRING;
						goto END;
					}
				}
				break;
			}
			case ',' :
			{
				// below block basically brings us out of everythi9ng into block in which corresponding , belongs
				if( node->type==NULL_JSON )
				{
					node->end_index = json - 1;
					node = get_non_key_parent_node(node);
				}

				// since we are already out now we test if the last added child is as per requirement of the ARRAY_JSON or OBJECT_JSON respectively
				if( node->type == ARRAY_JSON && node->children[node->child_count-1]->is_key == 1 )
				{
					(*error) = ARRAY_ELEMENT_CAN_NOT_BE_KEY_VALUE;
					goto END;
				}
				else if( node->type == OBJECT_JSON && node->children[node->child_count-1]->is_key == 0 )
				{
					(*error) = OBJECT_ELEMENT_HAS_TO_BE_KEY_VALUE;
					goto END;
				}
				break;
			}
			case '\"' :
			{
				if(top(stack,&stack_count,stack_size) == (*json))
				{
					pop(stack,&stack_count,stack_size);
					node->end_index = json;
					node = get_non_key_parent_node(node);
				}
				else	// means it is starting quotation
				{
					push(stack,&stack_count,stack_size,(*json));
					json_node* new_node = get_new_json_node();
					new_node->type = STRING_JSON;
					new_node->start_index = json;
					add_child(node,new_node);
					node = new_node;
				}
				break;
			}
			case '\'' :
			{
				if(top(stack,&stack_count,stack_size) == (*json))
				{
					pop(stack,&stack_count,stack_size);
					node->end_index = json;
					node = get_non_key_parent_node(node);
				}
				else	// means it is starting quotation
				{
					push(stack,&stack_count,stack_size,(*json));
					json_node* new_node = get_new_json_node();
					new_node->type = STRING_JSON;
					new_node->start_index = json;
					add_child(node,new_node);
					node = new_node;
				}
				break;
			}
			default :
			{
				DEFAULT_CASE :
				if( (node->is_key == 1 && ( (*node->end_index) == (*node->start_index) ) ) || (node->is_key == 0 && (node->type == ARRAY_JSON || node->type == OBJECT_JSON) ) )
				{
					json_node* new_node = get_new_json_node();
					add_child(node,new_node);
					node = new_node;
				}
				if(node->start_index == NULL)
				{
					node->start_index = json;
				}
				node->end_index = json;
				if( (*json) == '\\' )
				{
					skip_char = 1;
				}
				break;
			}
		}
		json++;
	}

	END: free(stack);
	return root_node;
}

json_node* get_non_key_parent_node(json_node* node)
{
	json_node* result = node;
	if(result != NULL && result->parent != NULL)
	{
		result = result->parent;
		if(result->is_key==1 && result->type==STRING_JSON && result->parent!=NULL)
		{
			result = result->parent;
		}
	}
	return result;
}

void print_n_spaces(int n)
{
	for(int i=0;i<n;i++)
	{
		printf(" ");
	}
}

unsigned long long int get_digits_count(unsigned long long int n)
{
	if(n==0)
	{
		return 1;
	}
	unsigned long long int digits = 0;
	while(n>0)
	{
		n = n/10;
		digits++;
	}
	return digits;
}

void json_print(json_node* node,int n_spaces)
{
	if( node->is_key == 0 )
	{
		switch(node->type)
		{
			case ARRAY_JSON :
			{
				if( node->parent != NULL && node->parent->type == ARRAY_JSON && node!=node->parent->children[0] )
				{
					print_n_spaces(n_spaces);
				}
				printf("%c%c%d->",(*(node->start_index)),(*(node->end_index)),node->child_count);
				for(int i=0;i<node->child_count;i++)
				{
					int sub_object_spaces = ( (i == 0) ? 0 : n_spaces+4+get_digits_count(node->child_count) );
					json_print(node->children[i],sub_object_spaces);
				}
				break;
			}
			case OBJECT_JSON :
			{
				if( node->parent != NULL && node->parent->type == ARRAY_JSON && node!=node->parent->children[0] )
				{
					print_n_spaces(n_spaces);
				}
				printf("%c%c%d->",(*(node->start_index)),(*(node->end_index)),node->child_count);
				for(int i=0;i<node->child_count;i++)
				{
					int sub_object_spaces = ( (i == 0) ? 0 : n_spaces+4+get_digits_count(node->child_count) );
					json_print(node->children[i],sub_object_spaces);
				}
				break;
			}
			default :
			{
				if( node->parent->type == ARRAY_JSON && node!=node->parent->children[0] )
				{
					print_n_spaces(n_spaces);
				}
				if(node->end_index != NULL)
				{
					char prev_end = (*(node->end_index+1));
					(*(node->end_index+1)) = '\0';
					if(node->type == STRING_JSON)
					{
						printf("%s=(%llu)\n",node->start_index,node->string_hash);
					}
					else
					{
						printf("%s\n",node->start_index);
					}
					(*(node->end_index+1)) = prev_end;
				}
				else
				{
					printf("%s\n","uky");
				}
				break;
			}
		}
	}
	else
	{
		print_n_spaces(n_spaces);
		if(node->end_index != NULL)
		{
			char prev_end = (*(node->end_index+1));
			(*(node->end_index+1)) = '\0';
			printf("key=%s=(%llu)=>",node->start_index,node->string_hash);
			if(node->child!=NULL)
			{
				json_print(node->child,n_spaces + 9 + (node->end_index-node->start_index+1) + get_digits_count(node->string_hash) );
			}
			else
			{
				printf("\n");
			}
			(*(node->end_index+1)) = prev_end;
		}
		else
		{
			printf("key=%s=>\n","uky");
		}
	}
}

void json_reevaluate(json_node* node)
{
	if(node==NULL)
	{
		return;
	}
	switch(node->type)
	{
		case OBJECT_JSON :
		case ARRAY_JSON :
		{
			for(int i=0;i<node->child_count;i++)
			{
				json_reevaluate(node->children[i]);
			}
			break;
		}
		case STRING_JSON :
		{
			node->string_hash = getHashValueByLength(node->start_index + 1,node->end_index - node->start_index - 1);
			if(node->is_key == 1)
			{
				json_reevaluate(node->child);
			}
			break;
		}
		case NULL_JSON :
		{
			char prev_end = (*(node->end_index+1));
			(*(node->end_index+1)) = '\0';
			if(strcmp("null",node->start_index)==0)
			{
				node->type = NULL_JSON;
			}
			else if(strcmp("true",node->start_index)==0)
			{
				node->type = TRUE_JSON;
			}
			else if(strcmp("false",node->start_index)==0)
			{
				node->type = FALSE_JSON;
			}
			else
			{

			}
			(*(node->end_index+1)) = prev_end;
			break;
		}
		default :
		{
		}
	}
}

void remove_child(json_node* node)
{
	if(node->child != NULL)
	{
		json_delete(node->child);
	}
	node->child = NULL;
}

void remove_children(json_node* node)
{
	if(node->children != NULL)
	{
		for(int i=0;i<node->child_count;i++)
		{
			if(node->children[i] != NULL)
			{
				json_delete(node->children[i]);
			}
			node->children[i] = NULL;
		}
		node->child_count = 0;
		if( node->child_container_size > 0 && node->children != NULL)
		{
			free(node->children);
		}
		node->child_container_size = 0;
		node->children = NULL;
	}
}

void json_delete(json_node* node)
{
	if(node == NULL)
	{
		return;
	}
	remove_child(node);
	remove_children(node);
	free(node);
	return;
}

// return 0 is success and return -1 is failure
int add_child(json_node* parent,json_node* child)
{
	if( parent==NULL || child==NULL )
	{
		return -1;
	}
	else if( ( parent->type == ARRAY_JSON && child->is_key == 0 ) || ( parent->type == OBJECT_JSON ) )
	{
		remove_child(parent);
		if(parent->child_container_size <= parent->child_count || parent->child_container_size == 0)
		{
			int new_child_container_size = 2 * parent->child_container_size + 3;
			json_node** new_children = (json_node**) calloc(new_child_container_size,sizeof(json_node*));
			for(int i=0;i<parent->child_count;i++)
			{
				new_children[i] = parent->children[i];
			}
			if(parent->children != NULL)
			{
				free(parent->children);
			}
			parent->children = new_children;
			parent->child_container_size = new_child_container_size;
		}
		if(parent->child_container_size > parent->child_count)
		{
			parent->children[parent->child_count++] = child;
			child->parent = parent;
		}
	}
	else if(parent->is_key == 1 && parent->type == STRING_JSON)
	{
		remove_child(parent);
		remove_children(parent);
		parent->child = child;
		child->parent = parent;
	}
	else
	{
		return -1;
	}
	return 0;
}

char error_strings[5][47] = {
	"no error",
	"unknown error",
	"array element can not be key value pair type",
	"object element must be key value pair type",
	"key has to be strinh value"
};

char json_type_strings[7][13] = {
	"STRING_JSON",
	"NUMBER_JSON",
	"NULL_JSON",
	"TRUE_JSON",
	"FALSE_JSON",
	"ARRAY_JSON",
	"OBJECT_JSON"
};

int is_white_space(char c)
{
	return c == ' ' || c == '\t' || c == '\n' || c == '\v' || c == '\f' || c == '\r';
}