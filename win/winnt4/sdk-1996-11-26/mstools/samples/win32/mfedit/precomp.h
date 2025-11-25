/******************************Module*Header*******************************\
* Module Name: precomp.h
*
* Copyright (c) 1996 Microsoft Corporation
*
\**************************************************************************/
#include <stdlib.h>
#include <windows.h>
#include <string.h>
#include <stdarg.h>
#include <string.h>

#ifdef JAPAN
#define _MBCS
#include <mbstring.h>
#include <mbctype.h>
//
// Maybe this needs to be the last include
//
#include <ime.h>
#define My_mbstok _mbstok	// This may actually work for English as well
#else
#define My_mbstok strtok	// but just to be sure...
#endif

#include <stdio.h>
#include <commdlg.h>
#include <shellapi.h>
#include <math.h>
#include <gl\glaux.h>
#include <gl\gl.h>
#include <gl\glu.h>
#include "mfedit.h"
