
#include <stdio.h> // debugging!!
#include <stdlib.h>

#include "parser.h"
#include "parse_error.h"
#include "stack.h"
#include "op.h"


typedef struct {
	pstack_t   *program;
	pstack_t   *operator_stack;
	
	char       *token;
	char       *next;
	operator_t *last_operator;
	
	int        brackets;
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
	parser->brackets       = 0;
	
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
 * Dummy operator to stop flushing the operator stack.
 * Uninitialized, only use pointer.
 */
static operator_t op_block;

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
			parse_set_error( "Expected expression before '%s'.", parser->last_operator->string);
			return -1;
		}
		
		/* open bracket */
		if( *parser->token == '(' ) {
			parser->brackets++;
			parser->token++;
			stack_addoperator( parser->operator_stack, &op_block);
		}
		
		number = strtod( parser->token, &endptr);
		if( *endptr == ')' ) {
			if( parser->brackets == 0 ) {
				parse_set_error( "Closing bracket without opening one.");
				return -1;
			}
		}
		else if( *endptr != '\0' ) {
			parse_set_error( "Unrecognized character '%c'.", *endptr);
			return -1;
		}
		
		stack_addnumber( parser->program, number);
		
		/* close bracket */
		if( *endptr == ')' ) {
			while( stack_getoperator( parser->operator_stack) != &op_block )
				stack_addoperator( parser->program, stack_popoperator( parser->operator_stack));
			
			stack_del( parser->operator_stack);
			parser->brackets--;
		}
		
		if( !stack_isempty( parser->operator_stack) && stack_getoperator( parser->operator_stack) != &op_block ) {
			if( parser->last_operator != NULL ) {
				if( parser->last_operator->precedence < stack_getoperator( parser->operator_stack)->precedence )
					goto skip_flush;
			}
			// flush operator stack
			while( !stack_isempty( parser->operator_stack) && stack_getoperator( parser->operator_stack) != &op_block )
				stack_addoperator( parser->program, stack_popoperator( parser->operator_stack));
		}
	  skip_flush:
		
		if( parser->last_operator == NULL )
			break;
		
		stack_addoperator( parser->operator_stack, parser->last_operator);
		
		parser->token = parser->next;
	}
	
	if( parser->brackets > 0 ) {
		parse_set_error( "%d unclosed brackets.", parser->brackets);
		return -1;
	}
	
	return 0;
};


/*
 * Allocate memory for a program structure.
 * 
 * Returns: The allocated structure.
 */
program_t *
program_init()
{
	program_t *program = (program_t*)malloc( sizeof(program_t));
	
	program->error = NULL;
	
	return program;
};


/*
 * Frees the memory allocated to a program structure.
 * 
 * Parameters: program - The Program to be freed.
 */
void
program_free( program_t *program)
{
	if( program->error == NULL )
		stack_free( program->code);
	else
		free( program->error);
	
	free( program);
};


/*
 * Parses a string and returns the resulting program.
 * 
 * Parameters: string - The string to be parsed.
 * 
 * Returns: A pointer to the resulting program, or NULL when an error occurs.
 */
program_t *
parse_expression( char *string)
{
	parser_t  *parser  = parser_init();
	program_t *program = program_init();
	
	if( parse( parser, string) == -1 ) {
		stack_free( parser_finalize( parser));
		program->error = parse_error;
	}
	else
		program->code = parser_finalize( parser);
	
	return program;
};


/*
 * Executes a program using a given pstack structure.
 * 
 * Parameters: program - The program to be executed.
 *             stack   - The pstack structure to be used. (Must be cleared!)
 */
static void
execute_stack( program_t *program, pstack_t *stack)
{
	byte_t *ptr;
	
	
	for( ptr = program->code->data; ptr <= program->code->top; ptr++ ) {
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
execute_number( program_t *program, pstack_t *stack)
{
	execute_stack( program, stack);
	
	return stack_popnumber( stack);
};
	
	
	
