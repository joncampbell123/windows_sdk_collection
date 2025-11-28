/***
*stdexcpt.cxx - defines C++ standard exception classes
*
*	Copyright (c) 1994, Microsoft Corporation.  All rights reserved.
*
*Purpose:
*	Implementation of C++ standard exception classes, as specified in 
*	[lib.header.exception] (section 17.3.2 of 1/25/94 WP), as amended 
*	in San Diego on 3/10/94:
*
*	 exception (formerly xmsg)
*	   logic
*	     domain
*	   runtime
*	     range
*	     alloc
*
*Revision History:
*	04-27-94  BES   Module created.
*
*******************************************************************************/

#include <stdlib.h>
#include <string.h>
#include <eh.h>
#include "stdexcpt.h"

////////////////////////////////////////////////////////////////////////////////
//
// Implementation of class "exception"
//

exception::raise_handler exception::m_handler = NULL;

//
// Default constructor - initialize to blank
//
exception::exception ()
{
	m_what = NULL;
	m_doFree = 0;
}

//
// Standard constructor: initialize with copy of string
//
exception::exception ( const __exString& what )
{
	m_what = new char[strlen(what)+1];
	if	( m_what == NULL ) 
	{
		// 'new' didn't throw, so we'll do it
		_standard_new_handler(0);
	}
	strcpy( (char*)m_what, what );
	m_doFree = 1;
}

//
// Copy constructor
//
exception::exception ( const exception & that )
{
	m_doFree = that.m_doFree;
	if	( m_doFree )
	{
		m_what = new char[strlen(that.m_what) + 1];
		if	( m_what == NULL ) 
		{
			// 'new' didn't throw, so we'll do it
			_standard_new_handler(0);
		}
		strcpy( (char*)m_what, that.m_what );
	}
	else
		m_what = that.m_what;
}

//
// Assignment operator: destruct, then copy-construct
//
exception& exception::operator=( const exception & that )
{
	if	(this != &that) 
	{
		this->exception::~exception();
		this->exception::exception(that);
	}

	return *this;
}

//
// Destructor: free the storage used by the message string if it was
// dynamicly allocated
//
exception::~exception()
{
	if	( m_doFree )
	{
		delete[] (char*)m_what;
	}
}

//
// exception::handle_raise
//  Do all the standard stuff for 'raise()'; all implementations of 'raise()' 
//  must call this.
//
void exception::handle_raise ( void )
{
	if	( m_handler != NULL )
	{
		(*m_handler)(*this);
	}

	do_raise();
}

//
// exception::what
//  Returns the message string of the exception.
//  Default implementation of this method returns the stored string if there
//  is one, otherwise returns a standard string.
//
__exString exception::what ( void ) const
{
	if	( m_what != NULL )
	{
		return m_what;
	}
	else
	{
		return "Unknown exception";
	}
}

//
// exception::do_raise
//  Does per-class special processing when an exception is raised.
//  Default implementation does nothing.
//
void exception::do_raise ( void )
{
	return;
}

///////////////////////////////////////////////////////////////////////////////
//
// Implementation of class "xalloc"
//

//
// Default constructor - use base's default constructor
//  Thus xalloc exception has no message string stored in it.
//
xalloc::xalloc (): alloc()
{
}

//
// xalloc::what
//  Returns a fixed string, since the class doesn't store one.
//
__exString xalloc::what ( void ) const
{
	return "Out of memory in 'operator new'";
}
