//////////////////////////////////////////////////////////////////////////
//   Module:    AVWIN5.C                                                //
//   Target:    AVWIN.DLL                                               //
//                                                                      //
//   Summary:   This module contains the configuration dialog box	//
//		handler and utility routines for AVWIN.DLL.		//
//                                                                      //
//////////////////////////////////////////////////////////////////////////
//---------------------------------------------------------------------------
//
//  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
//  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
//  PURPOSE.
//
//  Copyright (c) 1994 - 1995 Microsoft Corporation.	All Rights Reserved.
//
//---------------------------------------------------------------------------


//////////////////////////////////////////
//  Includes                            //
//////////////////////////////////////////
#include <windows.h>
#include <ctype.h>
#include "avwin.h"
#include "avwinrc.h"
#include "global.h"


//////////////////////////////////////////////////////////////////////////
//      Configuration dialog box handler.                               //
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//      AVConfigDlgProc(hDlg, message, wParam, lParam)                  //
//                                                                      //
//      This function is no longer used by AVWIN.DLL, and is retained	//
//	for compatibility reasons only.  In previous versions, this	//
//	function was used by the AV_ConfigureVideo function.  Now the	//
//	AV_ConfigureVideo function brings up the program AVCONFIG.EXE	//
//	instead.							//
//////////////////////////////////////////////////////////////////////////
BOOL _export FAR PASCAL AVConfigDlgProc(
        HWND    hDlg,
        UINT    message,
        WPARAM  wParam,
        LPARAM  lParam)
        {
	return FALSE;
        }


//////////////////////////////////////////////////////////////////////////
//      Functions for reading parameters from INI files.                //
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//      AVGetIniArray(lpwArray, bSize, szSection, szEntry, szFilename)  //
//                                                                      //
//      Returns the number of values found on the selected line of the  //
//      INI file, and fills the given array with the values.  Values in //
//      the INI file may be in decimal or hex format, with hex values   //
//      ending with an 'h' or 'H'.                                      //
//                                                                      //
//      If no number string is found in the INI entry, 0 will be        //
//      returned.                                                       //
//////////////////////////////////////////////////////////////////////////
BYTE FAR PASCAL AVGetIniArray(
        LPWORD  lpwArray,               // Pointer to array to be filled.
        UINT    bSize,                  // Number of elements in array.
        LPSTR   lpSection,              // Section name in INI file.
        LPSTR   lpEntry,                // Entry name in INI file.
        LPSTR   lpDefault,              // Default string
        LPSTR   lpFilename )            // Path/filename of INI file.
        {
        char    szBuffer[128];
        LPSTR   lpBuffer, lpStart, lpEnd;
        UINT    bCount;

        GetPrivateProfileString( lpSection, lpEntry, lpDefault, szBuffer,
                sizeof(szBuffer), lpFilename );

        lpBuffer = (LPSTR)szBuffer;

        for (bCount = 0; bCount < bSize; bCount++)
                {
                lpStart = FindStartOfNumber(lpBuffer);
                if (!lpStart)
                        break;
                lpEnd = FindEndOfNumber(lpStart);

                if ((*lpEnd == 'h') || (*lpEnd == 'H'))
                        *lpwArray++ = (WORD)ahtoi(lpStart);
                else
                        *lpwArray++ = (WORD)DecimalToLong(lpStart);

                lpBuffer = lpEnd;
                }
        return bCount;
        }


//////////////////////////////////////////////////////////
//      FindStartOfNumber(szString)                     //
//                                                      //
//      Returns a pointer to the first character in the //
//      given string that is a decimal or hex digit.    //
//      If no digit is found before the end of the      //
//      string, a NULL pointer is returned.             //
//////////////////////////////////////////////////////////
LPSTR FindStartOfNumber(LPSTR szString)
        {
        char    cTemp;

        // Find beginning of the next hex or decimal string.
        while (TRUE)
                {
                cTemp = *szString;
                if ((cTemp == '\0') || (cTemp == ';'))
                        return NULL;
                if (isxdigit(cTemp))
                        return szString;
                szString++;
                }
        }


//////////////////////////////////////////////////////////
//      FindEndOfNumber(szString)                       //
//                                                      //
//      Returns a pointer to the first character in the //
//      given string that is not a decimal or hex digit.//
//////////////////////////////////////////////////////////
LPSTR FindEndOfNumber(LPSTR szString)
        {
        char    cTemp;

        // Find end of this number string.
        while (TRUE)
                {
                cTemp = *szString;
                if ((cTemp == '\0') || (cTemp == ';'))
                        return szString;
                if (!isxdigit(cTemp))
                        return szString;
                szString++;
                }
        }


//////////////////////////////////////////////////////////
//      ahtoi(ptr)                                      //
//                                                      //
//      Returns value of ascii hex string.              //
//////////////////////////////////////////////////////////
DWORD ahtoi( LPSTR ptr )
{
    unsigned int    c;
    unsigned long   hexnum;

    hexnum = 0;

    while( TRUE ) {
        c = toupper( (unsigned int)(*ptr) );

        if ( isxdigit(c) ) {
            hexnum = (hexnum << 4) | ((c > '9') ? c - 0x37 : c - 0x30);
        } else {
            return( hexnum );
        }
        ++ptr;
    }
}


//////////////////////////////////////////////////////////
//      DecimalToLong(ptr)                              //
//                                                      //
//      Returns value of ascii decimal string.          //
//////////////////////////////////////////////////////////
DWORD DecimalToLong(LPSTR ptr)
        {
        long    lTemp = 0;
        char    cTemp;

        while (cTemp = *ptr++)
                {
                if (!isdigit(cTemp))
                        break;
                lTemp = (10 * lTemp) + (cTemp - '0');
                }
        return lTemp;
        }


