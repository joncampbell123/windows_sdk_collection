/*$Header: strings.h,v 3.890 92/02/06 16:13:07 dtk FREEZE $ */
/**[f******************************************************************
* strings.h -
*
* Copyright (C) 1988,1989 Aldus Corporation
* Copyright (C) 1988-1990 Microsoft Corporation.
* Copyright (C) 1989,1990, 1991 Hewlett-Packard Company.
* All rights reserved.  Company confidential.
*
**f]*****************************************************************/
/*$Log */
/*********************************************************************
*
*  13 sep 91  SD       Made DEVINSTL_WARNING dependent on WIN31 
*  26 aug 91  SD       Bug #607:  Removed string ID's for duplex binding
*                      strings since they are now hard coded in the .rc file.
*  13 aug 91  SD       Added ID's for strings to build Setup title, and 
*                      the DevInstall() font warning.
*  13 aug 91  SD       Added IDS_DRIVER for BUG # 558.
*  11 feb 91  SD       Added WININI_OUTPUTBIN  and WININI_JOBOFFSET for ELI.
*  11 jul 90  SD       Added WININI_PGPROTECT fixing Bug #11.
*  09 jul 90  SD       Added envelope support; removed unsupported papers.
*  02 jan 90   peterbe Added S_DATE
*  27 sep 89   peterbe Added NULL_CART.
*  15 apr 89   peterbe Removed IDS_ABOUT, SF_SYSABOUT.
*  01 apr 89   peterbe Added SF_SYSABOUT, IDS_ABOUT for About dialog,menu
*   1-26-89    jimmat  Several changes caused by the split out of the
*          font installer (FINSTALL).
*   2-21-89    jimmat  Device Mode Dialog box changes for Windows 3.0.
*/
  
/*
* $Header: strings.h,v 3.890 92/02/06 16:13:07 dtk FREEZE $
*/
  
/*
* $Log:	strings.h,v $
 * Revision 3.890  92/02/06  16:13:07  16:13:07  dtk (Doug Kaltenecker)
 * Win3.1 Freeze
 * 
 * Revision 3.870  91/11/08  11:44:53  11:44:53  dtk (Doug Kaltenecker)
 * 3.1 Release Candidate 1
 * 
 * Revision 3.865  91/11/01  13:52:51  13:52:51  dtk (Doug Kaltenecker)
 * Beta release for Windows 3.1
 * 
 * Revision 3.862  91/10/25  13:48:11  13:48:11  dtk (Doug Kaltenecker)
 * BETA
 * 
 * Revision 3.860  91/10/23  09:49:45  09:49:45  dtk (Doug Kaltenecker)
 * WinWird Release
 * 
 * Revision 3.852  91/10/09  15:00:39  15:00:39  dtk (Doug Kaltenecker)
 * BETA
 * 
 * Revision 3.850  91/10/04  16:50:55  16:50:55  dtk (Doug Kaltenecker)
 * WINWORD RELEASE CANDIDATE
 * 
 * Revision 3.840  91/09/28  14:18:08  14:18:08  dtk (Doug Kaltenecker)
 * RELEASE CANDIDATE 3 FOR WINWORD
 * 
 * Revision 3.830  91/09/18  16:34:28  16:34:28  dtk (Doug Kaltenecker)
 * RELEASE
 * 
 * Revision 3.822  91/09/16  10:35:30  10:35:30  dtk (Doug Kaltenecker)
 * WINWORD RELEASE CANDIDATE 2 FOR WIN30.
 * 
 * Revision 3.821  91/09/13  08:39:52  08:39:52  daniels (Susan Daniels)
 * Make DEVINSTL_MESSAGE dependent on WIN31
 * 
 * Revision 3.820  91/09/06  14:13:08  14:13:08  dtk (Doug Kaltenecker)
 * RELEASE CANDIDATE FOR WINWORD 2.0
 * 
 * Revision 3.813  91/08/26  14:36:52  14:36:52  daniels (Susan Daniels)
 * Fix Bug #607: x in HorizDuplex cut in half.  Use Long Edge and Short Edge 
 * for all printers now, so removed ID's for old duplex binding strings.
 * 
 * Revision 3.812  91/08/22  14:33:02  14:33:02  dtk (Doug Kaltenecker)
 * BETA
 * 
 * Revision 3.809  91/08/14  10:32:24  10:32:24  daniels (Susan Daniels)
 * Added defines for new strings added to resource file.
 * 
 * Revision 3.808  91/08/13  15:46:59  15:46:59  daniels (Susan Daniels)
 * BUG #558: Putting name of printer in About box;add IDS_DEVICE.
 * 
 * Revision 3.807  91/08/08  10:32:17  10:32:17  dtk (Doug Kaltenecker)
 * PREBETA3
 * 
 * Revision 3.802  91/07/22  12:55:25  12:55:25  oakeson (Ken Oakeson)
 * BETA
 * 
 * Revision 3.799  91/07/02  11:52:47  11:52:47  daniels (Susan Daniels)
 * Beta
 * 
 * Revision 3.797  91/07/01  16:28:52  16:28:52  dtk (Doug Kaltenecker)
 * added string for TT as graphics
 * 
 * Revision 3.790  91/06/11  16:04:20  16:04:20  stevec (Steve Claiborne)
 * Freeze
 * 
 * Revision 3.786  91/06/11  15:45:32  15:45:32  dtk (Doug Kaltenecker)
 * Prettified files!
 * 
* Revision 3.785  91/05/22  14:57:57  14:57:57  stevec (Steve Claiborne)
* Beta version to MS
*
* Revision 3.780  91/05/15  15:58:05  15:58:05  stevec (Steve Claiborne)
* Beta
*
* Revision 3.776  91/04/10  09:55:21  09:55:21  daniels (Susan Daniels)
* Fixing Bugs #145 and #131:  Tray and duplex mode not kept between Windows
* sessions.
*
* Revision 3.770  91/03/25  15:36:57  15:36:57  stevec (Steve Claiborne)
* maintance release
*
* Revision 3.760  91/03/12  07:53:47  07:53:47  stevec (Steve Claiborne)
* Maintance release
*
* Revision 3.755  91/03/03  07:47:02  07:47:02  stevec (Steve Claiborne)
* March 3 Freeze
*
* Revision 3.721  91/02/11  19:12:33  19:12:33  daniels (Susan Daniels)
* Adding ELI
*
* Revision 3.700.1.5  91/02/05  10:48:40  10:48:40  stevec (Steve Claiborne)
* Freeze
*
* Revision 3.700.1.1  91/02/05  09:44:28  09:44:28  daniels (Susan Daniels)
* Adding ELI support
*
* Revision 3.700  91/01/19  09:01:15  09:01:15  stevec (Steve Claiborne)
* Release
*
* Revision 3.685  91/01/14  15:44:14  15:44:14  stevec (Steve Claiborne)
* Freeze
*
* Revision 3.681  91/01/14  10:18:27  10:18:27  stevec (Steve Claiborne)
* Updated the copy right stmt.
*
* Revision 3.680  91/01/10  16:17:33  16:17:33  stevec (Steve Claiborne)
* Freeze
*
* Revision 3.670  90/12/14  14:55:05  14:55:05  stevec (Steve Claiborne)
* freeze for 12-14-90 ver. 3.670
*
* Revision 3.665  90/12/10  15:36:44  15:36:44  stevec (Steve Claiborne)
* Freeze
*
* Revision 3.660  90/12/07  14:51:13  14:51:13  stevec (Steve Claiborne)
* Freeze 12-7-90
*
* Revision 3.650  90/11/30  08:13:05  08:13:05  stevec (Steve Claiborne)
* Freeze 3.650, 11-30-90
*
* Revision 3.603  90/11/30  07:57:33  07:57:33  tshannon (Terry Shannon)
* Modified gray scaling dialig box
*
* Revision 3.602  90/11/19  08:51:52  08:51:52  tshanno
n (Terry Shannon)
* Tuned gray scaling for HP Laserjets.  Also added lighten g
ray scale
* button.  Terry Shannon 11-19-90
*
* Revision 3.601  90/08/24
13:24:29  13:24:29  daniels (Susan Daniels)
* ../message.txt
*
* Revision 3
.600  90/08/03  11:10:47  11:10:47  stevec (Steve Claiborne)
* This is the Aug
. 3 release ver. 3.600
*
* Revision 3.551  90/07/31  14:34:49  14:34:49  ste
vec (Steve Claiborne)
* Modified to allow user to turn on/off grayscaling
*
  
* Revision 3.551  90/07/27  12:57:04  12:57:04  daniels ()
* Added WININI_GRAY
SCALE
*
* Revision 3.540  90/07/25  12:35:59  12:35:59  stevec (Steve Claib
orne)
* Experimental freeze 3.54
*
* Revision 3.523  90/07/19  13:22:20  13
:22:20  daniels ()
* Adding A4 to PageProtect dialog box.
*
* Revision 3.52
2  90/07/17  12:23:18  12:23:18  daniels ()
* Bug #11:  Add #define for WININI
_PGPROTECT.
*
* Revision 3.520  90/06/13  16:53:45  16:53:45  root ()
* 5_2
_release
*
*
*    Rev 1.2   20 Feb 1990 15:50:22   vordaz
* Support for downloadables.
*/
  
#define NULL_PORT   1
  
// Menu message no. for date
#define S_DATE      5
  
#define ERROR_BASE  10
#define WARNING_BASE    50
#define DEVNAME_BASE    100
#define CART_BASE   200
#define ROM_ESC_BASE    300
#define CART_ESC_BASE   400
  
/*  Strings read from the win.ini file.
*/
#define WININI_BASE         1000
#define WININI_PAPER        (WININI_BASE)
#define WININI_COPIES       (WININI_BASE+1)
#define WININI_ORIENT       (WININI_BASE+2)
#define WININI_PRTRESFAC    (WININI_BASE+3)
#define WININI_TRAY         (WININI_BASE+4)
#define WININI_PRTINDEX     (WININI_BASE+5)
#define WININI_NUMCART      (WININI_BASE+6)
#define WININI_DUPLEX       (WININI_BASE+7)

/* BUG #131 and BUG #145:  Move WIN_INI defines that
* followed the cartridge indices to be before them,
* and to locate outputbin and joboffset after prtcaps
* so that they are read/written from/to the win.ini file
* after the prtcaps are updated.
*/
#define WININI_TXWHITE      (WININI_BASE+8)
#define WININI_OPTIONS      (WININI_BASE+9)
#define WININI_FSVERS       (WININI_BASE+10)
#define WININI_PRTCAPS      (WININI_BASE+11)
#define WININI_PAPERIND     (WININI_BASE+12)
#define WININI_CARTRIDGE    (WININI_BASE+13)
#define WININI_PRTCAPS2     (WININI_BASE+14)
#define WININI_PGPROTECT    (WININI_BASE+15)
#define WININI_GRAYSCALE    (WININI_BASE+16)
#define WININI_BRIGHTEN     (WININI_BASE+17)
#define WININI_TTRASTER     (WININI_BASE+18)        /* TT as raster 6-19 dtk */
#define WININI_OUTPUTBIN    (WININI_BASE+19)        /*ELI*/
#define WININI_JOBOFFSET    (WININI_BASE+20)        /*ELI*/
  
/* cartridge indices should be last in this list */
#define WININI_CARTINDEX    (WININI_BASE+21)
#define WININI_CARTINDEX1   (WININI_BASE+22)
#define WININI_CARTINDEX2   (WININI_BASE+23)
#define WININI_CARTINDEX3   (WININI_BASE+24)
#define WININI_CARTINDEX4   (WININI_BASE+25)
#define WININI_CARTINDEX5   (WININI_BASE+26)
#define WININI_CARTINDEX6   (WININI_BASE+27)
#define WININI_CARTINDEX7   (WININI_BASE+28)
#define WININI_LAST         (WININI_BASE+29)
  
#define FSUM_NAME           (WININI_LAST+1)
#define FSUM_MEMLIMIT       (WININI_LAST+2)
#define FSUM_FILEPREFIX     (WININI_LAST+3)
#define FSUM_FILEEXTENSION  (WININI_LAST+4)
#define FSUM_MESSAGE        (WININI_LAST+6)
#define FSUM_MSGLAST        (FSUM_MESSAGE+20)
  
#define IDS_NUMCARTS        (FSUM_MSGLAST+1)
#define IDS_NOCARTS         (FSUM_MSGLAST+2)
#define IDS_WINDOWS         (FSUM_MSGLAST+3)
#define IDS_SPOOLER         (FSUM_MSGLAST+4)
  
#define IDS_UPPER           (FSUM_MSGLAST+5)
#define IDS_LOWER           (FSUM_MSGLAST+6)
#define IDS_MANUAL          (FSUM_MSGLAST+7)
#define IDS_ENVELOPE        (FSUM_MSGLAST+8)
#define IDS_AUTO            (FSUM_MSGLAST+9)
  
#define IDS_POINT           (FSUM_MSGLAST+10)
#define IDS_BOLD            (FSUM_MSGLAST+11)
#define IDS_ITALIC          (FSUM_MSGLAST+12)
  
#define IDS_LETTER          (FSUM_MSGLAST+13)
#define IDS_LEGAL           (FSUM_MSGLAST+14)
#define IDS_EXEC            (FSUM_MSGLAST+15)
#define IDS_A4              (FSUM_MSGLAST+16)
#define IDS_MONARCH         (FSUM_MSGLAST+17)
#define IDS_COM10           (FSUM_MSGLAST+18)
#define IDS_DL              (FSUM_MSGLAST+19)
#define IDS_C5              (FSUM_MSGLAST+20)
  
#define IDS_OFF             (FSUM_MSGLAST+21)
#define IDS_LTR             (FSUM_MSGLAST+22)
#define IDS_LGL             (FSUM_MSGLAST+23)
#define IDS_A4P             (FSUM_MSGLAST+24)
  
#define SF_SOFTFONTS        (FSUM_MSGLAST+25)
  
#define NULL_CART           (FSUM_MSGLAST+26)
  
#define IDS_SMOOTH          (FSUM_MSGLAST+27)
#define IDS_DETAIL          (FSUM_MSGLAST+28)
#define IDS_SCANNED         (FSUM_MSGLAST+29)
  
#define IDS_300DPI          (FSUM_MSGLAST+30)
#define IDS_150DPI          (FSUM_MSGLAST+31)
#define IDS_75DPI           (FSUM_MSGLAST+32)

#define IDS_DRIVER          (FSUM_MSGLAST+33)
#define IDS_SETUP           (FSUM_MSGLAST+34)
#define IDS_ON              (FSUM_MSGLAST+35)
#define IDS_WARNING         (FSUM_MSGLAST+36)
#define IDS_LAST_ENTRY      (FSUM_MSGLAST+37)

/*error string constants*/
#define MAX_PERM_DL         (ERROR_BASE+1)
  
/*warning string constants*/
#define SOFT_LIMIT          (WARNING_BASE+1)

/* This is OK because all files that include strings.h, also include resource.h
 * which includes build.h.
 */
#if defined(WIN31)
#define DEVINSTL_WARNING    (WARNING_BASE+2)
#endif
