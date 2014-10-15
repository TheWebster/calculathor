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
	data_t    data;
	
} var_t;

#define VAR( name, type, content)    { (name), { (content_t)(content), DATA_NO_LINK, (type)}}
#define VAR_END                      { "", { (content_t)0.0, 0, 0}} 

var_t varlist[] = {
	VAR( "number1", DATA_NUMBER, NULL),
	VAR( "number2", DATA_NUMBER, NULL),
	VAR( "string1", DATA_STRING, NULL),
	VAR( "string2", DATA_STRING, NULL),
	VAR_END
};

var_t runtime_vars[] = {
	VAR( "minutes", DATA_NUMBER, 0.0),
	VAR( "seconds", DATA_NUMBER, 0.0),
	VAR_END
};


static var_t *
get_var( char *string, var_t *vars)
{
	var_t *ptr;
	
	
	for( ptr = vars; ptr->data.type != 0; ptr++ ) {
		if( strcmp( ptr->name, string) == 0 )
			return ptr;
	}
	return NULL;
};


static int
symbol_parser( data_t *data, char *string)
{
	var_t *ptr = get_var( string, varlist);
	
	
	if( ptr ) {
		data->contents.prog = ptr->data.contents.prog;
		data->type          = ptr->data.type;
		data->link          = DATA_CONTENT_LINK;
		return 0;
	}
	
	ptr = get_var( string, runtime_vars);
	
	if( ptr ) {
		data->contents = ptr->data.contents;
		data->type     = ptr->data.type;
		data->link     = DATA_DIRECT_LINK;
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
		if( !(var = get_var( key, varlist)) ) {
			printf( "Line %d: Key '%s' not found.\n", lineno, key);
			return -1;
		}
		
		var->data.contents.prog = program_init();
		strip_string( value);
		if( parse_expression( value, var->data.contents.prog, stacksiz, var->data.type, symbol_parser) == -1 ) {
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
	runtime_vars[0].data.contents.ptr_number = &minutes;
	runtime_vars[1].data.contents.ptr_number = &seconds;
	
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
			if( var->data.contents.prog == NULL ) {
				printf( "'%s' is uninitialized.\n", string);
				continue;
			}
			
			tim = time( NULL);
			br_tim = gmtime( &tim);
			minutes = br_tim->tm_min;
			seconds = br_tim->tm_sec;
			
			printf( "   Address: %p\n", var->data.contents.prog);
			print_program( var->data.contents.prog);
			
			if( var->data.type == DATA_NUMBER )
				printf( "Result: %f\n\n", execute_number( var->data.contents.prog, ex_stack));
			else if( var->data.type == DATA_STRING )
				printf( "Result: %s\n\n", execute_string( var->data.contents.prog, ex_stack));
		}
		stack_free( ex_stack);
	}
	
	return 0;
};
	
	
	
