/***
*txtmode.c - set global text mode flag
*
*       Copyright (c) 1989-1994, Microsoft Corporation. All rights reserved.
*
*Purpose:
*       Sets the global file mode to text.  This is the default.
*
*******************************************************************************/

#ifndef DLL_FOR_WIN32S

#include <cruntime.h>
#include <stdlib.h>

int _fmode = 0;                 /* set text mode */

#endif  /* DLL_FOR_WIN32S */
