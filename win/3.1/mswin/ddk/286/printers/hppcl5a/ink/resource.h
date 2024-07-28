/**[f******************************************************************
* resource.h -
*
* Copyright (C) 1988,1989 Aldus Corporation
* Copyright (C) 1988-1990 Microsoft Corporation.
* Copyright (C) 1989,1990, 1991 Hewlett-Packard Company.
* All rights reserved.  Company confidential.
*
**f]*****************************************************************/
/*********************************************************************
*  05 sep 91  SD       Include build.h so that can use compiler options
*                      in drivinit.h.  Added dependency on build.h to all
*                      files that were dependent on resource.h in pclsrc.
*  03 sep 91  SD       BUG #619: Change default constants to LJIII values.
*  02 aug 91  SD       Added control ID for driver name in About dialog
*                      for Win 31 to match title on Setup dialog.
*  26 jan 91  SD       Added Challis support.
*  23 jan 91  SD       Added ELI support.  Look for ELI to see changes.
*  27 jul 90  SD       Added grayscale field to PCLDEVMODE to allow
*                      user to turn gray scaling on and off.   Added
*                      #define GRAYSCBOX for checkbox.
*  11 jul 90  SD       Added pageprotect field to PCLDEVMODE for Bug #11
*                      and #defines for OFF, LTR, LGL, LTRMEM, LGLMEM.
*  10 jul 90  SD       Added #defines for PGBOX  and PGLABEL for Bug # 11
*  15 jun 90  SD       Decreased MAX_PAPERLIST to 1
*
*  11 jun 90  SD       Increased MAX_PRINTERS to 11 for LJ IIID support.
*
*  26 jan 90  SD       Remove prtCaps2 field for in the box version.
*
*  25 jan 90  SD       Decreased MAX_PAPERLIST to 5
*
*  22 jan 90  SD       Decreased MAX_PRINTERS to 6
*
*  18 dec 89  SD       Added definition for JETCAPS2
*
*  18 dec 89  SD       Added prtCaps2 field to PCLDEVMODE structure
*
*  15 dec 89  SD       Defined HPLJIII as a IIP without lower tray
*
*  15 dec 89  SD       Increased MAX_PRINTERS to 77
*
*  27 nov 89   peterbe Increased MAX_PRINTERS to 72
*
*  25 oct 89   peterbe Increased MAX_PRINTERS to 70
*
*  19 sep 89   peterbe Changed LITTLESPUD to HPLJIIP.
*
*  06 sep 89   peterbe Changed #include "drivinit.h" to #include <drivinit.h>
*
*  29 aug 89   peterbe Removed old, commented-out defs.
*          Defined LITTLESPUD.
*
*  25 aug 89   craigc  Symbol font conversion. (added MATH8 set)
*
*  22 aug 89   peterbe Increased MAX_PRINTERS to 60 again.
*
*  21 aug 89   peterbe Increased MAX_PRINTERS to 60, then back to 50.
*
*  22 jun 89   peterbe Added CAP_XXX, reserved for Microsoft use.
*
*  07 jun 89   peterbe Added IDPORTLAND for orientation group's icon
*          Also added IDHELP.
*
*  16 may 89   peterbe Added () pair in #define OPTN_IDDPTEK.
*
*  15 apr 89   peterbe Added IDABOUT for About pushbutton.
*
*  01 apr 89   peterbe Added SFABOUT for About dialog.
*
*   1-26-89    jimmat  Many changes cause by the split-out of the font
*          installer (FINSTALL).  I don't know why some of this
*          stuff is in a file named resource.h, but...
*   2-06-89    jimmat  Changes to DEVMODE structure for new driver
*          initialization interface.
*   2-21-89    jimmat  Device Mode Dialog box changes for Windows 3.0.
*/
  
/*
* $Header: resource.h,v 3.890 92/02/06 16:13:35 dtk FREEZE $
*/
  
/*
* $Log:	resource.h,v $
 * Revision 3.890  92/02/06  16:13:35  16:13:35  dtk (Doug Kaltenecker)
 * Win3.1 Freeze
 * 
 * Revision 3.870  91/11/08  11:45:25  11:45:25  dtk (Doug Kaltenecker)
 * 3.1 Release Candidate 1
 * 
 * Revision 3.865  91/11/01  13:53:23  13:53:23  dtk (Doug Kaltenecker)
 * Beta release for Windows 3.1
 * 
 * Revision 3.862  91/10/25  13:48:42  13:48:42  dtk (Doug Kaltenecker)
 * BETA
 * 
 * Revision 3.860  91/10/23  09:50:18  09:50:18  dtk (Doug Kaltenecker)
 * WinWird Release
 * 
 * Revision 3.852  91/10/09  15:01:09  15:01:09  dtk (Doug Kaltenecker)
 * BETA
 * 
 * Revision 3.850  91/10/04  16:51:26  16:51:26  dtk (Doug Kaltenecker)
 * WINWORD RELEASE CANDIDATE
 * 
 * Revision 3.840  91/09/28  14:18:39  14:18:39  dtk (Doug Kaltenecker)
 * RELEASE CANDIDATE 3 FOR WINWORD
 * 
 * Revision 3.830  91/09/18  16:35:00  16:35:00  dtk (Doug Kaltenecker)
 * RELEASE
 * 
 * Revision 3.822  91/09/16  10:36:15  10:36:15  dtk (Doug Kaltenecker)
 * WINWORD RELEASE CANDIDATE 2 FOR WIN30.
 * 
 * Revision 3.820  91/09/06  14:13:40  14:13:40  dtk (Doug Kaltenecker)
 * RELEASE CANDIDATE FOR WINWORD 2.0
 * 
 * Revision 3.814  91/09/05  17:34:17  17:34:17  daniels (Susan Daniels)
 * Include build.h ahead of the include for drivinit.h.
 * 
 * Revision 3.813  91/09/04  13:28:44  13:28:44  daniels (Susan Daniels)
 * Fix bug #619:  Setting constants for defaults to LJIII.
 * 
 * Revision 3.812  91/08/22  14:33:33  14:33:33  dtk (Doug Kaltenecker)
 * BETA
 * 
 * Revision 3.808  91/08/16  15:49:03  15:49:03  daniels (Susan Daniels)
 * Fix BUG 558: change driver name in About box to match title on Setup box.
 * 
 * Revision 3.807  91/08/08  10:32:48  10:32:48  dtk (Doug Kaltenecker)
 * PREBETA3
 * 
 * Revision 3.802  91/07/22  12:55:59  12:55:59  oakeson (Ken Oakeson)
 * BETA
 * 
 * Revision 3.799  91/07/02  11:53:20  11:53:20  daniels (Susan Daniels)
 * Beta
 * 
 * Revision 3.797  91/07/01  16:28:28  16:28:28  dtk (Doug Kaltenecker)
 * added string for TT as graphics
 * 
 * Revision 3.790  91/06/11  16:04:54  16:04:54  stevec (Steve Claiborne)
 * Freeze
 * 
 * Revision 3.786  91/06/11  15:46:15  15:46:15  dtk (Doug Kaltenecker)
 * Prettified files!
 * 
* Revision 3.785  91/05/22  14:58:30  14:58:30  stevec (Steve Claiborne)
* Beta version to MS
*
* Revision 3.780  91/05/15  15:58:39  15:58:39  stevec (Steve Claiborne)
* Beta
*
* Revision 3.775  91/04/05  14:32:32  14:32:32  stevec (Steve Claiborne)
* Beta release to MS
*
* Revision 3.770  91/03/25  15:37:30  15:37:30  stevec (Steve Claiborne)
* maintance release
*
* Revision 3.760  91/03/12  07:54:19  07:54:19  stevec (Steve Claiborne)
* Maintance release
*
* Revision 3.755  91/03/03  07:47:34  07:47:34  stevec (Steve Claiborne)
* March 3 Freeze
*
* Revision 3.722  91/03/01  16:20:58  16:20:58  daniels (Susan Daniels)
* Adding Challis in list box.
*
* Revision 3.700.1.5  91/02/05  10:49:16  10:49:16  stevec (Steve Claiborne)
* Freeze
*
* Revision 3.700.1.1  91/02/05  09:44:03  09:44:03  daniels (Susan Daniels)
* Adding ELI and Challis support
*
* Revision 3.700  91/01/19  09:01:47  09:01:47  stevec (Steve Claiborne)
* Release
*
* Revision 3.685  91/01/14  15:44:46  15:44:46  stevec (Steve Claiborne)
* Freeze
*
* Revision 3.681  91/01/14  10:18:58  10:18:58  stevec (Steve Claiborne)
* Updated the copy right stmt.
*
* Revision 3.680  91/01/10  16:18:06  16:18:06  stevec (Steve Claiborne)
* Freeze
*
* Revision 3.670  90/12/14  14:55:38  14:55:38  stevec (Steve Claiborne)
* freeze for 12-14-90 ver. 3.670
*
* Revision 3.665  90/12/10  15:37:16  15:37:16  stevec (Steve Claiborne)
* Freeze
*
* Revision 3.660  90/12/07  14:51:47  14:51:47  stevec (Steve Claiborne)
* Freeze 12-7-90
*
* Revision 3.650  90/11/30  08:13:36  08:13:36  stevec (Steve Claiborne)
* Freeze 3.650, 11-30-90
*
* Revision 3.603  90/11/30  07:57:30  07:57:30  tshannon (Terry Shannon)
* Modified gray scaling dialig box
*
* Revision 3.602  90/11/19  08:51:49  08:51:49  tshannon (Terry Shannon)
* Tuned gray scaling for HP Laserjets.  Also added lighten gray scale
* button.  Terry Shannon 11-19-90
*
* Revision 3.601  90/08/24  13:24:27  13:24:27  daniels (Susan Daniels)
* ../message.txt
*
* Revision 3.600  90/08/03  11:11:22  11:11:22  stevec (Steve Claiborne)
* This is the Aug. 3 release ver. 3.600
*
* Revision 3.551  90/07/31  14:34:46  14:34:46  stevec (Steve Claiborne)
* Modified to allow user to turn on/off grayscaling
*
* Revision 3.551  90/07/27  12:56:05  12:56:05  daniels ()
* Added gray scale definition, and field to PCLDEVMODE
*
* Revision 3.540  90/07/25  12:37:10  12:37:10  stevec (Steve Claiborne)
* Experimental freeze 3.54
*
* Revision 3.523  90/07/19  13:21:56  13:21:56  daniels ()
* Adding A4 to PageProtect dialog box.
*
* Revision 3.522  90/07/17  12:20:39  12:20:39  daniels ()
* Bug #11:  add BYTE pageprotect field to PCLDEVMODE;
*
* Revision 3.521  90/06/21  17:11:05  17:11:05  daniels ()
* Incresased MAX_PRINTERS to 11 to add LJ IIID support.
* Fixed bug #16:  Corrected MAXp_pAPERLIST from 5 to 1.
*
*
*    Rev 1.2   20 Feb 1990 15:49:58   vordaz
* Support for downloadables.
*/
  
#include <build.h>
// get this from include directory, not current.
#include <drivinit.h>
  
/*  Compiler switches.
*/
#define SOFTFONTS_ENABLED
#define DIALOG_MESSAGES
  
  
/* Logical font constants */
  
#define DEFAULT_PITCH         0
#define FIXED_PITCH           1
#define VARIABLE_PITCH        2
  
/* GDI font families. */
  
#define FF_DONTCARE     (0<<4)  /* Don't care or don't know. */
#define FF_ROMAN        (1<<4)  /* Variable stroke width, serifed. */
/* Times Roman, Century Schoolbook, etc. */
#define FF_SWISS        (2<<4)  /* Variable stroke width, sans-serifed. */
/* Helvetica, Swiss, etc. */
#define FF_MODERN       (3<<4)  /* Constant stroke width, serifed or sans-serifed. */
/* Pica, Elite, Courier, etc. */
#define FF_SCRIPT       (4<<4)  /* Cursive, etc. */
#define FF_DECORATIVE   (5<<4)  /* Old English, etc. */
  
  
/*  Resource file families
*/
  
#define XFACES     256
#define TRANSTBL   257
#define PAPERFMT   258
#define MYFONT     259
#define PCMFILE    260
  
/*  Resource constants
*/
#define XTBL_USASCII    1
#define XTBL_ROMAN8     2
#define XTBL_GENERIC7   3
#define XTBL_GENERIC8   4
#define XTBL_ECMA94 5
#define XTBL_MATH8  6
  
#define PAPER1          1
  
#define FACES1          1
  
/* dialogs */
#define DTMODE      1
#define OPTIONS     2
#define LOTSOF_FONTSDLG 3
  
#define SFABOUT     4
  
/* printer capability flags in .epCaps */
  
#define HPJET       0x0001  /*printer has capabilities of a 'basic' laserjet*/
#define HPPLUS      0x0002  /*printer has capabilities of a laserjet plus*/
#define HP500       0x0004  /*printer has capabilities of a laserjet 500*/
#define LOTRAY      0x0008  /*lower tray is handled*/
#define NOSOFT      0x0010  /*printer doesn't support d.l. fonts*/
#define NOMAN       0x0020  /*manual feed is not supported*/
#define NOBITSTRIP  0x0040  /*print cannot support internal bit stripping*/
#define MANENVFEED  0x0080  /*printer supports manual envelope feed*/
#define HPEMUL      0x0100  /*printer emulates an hplaserjet*/
#define NEWENVFEED  0x0200  /*printer supports new (LJ IID) envelope feed*/
#define HPIIDDUPLEX 0x0400  /*printer can print LJ IID duplex*/
#define HPLJIIP     0x0800  /*Has HP LJ IIP white rules and compression */
#define PRINTDUPLEX 0x1000  /*printer can print duplex*/
#define AUTOSELECT  0x2000  /*printer selects bin based on paper size*/
#define BOTHORIENT  0x4000  /*printer does autorotation of fonts*/
#define HPSERIESII  0x8000  /*printer has capabilities of a series II*/
#define SCALEFONTS  0x0001  /*printer supports scalable font -- LJIII*/
#define COMPFONTS   0x0002  /*printer supports font compression -- LJIII*/
#define LOWERBIN    0X0004  /*printer has two sw selectable output trays -- ELI */
#define JOBOFFSET   0x0008  /*printer has sw selectable job offset capability -- ELI */
#define PJL         0x0010  /*printer has PJL language switching for PCL -- ELI */
  
#define ANYENVFEED  (MANENVFEED | NEWENVFEED)
#define ANYDUPLEX   (PRINTDUPLEX | HPIIDDUPLEX)
  
/* bits for the options dialog
*
*  IF YOU ADD A CAP BIT HERE, ALSO PUT IT IN DEVICE.I
*/
#define OPTIONS_DPTEKCARD   0x0001  /* enable DP-TEK LaserPort */
#define OPTIONS_RESETJOB    0x0002  /* reset printer between jobs */
#define OPTIONS_FORCESOFT   0x0004  /* always do soft fonts */
#define OPTIONS_VERTCLIP    0x0008  /* allow vertical clip */
  
/*default caps = HP LaserJet III*/
  
#define JETCAPS  (MANENVFEED | HPEMUL | NEWENVFEED | HPLJIIP | BOTHORIENT | HPSERIESII)   /* BUG #619 */
#define JETCAPS2  (SCALEFONTS | COMPFONTS)                /* BUG #619 */
#define JETOPTS     OPTIONS_RESETJOB                      /* BUG #619 */
#define DEVMODE_MAXCART 8
  
/* pageprotect values */
  
#define OFF 0
#define LTR 1
#define LGL 2
#define A4  3
  
/* additional memory used for pageprotection in Kbytes*/
#define LTRMEM    869
#define LGLMEM   1068
  
typedef struct {
    DEVMODE dm;               /* standard Device Mode structure       */
    short prtResFac;              /* printer resolution shift factor      */
    short prtCaps;            /* bit field of printer capabilites     */
    short prtCaps2;           /* Tetra II: 2nd bit field of printer caps*/
    short paperInd;           /* idx in PaperList of supported papers */
    short prtIndex;           /* index DEVNAME in string table        */
    short cartIndex[DEVMODE_MAXCART]; /* index in string table for cart       */
    short numCartridges;          /* number of cartridges selected        */
    short cartind[DEVMODE_MAXCART];   /* index to cartridge font          */
    short cartcount[DEVMODE_MAXCART]; /* number of cartridge fonts        */
    short romind;             /* index from ROM_ESC_BASE to rom fonts */
    short romcount;           /* number of rom fonts              */
    short availmem;           /* available mem in Kbytes          */
    BYTE  pageprotect;        /* 0=OFF, 1=LTR, 2=LGL           */
    BYTE  reartray;           /* 0=no, 1=yes                   */   /* ELI */
    BYTE  offset;             /* 0=no, 1=yes                   */ /* ELI */
    short maxPgSoft;              /* limit of soft fonts per page         */
    short maxSoft;            /* max softfonts that can be downloaded */
    short txwhite;            /* white text intensity             */
    short options;            /* bit field for options dialog         */
    short fsvers;             /* fontSummary version number       */
    BYTE  grayscale;          /* 0=off, 1=on */
    BYTE  brighten;           /* 0=off, 1=on */
    BYTE  TTRaster;           /* 0=off, 1=on */
} PCLDEVMODE;
  

typedef PCLDEVMODE far *LPPCLDEVMODE;
  
/*scalefactors for GDI to go from 300dpi to printer res*/
#define SF75    2
#define SF150   1
#define SF300   0
/* dialog and device mode constants */
  
#define PRTBOX      10
#define MEMBOX      11
#define CARTBOX     12
#define PGBOX       13
#define PGLABEL     14
  
#define PORTRAIT    15
#define LANDSCAPE   16
#define IDPORTLAND  17
  
#define SIZEBOX     20
#define GRAYSCBOX   21
#define BINGROUPBOX 22                   /* ELI */
#define UPPERBINBOX 23                   /* ELI */
#define LOWERBINBOX 24                   /* ELI */
#define JOBSEPBOX   25                   /* ELI */

#define IDDRIVER    26                   /* win 31 About dialog's driver name */  
  
#define DPI75       30
#define DPI150      31
#define DPI300      32
  
#define TRAYBOX     33

#define TTRASTER    34      /* for TT as graphics check box, 6-19 dtk */                    
  
#define COPYBOX     40
#define NUMCARTS    41
#define NODUPLEX    42
#define VDUPLEX     43
#define HDUPLEX     44
  
#define IDSOFTFONT  50
#define IDOPTION    51
#define IDABOUT     52
#define IDHELP      53
  
#define GRBOX       54     /* For common dialog changes */

/* Icon controls for options dialog */
  
#define OPT_ICON    70
  
/* other options dialog constants */
  
#define OPTN_DLG_BASE   100
#define OPTN_IDDPTEK    (OPTN_DLG_BASE+OPTIONS_DPTEKCARD)
  
  
#ifdef DIALOG_MESSAGES
/* Loading lots of fonts dialog constants */
#define LOTSFONT_PCNT   200
#define LOTSFONT_FONT   201
#endif
  
  
/*actual memory values*/
  
#define JETMEM      700         /*available mem on LaserJet III  BUG #619*/
  
/*default # fonts per page/job*/
  
#define MAXPGSOFT 32767                        /* BUG #619 */
#define MAXSOFT   32767                        /* BUG #619 */
  
#define MAX_COPIES  9999
  
/*stuff for print dialog*/
  
#define MAX_PRINTERS        30                 /* ELI and Challis */
#define MAX_CARTRIDGES   50
#define MAX_PAPERLIST     2                 /* ELI */
#define MAX_PAPERSOURCES 10
#define MAX_PAPERSIZES   10
#define STR_LEN      100
  
/*number of cartridges visible in the cartridge list box*/
  
#define NUMVISIBLE_CARTS 5
