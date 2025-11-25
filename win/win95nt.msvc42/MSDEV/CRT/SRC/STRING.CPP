// string -- template string support functions
#include <istream>

		// report a length_error
void __cdecl _Xlen()
	{_THROW(length_error("string too long")); }

		// report an out_of_range error
void __cdecl _Xran()
	{_THROW(out_of_range("invalid string position")); }

/*
 * Copyright (c) 1995 by P.J. Plauger.  ALL RIGHTS RESERVED. 
 * Consult your license regarding permissions and restrictions.
 */
