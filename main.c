
#include <stdio.h>

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
	program_t *program;
	pstack_t  *stack;
	
	strip_string( argv[1]);
	printf( "Stripped string: \"%s\"\n\n", argv[1]);
	
	program = parse_expression( argv[1], &stacks);
	if( prog_has_error(program) ) {
		printf( "Error: %s\n", program->error);
		program_free( program);
		return 1;
	}
	
	printf( "Needed stacksize: %d\n\n", stacks);
	stack = stack_init( stacks);
	
	printf( "\nResult: %f\n", execute_number( program, stack));
	program_free( program);
	stack_free( stack);
	
	return 0;
};
	
	
	
