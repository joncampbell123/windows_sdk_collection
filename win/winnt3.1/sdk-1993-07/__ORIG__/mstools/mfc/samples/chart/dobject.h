// dobject.h : Declares Chart data object classes.
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

#ifndef __DOBJECT_H__
#define __DOBJECT_H__

/////////////////////////////////////////////////////////////////////////////
// CChartData
// One data point for the chart.

class CChartData : public CObject
{
	DECLARE_SERIAL(CChartData)

public:
	short  height;
	char   szName[40];

	void Serialize(CArchive&);
};

/////////////////////////////////////////////////////////////////////////////
// CChartObject

class CChartObject : public CObject
{
	DECLARE_SERIAL(CChartObject)

public:

	CString m_Title;
	CObList* m_pChartData;
	WORD m_nType;
	BOOL m_bDirty;
	
	CChartObject();
	~CChartObject();
	void Serialize(CArchive&);
	void RemoveAll();
};

/////////////////////////////////////////////////////////////////////////////

#endif // __DOBJECT_H__
