// This is a part of the Microsoft Foundation Classes C++ library. 
// Copyright (C) 1992 Microsoft Corporation 
// All rights reserved. 
//  
// This source code is only intended as a supplement to the 
// Microsoft Foundation Classes Reference and Microsoft 
// QuickHelp documentation provided with the library. 
// See these sources for detailed information regarding the 
// Microsoft Foundation Classes product. 

// Ths module serves two purposes:
//    1. Provide a function that returns the current version
//       number of the AFX library.
//    2. Provide a guaranteed exported symbol in an application
//       to work around a loader problem in Windows 3.0.  This
//       module must be compiled with the /GEe switch in C7.00.

#include "afx.h"
#pragma hdrstop

#ifdef _WINDOWS
extern "C" int FAR PASCAL __export _afx_version()
#else
extern "C" int FAR PASCAL _afx_version()
#endif
{ 
	return _MFC_VER;
}
