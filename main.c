
#include <stdio.h>

#include "parser.h"



int main( int argc, char *argv[])
{
	program_t *program = parse_expression( argv[1]);
	pstack_t  *stack;
	
	if( prog_has_error(program) ) {
		printf( "Error: %s\n", program->error);
		program_free( program);
		return 1;
	}
	
	stack = stack_init( 16);
	
	printf( "\nResult: %f\n", execute_number( program, stack));
	program_free( program);
	stack_free( stack);
	
	return 0;
};
	
	
	
