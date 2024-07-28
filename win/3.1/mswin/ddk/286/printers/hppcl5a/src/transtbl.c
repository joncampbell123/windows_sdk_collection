/**[f******************************************************************
* transtbl.c -
*
* Copyright (C) 1988,1989 Aldus Corporation.
* Copyright (C) 1989-1990 Microsoft Corporation.
* Copyright (C) 1989,1990, 1991 Hewlett-Packard Company.
* All rights reserved.
* Company confidential.
*
**f]*****************************************************************/
  
/*
* $Header: transtbl.c,v 3.890 92/02/06 16:12:27 dtk FREEZE $
*/
  
/*
* $Log:	transtbl.c,v $
 * Revision 3.890  92/02/06  16:12:27  16:12:27  dtk (Doug Kaltenecker)
 * Win3.1 Freeze
 * 
 * Revision 3.870  91/11/08  11:44:11  11:44:11  dtk (Doug Kaltenecker)
 * 3.1 Release Candidate 1
 * 
 * Revision 3.865  91/11/01  13:52:09  13:52:09  dtk (Doug Kaltenecker)
 * Beta release for Windows 3.1
 * 
 * Revision 3.862  91/10/25  13:47:29  13:47:29  dtk (Doug Kaltenecker)
 * BETA
 * 
 * Revision 3.860  91/10/23  09:48:57  09:48:57  dtk (Doug Kaltenecker)
 * WinWird Release
 * 
 * Revision 3.852  91/10/09  14:59:57  14:59:57  dtk (Doug Kaltenecker)
 * BETA
 * 
 * Revision 3.850  91/10/04  16:50:12  16:50:12  dtk (Doug Kaltenecker)
 * WINWORD RELEASE CANDIDATE
 * 
 * Revision 3.840  91/09/28  14:17:28  14:17:28  dtk (Doug Kaltenecker)
 * RELEASE CANDIDATE 3 FOR WINWORD
 * 
 * Revision 3.830  91/09/18  16:33:45  16:33:45  dtk (Doug Kaltenecker)
 * RELEASE
 * 
 * Revision 3.822  91/09/16  10:34:27  10:34:27  dtk (Doug Kaltenecker)
 * WINWORD RELEASE CANDIDATE 2 FOR WIN30.
 * 
 * Revision 3.820  91/09/06  14:12:24  14:12:24  dtk (Doug Kaltenecker)
 * RELEASE CANDIDATE FOR WINWORD 2.0
 * 
 * Revision 3.812  91/08/22  14:32:19  14:32:19  dtk (Doug Kaltenecker)
 * BETA
 * 
 * Revision 3.808  91/08/21  15:34:52  15:34:52  jsmart (Jerry Smart)
 * add roman8 remapping support
 * 
 * Revision 3.807  91/08/08  10:31:36  10:31:36  dtk (Doug Kaltenecker)
 * PREBETA3
 * 
 * Revision 3.802  91/07/22  12:54:39  12:54:39  oakeson (Ken Oakeson)
 * BETA
 * 
 * Revision 3.799  91/07/02  11:52:02  11:52:02  daniels (Susan Daniels)
 * Beta
 * 
 * Revision 3.796  91/06/26  11:26:22  11:26:22  stevec (Steve Claiborne)
 * BETA
 * 
 * Revision 3.790  91/06/11  16:03:37  16:03:37  stevec (Steve Claiborne)
 * Freeze
 * 
 * Revision 3.786  91/06/11  15:44:42  15:44:42  dtk (Doug Kaltenecker)
 * Prettified files!
 * 
* Revision 3.785  91/05/22  14:57:14  14:57:14  stevec (Steve Claiborne)
* Beta version to MS
*
* Revision 3.780  91/05/15  15:57:23  15:57:23  stevec (Steve Claiborne)
* Beta
*
* Revision 3.775  91/04/05  14:31:16  14:31:16  stevec (Steve Claiborne)
* Beta release to MS
*
* Revision 3.770  91/03/25  15:36:15  15:36:15  stevec (Steve Claiborne)
* maintance release
*
* Revision 3.760  91/03/12  07:53:01  07:53:01  stevec (Steve Claiborne)
* Maintance release
*
* Revision 3.755  91/03/03  07:46:20  07:46:20  stevec (Steve Claiborne)
* March 3 Freeze
*
* Revision 3.720  91/02/11  09:15:38  09:15:38  stevec (Steve Claiborne)
* Aldus version
*
* Revision 3.711  91/02/08  16:26:45  16:26:45  stevec (Steve Claiborne)
* Added debuging
*
* Revision 3.710  91/02/04  15:47:55  15:47:55  stevec (Steve Claiborne)
* Aldus freeze
*
* Revision 3.700  91/01/19  09:00:35  09:00:35  stevec (Steve Claiborne)
* Release
*
* Revision 3.685  91/01/14  15:43:31  15:43:31  stevec (Steve Claiborne)
* Freeze
*
* Revision 3.681  91/01/14  10:17:48  10:17:48  stevec (Steve Claiborne)
* Updated the copy right stmt.
*
* Revision 3.680  91/01/10  16:16:53  16:16:53  stevec (Steve Claiborne)
* Freeze
*
* Revision 3.670  90/12/14  14:54:19  14:54:19  stevec (Steve Claiborne)
* freeze for 12-14-90 ver. 3.670
*
* Revision 3.665  90/12/10  15:36:03  15:36:03  stevec (Steve Claiborne)
* Freeze
*
* Revision 3.660  90/12/07  14:50:31  14:50:31  stevec (Steve Claiborne)
* Freeze 12-7-90
*
* Revision 3.650  90/11/30  08:12:25  08:12:25  stevec (Steve Claiborne)
* Freeze 3.650, 11-30-90
*
* Revision 3.601  90/08/24  11:38:01  11:38:01  daniels (Susan Daniels)
* message.txt
*
* Revision 3.600  90/08/03  11:10:00  11:10:00  stevec (Steve Claiborne)
* This is the Aug. 3 release ver. 3.600
*
* Revision 3.550  90/07/27  11:31:15  11:31:15  root ()
* Experimental freeze 3.55
*
* Revision 3.541  90/07/27  08:20:42  08:20:42  oakeson ()
* Only get trans table for USASCII, ECMA-94, or Generic7
*
* Revision 3.540  90/07/25  12:34:49  12:34:49  stevec (Steve Claiborne)
* Experimental freeze 3.54
*
* Revision 3.521  90/07/19  12:39:22  12:39:22  oakeson ()
* Changed Roman8, Math8, and default symsets to Generic8
*
*/
  
  
/***************************************************************************/
/******************************   transtbl.c   *****************************/
/*
*  TransTbl:  Utility for reading character translation table from resources.
*/
  
// history
// 25 aug 89    craigc      Added MATH8 symbol font support.
// 27 apr 89    peterbe     Tabs @ 8 spaces, add a FF.
  
#include "nocrap.h"
#include "windows.h"
#include "pfm.h"
#include "transtbl.h"
#define NO_PRINTER_STUFF
#include "hppcl.h"
#include "resource.h"
#include "debug.h"
  
  
#define DBGtrans(msg) DBMSG(msg)
  
  
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
  
    /* Tetra -- only translate USASCII, ECMA94, Generic7 */
#ifdef DEBUG_FUNCT
    DB(("Entering GetTransTable\n"));
#endif
    switch (symbolSet)
    {
            /* Added Roman8 table for old bit mapped cartridges
               for Europe      jcs     */
	case epsymRoman8:
	    DBGtrans(("GetTransTable(): Roman8 translation\n"));
	    transResID = (LPSTR)XTBL_ROMAN8;
	    break;
        case epsymUSASCII:
            DBGtrans(("GetTransTable(): USASCII translation\n"));
            transResID = (LPSTR)XTBL_USASCII;
            break;
        case epsymECMA94:
            DBGtrans(("GetTransTable(): ECMA94 translation\n"));
            transResID = (LPSTR)XTBL_ECMA94;
            break;
            /* Must be epsymGENERIC7 */
        default:
            DBGtrans(("GetTransTable(): GENERIC7 translation\n"));
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
  
    DBGtrans(("GetTransTable(): ...return %d\n", hTransData));
  
#ifdef DEBUG_FUNCT
    DB(("Entering GetTransTable\n"));
#endif
    return (hTransData);
}
