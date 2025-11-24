/*****************************************************************/ 
/**               Microsoft Windows 95                          **/
/**           Copyright (C) Microsoft Corp., 1991-1995          **/
/*****************************************************************/ 

/*
    assert.h
    environment independent assertion/logging routines

    Usage:

        ASSERT(exp)     Evaluates its argument.  If "exp" evals to
                        FALSE, then the app will terminate, naming
                        the file name and line number of the assertion
                        in the source.

        UIASSERT(exp)   Synonym for ASSERT.

        ASSERTSZ(exp,sz) As ASSERT, except will also print the message
                        "sz" with the assertion message should it fail.

        REQUIRE(exp)    As ASSERT, except that its expression is still
                        evaluated in retail versions.  (Other versions
                        of ASSERT disappear completely in retail builds.)

    The ASSERT macros expect a symbol _FILENAME_DEFINED_ONCE, and will
    use the value of that symbol as the filename if found; otherwise,
    they will emit a new copy of the filename, using the ANSI C __FILE__
    macro.  A client sourcefile may therefore define __FILENAME_DEFINED_ONCE
    in order to minimize the DGROUP footprint of a number of ASSERTs.

*/

#ifndef _NPASSERT_H_
#define _NPASSERT_H_

#if defined(__cplusplus)
extern "C"
{
#else
extern
#endif

void UIAssertHelper( PSTR pszFileName, UINT nLine );
void UIAssertSzHelper( PSTR pszMessage, PSTR pszFileName, UINT nLine );

#if defined(__cplusplus)
}
#endif

#if defined(DEBUG)

# if defined(_FILENAME_DEFINED_ONCE)

#  define ASSERT(exp) \
    { if (!(exp)) UIAssertHelper(_FILENAME_DEFINED_ONCE, __LINE__); }

#  define ASSERTSZ(exp, sz) \
    { if (!(exp)) UIAssertSzHelper((sz), _FILENAME_DEFINED_ONCE, __LINE__); }

# else

#  define ASSERT(exp) \
    { if (!(exp)) UIAssertHelper(__FILE__, __LINE__); }

#  define ASSERTSZ(exp, sz) \
    { if (!(exp)) UIAssertSzHelper((sz), __FILE__, __LINE__); }

# endif

# define UIASSERT(exp)  ASSERT(exp)
# define REQUIRE(exp)   ASSERT(exp)

#else // !DEBUG

# define ASSERT(exp)        ;
# define UIASSERT(exp)      ;
# define ASSERTSZ(exp, sz)  ;
# define REQUIRE(exp)       { (exp); }

#endif // DEBUG

#endif // _NPASSERT_H_
