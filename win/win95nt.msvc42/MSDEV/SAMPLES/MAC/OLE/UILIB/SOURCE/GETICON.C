/*
 *  GetIconSeg.C
 *
 *  Functions to create DVASPECT_ICON metafile from filename or classname.
 *
 *  OleStdGetUserTypeOfClass
 *  OleStdGetAuxUserType
 *
 *  (c) Copyright Microsoft Corp. 1992-1994 All Rights Reserved
 */



#if defined(USEHEADER)
#include "OleHeaders.h"
#endif

#if defined(__MWERKS__) && !defined(__powerc)
#pragma pointers_in_D0
#endif

#if defined(_MSC_VER) && defined(__powerc)
#include <msvcmac.h>
#endif

#include <Events.h>
#include <Menus.h>
#ifndef _MSC_VER
#include <AppleEvents.h>
#include <Processes.h>
#else
#include <AppleEve.h>
#include <Processe.h>
#endif

#include <ole2.h>

#include <stdio.h>
#include <string.h>
// #include "GetIcon.h"
#include "ole2ui.h"


OLEDBGDATA


/*
 * OleStdGetUserTypeOfClass(REFCLSID, char *, unsigned int, HKEY)
 *
 * Purpose:
 *  Returns the user type (human readable class name) of the specified class.
 *
 * Parameters:
 *  rclsid          pointer to the clsid to retrieve user type of.
 *  lpszUserType    pointer to buffer to return user type in.
 *  cch             length of buffer pointed to by lpszUserType
 *  hKey            hKey for reg db - if this is NULL, then we
 *                   open and close the reg db within this function.  If it
 *                   is non-NULL, then we assume it's a valid key to the
 *                   \ root and use it without closing it. (useful
 *                   if you're doing lots of reg db stuff).
 *
 * Return Value:
 *  unsigned int    Number of characters in returned string.  0 on error.
 *
 */

#ifndef _MSC_VER
#pragma segment GetIconSeg
#else
#pragma code_seg("GetIconSeg", "SWAPPABLE")
#endif
STDAPI_(unsigned long) OleStdGetUserTypeOfClass(REFCLSID rclsid, char * lpszUserType, unsigned int cch, HKEY hKey)
{

   unsigned long    dw;
   long     lRet;
   char     *lpszCLSID, *lpszProgID;
   Boolean  fFreeProgID = false;
   Boolean  bCloseRegDB = false;
   char     szKey[128];
   LPMALLOC lpIMalloc;

   if (hKey == (HKEY)NULL)
   {

     //Open up the root key.
     lRet=RegOpenKey(HKEY_CLASSES_ROOT, NULL, &hKey);

     if ((long)ERROR_SUCCESS!=lRet)
       return (unsigned int)false;

     bCloseRegDB = true;
   }

   // Get a string containing the class name
   StringFromCLSID(rclsid, &lpszCLSID);

   sprintf(szKey, "CLSID\\%s", lpszCLSID);

   dw=cch;
   lRet = RegQueryValue(hKey, szKey, lpszUserType, (long *)&dw);

   if ((long)ERROR_SUCCESS!=lRet)
       dw = 0;

   if ( ((long)ERROR_SUCCESS!=lRet) && (CoIsOle1Class(rclsid)) )
   {
      // We've got an OLE 1.0 class, so let's try to get the user type
      // name from the ProgID entry.

      ProgIDFromCLSID(rclsid, &lpszProgID);
      fFreeProgID = true;

      dw = cch;
      lRet = RegQueryValue(hKey, lpszProgID, lpszUserType, (long *)&dw);

      if ((long)ERROR_SUCCESS != lRet)
        dw = 0;
   }


   if (NOERROR == CoGetMalloc(MEMCTX_TASK, &lpIMalloc))
   {
       if (fFreeProgID)
         lpIMalloc->lpVtbl->Free(lpIMalloc, (void *)lpszProgID);

       lpIMalloc->lpVtbl->Free(lpIMalloc, (void *)lpszCLSID);
       lpIMalloc->lpVtbl->Release(lpIMalloc);
   }

   if (bCloseRegDB)
      RegCloseKey(hKey);

   return (unsigned int)dw;

}



/*
 * OleStdGetAuxUserType(RCLSID, short, char *, int, HKEY)
 *
 * Purpose:
 *  Returns the specified AuxUserType from the reg db.
 *
 * Parameters:
 *  rclsid          pointer to the clsid to retrieve aux user type of.
 *  hKey            hKey for reg db - if this is NULL, then we
 *                   open and close the reg db within this function.  If it
 *                   is non-NULL, then we assume it's a valid key to the
 *                   \ root and use it without closing it. (useful
 *                   if you're doing lots of reg db stuff).
 *  wAuxUserType    which aux user type field to look for.  In 4/93 release
 *                  2 is short name and 3 is exe name.
 *  lpszUserType    pointer to buffer to return user type in.
 *  cch             length of buffer pointed to by lpszUserType
 *
 * Return Value:
 *  unsigned int            Number of characters in returned string.  0 on error.
 *
 */

#ifndef _MSC_VER
#pragma segment GetIconSeg
#else
#pragma code_seg("GetIconSeg", "SWAPPABLE")
#endif
STDAPI_(unsigned long) OleStdGetAuxUserType(REFCLSID rclsid, short wAuxUserType,  char *lpszAuxUserType,
                                  int cch, HKEY hKey)
{
	HKEY			hThisKey;
	Boolean			fCloseRegDB = false;
	unsigned long	dw;
	long			lRet;
	char			*lpszCLSID;
	LPMALLOC		lpIMalloc;
	char			szKey[OLEUI_CCHKEYMAX];
	char			szTemp[32];

	lpszAuxUserType[0] = 0;

	if (hKey == (HKEY)NULL)
	{
		lRet = RegOpenKey(HKEY_CLASSES_ROOT, NULL, &hThisKey);

		if (ERROR_SUCCESS != lRet)
			 return 0;

		fCloseRegDB = true;
	}
	else
		hThisKey = hKey;

	StringFromCLSID(rclsid, &lpszCLSID);

	strcpy(szKey, "CLSID\\");
	strcat(szKey, lpszCLSID);
	sprintf(szTemp, "\\AuxUserType\\%ld", (long)wAuxUserType);
	strcat(szKey, szTemp);

	dw = cch;

	lRet = RegQueryValue(hThisKey, szKey, lpszAuxUserType, (long *)&dw);

	if (ERROR_SUCCESS != lRet) {
	  dw = 0;
	  lpszAuxUserType[0] = 0;
	}

	if (fCloseRegDB)
		RegCloseKey(hThisKey);

	if (NOERROR == CoGetMalloc(MEMCTX_TASK, &lpIMalloc))
	{
		 lpIMalloc->lpVtbl->Free(lpIMalloc, (void *)lpszCLSID);
		 lpIMalloc->lpVtbl->Release(lpIMalloc);
	}

	return (unsigned int)dw;
}
