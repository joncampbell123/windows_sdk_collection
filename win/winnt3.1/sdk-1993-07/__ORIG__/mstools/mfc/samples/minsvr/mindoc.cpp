// This is a part of the Microsoft Foundation Classes C++ library.
// Copyright (C) 1992 Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Microsoft Foundation Classes Reference and Microsoft
// QuickHelp documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft Foundation Classes product.

#include "minsvr.h"

/////////////////////////////////////////////////////////////////////////////
// OLE Server functionality

COleServerItem* CMinDoc::OnGetDocument()
{
	return &m_item;
}

COleServerItem* CMinDoc::OnGetItem(LPCSTR)
{
	// embedded only server - no linked items
	return NULL;
}

/////////////////////////////////////////////////////////////////////////////

COleServerItem* CMinDoc::GetNextItem(POSITION& rPosition)
{
	if (rPosition == NULL)
	{
		rPosition = (POSITION) 1;
		return &m_item;
	}
	else
	{
		return NULL;
	}
}

/////////////////////////////////////////////////////////////////////////////
