// mpfile.cpp : Defines the child's file code for the application.
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

#include "multipad.h"

#include <commdlg.h>

#ifndef _NTWIN
#pragma code_seg("_MPFILE")
#endif

static char BASED_CODE szFileDialogFilter[] = 
		"Text Files (*.txt)|*.txt|All Files (*.*)|*.*||";
static char BASED_CODE szFileDialogExt[] = "txt";

extern void AddFileToMRU(const char* szFileName);
	
/////////////////////////////////////////////////////////////////////////////

// AlreadyOpen:
// Just looks through the children for a child with the same filename.
// Strange path equivalents can confuse this simple approach, but these
// are rare.
//
CMPChild* CMPFrame::AlreadyOpen(char* szFile)
{
	CWnd* pwndCheck;
	char szChild[64];
	
	// Check each MDI child window in Multipad.
	//
	for (pwndCheck = CWnd::FromHandle(::GetWindow(m_hWndMDIClient, GW_CHILD));
		pwndCheck != NULL; pwndCheck = pwndCheck->GetNextWindow())
	{
		// Skip icon title windows.
		//
		if (pwndCheck->GetWindow(GW_OWNER) != NULL)
			continue;
		
		// Get current child window's name
		//
		pwndCheck->GetWindowText(szChild, sizeof (szChild));
	
		if (lstrcmp(szChild, szFile) == 0)
			return (CMPChild*)pwndCheck;
	}
	
	// No match found -- file is not open.
	//
	return NULL;
}

// CMPChild constructor:
// Opens the given file, reads the text into a buffer, and ties the buffer
// to a CEdit inside the child.
//
CMPChild::CMPChild(char* szName)
{
	static int cUntitled = 0;
	char sz[160];
	
	if (!szName)
	{
		char* pch = sz + ::LoadString(AfxGetResourceHandle(), IDS_UNTITLED,
			sz, sizeof(sz));
		sprintf(pch, "%d", ++cUntitled);
	}
	else
	{
		strcpy(sz, szName);
	}
	
	if (CMPFrame::GetActiveChild() == NULL)
		Create(NULL, sz, WS_MAXIMIZE);
	else
		Create(NULL, sz, 0L);
	
	if (szName != NULL)
	{
		if (!LoadFile(szName))
		{
			DestroyWindow();
		}
	}
}

// LoadFile:
// Reads one file to replace the current buffer's text.
//
int CMPChild::LoadFile(char* pName)
{
	UINT nLength;
	HANDLE hTextWnd;
	LPSTR lpBuffer;
	CFile file;
	
	// The file has a title, so reset the UNTITLED flag.
	//
	m_bUntitled = FALSE;
	
	if (!file.Open(pName, CFile::modeRead))
		goto error;

	nLength = (UINT) file.GetLength();

	// Attempt to reallocate the edit control's buffer to the file size.
	//
	hTextWnd = m_edit.GetHandle();
	if (LocalReAlloc(hTextWnd, nLength + 1, LHND) == NULL)
	{
		// Couldn't reallocate to new size -- error!
		//
		file.Close();

		TRACE("FILE TOO BIG %ld %s\n", file.GetLength(), pName);
		MPError(MB_OK | MB_ICONHAND, IDS_FILETOOBIG, (LPCSTR)pName);
		return FALSE;
	}
	
	// Read the file into the buffer.
	// If that fails, tell the user.
	//
	TRY
	{
		file.Read((lpBuffer = (LPSTR)LocalLock (hTextWnd)), nLength);
	}
	CATCH(CFileException, e)
	{
		MPError(MB_OK | MB_ICONHAND, IDS_CANTREAD, (LPCSTR)pName);
	}
	END_CATCH
	
	// Zero-terminate the edit buffer.
	//
	lpBuffer[nLength] = 0;
	LocalUnlock(hTextWnd);
	
	m_edit.SetHandle(hTextWnd);
	file.Close();
	
	AddFileToMRU(pName);
	
	return TRUE;
	
error:
	// Report the error and quit.
	//
	MPError(MB_OK | MB_ICONHAND, IDS_CANTOPEN, (LPCSTR)pName);
	return FALSE;
}

// ReadFile:
// Gets a filename from the user, then creates a new CMPChild for that file.
//
void CMPFrame::ReadFile()
{
	CFileDialog fileDialog(TRUE, szFileDialogExt, NULL,
		OFN_HIDEREADONLY, szFileDialogFilter);
	
	if (fileDialog.DoModal() == IDOK)
		ReadFile(fileDialog.GetPathName());
}

void CMPFrame::ReadFile(const char* szFile)
{
	CWnd* pwndFile;
	CFileStatus fileStatus;
	
	// Get full path to file.
	//
	if (CFile::GetStatus(szFile, fileStatus))
	{
		// Is file already open?
		// If so, just bring the file's window to the top.
		//
		if ((pwndFile = AlreadyOpen(fileStatus.m_szFullName)) != NULL)
		{
			pwndFile->BringWindowToTop();
			return;
		}
	}
	
	// Make a new window and load file into it.
	//
	new CMPChild(fileStatus.m_szFullName);
}

// SaveFile:
// Writes the text into its file.
//
void CMPChild::SaveFile()
{
	HANDLE hTextWnd;
	LPSTR lpExt;
	char szFile[128];
	UINT cch;
	CFile file;

	GetWindowText(szFile, sizeof(szFile));
	for (lpExt = szFile; *lpExt; lpExt++)
	{
		switch (*lpExt)
		{
			case '.':
			 cch = TRUE;
			 break;

			case '\\':
			case ':' :
			 cch = FALSE;
			 break;
		}
	}

	if (!cch)
	{
		::LoadString(AfxGetResourceHandle(), IDS_ADDEXT, lpExt,
			lpExt - (LPSTR)szFile);
	}


	if (!file.Open(szFile, CFile::modeCreate | CFile::modeWrite))
	{
		MPError (MB_OK | MB_ICONHAND, IDS_CANTCREATE, (LPCSTR)szFile);
		return;
	}
	
	// Find out the length of the text in the edit control
	cch = m_edit.GetWindowTextLength();

	// Obtain a handle to the text buffer
	hTextWnd = m_edit.GetHandle();
	lpExt = (LPSTR)LocalLock (hTextWnd);
	
	// Write out the contents of the buffer to the file.
	TRY
	{
		file.Write(lpExt, cch);
		file.Close();
	}
	CATCH(CFileException, e)
	{
		file.Close();
		MPError(MB_OK | MB_ICONHAND, IDS_CANTWRITE, (LPCSTR)szFile);
	}
	END_CATCH

	// Clean up
	LocalUnlock(hTextWnd);
	m_edit.SetHandle(hTextWnd);
	
	m_bChanged = FALSE;
}

BOOL CMPChild::ChangeFile()
{
	char szTitle[_MAX_PATH];
	GetWindowText((LPSTR)szTitle, sizeof(szTitle));
	::GetFileTitle((LPSTR)szTitle, (LPSTR)szTitle, sizeof(szTitle));
	
	CFileDialog fileDialog(FALSE, szFileDialogExt, szTitle,
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, szFileDialogFilter);

	if (fileDialog.DoModal() == IDOK)
	{
		CString s = fileDialog.GetPathName();
		AddFileToMRU(s);
		SetWindowText(s);
		m_bUntitled = FALSE;
		return TRUE;
	}
	else
		return FALSE;
}
