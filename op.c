
#include <stdlib.h>
#include <string.h>

#include "parser.h"
#include "stack.h"
#include "op.h"


/*
 * Callback function for adding (+).
 */
void
op_add( pstack_t *stack)
{
	double reg = stack_popnumber( stack);
	
	reg += stack_popnumber( stack);
	stack_add( stack, (data_t)reg, DATA_TYPE_NUMBER);
};


/*
 * Callback function for substracting (-).
 */
void
op_sub( pstack_t *stack)
{
	double reg = stack_popnumber( stack);
	
	reg = stack_popnumber( stack) - reg;
	stack_addnumber( stack, reg);
};


/*
 * Callback function for multiplying (*).
 */
void
op_mul( pstack_t *stack)
{
	double reg = stack_popnumber( stack);
	
	reg = stack_popnumber( stack) * reg;
	stack_addnumber( stack, reg);
};


/*
 * Callback function for dividing (/).
 */
void
op_div( pstack_t *stack)
{
	double reg = stack_popnumber( stack);
	
	reg = stack_popnumber( stack) / reg;
	stack_addnumber( stack, reg);
};


operator_t op_list[] = {
	{ "*" , 1, -1, op_mul },
	{ "/" , 1, -1, op_div },
	{ "+" , 2, -1, op_add },
	{ "-" , 2, -1, op_sub },
	{ NULL, 0, 0 , NULL  }
};


/*
 * Compares two strings until one of them reaches its end.
 * 
 * Parameters: s1 and s2 - Strings to be compared.
 * 
 * Returns: 0 when the two strings are equal to the point where one of them
 *          ends. -1 otherwise.
 */
static int
strlcmp( char *s1, char *s2)
{
	if( *s1 == '\0' || *s2 == '\0' )
		return -1;
	
	while( *s1 != '\0' && *s2 != '\0' ) {
		if( *s1 != *s2 )
			return -1;
		
		s1++, s2++;
	}
	
	return 0;
};


/*
 * Searches an operator in a string.
 * When it finds one, it replaces it with '\0', stores a pointer to the rest of the string
 * and returns a pointer to the operator struct.
 * 
 * Parameters: string - The string to be searched.
 *             next   - Pointer to the rest of the string is stored here.
 * 
 * Returns: A pointer to the found operator, NULL when none is found.
 */
operator_t *
op_token( char *string, char **next)
{
	char *hlp_string = string;
	
	
	for(; *hlp_string; hlp_string++ ) {
		operator_t *ops = op_list;
		
		
		for(; ops->string; ops++ ) {
			if( strlcmp( ops->string, hlp_string) == 0 ) {
				if( hlp_string == string ) {
					if( ops->function == op_add || ops->function == op_sub )
						continue;
				}

				*hlp_string = '\0';
				*next = hlp_string + strlen( ops->string);
				
				return ops;
			}
		}
	}
	
	return NULL;
};
