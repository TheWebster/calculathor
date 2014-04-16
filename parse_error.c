
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

char *parse_error;


/*
 * Concatenate a string and copy the pointer to parse_error
 * 
 * Parameters: See printf.
 */
void
parse_set_error( char *format, ...)
{
	va_list vargs;
	int     size;
	
	va_start( vargs, format);
	size = vsnprintf( parse_error, 0, format, vargs);
	va_end( vargs);
	
	va_start( vargs, format);
	parse_error = (char*)malloc( size + 1);
	vsnprintf( parse_error, size+1, format, vargs);
	va_end(vargs);
};
	
