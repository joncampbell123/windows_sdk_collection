/**[f******************************************************************
* pclstub.c -
*
* Copyright (C) 1988,1989 Aldus Corporation.
* Copyright (C) 1989-1990 Microsoft Corporation.
* Copyright (C) 1989,1990, 1991 Hewlett-Packard Company.
* All rights reserved.
* Company confidential.
*
**f]*****************************************************************/
  
/*
*
*  PCLDOS.C - stub routine for HPPCL.DRV to report the version number
*             if HPPCL.DRV is executed without running windows.
*
*  $Revision: 3.890 $
*  $Date: 92/02/06 16:12:30 $
*  $Author: dtk $
*
*  $Log:	pclstub.c,v $
 * Revision 3.890  92/02/06  16:12:30  16:12:30  dtk (Doug Kaltenecker)
 * Win3.1 Freeze
 * 
 * Revision 3.870  91/11/08  11:44:15  11:44:15  dtk (Doug Kaltenecker)
 * 3.1 Release Candidate 1
 * 
 * Revision 3.865  91/11/01  13:52:13  13:52:13  dtk (Doug Kaltenecker)
 * Beta release for Windows 3.1
 * 
 * Revision 3.862  91/10/25  13:47:33  13:47:33  dtk (Doug Kaltenecker)
 * BETA
 * 
 * Revision 3.860  91/10/23  09:49:01  09:49:01  dtk (Doug Kaltenecker)
 * WinWird Release
 * 
 * Revision 3.852  91/10/09  15:00:01  15:00:01  dtk (Doug Kaltenecker)
 * BETA
 * 
 * Revision 3.850  91/10/04  16:50:16  16:50:16  dtk (Doug Kaltenecker)
 * WINWORD RELEASE CANDIDATE
 * 
 * Revision 3.840  91/09/28  14:17:32  14:17:32  dtk (Doug Kaltenecker)
 * RELEASE CANDIDATE 3 FOR WINWORD
 * 
 * Revision 3.830  91/09/18  16:33:49  16:33:49  dtk (Doug Kaltenecker)
 * RELEASE
 * 
 * Revision 3.822  91/09/16  10:34:33  10:34:33  dtk (Doug Kaltenecker)
 * WINWORD RELEASE CANDIDATE 2 FOR WIN30.
 * 
 * Revision 3.820  91/09/06  14:12:28  14:12:28  dtk (Doug Kaltenecker)
 * RELEASE CANDIDATE FOR WINWORD 2.0
 * 
 * Revision 3.812  91/08/22  14:32:23  14:32:23  dtk (Doug Kaltenecker)
 * BETA
 * 
 * Revision 3.807  91/08/08  10:31:40  10:31:40  dtk (Doug Kaltenecker)
 * PREBETA3
 * 
 * Revision 3.802  91/07/22  12:54:43  12:54:43  oakeson (Ken Oakeson)
 * BETA
 * 
 * Revision 3.799  91/07/02  11:52:05  11:52:05  daniels (Susan Daniels)
 * Beta
 * 
 * Revision 3.796  91/06/26  11:26:26  11:26:26  stevec (Steve Claiborne)
 * BETA
 * 
 * Revision 3.790  91/06/11  16:03:41  16:03:41  stevec (Steve Claiborne)
 * Freeze
 * 
 * Revision 3.786  91/06/11  15:44:46  15:44:46  dtk (Doug Kaltenecker)
 * Prettified files!
 * 
* Revision 3.785  91/05/22  14:57:17  14:57:17  stevec (Steve Claiborne)
* Beta version to MS
*
* Revision 3.780  91/05/15  15:57:27  15:57:27  stevec (Steve Claiborne)
* Beta
*
* Revision 3.775  91/04/05  14:31:20  14:31:20  stevec (Steve Claiborne)
* Beta release to MS
*
* Revision 3.770  91/03/25  15:36:18  15:36:18  stevec (Steve Claiborne)
* maintance release
*
* Revision 3.760  91/03/12  07:53:05  07:53:05  stevec (Steve Claiborne)
* Maintance release
*
* Revision 3.755  91/03/03  07:46:24  07:46:24  stevec (Steve Claiborne)
* March 3 Freeze
*
* Revision 3.720  91/02/11  09:15:41  09:15:41  stevec (Steve Claiborne)
* Aldus version
*
* Revision 3.711  91/02/08  16:26:15  16:26:15  stevec (Steve Claiborne)
* Added debuging
*
* Revision 3.710  91/02/04  15:47:58  15:47:58  stevec (Steve Claiborne)
* Aldus freeze
*
* Revision 3.700  91/01/19  09:00:39  09:00:39  stevec (Steve Claiborne)
* Release
*
* Revision 3.685  91/01/14  15:43:35  15:43:35  stevec (Steve Claiborne)
* Freeze
*
* Revision 3.681  91/01/14  10:17:51  10:17:51  stevec (Steve Claiborne)
* Updated the copy right stmt.
*
* Revision 3.680  91/01/10  16:16:57  16:16:57  stevec (Steve Claiborne)
* Freeze
*
* Revision 3.670  90/12/14  14:54:23  14:54:23  stevec (Steve Claiborne)
* freeze for 12-14-90 ver. 3.670
*
* Revision 3.665  90/12/10  15:36:06  15:36:06  stevec (Steve Claiborne)
* Freeze
*
* Revision 3.660  90/12/07  14:50:35  14:50:35  stevec (Steve Claiborne)
* Freeze 12-7-90
*
* Revision 3.650  90/11/30  08:12:29  08:12:29  stevec (Steve Claiborne)
* Freeze 3.650, 11-30-90
*
* Revision 3.601  90/08/24  11:37:50  11:37:50  daniels (Susan Daniels)
* message.txt
*
* Revision 3.600  90/08/03  11:10:02  11:10:02  stevec (Steve Claiborne)
* This is the Aug. 3 release ver. 3.600
*
* Revision 3.550  90/07/27  11:31:17  11:31:17  root ()
* Experimental freeze 3.55
*
* Revision 3.540  90/07/25  12:34:51  12:34:51  stevec (Steve Claiborne)
* Experimental freeze 3.54
*
* Revision 3.520  90/06/13  16:52:50  16:52:50  root ()
* 5_2_release
*
*
*    Rev 1.1   26 Feb 1990 13:59:56   daniels
* Updated version, changed LaserJet to LaserJet III, and added
* HP copyright statement.
*
*    Rev 1.0   06 Feb 1990 10:48:58   unknown
* Initial revision.
*
*    Rev 1.2   28 Oct 1988 15:35:22   dar
* Added copyright notice
*
*    Rev 1.1   23 Mar 1988 13:55:10   msd
* Put in prototype for strlen so this file compiles under warning level 2.
*
*    Rev 1.0   18 Feb 1988 15:11:16   steved
* Initial revision.
*
*/
#include "printer.h"
#include "dosutils.h"
#include "hppcl.h"
#include "resource.h"
#include "pfm.h"
#include "fontpriv.h"
#include "version.h"
  
int strlen(char *);
  
#define STDOUT 1
  
LOCAL void blast(s)
char *s;
{
    WORD writ;
    dos_write(STDOUT, s, strlen(s), &writ);
}
  
void main()
{
    blast("Hewlett-Packard PCL/HP LaserJet III printer driver\r\n");
    blast(VNUM);
    blast(VDATE);
    blast("\r\n");
    blast(
    "Copyright (C) Hewlett-Packard Co., 1989-1991. All rights reserved.\r\n");
    blast(
    "Copyright (C) Microsoft Corporation, 1985-1990. All rights reserved.\r\n");
    blast(
    "Copyright (C) Aldus Corporation, 1987-1989.  All rights reserved.\r\n");
    blast("\nThis file is used by Windows and is not an executable program.\r\n");
    blast("\n");
}
