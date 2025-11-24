// dobject.cpp : Defines the class behaviors for the Chart data objects.
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

#include "chart.h"

/////////////////////////////////////////////////////////////////////////////
// CChartObject

// CChartObject is a container class of the chart's title and list of
// data points in the graph.  The list is a CObList (see afxcoll.h)
// of CChartData points.

IMPLEMENT_SERIAL(CChartObject, CObject, 0)

// Constructor:
// Creates the list for the CChartData objects
//
CChartObject::CChartObject()
{
	m_pChartData = new CObList;
	m_nType = IDM_BAR;
	m_bDirty = FALSE;
}


// Destructor:
// Delete all the contents, too.
//
CChartObject::~CChartObject()
{
	// Removes & Deletes all objects contained in the chart data list
	//
	RemoveAll();        
	delete m_pChartData;
}

// Serialize:
//
void CChartObject::Serialize(CArchive& ar)
{
	// Serialize the base object first.
	//
	CObject::Serialize(ar);

	// Input or output our items to the archive.
	//
	if(ar.IsStoring())
	{
		ar << m_Title;
		ar << m_pChartData;
		ar << m_nType;
	}
	else
	{
		RemoveAll();
		delete m_pChartData;
			// Delete old data before reading in new data
		ar >> m_Title;
		ar >> m_pChartData;
		ar >> m_nType;
	}
}

// RemoveAll:
// A quick way of clearing all of the elements in the m_pChartData member.
//
void CChartObject::RemoveAll()
{
	if (m_pChartData != NULL)
	{
		if (m_pChartData->GetCount() > 0)
		{
			// Delete the elements.
			//
			while (!m_pChartData->IsEmpty())
			{
				CChartData* ptr;
				ptr = (CChartData*)m_pChartData->RemoveHead();
				delete ptr;
			}

			// Delete all the list's pointers to the now deleted elements.
			//
			m_pChartData->RemoveAll();
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// CChartData

// CChartData is a data point for the chart.  It contains two fields.
// The height is the value of the data point and szName is the label
// for the data point.

IMPLEMENT_SERIAL(CChartData, CObject, 0)

// Serialize:
//
void CChartData::Serialize(CArchive& ar)
{
	// Serialize the base object first.
	//
	CObject::Serialize(ar);
	
	// Input or output our items to the archive.
	//
	if(ar.IsStoring())
	{
		ar << (WORD)height;
		ar << szName;
	}
	else
	{
		WORD tmp;
		CString name;
		ar >> tmp;
		height = (short)tmp;
		ar >> name;
		strcpy(szName,name);
	}
}
