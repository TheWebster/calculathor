
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "parser.h"

#define READ_FILE   "./READ_FILE"
#define LINE_LENGTH 255


void
strip_string( char *string)
{
	char *ptr, *back_ptr;
	int  in_string = 0;
	
	
	for( ptr = string, back_ptr = ptr; *ptr != '\0'; ptr++, back_ptr++ ) {
		if( in_string ) {
			if( *ptr == '"' )
				in_string = 0;
		}
		else {
			if( *ptr == ' ' || *ptr == '\n' || *ptr == '\t' )
				ptr++;
		}
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


static var_t *
get_var( char *string)
{
	var_t *ptr;
	
	
	for( ptr = varlist; ptr->type != 0; ptr++ ) {
		if( strcmp( ptr->name, string) == 0 )
			return ptr;
	}
	return NULL;
};


static int
symbol_parser( data_t *data, char *string)
{
	var_t *ptr = get_var( string);
	
	
	if( ptr ) {
		data->contents.prog = ptr->prog;
		data->type          = ptr->type;
		data->link          = DATA_CONTENT_LINK;
		return 0;
	}
	
	return -1;
};


static int
read_file( FILE *file, int *stacksiz)
{
	int  lineno = 1;
	char string[LINE_LENGTH+1];
	
	while( fgets( string, LINE_LENGTH, file) != NULL ) {
		char   *key, *value;
		var_t  *var;
		
		
		
		if( string[strlen(string)-1] != '\n' ) {
			printf( "Line %d: Line too long.\n", lineno);
			return -1;
		}
		
		key = strtok_r( string, ":", &value);
		if( !(var = get_var( key)) ) {
			printf( "Line %d: Key '%s' not found.\n", lineno, key);
			return -1;
		}
		
		var->prog = program_init();
		strip_string( value);
		if( parse_expression( value, var->prog, stacksiz, var->type, symbol_parser) == -1 ) {
			printf( "Line %d: %s\n", lineno, parse_error);
			return -1;
		}
		
		lineno++;
	}
	
	return 0;
};
	
	

		
int main( int argc, char *argv[])
{
	int   stack_size = 0;
	char *string = NULL;
	pstack_t *ex_stack;
	
	FILE *file = fopen( READ_FILE, "r");
	
	
	if( !file ) {
		printf( "No file!\n");
		return 1;
	}
	
	if( read_file( file, &stack_size) == -1 )
		return 1;
	
	fclose( file);
	
	
	while( 1 ) {
		data_t dat;
		
		
		ex_stack = stack_init( stack_size);
		
		printf( "Variable: ");
		string = get_string();
		if( symbol_parser( &dat, string) == 0 ) {
			if( dat.contents.prog == NULL ) {
				printf( "'%s' is uninitialized.\n", string);
				continue;
			}
			
			printf( "   Address: %p\n", dat.contents.prog);
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
	
	
	
