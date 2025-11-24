// This is a part of the Microsoft Foundation Classes C++ library. 
// Copyright (C) 1992 Microsoft Corporation 
// All rights reserved. 
//  
// This source code is only intended as a supplement to the 
// Microsoft Foundation Classes Reference and Microsoft 
// QuickHelp documentation provided with the library. 
// See these sources for detailed information regarding the 
// Microsoft Foundation Classes product. 

#include "afxole.h"
#pragma hdrstop

#include "afxoleui.h"       // user interface parts

#include "shellapi.h"

#ifdef AFX_OLE_SEG
#pragma code_seg(AFX_OLE_SEG)
#endif

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// User interface for COleClientItem

int COleClientItem::cWaitForRelease = 0;
BOOL COleClientItem::InWaitForRelease()
{
	return cWaitForRelease != 0;
}

void COleClientItem::WaitForServer()
{
	// enforce synchronous action from the server
	if (afxTraceFlags & 0x10)
		TRACE("WAITING for server\n");

	ASSERT(m_lpObject != NULL);
	ASSERT(cWaitForRelease == 0);
#ifdef _DEBUG
	m_lastStatus = OLE_WAIT_FOR_RELEASE;
#endif
	cWaitForRelease++;

	// OnRelease may NULL out our m_lpObject
	while (m_lpObject != NULL && ::OleQueryReleaseStatus(m_lpObject) != OLE_OK)
	{
		TRY
		{
			AfxGetApp()->PumpMessage();
		}
		CATCH(CException,e)
		{
			TRACE("DANGER: caught exception in WaitForServer - continuing\n");
		}
		END_CATCH
	}
	cWaitForRelease--;

	if (afxTraceFlags & 0x10)
		TRACE("DONE WAITING for server\n");
}

BOOL COleClientItem::ReportError(OLESTATUS status)
	// return TRUE if error or warning reported
{
	UINT idString = 0;

	switch (status)
	{
	default:
		return FALSE;       // nothing sensible to report

	case OLE_ERROR_STATIC:
		idString = AFX_ERROR_STATIC_OBJECT;
		break;

	case OLE_ERROR_REQUEST_PICT:
	case OLE_ERROR_ADVISE_RENAME:
	case OLE_ERROR_SHOW:
	case OLE_ERROR_OPEN:
	case OLE_ERROR_NETWORK:
	case OLE_ERROR_ADVISE_PICT:
	case OLE_ERROR_COMM:
	case OLE_ERROR_LAUNCH:
		// invalid link
		idString = AFX_ERROR_FAILED_TO_CONNECT;
		break;

	case OLE_ERROR_DOVERB:
		idString = AFX_ERROR_BAD_VERB;
		break;

	case OLE_BUSY:
		idString = AFX_ERROR_SERVER_BUSY;
		break;

	case OLE_ERROR_MEMORY:
		idString = AFX_ERROR_MEMORY;
		break;
	}

	CString s;
	s.LoadString(idString);

	// bring up a message box in the topmost window
	CWinApp* pApp = AfxGetApp();
	ASSERT(pApp != NULL && pApp->m_pMainWnd != NULL);
	pApp->m_pMainWnd->MessageBox(s, pApp->m_pszAppName,
			MB_OK | MB_ICONEXCLAMATION);

	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// OLE Object Verb Menu helpers

// Parameters:
//      pClient = client object to operate on (NULL => none)
//      pMenu = menu to modify
//      iMenuItem = index into menu where menu item or popup is to be placed
//              (note will delete the old one)
//      nIDVerbMin = first menu command id for sending to pClient
//
// Supported cases:
//  NULL client "&Object" disabled
//  0 verbs       "<Object Class> &Object"
//  1 verb (no name) "<Object Class> &Object"
//  1 verb == edit   "<Object Class> &Object"
//  1 verb != edit   "<verb> <Object Class> &Object"
//  more than 1 verb "<Object Class> &Object" => verbs

void AfxOleSetEditMenu(COleClientItem* pClient, CMenu* pMenu,
	UINT iMenuItem, UINT nIDVerbMin)
{
	ASSERT(pMenu != NULL);

	static CString NEAR strObjectVerb;           // "&Object"
	static CString NEAR strEditVerb;             // "Edit"
	static BOOL bInited = FALSE;
	if (!bInited)
	{
		VERIFY(strObjectVerb.LoadString(AFX_IDS_OBJECT_MENUITEM));
		VERIFY(strEditVerb.LoadString(AFX_IDS_EDIT_VERB));
	}

	pMenu->DeleteMenu(iMenuItem, MF_BYPOSITION); // get rid of old UI

	HANDLE hLinkData = NULL;
	UINT mfObjectVerb = MF_GRAYED|MF_DISABLED;

	if (pClient != NULL)
	{
		// get type from object
		hLinkData = pClient->GetLinkFormatData();
		mfObjectVerb = MF_ENABLED;
	}

	LPCSTR   lpszData;
	// use the link data to determine what class we are talking about

	if (hLinkData == NULL ||
	   (lpszData = (LPCSTR)::GlobalLock(hLinkData)) == NULL)
	{
		// not a valid link, just use the simple '&Object' format disabled
		pMenu->InsertMenu(iMenuItem, MF_BYPOSITION, nIDVerbMin, strObjectVerb);
		pMenu->EnableMenuItem(iMenuItem, mfObjectVerb | MF_BYPOSITION |
			MF_GRAYED|MF_DISABLED);
		return;
	}

	LONG    lSize;
	char    szClass[OLE_MAXNAMESIZE];
	char    szBuffer[OLE_MAXNAMESIZE+40];

	// get real language class of object in szClass for menu
	lSize = OLE_MAXNAMESIZE;
	if (::RegQueryValue(HKEY_CLASSES_ROOT, lpszData, szClass,
		&lSize) != ERROR_SUCCESS)
	{
		// no localized class name, use unlocalized name
		lstrcpy(szClass, lpszData);
	}
	::GlobalUnlock(hLinkData);

	// determine list of available verbs
	char    szFirstVerb[OLE_MAXNAMESIZE];
	HMENU  hPopup = NULL;
	int     cVerbs = 0;

	while (1)
	{
		wsprintf(szBuffer, "%s\\protocol\\StdFileEditing\\verb\\%d",
				 (LPCSTR)lpszData, cVerbs);

		/* get verb name */
		char    szVerb[OLE_MAXNAMESIZE];
		lSize = OLE_MAXNAMESIZE;
		if (::RegQueryValue(HKEY_CLASSES_ROOT, szBuffer, szVerb, &lSize) != 0)
		{
			// finished counting verbs
			break;
		}
		cVerbs++;

		if (_stricmp(szVerb, strEditVerb) == 0)
			strcpy(szVerb, strEditVerb);    // use 'Edit' not 'EDIT'

		if (cVerbs == 1)
		{
			// save first verb (special case if this is it)
			strcpy(szFirstVerb, szVerb);
		}
		else
		{
			// overflow into popup
			if (cVerbs == 2)
			{
				// start the popup
				ASSERT(hPopup == NULL);
				hPopup = CreatePopupMenu();

				// now add the first verb
				InsertMenu(hPopup, -1, MF_BYPOSITION, nIDVerbMin + 0,
					szFirstVerb);
			}

			ASSERT(hPopup != NULL);
			InsertMenu(hPopup, -1, MF_BYPOSITION, nIDVerbMin + cVerbs - 1,
					szVerb);
		}
	}

	if (cVerbs >= 2)
	{
		// install the popup
		wsprintf(szBuffer, "%s %s", (LPCSTR)szClass, (LPCSTR)strObjectVerb);
		pMenu->InsertMenu(iMenuItem, MF_BYPOSITION|MF_POPUP, (UINT)hPopup, szBuffer);
	}
	else if (cVerbs == 0 || _stricmp(szFirstVerb, strEditVerb) == 0)
	{
		// no verbs or redundant 'edit' verb
		wsprintf(szBuffer, "%s %s", (LPCSTR)szClass, (LPCSTR)strObjectVerb);
		pMenu->InsertMenu(iMenuItem, MF_BYPOSITION, nIDVerbMin, szBuffer);
	}
	else
	{
		// use that verb in menu item
		ASSERT(cVerbs == 1);
		wsprintf(szBuffer, "%s %s %s", (LPCSTR)szFirstVerb, (LPCSTR)szClass,
			(LPCSTR)strObjectVerb);
		pMenu->InsertMenu(iMenuItem, MF_BYPOSITION, nIDVerbMin, szBuffer);
	}

	// enable what we added
	pMenu->EnableMenuItem(iMenuItem, MF_ENABLED|MF_BYPOSITION);
}

/////////////////////////////////////////////////////////////////////////////
// InsertObject dialog

class CInsertNewObjectDlg : public CModalDialog
{
public:
	CString&    m_rClassName;

	CInsertNewObjectDlg(CString& rReturn)
			: CModalDialog(AFX_IDD_INSERTNEWOBJECT),
				m_rClassName(rReturn)
		{ }

	BOOL OnInitDialog();
	void OnOK();
	DECLARE_MESSAGE_MAP()
};

BEGIN_MESSAGE_MAP(CInsertNewObjectDlg, CModalDialog)
	ON_LBN_DBLCLK(AFX_IDC_LISTBOX, OnOK)
END_MESSAGE_MAP()

BOOL CInsertNewObjectDlg::OnInitDialog()
{
	int cClasses = 0;
	CListBox* pList = (CListBox*)GetDlgItem(AFX_IDC_LISTBOX);

	pList->ResetContent();

	char szClass[OLE_MAXNAMESIZE];
	int i = 0;
	while (::RegEnumKey(HKEY_CLASSES_ROOT, i++, szClass, OLE_MAXNAMESIZE) == 0)
	{
		if (*szClass == '.')
			continue;       // skip extensions

		// See if this class really refers to a server
		LONG lSize;
		HKEY hkey = NULL;
		char szExec[OLE_MAXNAMESIZE+40];
		lstrcpy(szExec, szClass);
		lstrcat(szExec, "\\protocol\\StdFileEditing\\server");

		if (::RegOpenKey(HKEY_CLASSES_ROOT, szExec, &hkey) == 0)
		{
			// since it has a server - add it to the list
			char szName[OLE_MAXNAMESIZE];
			lSize = OLE_MAXNAMESIZE;
			if (::RegQueryValue(HKEY_CLASSES_ROOT, szClass,
			  szName, &lSize) == 0)
			{
				// we have a class name
				pList->AddString(szName);
				cClasses++;
			}
			::RegCloseKey(hkey);
		}
	}

	if (cClasses == 0)
	{
		// whoops - nothing to choose from
		EndDialog(IDCANCEL);
	}
	pList->SetCurSel(0);
	return TRUE;
}

void CInsertNewObjectDlg::OnOK()
{
	CListBox* pList = (CListBox*)GetDlgItem(AFX_IDC_LISTBOX);

	char szKey[OLE_MAXNAMESIZE];
	pList->GetText(pList->GetCurSel(), szKey);

	char szClass[OLE_MAXNAMESIZE];
	int i = 0;
	while (::RegEnumKey(HKEY_CLASSES_ROOT, i++, szClass, OLE_MAXNAMESIZE) == 0)
	{
		if (*szClass == '.')
			continue;       // skip extensions

		// See if this class really refers to a server
		LONG lSize;
		HKEY hkey = NULL;
		char szExec[OLE_MAXNAMESIZE+40];
		lstrcpy(szExec, szClass);
		lstrcat(szExec, "\\protocol\\StdFileEditing\\server");

		if (::RegOpenKey(HKEY_CLASSES_ROOT, szExec, &hkey) == 0)
		{
			// we found a match - see if appropriate name
			char szName[OLE_MAXNAMESIZE];
			lSize = OLE_MAXNAMESIZE;
			if (::RegQueryValue(HKEY_CLASSES_ROOT, szClass,
			  szName, &lSize) == 0)
			{
				// it is a named class - see if it matches key
				if (strcmp(szKey, szName) == 0)
				{
					// this is it
					m_rClassName = szClass;
					CModalDialog::OnOK();   // terminate dialog
					return;
				}
			}
			::RegCloseKey(hkey);
		}
	}

	// didn't find it
	EndDialog(IDCANCEL);
}


BOOL AfxOleInsertDialog(CString& name)
{
	CInsertNewObjectDlg dlg(name);

	return (dlg.DoModal() == IDOK);
}

/////////////////////////////////////////////////////////////////////////////
