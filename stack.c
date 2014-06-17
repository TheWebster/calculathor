
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "parser.h"
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
 * Clears a pstack structure. After clearing it can be used as if it was just created.
 * 
 * Parameters: stack - The pstack structure to be cleared.
 */
void
stack_clear( pstack_t *stack)
{
	stack->next = stack->data;
	stack->top  = stack->data - 1;
};


/*
 * Frees all the memory allocated to a pstack structure.
 * 
 * Parameters: stack - The pstack structure to be freed.
 */
void
stack_free( pstack_t *stack)
{
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
stack_add( pstack_t *stack, content_t data, int datatype)
{
	if( stack->top == stack->end )
		stack_realloc( stack);
	
	stack->next->contents = data;
	stack->next->type     = datatype;
	
	stack->next++;
	stack->top++;
};


/*
 * Deletes an element of data from a pstack structure.
 * 
 * Parameters: stack - I'm fuckin' sick of typing this bullshit all over again and again and again. Just use some common sense.
 */
void
stack_del( pstack_t *stack)
{
	stack->next--;
	stack->top--;
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



