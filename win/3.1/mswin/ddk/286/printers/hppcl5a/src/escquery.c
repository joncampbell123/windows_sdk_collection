/**[f******************************************************************
* escquery.c -
*
* Copyright (C) 1988,1989 Aldus Corporation
* Copyright (C) 1988-1990 Microsoft Corporation.
* Copyright (C) 1989,1990, 1991 Hewlett-Packard Company.
*
**f]*****************************************************************/
  
/*$Header: escquery.c,v 3.890 92/02/06 16:11:41 dtk FREEZE $*/
  
/*$Log:	escquery.c,v $
 * Revision 3.890  92/02/06  16:11:41  16:11:41  dtk (Doug Kaltenecker)
 * Win3.1 Freeze
 * 
 * Revision 3.870  91/11/08  11:43:25  11:43:25  dtk (Doug Kaltenecker)
 * 3.1 Release Candidate 1
 * 
 * Revision 3.865  91/11/01  13:51:25  13:51:25  dtk (Doug Kaltenecker)
 * Beta release for Windows 3.1
 * 
 * Revision 3.863  91/10/28  14:10:17  14:10:17  dtk (Doug Kaltenecker)
 * added #ifdef WIN31 around RESETDEVICE 
 * 
 * Revision 3.862  91/10/25  13:46:45  13:46:45  dtk (Doug Kaltenecker)
 * BETA
 * 
 * Revision 3.860  91/10/23  09:48:11  09:48:11  dtk (Doug Kaltenecker)
 * WinWird Release
 * 
 * Revision 3.852  91/10/09  14:59:14  14:59:14  dtk (Doug Kaltenecker)
 * BETA
 * 
 * Revision 3.850  91/10/04  16:49:22  16:49:22  dtk (Doug Kaltenecker)
 * WINWORD RELEASE CANDIDATE
 * 
 * Revision 3.840  91/09/28  14:16:45  14:16:45  dtk (Doug Kaltenecker)
 * RELEASE CANDIDATE 3 FOR WINWORD
 * 
 * Revision 3.830  91/09/18  16:32:59  16:32:59  dtk (Doug Kaltenecker)
 * RELEASE
 * 
 * Revision 3.822  91/09/16  10:33:22  10:33:22  dtk (Doug Kaltenecker)
 * WINWORD RELEASE CANDIDATE 2 FOR WIN30.
 * 
 * Revision 3.820  91/09/06  14:11:39  14:11:39  dtk (Doug Kaltenecker)
 * RELEASE CANDIDATE FOR WINWORD 2.0
 * 
 * Revision 3.812  91/08/22  14:31:36  14:31:36  dtk (Doug Kaltenecker)
 * BETA
 * 
 * Revision 3.807  91/08/08  10:30:53  10:30:53  dtk (Doug Kaltenecker)
 * PREBETA3
 * 
 * Revision 3.802  91/07/22  12:53:46  12:53:46  oakeson (Ken Oakeson)
 * BETA
 * 
 * Revision 3.799  91/07/02  11:51:14  11:51:14  daniels (Susan Daniels)
 * Beta
 * 
 * Revision 3.796  91/06/26  11:25:32  11:25:32  stevec (Steve Claiborne)
 * BETA
 * 
 * Revision 3.790  91/06/11  16:02:48  16:02:48  stevec (Steve Claiborne)
 * Freeze
 * 
 * Revision 3.786  91/06/11  15:43:27  15:43:27  dtk (Doug Kaltenecker)
 * Prettified files!
 * 
* Revision 3.785  91/05/22  14:56:30  14:56:30  stevec (Steve Claiborne)
* Beta version to MS
*
* Revision 3.780  91/05/15  15:56:38  15:56:38  stevec (Steve Claiborne)
* Beta
*
* Revision 3.776  91/05/15  14:42:27  14:42:27  daniels (Susan Daniels)
* Fix Bug #197 (see also control.c).
*
* Revision 3.775  91/04/05  14:30:32  14:30:32  stevec (Steve Claiborne)
* Beta release to MS
*
* Revision 3.770  91/03/25  15:35:32  15:35:32  stevec (Steve Claiborne)
* maintance release
*
* Revision 3.760  91/03/12  07:52:12  07:52:12  stevec (Steve Claiborne)
* Maintance release
*
* Revision 3.755  91/03/03  07:45:36  07:45:36  stevec (Steve Claiborne)
* March 3 Freeze
*
* Revision 3.720  91/02/11  09:14:54  09:14:54  stevec (Steve Claiborne)
* Aldus version
*
* Revision 3.711  91/02/08  16:25:19  16:25:19  stevec (Steve Claiborne)
* Added debuging
*
* Revision 3.710  91/02/04  15:47:13  15:47:13  stevec (Steve Claiborne)
* Aldus freeze
*
* Revision 3.701  91/02/04  12:35:39  12:35:39  oakeson (Ken Oakeson)
* return TRUE for SETCHARSET
*
* Revision 3.700  91/01/19  08:59:55  08:59:55  stevec (Steve Claiborne)
* Release
*
* Revision 3.685  91/01/14  15:42:48  15:42:48  stevec (Steve Claiborne)
* Freeze
*
* Revision 3.681  91/01/14  10:17:07  10:17:07  stevec (Steve Claiborne)
* Updated the copy right stmt.
*
* Revision 3.680  91/01/10  16:16:12  16:16:12  stevec (Steve Claiborne)
* Freeze
*
* Revision 3.670  90/12/14  14:53:37  14:53:37  stevec (Steve Claiborne)
* freeze for 12-14-90 ver. 3.670
*
* Revision 3.665  90/12/10  15:35:20  15:35:20  stevec (Steve Claiborne)
* Freeze
*
* Revision 3.660  90/12/07  14:49:51  14:49:51  stevec (Steve Claiborne)
* Freeze 12-7-90
*
* Revision 3.650  90/11/30  08:11:45  08:11:45  stevec (Steve Claiborne)
* Freeze 3.650, 11-30-90
*
* Revision 3.603  90/10/25  17:12:51  17:12:51  oakeson (Ken Oakeson)
* Removed #ifdef from truetype code
*
* Revision 3.602  90/08/24  11:37:28  11:37:28  daniels (Susan Daniels)
* message.txt
*
* Revision 3.601  90/08/14  15:25:15  15:25:15  oakeson (Ken Oakeson)
* Avoided getting extended text metrics for TrueType fonts
*
* Revision 3.600  90/08/03  11:09:15  11:09:15  stevec (Steve Claiborne)
* This is the Aug. 3 release ver. 3.600
*
* Revision 3.550  90/07/27  11:30:14  11:30:14  root ()
* Experimental freeze 3.55
*
* Revision 3.541  90/07/27  08:15:03  08:15:03  oakeson ()
* Allow app to get kerning info from pfm OR resources
*
* Revision 3.540  90/07/25  12:33:48  12:33:48  stevec (Steve Claiborne)
* Experimental freeze 3.54
*
* Revision 3.522  90/07/21  10:39:47  10:39:47  stevec (Steve Claiborne)
* Removed PCL_* lock function calls
*
* Revision 3.521  90/06/21  17:06:24  17:06:24  daniels ()
* Put duplexing badk in with #ifdef DUPLEX
*
*/
  
/**********************************************************************
*
*  14 may 90  SD    Added RESETDEVICE:  Bug #125.
*  20 Jul 90  SJC      Removed PCL_* - don't need to lock segments.
* 11 jun 90   SD    Used #ifdef DUPLEX to add back in duplexing support.
* 01 feb 90    Ken O   Removed duplexing support
* 20 jan 90    Ken O   Removed non-Galaxy support
* 19 sep 89    peterbe Moved LJ IIP code here from include file.
*          See comments for values returned!
* 27 apr 89    peterbe Tabs are 8 spaces
*   1-17-89    jimmat  Added PCL_* entry points to lock/unlock data seg.
*   2-06-89    jimmat  Was returning FALSE for ENABLEDUPLEX in HP IID.
*/
  
#include "generic.h"
#include "resource.h"
#include "extescs.h"
#include "truetype.h"
  
/* Turns on duplexing support
*/
#define DUPLEX
  
int far PASCAL Control_II(LPDEVICE, short, LPSTR, LPPOINT);
  
int   FAR PASCAL Control(LPDEVICE,short,LPSTR,LPPOINT);
  
  
/*  Control
*
*  Windows Escape() function QUERYESCSUPPORT.  The function is organized
*  so the bulk of the work is done in control.c, this file supports only
*  QUERYESCSUPPORT and some small speedups.
*/
int far PASCAL Control(lpDevice, function, lpInData, lpOutData)
LPDEVICE lpDevice;
short function;
LPSTR lpInData;
LPPOINT lpOutData;
{
#ifdef DEBUG_FUNCT
    DB(("Entering Control\n"));
#endif
    if (function == QUERYESCSUPPORT)
    {
        short i = *(short far *)lpInData;
  
        DBMSG(("QUERYESCSUPPORT(%d)\n", i));
  
        switch (i)
        {
            case NEWFRAME:
            case ABORTDOC:
            case NEXTBAND:
            case STARTDOC:
            case ENDDOC:
            case SETABORTPROC:
            case QUERYESCSUPPORT:
            case DRAFTMODE:
            case GETPHYSPAGESIZE:
            case GETPRINTINGOFFSET:
            case GETSCALINGFACTOR:
            case SETCOPYCOUNT:
            case BANDINFO:
            case DEVICEDATA:
            case SETALLJUSTVALUES:
            case GETEXTENDEDTEXTMETRICS:
            case GETPAIRKERNTABLE:
            case GETTRACKKERNTABLE:
            case GETSETPRINTORIENT:
            case GETSETPAPERBINS:
            case ENUMPAPERBINS:
            case GETTECHNOLOGY:
            case SETCHARSET:
#ifdef WIN31
            case RESETDEVICE:                /* Implement RESETDEVICE: Bug #125 */
#endif
                return TRUE;
  
            case DRAWPATTERNRECT:
  
                // We return 0, 1 or 2 here.
                //  0   means NO rules supported
                //  1   means rules supported on all but basic LJ
                //  2   means 'white rules' on IIP/III supported also.
  
                /* TETRA -- removed non-Galaxy support */
#ifdef VISUALEDGE
                //  LaserPort is not currently supporting rules
                if (lpDevice->epOptions & OPTIONS_DPTEKCARD)
                    return 0;
#endif
  
                // Report extended DRAWPATTERNRECT (white rules)
                return 2;
  
            case ENABLEDUPLEX:
#ifdef DUPLEX
                if (lpDevice->epCaps & ANYDUPLEX)
                    return TRUE; /*Printer can print duplex */
                else
#endif
                    return FALSE;
  
            default:
                return FALSE;
        }
    }
    else
    {
        if (function == GETEXTENDEDTEXTMETRICS)
        {
            LPFONTINFO lpFont = ((LPEXTTEXTDATA)lpInData)->lpFont;
  
            /* speedup for truetype fonts (which do not kern currently)
            */
            if (lpFont->dfType & TYPE_TRUETYPE)
                return FALSE;
        }
  
        return (Control_II(lpDevice,function,lpInData,lpOutData));
    }
#ifdef DEBUG_FUNCT
    DB(("Exiting Control\n"));
#endif
}
