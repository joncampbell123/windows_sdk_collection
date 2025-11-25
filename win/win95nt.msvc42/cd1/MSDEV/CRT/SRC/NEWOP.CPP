// newop operator new(size_t) for Microsoft C++
#include <cstdlib>
#include <new>

_C_LIB_DECL
int _callnewh(size_t size);
_END_C_LIB_DECL

#define RESERVE_SIZE	256	/* for out-of-heap handling */

void *__cdecl operator new(size_t size) _THROW1(::bad_alloc)
	{	// try to allocate size bytes
	static void *pres = 0;
		{::_Lockit _Lk;
		if (pres == 0)
			pres = malloc(RESERVE_SIZE);
		}
	void *p;
	while ((p = malloc(size)) == 0)
		{	// handle failure to allocate
			{::_Lockit _Lk;
			if (pres != 0)
				{	// free reserve space
				free(pres);
				pres = 0;
				}
			}
		if (_callnewh(size) == 0)
			break;
		}
	if (p == 0)
		::_Nomemory();
	return (p);
	}

/*
 * Copyright (c) 1994 by P.J. Plauger.  ALL RIGHTS RESERVED. 
 * Consult your license regarding permissions and restrictions.
 */
