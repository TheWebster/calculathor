typedef void (*op_t)(pstack_t*);

typedef struct {
	char *string;
	int  precedence;
	
	#define OP_TYPE_BINARY        0
	#define OP_TYPE_MINUS         1
	#define OP_TYPE_PLUS          2
	#define OP_TYPE_UNARY_NOT     3
	#define OP_TYPE_OPEN_BRACKET  4
	#define OP_TYPE_CLOSE_BRACKET 5
	int  op_type;
	
	uint16_t needed_type;
	uint16_t returned_type;
	
	op_t function;
} operator_t;

extern operator_t op_list[];
extern operator_t operator_unary_minus;

operator_t *op_token( char *string, char **next);
