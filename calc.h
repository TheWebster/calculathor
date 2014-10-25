/* ****************************************************************** *\
 * calc.h                                                             *
 *                                                                    *
 * Project:     CalculaThor                                           *
 * Author:      Christian Weber (ChristianWeber802@gmx.net)           *
 *                                                                    *
 * Description: User include file.                                    *
\* ****************************************************************** */

typedef struct pstack pstack_t;
typedef struct pprogram program_t;


#define DATA_NUMBER    (1 << 0)
#define DATA_STRING    (1 << 1)

#define DATA_EXTERN_01 (1 << 2)
#define DATA_EXTERN_02 (1 << 3)
#define DATA_EXTERN_03 (1 << 4)
#define DATA_EXTERN_04 (1 << 5)
#define DATA_EXTERN_05 (1 << 6)
#define DATA_EXTERN_06 (1 << 7)
#define DATA_EXTERN_07 (1 << 8)
#define DATA_EXTERN_08 (1 << 9)
#define DATA_EXTERN_09 (1 << 10)
#define DATA_EXTERN_10 (1 << 11)
#define DATA_EXTERN_11 (1 << 12)
#define DATA_EXTERN_12 (1 << 13)
#define DATA_EXTERN_13 (1 << 14)
#define DATA_EXTERN_14 (1 << 15)

#define DATA_ANY       UINT16_MAX

#define DATA_NO_LINK       0
#define DATA_CONTENT_LINK  1
#define DATA_DIRECT_LINK   2


/** int                                                                    **/
/** token_callback( void *data, uint16_t *type, int *link, char *string)   **/
/* Callback funktion to recognize symbols (variables, constants,...).
 * Parameters: data   - found data is stored here.
 *             type   - type of found data is stored here.
 *             link   - link type of found data is stored here.
 *             string - name of the symbol.
 * Returns: 0 when symbol is found, -1 when not.
 */
typedef int (*token_callback)( void**, uint16_t*, int*, char*);

extern char *parse_error;
pstack_t  *stack_init( int chunk_size);
void      stack_free( pstack_t *stack);

program_t *program_init();
void      program_free( program_t *program);

void      print_program( program_t *prog);
int       parse_expression( char *string, program_t *program, int *stacksize, uint16_t allowed_type, token_callback token_cb);
void      *execute_data( program_t *program, pstack_t *stack);
#define   execute_number( program, stack)  ((double*)execute_data((program), (stack)))
#define   execute_string( program, stack)  ((char*)execute_data((program), (stack)))
