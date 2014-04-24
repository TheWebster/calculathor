
typedef union {
	double     number;
	void       *ptr;
} content_t;


#define DATA_HARD_PTR  (1 << 0)
#define DATA_SOFT_PTR  (1 << 1)
#define DATA_PTR       (DATA_HARD_PTR | DATA_SOFT_PTR)

#define DATA_NUMBER    (1 << 3)

typedef struct {
	content_t contents;
	int       type;
} data_t;


typedef struct pstack  pstack_t;


typedef struct {
	pstack_t *code;
	
	char     *error;
} program_t;

#define prog_has_error( prog)   ((prog)->error != NULL)

pstack_t  *stack_init( int chunk_size);
void      stack_free( pstack_t *stack);
void      program_free( program_t *program);
program_t *parse_expression( char *string, int *stacksize);
double    execute_number( program_t *program, pstack_t *stack);
