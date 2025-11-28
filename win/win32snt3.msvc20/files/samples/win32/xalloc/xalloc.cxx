/*
 * This code demonstrates the standard exception class hierarchy (using
 * xalloc) by showing what happens when the 'new' operator fails. 
 *
 * To use standard exceptions in your code, include "stdexcept.h" and add 
 * stdexcpt.cxx (hierarchy) and stdnewh.cxx (xalloc) to your project.
 */

#include <stdlib.h>
#include <new.h>
#include <eh.h>
#include <stdio.h>
#include <conio.h>
#include <malloc.h>
#include "stdexcpt.h"
#include "xalloc.h"

// Global variable controlling whether 'new' fails
int failNew = 0;  

// This function overrides the 'new' operator with a version that fails
// when failNew is true.
void* operator new( size_t size )
{
	void *result;

	if	( failNew
		 ||
		  ( result = malloc(size) ) == NULL
	) {
		_PNH handler = _query_new_handler();

		if	( handler ) {
			(*handler)(size);
		}
	
		return NULL;
	}

	return result;
}

// This function tests the 'new' operator, checking for an xalloc
// exception and displaying a message
void OperatorNewTest (void)
{
	try {
		int *pi = new int;
	}
	// Catch by value
	catch (xalloc xal) {	
		printf("Memory allocation failure: %s\n", xal.what());
		// Not handled, so throw exception to next handler
		throw;
	}
}

// This function throws a user-defined NonPositive exception if
// it is passed a nonpositive parameter.
void PositiveTest (int value)
{
	if (value<=0)
		{
		static NonPositive ex("PositiveTest() nonpositive parameter");
		ex.raise();
		}
	while( value-- > 0)
		printf("*");
	printf("\n");
}

void main (void)
{
	// Enable xalloc exception for 'new' operator
	_set_new_handler( _standard_new_handler );

	// Demonstrate what happens when 'new' fails:
	failNew = 1;

	try {
		OperatorNewTest();
	}
	catch (exception& ex) {
		printf( "Exception caught: %s\n", ex.what() );
	}

	failNew = 0;

	// Demonstrate a user-defined exception
	try {
 		PositiveTest(7);
		PositiveTest(0);
 	}
	catch (exception& ex) {
		printf( "Exception caught: %s\n", ex.what() );
	}
	printf("\nPress any key to continue...");
	_getch();
}

