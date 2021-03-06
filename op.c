/* ****************************************************************** *\
 * op.c                                                               *
 *                                                                    *
 * Project:     CalculaThor                                           *
 * Author:      Christian Weber (ChristianWeber802@gmx.net)           *
 *                                                                    *
 * Description: Definition of operators.                              *
\* ****************************************************************** */

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "calc.h"
#include "op.h"
#include "data.h"
#include "stack.h"


/*
 * Callback function for adding (+).
 */
void
op_add( pstack_t *stack)
{
	double reg = stack_popnumber( stack);
	
	reg += stack_popnumber( stack);
	stack_addnumber( stack, reg);
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


/*
 * Callback function for "Less than" (<).
 */
void
op_lt( pstack_t *stack)
{
	double reg = stack_popnumber( stack);
	
	if( stack_popnumber( stack) < reg )
		stack_addnumber( stack, 1.0);
	else
		stack_addnumber( stack, 0.0);
};


/*
 * Callback function for "Less than or equal" (<=).
 */
void
op_le( pstack_t *stack)
{
	double reg = stack_popnumber( stack);
	
	if( stack_popnumber( stack) <= reg )
		stack_addnumber( stack, 1.0);
	else
		stack_addnumber( stack, 0.0);
};


/*
 * Callback function for "Greater than" (>).
 */
void
op_gt( pstack_t *stack)
{
	double reg = stack_popnumber( stack);
	
	if( stack_popnumber( stack) > reg )
		stack_addnumber( stack, 1.0);
	else
		stack_addnumber( stack, 0.0);
};


/*
 * Callback function for "Greater than or equal" (>=).
 */
void
op_ge( pstack_t *stack)
{
	double reg = stack_popnumber( stack);
	
	if( stack_popnumber( stack) >= reg )
		stack_addnumber( stack, 1.0);
	else
		stack_addnumber( stack, 0.0);
};


/*
 * Compares the two last elements of a stack, and deletes them from the stack.
 * (Helper for op_eq and op_neq)
 * 
 * Returns: 0 when both operands are equal, -1 when they are not equal or of different types.
 */
static int
op_hlp_eq( pstack_t *stack)
{
	data_t *a = stack->top;
	data_t *b = stack->top - 1;
	int    ret = -1;
	
	
	if( a->type == b->type ) {
		if( a->type == DATA_STRING )
			ret = strcmp( a->contents.string, b->contents.string);
		else if( a->contents.number == b->contents.number ) ret = 0;
		else if( a->contents.ptr    == b->contents.ptr ) ret = 0;
	}
	
	stack_del( stack);
	stack_del( stack);
	
	return ret;
};


/*
 * Callback function for "Equal" (==).
 */
void
op_eq( pstack_t *stack)
{
	if( op_hlp_eq( stack) == 0 )
		stack_addnumber( stack, 1.0);
	else
		stack_addnumber( stack, 0.0);
};


/*
 * Callback function for "Not equal" (!=).
 */
void
op_neq( pstack_t *stack)
{
	if( op_hlp_eq( stack) != 0 )
		stack_addnumber( stack, 1.0);
	else
		stack_addnumber( stack, 0.0);
};


/*
 * Converts the last element of stack to a logical value and deletes it from the stack.
 * (Helper for op_and and op_or)
 * 
 * Returns: 0 when a number is 0 or a string is empty, 1 when a number is != 0,
 *          a string is not empty or the element is of different type.
 */
static int
op_hlp_logic( pstack_t *stack)
{
	int    ret = 1;
	
	
	if( stack->top->type == DATA_NUMBER ) {
		if( stack->top->contents.number == 0.0 )
			ret = 0;
	}
	else if( stack->top->type == DATA_STRING ) {
		if( *stack->top->contents.string == '\0' )
			ret = 0;
	}
	
	stack_del( stack);
	return ret;
};
	
	
	
	
	


/*
 * Callback function for "And" (&&).
 */
void
op_and( pstack_t *stack)
{
	if( op_hlp_logic( stack) && op_hlp_logic( stack) )
		stack_addnumber( stack, 1.0);
	else
		stack_addnumber( stack, 0.0);
};


/*
 * Callback function for "Or" (||).
 */
void
op_or( pstack_t *stack)
{
	if( op_hlp_logic( stack) || op_hlp_logic( stack) )
		stack_addnumber( stack, 1.0);
	else
		stack_addnumber( stack, 0.0);
};


/*
 * Callback function for "Unary Minus" (-).
 */
void
op_unary_minus( pstack_t *stack)
{
	double reg = stack_popnumber( stack);
	
	stack_addnumber( stack, -reg);
};


/*
 * Callback function for "Not" (!).
 */
void
op_not( pstack_t *stack)
{
	if( op_hlp_logic( stack) )
		stack_addnumber( stack, 0.0);
	else
		stack_addnumber( stack, 1.0);
};


operator_t op_list[] = {
	//Arithmetic
	{ "*" , 2, OP_TYPE_BINARY, DATA_NUMBER, DATA_NUMBER, op_mul },
	{ "/" , 2, OP_TYPE_BINARY, DATA_NUMBER, DATA_NUMBER, op_div },
	{ "+" , 3, OP_TYPE_PLUS  , DATA_NUMBER, DATA_NUMBER, op_add },
	{ "-" , 3, OP_TYPE_MINUS , DATA_NUMBER, DATA_NUMBER, op_sub },
	
	//Comparing (two char operators need to be listed first to be recognised!)
	{ "<=", 4, OP_TYPE_BINARY, DATA_NUMBER, DATA_NUMBER, op_le  },
	{ ">=", 4, OP_TYPE_BINARY, DATA_NUMBER, DATA_NUMBER, op_ge  },
	{ "==", 5, OP_TYPE_BINARY, DATA_ANY   , DATA_NUMBER, op_eq  },
	{ "!=", 5, OP_TYPE_BINARY, DATA_ANY   , DATA_NUMBER, op_neq },
	{ "<" , 4, OP_TYPE_BINARY, DATA_NUMBER, DATA_NUMBER, op_lt  },
	{ ">" , 4, OP_TYPE_BINARY, DATA_NUMBER, DATA_NUMBER, op_gt  },
	
	//Logical
	{ "&&", 6, OP_TYPE_BINARY, DATA_ANY   , DATA_NUMBER, op_and },
	{ "||", 6, OP_TYPE_BINARY, DATA_ANY   , DATA_NUMBER, op_or  },
	
	//Special operators
	{ "!" , 1,  OP_TYPE_UNARY_NOT    , DATA_ANY, DATA_NUMBER, op_not },
	{ "(" , 0,  OP_TYPE_OPEN_BRACKET , 0       , 0          , NULL },
	{ ")" , 0,  OP_TYPE_CLOSE_BRACKET, 0       , 0          , NULL },
		
	{ NULL, 0, 0, 0, 0, NULL   }
};


operator_t operator_unary_minus = { "-", 1, OP_TYPE_UNARY_NOT, DATA_NUMBER, DATA_NUMBER, op_unary_minus };


/*
 * Compares two strings until s1 reaches its end.
 * 
 * Parameters: s1 and s2 - Strings to be compared.
 * 
 * Returns: 0 when the two strings are equal to the point where s1
 *          ends. -1 otherwise.
 */
static int
strlcmp( char *s1, char *s2)
{
	while( *s1 != '\0' ) {
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
	int  in_string   = 0;
	
	
	for(; *hlp_string; hlp_string++ ) {
		if( in_string ) {
			if( *hlp_string == '"' ) {
				in_string = 0;
				*hlp_string = '\0';
				*next = hlp_string + 1;
				return NULL;
			}
		}
		else {
			if( *hlp_string == '"' )
				in_string = 1;
			else {
				operator_t *ops = op_list;
				
				
				for(; ops->string; ops++ ) {
					if( strlcmp( ops->string, hlp_string) == 0 ) {
						*hlp_string = '\0';
						*next = hlp_string + strlen( ops->string);
						
						return ops;
					}
				}
			}
		}
	}
	
	*next = hlp_string;
	
	return NULL;
};
