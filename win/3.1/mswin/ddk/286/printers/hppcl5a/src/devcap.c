/*$Header: devcap.c,v 3.890 92/02/06 16:11:51 dtk FREEZE $*/
/**********************************************************************
* devcap.c -
*
* Copyright (C) 1989-1990 Microsoft Corporation.  All rights reserved.
* Copyright (C) 1989,1990, 1991 Hewlett-Packard Company.
*
*********************************************************************/
/*$Log:	devcap.c,v $
 * Revision 3.890  92/02/06  16:11:51  16:11:51  dtk (Doug Kaltenecker)
 * Win3.1 Freeze
 * 
 * Revision 3.873  92/01/27  11:15:31  11:15:31  daniels (Susan Daniels)
 * Bug #803:  Adding DC_PAPERNAMES.
 * 
 * Revision 3.872  91/12/17  16:25:56  16:25:56  jsmart (Jerry Smart)
 * Redifined swapp as an int.  jcs
 * 
 * Revision 3.871  91/12/17  16:16:28  16:16:28  daniels (Susan Daniels)
 * Fix Bugs #613 and 749:  fully implement DC_BINNAMES.  Also make parameter
 * to GetEnvironment dependent on WIN31 (use device rather than port).
 * 
 * Revision 3.870  91/11/08  11:43:37  11:43:37  dtk (Doug Kaltenecker)
 * 3.1 Release Candidate 1
 * 
 * Revision 3.867  91/11/08  09:21:44  09:21:44  dtk (Doug Kaltenecker)
 * Fixed erroneous comment around line 280.
 * 
 * Revision 3.866  91/11/07  14:59:18  14:59:18  jsmart (Jerry Smart)
 * Fix for MS Bug 13720.    DC_PAPERSIZE needs all paper sizes in portrait
 * order.      jsmart
 * 
 * Revision 3.865  91/11/01  13:51:36  13:51:36  dtk (Doug Kaltenecker)
 * Beta release for Windows 3.1
 * 
 * Revision 3.862  91/10/25  13:46:56  13:46:56  dtk (Doug Kaltenecker)
 * BETA
 * 
 * Revision 3.860  91/10/23  09:48:22  09:48:22  dtk (Doug Kaltenecker)
 * WinWird Release
 * 
 * Revision 3.852  91/10/09  14:59:24  14:59:24  dtk (Doug Kaltenecker)
 * BETA
 * 
 * Revision 3.850  91/10/04  16:49:36  16:49:36  dtk (Doug Kaltenecker)
 * WINWORD RELEASE CANDIDATE
 * 
 * Revision 3.840  91/09/28  14:16:55  14:16:55  dtk (Doug Kaltenecker)
 * RELEASE CANDIDATE 3 FOR WINWORD
 * 
 * Revision 3.830  91/09/18  16:33:10  16:33:10  dtk (Doug Kaltenecker)
 * RELEASE
 * 
 * Revision 3.822  91/09/16  10:33:36  10:33:36  dtk (Doug Kaltenecker)
 * WINWORD RELEASE CANDIDATE 2 FOR WIN30.
 * 
 * Revision 3.820  91/09/06  14:11:50  14:11:50  dtk (Doug Kaltenecker)
 * RELEASE CANDIDATE FOR WINWORD 2.0
 * 
 * Revision 3.812  91/08/22  14:31:46  14:31:46  dtk (Doug Kaltenecker)
 * BETA
 * 
 * Revision 3.807  91/08/08  10:31:04  10:31:04  dtk (Doug Kaltenecker)
 * PREBETA3
 * 
 * Revision 3.802  91/07/22  12:53:58  12:53:58  oakeson (Ken Oakeson)
 * BETA
 * 
 * Revision 3.800  91/07/19  12:18:52  12:18:52  daniels (Susan Daniels)
 * Bug #527: parameters passed to labdivc() in wrong order for calculating
 * paper sizes in 1/10mm.
 * 
 * Revision 3.799  91/07/02  11:51:25  11:51:25  daniels (Susan Daniels)
 * Beta
 * 
 * Revision 3.796  91/06/26  11:25:44  11:25:44  stevec (Steve Claiborne)
 * BETA
 * 
 * Revision 3.790  91/06/11  16:03:00  16:03:00  stevec (Steve Claiborne)
 * Freeze
 * 
 * Revision 3.787  91/06/11  15:43:46  15:43:46  dtk (Doug Kaltenecker)
 * Prettified files!
 * 
* Revision 3.786  91/05/28  11:02:36  11:02:36  daniels (Susan Daniels)
* Fix BUG #213:  Added DC_BINNAMES capability to DeviceCapabilities().
*
* Revision 3.780  91/05/15  15:56:49  15:56:49  stevec (Steve Claiborne)
* Beta
*
* Revision 3.775  91/04/05  14:30:42  14:30:42  stevec (Steve Claiborne)
* Beta release to MS
*
* Revision 3.770  91/03/25  15:35:42  15:35:42  stevec (Steve Claiborne)
* maintance release
*
* Revision 3.760  91/03/12  07:52:24  07:52:24  stevec (Steve Claiborne)
* Maintance release
*
* Revision 3.755  91/03/03  07:45:46  07:45:46  stevec (Steve Claiborne)
* March 3 Freeze
*
* Revision 3.720  91/02/11  09:15:05  09:15:05  stevec (Steve Claiborne)
* Aldus version
*
* Revision 3.711  91/02/08  16:24:42  16:24:42  stevec (Steve Claiborne)
* Added debuging
*
* Revision 3.710  91/02/04  15:47:24  15:47:24  stevec (Steve Claiborne)
* Aldus freeze
*
* Revision 3.700  91/01/19  09:00:06  09:00:06  stevec (Steve Claiborne)
* Release
*
* Revision 3.685  91/01/14  15:43:00  15:43:00  stevec (Steve Claiborne)
* Freeze
*
* Revision 3.681  91/01/14  10:17:17  10:17:17  stevec (Steve Claiborne)
* Updated the copy right stmt.
*
* Revision 3.680  91/01/10  16:16:22  16:16:22  stevec (Steve Claiborne)
* Freeze
*
* Revision 3.670  90/12/14  14:53:49  14:53:49  stevec (Steve Claiborne)
* freeze for 12-14-90 ver. 3.670
*
* Revision 3.665  90/12/10  15:35:32  15:35:32  stevec (Steve Claiborne)
* Freeze
*
* Revision 3.660  90/12/07  14:50:02  14:50:02  stevec (Steve Claiborne)
* Freeze 12-7-90
*
* Revision 3.650  90/11/30  08:11:56  08:11:56  stevec (Steve Claiborne)
* Freeze 3.650, 11-30-90
*
* Revision 3.601  90/08/24  11:36:57  11:36:57  daniels (Susan Daniels)
* message.txt
*
* Revision 3.600  90/08/03  11:09:27  11:09:27  stevec (Steve Claiborne)
* This is the Aug. 3 release ver. 3.600
*
* Revision 3.550  90/07/27  11:30:32  11:30:32  root ()
* Experimental freeze 3.55
*
* Revision 3.540  90/07/25  12:34:03  12:34:03  stevec (Steve Claiborne)
* Experimental freeze 3.54
*
* Revision 3.521  90/06/21  17:05:50  17:05:50  daniels ()
* Put duplexing badk in with #ifdef DUPLEX
* */
//
// 27 jan 92   SD    Added support for DC_PAPERNAMES for win31.  Also OK for 30.
// 17 dec 91   SD    BUG #613 and #749: Do full implementation of DC_BINNAMES
//                   as WIN31 common dialogs require it.
// 17 dec 91   SD    Change parameter to GetEnvironment() to device rather than
//                   port for WIN31.                       
// 17 jul 91   SD    BUG #527: In DeviceCapabilities, DC_PAPERSIZES, call to
//                   labdivc() had parameters in wrong order.
// 22 may 91   SD    BUG #213: In DeviceCapabilities, return 0 for DC_BINNAMES.
// 11 jun 90   SD    Used #ifdef DUPLEX to put back in duplexing support.
// 01 feb 90    KLO Removed duplexing support
// 07 aug 89    peterbe Changed lstrcmp() to lstrcmpi().
//   2-14-89    jimmat  Created as part of the Driver Initialization
//          changes.
//
  
#include "nocrap.h"
#undef  NOMEMMGR
#undef  NOGDI
#include "windows.h"
#include "hppcl.h"
#include "resource.h"
#include "debug.h"
#include "environ.h"
#define NO_OUTUTIL
#include "utils.h"
#define NO_PAPERBANDCRAP
#include "paperfmt.h"
#include "paper.h"
#include "strings.h"
  
/* Turns on duplexing support
*/
#define DUPLEX
  
//LPSTR   FAR PASCAL lstrcpy(LPSTR, LPSTR);
//int FAR PASCAL lstrcmpi(LPSTR, LPSTR);
  
#ifdef DEBUG
#define DBGDevCap(msg)      DBMSG(msg)
#else
#define DBGDevCap(msg)    /*null*/
#endif
  
  
extern  HANDLE hLibInst;        /* driver library instance handle */
  
#define LOCAL static

LOCAL WORD GetBinList(int, LPSTR);

#define BINSTRLEN 24
#define PAPERSTRLEN 64
  
/*  DeviceCapabilities -
*
*  This function returns the capabilities of the device driver to
*  the caller.
*/
  
DWORD FAR PASCAL
DeviceCapabilities(LPSTR lpDevice, LPSTR lpPort, WORD nIndex,
LPSTR lpOutput, LPPCLDEVMODE lpDevmode) {
  
    DWORD rc;
    LPPOINT pp;
    int i, pcap, swapp;
    short FAR *wp;
    PAPERFORMAT pf;
    PCLDEVMODE CurEnv;
    LPPCLDEVMODE lpDm;
    WORD paperBits[MAX_PAPERLIST];
#ifdef DEBUG_FUNCT
    DB(("Entering DeviceCapabilities\n"));
#endif
  
    LockSegment(-1);            /* nail down our data segment */
  
    DBGDevCap(("DeviceCapabilities(%lp,%lp,%d,%lp,%lp)\n",
    lpDevice,lpPort,nIndex,lpOutput,lpDevmode));
  
    /*  If the caller didn't pass in a DEVMODE pointer, then get/build
    *  a current one
    */
  
    if (!lpDevmode) {
        lstrcpy(CurEnv.dm.dmDeviceName, lpDevice);
        {
            int result;
#ifdef WIN31
            result = GetEnvironment(lpDevice,(LPSTR)&CurEnv,sizeof(PCLDEVMODE));
#else
            result = GetEnvironment(lpPort,(LPSTR)&CurEnv,sizeof(PCLDEVMODE));
#endif
  
        if (!result ||
            lstrcmpi(lpDevice, CurEnv.dm.dmDeviceName))
  
            MakeEnvironment(&CurEnv, lpDevice, lpPort, NULL);
        }
        lpDm = &CurEnv;         /* use current/constructed DEVMODE */
  
    } else
  
        lpDm = lpDevmode;       /* use caller's DEVMODE struct */
  
  
    /*  Return capability value(s) to caller based on requested index
    */
  
    switch (nIndex) {
  
        case DC_FIELDS:
  
            rc = lpDm->dm.dmFields;
            break;
  
  
        case DC_PAPERS:         /* return value is # supported paper */
        case DC_PAPERSIZE:      /* sizes, and (maybe) list of papers */
            /* or sizes in 10ths of a mm         */
  
            if (nIndex == DC_PAPERS) {
                wp = (short FAR *)lpOutput;
                pp = NULL;
            } else {
                pp = (LPPOINT)lpOutput;
                wp = NULL;
            }
  
            GetPaperBits(hLibInst,paperBits); /* supported papers by printer */
  
            for (i = DMPAPER_FIRST, rc = 0; i <= DMPAPER_LAST; i++)
                if (paperBits[lpDm->paperInd] & Paper2Bit(i)) {
                    rc++;
                    if (wp)     /* DC_PAPERS index */
                        *wp++ = i;
                    if (pp) {       /* DC_PAPERSIZE index */
  
                        if (GetPaperFormat(&pf,hLibInst,lpDm->paperInd,i,
                        lpDm->dm.dmOrientation)) {
                            pp->x = (int) labdivc((long)pf.xPhys,
                            (long)254,(long)HDPI);
                            pp->y = (int) labdivc((long)pf.yPhys,
                            (long)254,(long)VDPI);
                            
                /* Fix for MS Bug 13720.  PAPERFMT.H information needs to be
                 * to be corrected to portrait order for landscape paper sizes
                 * to accomodate DC_PAPERSIZE which handles all paper sizes in
                 * portrait orientation.    jcs  11/5/91    
                 */    
                            if (lpDm->dm.dmOrientation == DMORIENT_LANDSCAPE) 
                            {
                                swapp = pp->x;
                                pp->x = pp->y;
                                pp->y = swapp;
                            }
                            pp++;
                        } else
                            rc--;   /* shouldn't happen, but... */
                    }
                }
            break;

        case DC_PAPERNAMES:   /* return no. of supported papers, and maybe
                               * list of supported paper names.
                               */
            GetPaperBits(hLibInst,paperBits); /* supported papers by printer */
  
            for (i = DMPAPER_FIRST, rc = 0; i <= DMPAPER_LAST; i++)
            {
                if (paperBits[lpDm->paperInd] & Paper2Bit(i))
                {
                    rc++;
  
                    /* if requested, add name to list */
                    if (lpOutput && LoadString(hLibInst,PaperBit2Str(Paper2Bit(i)),lpOutput,PAPERSTRLEN))
                            lpOutput += PAPERSTRLEN;
                }
            }
            break;
  
        case DC_BINNAMES:            /* Added this case to fix BUG #213 */
            pcap = lpDm->prtCaps;
            rc = GetBinList(pcap, lpOutput);
            break;
  
        case DC_BINS:           /* return value is # supported paper */
                                /* bins, and (maybe) the list of bins*/
            pcap = lpDm->prtCaps;
            wp = (short FAR *)lpOutput;
  
            /* following check _very_ similar to code in MergeEnvironment() */
  
            for (i = DMBIN_FIRST, rc = 0; i <= DMBIN_LAST; i++)
                if (i == DMBIN_UPPER ||
                    ((i == DMBIN_LOWER) && (pcap & LOTRAY))    ||
                    ((i == DMBIN_MANUAL) && !(pcap & NOMAN))   ||
                    ((i == DMBIN_AUTO) && (pcap & AUTOSELECT)) ||
                ((i == DMBIN_ENVELOPE) && (pcap & ANYENVFEED))) {
                    rc++;
                    if (wp)
                        *wp++ = i;
                }
            break;
  
  
        case DC_DUPLEX:         /* return 0 for no duplex */
  
#ifdef DUPLEX
            rc = (lpDm->prtCaps & ANYDUPLEX) !=0;
#else
            rc = 0;
#endif
            break;
  
  
        case DC_SIZE:
  
            rc = lpDm->dm.dmSize;
            break;
  
  
        case DC_EXTRA:
  
            rc = lpDm->dm.dmDriverExtra;
            break;
  
  
        case DC_VERSION:
  
            rc = lpDm->dm.dmSpecVersion;
            break;
  
  
        case DC_DRIVER:
  
            rc = lpDm->dm.dmDriverVersion;
            break;
  
        default:
  
            rc = SP_ERROR;
            break;
  
    }
  
    DBGDevCap(("DeviceCapabilities() returning %ld\n",rc));
  
    UnlockSegment(-1);          /* DS now free to be... somewhere else */
  
#ifdef DEBUG_FUNCT
    DB(("Exiting DeviceCapabilities\n"));
#endif
    return(rc);
}

LOCAL WORD GetBinList(int pcap, LPSTR lpOutput)
{
  
    int   i, tray;
    short cBins;
#ifdef DEBUG_FUNCT
    DB(("Entering GetBinList\n"));
#endif
  
    DBGDevCap(("GetBinList(%lp,%lp)\n",
    lpDevmode, lpOutput));
  
  
  
    for (cBins = 0, tray = DMBIN_FIRST; tray <= DMBIN_LAST; ++tray)
    {
        i = 0;                                /* BUG #70 -- added init for i */
  
        switch (tray)
        {
  
            case DMBIN_UPPER:
                i = IDS_UPPER;
                break;
            case DMBIN_LOWER:
                if (pcap & LOTRAY)  /* BUG #70 -- Removed ';' */
                    i = IDS_LOWER;
                break;
            case DMBIN_MANUAL:
                if (!(pcap & NOMAN))
                    i = IDS_MANUAL;
                break;
            case DMBIN_ENVELOPE:
                if (pcap & ANYENVFEED)
                    i = IDS_ENVELOPE;
                break;
            case DMBIN_AUTO:
                if (pcap & AUTOSELECT)
                    i = IDS_AUTO;
                    break;               /* BUG #70 -- added break stmt */
            default:
                break;
        }
  
        if (i)
        {
  
            /*  List of bin description strings.
             */
            if (lpOutput)
            {
                LoadString(hLibInst, i, lpOutput, BINSTRLEN);
                lpOutput += BINSTRLEN;
            }
            cBins++;
        }
    } /* end for */
  
#ifdef DEBUG_FUNCT
    DB(("Exiting GetBinList\n"));
#endif
    return (cBins);                   /* BUG #60 */
}
 
