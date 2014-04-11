

typedef struct pstack  pstack_t;

pstack_t *stack_init( int chunk_size);
void     stack_free( pstack_t *stack);
pstack_t *parse_expression( char *string);
double   execute_number( pstack_t *program, pstack_t *stack);
