/*** 
*dispdemo.h
*
*  Copyright (C) 1992, Microsoft Corporation.  All Rights Reserved.
*  Information Contained Herein Is Proprietary and Confidential.
*
*Purpose:
*  IDispatch Demo App definitions.
*
*Implementation Notes:
*
*****************************************************************************/

#include "hostenv.h"
#include "resource.h"
#include "clsid.h"

#ifdef _MAC
# define UNUSED(X) ((void)(void*)&(X))
#else
# define UNUSED(X) (X)
#endif

#ifndef NEAR
# define NEAR
#endif

#define DIM(X) (sizeof(X) / sizeof(X[0]))

#define ASSERT(X) ChkAssert(X, __FILE__, __LINE__)


#ifdef __cplusplus
extern "C" {
#endif

STDAPI InitOle(void);
STDAPI UninitOle(void);

STDAPI DoPoly(CLSID);

void
ChkAssert(int, char*, int);

#ifdef _MAC
void DbPrintf(char*, ...);
#endif

#ifdef __cplusplus
}
#endif

