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
	PROD_TOK_REGEXP,
	PROD_TOK_ALTER,
	PROD_TOK_CONCAT,
	PROD_TOK_REPEAT,
	PROD_TOK_PRIMITIVE,
	PROD_TOK_LEFT_PARENTHESIS,
	PROD_TOK_RIGHT_PARENTHESIS,
	PROD_TOK_STAR_SYM,
} production_token_t;

/*
 * Prodution, for example:
 * Alter -> Alter | Concat
 */ 
typedef struct 
{
	production_token_t head;
	production_token_t *body;
} production_t;

production_t *
create_production(production_token_t head, production_token_t *body);

void
destroy_production(production_t *production_ptr);

#endif
