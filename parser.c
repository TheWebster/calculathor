
#include <stdio.h> // debugging!!
#include <stdlib.h>

#include "parser.h"
#include "stack.h"
#include "op.h"
#include "error.h"


typedef struct {
	pstack_t   *program;
	pstack_t   *operator_stack;
	
	char       *token;
	char       *next;
	operator_t *last_operator;
} parser_t;


#define PARSER_PROGRAMM_CHUNKSIZE 16
#define PARSER_OPSTACK_CHUNKSIZE  16


/*
 * Allocates a parser structure.
 * 
 * Returns: A pointer to the allocated structure.
 */
static parser_t *
parser_init()
{
	parser_t *parser = (parser_t*)malloc( sizeof(parser_t));
	
	parser->token          = NULL;
	parser->next           = NULL;
	parser->last_operator  = NULL;
	parser->operator_stack = stack_init( PARSER_OPSTACK_CHUNKSIZE);
	parser->program        = stack_init( PARSER_PROGRAMM_CHUNKSIZE);
	
	return parser;
};


/*
 * Frees the memory of the parser struct except for the actual program.
 * 
 * Parameters: parser - A pointer to the parser structure to free.
 *
 * Returns: The program.
 */
static pstack_t *
parser_finalize( parser_t *parser)
{
	pstack_t *ret = parser->program;
	
	
	stack_free( parser->operator_stack);
	free( parser);
	
	return ret;
};


/*
 * Parses a string, using a given parser structure.
 * 
 * Parameters: parser - A pointer to the parser structure to be used.
 *             string - The string to be parsed.
 * 
 * Returns: 0 on success, -1 when an error occurs.
 */
static int
parse( parser_t *parser, char *string)
{
	parser->token = string;
	
	while( (parser->last_operator = op_token( parser->token, &parser->next)) != NULL || *parser->next != '\0' ) {
		char   *endptr;
		double number;
		
		
		if( *parser->next == '\0' ) {
			return -1;
		}
		
		number = strtod( parser->token, &endptr);
		if( *endptr != '\0' ) {
			return -1;
		}
		
		stack_addnumber( parser->program, number);
		
		if( !stack_isempty( parser->operator_stack) ) {
			if( parser->last_operator != NULL ) {
				if( parser->last_operator->precedence < stack_getoperator( parser->operator_stack)->precedence )
					goto skip_flush;
			}
			
			while( !stack_isempty( parser->operator_stack) )
				stack_addoperator( parser->program, stack_popoperator( parser->operator_stack));
		}
	  skip_flush:
		
		if( parser->last_operator == NULL )
			break;
		
		stack_addoperator( parser->operator_stack, parser->last_operator);
		
		parser->token = parser->next;
	}
	
	return 0;
};


/*
 * Parses a string and returns the resulting program.
 * 
 * Parameters: string - The string to be parsed.
 * 
 * Returns: A pointer to the resulting program, or NULL when an error occurs.
 */
pstack_t *
parse_expression( char *string)
{
	parser_t *parser = parser_init();
	
	
	if( parse( parser, string) == -1 ) {
		stack_free( parser_finalize( parser));
		return NULL;
	}
	
	return parser_finalize( parser);
};


/*
 * Executes a program using a given pstack structure.
 * 
 * Parameters: program - The program to be executed.
 *             stack   - The pstack structure to be used. (Must be cleared!)
 */
static void
execute_stack( pstack_t *program, pstack_t *stack)
{
	byte_t *ptr;
	
	
	for( ptr = program->data; ptr <= program->top; ptr++ ) {
		if( ptr->type == DATA_TYPE_NUMBER ) {
			stack_addnumber( stack, ptr->contents.number);
			printf( "  NUMBER  %f\n", ptr->contents.number);
		}
		else if( ptr->type == DATA_TYPE_OPERATOR ) {
			printf( "  OP      %s\n", ptr->contents.operator->string);
			ptr->contents.operator->function( stack);
		}
	}
};


/*
 * Executes a program using a given pstack structure, clears the pstack and returns the result as double.
 * 
 * Parameters: program - The program top be executed.
 *             stack   - The pstack structure to be used. (Must be cleared!)
 * 
 * Returns: The result as double.
 */
double
execute_number( pstack_t *program, pstack_t *stack)
{
	execute_stack( program, stack);
	
	return stack_popnumber( stack);
};
	
	
	
