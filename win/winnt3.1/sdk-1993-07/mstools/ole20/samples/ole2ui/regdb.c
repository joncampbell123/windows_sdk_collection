/*
 *  REGDB.C
 *
 *  Functions to query the registration database
 *
 *  OleStdGetMiscStatusOfClass
 *  OleStdGetDefaultFileFormatOfClass
 *
 *  NOTE: more regdb functions are in geticon.c --
 *      OleStdGetAuxUserType
 *      OleStdGetUserTypeOfClass
 *
 *    (c) Copyright Microsoft Corp. 1992-1993 All Rights Reserved
 *
 */

#define STRICT  1
#include "ole2ui.h"   
#include <ctype.h>

OLEDBGDATA


// Replacement for stdlib atol, 
// which didn't work and doesn't take far pointers.
// Must be tolerant of leading spaces.
// 
//
static LONG Atol(LPSTR lpsz)
{
    signed int sign = +1;
    UINT base = 10;
    LONG l = 0;

    if (NULL==lpsz)
    {
        OleDbgAssert (0);
        return 0;
    }
    while (isspace(*lpsz))
        lpsz++;
    
    if (*lpsz=='-')
    {
        lpsz++;
        sign = -1;
    }      
    if (lpsz[0]=='0' && lpsz[1]=='x')
    {
        base = 16;  
        lpsz+=2;
    }

    if (base==10)
    {
        while (isdigit(*lpsz))
        {
            l = l * base + *lpsz - '0';
            lpsz++;
        }
    }
    else
    {
        OleDbgAssert (base==16);
        while (isxdigit(*lpsz))
        {
            l = l * base + isdigit(*lpsz) ? *lpsz - '0' : toupper(*lpsz) - 'A' + 10;
            lpsz++;
        }
    }
    return l * sign;
}




/*
 * OleStdGetMiscStatusOfClass(REFCLSID, HKEY)
 *
 * Purpose:
 *  Returns the value of the misc status for the given clsid.
 *
 * Parameters:
 *  rclsid          pointer to the clsid to retrieve user type of.
 *  hKey            hKey for reg db - if this is NULL, then we
 *                   open and close the reg db within this function.  If it 
 *                   is non-NULL, then we assume it's a valid key to the 
 *                   \\CLSID root and use it without closing it. (useful 
 *                   if you're doing lots of reg db stuff).
 *
 * Return Value:
 *  BOOL            TRUE on success, FALSE on failure.
 *
 */
STDAPI_(BOOL) OleStdGetMiscStatusOfClass(REFCLSID rclsid, HKEY hKey, DWORD FAR * lpdwValue)
{
   DWORD dw;
   LONG  lRet;
   LPSTR lpszCLSID;
   char  szKey[64];
   char  szMiscStatus[OLEUI_CCHKEYMAX];
   BOOL  bCloseRegDB = FALSE;

   if (hKey == NULL)
   {

     //Open up the root key.
     lRet=RegOpenKey(HKEY_CLASSES_ROOT, "CLSID", &hKey);

     if ((LONG)ERROR_SUCCESS!=lRet)
       return FALSE;

     bCloseRegDB = TRUE;
   }

   // Get a string containing the class name 
   StringFromCLSID(rclsid, &lpszCLSID);

   // Construct key
   lstrcpy(szKey, lpszCLSID);
   lstrcat(szKey, "\\MiscStatus");


   dw=OLEUI_CCHKEYMAX;
   lRet = RegQueryValue(hKey, szKey, (LPSTR)szMiscStatus, &dw);

   if ((LONG)ERROR_SUCCESS!=lRet)
   {
       OleStdFreeString(lpszCLSID, NULL);

       if (bCloseRegDB)
          RegCloseKey(hKey);

       return FALSE;

   }

   *lpdwValue = Atol((LPSTR)szMiscStatus);

   OleStdFreeString(lpszCLSID, NULL);

   if (bCloseRegDB)
      RegCloseKey(hKey);

   return TRUE;


}


/*
 * CLIPFORMAT OleStdGetDefaultFileFormatOfClass(REFCLSID, HKEY)
 *
 * Purpose:
 *  Returns the default file format of the specified class.
 *  this is entered in REGDB as follows:
 *      CLSID\{...}\DataFormats\DefaultFile = <cfFmt>
 *
 * Parameters:
 *  rclsid          pointer to the clsid to retrieve user type of.
 *  hKey            hKey for reg db- if this is NULL, then we
 *                   open and close the reg db within this function.  If it 
 *                   is non-NULL, then we assume it's a valid key to the 
 *                   \ root and use it without closing it. (useful 
 *                   if you're doing lots of reg db stuff).
 *
 * Return Value:
 *  cfFmt   -- DefaultFile format
 *  NULL    -- failed to get default file format
 *
 */
STDAPI_(CLIPFORMAT) OleStdGetDefaultFileFormatOfClass(
        REFCLSID        rclsid, 
        HKEY            hKey
)       
{
   CLIPFORMAT cfFmt = 0;
   DWORD dw;
   LONG  lRet;
   LPSTR lpszCLSID;
   BOOL  bCloseRegDB = FALSE;
   char  szKey[128];
   char  szDefaultFile[OLEUI_CCHKEYMAX];
   BOOL  bStatus = TRUE;


   if (hKey == NULL)
   {

     //Open up the root key.
     lRet=RegOpenKey(HKEY_CLASSES_ROOT, NULL, &hKey);

     if ((LONG)ERROR_SUCCESS!=lRet)
       return 0;

     bCloseRegDB = TRUE;
   }


   // Get a string containing the class name 
   StringFromCLSID(rclsid, &lpszCLSID);

   // Construct key
   wsprintf(szKey, "CLSID\\%s\\DataFormats\\DefaultFile", lpszCLSID);

   dw=OLEUI_CCHKEYMAX;
   lRet = RegQueryValue(hKey, szKey, (LPSTR)szDefaultFile, (LONG FAR *)&dw);

   if ((LONG)ERROR_SUCCESS!=lRet)
       bStatus = FALSE;
   else {
       /* if the format is a number, then it should refer to one of the
       **    standard Windows formats.
       */
       if (isdigit(szDefaultFile[0])) 
           cfFmt = (CLIPFORMAT)Atol(szDefaultFile);
       else 
           cfFmt = RegisterClipboardFormat(szDefaultFile);
   }

   if (bCloseRegDB)
      RegCloseKey(hKey);

   return cfFmt;
}

