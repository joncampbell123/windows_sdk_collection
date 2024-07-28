/**[f******************************************************************
* transtbl.c -
*
* Copyright (C) 1988,1989 Aldus Corporation
* Copyright (C) 1989-1990 Microsoft Corporation.
* Copyright (C) 1989,1990,1991 Hewlett-Packard Company.
*     All rights reserved.
*     Company confidential.
*
**f]*****************************************************************/
  
/*
 * $Header: 
 */

/*
 * $Log:
 */

/******************************   transtbl.c   *****************************/
/*
*  TransTbl:  Utility for reading character translation table from resources.
*/
/***************************************************************************/
//
// History
//  17 jul 89   peterbe     Added DESKJET8 support from HP.
//                  HP put this in wrong place relative
//                  to 'case epsymGENERIC7:',
//                     'case epsymUSLegal:'
//
/***************************************************************************/
  
#include "nocrap.h"
#include "windows.h"
#include "pfm.h"
#include "transtbl.h"
#define NO_PRINTER_STUFF
#include "hppcl.h"
#include "resource.h"
  
  
  
/*  GetTransTable
*
*  Read the translation table from the resources, the caller is
*  responsible for unlocking/removing the table.
*/
HANDLE FAR PASCAL GetTransTable(hModule, lpLPTransTbl, symbolSet)
HANDLE hModule;
LPSTR FAR *lpLPTransTbl;
SYMBOLSET symbolSet;
{
    LPSTR transResID = 0L;
    HANDLE hTransInfo = 0;
    HANDLE hTransData = 0;
  
    *lpLPTransTbl = 0L;
  
    switch (symbolSet)
    {
        case epsymRoman8:
            transResID = (LPSTR)XTBL_ROMAN8;
            break;
        case epsymUSASCII:
            transResID = (LPSTR)XTBL_USASCII;
            break;
        case epsymECMA94:
            transResID = (LPSTR)XTBL_ECMA94;
            break;
        case epsymGENERIC8:
        case epsymKana8:
        case epsymMath8:
            transResID = (LPSTR)XTBL_GENERIC8;
            break;
        case epsymDESKJET8:
            transResID = (LPSTR)XTBL_DESKJET8;
            break;
        case epsymGENERIC7:
        case epsymUSLegal:
        default:
            transResID = (LPSTR)XTBL_GENERIC7;
            break;
    }
  
    if ((hTransInfo = FindResource(hModule, transResID, (LPSTR)TRANSTBL)) &&
        (hTransData = LoadResource(hModule, hTransInfo)))
    {
        if (!(*lpLPTransTbl = LockResource(hTransData)))
        {
            FreeResource(hTransData);
            hTransData = 0;
            hTransInfo = 0;
        }
    }
  
  
    return (hTransData);
}
  
