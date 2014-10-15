/* ****************************************************************** *\
 * demo_symbols.c                                                     *
 *                                                                    *
 * Project:     CalculaThor                                           *
 * Author:      Christian Weber (ChristianWeber802@gmx.net)           *
 *                                                                    *
 * Description: Contains a reference application for calculathor.     *
 *              Symbol definitions.                                   *
\* ****************************************************************** */

#include <stdint.h>
#include <string.h>

#include "parser.h"
#include "demo_symbols.h"


#define VAR( name, type, content)    { (name), { (content_t)(content), DATA_NO_LINK, (type)}}
#define VAR_END                      { "", { (content_t)0.0, 0, 0}} 

var_t varlist[] = {
	VAR( "number1", DATA_NUMBER, NULL),
	VAR( "number2", DATA_NUMBER, NULL),
	VAR( "string1", DATA_STRING, NULL),
	VAR( "string2", DATA_STRING, NULL),
	VAR( "align"  , DATA_ALIGN , NULL),
	VAR_END
};

var_t runtime_vars[] = {
	VAR( "minutes", DATA_NUMBER, 0.0),
	VAR( "seconds", DATA_NUMBER, 0.0),
	VAR_END
};

var_t sym_align[] = {
	VAR( "left"  , DATA_ALIGN, 0),
	VAR( "right" , DATA_ALIGN, 1),
	VAR( "center", DATA_ALIGN, 2),
	VAR_END
};


var_t *
get_var( char *string, var_t *vars)
{
	var_t *ptr;
	
	
	for( ptr = vars; ptr->data.type != 0; ptr++ ) {
		if( strcmp( ptr->name, string) == 0 )
			return ptr;
	}
	return NULL;
};


int
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
	
	ptr = get_var( string, sym_align);
	
	if( ptr ) {
		data->contents = ptr->data.contents;
		data->type     = ptr->data.type;
		data->link     = DATA_NO_LINK;
		return 0;
	}
	
	return -1;
};
