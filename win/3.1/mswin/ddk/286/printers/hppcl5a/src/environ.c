/*$Header: environ.c,v 3.890 92/02/06 16:11:38 dtk FREEZE $*/
/**[f******************************************************************
* environ.c -
*
* Copyright (C) 1988,1989 Aldus Corporation
* Copyright (C) 1988-1990 Microsoft Corporation.
* Copyright (C) 1989,1990, 1991 Hewlett-Packard Company.
* All rights reserved.  Company confidential.
*
**f]*****************************************************************/
/***************************************************************************/
/*$Log:	environ.c,v $
 * Revision 3.890  92/02/06  16:11:38  16:11:38  dtk (Doug Kaltenecker)
 * Win3.1 Freeze
 * 
 * Revision 3.872  91/12/03  15:20:54  15:20:54  daniels (Susan Daniels)
 * Fix bug #747:  Some apps passing in driver name rather than printer name.
 * Modified MakeEnvironment() to verify lpDeviceName.
 * 
 * Revision 3.871  91/11/16  14:54:51  14:54:51  daniels (Susan Daniels)
 * Adding capability to handle printer aliases (custom names) in Win31.
 * 
 * Revision 3.870  91/11/08  11:43:22  11:43:22  dtk (Doug Kaltenecker)
 * 3.1 Release Candidate 1
 * 
 * Revision 3.865  91/11/01  13:51:21  13:51:21  dtk (Doug Kaltenecker)
 * Beta release for Windows 3.1
 * 
 * Revision 3.862  91/10/25  13:46:42  13:46:42  dtk (Doug Kaltenecker)
 * BETA
 * 
 * Revision 3.860  91/10/23  09:48:07  09:48:07  dtk (Doug Kaltenecker)
 * WinWird Release
 * 
 * Revision 3.852  91/10/09  14:59:10  14:59:10  dtk (Doug Kaltenecker)
 * BETA
 * 
 * Revision 3.850  91/10/04  16:49:17  16:49:17  dtk (Doug Kaltenecker)
 * WINWORD RELEASE CANDIDATE
 * 
 * Revision 3.840  91/09/28  14:16:41  14:16:41  dtk (Doug Kaltenecker)
 * RELEASE CANDIDATE 3 FOR WINWORD
 * 
 * Revision 3.830  91/09/18  16:32:56  16:32:56  dtk (Doug Kaltenecker)
 * RELEASE
 * 
 * Revision 3.822  91/09/16  10:33:16  10:33:16  dtk (Doug Kaltenecker)
 * WINWORD RELEASE CANDIDATE 2 FOR WIN30.
 * 
 * Revision 3.820  91/09/06  14:11:35  14:11:35  dtk (Doug Kaltenecker)
 * RELEASE CANDIDATE FOR WINWORD 2.0
 * 
 * Revision 3.814  91/09/06  10:44:06  10:44:06  daniels (Susan Daniels)
 * Use compiler options to accommodate WIN31 changes to DEVMODE, e.g.
 * no more dmDriverData field, so need to correct sizeof calculations.
 * 
 * Revision 3.813  91/09/04  13:27:39  13:27:39  daniels (Susan Daniels)
 * Fix Bugs #619 and #620.  These also fixed #580.  Setting defaults to LJIII.
 * 
 * Revision 3.812  91/08/22  14:31:32  14:31:32  dtk (Doug Kaltenecker)
 * BETA
 * 
 * Revision 3.807  91/08/08  10:30:49  10:30:49  dtk (Doug Kaltenecker)
 * PREBETA3
 * 
 * Revision 3.802  91/07/22  12:53:42  12:53:42  oakeson (Ken Oakeson)
 * BETA
 * 
 * Revision 3.799  91/07/02  11:51:10  11:51:10  daniels (Susan Daniels)
 * Beta
 * 
 * Revision 3.798  91/07/02  09:20:31  09:20:31  dtk (Doug Kaltenecker)
 * Added win.ini handeling for TT as graphics
 * 
 * Revision 3.797  91/07/01  11:21:49  11:21:49  daniels (Susan Daniels)
 * Changes for DevInstall() DDI.
 * 
 * Revision 3.796  91/06/26  11:25:28  11:25:28  stevec (Steve Claiborne)
 * BETA
 * 
 * Revision 3.792  91/06/25  17:14:52  17:14:52  daniels (Susan Daniels)
 * Added compiler options for Win31 or Win30. Also some fixes.
 * 
 * Revision 3.791  91/06/17  17:18:29  17:18:29  daniels (Susan Daniels)
 * Fix BUG # 516:  Removing list of printers from printer Setup.  See also
 * devmode.c and hppcl.rc..
 * 
 * Revision 3.790  91/06/11  16:02:45  16:02:45  stevec (Steve Claiborne)
 * Freeze
 * 
 * Revision 3.786  91/06/11  15:43:23  15:43:23  dtk (Doug Kaltenecker)
 * Prettified files!
 * 
* Revision 3.785  91/05/22  14:56:27  14:56:27  stevec (Steve Claiborne)
* Beta version to MS
*
* Revision 3.781  91/05/22  09:47:16  09:47:16  daniels (Susan Daniels)
* Fix BUG #215: dmDriverExtra field set wrong in DEVMODE.
*
* Revision 3.780  91/05/15  15:56:35  15:56:35  stevec (Steve Claiborne)
* Beta
*
* Revision 3.777  91/04/12  14:39:54  14:39:54  daniels (Susan Daniels)
* Fix BUG #131:  Duplex mode not kept between windows sessions.
* Correct fix made for BUG #145 in previous 3.776
*
* Revision 3.776  91/04/10  09:54:34  09:54:34  daniels (Susan Daniels)
* Fixing Bug #145:  Tray selection not retained between Windows sessions.
*
* Revision 3.770  91/03/25  15:35:28  15:35:28  stevec (Steve Claiborne)
* maintance release
*
* Revision 3.760  91/03/12  07:52:09  07:52:09  stevec (Steve Claiborne)
* Maintance release
*
* Revision 3.755  91/03/03  07:45:33  07:45:33  stevec (Steve Claiborne)
* March 3 Freeze
*
* Revision 3.724  91/02/28  10:41:02  10:41:02  daniels (Susan Daniels)
* Fix BUG #139:  trashed paperind= value in win.ini caused grief.
*
* Revision 3.723  91/02/26  11:12:15  11:12:15  daniels (Susan Daniels)
* Fix for BUG #141:  dmSize field set wrong in DefaultEnvironment().
*
* Revision 3.722  91/02/19  08:23:28  08:23:28  daniels (Susan Daniels)
* Fixing oversight for adding ELI.
*
* Revision 3.721  91/02/11  19:12:07  19:12:07  daniels (Susan Daniels)
* Adding ELI
*
* Revision 3.720  91/02/11  09:14:50  09:14:50  stevec (Steve Claiborne)
* Aldus version
*
* Revision 3.711  91/02/08  16:25:12  16:25:12  stevec (Steve Claiborne)
* Added debuging
*
* Revision 3.710  91/02/04  15:47:11  15:47:11  stevec (Steve Claiborne)
* Aldus freeze
*
* Revision 3.700  91/01/19  08:59:52  08:59:52  stevec (Steve Claiborne)
* Release
*
* Revision 3.685  91/01/14  15:42:45  15:42:45  stevec (Steve Claiborne)
* Freeze
*
* Revision 3.681  91/01/14  10:17:03  10:17:03  stevec (Steve Claiborne)
* Updated the copy right stmt.
*
* Revision 3.680  91/01/10  16:16:09  16:16:09  stevec (Steve Claiborne)
* Freeze
*
* Revision 3.670  90/12/14  14:53:34  14:53:34  stevec (Steve Claiborne)
* freeze for 12-14-90 ver. 3.670
*
* Revision 3.665  90/12/10  15:35:18  15:35:18  stevec (Steve Claiborne)
* Freeze
*
* Revision 3.660  90/12/07  14:49:48  14:49:48  stevec (Steve Claiborne)
* Freeze 12-7-90
*
* Revision 3.650  90/11/30  08:11:43  08:11:43  stevec (Steve Claiborne)
* Freeze 3.650, 11-30-90
*
* Revision 3.604  90/11/19  08:51:31  08:51:31  tshannon (Terry Shannon)
* Tuned gray scaling for HP Laserjets.  Also added lighten gray scale
* button.  Terry Shannon 11-19-90
*
* Revision 3.603  90/11/09  12:05:00  12:05:00  daniels (Susan Daniels)
* Fixed Bug #71.
*
* Revision 3.602  90/08/28  09:46:52  09:46:52  daniels (Susan Daniels)
* Fixed bug #24:  Set DM_DEFAULTSOURCE bit in dmFields.
*
* Revision 3.601  90/08/24  11:37:26  11:37:26  daniels ()
* message.txt
*
* Revision 3.600  90/08/03  11:09:12  11:
*
*    Rev 1.3   20 Feb 1990 15:45:24   vordaz
* Support for downloadables.
*
*    Rev 1.1   09 Feb 1990 16:06:24   daniels
* Added parsing for caps2 field to GetPrtItem().
* Added code to update lpDevmode->prtCaps2 in MakeEnvironment() in
* two places.
*/
/******************************   Environ.c   ******************************/
//
//  Environ:    Procs for reading/creating devmode
//
//  03 dec 91  SD     Add code for case when app passes in driver name rather
//                    than printer name.
//  16 nov 91  SD     Add printer aliasing for Win31.
//  03 sep 91  SD     BUG #620:  Fix SetIndex() so it doesn't run past the
//                    last printer in the list.
//  03 sep 91  SD     Bug #619:  Set default printer values to LJIII.
//  29 jun 91  SD     Added code to GetWinIniEnv() for Win 3.1 to read font
//                    info from [driver,port], and other info from [device,
//                    port].
//  25 jun 91  SD     Use Compiler options for Window 3.1 build.
//  25 jun 91  SD     Added code to validate dmFields to ModifyEnvironment()
//                    for Common Dialogs.  Code also OK for Win 30.
//  13 jun 91  SD     Added type conversions to GetWinIniEnv() for the output
//                    bin and job offset cases to remove compiler warnings. 
//  13 jun 91  SD     BUG #516:  Removing list of printers from Printers Setup.  
//  22 may 91  SD     BUG #215: Correct calculation of dmDriverExtra in DEVMODE.
//  28 feb 91  SD     BUG #139: In MakeEnvironment(), get paper index from
//                    printer string after call to GetPrtItems().
//  26 feb 91  SD     BUG #141: In DefaultEnvironment() fixed calculation of
//                    dmSize.
//  11 feb 91  SD     Added ELI support.
//  09 nov 90  SD     Bug #71:  This was fixed 10/2/90, but somehow didn't
//                    get archived.  Set dm.dmPrintQuality to DMRES_HIGH
//                    in DefaultEnvironment().
//  28 aug 90  SD     Bug #24:  Added code to DefaultEnvironment() to set
//                    the DM_DEFAULTSOURCE bit in dmFields.  Corrected
//                    code in MakeEnvironment to set dmDefaultSource.
//  27 jul 90  SD     Set lpDevmode->grayscale in DefaultEnvironment().
//                    Added parsing for WININI_GRAYSCALE to GetWinIniEnv();
//  11 jul 90  SD     Added parsing for WININI_PGPROTECT to GetWinIniEnv();
//                    Part of fix for Bug #11.
//  09 jul 90  SD     Added envelope support; removed unsupported papers.
//  11 jun 90  SD     Used #ifdef DUPLEX to put back in duplexing support.
//  01 feb 90  KLO  removed duplexing support
//  09 jan 90  clarkc   duplex setting from win.ini no longer overruled.
//  09 oct 89   peterbe isUSA() algorithm changed.  TRUE for all countries in
//          the Americas.
//  29 sep 89   peterbe isUSA() returns TRUE now for default country (0) and
//          most countries in the Americas.  Also, now call isUSA()
//          in GetWinIniEnv().
//  28 sep 89   craigc  fixed cartridge not being selected.
//  03 aug 89   peterbe Commented out NO_PRIVATE_HACK stuff -- remove later.
//   2-06-89    jimmat  Changes related to externalizing HP cartridge info.
//   2-07-89    jimmat  Driver Initialization changes.
//   2-20-89    jimmat  Driver/Font Installer use same WIN.INI section (again)
//
//#define DEBUG
  
#include "generic.h"
#include "resource.h"
#include "strings.h"
#define PRTCARTITEMS
#include "environ.h"
#include "utils.h"
#include "version.h"
#include "lclstr.h"
#include "country.h"
#include "build.h"
  
LPSTR   FAR PASCAL lstrcpy(LPSTR, LPSTR);
LPSTR   FAR PASCAL lstrcat( LPSTR, LPSTR );
int FAR PASCAL lstrlen( LPSTR );
  
/* Turns on duplexing capabilities
*/
#define DUPLEX
  
/*  Utilities
*/
#include "getint.c"
  
#define LOCAL static
  
  
#ifdef DEBUG
#define LOCAL_DEBUG
#endif
  
#ifdef LOCAL_DEBUG
#define DBGentry(msg) DBMSG(msg)
#define DBGEnv(msg) DBMSG(msg)
#else
#define DBGentry(msg) /*null*/
#define DBGEnv(msg)   /*null*/
#endif
  
  
extern HANDLE hLibInst;     /* driver's instance handel */
  
/*  Forward refs
*/
LOCAL void DefaultEnvironment(LPPCLDEVMODE, LPSTR, HANDLE);
LOCAL void GetWinIniEnv(LPPCLDEVMODE, LPSTR, LPSTR, HANDLE);
LOCAL BOOL isUSA(void);

#if defined(WIN31)
LOCAL void ParseDevName(LPSTR, LPSTR);
LOCAL void SetIndex(LPPCLDEVMODE, LPSTR);
LOCAL BOOL isours(LPSTR); 
#endif   
  
#ifdef LOCAL_DEBUG
LOCAL void
dumpDevMode(LPPCLDEVMODE lpEnv) {
    /*
    DBMSG(("   dmDeviceName: %ls\n",(LPSTR)lpEnv->dm.dmDeviceName));
    DBMSG(("   dmSpecVersion: %4xh\n",lpEnv->dm.dmSpecVersion));
    DBMSG(("   dmDriverVersion: %4xh\n",lpEnv->dm.dmDriverVersion));
    DBMSG(("   dmSize: %d\n",lpEnv->dm.dmSize));
    DBMSG(("   dmDriverExtra: %d\n",lpEnv->dm.dmDriverExtra));
    DBMSG(("   dmFields: %8lxh\n",lpEnv->dm.dmFields));
    DBMSG(("   dmOrientation: %d\n",lpEnv->dm.dmOrientation));
    DBMSG(("   dmPaperSize: %d\n",lpEnv->dm.dmPaperSize));
#ifdef FLARP
    DBMSG(("   dmPaperLength: %d\n",lpEnv->dm.dmPaperLength));
    DBMSG(("   dmPaperWidth: %d\n",lpEnv->dm.dmPaperWidth));
    DBMSG(("   dmXMargin: %d\n",lpEnv->dm.dmXMargin));
    DBMSG(("   dmYMargin: %d\n",lpEnv->dm.dmYMargin));
    DBMSG(("   dmLogicalLength: %d\n",lpEnv->dm.dmLogicalLength));
    DBMSG(("   dmLogicalWidth: %d\n",lpEnv->dm.dmLogicalWidth));
#endif
    DBMSG(("   dmCopies: %d\n",lpEnv->dm.dmCopies));
    DBMSG(("   dmDefaultSource: %d\n",lpEnv->dm.dmDefaultSource));
    DBMSG(("   dmPrintQuality: %d\n",lpEnv->dm.dmPrintQuality));
    DBMSG(("   dmColor: %d\n",lpEnv->dm.dmColor));
    DBMSG(("   dmDuplex: %d\n",lpEnv->dm.dmDuplex));
    DBMSG(("   prtIndex: %d\n",lpEnv->prtIndex));
    DBMSG(("   paperInd: %d\n",lpEnv->paperInd));
    */
    DBMSG(("   reartray: %d\n",lpEnv->reartray));
    DBMSG(("   offset: %d\n",lpEnv->offset));
}
#endif
  
  
/***************************************************************************/
/**************************   Global Procedures   **************************/
  
  
/*  MakeEnvironment
*
*  Initialize the devmode data structure.  First try to get info from
*  win.ini -- if that fails, use information from first printer and
*  first cartridge listed in resources -- if that fails, use constants.
*/
void FAR PASCAL
MakeEnvironment(lpDevmode, lpDeviceName, lpPortName, lpProfile)
LPPCLDEVMODE lpDevmode;
LPSTR lpDeviceName;
LPSTR lpPortName;
LPSTR lpProfile;
{
    PRTINFO prtInfo;
#if defined(CARTS_IN_RESOURCE)  /*--------------------------------------*/
    CARTINFO cartInfo;
#endif  /*-- defined(CARTS_IN_RESOURCE) --------------------------------*/

#if defined(WIN31)
    char tempbuf[STR_LEN];     /* temp storage for string of printer info */
    char devname[STR_LEN];     /* string to hold device name */
    char realname[CCHDEVICENAME]; /* string to hold the real printer name */
    int length;
#endif

#ifdef DEBUG_FUNCT
    DB(("Entering MakeEnvironment\n"));
#endif
    DBGEnv(("MakeEnvironment(%lp,%lp,%lp,%lp)\n",
    lpDevmode, lpDeviceName, lpPortName, lpProfile));
    DBGEnv(("   %ls on %ls\n", lpDeviceName, lpPortName));

#if defined(WIN31)  
     /*  MS added support for printer aliasing.  The string in lpDeviceName
     *  may be an application assigned custom name for the printer.  The
     *  custom name is used everywhere, except for accessing the list of
     *  printers in the resource file.  The true printer name can be
     *  found in the win.ini file in the section [PrinterAliases] by using
     *  the DeviceName as the key.  If there is no entry, there is no alias
     *  and the DeviceName is used for everything.
     */
    
    /*  Some apps like PPS Windows Works and PerFORM PRO pass in the driver
     *  name instead of the printer name in lpDeviceName.  Need to verify
     *  that the name is indeed one of our printer names, or an alias for
     *  one. If the name is not, then get the Windows default printer from
     *  the win.ini and use that.  If it is not one of our printers, use
     * the driver default.         (BUG # 747)
     */
    
    /* Check for printer aliasing */
    length = GetProfileString("PrinterAliases",lpDeviceName,"",
                               (LPSTR) realname, CCHDEVICENAME);
    if (length == 0)
        lstrncpy((LPSTR) realname, lpDeviceName, CCHDEVICENAME);
    
    if (!isours((LPSTR)realname))
    {
        /* realname not one of our printer names.  App probably passed
         * in the driver name, so get the  Windows default printer from
         * the win.ini file, and see if that is one of our printers.  If
         * so, then use it, if not then use the driver's default printer.
         */
        int i;

        length = GetProfileString("windows", "device", "", (LPSTR)tempbuf, STR_LEN);
        for (i = 0; (tempbuf[i] != ',') && (i < length); i++)
        {
            devname[i] = tempbuf[i];
        }
        devname[i] = '\0';
       /* Check for printer aliasing. If one is found, check and see if it
        * is one of our printers.
        */
        length = GetProfileString("PrinterAliases", (LPSTR)devname,"",
                               (LPSTR) tempbuf, STR_LEN);
        if (length == 0)
            lstrcpy((LPSTR)tempbuf, (LPSTR)devname);
        if (isours((LPSTR)tempbuf))
        {
            lstrcpy((LPSTR)realname, (LPSTR)tempbuf);
            lstrcpy(lpDeviceName,(LPSTR)devname);
        }
        else
        {
             LoadString(hLibInst, DEVNAME_BASE + 0,
                   (LPSTR)tempbuf, STR_LEN);
             ParseDevName((LPSTR)tempbuf, (LPSTR)realname);
             lstrcpy(lpDeviceName,(LPSTR)realname);
        }
    }
#endif
 
    /*  Start off with defaults
    */
    DefaultEnvironment(lpDevmode, lpDeviceName, hLibInst);
#ifdef DEBUG
    DBMSG(("After DefaultEnvironment()\n"));
    dumpDevMode((lpDevmode));
#endif
    /*  Read what we can from win.ini
    */
    GetWinIniEnv(lpDevmode, lpPortName, lpProfile, hLibInst);
#ifdef DEBUG
    DBMSG(("After GetWinIniEnv\n"));
    dumpDevMode((lpDevmode));
#endif



#if defined(WIN31)
    /* BUG #516:  Check to see if the printer index read from the win.ini is for the
     * same printer name as lpDeviceName.  If it can't be read from the
     * resources, or if it is different, use the printer index for the
     * first printer listed with lpDeviceName.
     */

     if (LoadString(hLibInst, DEVNAME_BASE + lpDevmode->prtIndex,
                   (LPSTR)tempbuf, STR_LEN))
    {
        ParseDevName((LPSTR)tempbuf, (LPSTR)devname);
        if (lstrcmpi((LPSTR)realname, (LPSTR)devname))   /* lstrcmpi = 0 if the strings are identical */
            SetIndex(lpDevmode, realname);
    }
    else
    {
        SetIndex(lpDevmode, realname);
    }
#endif
  
    /*  Read printer and cartridge information from the resources.
    */
    if (GetPrtItem((PRTINFO FAR*)&prtInfo, lpDevmode->prtIndex, hLibInst))
    {
        /*  Info read, fill in devmode.
        */
        short ind;
  
        lpDevmode->availmem = prtInfo.availmem;
        lpDevmode->romind = prtInfo.romind;
        lpDevmode->romcount = prtInfo.romcount;
        lpDevmode->prtCaps= prtInfo.caps;
        lpDevmode->prtCaps2= prtInfo.caps2;          /*Tetra II*/
        lpDevmode->maxPgSoft = prtInfo.maxpgsoft;
        lpDevmode->maxSoft = prtInfo.maxsoft;
        lpDevmode->paperInd = prtInfo.indpaperlist;  /* BUG #139 */
        if (lpDevmode->numCartridges > prtInfo.numcart)
            lpDevmode->numCartridges = prtInfo.numcart;
        /* Begin BUG #145:  Put validity check for dmDefaultSource here.  This is a
        * correction of the code that was moved into the else "Use default" block.
        */
        if ((lpDevmode->prtCaps & (AUTOSELECT|LOTRAY|ANYENVFEED|NOMAN)) != NOMAN)
        {
            lpDevmode->dm.dmFields |= DM_DEFAULTSOURCE;
            if (!(lpDevmode->prtCaps & LOTRAY))
                lpDevmode->dm.dmDefaultSource = DMBIN_UPPER;
        }
        else
        {
            lpDevmode->dm.dmFields &= ~DM_DEFAULTSOURCE;
            lpDevmode->dm.dmDefaultSource = DMBIN_UPPER;
        }
  
        /* Begin BUG #131:  Put validity check for duplex mode here. */
        /* Added verification of dmFields for Common Dialogs. Also OK for Win 30!*/
        if (!(lpDevmode->prtCaps & ANYDUPLEX))
        {
            lpDevmode->dm.dmDuplex = DMDUP_SIMPLEX;
            lpDevmode->dm.dmFields &= ~DM_DUPLEX;
        }
        else
            lpDevmode->dm.dmFields |= DM_DUPLEX;


        /*  Get cartridge info for each cartridge selected.
        */
  
#if defined(CARTS_IN_RESOURCE)  /*--------------------------------------*/
  
        for (ind = 0;
            (ind < DEVMODE_MAXCART) && (ind < lpDevmode->numCartridges) &&
            GetCartItem(&cartInfo, lpDevmode->cartIndex[ind], hLibInst);
            ++ind)
        {
            lpDevmode->cartind[ind] = cartInfo.cartind;
            lpDevmode->cartcount[ind] = cartInfo.cartcount;
        }
  
#else   /*-- defined(CARTS_IN_RESOURCE) --------------------------------*/
  
        /* Now that cartridges are externalized, cartIndex[ind] can
        only be > 0 if the user edited his win.ini file by hand,
        which he/she had best not do.  At some point in time, we
        should go through the code and make cartridge indexes
        >= 0 again--now that there is only one type (external),
        there is no need to have indexes above and below 0. */
  
        /* craigc-sort of did this.  cartridges now back in but in the
        * more convenient form of resource PCM's.
        */
  
        for (ind = 0; (ind < DEVMODE_MAXCART) &&
            (ind < lpDevmode->numCartridges); ++ind)
        {
            lpDevmode->cartind[ind] = 1;
            lpDevmode->cartcount[ind] = 0;
        }
  
#endif  /*-- defined(CARTS_IN_RESOURCE) --------------------------------*/
  
        lpDevmode->numCartridges = ind; 
  
        DBMSG(("environ: numcart=%d\n",ind));
    }
    else
    {
        /*  Use defaults.
        */
        DefaultEnvironment(lpDevmode, lpDeviceName, hLibInst);
        lstrcpy((LPSTR)lpDevmode->dm.dmDeviceName, "HP LaserJet III");   /* BUG #619 */
        lpDevmode->availmem = JETMEM;
        lpDevmode->romind = 0;
        lpDevmode->romcount = 15;                    /* BUG #619 */
        /* Note:  Code that deals with listing the cartridges expects the
         * value in numCartridges to be > 0.
         */
        lpDevmode->numCartridges = 1;
        lpDevmode->prtCaps= JETCAPS;
        lpDevmode->prtCaps2= JETCAPS2;                      /*Tetra II*/
        lpDevmode->maxPgSoft = MAXPGSOFT;             /* BUG #619 */
        lpDevmode->maxSoft = MAXSOFT;                 /* BUG #619 */
        lpDevmode->paperInd = 0;                      /*BUG #139*/
  
        /* Begin BUG #145:  Moved setting dmFields and dmDefault source
        * from outside the else use defaults block to inside it.
        */
        if ((lpDevmode->prtCaps & (AUTOSELECT|LOTRAY|ANYENVFEED|NOMAN)) != NOMAN)
        {
            lpDevmode->dm.dmFields |= DM_DEFAULTSOURCE;
            lpDevmode->dm.dmDefaultSource = DMBIN_UPPER;
        }
        else
            lpDevmode->dm.dmFields &= ~DM_DEFAULTSOURCE;
  
    }
  
    /*  Force loading of soft fonts if the user requested it.
    */
    if (lpDevmode->options & OPTIONS_FORCESOFT)
        lpDevmode->prtCaps &= ~(NOSOFT);
  
    /*  Make adjustments to DEVMODE based on printer capabilities --
    NOTE: very similar code exists in devmode.c, if you update this
    you may need to update that also */
    /* BUG #131:  Don't reset duplex mode here! */
  
    //#ifdef DUPLEX
    //    if (lpDevmode->prtCaps & ANYDUPLEX) {
    //  lpDevmode->dm.dmFields |= DM_DUPLEX;
    //  lpDevmode->dm.dmDuplex = DMDUP_SIMPLEX;
    //    } else
    //  lpDevmode->dm.dmDuplex = 0;
    //#else
    //    lpDevmode->dm.dmDuplex = DMDUP_SIMPLEX;
    //#endif
  
#ifdef LOCAL_DEBUG
    DBMSG(("MakeEnvironment returning:\n"));
    dumpDevMode(lpDevmode);
#endif
#ifdef DEBUG_FUNCT
    DB(("Exiting MakeEnvironment\n"));
#endif
}
  
  
/*  GetPrtItem
*
*  Get the printer information for one string in the resource file.
*/
BOOL FAR PASCAL GetPrtItem(lpPrtInfo, ind, hModule)
PRTINFO FAR *lpPrtInfo;
short ind;
HANDLE hModule;
{
    char tempbuf[STR_LEN];
    LPSTR infoptr;
    LPSTR bufptr;
    short i;
    char sepchar;
#ifdef DEBUG_FUNCT
    DB(("Entering GetPrtItem\n"));
#endif
  
    //    DBGentry(("GetPrtItem(%lp,%d,%d)\n", lpPrtInfo, ind, (HANDLE)hModule));
  
    if (!LoadString(hModule, DEVNAME_BASE+ind, (LPSTR)tempbuf, STR_LEN))
        return FALSE;
  
    bufptr = (LPSTR)tempbuf;
  
    /*  separator character is the first char in string
    */
    sepchar = *bufptr++;
  
    /*  parse string to get devname
    */
    for (i = 0, infoptr = (LPSTR)lpPrtInfo->devname;
        *bufptr && (*bufptr != sepchar); i++)
    {
        if (i < DEV_NAME_LEN - 1)
            *infoptr++ = *bufptr;
        ++bufptr;
    }
    *infoptr = '\0';
  
    /*  parse for other values
    */
    if (*bufptr && (i > 0))
    {
        /*  increment after each field to move past sepchar
        */
        bufptr++;
        lpPrtInfo->availmem = GetInt((LPSTR FAR *)&bufptr, sepchar);
  
        if (*bufptr && (*bufptr == sepchar))
        {
            bufptr++;
            for (infoptr = (LPSTR)lpPrtInfo->realmem;
                *bufptr && (*bufptr != sepchar);
                *infoptr++ = *bufptr++)
                ;
            *infoptr='\0';
  
            if (*bufptr && (*bufptr == sepchar))
            {
                bufptr++;
                lpPrtInfo->caps = GetInt((LPSTR FAR *)&bufptr, sepchar);
                bufptr++;
                lpPrtInfo->caps2 = GetInt((LPSTR FAR *)&bufptr, sepchar); /*Tetra II*/
                bufptr++;
                lpPrtInfo->romind=GetInt((LPSTR FAR *)&bufptr,sepchar);
                bufptr++;
                lpPrtInfo->romcount=GetInt((LPSTR FAR *)&bufptr,sepchar);
                bufptr++;
                lpPrtInfo->maxpgsoft=GetInt((LPSTR FAR *)&bufptr,sepchar);
                bufptr++;
                lpPrtInfo->maxsoft=GetInt((LPSTR FAR *)&bufptr,sepchar);
                bufptr++;
                lpPrtInfo->numcart=GetInt((LPSTR FAR *)&bufptr,sepchar);
                bufptr++;
                lpPrtInfo->indpaperlist=GetInt((LPSTR FAR *)&bufptr,sepchar);
#ifdef LOCAL_DEBUG
                //               DBMSG(("GetPrtItem returning index to paper list = %d\n",
                //                                         lpPrtInfo->indpaperlist));
#endif
                return TRUE;
            }
        }
    }
  
#ifdef DEBUG_FUNCT
    DB(("Exiting GetPrtItem\n"));
#endif
    return FALSE;
}
  
  
#if defined(CARTS_IN_RESOURCE)  /*--------------------------------------*/
  
/*  GetCartItem
*
*  Get the cartridge information for one string in the resource file.
*/
BOOL FAR PASCAL GetCartItem(lpCartInfo, ind, hModule)
CARTINFO FAR *lpCartInfo;
short ind;
HANDLE hModule;
{
    char tempbuf[STR_LEN];
    LPSTR infoptr;
    LPSTR bufptr;
    short i;
    char sepchar;
#ifdef DEBUG_FUNCT
    DB(("Entering GetCartItem\n"));
#endif
  
    DBGentry(("GetCartItem(%lp,%d,%d)\n",
    lpCartInfo, ind, (HANDLE)hModule));
  
    /* if cartridge comes from a PCM, there's no resource information,
    * but we'll assume the cartridge is ok ...craigc
    */
    if (ind<0)
    {
        lpCartInfo->cartind=1;
        lpCartInfo->cartcount=0;
        return TRUE;
    }
  
    if (!LoadString(hModule, CART_BASE+ind, (LPSTR)tempbuf, STR_LEN))
        return FALSE;
  
    bufptr = (LPSTR)tempbuf;
  
    /*  get separator character which is first char in string
    */
    sepchar = *bufptr++;
  
    /*  parse string to get devname
    */
    for (i = 0, infoptr = (LPSTR)lpCartInfo->cartname;
        *bufptr && (*bufptr != sepchar); i++)
    {
        if (i < DEV_NAME_LEN - 1)
            *infoptr++ = *bufptr;
        ++bufptr;
    }
    *infoptr='\0';
  
    /*  parse for other values
    */
    if (*bufptr && (i > 0))
    {
        /*  increment past sepchar and extract values
        */
        bufptr++;
        lpCartInfo->cartind = GetInt((LPSTR FAR *)&bufptr, sepchar);
        bufptr++;
        lpCartInfo->cartcount = GetInt((LPSTR FAR *)&bufptr, sepchar);
  
        #if 0
        dumpCartInfo(lpCartInfo);
        #endif
  
        return TRUE;
    }
  
#ifdef DEBUG_FUNCT
    DB(("Exiting GetCartItem\n"));
#endif
    return FALSE;
  
}
  
#endif  /*-- defined(CARTS_IN_RESOURCE) --------------------------------*/
  
  
/***************************************************************************/
/**************************   Local Procedures   ***************************/
  
  
/*  DefaultEnvironment
*
*  Set up the default environment for the printer.
*/
  
LOCAL void
DefaultEnvironment(LPPCLDEVMODE lpDevmode, LPSTR lpDeviceName, HANDLE hModule) {
  
#ifdef DEBUG_FUNCT
    DB(("Entering DefaultEnvironment\n"));
#endif
    DBGEnv(("DefaultEnvironment(%lp,%d)\n",
    lpDevmode, lpDeviceName, hModule));
  
    lmemset((LPSTR)lpDevmode, 0, sizeof(PCLDEVMODE));
    lstrcpy((LPSTR)lpDevmode->dm.dmDeviceName, (LPSTR)lpDeviceName);
    lpDevmode->dm.dmSpecVersion = DM_SPECVERSION;
    lpDevmode->dm.dmDriverVersion = VNUMint;
#if defined(WIN31)
    /* WIN31 has removed the dmDriverExtra field.  "The way it was defined
     * was confusing.  You should assume it was never there."
     */
    lpDevmode->dm.dmSize = sizeof(DEVMODE);  
    lpDevmode->dm.dmDriverExtra = sizeof(PCLDEVMODE) - sizeof(DEVMODE);
#else
    lpDevmode->dm.dmSize = sizeof(DEVMODE) - sizeof(BYTE);    /* BUG #141 */
    lpDevmode->dm.dmDriverExtra = sizeof(PCLDEVMODE) - sizeof(DEVMODE) + sizeof(BYTE);/*BUG #215*/
#endif
    lpDevmode->dm.dmFields = (DM_ORIENTATION | DM_PAPERSIZE | DM_COPIES |
    DM_DEFAULTSOURCE | DM_PRINTQUALITY);
  
    lpDevmode->dm.dmOrientation = DMORIENT_PORTRAIT;
  
    lpDevmode->dm.dmPaperSize = (isUSA()) ? DMPAPER_LETTER : DMPAPER_A4;
    lpDevmode->dm.dmDefaultSource = DMBIN_UPPER;
  
    lpDevmode->dm.dmCopies = 1;
    lpDevmode->dm.dmPrintQuality = DMRES_HIGH;           /* Fix BUG#71 */
    lpDevmode->prtResFac=SF300;
    lpDevmode->pageprotect=OFF;

    /* Fix default duplex bug */
    lpDevmode->dm.dmDuplex = DMDUP_SIMPLEX;
    lpDevmode->offset = OFF;                        /* ELI */
    lpDevmode->reartray = OFF;                         /* ELI */
  
  
    /*  Index to first printer in resources, and no cartridge selected.
    */
    lpDevmode->prtIndex = 0;
     /* Note:  Code that deals with listing the cartridges expects the
      * value in numCartridges to be > 0.
      */
    lpDevmode->numCartridges = 1;  
    lpDevmode->txwhite = 255;
    lpDevmode->options = JETOPTS;
    lpDevmode->fsvers = 0;
    lpDevmode->grayscale = 1;
    lpDevmode->brighten = 0;
    lpDevmode->TTRaster = 0;    /* TrueType fonts default as device fonts 6-19 dtk */
  
#ifdef LOCAL_DEBUG
    DBMSG(("DefaultEnvironment returning:\n"));
    dumpDevMode(lpDevmode);
#endif
#ifdef DEBUG_FUNCT
    DB(("Exiting DefaultEnvironment\n"));
#endif
}
  
  
/*  GetWinIniEnv
*
*  Get the environment information from a .INI file.
*/
LOCAL void GetWinIniEnv(lpDevmode, lpPortName, lpProfile, hModule)
LPPCLDEVMODE lpDevmode;
LPSTR lpPortName;
LPSTR lpProfile;
HANDLE hModule;
{
    char appName[64];
    char name[16];
    short ind, data, tmp;
  
    //#ifndef NO_PRIVATE_HACK
    //    HANDLE hKernel;
    //    FARPROC GetPint;
    //    HANDLE FAR PASCAL LoadLibrary(LPSTR);
    //    HANDLE FAR PASCAL FreeLibrary(HANDLE);
    //#define MAKEINTRESOURCE(n) ((LPSTR)((DWORD)((WORD)n)))
    //
    //    if ((hKernel = LoadLibrary("KERNEL.EXE")) >= 32)
    //  GetPint = GetProcAddress(hKernel,MAKEINTRESOURCE(127));
    //#endif
  
#ifdef DEBUG_FUNCT
    DB(("Entering GetWinIniEnv\n"));
#endif
#if defined(WIN31)
    MakeAppName(lpDevmode->dm.dmDeviceName,lpPortName,appName,sizeof(appName));
#else
    MakeAppName(ModuleNameStr,lpPortName,appName,sizeof(appName));
#endif  
    DBGEnv(("GetWinIniEnv(%lp,%lp,%lp), appName=%ls\n",
    lpDevmode, lpPortName, lpProfile, (LPSTR)appName));
  
    /*  For each dialog item in Win 30, and all but fsvers in Win 31.
    */
    for (ind = WININI_BASE; ind < WININI_LAST; ++ind)
    {
        /*  Load key name of item from resources and get its
        *  corresponding info field from win.ini.
        */
        name[0] = '\0';
        if (LoadString(hModule, ind, (LPSTR)name, sizeof(name)))
        {
            /*  Get data from .INI, use -1 to indicate failure.
            */
            //#ifndef NO_PRIVATE_HACK
            //      data = (lpProfile && GetPint) ?
            //          (int) GetPint((LPSTR)appName,(LPSTR)name,(int)-1,
            //                (LPSTR)lpProfile) :
            //#else
            data = lpProfile ? GetPrivateProfileInt(appName,name,-1,lpProfile) :
            //#endif
            GetProfileInt(appName,name,-1);
  
  
            // force paper to default to 1 (letter)
            // but return -1 for other values
            //if ((data < 0) && (ind != WININI_PAPER))
            if (data < 0)
                continue;
  
            tmp = ind;
  
            /*  Transfer info to devmode.
            */
            switch (ind)
            {
                case WININI_PAPER:
                switch(data)
                {       // we read some normal paper size
                    case DMPAPER_LETTER:
                    case DMPAPER_LEGAL:
                    case DMPAPER_A4:
                    case DMPAPER_EXECUTIVE:
                    case DMPAPER_MONARCH:
                    case DMPAPER_COM10:
                    case DMPAPER_DL:
                    case DMPAPER_C5:
                        lpDevmode->dm.dmPaperSize = data;
                        break;
  
                    default:    // 0 or -1, so set country default
                        lpDevmode->dm.dmPaperSize = (isUSA()) ?
                        DMPAPER_LETTER : DMPAPER_A4;
                }
                    break;
  
                case WININI_COPIES:
                    /*  Size of edit box for copies limits to
                    *  9999 copies.
                    */
                    if (data < 1)
                        data = 1;
                    else if (data > MAX_COPIES)
                        data = MAX_COPIES;
                    lpDevmode->dm.dmCopies = data;
                    break;
  
                case WININI_ORIENT:
                    if ((data == DMORIENT_PORTRAIT) ||
                        (data == DMORIENT_LANDSCAPE))
                        lpDevmode->dm.dmOrientation = data;
                    else
                        lpDevmode->dm.dmOrientation = DMORIENT_PORTRAIT;
                    break;
  
                case WININI_PRTRESFAC:
                    if (data == SF300)
                        lpDevmode->dm.dmPrintQuality = DMRES_HIGH;
                    else if (data == SF150)
                        lpDevmode->dm.dmPrintQuality = DMRES_MEDIUM;
                    else {
                        data = SF75;
                        lpDevmode->dm.dmPrintQuality = DMRES_LOW;
                    }
                    lpDevmode->prtResFac = data;
                    break;
  
                case WININI_TRAY:
                    if ((data == DMBIN_UPPER)  || (data == DMBIN_LOWER) ||
                        (data == DMBIN_MANUAL) || (data == DMBIN_AUTO)  ||
                        (data == DMBIN_ENVELOPE))
                        lpDevmode->dm.dmDefaultSource = data;
                    else
                        lpDevmode->dm.dmDefaultSource = DMBIN_UPPER;
                    break;
                    /*BEGIN ELI */
                case WININI_OUTPUTBIN:
                    lpDevmode->reartray = (BYTE) data;
                    break;

#ifdef WIN31
                /* added for TT as raster - 6-19 dtk
                 */
                case WININI_TTRASTER:
                    lpDevmode->TTRaster = (BYTE) data;
                    break;
#endif

                case WININI_JOBOFFSET:
                    lpDevmode->offset = (BYTE) data;
                    break;
                    /* END ELI */
  
                case WININI_PRTINDEX:
                    if (data < 0)
                        data = 0;
                    else if (data > MAX_PRINTERS - 1)
                        data = MAX_PRINTERS - 1;
                    lpDevmode->prtIndex = data;
                    break;
  
                case WININI_NUMCART:
                    DBMSG(("requested numcart is %d",data));
                    if (data < 0)
                        data = 0;
                    else if (data > DEVMODE_MAXCART)
                        data = DEVMODE_MAXCART;
                    lpDevmode->numCartridges = data;
                    DBMSG((", using %d.\n",data));
                    break;
  
                case WININI_DUPLEX:
                    /* There's no need to check for DMDUP_SIMPLEX.  9 Jan 1990  clarkc  */
                    if (data == DMDUP_VERTICAL || data == DMDUP_HORIZONTAL)
                        lpDevmode->dm.dmDuplex = data;
                    else
                        lpDevmode->dm.dmDuplex = DMDUP_SIMPLEX;
                    break;
#ifndef WIN31  
                case WININI_CARTINDEX:
                case WININI_CARTINDEX1:
                case WININI_CARTINDEX2:
                case WININI_CARTINDEX3:
                case WININI_CARTINDEX4:
                case WININI_CARTINDEX5:
                case WININI_CARTINDEX6:
                case WININI_CARTINDEX7:
                    tmp -= WININI_CARTINDEX;
  
                    if (tmp < lpDevmode->numCartridges)
                    {
                        if (data < 0)
                            data = 0;
                        lpDevmode->cartIndex[tmp] = - data;
                        DBMSG(("lpdevmode->cartindex[%d]=%d\n",
                        tmp,lpDevmode->cartIndex[tmp]));
                    }
                    break;
#endif  
                case WININI_TXWHITE:
                    lpDevmode->txwhite = data;
                    break;
  
                case WININI_OPTIONS:
                    lpDevmode->options = data;
                    break;
#ifndef WIN31  
                case WININI_FSVERS:
                    lpDevmode->fsvers = data;
                    break;
#endif  
                case WININI_PRTCAPS:
                    /*  Note: this is superseded by whatever exists
                    * in the internal resources.  We write this
                    *  field to the win.ini so other apps can use
                    *  the information.
                    */
                    lpDevmode->prtCaps = data;
                    break;
  
                case WININI_PRTCAPS2:
                    /*  Note: this is superseded by whatever exists
                    * in the internal resources.  We write this
                    *  field to the win.ini so other apps can use
                    *  the information.
                    */
                    lpDevmode->prtCaps2 = data;
                    break;
  
                case WININI_PAPERIND:
                    lpDevmode->paperInd = data;
                    break;
  
                case WININI_PGPROTECT:
                    lpDevmode->pageprotect = (BYTE) data;
                    break;
  
                case WININI_GRAYSCALE:
                    lpDevmode->grayscale = (BYTE) data;
                    break;
  
  
                case WININI_BRIGHTEN:
                    lpDevmode->brighten = (BYTE) data;
                    break;
  
  
                default:
                    break;
            }
        } /* end if LoadString()*/
    }  /* end for ind */

#if defined(WIN31)
    /* Read fsvers info from [driver,port]
    */
    MakeAppName(ModuleNameStr,lpPortName,appName,sizeof(appName));
    name[0] = '\0';
    if (LoadString(hModule, WININI_FSVERS, (LPSTR)name, sizeof(name)))
    {
        /*  Get data from .INI, use -1 to indicate failure.
        */
        data = lpProfile ? GetPrivateProfileInt(appName,name,-1,lpProfile) :
                           GetProfileInt(appName,name,-1);
        if (data >= 0)
        {
            /*  Transfer info to devmode.
            */
            lpDevmode->fsvers = data;
        }
    } /* end if LoadString() */

    /* Read cartridge info from [driver,port] */
    for (ind = WININI_CARTINDEX; ind < WININI_LAST; ++ind)
    {
        /*  Load key name of item from resources and get its
        *  corresponding info field from win.ini.
        */
        name[0] = '\0';
        if (LoadString(hModule, ind, (LPSTR)name, sizeof(name)))
        {
            data = lpProfile ? GetPrivateProfileInt(appName,name,-1,lpProfile) :
                               GetProfileInt(appName,name,-1);
            tmp = ind;
            tmp -= WININI_CARTINDEX;
            if (tmp < lpDevmode->numCartridges)
            {
                if (data < 0)
                   data = 0;
                lpDevmode->cartIndex[tmp] = - data;
                DBMSG(("lpdevmode->cartindex[%d]=%d\n",
                        tmp,lpDevmode->cartIndex[tmp]));
            }
        }
    }
#endif /* WIN31 */

    //#ifndef NO_PRIVATE_HACK
    //    if (hKernel >= 32)
    //  FreeLibrary(hKernel);
    //#endif
  
#ifdef LOCAL_DEBUG
    DBMSG(("GetWinIniEnv returning:\n"));
    dumpDevMode(lpDevmode);
#endif
#ifdef DEBUG_FUNCT
    DB(("Exiting GetWinIniEnv\n"));
#endif
}
  
//  isUSA
//
//  Read win.ini and return country code
//
//  Note that if a country was set up, it will have a valid nonzero country
//  code.
//
//  This version of the function returns TRUE for ANY Western-hemisphere
//  country  (USA, CANADA, any area with dial code beginning with 5:
//  5n, 5nn)
  
LOCAL BOOL isUSA()
{
    int iCountry;
  
#ifdef DEBUG_FUNCT
    DB(("Entering IsUSA\n"));
#endif
    iCountry = GetProfileInt((LPSTR)"intl", (LPSTR)"icountry", USA_COUNTRYCODE);
    switch(iCountry)
    {
        case 0:             // string was there but 0
        case USA_COUNTRYCODE:       // DEFINED in COUNTRY.H
        case FRCAN_COUNTRYCODE:
            return TRUE;
  
        default:
            if (((iCountry >= 50) && (iCountry < 60)) ||
                ((iCountry >= 500) && (iCountry < 600)))
                return TRUE;
            else
                return FALSE;
    }
  
#ifdef DEBUG_FUNCT
    DB(("Exiting IsUSA\n"));
#endif
}   // isUSA()

#if defined(WIN31)
/* BUG 516:  Added this module as part of fix. */
/************************************************************************
 *
 *  LOCAL void ParseDevName(LPSTR prtinfostr, LPSTR devname);
 *
 *  Function:   Parses the printer information string (obtained from the
 *              resource file) to obtain the device name.
 *
 *  Inputs:     lpTempbuf    -- pointer to printer information string.
 *              lpDevname    -- pointer to a string.
 *
 *  Outputs:    lpDevname    -- will contain the device name from the printer
 *                            information string.
 *
 **************************************************************************/
LOCAL void ParseDevName(lpTempbuf,lpDevname)
LPSTR lpTempbuf;
LPSTR lpDevname;
{
    char sepchar;
    LPSTR bufptr;
    int i;
    
    bufptr = lpTempbuf;
    sepchar = *bufptr++;   /* First char in string is separator character */
    for (i = 0; *bufptr && (*bufptr != sepchar); i++)
    {
        if (i < DEV_NAME_LEN - 1)
            *lpDevname++ = *bufptr++;
    }
    *lpDevname = '\0';
}
#endif /* WIN31 */

#if defined(WIN31)
/* BUG 516:  Added this module as part of fix. */
/***********************************************************************
 *
 * LOCAL void SetIndex(LPPCLDEVMODE lpDevmode, LPSTR lpDeviceName)
 *
 * Function:    Sets the prtIndex field in the PCLDEVMODE structure to the
 *              index of the first printer in the list of printers that
 *              has the same device name as lpDeviceName.
 *
 * Inputs:  lpDevmode -- pointer to the current LPPCLDEVMODE structure.
 *          lpDeviceName -- pointer to the string containing the device
 *                          name to be matched in the list of printers.
 *
 * Ouput:   The prtIndex field in lpDevmode will be changed to contain the
 *          index of the first printer in the list that matches the device
 *          name.
 *
 **************************************************************************/
LOCAL void SetIndex(lpDevmode, lpDeviceName)
LPPCLDEVMODE lpDevmode;
LPSTR lpDeviceName;
{
    int i;
    char DevName[STR_LEN];
    char Prtinfo[STR_LEN];
    LPSTR bufptr = (LPSTR) Prtinfo;
    BOOL Found = FALSE;
    
    for (i = 0; i+1 < MAX_PRINTERS && !Found;  i++)  /*BUG #620: "+1 "*/
    {
        if (LoadString(hLibInst, DEVNAME_BASE + i, bufptr, STR_LEN));
        {
            ParseDevName(bufptr, (LPSTR) DevName);
            if (!lstrcmpi((LPSTR) DevName, lpDeviceName))
                Found = TRUE;
        }
    }

    if (Found)
        lpDevmode->prtIndex = i - 1;
    else
        lpDevmode->prtIndex = 0; /* In case of error, set to first printer */
}
#endif /* WIN31 */

#if defined (WIN31)
/***********************************************************************
 *  isours
 *
 *  returns true if name is one of our printers.
 *
 ***********************************************************************/
LOCAL BOOL isours(name) 
LPSTR name;
{
    int i;
    char tempbuf[STR_LEN];
    char printer[STR_LEN];
    BOOL found = FALSE;

    for (i=0;  !found && i < MAX_PRINTERS; i++)
            {
                LoadString(hLibInst, DEVNAME_BASE + i, (LPSTR)tempbuf, STR_LEN);
                ParseDevName((LPSTR)tempbuf, (LPSTR)printer);
                found = !lstrcmpi((LPSTR)name, (LPSTR)printer);
            }
    return (found);
}
#endif



