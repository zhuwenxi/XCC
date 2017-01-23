#ifndef __PRODUCTION_H__
#define __PRODUCTION_H__

#include "../util.h"

/*
 * Production token, including:
 * "Regexp"
 * "Alter:
 * "Concat"
 * "Repeat"
 * "Primitive"
 * "("
 * ")"
 * "*"
 *
 */ 
typedef enum 
{
#define PRODUCTION(prod, desc) prod,
#include "production_token.def"
PRODUCTION_LIMI,
#undef PRODUCTION
} production_token_t;




/*
 * Prodution, for example:
 * Alter -> Alter | Concat
 */ 
typedef struct 
{
	production_token_t head;
	production_token_t *body;
	unsigned int body_size;
} production_t;




/*
 * Construction & de-contruction functions for production_t
 */
production_t *
create_production(production_token_t head, production_token_t *body, unsigned int body_size);

void
destroy_production(production_t *production_ptr);


/*
 * Print a production
 */
void
print_production(production_t *prod);

#endif
