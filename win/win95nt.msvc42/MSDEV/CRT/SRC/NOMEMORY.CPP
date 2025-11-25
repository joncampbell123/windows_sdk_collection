// nomemory -- report out of memory
#include <new>

void __cdecl _Nomemory()
	{	// report out of memory
	static const bad_alloc nomem;
	_THROW(nomem);
	}


/*
 * Copyright (c) 1994 by P.J. Plauger.  ALL RIGHTS RESERVED. 
 * Consult your license regarding permissions and restrictions.
 */
