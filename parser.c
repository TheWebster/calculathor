/* ****************************************************************** *\
 * parser.c                                                           *
 *                                                                    *
 * Project:     CalculaThor                                           *
 * Author:      Christian Weber (ChristianWeber802@gmx.net)           *
 *                                                                    *
 * Description: Parsing functions.                                    *
\* ****************************************************************** */

#include <stdarg.h>
#include <stdint.h>
#include <stdio.h> // debugging!!
#include <stdlib.h>
#include <string.h>

#include "calc.h"
#include "op.h"
#include "data.h"
#include "stack.h"


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
	int      stacksize;
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
static void
stacksize_inc( parser_t *parser, int incr)
{
	parser->current_stacksize += incr;
	parser->needed_stacksize = (parser->current_stacksize > parser->needed_stacksize) ? parser->current_stacksize
	                                                                                  : parser->needed_stacksize;
};

/* Global error string */
char *parse_error = NULL;


/*
 * Sets parse_error to a string.
 * (Usage just like printf)
 */
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


/*
 * Checks whether a number of operands from the program stack is compatible with an operator.
 * 
 * Parameters: parser - Parser struct to get the operands from.
 *             op     - The operator to check.
 *             number - The number of operands to check.
 * 
 * Returns: 0 if the operands are compatible with the operand and -1 if not.
 */
static int
check_type( parser_t *parser, operator_t *op, int number)
{
	int        i;
	
	
	if( (parser->program->end - parser->program->data) < number ) {
		set_error( "Not enough arguments for operator '%s'.", op->string);
		return -1;
	}
	
	for( i = 0; i < number; i++ ) {
		data_t *d = parser->program->top - i;
		
		
		if( d->type == DATA_OPERATOR ) {
			operator_t *opa = (operator_t*)(d->contents.ptr);
			
			if( op->needed_type & opa->returned_type ) continue;
		}
		else
			if( op->needed_type & d->type ) continue;
		
		set_error( "Invalid type for operator '%s'.", op->string);
		return -1;
	}
	
	return 0;
};
		


/*
 * Flushes the operator_stack into the program stack until it is empty, encounters the
 * "open bracket" operator or one of the operators precedence is higher than the
 * specified.
 * 
 * Parameters: parser     - Parser struct with operator stack.
 *             precedence - Only flush until an operators precedence is higher than this.
 * 
 * Returns: 0 when successfull, -1, when invalid types are used.
 */
static int
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
		
		/* typechecking */
		if( op->op_type == OP_TYPE_UNARY_NOT ) {
			if( check_type( parser, op, 1) == -1 ) return -1;
		}
		else {
			if( check_type( parser, op, 2) == -1 ) return -1;
		}
		
		stack_addoperator( parser->program, op);
		stack_del( parser->operator_stack);
		stacksize_inc( parser, increment);
	}
	
	return 0;
};


/*
 * Checks whether a binary operator would be valid in this position.
 * 
 * Parameters: parser - Parser struct.
 * 
 * Returns: 0 if a binary operator is valid, -1 when not.
 */
static int
binary_op_valid( parser_t *parser)
{
	if( parser->last_token == NULL ) return -1;
	
	if( parser->last_token->type == DATA_OPERATOR ) {
		operator_t *op = (operator_t*)parser->last_token->contents.ptr;
		
		if( op->op_type != OP_TYPE_CLOSE_BRACKET ) return -1;
	}
	
	return 0;
};


/*
 * Checks whether an unary operator would be valid in this position.
 * 
 * Parameters: parser - Parser struct.
 * 
 * Returns: 0 if an unary operator would be valid, -1 if not.
 */
static int
unary_op_valid( parser_t *parser)
{
	if( parser->last_token == NULL ) return 0;
	
	if( parser->last_token->type == DATA_OPERATOR ) {
		operator_t *op = (operator_t*)parser->last_token->contents.ptr;
		
		if( op->op_type == OP_TYPE_OPEN_BRACKET || op->op_type == OP_TYPE_UNARY_NOT ) return 0;
	}
	
	return -1;
};


/*
 * Checks whether a value would be valid in this position.
 * 
 * Parameters: parser - Parser struct.
 * 
 * Returns: 0 if a value would be valid, -1 if not.
 */
static int
value_valid( parser_t *parser)
{
	if( parser->last_token == NULL )
		return 0;
	
	if( parser->last_token->type == DATA_OPERATOR ) {
		operator_t *op = (operator_t*)parser->last_token->contents.ptr;
		
		if( op->op_type != OP_TYPE_CLOSE_BRACKET ) return 0;
	}
	
	return -1;
};


/*
 * Parses a string, using a given parser structure.
 * 
 * Parameters: parser       - A pointer to the parser structure to be used.
 *             string       - The string to be parsed.
 *             allowed_type - Only this type is allowed to return from the equation.
 *             token_cb     - Callback function to recognize external types.
 * 
 * Returns: 0 on success, -1 when an error occurs.
 */
static int
parse( parser_t *parser, char *string, uint16_t allowed_type, token_callback token_cb)
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
			
			if( *parser->variable == '"' && parser->last_operator == NULL ) {
				parser->variable++;
				stack_addstring( parser->program, parser->variable);
				stacksize_inc( parser, 1);
				
				parser->last_token = parser->program->top;
			}
			else {
				number = strtod( parser->variable, &endptr);
				if( *endptr == '\0' ) {
					stack_addnumber( parser->program, number);
					stacksize_inc( parser, 1);
					
					parser->last_token = parser->program->top;
				}
				else {
					data_t dat;
					
					
					if( token_cb( &dat.contents.ptr, &dat.type, &dat.link, parser->variable) == 0 ) {
						stack_add( parser->program, dat.contents, dat.type, dat.link);
						if( dat.link == DATA_CONTENT_LINK )
							stacksize_inc( parser, dat.contents.prog->stacksize);
						else
							stacksize_inc( parser, 1);
						
						parser->last_token = parser->program->top;
					}
					else {
						set_error( "Unknown variable '%s'.", parser->variable);
						return -1;
					}
				}
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
					if( flush_opstack( parser, 10) == -1 ) return -1;
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
			
			if( flush_opstack( parser, parser->last_operator->precedence) == -1 ) return -1;
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
	
	if( flush_opstack( parser, 10) == -1 ) return -1;
	
	if( parser->program->top->type == DATA_OPERATOR ) {
		operator_t *op = stack_getoperator( parser->program);
		
		if( allowed_type & op->returned_type ) return 0;
	}
	else {
		if( allowed_type & parser->program->top->type ) return 0;
	}
			
	set_error( "Resulting type not allowed.");
	
	return -1;
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
 * Parameters: string       - The string to be parsed.
 *             program      - A pointer to an initialized program structure.
 *             stacksize    - The needed stacksize for execution will be stored here.
 *             allowed_type - Only this type is allowed to be returned by the equation.
 *             token_cb     - Callback function to recognize external types.
 * 
 * Returns: A pointer to the resulting program, or NULL when an error occurs.
 */
int
parse_expression( char *string, program_t *program, int *stacksize, uint16_t allowed_type, token_callback token_cb)
{
	parser_t  *parser  = parser_init();
	
	if( parse( parser, string, allowed_type, token_cb) == -1 ) {
		stack_free( parser_finalize( parser));
		return -1;
	}
	else {
		*stacksize = (parser->needed_stacksize > *stacksize) ? parser->needed_stacksize
		                                                     : *stacksize;
		program->stacksize = parser->needed_stacksize;
		program->code      = parser_finalize( parser);
	}
	
	return 0;
};


static void
print_stack( pstack_t *stack)
{
	data_t *ptr;
	
	
	for( ptr = stack->data; ptr <= stack->top; ptr++ ) {
		if( ptr->link == DATA_NO_LINK )
			printf( "  VALUE       ");
		else if( ptr->link == DATA_DIRECT_LINK )
			printf( "  DIRECT_LINK ");
		else if( ptr->link == DATA_CONTENT_LINK ) {
			printf( "  CONTENT_LINK");
			
			if( ptr->type & DATA_NUMBER )
				printf( "  NUMBER    %p\n", ptr->contents.ptr);
			else if( ptr->type & DATA_STRING )
				printf( "  STRING    %p\n", ptr->contents.ptr);
			else
				printf( "  EXTERNAL  %p\n", ptr->contents.ptr);
			
			continue;
		}
		
		if( ptr->type & DATA_NUMBER ) {
			if( ptr->link == DATA_DIRECT_LINK )
				printf( "  NUMBER    %f\n", *ptr->contents.ptr_number);
			else
				printf( "  NUMBER    %f\n", ptr->contents.number);
		}
		else if( ptr->type & DATA_STRING )
			printf( "  STRING    %s\n", ptr->contents.string);
		else if( ptr->type & DATA_OPERATOR )
			printf( "  OP        %s\n", ((operator_t*)ptr->contents.ptr)->string);
		else
			printf( "  EXTERNAL  %p\n", ptr->contents.ptr);
	}
};


void
print_program( program_t *prog)
{
	print_stack( prog->code);
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
	data_t    *ptr;
	
	
	for( ptr = program->code->data; ptr <= program->code->top; ptr++ ) {
		if( ptr->type & DATA_OPERATOR ) {
			ptr->contents.op->function( stack);
			continue;
		}
		else if( ptr->link == DATA_CONTENT_LINK )
			execute_stack( ptr->contents.prog, stack);
		else if( ptr->link == DATA_DIRECT_LINK ) {
			if( ptr->type & DATA_NUMBER )
				stack_addnumber( stack, *ptr->contents.ptr_number);
			else
				stack_add( stack, ptr->contents, ptr->type, DATA_DIRECT_LINK);
		}
		else
			stack_add( stack, ptr->contents, ptr->type, DATA_DIRECT_LINK);
	}
};


/*
 * Executes a program using a given pstack structure and
 * returns the result as content.
 * 
 * Parameters: program - The program to be executed.
 *             stack   - The pstack structure to be used. (Must be cleared and will be cleared after use!)
 * 
 * Returns: A void pointer to the result or NULL when programm is NULL.
 */
void *
execute_data( program_t *program, pstack_t *stack)
{
	if( program == NULL )
		return NULL;
	
	execute_stack( program, stack);
	
	if( stack->top->type == DATA_NUMBER ) return &stack->top->contents.number;
	
	return stack->top->contents.ptr;
};


	
	
	
