
/******************************************************************************\
*       This is a part of the Microsoft Source Code Samples. 
*       Copyright (C) 1993-1995 Microsoft Corporation.
*       All rights reserved. 
*       This source code is only intended as a supplement to 
*       Microsoft Development Tools and/or WinHelp documentation.
*       See these sources for detailed information regarding the 
*       Microsoft samples programs.
\******************************************************************************/

#include "resource.h"

#ifndef TEXT
// this is non-Unicode Win16 build
#define CHAR   char
#define UCHAR  unsigned char
#define TCHAR  CHAR
#define LPTSTR LPSTR
#define TEXT(x) x
#define COORD   int

#else
#define COORD   LONG
#define HINSTANCE HMODULE
#endif


// Manifest Constants
#define DONE TRUE
#define INCH1     1440
#define INCH2      720  // half
#define INCH4      360  // quarter
#define INCH8      180  // eigth
#define INCH10     144  // tenth

// Paper Size: Redefine for non-US
#define PAGEWIDTH  (8*INCH1+INCH2)
#define PAGEHEIGHT (11*INCH1)

