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

json_node* json_parse(char* json,json_error* error)
{
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

	while(json!='\0')
	{
		if(skip_char == 1)
		{
			json++;
			skip_char = 0;
			continue;
		}
		printf("---%c---\n",(*json));
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
				node->end_index = json;
				if( node->type != STRING_JSON && node->type != OBJECT_JSON && node->type != ARRAY_JSON )
				{
					node = node->parent;
				}
				else
				{
					node = node->parent->parent;
				}
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
				node->end_index = json;
				if( node->type != STRING_JSON && node->type != OBJECT_JSON && node->type != ARRAY_JSON )
				{
					node = node->parent->parent;
				}
				else
				{
					node = node->parent->parent->parent;
				}
				break;
			}
			case ':' :
			{
				if(node->type == OBJECT_JSON)
				{
					node = node->children[(node->child_count)-1];
					node->is_key = 1;
				}
				break;
			}
			case ',' :
			{
				if(node->type!=OBJECT_JSON || node->type!=ARRAY_JSON || node->type!=STRING_JSON)
				{
					node = node->parent;
					if( node->is_key == 1 && node->type == STRING_JSON )
					{
						node = node->parent;
					}
				}
				break;
			}
			case '\"' :
			{
				if(top(stack,&stack_count,stack_size) == (*json))
				{
					pop(stack,&stack_count,stack_size);
					node->end_index = json;
					node = node->parent;
				}
				else	// means it is starting quotation
				{
					push(stack,&stack_count,stack_size,(*json));
					json_node* new_node = get_new_json_node();
					new_node->type = STRING_JSON;
					new_node->start_index = json;
				}
				break;
			}
			case '\'' :
			{
				if(top(stack,&stack_count,stack_size) == (*json))
				{
					pop(stack,&stack_count,stack_size);
					node->end_index = json;
					node = node->parent;
				}
				else	// means it is starting quotation
				{
					push(stack,&stack_count,stack_size,(*json));
					json_node* new_node = get_new_json_node();
					new_node->type = STRING_JSON;
					new_node->start_index = json;
				}
				break;
			}
			default :
			{
				if(node->start_index == NULL)
				{
					node->start_index = json;
				}
				else
				{
					node->end_index = json;
				}
				if((*json)=='\\')
				{
					skip_char = 1;
				}
				break;
			}
		}
		json_print(root_node,0);
		printf("\n\n");
		json++;
	}

	free(stack);
	return root_node;
}

void print_n_spaces(int n)
{
	for(int i=0;i<n;i++)
	{
		printf(" ");
	}
}

void json_print(json_node* node,int n_spaces)
{
	print_n_spaces(n_spaces);
	if( node->is_key == 0 )
	{
		switch(node->type)
		{
			case ARRAY_JSON :
			{
				printf("[]%d->",node->child_count);
				for(int i=0;i<node->child_count;i++)
				{
					json_print(node->children[i],n_spaces+5);
				}
				break;
			}
			case OBJECT_JSON :
			{
				printf("{}%d->",node->child_count);
				for(int i=0;i<node->child_count;i++)
				{
					json_print(node->children[i],n_spaces+5);
				}
				break;
			}
			default :
			{
				char prev_end = (*(node->end_index));
				(*(node->end_index)) = '\0';
				printf("%s\n",node->end_index);
				(*(node->end_index)) = prev_end;
				break;
			}
		}
	}
	else
	{
		char prev_end = (*(node->end_index));
		(*(node->end_index)) = '\0';
		printf("key=%s=>\n",node->end_index);
		(*(node->end_index)) = prev_end;
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
	else if( ( parent->type == ARRAY_JSON && child->is_key == 0 ) || ( parent->type == OBJECT_JSON && child->is_key == 1 ) )
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
		}
	}
	else if(parent->is_key == 1 && parent->type == STRING_JSON)
	{
		remove_child(parent);
		remove_children(parent);
		parent->child = child;
	}
	else
	{
		return -1;
	}
	return 0;
}