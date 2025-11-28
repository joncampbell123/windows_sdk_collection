/***
*xalloc.h - Sample user include file for standard exception classes
*
*	Copyright (c) 1994, Microsoft Corporation. All rights reserved.
*
*Purpose:
*	This file presents an interface to the standard exception classes,
*	as specified by the ANSI X3J16/ISO SC22/WG21 Working Paper for
*	Draft C++, January 1994, and ammended in March 1994.
*
****/

//
// Standard exception class heirarchy (ref. 1/94 WP 17.3.2.1, as ammended 3/94).
//
// exception (formerly xmsg)
//   logic
//     domain
//   runtime
//     range
//     alloc
//       xalloc
//

typedef const char *__exString;

class  exception 
{
public:
	typedef void (*raise_handler)(exception&);
	static raise_handler set_raise_handler(raise_handler);

	exception(const __exString&);
	exception(const exception&);
	exception& operator= (const exception&);
	virtual ~exception();

	void raise()	{ handle_raise();	throw *this;	};
	virtual __exString what() const;

protected:
	exception();
	virtual void do_raise();
	void handle_raise();

private:
	__exString m_what;
	int m_doFree;
	static raise_handler m_handler;
};

typedef exception xmsg;		// A synonym for folks using older standard

//
//  logic
//
class  logic: public exception 
{
public:
	logic(const __exString& what): exception(what) {};
	void raise()	{ handle_raise();	throw *this;	};
};

//
//   domain
//
class  domain: public logic
{
public:
	domain(const __exString& what): logic(what) {};
	void raise()	{ handle_raise();	throw *this;	};
};

//
//  runtime
//
class  runtime: public exception
{
public:
	runtime(const __exString& what): exception(what) {};
	void raise()	{ handle_raise();	throw *this;	};

protected:
	runtime(): exception() {};
};

//
//   range
//
class  range: public runtime
{
public:
	range(const __exString& what): runtime(what) {};
	void raise()	{ handle_raise();	throw *this;	};
};

//
//   alloc
//
class  alloc: public runtime
{
public:
	alloc(): runtime() {};
	void raise()	{ handle_raise();	throw *this;	};
};

//
//     xalloc
//
class  xalloc: public alloc
{
public:
	xalloc();
	virtual __exString what (void) const;
	void raise()	{ handle_raise();	throw *this;	};
};

//
// The function to install with set_new_handler in order for 'new' to throw xalloc
// upon failure to allocate memory.
//
int __cdecl _standard_new_handler( size_t );

