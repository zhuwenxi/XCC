#include <stdlib.h>

#include "production.h"

production_t *
create_production(production_token_t head, production_token_t *body)
{
	production_t *prod_ptr = malloc(sizeof(production_t));
	prod_ptr->head = head;
	prod_ptr->body = body;
}

void
destroy_production(production_t *production_ptr)
{
	free(production_ptr->body);
	free(production_ptr);
}
