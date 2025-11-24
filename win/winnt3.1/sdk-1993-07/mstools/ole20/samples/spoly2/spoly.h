/*** 
*spoly.h - Application-wide definitions
*
*  Copyright (C) 1992, Microsoft Corporation.  All Rights Reserved.
*  Information Contained Herein Is Proprietary and Confidential.
*
*Purpose:
*
*Implementation Notes:
*
*****************************************************************************/

#include "clsid.h"

#define IDM_CLEAR	1
#define IDM_DUMP	2

#define IDM_FIRSTCHILD	100

#define DIM(X) (sizeof(X) / sizeof(X[0]))

#ifndef EXPORT
# define EXPORT __export
#endif

#if defined(WIN32)
# define CC_CALL        CC_STDCALL
# define METHODCALLTYPE __stdcall
#else
# define CC_CALL        CC_PASCAL
# define METHODCALLTYPE __pascal
#endif
