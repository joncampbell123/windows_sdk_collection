/*** 
*hostenv.h
*
*  Copyright (C) 1992-1994, Microsoft Corporation.  All Rights Reserved.
*
*Purpose:
*  Generic host specific includes.
*
*Implementation Notes:
*
*****************************************************************************/

# include <windows.h>
# include <ole2.h>
# include <dispatch.h>

# if defined(UNICODE)
    #define TCHAR		WCHAR
    #define TSTR(str)		L##str
    #define STRING(str)	        (str)	    
    #define WIDESTRING(str)	(str)	    	    
# else
    #define TCHAR		char
    #define TSTR(str)		str	
    #define STRING(str)	        AnsiString(str)
    #define WIDESTRING(str)	WideString(str)	    	    
    extern "C" char FAR* AnsiString(OLECHAR FAR* strIn);
    extern "C" OLECHAR FAR* WideString(char FAR* strIn);
# endif

