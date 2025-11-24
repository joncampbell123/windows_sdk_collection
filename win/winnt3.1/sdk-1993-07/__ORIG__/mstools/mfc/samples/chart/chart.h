// chart.h : Declares the class interfaces for the Chart application.
//
// This is a part of the Microsoft Foundation Classes C++ library.
// Copyright (C) 1992 Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Microsoft Foundation Classes Reference and Microsoft
// QuickHelp documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft Foundation Classes product.
//

#ifndef __CHART_H__
#define __CHART_H__

#include <afxwin.h>
#include <afxcoll.h>
#include <afxdlgs.h>

#include "resource.h"
#include "dobject.h"
#include "chartwnd.h"
#include "chartdlg.h"

/////////////////////////////////////////////////////////////////////////////

class CTheApp : public CWinApp
{
public:
	virtual BOOL InitInstance();
	virtual ~CTheApp();
};

/////////////////////////////////////////////////////////////////////////////

#endif // __CHART_H__
