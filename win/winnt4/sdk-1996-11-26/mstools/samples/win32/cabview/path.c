//*******************************************************************************************
//
// Filename : Path.c
//	
//				Useful Path manipulation routines
//
// Copyright (c) 1994 - 1996 Microsoft Corporation. All rights reserved
//
//*******************************************************************************************

#include "pch.h"
#include "path.h"
#include "debug.h"
#include "strings.h"

//
// Inline function to check for a double-backslash at the
// beginning of a string
//

__inline BOOL DBL_BSLASH(LPCTSTR psz)
{
    return (psz[0] == TEXT('\\') && psz[1] == TEXT('\\'));
}

// returns a pointer to the extension of a file.
//
// in:
//      qualified or unqualfied file name
//
// returns:
//      pointer to the extension of this file.  if there is no extension
//      as in "foo" we return a pointer to the NULL at the end
//      of the file
//
//      foo.txt     ==> ".txt"
//      foo         ==> ""
//      foo.        ==> "."
//

LPTSTR PathFindExtension(LPCTSTR pszPath)
{
    LPCTSTR pszDot;

    for (pszDot = NULL; *pszPath; pszPath = CharNext(pszPath))
    {
        switch (*pszPath) {
        case TEXT('.'):
            pszDot = pszPath;         // remember the last dot
            break;
        case TEXT('\\'):
        case TEXT(' '):         // extensions can't have spaces
            pszDot = NULL;       // forget last dot, it was in a directory
            break;
        }
    }

    // if we found the extension, return ptr to the dot, else
    // ptr to end of the string (NULL extension) (cast->non const)
    return pszDot ? (LPTSTR)pszDot : (LPTSTR)pszPath;
}

//--------------------------------------------------------------------------
// Return a pointer to the end of the next path componenent in the string.
// ie return a pointer to the next backslash or terminating NULL.
LPCTSTR GetPCEnd(LPCTSTR lpszStart)
{
        LPCTSTR lpszEnd;

        lpszEnd = StrChr(lpszStart, TEXT('\\'));
        if (!lpszEnd)
        {
                lpszEnd = lpszStart + lstrlen(lpszStart);
        }

        return lpszEnd;
}
//--------------------------------------------------------------------------
// Given a pointer to the end of a path component, return a pointer to
// its begining.
// ie return a pointer to the previous backslash (or start of the string).
LPCTSTR PCStart(LPCTSTR lpszStart, LPCTSTR lpszEnd)
{
    LPCTSTR lpszBegin = StrRChr(lpszStart, lpszEnd, TEXT('\\'));
    if (!lpszBegin)
    {
            lpszBegin = lpszStart;
    }
    return lpszBegin;
}

//--------------------------------------------------------------------------
// Fix up a few special cases so that things roughly make sense.
void NearRootFixups(LPTSTR lpszPath, BOOL fUNC)
    {
    // Check for empty path.
    if (lpszPath[0] == TEXT('\0'))
        {
        // Fix up.
        lpszPath[0] = TEXT('\\');
        lpszPath[1] = TEXT('\0');
        }
    // Check for missing slash.
    if (!IsDBCSLeadByte(lpszPath[0]) && lpszPath[1] == TEXT(':') && lpszPath[2] == TEXT('\0'))
        {
        // Fix up.
        lpszPath[2] = TEXT('\\');
        lpszPath[3] = TEXT('\0');
        }
    // Check for UNC root.
    if (fUNC && lpszPath[0] == TEXT('\\') && lpszPath[1] == TEXT('\0'))
        {
        // Fix up.
        lpszPath[0] = TEXT('\\');
        lpszPath[1] = TEXT('\\');
        lpszPath[2] = TEXT('\0');
        }
    }

//--------------------------------------------------------------------------
// Canonicalizes a path.
BOOL PathCanonicalize(LPTSTR lpszDst, LPCTSTR lpszSrc)
    {
    LPCTSTR lpchSrc;
    LPCTSTR lpchPCEnd;           // Pointer to end of path component.
    LPTSTR lpchDst;
    BOOL fUNC;
    int cbPC;

    fUNC = PathIsUNC(lpszSrc);    // Check for UNCness.

    // Init.
    lpchSrc = lpszSrc;
    lpchDst = lpszDst;

    while (*lpchSrc)
        {
        // this should just return the count
        lpchPCEnd = GetPCEnd(lpchSrc);
        cbPC = (lpchPCEnd - lpchSrc)+1;

        // Check for slashes.
        if (cbPC == 1 && *lpchSrc == TEXT('\\'))
            {
            // Just copy them.
            *lpchDst = TEXT('\\');
            lpchDst++;
            lpchSrc++;
            }
        // Check for dots.
        else if (cbPC == 2 && *lpchSrc == TEXT('.'))
            {
            // Skip it...
            // Are we at the end?
            if (*(lpchSrc+1) == TEXT('\0'))
                {
                lpchDst--;
                lpchSrc++;
                }
            else
                lpchSrc += 2;
            }
        // Check for dot dot.
        else if (cbPC == 3 && *lpchSrc == TEXT('.') && *(lpchSrc + 1) == TEXT('.'))
            {
            // make sure we aren't already at the root
            if (!PathIsRoot(lpszDst))
                {
                // Go up... Remove the previous path component.
                lpchDst = (LPTSTR)PCStart(lpszDst, lpchDst - 1);
                }
            else
                {
                // When we can't back up, remove the trailing backslash
                // so we don't copy one again. (C:\..\FOO would otherwise
                // turn into C:\\FOO).
                if (*(lpchSrc + 2) == TEXT('\\'))
                    {
                    lpchSrc++;
                    }
                }
            lpchSrc += 2;       // skip ".."
            }
        // Everything else
        else
            {
            // Just copy it.
            lstrcpyn(lpchDst, lpchSrc, cbPC);
            lpchDst += cbPC - 1;
            lpchSrc += cbPC - 1;
            }
        // Keep everything nice and tidy.
        *lpchDst = TEXT('\0');
        }

    // Check for weirdo root directory stuff.
    NearRootFixups(lpszDst, fUNC);

    return TRUE;
    }


// Modifies:
//      szRoot
//
// Returns:
//      TRUE if a drive root was found
//      FALSE otherwise
//
BOOL PathStripToRoot(LPTSTR pszRoot)
{
    while(!PathIsRoot(pszRoot))
    {
        if (!PathRemoveFileSpec(pszRoot))
        {
            // If we didn't strip anything off,
            // must be current drive
            return(FALSE);
        }
    }

    return(TRUE);
}


// concatinate lpszDir and lpszFile into a properly formed path
// and canonicalizes any relative path pieces
//
// returns:
//  pointer to destination buffer
//
// lpszDest and lpszFile can be the same buffer
// lpszDest and lpszDir can be the same buffer
//
// assumes:
//      lpszDest is MAX_PATH bytes
//
//

LPTSTR PathCombine(LPTSTR lpszDest, LPCTSTR lpszDir, LPCTSTR lpszFile)
{
    TCHAR szTemp[MAX_PATH];
    LPTSTR pszT;

    if (!lpszFile || *lpszFile==TEXT('\0')) {

        lstrcpyn(szTemp, lpszDir, ARRAYSIZE(szTemp));       // lpszFile is empty

    } else if (lpszDir && *lpszDir && PathIsRelative(lpszFile)) {

        lstrcpyn(szTemp, lpszDir, ARRAYSIZE(szTemp));
        pszT = PathAddBackslash(szTemp);
        if (pszT) {
            int iLen = lstrlen(szTemp);
            if ((iLen + lstrlen(lpszFile)) < ARRAYSIZE(szTemp)) {
                lstrcpy(pszT, lpszFile);
            } else
                return NULL;
        } else
            return NULL;

    } else if (lpszDir && *lpszDir &&
        *lpszFile == TEXT('\\') && !PathIsUNC(lpszFile)) {

        lstrcpyn(szTemp, lpszDir, ARRAYSIZE(szTemp));
        // Note that we do not check that an actual root is returned;
        // it is assumed that we are given valid parameters
        PathStripToRoot(szTemp);

        pszT = PathAddBackslash(szTemp);
        if (pszT)
        {
            // Skip the backslash when copying
            lstrcpyn(pszT, lpszFile+1, ARRAYSIZE(szTemp) - 1 - (pszT-szTemp));
        } else
            return NULL;

    } else {

        lstrcpyn(szTemp, lpszFile, ARRAYSIZE(szTemp));     // already fully qualified file part

    }

    PathCanonicalize(lpszDest, szTemp); // this deals with .. and . stuff

    return lpszDest;
}

// rips the last part of the path off including the backslash
//      C:\foo      -> C:\      ;
//      C:\foo\bar  -> C:\foo
//      C:\foo\     -> C:\foo
//      \\x\y\x     -> \\x\y
//      \\x\y       -> \\x
//      \\x         -> ?? (test this)
//      \foo        -> \  (Just the slash!)
//
// in/out:
//      pFile   fully qualified path name
// returns:
//      TRUE    we stripped something
//      FALSE   didn't strip anything (root directory case)
//

BOOL PathRemoveFileSpec(LPTSTR pFile)
{
    LPTSTR pT;
    LPTSTR pT2 = pFile;

    for (pT = pT2; *pT2; pT2 = CharNext(pT2)) {
        if (*pT2 == TEXT('\\'))
            pT = pT2;             // last "\" found, (we will strip here)
        else if (*pT2 == TEXT(':')) {   // skip ":\" so we don't
            if (pT2[1] ==TEXT('\\'))    // strip the "\" from "C:\"
                pT2++;
            pT = pT2 + 1;
        }
    }
    if (*pT == 0)
        return FALSE;   // didn't strip anything

    //
    // handle the \foo case
    //
    else if ((pT == pFile) && (*pT == TEXT('\\'))) {
        // Is it just a '\'?
        if (*(pT+1) != TEXT('\0')) {
            // Nope.
            *(pT+1) = TEXT('\0');
            return TRUE;        // stripped something
        }
        else        {
            // Yep.
            return FALSE;
        }
    }
    else {
        *pT = 0;
        return TRUE;    // stripped something
    }
}

// add a backslash to a qualified path
//
// in:
//  lpszPath    path (A:, C:\foo, etc)
//
// out:
//  lpszPath    A:\, C:\foo\    ;
//
// returns:
//  pointer to the NULL that terminates the path


LPTSTR PathAddBackslash(LPTSTR lpszPath)
{
    LPTSTR lpszEnd;

    // try to keep us from tromping over MAX_PATH in size.
    // if we find these cases, return NULL.  Note: We need to
    // check those places that call us to handle their GP fault
    // if they try to use the NULL!
    int ichPath = lstrlen(lpszPath);
    if (ichPath >= (MAX_PATH - 1))
    {
        Assert(FALSE);      // Let the caller know!
        return(NULL);
    }

    lpszEnd = lpszPath + ichPath;

    // this is really an error, caller shouldn't pass
    // an empty string
    if (!*lpszPath)
        return lpszEnd;

    /* Get the end of the source directory
    */
    switch(*CharPrev(lpszPath, lpszEnd)) {
    case TEXT('\\'):
        break;

    default:
        *lpszEnd++ = TEXT('\\');
        *lpszEnd = TEXT('\0');
    }
    return lpszEnd;
}


// Returns a pointer to the last component of a path string.
//
// in:
//      path name, either fully qualified or not
//
// returns:
//      pointer into the path where the path is.  if none is found
//      returns a poiter to the start of the path
//
//  c:\foo\bar  -> bar
//  c:\foo      -> foo
//  c:\foo\     -> c:\foo\      ( is this case busted?)
//  c:\         -> c:\          ( this case is strange)
//  c:          -> c:
//  foo         -> foo


LPTSTR PathFindFileName(LPCTSTR pPath)
{
    LPCTSTR pT;

    for (pT = pPath; *pPath; pPath = CharNext(pPath)) {
        if ((pPath[0] == TEXT('\\') || pPath[0] == TEXT(':')) && pPath[1] && (pPath[1] != TEXT('\\')))
            pT = pPath + 1;
    }

    return (LPTSTR)pT;   // const -> non const
}

//---------------------------------------------------------------------------
// Returns TRUE if the given string is a UNC path.
//
// TRUE
//      "\\foo\bar"
//      "\\foo"         <- careful
//      "\\"
// FALSE
//      "\foo"
//      "foo"
//      "c:\foo"

BOOL PathIsUNC(LPCTSTR pszPath)
{
    return DBL_BSLASH(pszPath);
}

//---------------------------------------------------------------------------
// Return TRUE if the path isn't absoulte.
//
// TRUE
//      "foo.exe"
//      ".\foo.exe"
//      "..\boo\foo.exe"
//
// FALSE
//      "\foo"
//      "c:bar"     <- be careful
//      "c:\bar"
//      "\\foo\bar"

BOOL PathIsRelative(LPCTSTR lpszPath)
{
    // The NULL path is assumed relative
    if (*lpszPath == 0)
        return TRUE;

    // Does it begin with a slash ?
    if (lpszPath[0] == TEXT('\\'))
        return FALSE;
    // Does it begin with a drive and a colon ?
    else if (!IsDBCSLeadByte(lpszPath[0]) && lpszPath[1] == TEXT(':'))
        return FALSE;
    // Probably relative.
    else
        return TRUE;
}

#pragma data_seg(".text", "CODE")

const TCHAR c_szColonSlash[] = TEXT(":\\");

#pragma data_seg()

// check if a path is a root
//
// returns:
//  TRUE for "\" "X:\" "\\foo\asdf" "\\foo\"
//  FALSE for others

BOOL  PathIsRoot(LPCTSTR pPath)
{
    if (!IsDBCSLeadByte(*pPath))
    {
        if (!lstrcmpi(pPath + 1, c_szColonSlash))                  // "X:\" case
            return TRUE;
    }

    if ((*pPath == TEXT('\\')) && (*(pPath + 1) == 0))        // "\" case
        return TRUE;

    if (DBL_BSLASH(pPath))      // smells like UNC name
    {
        LPCTSTR p;
        int cBackslashes = 0;

        for (p = pPath + 2; *p; p = CharNext(p)) {
            if (*p == TEXT('\\') && (++cBackslashes > 1))
               return FALSE;   /* not a bare UNC name, therefore not a root dir */
        }
        return TRUE;    /* end of string with only 1 more backslash */
                        /* must be a bare UNC, which looks like a root dir */
    }
    return FALSE;
}

BOOL OnExtList(LPCTSTR pszExtList, LPCTSTR pszExt)
{
    for (; *pszExtList; pszExtList += lstrlen(pszExtList) + 1)
    {
        if (!lstrcmpi(pszExt, pszExtList))
        {
            return TRUE;        // yes
        }
    }

    return FALSE;
}

#pragma data_seg(".text", "CODE")
// what about .cmd?
const TCHAR achExes[] = TEXT(".bat\0.pif\0.exe\0.com\0");
#pragma data_seg()

// determine if a path is a program by looking at the extension
//
BOOL PathIsExe(LPCTSTR szFile)
{
    LPCTSTR temp = PathFindExtension(szFile);
    return OnExtList((LPCTSTR) achExes, temp);
}

