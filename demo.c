/* ****************************************************************** *\
 * demo.c                                                             *
 *                                                                    *
 * Project:     CalculaThor                                           *
 * Author:      Christian Weber (ChristianWeber802@gmx.net)           *
 *                                                                    *
 * Description: Contains a reference application for calculathor.     *
\* ****************************************************************** */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "calc.h"
#include "demo_symbols.h"

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
		if( !(var = get_var( key, varlist)) ) {
			printf( "Line %d: Key '%s' not found.\n", lineno, key);
			return -1;
		}
		
		var->stuff.prog = program_init();
		strip_string( value);
		if( parse_expression( value, var->stuff.prog, stacksiz, var->type, symbol_parser) == -1 ) {
			printf( "Line %d: %s\n", lineno, parse_error);
			return -1;
		}
		
		lineno++;
	}
	
	return 0;
};
	
	

		
int main( int argc, char *argv[])
{
	//direct linked numbers - these change during runtime
	double minutes, seconds;
	
	int   stack_size = 0;
	char *string = NULL;
	pstack_t *ex_stack;
	
	FILE *file = fopen( READ_FILE, "r");
	
	
	if( !file ) {
		printf( "No file!\n");
		return 1;
	}
	
	// associate direct links
	runtime_vars[0].stuff.ptr = (void*)&seconds;
	runtime_vars[1].stuff.ptr = (void*)&minutes;
	
	if( read_file( file, &stack_size) == -1 )
		return 1;
	
	fclose( file);
	
	while( 1 ) {
		var_t  *var;
		time_t tim;
		struct tm *br_tim;
		
		
		ex_stack = stack_init( stack_size);
		
		printf( "Variable: ");
		string = get_string();
		if( (var = get_var( string, varlist)) != NULL ) {
			if( var->stuff.prog == NULL ) {
				printf( "'%s' is uninitialized.\n", string);
				continue;
			}
			
			tim = time( NULL);
			br_tim = gmtime( &tim);
			minutes = br_tim->tm_min;
			seconds = br_tim->tm_sec;
			
			printf( "   Address: %p\n", var->stuff.prog);
			print_program( var->stuff.prog);
			
			if( var->type == DATA_NUMBER )
				printf( "Result: %f\n\n", *execute_number( var->stuff.prog, ex_stack));
			else if( var->type == DATA_STRING )
				printf( "Result: %s\n\n", execute_string( var->stuff.prog, ex_stack));
			else if( var->type == DATA_ALIGN ) {
				int align = *execute_align( var->stuff.prog, ex_stack);
				
				if     ( align == ALIGN_LEFT   ) printf( "Result: 'left'\n\n");
				else if( align == ALIGN_RIGHT  ) printf( "Result: 'right'\n\n");
				else if( align == ALIGN_CENTER ) printf( "Result: 'center'\n\n");
			}
				
		}
		stack_free( ex_stack);
	}
	
	return 0;
};
	
	
	
