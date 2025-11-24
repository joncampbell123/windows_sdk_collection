// This is a part of the Microsoft Foundation Classes C++ library.
// Copyright (C) 1992 Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Microsoft Foundation Classes Reference and Microsoft
// QuickHelp documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft Foundation Classes product.

#include "oclient.h"
#include "mainwnd.h"
#include "itemwnd.h"

#include <afxdlgs.h>

/////////////////////////////////////////////////////////////////////////////
// File menu commands

void CMainWnd::OnFileNew()
{
	if (!SaveAsNeeded())
		return;

	InitFile(FALSE);
}

void CMainWnd::OnFileOpen()
{
	if (!SaveAsNeeded())
		return;

	CString newName = m_fileName;
	if (!DoFileDlg(newName, IDS_OPENFILE, OFN_HIDEREADONLY | OFN_FILEMUSTEXIST))
		return;

	CFile file;
	if (!file.Open(newName, CFile::modeRead | CFile::shareDenyWrite))
	{
		ErrorMessage(E_FAILED_TO_READ_FILE);
		return;
	}

	/* Deregister the current document, and re-init */
	m_fileName = newName;
	InitFile(TRUE);

	/* If we failed to read, we already destroyed
	 * all the objects, so go back to File New-like state */
	Hourglass(TRUE);
	TRY
	{
		CArchive loadArchive(&file, CArchive::load);
		this->Serialize(loadArchive);       // load me
	}
	CATCH (CException, e)
	{
		Hourglass(FALSE);
		ErrorMessage(E_FAILED_TO_READ_FILE);
		InitFile(FALSE);    // just re-init it like New
		file.Close();
		return;
	}
	END_CATCH

	Hourglass(FALSE);
	m_fDirty = FALSE;
	file.Close();
}


void CMainWnd::OnFileSave()
{
	// do the save
	DoSave(m_fileName);
}


void CMainWnd::OnFileSaveAs()
{
	// save to new name
	DoSave(NULL);
}

/////////////////////////////////////////////////////////////////////////////
// Basic file operations

void CMainWnd::InitFile(BOOL fOpen)
{
	m_fDirty = FALSE;

	/* If New, reset the file name */
	if (!fOpen)
		m_fileName.Empty();

	/* Deregister the edited document, and wipe out the objects. */
	DeregisterDoc();

	/* Reset the title bar, and register the OLE client document */
	SetTitle();
}

static void FormatString1(CString&s, UINT idFormat,
	const char* szData, const char* szAlternate)
{
	char szTmp[CBMESSAGEMAX];
	CString format;
	format.LoadString(idFormat);

	if (szData[0] == '\0')
		szData = szAlternate;

	wsprintf(szTmp, (LPCSTR)format, (LPCSTR)szData);
	s = szTmp;
}

BOOL CMainWnd::SaveAsNeeded()
{
	if (!m_fDirty)
		return TRUE;

	CString prompt;
	FormatString1(prompt, IDS_MAYBESAVE, m_fileName, strUntitled);

	/* Ask "Do you wish to save your changes?" */
	switch (MessageBox(prompt, AfxGetAppName(), MB_YESNOCANCEL | MB_ICONQUESTION))
	{
	case IDCANCEL:
		return FALSE;       // don't continue
		break;

	case IDYES:
		/* If so, either Save or Update, as appropriate. */
		if (!DoSave(m_fileName))
			return FALSE;       // don't continue
		break;

	case IDNO:
		/* If not saving changes, revert the document */
		m_document.NotifyRevert();
		break;

	default:
		ASSERT(FALSE);
		break;
	}
	return TRUE;    // keep going
}

BOOL CMainWnd::DoSave(const char* szFileName)
		// note: can be different than 'm_fileName' for SaveAs
		// will set 'm_fileName' if successful
{
	CString newName = szFileName;
	if (newName.IsEmpty())
	{
		// no file name, prompt for new one
		if (!DoFileDlg(newName, IDS_SAVEFILE,
		  OFN_HIDEREADONLY | OFN_PATHMUSTEXIST))
			return FALSE;
	}

	CFile file;
	if (!file.Open(newName, CFile::modeCreate |
	  CFile::modeReadWrite | CFile::shareExclusive))
	{
		ErrorMessage(E_INVALID_FILENAME);
		return FALSE;
	}

	Hourglass(TRUE);
	TRY
	{
		CArchive saveArchive(&file, CArchive::store);
		this->Serialize(saveArchive);       // save me
	}
	CATCH (CException, e)
	{
		Hourglass(FALSE);
		ErrorMessage(E_FAILED_TO_SAVE_FILE);
		file.Close();
		return FALSE;
	}
	END_CATCH

	Hourglass(FALSE);
	file.Close();

	// notify library
	m_document.NotifySaved();
	m_fDirty = FALSE;

	// Reset the title and change the document name
	m_fileName = newName;
	SetTitle();

	return TRUE;        // success
}

/////////////////////////////////////////////////////////////////////////////
// Actual reading/writing of data

void CMainWnd::Serialize(CArchive& ar)
{
	CFrameWnd::Serialize(ar);   // does nothing I hope
	WORD wMagic = 0x0DAF;

	if (ar.IsStoring())
	{
		ar << wMagic;
		// First count up number of OLE Objects to write
		CItemWnd* pItemWnd;
		WORD    cCompleteObjectsToWrite = 0;
		WORD    cIncompleteObjects = 0;

		for (pItemWnd = (CItemWnd*)GetTopWindow(); pItemWnd != NULL;
			pItemWnd = (CItemWnd*)pItemWnd->GetNextWindow())
		{
			if (pItemWnd->IsComplete())
				cCompleteObjectsToWrite++;
			else
				cIncompleteObjects++;
		}

		TRACE("%d complete objects to write, %d incomplete ones to ignore\n",
			cCompleteObjectsToWrite, cIncompleteObjects);

		CString prompt;
		prompt.LoadString(IDS_SAVEINCOMPLETE);
		if (cIncompleteObjects > 0 && MessageBox(prompt, AfxGetAppName(),
			MB_YESNO | MB_ICONQUESTION) != IDYES)
		{
			TRACE("Aborting save\n");
			AfxThrowArchiveException(CArchiveException::generic);
		}

		ar << cCompleteObjectsToWrite;

#ifdef _DEBUG
		WORD    cObjectsWritten = 0;
#endif
		// reverse order (easier to rebuild that way)
		for (pItemWnd = (CItemWnd*)GetTopWindow(); pItemWnd != NULL;
			pItemWnd = (CItemWnd*)pItemWnd->GetNextWindow())
		{
			if (pItemWnd->IsComplete())
			{
				pItemWnd->Serialize(ar);
				// NOTE: don't use WriteObject since we know the actual
				//  class for pItemWnd
#ifdef _DEBUG
				cObjectsWritten++;
#endif
			}
		}
#ifdef _DEBUG
		ASSERT(cObjectsWritten == cCompleteObjectsToWrite);
#endif
	}
	else // loading
	{
		WORD w;
		ar >> w;

		if (w != wMagic)
		{
			TRACE("invalid magic number at start of file\n");
			AfxThrowArchiveException(CArchiveException::generic);
		}

		WORD cObjectsToRead;
		ar >> cObjectsToRead;

		for (UINT iObj = 0; iObj < cObjectsToRead; iObj++)
		{
			CItemWnd* pItemWnd = new CItemWnd(this);

			pItemWnd->Serialize(ar);
		}
	}
}


/////////////////////////////////////////////////////////////////////////////

void CMainWnd::SetTitle()
{
	const char* pszName;
	char szTitle[CBMESSAGEMAX + _MAX_PATH];

	pszName = (m_fileName.IsEmpty()) ? strUntitled : m_fileName;
	wsprintf(szTitle, "%s - %s", (LPCSTR)AfxGetAppName(), (LPCSTR)pszName);

	/* Perform the client document registration */
	if (m_document.IsOpen())
	{
		m_document.NotifyRename(pszName);
	}
	else
	{
		if (!m_document.Register(AfxGetAppName(), pszName))
		{
			ErrorMessage(W_FAILED_TO_NOTIFY);
			ASSERT(!m_document.IsOpen());
		}
	}

	SetWindowText(szTitle);
}

void CMainWnd::DeregisterDoc()
{
	/* Destroy all the objects */
	ClearAll();

	/* Release the document */
	if (m_document.IsOpen())
	{
		m_document.Revoke();
	}
}

/////////////////////////////////////////////////////////////////////////////
// File Dialog prompts

static void Normalize(LPSTR lpstrFile);

BOOL CMainWnd::DoFileDlg(CString& fileName, UINT nIDTitle, DWORD lFlags)
{
	CString title;
	CString filter;
	CString ext;
	VERIFY(title.LoadString(nIDTitle));
	VERIFY(filter.LoadString(IDS_FILTER));
	VERIFY(ext.LoadString(IDS_EXTENSION));
	

	CFileDialog dlg(TRUE, ext, fileName, lFlags, filter);
	
	dlg.m_ofn.lpstrTitle = title;

	if (dlg.DoModal() == IDCANCEL)
		return FALSE;

	fileName = dlg.GetFileName() + '.' + dlg.GetFileExt();
	return TRUE;
}
