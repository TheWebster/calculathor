
#define p_realloc( ptr, type, size)   (ptr) = (type*)realloc( (ptr), (size)*sizeof(type))


struct pstack {
	data_t *data;
	data_t *end;
	data_t *top;
	data_t *next;
	
	int    size;
	int    chunk_size;
};


pstack_t *stack_init( int chunk_size);

void     stack_add( pstack_t *stack, content_t data, uint16_t datatype, int link);
#define  stack_addnumber( stack, number)      stack_add( (stack), (content_t)(number), DATA_NUMBER, DATA_NO_LINK)
void     stack_addstring( pstack_t *stack, char *string);
#define  stack_addoperator( stack, operator)  stack_add( (stack), (content_t)((void*)(operator)), DATA_OPERATOR, DATA_NO_LINK)

#define  stack_getoperator( stack)            ((operator_t*)(stack)->top->contents.ptr)

void        stack_del( pstack_t *stack);
double      stack_popnumber( pstack_t *stack);
operator_t *stack_popoperator( pstack_t *stack);

#define  stack_isempty( stack)   ( (stack)->next == (stack)->data )
	
