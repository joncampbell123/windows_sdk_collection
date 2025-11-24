// This is a part of the Microsoft Foundation Classes C++ library.
// Copyright (C) 1992 Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Microsoft Foundation Classes Reference and Microsoft
// QuickHelp documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft Foundation Classes product.

// Globals declarations for OLE CLIENT DEMO APP

#include <afxole.h>
#include "resource.h"

/////////////////////////////////////////////////////////////////////////////
// Misc constants

extern CString strUntitled;
#define OBJ_NAME_PREFIX "O'Client #"

/////////////////////////////////////////////////////////////////////////////
// Default size of embedded object

#define CXDEFAULT       120     /* Default object size */
#define CYDEFAULT       100
void FixObjectBounds(CRect& rect);

/////////////////////////////////////////////////////////////////////////////
// Defines for maximum sizes of files
#define CBMESSAGEMAX    80      /* messages and file names */

/////////////////////////////////////////////////////////////////////////////
