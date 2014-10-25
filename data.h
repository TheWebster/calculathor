/* ****************************************************************** *\
 * data.h                                                             *
 *                                                                    *
 * Project:     CalculaThor                                           *
 * Author:      Christian Weber (ChristianWeber802@gmx.net)           *
 *                                                                    *
 * Description: Definition of data structure.                         *
\* ****************************************************************** */
typedef union {
	double     number;
	double     *ptr_number;
	char       *string;
	program_t  *prog;
	void       *ptr;
	int        integer;
	operator_t *op;
} content_t;

typedef struct {
	content_t contents;
	int       link;
	uint16_t  type;
} data_t;



#define DATA_OPERATOR      3
