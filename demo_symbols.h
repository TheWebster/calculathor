/* ****************************************************************** *\
 * demo_symbols.h                                                     *
 *                                                                    *
 * Project:     CalculaThor                                           *
 * Author:      Christian Weber (ChristianWeber802@gmx.net)           *
 *                                                                    *
 * Description: Contains a reference application for calculathor.     *
 *              Symbol definitions.                                   *
\* ****************************************************************** */

#define DATA_ALIGN    DATA_EXTERN_01
#define ALIGN_LEFT    0
#define ALIGN_RIGHT   1
#define ALIGN_CENTER  2

#define execute_align( program, stack)  ((int*)execute_data( program, stack))

typedef union {
	int       number;
	void      *ptr;
	program_t *prog;
} stuff_t;

typedef struct {
	char      name[32];
	
	uint16_t  type;
	
	stuff_t   stuff;
} var_t;

extern var_t varlist[];
extern var_t runtime_vars[];

var_t *get_var( char *string, var_t *vars);
int    symbol_parser( void **data, uint16_t *type, int *link, char *string);
