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

#include "calc.h"
#include "demo_symbols.h"



#define VAR_END                      { "", 0, (stuff_t)NULL} 

var_t varlist[] = {
	{ "number1", DATA_NUMBER, (stuff_t)NULL },
	{ "number2", DATA_NUMBER, (stuff_t)NULL },
	{ "string1", DATA_STRING, (stuff_t)NULL },
	{ "string2", DATA_STRING, (stuff_t)NULL },
	{ "align"  , DATA_ALIGN , (stuff_t)NULL },
	VAR_END
};

var_t runtime_vars[] = {
	{ "seconds", DATA_NUMBER, (stuff_t)NULL },
	{ "minutes", DATA_NUMBER, (stuff_t)NULL },
	VAR_END
};

var_t sym_align[] = {
	{ "left"  , DATA_ALIGN, (stuff_t)ALIGN_LEFT   },
	{ "center", DATA_ALIGN, (stuff_t)ALIGN_CENTER },
	{ "right" , DATA_ALIGN, (stuff_t)ALIGN_RIGHT  },
	VAR_END
};


var_t *
get_var( char *string, var_t *vars)
{
	var_t *ptr;
	
	
	for( ptr = vars; ptr->type != 0; ptr++ ) {
		if( strcmp( ptr->name, string) == 0 )
			return ptr;
	}
	return NULL;
};


int
symbol_parser( void **data, uint16_t *type, int *link, char *string)
{
	var_t *ptr = get_var( string, varlist);
	
	
	if( ptr ) {
		*data = (void*)ptr->stuff.prog;
		*type = ptr->type;
		*link = DATA_CONTENT_LINK;
		return 0;
	}
	
	ptr = get_var( string, runtime_vars);
	
	if( ptr ) {
		*data = ptr->stuff.ptr;
		*type = ptr->type;
		*link = DATA_DIRECT_LINK;
		return 0;
	}
	
	ptr = get_var( string, sym_align);
	
	if( ptr ) {
		*data = (void*)&ptr->stuff.number;
		*type = ptr->type;
		*link = DATA_NO_LINK;
		return 0;
	}
	
	return -1;
};
