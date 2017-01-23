#include <stdio.h>
#include <stdlib.h>
#include "regexp.h"

void regexp_main()
{
	production_token_t head = PROD_TOK_REPEAT;
	unsigned int body_size = 2;
	
	production_token_t *body = (production_token_t *)malloc(body_size * sizeof(production_token_t));
	body[0] = PROD_TOK_CONCAT;
	body[1] = PROD_TOK_REPEAT;
	production_t *prod_ptr = create_production(head, body, body_size);

	print_production(prod_ptr);

	destroy_production(prod_ptr);
	return;
}
