//*******************************************************************************************
//
// Filename : IUtil.c
//	
//				Implementation of Cab_MergeMenus and some other useful
//              routines
//
// Copyright (c) 1994 - 1996 Microsoft Corporation. All rights reserved
//
//*******************************************************************************************


#include "pch.h"
#include "Cabvw2.h"

// Copy a menu onto the beginning or end of another menu
// Adds uIDAdjust to each menu ID (pass in 0 for no adjustment)
// Will not add any item whose adjusted ID is greater than uMaxIDAdjust
// (pass in 0xffff to allow everything)
// Returns one more than the maximum adjusted ID that is used
//

BOOL _SHIsMenuSeparator(HMENU hm, int i)
{
        MENUITEMINFO mii;

        mii.cbSize = sizeof(MENUITEMINFO);
        mii.fMask = MIIM_TYPE;
        mii.cch = 0;    // WARNING: We MUST initialize it to 0!!!
        if (!GetMenuItemInfo(hm, i, TRUE, &mii))
        {
                return(FALSE);
        }

        if (mii.fType & MFT_SEPARATOR)
        {
                return(TRUE);
        }

        return(FALSE);
}

UINT Cab_MergeMenus(HMENU hmDst, HMENU hmSrc, UINT uInsert, UINT uIDAdjust, UINT uIDAdjustMax, ULONG uFlags)
{
        int nItem;
        HMENU hmSubMenu;
        BOOL bAlreadySeparated;
        MENUITEMINFO miiSrc;
        char szName[256];
        UINT uTemp, uIDMax = uIDAdjust;

        if (!hmDst || !hmSrc)
        {
                goto MM_Exit;
        }

        nItem = GetMenuItemCount(hmDst);
        if (uInsert >= (UINT)nItem)
        {
                uInsert = (UINT)nItem;
                bAlreadySeparated = TRUE;
        }
        else
        {
                bAlreadySeparated = _SHIsMenuSeparator(hmDst, uInsert);;
        }

        if ((uFlags & MM_ADDSEPARATOR) && !bAlreadySeparated)
        {
                // Add a separator between the menus
                InsertMenu(hmDst, uInsert, MF_BYPOSITION | MF_SEPARATOR, 0, NULL);
                bAlreadySeparated = TRUE;
        }


        // Go through the menu items and clone them
        for (nItem = GetMenuItemCount(hmSrc) - 1; nItem >= 0; nItem--)
        {
        miiSrc.cbSize = sizeof(MENUITEMINFO);
        miiSrc.fMask = MIIM_STATE | MIIM_ID | MIIM_SUBMENU | MIIM_CHECKMARKS | MIIM_TYPE | MIIM_DATA;
                // We need to reset this every time through the loop in case
                // menus DON'T have IDs
                miiSrc.fType = MFT_STRING;
                miiSrc.dwTypeData = szName;
                miiSrc.dwItemData = 0;
                miiSrc.cch        = sizeof(szName);

                if (!GetMenuItemInfo(hmSrc, nItem, TRUE, &miiSrc))
                {
                        continue;
                }

                if (miiSrc.fType & MFT_SEPARATOR)
                {
                        // This is a separator; don't put two of them in a row
                        if (bAlreadySeparated)
                        {
                                continue;
                        }

                        bAlreadySeparated = TRUE;
                }
                else if (miiSrc.hSubMenu)
                {
                        if (uFlags & MM_SUBMENUSHAVEIDS)
                        {
                                // Adjust the ID and check it
                                miiSrc.wID += uIDAdjust;
                                if (miiSrc.wID > uIDAdjustMax)
                                {
                                        continue;
                                }

                                if (uIDMax <= miiSrc.wID)
                                {
                                        uIDMax = miiSrc.wID + 1;
                                }
                        }
                        else
                        {
                                // Don't set IDs for submenus that didn't have
                                // them already
                                miiSrc.fMask &= ~MIIM_ID;
                        }

                        hmSubMenu = miiSrc.hSubMenu;
                        miiSrc.hSubMenu = CreatePopupMenu();
                        if (!miiSrc.hSubMenu)
                        {
                                goto MM_Exit;
                        }

                        uTemp = Cab_MergeMenus(miiSrc.hSubMenu, hmSubMenu, 0, uIDAdjust,
                                uIDAdjustMax, uFlags&MM_SUBMENUSHAVEIDS);
                        if (uIDMax <= uTemp)
                        {
                                uIDMax = uTemp;
                        }

                        bAlreadySeparated = FALSE;
                }
                else
                {
                        // Adjust the ID and check it
                        miiSrc.wID += uIDAdjust;
                        if (miiSrc.wID > uIDAdjustMax)
                        {
                                continue;
                        }

                        if (uIDMax <= miiSrc.wID)
                        {
                                uIDMax = miiSrc.wID + 1;
                        }

                        bAlreadySeparated = FALSE;
                }

                if (!InsertMenuItem(hmDst, uInsert, TRUE, &miiSrc))
                {
                        goto MM_Exit;
                }
        }

        // Ensure the correct number of separators at the beginning of the
        // inserted menu items
        if (uInsert == 0)
        {
                if (bAlreadySeparated)
                {
                        DeleteMenu(hmDst, uInsert, MF_BYPOSITION);
                }
        }
        else
        {
                if (_SHIsMenuSeparator(hmDst, uInsert-1))
                {
                        if (bAlreadySeparated)
                        {
                                DeleteMenu(hmDst, uInsert, MF_BYPOSITION);
                        }
                }
                else
                {
                        if ((uFlags & MM_ADDSEPARATOR) && !bAlreadySeparated)
                        {
                                // Add a separator between the menus
                                InsertMenu(hmDst, uInsert, MF_BYPOSITION | MF_SEPARATOR, 0, NULL);
                        }
                }
        }

MM_Exit:
        return(uIDMax);
}



int OleStrToStrN(LPSTR psz, int cchMultiByte, LPCOLESTR pwsz, int cchWideChar)
{
    return WideCharToMultiByte(CP_ACP, 0, pwsz, cchWideChar, psz, cchMultiByte, NULL, NULL);
}

int OleStrToStr(LPSTR psz, LPCOLESTR pwsz)
{
    return WideCharToMultiByte(CP_ACP, 0, pwsz, -1, psz, MAX_PATH, NULL, NULL);
}

int StrToOleStrN(LPOLESTR pwsz, int cchWideChar, LPCSTR psz, int cchMultiByte)
{
    return MultiByteToWideChar(CP_ACP, 0, psz, cchMultiByte, pwsz, cchWideChar);
}

int StrToOleStr(LPOLESTR pwsz, LPCSTR psz)
{
    return MultiByteToWideChar(CP_ACP, 0, psz, -1, pwsz, MAX_PATH);
}

/*
 * 
 */


UINT ILGetSize(LPCITEMIDLIST pidl)
{
    UINT cbTotal = 0;
    if (pidl)
    {
        VALIDATE_PIDL(pidl);
	cbTotal += sizeof(pidl->mkid.cb);	// Null terminator
	while (pidl->mkid.cb)
	{
	    cbTotal += pidl->mkid.cb;
	    pidl = _ILNext(pidl);
	}
    }

    return cbTotal;
}


LPITEMIDLIST ILClone(LPCITEMIDLIST pidl)
{
    UINT cb = ILGetSize(pidl);
    LPMALLOC pMalloc;
	LPITEMIDLIST pidlRet;

	SHGetMalloc(&pMalloc); 
	pidlRet = (LPITEMIDLIST)pMalloc->lpVtbl->Alloc(pMalloc, cb);
	pMalloc->lpVtbl->Release(pMalloc);

    if (pidlRet)
    {
	// Notes: no need to zero-init.
	hmemcpy(pidlRet, pidl, cb);
    }
    return pidlRet;
}


void ILFree(LPITEMIDLIST pidl)
{
	LPMALLOC pMalloc;

    if (pidl)
    {
#ifdef DEBUG
        UINT cbSize = ILGetSize(pidl);
        VALIDATE_PIDL(pidl);

        // Fill the memory with some bad value...
        _fmemset(pidl, 0xE5, cbSize);

        // If large enough try to save the call return address of who
        // freed us in the 3-6 byte of the structure.
        if (cbSize >= 6)
            *((UINT*)((LPSTR)pidl + 2)) =  *(((UINT*)&pidl) - 1);

#endif
		SHGetMalloc(&pMalloc);
		pMalloc->lpVtbl->Free(pMalloc, pidl);
		pMalloc->lpVtbl->Release(pMalloc);

    }
}

