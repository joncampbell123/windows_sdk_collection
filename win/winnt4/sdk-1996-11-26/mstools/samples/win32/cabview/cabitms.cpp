//*******************************************************************************************
//
// Filename : CabItms.cpp
//	
//				Implementation file for CMemFile, CCabEnum and CCabExtract
//
// Copyright (c) 1994 - 1996 Microsoft Corporation. All rights reserved
//
//*******************************************************************************************

#include "pch.h"

#include "thisdll.h"

#include "resource.h"

#include "path.h"		
#include "fdi.h"
#include "cabitms.h"

class CMemFile
{
public:
	CMemFile(HGLOBAL *phMem, DWORD dwSize);
	~CMemFile() {}

	BOOL Create(LPCSTR pszFile, int fnAttribute);
	BOOL Open(LPCSTR pszFile, int oflag);
	LONG Seek(LONG dist, int seektype);
	UINT Read(LPVOID pv, UINT cb);
	UINT Write(LPVOID pv, UINT cb);
	HFILE Close();

private:
	HFILE m_hf;

	HGLOBAL *m_phMem;
	DWORD m_dwSize;
	LONG m_lLoc;
} ;


CMemFile::CMemFile(HGLOBAL *phMem, DWORD dwSize) : m_hf(HFILE_ERROR), m_lLoc(0)
{
	m_phMem = phMem;
	m_dwSize = dwSize;

	if (phMem)
	{
		*phMem = NULL;
	}
}


BOOL CMemFile::Create(LPCSTR pszFile, int fnAttribute)
{
	if (m_phMem)
	{
		if (*m_phMem)
		{
			return(FALSE);
		}

		*m_phMem = GlobalAlloc(LMEM_FIXED, m_dwSize);
		return(*m_phMem != NULL);
	}
	else
	{
		if (m_hf != HFILE_ERROR)
		{
			return(FALSE);
		}

		m_hf = _lcreat(pszFile, fnAttribute);
		return(m_hf != HFILE_ERROR);
	}
}


BOOL CMemFile::Open(LPCSTR pszFile, int oflag)
{
	if (m_phMem)
	{
		return(FALSE);
	}
	else
	{
		if (m_hf != HFILE_ERROR)
		{
			return(FALSE);
		}

		m_hf = _lopen(pszFile, oflag);
		return(m_hf != HFILE_ERROR);
	}
}


LONG CMemFile::Seek(LONG dist, int seektype)
{
	if (m_phMem)
	{
		if (!*m_phMem)
		{
			return(HFILE_ERROR);
		}

		switch (seektype)
		{
		case FILE_BEGIN:
			break;

		case FILE_CURRENT:
			dist += m_lLoc;
			break;

		case FILE_END:
			dist = m_dwSize - dist;
			break;

		default:
			return(HFILE_ERROR);
		}

		if (dist<0 || dist>(LONG)m_dwSize)
		{
			return(HFILE_ERROR);
		}

		m_lLoc = dist;
		return(dist);
	}
	else
	{
		return(_llseek(m_hf, dist, seektype));
	}
}


UINT CMemFile::Read(LPVOID pv, UINT cb)
{
	if (m_phMem)
	{
		if (!*m_phMem)
		{
			return((UINT)HFILE_ERROR);
		}

		if (cb > m_dwSize - m_lLoc)
		{
			cb = m_dwSize - m_lLoc;
		}

		hmemcpy(pv, (LPSTR)(*m_phMem)+m_lLoc, cb);
		m_lLoc += cb;
		return(cb);
	}
	else
	{
		return(_lread(m_hf, pv, cb));
	}
}


UINT CMemFile::Write(LPVOID pv, UINT cb)
{
	if (m_phMem)
	{
		if (!*m_phMem)
		{
			return((UINT)HFILE_ERROR);
		}

		if (cb > m_dwSize - m_lLoc)
		{
			cb = m_dwSize - m_lLoc;
		}

		hmemcpy((LPSTR)(*m_phMem)+m_lLoc, pv, cb);
		m_lLoc += cb;
		return(cb);
	}
	else
	{
		return(_lwrite(m_hf, (LPCSTR)pv, cb));
	}
}


HFILE CMemFile::Close()
{
	HFILE hRet;

	if (m_phMem)
	{
		hRet = *m_phMem ? 0 : HFILE_ERROR;
	}
	else
	{
		hRet = _lclose(m_hf);
	}

	delete this;

	return(hRet);
}

//*****************************************************************************
//
// CCabEnum
//
// Purpose:
//
//        Class encapsulating all the FDI operations
//
// Comments:
//
//*****************************************************************************

class CCabEnum
{
public:
	CCabEnum() : m_hfdi(0) {}
	~CCabEnum() {}

protected:
	static void HUGE * FAR DIAMONDAPI CabAlloc(ULONG cb);
	static void FAR DIAMONDAPI CabFree(void HUGE *pv);
	static int  FAR DIAMONDAPI CabOpen(char FAR *pszFile, int oflag, int pmode);
	static UINT FAR DIAMONDAPI CabRead(int hf, void FAR *pv, UINT cb);
	static UINT FAR DIAMONDAPI CabWrite(int hf, void FAR *pv, UINT cb);
	static int  FAR DIAMONDAPI CabClose(int hf);
	static long FAR DIAMONDAPI CabSeek(int hf, long dist, int seektype);

	BOOL StartEnum();
	BOOL SimpleEnum(LPCSTR szCabFile, PFNFDINOTIFY pfnCallBack, LPVOID pv);
	void EndEnum();

	HFDI m_hfdi;

private:
	static CMemFile * s_hSpill;
} ;


CMemFile * CCabEnum::s_hSpill = NULL;

void HUGE * FAR DIAMONDAPI CCabEnum::CabAlloc(ULONG cb)
{
    return(GlobalAllocPtr(GHND, cb));
}

void FAR DIAMONDAPI CCabEnum::CabFree(void HUGE *pv)
{
    GlobalFreePtr(pv);
}

int  FAR DIAMONDAPI CCabEnum::CabOpen(char FAR *pszFile, int oflag, int pmode)
{
    if(!pszFile)
    {
      return -1;
    }

    // See if we are opening the spill file.
    if( *pszFile=='*' )
    {
		char szSpillFile[MAX_PATH];
		char szTempPath[MAX_PATH];

        if(s_hSpill != NULL)
            return -1;

		GetTempPath(sizeof(szTempPath), szTempPath);
		GetTempFileName(szTempPath, "fdi", 0, szSpillFile);

        s_hSpill = new CMemFile(NULL, 0);
		if (!s_hSpill)
		{
			return(-1);
		}
        if (!s_hSpill->Create(szSpillFile, 0))
		{
			delete s_hSpill;
			s_hSpill = NULL;
			return(-1);
		}
           
        // Set its extent.
        if( s_hSpill->Seek( ((FDISPILLFILE FAR *)pszFile)->cbFile-1, 0) == HFILE_ERROR)
        {
			s_hSpill->Close();
			s_hSpill = NULL;
			return -1;
        }
        s_hSpill->Write(szSpillFile, 1);

        return (int)s_hSpill;
    }

    CMemFile *hFile = new CMemFile(NULL, 0);
	if (!hFile)
	{
		return(-1);
	}

    while (!hFile->Open(pszFile, oflag))
    {
#if 1	// TODO: No UI for inserting a disk at this point
		delete hFile;
		return(-1);
#else
       // Failed to open the source.
       if (!LoadString (g_hInst, IDS_DISKPROMPT, szText, MAX_STRTABLE_LEN))
           return -1;
           
	   char szText[MAX_PATH];

       wsprintf (g_pErrorBuffer, (LPSTR)szText, (LPSTR)g_pCabName);

       // Use hwndIniting to have a parent window 
       if ( MyMessageBox(g_hwndIniting, g_pErrorBuffer,
                    MAKEINTRESOURCE(IDS_DISKPROMPT_TIT),
                    MB_OKCANCEL|MB_ICONSTOP, 0) == IDOK )
          continue;
       else
          return -1;
#endif
    }
   	
    return((int)hFile);
}


UINT FAR DIAMONDAPI CCabEnum::CabRead(int hf, void FAR *pv, UINT cb)
{
	CMemFile *hFile = (CMemFile *)hf;

    if (hFile->Read(pv, cb) != cb)
    {    
        cb = (UINT)-1;
	}

    return(cb);    
}


UINT FAR DIAMONDAPI CCabEnum::CabWrite(int hf, void FAR *pv, UINT cb)
{
	CMemFile *hFile = (CMemFile *)hf;

    if (hFile->Write(pv, cb) != cb)
	{
		cb = (UINT)-1;
	}
  
	return(cb);
}


int  FAR DIAMONDAPI CCabEnum::CabClose(int hf)
{
	CMemFile *hFile = (CMemFile *)hf;

    // Special case for the deletion of the spill file.
    if(hFile == s_hSpill)
	{
        s_hSpill = NULL;
	}

    return (hFile->Close());
}


long FAR DIAMONDAPI CCabEnum::CabSeek(int hf, long dist, int seektype)
{
	CMemFile *hFile = (CMemFile *)hf;

    return(hFile->Seek(dist, seektype));
}


BOOL CCabEnum::StartEnum()
{
	if (m_hfdi)
	{
		// We seem to already be enumerating
		return(FALSE);
	}

	ERF erf;

	m_hfdi = FDICreate(
		CabAlloc,
        CabFree,
        CabOpen,
        CabRead,
        CabWrite,
        CabClose,
        CabSeek,
        cpu80386,
        &erf);

	return(m_hfdi != NULL);
}


BOOL CCabEnum::SimpleEnum(LPCSTR szCabFile, PFNFDINOTIFY pfnCallBack, LPVOID pv)
{
	char szCabPath[MAX_PATH];
	char szCabName[MAX_PATH];

	// Path should be fully qualified
	lstrcpyn(szCabPath, szCabFile, sizeof(szCabPath));
	LPSTR pszName = PathFindFileName(szCabPath);
	if (!pszName)
	{
		return(FALSE);
	}

	lstrcpy(szCabName, pszName);
	*pszName = '\0';

	if (!StartEnum())
	{
		return(FALSE);
	}

	BOOL bRet = FDICopy(
		m_hfdi,
		szCabName,
		szCabPath,		// path to cabinet files
		0,				// flags
		pfnCallBack,
		NULL,
		pv);

	EndEnum();

	return(bRet);
}


void CCabEnum::EndEnum()
{
	if (!m_hfdi)
	{
		return;
	}

	FDIDestroy(m_hfdi);
	m_hfdi = NULL;
}


class CCabItemsCB : private CCabEnum
{
public:
	CCabItemsCB(CCabItems::PFNCABITEM pfnCallBack, LPARAM lParam)
	{
		m_pfnCallBack = pfnCallBack;
		m_lParam = lParam;
	}
	~CCabItemsCB() {}

	BOOL DoEnum(LPCSTR szCabFile)
	{
		return(SimpleEnum(szCabFile, CabItemsNotify, this));
	}

private:
	static int  FAR DIAMONDAPI CabItemsNotify(FDINOTIFICATIONTYPE fdint,
		PFDINOTIFICATION pfdin);

	CCabItems::PFNCABITEM m_pfnCallBack;
	LPARAM m_lParam;
} ;


int FAR DIAMONDAPI CCabItemsCB::CabItemsNotify(FDINOTIFICATIONTYPE fdint, PFDINOTIFICATION pfdin)
{
	CCabItemsCB *pThis = (CCabItemsCB *)pfdin->pv;			

    // uiYield( g_hwndSetup );
    
    switch (fdint)
    {
    case fdintCOPY_FILE:
		pThis->m_pfnCallBack(pfdin->psz1, pfdin->cb, pfdin->date, pfdin->time, pfdin->attribs,
			pThis->m_lParam);
		break;

	default:
		break;
    } // end switch

    return 0;
}

//*****************************************************************************
//
// CCabItems::EnumItems
//
// Purpose:
//
//        Enumerate the items in the cab file
//
//
// Comments:
//
//         lParam contains pointer to CCabFolder
//
//*****************************************************************************

BOOL CCabItems::EnumItems(PFNCABITEM pfnCallBack, LPARAM lParam)
{
	CCabItemsCB cItems(pfnCallBack, lParam);

	return(cItems.DoEnum(m_szCabFile));
}

//*****************************************************************************
//
// CCabExtractCB
//
// Purpose:
//
//        handles the call back while extracting Cab files
//
//
//*****************************************************************************

class CCabExtractCB : private CCabEnum
{
public:
	CCabExtractCB(LPCSTR szDir, HWND hwndOwner, CCabExtract::PFNCABEXTRACT pfnCallBack,
		LPARAM lParam)
	{
		m_szDir = szDir;
		m_hwndOwner = hwndOwner;
		m_pfnCallBack = pfnCallBack;
		m_lParam = lParam;
		m_bTryNextCab = FALSE;
	}
	~CCabExtractCB() {}

	BOOL DoEnum(LPCSTR szCabFile)
	{
		return(SimpleEnum(szCabFile, CabExtractNotify, this));
	}

private:
	static int  FAR DIAMONDAPI CabExtractNotify(FDINOTIFICATIONTYPE fdint,
		PFDINOTIFICATION pfdin);
	static int CALLBACK CCabExtractCB::BrowseNotify(HWND hwnd, UINT uMsg, LPARAM lParam,
		LPARAM lpData);

	LPCSTR m_szDir;
	HWND m_hwndOwner;
	CCabExtract::PFNCABEXTRACT m_pfnCallBack;
	LPARAM m_lParam;
	BOOL m_bTryNextCab;
	PFDINOTIFICATION m_pfdin;
} ;


int CALLBACK CCabExtractCB::BrowseNotify(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData)
{
	CCabExtractCB *pThis = (CCabExtractCB *)lpData;			

	switch (uMsg)
	{
	case BFFM_INITIALIZED:
	{
		// Set initial folder
		LPSTR pszEnd = PathAddBackslash(pThis->m_pfdin->psz3);
		if (pszEnd - pThis->m_pfdin->psz3 > 3)
		{
			// No problems if not drive root
			*(pszEnd - 1) = '\0';
		}
		SendMessage(hwnd, BFFM_SETSELECTION, 1, (LPARAM)pThis->m_pfdin->psz3);
		break;
	}

	default:
		return(0);
	}

	return(1);
}


int FAR DIAMONDAPI CCabExtractCB::CabExtractNotify(FDINOTIFICATIONTYPE fdint,
	PFDINOTIFICATION pfdin)
{
	CCabExtractCB *pThis = (CCabExtractCB *)pfdin->pv;			

    // uiYield( g_hwndSetup );
    
    switch (fdint)
    {
	case fdintCABINET_INFO:
		pThis->m_bTryNextCab = TRUE;
		break;

	case fdintNEXT_CABINET:
	{
		if (pThis->m_bTryNextCab)
		{
			// Automatically open next cab if already in default dir
			pThis->m_bTryNextCab = FALSE;
			return(1);
		}

		pThis->m_pfdin = pfdin;

		char szTitle[80];
		LoadString(g_ThisDll.GetInstance(), IDS_NEXTCABBROWSE, szTitle, sizeof(szTitle));

		BROWSEINFO bi;
		bi.hwndOwner = pThis->m_hwndOwner;
		bi.pidlRoot = NULL;
		bi.pszDisplayName = NULL;
		bi.lpszTitle = szTitle;
		bi.ulFlags = BIF_RETURNONLYFSDIRS;
		bi.lpfn = BrowseNotify;
		bi.lParam = (LPARAM)pThis;

		LPITEMIDLIST pidl = SHBrowseForFolder(&bi);

		if (bi.pidlRoot)
		{
			ILFree((LPITEMIDLIST)bi.pidlRoot);
		}

		if (!pidl)
		{
			return(-1);
		}

		BOOL bSuccess = SHGetPathFromIDList(pidl, pfdin->psz3);
		ILFree(pidl);

		if (bSuccess)
		{
			PathAddBackslash(pfdin->psz3);
			return(1);
		}

		return(-1);
	}

    case fdintCOPY_FILE:
	{
		HGLOBAL *phMem = pThis->m_pfnCallBack(pfdin->psz1, pfdin->cb, pfdin->date,
			pfdin->time, pfdin->attribs, pThis->m_lParam);
		if (!phMem)
		{
			break;
		}

		char szTemp[MAX_PATH];

		CMemFile *hFile;

		if (pThis->m_szDir == DIR_MEM)
		{
			*szTemp = '\0';
			hFile = new CMemFile(phMem, pfdin->cb);
		}
		else
		{
			PathCombine(szTemp, pThis->m_szDir, pfdin->psz1);
			hFile = new CMemFile(NULL, 0);
		}

		if (!hFile)
		{
			return(-1);
		}

		if (hFile->Create(szTemp, OF_WRITE))
		{
			return((int)hFile);
		}

		delete hFile;
		return(-1);
	}

    case fdintCLOSE_FILE_INFO:
	{
		CMemFile *hFile = (CMemFile *)pfdin->hf;

		return(hFile->Close() == 0);
	}

	default:
		break;
    } // end switch

    return 0;
}


BOOL CCabExtract::ExtractItems(HWND hwndOwner, LPCSTR szDir, PFNCABEXTRACT pfnCallBack, LPARAM lParam)
{
	char szTempDir[MAX_PATH];

	if (!szDir)
	{
		szDir = szTempDir;

		char szTitle[80];
		LoadString(g_ThisDll.GetInstance(), IDS_EXTRACTBROWSE, szTitle, sizeof(szTitle));

		BROWSEINFO bi;
		bi.hwndOwner = hwndOwner;
		bi.pidlRoot = NULL;
		bi.pszDisplayName = NULL;
		bi.lpszTitle = szTitle;
		bi.ulFlags = BIF_RETURNONLYFSDIRS;
		bi.lpfn = NULL;

		LPITEMIDLIST pidl = SHBrowseForFolder(&bi);
		if (!pidl)
		{
			return(FALSE);
		}

		BOOL bSuccess = SHGetPathFromIDList(pidl, szTempDir);

		ILFree(pidl);
		if (!bSuccess)
		{
			return(FALSE);
		}
	}

	CCabExtractCB cExtract(szDir, hwndOwner, pfnCallBack, lParam);

	// Display Wait cursor until done copying
	CWaitCursor cWait;

	return(cExtract.DoEnum(m_szCabFile));
}
