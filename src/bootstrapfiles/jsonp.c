#include<jsonp.h>

json_error json_parse(char* json,json_error* error)
{
	json_node* node;

	return node;
}

void print_n_spaces(int n)
{
	for(int i=0;i<n;i++)
	{
		printf(" ");
	}
}

void json_print(json_node* node,char* json_str,int n_spaces)
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
					json_print(node->children[i],json_str,n_spaces+5);
				}
				break;
			}
			case OBJECT_JSON :
			{
				printf("{}%d->",node->child_count);
				for(int i=0;i<node->child_count;i++)
				{
					json_print(node->children[i],json_str,n_spaces+5);
				}
				break;
			}
			default :
			{
				char* prev_end = json_str[node->end_index];
				json_str[node->end_index] = '\0';
				printf("%s\n");
				json_str[node->end_index] = prev_end;
				break;
			}
		}
	}
	else
	{
		char* prev_end = json_str[node->end_index];
		json_str[node->end_index] = '\0';
		printf("key=%s=>\n");
		json_str[node->end_index] = prev_end;
	}
}

void json_delete(json_node* node)
{
	if(node == NULL)
	{
		return;
	}
	if(node->child != NULL)
	{
		json_delete(node->child);
	}
	if(node->children != NULL)
	{
		for(int i=0;i<node->child_count;i++)
		{
			json_delete(node->children[i]);
		}
		free(node->children);
	}
	return;
}