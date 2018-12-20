#include<jsonp.h>

json_node* get_new_json_node()
{
	json_node* node = (json_node*) calloc(1,sizeOf(json_node));
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

json_node* json_parse(char* json,json_error* error)
{
	json_node* node = NULL;



	return node;
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
		remove_child(node);
		if(node->child_container_size <= node->child_count || node->child_container_size == 0)
		{
			int new_child_container_size = 2 * node->child_container_size + 3;
			json_node** new_children = (json_node**) calloc(new_child_container_size,sizeof(json_node*));
			for(int i=0;i<node->child_count;i++)
			{
				new_children[i] = node->children[i];
			}
			if(node->children != NULL)
			{
				free(node->children);
			}
			node->children = new_children;
			node->child_container_size = new_child_container_size;
		}
		if(node->child_container_size > node->child_count)
		{
			node->children[node->child_count++] = child;
		}
	}
	else if(parent->is_key == 1 && parent->type == STRING_JSON)
	{
		remove_child(node);
		remove_children(node);
		node->child = child;
	}
	else
	{
		return -1;
	}
}