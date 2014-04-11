
#include <stdio.h>

#include "parser.h"



int main( int argc, char *argv[])
{
	pstack_t *program = parse_expression( argv[1]);
	pstack_t *stack;
	
	if( program == NULL ) {
		printf( "Error!\n");
		return 1;
	}
	
	stack = stack_init( 16);
	
	printf( "\nResult: %f\n", execute_number( program, stack));
	stack_free( stack);
	
	return 0;
};
	
	
	
