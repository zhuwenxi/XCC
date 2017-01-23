#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <assert.h>

#include "production.h"

static char * prod_desc_table[] = 
{
#define PRODUCTION(prod, desc) desc, 
#include "production_token.def"
#undef PRODUCTION
};

production_t *
create_production(production_token_t head, production_token_t *body, unsigned int body_size)
{
	production_t *prod_ptr = malloc(sizeof(production_t));
	prod_ptr->head = head;
	prod_ptr->body = body;
	prod_ptr->body_size = body_size;
}

void
destroy_production(production_t *production_ptr)
{
	free(production_ptr->body);
	free(production_ptr);
}

void
print_production(production_t *prod)
{
	assert(prod != NULL);	
	
	// "print_str" will be used to hold the string literal to be printed.
	char *print_str;
	unsigned int print_str_len = 0;

	
	//
	// Calculate the storage size we should assigned for "print_str".
	// 
	production_token_t head = prod->head;
	char * head_str = prod_desc_table[head];
	print_str_len += strlen(head_str);

	production_token_t *body = prod->body;
	assert(body != NULL);

	// Restore 4 extra byte for " -> ".
	print_str_len += 4;
	int i;
	for (i = 0; i < prod->body_size; i ++)
	{
		production_token_t token = prod->body[i];
		char *body_str = prod_desc_table[token];
		print_str_len += strlen(body_str);

		if (i != 0)
		{
			// Restore 1 extra byte for " ".
			print_str_len += 1;
		}
	}

	// Restore 1 extra byte for "\0".
	print_str_len += 1;
	DBCODE(true, printf("print_str_len:%d\n", print_str_len));

	//
	// Start to fill in "print_str".
	// 
	print_str = (char *)malloc(print_str_len * sizeof(char));

	// Fill in production's head.
	strcpy(print_str, head_str);

	// Fill in "->".
	strcat(print_str, " -> ");

	// Fill in production's body.
	for (i = 0; i < prod->body_size; i++)
	{
		production_token_t token = prod->body[i];
		char *body_str = prod_desc_table[token];
		
		if (i != 0)
		{
			// Fill in " ".
			strcat(print_str, " ");	
		}

		// Fill in every token in production's body.
		strcat(print_str, body_str);
	}

	printf("%s\n", print_str);
}
