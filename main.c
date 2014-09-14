
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

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
	string[i] = '\0';
	
	return string;
};

typedef struct {
	char      name[32];
	uint16_t  type;
	program_t *prog;
} var_t;

var_t varlist[] = {
	{ "number1", DATA_NUMBER, NULL },
	{ "number2", DATA_NUMBER, NULL },
	{ "string1", DATA_STRING, NULL },
	{ "string2", DATA_STRING, NULL },
	
	{ ""       , 0          , NULL }
};


static int
symbol_parser( data_t *data, char *string)
{
	var_t *ptr;
	
	
	for( ptr = varlist; ptr->type != 0; ptr++ ) {
		if( strcmp( ptr->name, string) == 0 ) {
			data->contents.prog = ptr->prog;
			data->type          = ptr->type;
			data->link          = DATA_CONTENT_LINK;
			
			return 0;
		}
	}
	
	return -1;
};

		
int main( int argc, char *argv[])
{
	var_t *var;
	int   stack_size = 0;
	char *string = NULL;
	pstack_t *ex_stack;
	
	
	for( var = varlist; var->type != 0; var++ ) {
		int  stacks;
		
		
		var->prog = program_init();
		free( parse_error);
		parse_error = NULL;
		do {
			if( parse_error )
				printf( "ERROR: %s\n", parse_error);
				
			free( string);
			printf( "%s: ", var->name);
			string = get_string();
			strip_string( string);
			printf( "Stripped string: %s\n", string);
		} while( parse_expression( string, var->prog, &stacks, var->type, symbol_parser) == -1 );
		
		stack_size = (stack_size > stacks) ? stack_size
		                                   : stacks;
		printf( "\nNeeded stacksize: %d\n\n\n", stack_size);
	}
	
	free( string);
	while( 1 ) {
		data_t dat;
		
		
		ex_stack = stack_init( stack_size);
		
		printf( "Variable: ");
		string = get_string();
		if( symbol_parser( &dat, string) == 0 ) {
			print_program( dat.contents.prog);
			
			if( dat.type == DATA_NUMBER )
				printf( "Result: %f\n\n", execute_number( dat.contents.prog, ex_stack));
			else if( dat.type == DATA_STRING )
				printf( "Result: %s\n\n", execute_string( dat.contents.prog, ex_stack));
		}
		stack_free( ex_stack);
	}
	
	return 0;
};
	
	
	
