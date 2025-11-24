// This is a part of the Microsoft Foundation Classes C++ library.
// Copyright (C) 1992 Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Microsoft Foundation Classes Reference and Microsoft
// QuickHelp documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft Foundation Classes product.


/////////////////////////////////////////////////////////////////////////////

#ifndef __AFXOLE_H__
#include <afxole.h>
#endif

/////////////////////////////////////////////////////////////////////////////
// CBibServer:

class CBibServer : public COleServer
{
public:
	CBibServer() : COleServer(FALSE)
		{ }

	void    SetLaunchEmbedded()
			{ m_bLaunchEmbedded = TRUE; }

protected:
// Overridables for OLE Server requests
	virtual COleServerDoc* OnOpenDoc(LPCSTR lpszDoc);
	virtual COleServerDoc* OnCreateDoc(LPCSTR lpszClass, LPCSTR lpszDoc);
	virtual COleServerDoc* OnEditDoc(LPCSTR lpszClass, LPCSTR lpszDoc);
};

/////////////////////////////////////////////////////////////////////////////

