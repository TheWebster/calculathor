
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "parser.h"
#include "op.h"
#include "stack.h"


/*
 * Increases the size of a given pstack strucure by its chunksize.
 * 
 * Parameters: stack - The pstack structure to be resized.
 */
static void
stack_realloc( pstack_t *stack)
{
	p_realloc( stack->data, data_t, stack->size + stack->chunk_size);
	
	stack->top  = stack->data + stack->size - 1;
	stack->next = stack->data + stack->size;
	
	stack->size += stack->chunk_size;
	stack->end  = stack->data + stack->size - 1;
};


/*
 * Allocates a pstack structure with agiven chunksize.
 * 
 * Parameters: chunk_size - The chunksize of the data member of the struct.
 * 
 * Returns: The allocated struct.
 */
pstack_t *
stack_init( int chunk_size)
{
	pstack_t *stack = (pstack_t*)malloc( sizeof(pstack_t));
	
	
	memset( stack, 0, sizeof(pstack_t));
	
	stack->chunk_size = chunk_size;
	stack_realloc( stack);
	
	return stack;
};


/*
 * Deletes an element of data from a pstack structure.
 * 
 * Parameters: stack - I'm fuckin' sick of typing this bullshit all over again and again and again. Just use some common sense.
 */
void
stack_del( pstack_t *stack)
{
	if( stack->top->type == DATA_STRING && stack->top->link == DATA_NO_LINK )
		free( stack->top->contents.string);
		
	stack->next--;
	stack->top--;
};


/*
 * Clears a pstack structure. After clearing it can be used as if it was just created.
 * 
 * Parameters: stack - The pstack structure to be cleared.
 */
void
stack_clear( pstack_t *stack)
{
	while( !stack_isempty( stack) )
		stack_del( stack);
};


/*
 * Frees all the memory allocated to a pstack structure.
 * 
 * Parameters: stack - The pstack structure to be freed.
 */
void
stack_free( pstack_t *stack)
{
	stack_clear( stack);
	free( stack->data);
	free( stack);
};


/*
 * Adds data to a pstack structure. When structure is full it is
 * reallocated before adding data.
 * 
 * Parameters: stack    - The pstack structure to add data to.
 *             data     - The actual data.
 *             datatype - Type of the data.
 */
void
stack_add( pstack_t *stack, content_t data, uint16_t datatype, int link)
{
	if( stack->top == stack->end )
		stack_realloc( stack);
	
	stack->next->contents = data;
	stack->next->type     = datatype;
	stack->next->link     = link;
	
	stack->next++;
	stack->top++;
};


/*
 * Adds a string to a pstack structure. When structure is full it is
 * reallocated before adding data.
 * 
 * Parameters: stack    - The pstack structure to add data to.
 *             string   - The string to add.
 */
void
stack_addstring( pstack_t *stack, char *string)
{
	char *ptr = (char*)malloc( strlen( string)+1);
	
	strcpy( ptr, string);
	stack_add( stack, (content_t)ptr, DATA_STRING, DATA_NO_LINK);
};


/*
 * Deletes a number from a pstack structure and returns its value.
 * 
 * Parameters: No.
 * 
 * Returns: The number, silly.
 */
double
stack_popnumber( pstack_t *stack)
{
	double number = stack->top->contents.number;
	
	stack_del( stack);
	
	return number;
};

/*
 * Deletes an operator from a pstack structure and returns it.
 */
operator_t *
stack_popoperator( pstack_t *stack)
{
	operator_t *operator = (operator_t*)stack->top->contents.ptr;
	
	stack_del( stack);
	
	return operator;
};



