
void op_add( pstack_t *stack);
void op_sub( pstack_t *stack);
void op_mul( pstack_t *stack);
void op_div( pstack_t *stack);
void op_mod( pstack_t *stack);

extern operator_t op_list[];

operator_t *op_token( char *string, char **next);
