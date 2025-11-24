// This is a part of the Microsoft Foundation Classes C++ library. 
// Copyright (C) 1992 Microsoft Corporation 
// All rights reserved. 
//  
// This source code is only intended as a supplement to the 
// Microsoft Foundation Classes Reference and Microsoft 
// QuickHelp documentation provided with the library. 
// See these sources for detailed information regarding the 
// Microsoft Foundation Classes product. 

#ifdef _WINDOWS
#include "afxwin.h"
#else
#include "afx.h"
#endif
#pragma hdrstop

#include <stdlib.h>

#ifdef AFX_CORE_SEG
#pragma code_seg(AFX_CORE_SEG)
#endif

/*
 *  NOTE: in separate module so if can be replaced if needed
 */

void CDECL AfxAbort()
{
	TRACE("AfxAbort called\n");
#ifdef _WINDOWS
	AfxWinTerm();
#endif
	abort();
}
