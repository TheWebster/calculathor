/* ****************************************************************** *\
 * demo_symbols.h                                                     *
 *                                                                    *
 * Project:     CalculaThor                                           *
 * Author:      Christian Weber (ChristianWeber802@gmx.net)           *
 *                                                                    *
 * Description: Contains a reference application for calculathor.     *
 *              Symbol definitions.                                   *
\* ****************************************************************** */

#define DATA_ALIGN  DATA_EXTERN_01
#define execute_align( program, stack)  execute_int( program, stack)

typedef struct {
	char      name[32];
	data_t    data;
	
} var_t;

extern var_t varlist[];
extern var_t runtime_vars[];

var_t *get_var( char *string, var_t *vars);
int    symbol_parser( data_t *data, char *string);
