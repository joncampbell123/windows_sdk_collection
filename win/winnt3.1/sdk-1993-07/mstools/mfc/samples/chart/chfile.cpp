// chfile.cpp : Defines the file input and output logic.
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

#include <commdlg.h>

/////////////////////////////////////////////////////////////////////////////

// LoadFile:
// Load the named file.  Uses Foundation archive/serialization format.
//
int CChartWnd::LoadFile(const char* pName)
{
	CFile file;
	
	// The file has a title, so reset the UNTITLED flag.
	//
	m_bUntitled = FALSE;
	
	if (!file.Open(pName, CFile::modeRead))
	{
		MessageBox("Not able to open file.","Chart",
			MB_ICONEXCLAMATION | MB_OK);
		return FALSE;
	}

	// Read the file into the buffer.
	// If it fails, inform the user.
	//

	if (m_pChartObject != NULL)
	{
		delete m_pChartObject;
		m_pChartObject = NULL;
	}

	// Set up a CArchive to serialize in the data.
	CArchive ar(&file, CArchive::load);

	TRY
	{
		m_bChartSerializedOK = TRUE;
		ar >> m_pChartObject;
		ASSERT(m_pChartObject->m_bDirty == FALSE);
	}
	CATCH(CArchiveException, e)
	{
		MessageBox("Not able to read from file.", "Chart",
			MB_ICONEXCLAMATION | MB_OK);

		m_bChartSerializedOK = FALSE;
	}
	AND_CATCH(CFileException, e)
	{
		MessageBox("Not able to read from file.", "Chart",
			MB_ICONEXCLAMATION | MB_OK);

		m_bChartSerializedOK = FALSE;
	}
	END_CATCH

	ar.Close();
	file.Close();

	return m_bChartSerializedOK;
}


// FileDlg:
// Put up an open or save file dialog, collect response (passed back
// via szFile parameter).  
//
BOOL CChartWnd::FileDlg( BOOL bOpen, int nMaxFile, LPSTR szFile)
{
	char szFilter[] = "Chart Files (*.cht)|*.cht||";
	CFileDialog fileDialog(bOpen, "cht", szFile, OFN_HIDEREADONLY, szFilter);

	if (fileDialog.DoModal() == IDCANCEL)
		return FALSE;
	_fstrcpy(szFile, fileDialog.GetPathName());
	return TRUE;
}




// ReadFile:
// Prompt user for a filename and try to read in the contained chart.
//
#define FILENAMELEN 128

void CChartWnd::ReadFile()
{
   
	// Set the length of the CString, m_szFileName, to the maximum 
	// file length.  This prevents overwriting.

	// If there's no filename, the dialog was canceled.
	//
	if (!FileDlg(TRUE, FILENAMELEN, m_szFileName.GetBuffer(FILENAMELEN)))
	{
		return;
	}

	// Reset the length of the CString, m_szFileName, to the actual
	// length of the file name.

	m_szFileName.ReleaseBuffer();

	// Load file into this window.
	//
	LoadFile(m_szFileName);
}

// SaveFile:
// Prompt user for a save file name and write the chart to the file
//
void CChartWnd::SaveFile(BOOL bNewFileName)
{
	CFile file;

	// Set the length of the buffer to the maximum file length.  By
	// doing this, possible memory overwriting is prevented.

	if (bNewFileName)
	{   
		if (m_bUntitled)
		{
			// set a default string if the filename has not
			// been set.
			m_szFileName = "chart.cht";
		}

		if (!FileDlg(FALSE, FILENAMELEN, 
			m_szFileName.GetBuffer(FILENAMELEN)))
		{
			return;
		}
	}

	// Have the m_szFileName "shrink" to the actual length of the
	// file name.

	m_szFileName.ReleaseBuffer();

	ASSERT(m_szFileName != "");
	m_bUntitled = FALSE;

	if (!file.Open(m_szFileName, CFile::modeCreate | CFile::modeWrite))
	{
		MessageBox("Not able to create file.", "Chart",
			MB_ICONEXCLAMATION | MB_OK);
		return;
	}
	
	// Write out the contents of the buffer to the file.
	// If this fails, inform the user.
	//
	CArchive ar(&file, CArchive::store);

	TRY
	{
		ar << m_pChartObject;
	}
	CATCH(CFileException, e)
	{
		MessageBox("Not able to write to file.", "Chart",
			MB_ICONEXCLAMATION | MB_OK);
	}
	AND_CATCH(CArchiveException, e)
	{
		MessageBox("Not able to write to file.", "Chart",
			MB_ICONEXCLAMATION | MB_OK);
	}
	END_CATCH

	ar.Close();
	file.Close();
	m_pChartObject->m_bDirty = FALSE;
}
