#ifndef __OPTS_H__
#define __OPTS_H__


#include "stddefs.h"

#define UTIL_LOG_ENABLE TRUE
#define LINKED_LIST_LOG_ENABLE TRUE

// options for hash_table
#define HASH_TABLE_LOG_ENABLE FALSE
#define HASH_TABLE_INSERT_LOG_ENABLE FALSE
#define HASH_TABLE_SEARCH_LOG_ENABLE FALSE
#define HASH_TABLE_DELETE_LOG_ENABLE FALSE

// options for LR_automata
#define LR_AUTOMATA_LOG_ENABLE FALSE
#define LR_AUTOMATA_CONSTRUCT_SET_LOG_ENABLE TRUE
#define LR_AUTOMATA_GOTO_TABLE_LOG_ENABLE FALSE
#define LR_AUTOMATA_ACTION_TABLE_LOG_ENABLE TRUE
#define LR_AUTOMATA_FIRST_SET_LOG_ENABLE TRUE
#define LR_AUTOMATA_FOLLOW_SET_LOG_ENABLE FALSE
#define LR_AUTOMATA_PARSE_LOG_ENABLE TRUE

// options for NFA
#define NFA_LOG_ENABLE FALSE

// options for DFA
#define DFA_LOG_ENABLE TRUE
#define DFA_SPLIT_LOG_ENABLE FALSE
#define DFA_MINIFY_LOG_ENABLE TRUE

// options for regexp
#define REGEXP_LOG_ENABLE TRUE

#endif
