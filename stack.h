
#define p_realloc( ptr, type, size)   (ptr) = (type*)realloc( (ptr), (size)*sizeof(type))
typedef void (*op_t)(pstack_t*);

typedef struct {
	char *string;
	int  precedence;
	op_t function;
} operator_t;

typedef union {
	double     number;
	void       *ptr;
	operator_t *operator;
} data_t;

typedef enum {
	DATA_TYPE_NUMBER,
	DATA_TYPE_PTR,
	DATA_TYPE_OPERATOR
} data_type_t;

typedef struct {
	data_t      contents;
	data_type_t type;
} byte_t;

struct pstack {
	byte_t *data;
	byte_t *end;
	byte_t *top;
	byte_t *next;
	
	int    size;
	int    chunk_size;
};


pstack_t *stack_init( int chunk_size);

void     stack_add( pstack_t *stack, data_t data, data_type_t datatype);
#define  stack_addnumber( stack, number)      stack_add( (stack), (data_t)(number), DATA_TYPE_NUMBER)
#define  stack_addoperator( stack, operator)  stack_add( (stack), (data_t)(operator), DATA_TYPE_OPERATOR)

#define  stack_getoperator( stack)            ((stack)->top->contents.operator)

void        stack_del( pstack_t *stack);
double      stack_popnumber( pstack_t *stack);
operator_t *stack_popoperator( pstack_t *stack);

#define  stack_isempty( stack)   ( (stack)->next == (stack)->data )
	
