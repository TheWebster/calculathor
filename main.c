
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


static char *
get_string()
{
	int  c = 0;
	int  i = 0;
	char *string = NULL;
	
	while( (c = fgetc( stdin)) != '\n' && c != EOF ) {
		string = (char*)realloc( string, (i + 2)*sizeof(char));
		string[i++] = c;
	}
	
	return string;
};

		
int main( int argc, char *argv[])
{
	int       stacks   = 0;
	program_t *program = program_init();
	pstack_t  *stack;
	char      *string;
	
	
	printf( "Enter string:\n");
	string = get_string();
	
	strip_string( string);
	printf( "Stripped string: \"%s\"\n\n", string);
	
	if( parse_expression( string, program, &stacks, DATA_NUMBER) == -1 ) {
		printf( "Error: %s\n", parse_error);
		program_free( program);
		free( parse_error);
		return 1;
	}
	
	print_program( program);
	printf( "Needed stacksize: %d\n\n", stacks);
	stack = stack_init( stacks);
	
	printf( "\nResult: %f\n", execute_number( program, stack));
	program_free( program);
	stack_free( stack);
	
	return 0;
};
	
	
	
