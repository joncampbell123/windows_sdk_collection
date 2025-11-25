// iomanip -- instantiations of iomanip
#include <iomanip>

		// function resetiosflags
static void rsfun(ios_base& iostr, ios_base::fmtflags mask)
	{	// reset specified flags
	iostr.setf(ios_base::_Fmtzero, mask);
	}

_Smanip<ios_base::fmtflags> __cdecl resetiosflags(ios_base::fmtflags mask)
	{	// extractor/inserter to reset specified flags
	return (_Smanip<ios_base::fmtflags>(&rsfun, mask));
	}

		// function setiosflags
static void sifun(ios_base& iostr, ios_base::fmtflags mask)
	{	// set specified flags
	iostr.setf(ios_base::_Fmtmask, mask);
	}

_Smanip<ios_base::fmtflags> __cdecl setiosflags(ios_base::fmtflags mask)
	{	// extractor/inserter to set specified flags
	return (_Smanip<ios_base::fmtflags>(&sifun, mask));
	}

		// function setbase
static void sbfun(ios_base& iostr, int n)
	{	// set base field
	iostr.setf(n == 8 ? ios_base::oct : n == 10 ? ios_base::dec
		: n == 16 ? ios_base::hex : ios_base::_Fmtzero,
			ios_base::basefield);
	}

_Smanip<int> __cdecl setbase(int n)
	{	// extractor/inserter to set base field
	return (_Smanip<int>(&sbfun, n));
	}

		// function setprecision
static void spfun(ios_base& iostr, streamsize n)
	{	// set precision field
	iostr.precision(n);
	}

_Smanip<streamsize> __cdecl setprecision(streamsize n)
	{	// extractor/inserter to set precision field
	return (_Smanip<streamsize>(&spfun, n));
	}

		// function setw
static void swfun(ios_base& iostr, streamsize n)
	{	// set width field
	iostr.width(n);
	}

_Smanip<streamsize> __cdecl setw(streamsize n)
	{	// extractor/inserter to set width field
	return (_Smanip<streamsize>(&swfun, n));
	}

/*
 * Copyright (c) 1994 by P.J. Plauger.  ALL RIGHTS RESERVED. 
 * Consult your license regarding permissions and restrictions.
 */
