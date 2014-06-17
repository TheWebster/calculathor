
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "parser.h"


void
strip_string( char *string)
{
	char *ptr, *back_ptr;
	
	
	for( ptr = string, back_ptr = ptr; *ptr != '\0'; ptr++, back_ptr++ ) {
		if( *ptr == ' ' || *ptr == '\n' || *ptr == '\t' )
			ptr++;
		
		*back_ptr = *ptr;
	}
	*back_ptr = '\0';
};
		
		
int main( int argc, char *argv[])
{
	int       stacks   = 0;
	program_t *program = program_init();
	pstack_t  *stack;
	
	strip_string( argv[1]);
	printf( "Stripped string: \"%s\"\n\n", argv[1]);
	
	if( parse_expression( argv[1], program, &stacks) == -1 ) {
		printf( "Error: %s\n", parse_error);
		program_free( program);
		free( parse_error);
		return 1;
	}
	
	printf( "Needed stacksize: %d\n\n", stacks);
	stack = stack_init( stacks);
	
	printf( "\nResult: %f\n", execute_number( program, stack));
	program_free( program);
	stack_free( stack);
	
	return 0;
};
	
	
	
