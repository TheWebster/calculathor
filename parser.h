

typedef struct pstack  pstack_t;


typedef struct {
	pstack_t *code;
	
	char     *error;
} program_t;

#define prog_has_error( prog)   ((prog)->error != NULL)

pstack_t  *stack_init( int chunk_size);
void      stack_free( pstack_t *stack);
void      program_free( program_t *program);
program_t *parse_expression( char *string);
double    execute_number( program_t *program, pstack_t *stack);
