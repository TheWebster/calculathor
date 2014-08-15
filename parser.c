
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h> // debugging!!
#include <stdlib.h>
#include <string.h>

#include "parser.h"
#include "stack.h"
#include "op.h"


typedef struct {
	pstack_t   *program;
	pstack_t   *operator_stack;
	
	char       *variable;
	char       *next;
	data_t     *last_token;
	operator_t *last_operator;
	
	int        brackets;
	int        current_stacksize;
	int        needed_stacksize;
} parser_t;

struct pprogram {
	pstack_t *code;
	data_t   preexec;
};


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
	
	parser->variable          = NULL;
	parser->next              = NULL;
	parser->last_token        = NULL;
	parser->last_operator     = NULL;
	parser->operator_stack    = stack_init( PARSER_OPSTACK_CHUNKSIZE);
	parser->program           = stack_init( PARSER_PROGRAMM_CHUNKSIZE);
	parser->brackets          = 0;
	parser->current_stacksize = 0;
	parser->needed_stacksize  = 0;
	
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
 * Increases the current stackksize and updates the needed stacksize.
 * Parameters: parser - pointer to parser struct.
 *             incr   - amount by which current_stacksize should be incremented.
 */
void
stacksize_inc( parser_t *parser, int incr)
{
	parser->current_stacksize += incr;
	parser->needed_stacksize = (parser->current_stacksize > parser->needed_stacksize) ? parser->current_stacksize
	                                                                                  : parser->needed_stacksize;
};


/*
 * Flushes the operator_stack into the program stack until it is empty or encounters
 * the op_block dummy.
 */
void
flush_opstack( parser_t *parser, int precedence)
{
	while( !stack_isempty( parser->operator_stack) ) {
		operator_t *op = stack_getoperator( parser->operator_stack);
		int        increment = (op->op_type != OP_TYPE_UNARY_NOT) ? -1 : 0;
		
		
		if( op->op_type == OP_TYPE_CLOSE_BRACKET ) {
			stack_del( parser->operator_stack);
			continue;
		}
		if( op->op_type == OP_TYPE_OPEN_BRACKET ) break;
		if( op->precedence > precedence ) break;
		
		stack_addoperator( parser->program, op);
		stack_del( parser->operator_stack);
		stacksize_inc( parser, increment);
	}
};

char *parse_error = NULL;

static void
set_error( char *format, ...)
{
	int     size;
	va_list vargs;
	
	if( parse_error != NULL ) {
		free( parse_error);
		parse_error = NULL;
	}
	
	va_start( vargs, format);
	size = vsnprintf( parse_error, 0, format, vargs);
	va_end( vargs);
	
	parse_error = (char*)malloc( size+1);
	va_start( vargs, format);
	vsnprintf( parse_error, size+1, format, vargs);
	va_end( vargs);
};


static int
binary_op_valid( parser_t *parser)
{
	if( parser->last_token == NULL )
		return -1;
	
	if( parser->last_token->type == DATA_OPERATOR ) {
		operator_t *op = (operator_t*)parser->last_token->contents.ptr;
		
		if( op->op_type != OP_TYPE_CLOSE_BRACKET )
			return -1;
	}
	
	return 0;
};


static int
unary_op_valid( parser_t *parser)
{
	if( parser->last_token == NULL )
		return 0;
	
	if( parser->last_token->type == DATA_OPERATOR ) {
		operator_t *op = (operator_t*)parser->last_token->contents.ptr;
		
		if( op->op_type == OP_TYPE_OPEN_BRACKET || op->op_type == OP_TYPE_UNARY_NOT )
			return 0;
	}
	
	return -1;
};


static int
value_valid( parser_t *parser)
{
	if( parser->last_token == NULL )
		return 0;
	
	if( parser->last_token->type == DATA_OPERATOR ) {
		operator_t *op = (operator_t*)parser->last_token->contents.ptr;
		
		if( op->op_type != OP_TYPE_CLOSE_BRACKET )
			return 0;
	}
	
	return -1;
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
	parser->variable = string;
	
	while( (parser->last_operator = op_token( parser->variable, &parser->next)) != NULL || *parser->variable != '\0' ) {
		if( *parser->variable != '\0' ) {
			char   *endptr;
			double number;
			
			if( value_valid( parser) == -1 ) {
				set_error( "Expected operator before value '%s'.", parser->variable);
				return -1;
			}
			
			number = strtod( parser->variable, &endptr);
			if( *endptr == '\0' ) {
				stack_addnumber( parser->program, number);
				stacksize_inc( parser, 1);
				
				parser->last_token = parser->program->top;
			}
			else {
				set_error( "Unknown variable '%s'.", parser->variable);
				return -1;
			}
		}
		
		if( parser->last_operator != NULL ) {
			switch( parser->last_operator->op_type ) {
				case OP_TYPE_MINUS:
					if( unary_op_valid( parser) == 0 ) {
						parser->last_operator = &operator_unary_minus;
						break;
					}
					goto binary;
					
				case OP_TYPE_PLUS:
					if( unary_op_valid( parser) == 0 )
						goto skip_op_handling;
				
			  binary:
				case OP_TYPE_BINARY:
					if( binary_op_valid( parser) == -1 ) {
						set_error( "Expected value before operator '%s'.", parser->last_operator->string);
						return -1;
					}
					break;
				
				case OP_TYPE_OPEN_BRACKET:
					if( value_valid( parser) == -1 ) {
						set_error( "Expected operator before open bracket.");
						return -1;
					}
					parser->brackets++;
					break;
				
				case OP_TYPE_CLOSE_BRACKET:
					if( binary_op_valid( parser) == -1 ) {
						set_error( "Expected value before closed brackets.");
						return -1;
					}
					if( parser->brackets == 0 ) {
						set_error( "Closing bracket without opening one.");
						return -1;
					}
					parser->brackets--;
					flush_opstack( parser, 10);
					stack_del( parser->operator_stack); // delete open bracket
					goto skip_op_handling;
					break;
				
				case OP_TYPE_UNARY_NOT:
					if( unary_op_valid( parser) == -1 ) {
						set_error( "Value before unary operator '%s'.", parser->last_operator->string);
						return -1;
					}
					break;
			}
			
			flush_opstack( parser, parser->last_operator->precedence);
		  skip_op_handling:
			stack_addoperator( parser->operator_stack, parser->last_operator);
			
			parser->last_token = parser->operator_stack->top;
		}
		parser->variable = parser->next;
	}
	
	if( parser->last_token->type == DATA_OPERATOR ) {
		operator_t *op = (operator_t*)parser->last_token->contents.ptr;
		
		if( op->op_type != OP_TYPE_CLOSE_BRACKET ) {
			set_error( "Expected value after operator '%s'.", op->string);
			return -1;
		}
	}
	if( parser->brackets > 0 ) {
		set_error( "%d unclosed brackets.", parser->brackets);
		return -1;
	}
	
	flush_opstack( parser, 10);
	
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
	
	memset( program, 0, sizeof(program_t));
	
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
	if( program->code != NULL )
		stack_free( program->code);
	
	if( program->preexec.type & DATA_STRING )
		free( program->preexec.contents.ptr);
	
	free( program);
};


/*
 * Parses a string and returns the resulting program.
 * 
 * Parameters: string - The string to be parsed.
 * 
 * Returns: A pointer to the resulting program, or NULL when an error occurs.
 */
int
parse_expression( char *string, program_t *program, int *stacksize)
{
	parser_t  *parser  = parser_init();
	
	if( parse( parser, string) == -1 ) {
		stack_free( parser_finalize( parser));
		return -1;
	}
	else {
		*stacksize = (parser->needed_stacksize > *stacksize) ? parser->needed_stacksize
		                                                     : *stacksize;
		program->code      = parser_finalize( parser);
	}
	
	return 0;
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
	data_t *ptr;
	
	
	for( ptr = program->code->data; ptr <= program->code->top; ptr++ ) {
		if( ptr->type & DATA_NUMBER ) {
			stack_addnumber( stack, ptr->contents.number);
			printf( "  NUMBER  %f\n", ptr->contents.number);
		}
		else if( ptr->type & DATA_OPERATOR ) {
			printf( "  OP      %s\n", ((operator_t*)ptr->contents.ptr)->string);
			((operator_t*)ptr->contents.ptr)->function( stack);
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
	
	
	
