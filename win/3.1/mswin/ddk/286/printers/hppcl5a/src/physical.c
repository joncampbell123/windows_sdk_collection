/**[f******************************************************************
* physical.c -
*
* Copyright (C) 1988,1989 Aldus Corporation.
* Copyright (C) 1988-1990 Microsoft Corporation.
* Copyright (C) 1989,1990, 1991 Hewlett-Packard Company.
* All rights reserved.
* Company confidential.
*
**f]*****************************************************************/
  
//*********************************************************************
//
// 9  Jan 91   SD       Use lpDrawMode->bkMode to determine whether
//                      text is to be whited out or not.  See "Device
//                      Adaptation Guide" Common Functions, ExtTextOut
//                      Comments section.  Remove shift factor as 75 and
//                      150 dpi mode doesn't affect text.
// 3  Dec 91   SD       Set opaque rectangle to text size when printing
//                      opaqued text.
// 1  Jan 91   SJC      Shifted xpos,ypos & ClipRect by ScaleFact to
//                      fix bug #109.
// 10 Sep 90   SJC      Moved initial xmoveto/ymoveto later in the code
//                      in order to fix bug #54.
// 20 Jul 90   SJC      Removed PCL_* - don't need to lock segments.
// 20 July 90  SJC      Added features for white text and print direction
//
// 09 may 90   SD       Fix string positioning, invalidate x position;
//                      fix from MS.  BUGFIX
//
// 08 feb 90   VO       Added scaling for fixed pitch scalables.
//
// 07 feb 90   VO       Added LastSize monitoring for scalables.
//
// 20 jan 90    Ken O   Removed non-Galaxy support
//
// 04 dec 89    clarkc  EndStyle() called before nextWidth added to size.
//                      nNotStruckOut subtracted from size when passed
//                      to EndStyle().  Fix for Bug #3770.
//
// 30 nov 89    clarkc  xpos no longer set to zero within the hack
//                      for the Z Cartridge.  Cliprect.left now has
//                      minimum of 0.  Fix for Bug #5822.
//
// 25 aug 89    craigc  Added MATH8 support
//
// 04 aug 89    peterbe Fixing bug 2229: was randomly printing bold justified
//          text: reset epXerr to 0 at end of str_out().
//
//   1-17-89    jimmat  Added PCL_* entry points to lock/unlock data seg.
//
// Notes: As of Aug 1, 1990 release, clipping rectangles are only utilized
//   for portrait (0 degree) retation.  This is because of the lack of
//   applications that do character rotation - none could be found that
//   did clipping and rotation at the same time.
//
  
/*
* $Header: physical.c,v 3.890 92/02/06 16:12:22 dtk FREEZE $
*/
  
/*
* $Log:	physical.c,v $
 * Revision 3.890  92/02/06  16:12:22  16:12:22  dtk (Doug Kaltenecker)
 * Win3.1 Freeze
 * 
 * Revision 3.881  92/01/23  11:29:24  11:29:24  jsmart (Jerry Smart)
 * Fix for bug #801, 657 & MS bug #13140   jcs  1/92
 * 
 * Revision 3.880  92/01/21  11:43:48  11:43:48  dtk (Doug Kaltenecker)
 * Fixed rotation by accepting a negative value
 * and strikeout by moving call to EndStyle.
 * 
 * Revision 3.879  92/01/17  12:07:28  12:07:28  daniels (Susan Daniels)
 * Put back fixes from 3.877 that were lost.  Fixing whiteout at 75 and 150 dpi.
 * 
 * Revision 3.878  92/01/10  11:29:22  11:29:22  dtk (Doug Kaltenecker)
 * Added Jerry's rotation clipping stuff Converted Jerry's rotation clipping stuff to a switch and 
 * fixed the underline bug when text is clipped on the left.
 * 
 * Revision 3.875  91/12/19  13:20:54  13:20:54  jsmart (Jerry Smart)
 * Fixed sign for nextwidth in character clipping - jcs
 * 
 * Revision 3.874  91/12/18  13:39:35  13:39:35  jsmart (Jerry Smart)
 * Fix MS Bug #8783  Rotated text not clipping properly
 * Fix HP Bug 753    270 degree text over clipped on right edge - jcs
 * 
 * Revision 3.873  91/12/04  11:45:46  11:45:46  daniels (Susan Daniels)
 * Modify ExtTextOut so that opaque text uses an opaque rectangle the size
 * of the text.
 * 
 * Revision 3.872  91/12/02  16:44:01  16:44:01  dtk (Doug Kaltenecker)
 *  Added a conditional to not do strikeout on a rotated font.
 * 
 * Revision 3.871  91/11/22  13:19:28  13:19:28  dtk (Doug Kaltenecker)
 * Win 3.1 Post Beta 3 version.
 * 
 * Revision 3.871  91/11/18  15:41:24  15:41:24  jsmart (Jerry Smart)
 * Fix for MS Bug 14975   White for text not printing in Excel
 * 
 * Revision 3.870  91/11/08  11:44:06  11:44:06  dtk (Doug Kaltenecker)
 * 3.1 Release Candidate 1
 * 
 * Revision 3.867  91/11/08  09:22:57  09:22:57  dtk (Doug Kaltenecker)
 * 
 * 
 * Revision 3.865  91/11/01  13:52:04  13:52:04  dtk (Doug Kaltenecker)
 * Beta release for Windows 3.1
 * 
 * Revision 3.862  91/10/25  13:47:24  13:47:24  dtk (Doug Kaltenecker)
 * BETA
 * 
 * Revision 3.860  91/10/23  09:48:52  09:48:52  dtk (Doug Kaltenecker)
 * WinWird Release
 * 
 * Revision 3.853  91/10/23  09:22:32  09:22:32  dtk (Doug Kaltenecker)
 * Added ifdefs around TT underline stuff.
 * 
 * Revision 3.852  91/10/09  14:59:52  14:59:52  dtk (Doug Kaltenecker)
 * BETA
 * 
 * Revision 3.851  91/10/09  14:47:44  14:47:44  dtk (Doug Kaltenecker)
 * Added the RuleIt routine for printing TrueType Underline and Strikeout.
 * 
 * Revision 3.850  91/10/04  16:50:06  16:50:06  dtk (Doug Kaltenecker)
 * WINWORD RELEASE CANDIDATE
 * 
 * Revision 3.841  91/10/04  15:07:21  15:07:21  dtk (Doug Kaltenecker)
 * Changed the way that verticle clipping is implemented, now it
 * will clip a character only if it lies completely outside the cliprect.
 * 
 * Revision 3.840  91/09/28  14:17:23  14:17:23  dtk (Doug Kaltenecker)
 * RELEASE CANDIDATE 3 FOR WINWORD
 * 
 * Revision 3.830  91/09/24  13:28:29  13:28:29  dtk (Doug Kaltenecker)
 * Put in the SCALABLEFONTINFO and ifdef TT'd it.
 * 
 * Revision 3.830  91/09/18  16:33:40  16:33:40  dtk (Doug Kaltenecker)
 * RELEASE
 * 
 * Revision 3.823  91/09/18  16:21:40  16:21:40  dtk (Doug Kaltenecker)
 * Cleaned up the pubchar stuff in str_out().
 * .,
 * 
 * Revision 3.822  91/09/16  10:34:20  10:34:20  dtk (Doug Kaltenecker)
 * WINWORD RELEASE CANDIDATE 2 FOR WIN30.
 * 
 * Revision 3.821  91/09/16  10:16:19  10:16:19  dtk (Doug Kaltenecker)
 * Added code to allow SETCHARSET to also remapp chars for the fonts
 * in the Z cartridge - ECMA 94 and also to return correct widths.
 * 
 * Revision 3.820  91/09/06  14:12:20  14:12:20  dtk (Doug Kaltenecker)
 * RELEASE CANDIDATE FOR WINWORD 2.0
 * 
 * Revision 3.813  91/09/04  11:44:57  11:44:57  dtk (Doug Kaltenecker)
 * Put #ifdef TT around the TTRaster stuff and included build.h.
 * 
 * Revision 3.812  91/08/22  14:32:14  14:32:14  dtk (Doug Kaltenecker)
 * BETA
 * 
 * Revision 3.811  91/08/22  14:07:36  14:07:36  dtk (Doug Kaltenecker)
 * changed the definition of DefaultChar.
 * 
 * Revision 3.810  91/08/22  13:50:05  13:50:05  tshannon (Terry Shannon)
 * Put in by Doug K, not sure about Terry's fix, and he ain't here.
 * 
 * Revision 3.809  91/08/21  14:29:42  14:29:42  jsmart (Jerry Smart)
 * added support for remapping for roman8/bitmapped cartridges
 * 
 * Revision 3.808  91/08/15  13:30:08  13:30:08  dtk (Doug Kaltenecker)
 * Commented out the VERTCLIP conditional on the check_clip call 
 * for the NewWave bug with clipping of bottom of text metafile.
 * 
 * Revision 3.807  91/08/08  10:31:31  10:31:31  dtk (Doug Kaltenecker)
 * PREBETA3
 * 
 * Revision 3.802  91/07/22  12:54:33  12:54:33  oakeson (Ken Oakeson)
 * BETA
 * 
 * Revision 3.800  91/07/21  12:35:35  12:35:35  dtk (Doug Kaltenecker)
 * added code to handle tt as raster and underline and strikeout
 * 
 * Revision 3.799  91/07/02  11:51:56  11:51:56  daniels (Susan Daniels)
 * Beta
 * 
 * Revision 3.798  91/07/01  16:30:21  16:30:21  dtk (Doug Kaltenecker)
 * added support for TT as graphics
 * 
 * Revision 3.797  91/06/28  15:53:02  15:53:02  stevec (Steve Claiborne)
 * Fixed bug #521 - where text was being clipped incorrectly in Excel and 
 * headers/footers weren't being printed.
 * 
 * Revision 3.796  91/06/26  11:26:16  11:26:16  stevec (Steve Claiborne)
 * BETA
 * 
 * Revision 3.790  91/06/11  16:03:31  16:03:31  stevec (Steve Claiborne)
 * Freeze
 * 
 * Revision 3.786  91/06/11  15:44:34  15:44:34  dtk (Doug Kaltenecker)
 * Prettified files!
 * 
 * Revision 3.785  91/05/22  14:57:09  14:57:09  stevec (Steve Claiborne)
 * Beta version to MS
 *
 * Revision 3.780  91/05/15  15:57:18  15:57:18  stevec (Steve Claiborne)
 * Beta
 *
 * Revision 3.776  91/05/09  10:53:35  10:53:35  oakeson (Ken Oakeson)
 * Call LoadWidthTable() if we don't have TT widths before downloading chars
 *
 * Revision 3.775  91/04/05  14:31:11  14:31:11  stevec (Steve Claiborne)
 * Beta release to MS
 *
 * Revision 3.770  91/03/25  15:36:10  15:36:10  stevec (Steve Claiborne)
 * maintance release
 *
 * Revision 3.761  91/03/20  15:34:23  15:34:23  stevec (Steve Claiborne)
 * Fixed bug #163 - Designer and Write clipping text at 75 & 150 DPI.
 * This entailed rolling out a fix that was implemented to correct a bug
 * that Excel has.
 *
 * Revision 3.760  91/03/12  07:52:56  07:52:56  stevec (Steve Claiborne)
 * Maintance release
 *
 * Revision 3.755  91/03/03  07:46:15  07:46:15  stevec (Steve Claiborne)
 * March 3 Freeze
 *
 * Revision 3.720  91/02/11  09:15:33  09:15:33  stevec (Steve Claiborne)
 * Aldus version
 *
 * Revision 3.714  91/02/11  03:22:40  03:22:40  oakeson (Ken Oakeson)
 * Wrote size string to printer for previous change (SETCHARSET...)
 *
 * Revision 3.713  91/02/11  00:41:59  00:41:59  oakeson (Ken Oakeson)
 * Allowed SETCHARSET chars with fixed pitch fonts.  Fixed char download
 * on demand bug with translated symsets.
 *
 * Revision 3.712  91/02/08  16:26:18  16:26:18  stevec (Steve Claiborne)
 * Added debuging
 *
 * Revision 3.711  91/02/07  08:38:30  08:38:30  stevec (Steve Claiborne)
 * Fixed the Excel clipping problem at 75 and 150 DPI - Bug # 124.
 *
 * Revision 3.710  91/02/04  15:47:50  15:47:50  stevec (Steve Claiborne)
 * Aldus freeze
 *
 * Revision 3.701  91/02/04  12:36:39  12:36:39  oakeson (Ken Oakeson)
 * Added support for SETCHARSET
 *
 * Revision 3.700  91/01/19  09:00:32  09:00:32  stevec (Steve Claiborne)
 * Release
 *
 * Revision 3.686  91/01/18  16:53:12  16:53:12  stevec (Steve Claiborne)
 * Skip clipping check if we are rotating text.  SJC
 *
 * Revision 3.685  91/01/14  15:43:26  15:43:26  stevec (Steve Claiborne)
 * Freeze
 *
 * Revision 3.682  91/01/14  10:17:43  10:17:43  stevec (Steve Claiborne)
 * Updated the copy right stmt.
 *
 * Revision 3.681  91/01/14  01:23:36  01:23:36  oakeson (Ken Oakeson)
 * Added error handling when lpSummary->hCharDL doesn't lock
 *
 * Revision 3.680  91/01/10  16:16:49  16:16:49  stevec (Steve Claiborne)
 * Freeze
 *
 * Revision 3.671  91/01/09  10:17:01  10:17:01  stevec (Steve Claiborne)
 * Fixed but #109 -- Shifted xpos,ypos and ClipRect by ScaleFact.  SJC
 *
 * Revision 3.670  90/12/14  14:54:16  14:54:16  stevec (Steve Claiborne)
 * freeze for 12-14-90 ver. 3.670
 *
 * Revision 3.665  90/12/10  15:35:59  15:35:59  stevec (Steve Claiborne)
 * Freeze
 *
 * Revision 3.661  90/12/10  10:04:43  10:04:43  stevec (Steve Claiborne)
 * Removed some unreferenced local variables.  SJC
 *
 * Revision 3.660  90/12/07  14:50:27  14:50:27  stevec (Steve Claiborne)
 * Freeze 12-7-90
 *
 * Revision 3.650  90/11/30  08:12:22  08:12:22  stevec (Steve Claiborne)
 * Freeze 3.650, 11-30-90
 *
 * Revision 3.610  90/11/09  11:30:42  11:30:42  stevec (Steve Claiborne)
 * Modified file to print raster first, text last.  SJC
 *
 * Revision 3.609  90/10/25  17:15:21  17:15:21  oakeson (Ken Oakeson)
 * Removed #ifdefs from around truetype code
 *
 * Revision 3.608  90/10/24  17:45:43  17:45:43  oakeson (Ken Oakeson)
 * Added comments for code review and put "already_down" variable before
 * "LoadFontString" function in conditional expression.
 *
 * Revision 3.607  90/09/12  17:26:28  17:26:28  oakeson (Ken Oakeson)
 * Put Ectl update outside of #ifdef TT_FONTKIT
 *
 * Revision 3.606  90/09/10  10:52:16  10:52:16  stevec (Steve Claiborne)
 * Corrected bug #54 - redundant xmoves.  SJC
 *
 * Revision 3.605  90/09/06  15:01:49  15:01:49  oakeson (Ken Oakeson)
 * Modified code to support dynamic storage of char download data
 *
 * Revision 3.604  90/08/29  17:44:45  17:44:45  oakeson (Ken Oakeson)
 * Avoided scaling transformations for TrueType fonts.
 * Checked output string for characters that haven't been downloaded yet
 * (for on demand character downloads)
 *
 * Revision 3.603  90/08/24  11:37:51  11:37:51  daniels ()
 * message.txt
 *
 * Revision 3.602  90/08/14  15:33:00  15:33:00  oakeson (Ken Oakeson)
 * Added TrueType support and removed line that reset lpDevice->epCurx
 * and cause reduntant sending of horiz move esc sequence to printer
 *
 * Revision 3.601  90/08/06  16:44:47  16:44:47  stevec (Steve Claiborne)
 * Corrected obscure character dropping problem.
 *
 * Revision 3.601  90/08/06  12:25:45  12:25:45  stevec (Steve Claiborne)
 * Moved StartStyle after initial xmoveto
 *
 * Revision 3.600  90/08/03  11:09:57  11:09:57  stevec (Steve Claiborne)
 * This is the Aug. 3 release ver. 3.600
 *
 * Revision 3.553  90/08/02  13:18:32  13:18:32  stevec (Steve Claiborne)
 * Added/subtracted dfAscent from xpos and/or ypos before xmoveto/ymoveto
 * in order to correct print direction initial positioning bug.
 *
 * Revision 3.551  90/08/01  09:23:34  09:23:34  stevec (Steve Claiborne)
 * Modified print rotation angles to be in tenths of a degree.
 *
 * Revision 3.550  90/07/27  11:31:11  11:31:11  root ()
 * Experimental freeze 3.55
 *
 * Revision 3.542  90/07/27  08:35:58  08:35:58  oakeson ()
 * Only use translation table for ECMA-94, USASCII, and Generic7
 *
 * Revision 3.541  90/07/26  14:58:37  14:58:37  stevec (Steve Claiborne)
 * Fixed underlining...
 *
 * Revision 3.540  90/07/25  12:34:44  12:34:44  stevec (Steve Claiborne)
 * Experimental freeze 3.54
 *
 * Revision 3.523  90/07/21  10:44:34  10:44:34  stevec (Steve Claiborne)
 * Added print rotation support
 * Added white text support
 * Removed PCL_* lock functions
 *
 * Revision 3.522  90/07/19  12:34:32  12:34:32  oakeson ()
 * Removed Math8 translation table
 *
 * Revision 3.521  90/07/19  09:43:32  09:43:32  stevec (Steve Claiborne)
 * Check in with no changes so Ken could modify
 *
 * Revision 3.520  90/06/13  16:52:43  16:52:43  root ()
 * 5_2_release
 *
 *
 *    Rev 1.2   09 May 1990 10:52:28   daniels
 * Fix string positioning, invalidate x position.  Fixes from MS
 * went into 3.42.
 *
 * Support for downloadables.
 */
  
//#define DEBUG
  
#include "generic.h"
#include "resource.h"
#define FONTMAN_UTILS
#include "fontman.h"
#include "strings.h"
#include "utils.h"
#define SEG_PHYSICAL
#include "memoman.h"
#include "transtbl.h"
#include "truetype.h"
#include "build.h"
  
  
/*  Utilities
*/
#include "message.c"
#include "lockfont.c"
  
// These are no longer uses.  SJC
#define shiftitL(x) (x)
#define shiftitR(x) (x)

/*  Debug switches
*/
#undef DBGdumpwidth
#define DBGtrace(msg)   /* DBMSG(msg) */
#define DBGextto(msg)   /* DBMSG(msg) */
#undef DBGEscapes
#define DBGStrOut(msg)  /* DBMSG(msg) */
#define DBGtrans(msg)   /* DBMSG(msg) */
#undef DBGdumpFontInfo
  
#define GetRValue(rgb) ((BYTE)(rgb))
#define GetGValue(rgb) ((BYTE)(((WORD)(rgb)) >> 8))
#define GetBValue(rgb) ((BYTE)((rgb)>>16))
  
#define WHITENESS   (DWORD)0x00FF0062  /* dest = WHITE */
#define EXTTEXT_D1 0x0002
#define EXTTEXT_D2 0x0004
#define MAXNUM_WIDTHS 100
#define WHITE_TEXT      "\033*v1O\033*v1T",10    /* esc sequence + # chars */
#define NO_WHITE_TEXT   "\033*v0O\033*v0T",10    /* esc sequence + # chars */

#define POSX 0            /* print direction constants sjc */
#define POSY 900          /* degree rotation is in 10ths of a degree */
#define NEGX 1800
#define NEGY 2700

// added from device.h for win31 dependancy - dtk
#ifdef WIN31
typedef SCALABLEFONTINFO  far * LPSCALABLEFONTINFO;
#endif


/*  Forward definitions.
 */
long far PASCAL dmExtTextOut(LPDEVICE, short, short, LPRECT, LPSTR,
	                         short, LPFONTINFO, LPDRAWMODE, LPTEXTXFORM, 
                             short far *, LPRECT, WORD);

long far PASCAL StrBlt(LPDEVICE,short,short,LPRECT,LPSTR,short,
                        LPFONTINFO,LPDRAWMODE,LPTEXTXFORM);

long far PASCAL ExtTextOut(LPDEVICE, short, short, LPRECT, LPSTR,
                           short, LPFONTINFO, LPDRAWMODE, LPTEXTXFORM, 
                           short far *, LPRECT, WORD);

int  far PASCAL allocate_mem(LPDEVICE);
//int check_clip(short,short,LPRECT);

static void  OverLap(LPDEVICE, short, short, short, short);
static short FakeBoldOverhang(LPFONTINFO, LPTEXTXFORM);
static long  str_out(LPDEVICE, short, short, LPFONTINFO, LPDRAWMODE,
                     LPTEXTXFORM, short, LPSTR, LPRECT, short far *);

static int  char_out(LPDEVICE, char, short, BYTE);
static int  RelXMove(LPDEVICE, short);
static int  StartStyle(LPDEVICE, LPTEXTXFORM, LPFONTINFO);
static int  EndStyle(LPDEVICE, LPTEXTXFORM, LPFONTINFO, short);
static BOOL SetJustification(LPDEVICE, LPDRAWMODE, LPJUSTBREAKTYPE,
                             LPJUSTBREAKREC, LPJUSTBREAKREC);

static short GetJustBreak (LPJUSTBREAKREC, JUSTBREAKTYPE);
static LPSTR DoubleSizeCopy(LPHANDLE, LPSTR, short, short);
static BOOL  isWhite(long, short);
/* added for printing TT underline a strikeout - dtk
 */
static BOOL  RuleIt(LPDEVICE, short, short, short);
static BOOL WhiteOut(LPDEVICE, short, short, short, short);
  

#ifdef DEBUG
static void debugStr(pstr, count)
LPSTR pstr;
short count;
{
    int i = 0;
    short escflag;
    DBMSG(("string: "));
  
    for (i = 0; i < count; i++, pstr++)
        if (*pstr == '\033')
            DBMSG(("ESC"));
        else
            DBMSG(("%c", *pstr));
  
    DBMSG(("\n"));
}
#endif
  
/**************************************************************************/
/*************************   Windows Text Stubs   *************************/
  
  
/*  StrBlt()
*
*  Windows 1.04 and earlier string output routine.
*/
long far PASCAL StrBlt(lpDevice, x, y, lpClipRect, lpString, count,
lpFont, lpDrawMode, lpXform)
LPDEVICE lpDevice;
short x;
short y;
LPRECT lpClipRect;
LPSTR lpString;
short count;
LPFONTINFO lpFont;
LPDRAWMODE lpDrawMode;
LPTEXTXFORM lpXform;
{
  
    if (!lpDevice->epType)
    {
        DBGtrace(("StrBlt(): ***calling dmStrBlt\n"));
  
        return (dmStrBlt(lpDevice, x, y, lpClipRect, lpString, count,
        lpFont, lpDrawMode, lpXform));
    }
  
    return (ExtTextOut(lpDevice, x, y, lpClipRect, lpString,
    count, lpFont, lpDrawMode, lpXform, 0L, 0L, 0));
  
}   // StrBlt()
  


/***********************************************************************
 ***********************************************************************
 *
 *   ExtTextOut()
 *
 *  Extended Text Output:  display a text string and return its width.
 *  If count < 0, then just return the width of the string.
 *  5-1-90 Added support for white text.  See variables white_text,
 *  WHITE_TEXT and NO_WHITE_TEXT.  We keep track if we write escape
 *  sequence for white text to the printer so that we can reset when
 *  we have output the string.  SJC
 *
 ***********************************************************************/

long far PASCAL ExtTextOut(lpDevice, x, y, lpClipRect, lpString, count, 
                           lpFont, lpDrawMode, lpXform, lpWidths, lpOpaqRect, 
                           options)
LPDEVICE lpDevice;
short x;
short y;
LPRECT lpClipRect;
LPSTR lpString;
short count;
LPFONTINFO lpFont;
LPDRAWMODE lpDrawMode;
LPTEXTXFORM lpXform;
short far *lpWidths;
LPRECT lpOpaqRect;
WORD options;
{
    long status = 0;
    short overhang;
    short i;
    RECT cliprect, opaqrect;
    short white_text=0;

#ifdef WIN31

/*  if TT as raster is turned on - dtk 
 */
    if ((!(lpFont->dfType & TYPE_DEVICE)) && lpDevice->epTTRaster) 
    {
        /* check to see if we need to do underline or strikeout 
         *          - N O T
         * We don't need to do this any more since GDI will fill in the
         * underline or strikeout! - dtk 10-91
         */
//        if((lpXform->ftUnderline) || (lpXform->ftStrikeOut))
//            RuleIt(lpDevice, lpFont, lpXform, lpWidths, count);

        /* basically do everything FixBandBitmap() does in stubs.c right?
         */
        if (lpDevice->epType) // this is our PDEVICE
        { 
            lpDevice->epMode |= ANYGRX;

            if (!(lpDevice->epNband == 2))
                return TRUE;

            lpDevice->epMode |= GRXFLAG;

            /* allocate memory for the bitmap 
             */
            allocate_mem(lpDevice);

            lpDevice = (LPDEVICE)&lpDevice->epBmpHdr;	// point at our bitmap
        }

        /* call gdi to fill in the bitmap
         */
        status = dmExtTextOut(
                lpDevice,
                x, y, lpClipRect, lpString, count,
                lpFont, lpDrawMode, lpXform, lpWidths, lpOpaqRect, options);

        return(status); 

    } /* end 'o TT raster stuff */

#endif


    if (!lpDevice->epType)
        return (0L);
  
    /*  Synthesized bold.
    */
    if (lpFont->dfWeight < lpXform->ftWeight)
        overhang = lpXform->ftOverhang;
    else
        overhang = 0;
  
    // Shift the x & y coordinates and the opaque rect. (if one exists) by
    // the scaling factor.  The opaque rect. is enlarged on all sides.
    // Fixes bug # 124
  
    /*  If count < 0, the caller wants only the string length.
    */
    if (count < 0)
    {
        lpClipRect = 0L;
        lpOpaqRect = 0L;
        goto getlength;
    }
  
    /*  Test for white type.  If we're just getting the extent of
     *  the string, always assume black type.
     */
    if (count > 0 && isWhite(lpDrawMode->TextColor, lpDevice->epTxWhite))
    {
        /* Invert the count so we won't output the string,
         * but will return its length.
         * 5/1/90 - No longer invert count so that str_out WILL output
         * the white text.
         *
         *    Fix for Bug # 659 & 801, MS Bug #13140     
         *    move this line to precede the str_out calls
         *    myWrite(lpDevice, WHITE_TEXT); 1/92 jcs   */
        white_text=1;
    }
    else
        lpDrawMode->TextColor = 0L;
  
  
    /*  Make local copies of clip and opaque rectangles, and shift them
     *  based upon x and y offsets.
     */
    if (lpClipRect)
    {
        lmemcpy((LPSTR) &cliprect, (LPSTR)lpClipRect, sizeof (RECT));
        lpClipRect = (LPRECT) &cliprect;
  
        if (OffsetClipRect(lpClipRect,
            lpDevice->epXOffset >> lpDevice->epScaleFac,
            lpDevice->epYOffset >> lpDevice->epScaleFac) <= 0)
        {
            lpClipRect = 0L;
            return (0L);
        }
    }
  
    if (lpOpaqRect)
    {
        lmemcpy((LPSTR) &opaqrect, (LPSTR)lpOpaqRect, sizeof(RECT));
        lpOpaqRect = (LPRECT) &opaqrect;
  
        if(OffsetClipRect(lpOpaqRect,
          lpDevice->epXOffset >> lpDevice->epScaleFac,
          lpDevice->epYOffset >> lpDevice->epScaleFac) <= 0)

            lpOpaqRect = 0L;
    }
  
  
    /*  Modify opaque and clip regions per options switches.
     */
    if (lpOpaqRect)
    {
        if (options & EXTTEXT_D2)
        {
            /*  lpOpaqRect should be used as a clipping rectangle.
             */
            if (lpClipRect)
                IntersectRect(lpClipRect, lpClipRect, lpOpaqRect);
            else
            {
                lmemcpy((LPSTR) &cliprect, (LPSTR)lpOpaqRect, sizeof (RECT));
                lpClipRect = (LPRECT) &cliprect;
            }
        }
  
        if (options & EXTTEXT_D1)
        {
            /*  lpOpaqRect should be used as an opaque rectangle.
             */
            if (lpClipRect)
                IntersectRect(lpOpaqRect, lpClipRect, lpOpaqRect);
        }
        else
            lpOpaqRect = 0L;
    }
  
    /*  If count == 0, then just output the opaque rectangle if supplied.
     */
    if (count == 0)
    {
        if (TEXTBAND)
            return (0L);
        else
            goto justopaque;
    }
  
    if (TEXTBAND)
    {
        short strikes;
  
        /*  If the background will be white'd out, we set the epOpaqText
         *  flag.  The BANDINFO escape will request the application to
         *  send down text on graphics bands.
         */
        if (lpOpaqRect || (lpDrawMode->bkMode == OPAQUE))
        {
            lpDevice->epOpaqText = TRUE;
        }
  
        for (strikes = 1; strikes <= overhang; strikes++)
        {
            /*    Fix for Bug # 659, 801 & MS Bug #13140  
                  turn on white text where it will
                  be used just before it is needed - jcs   1/92 */   
            if(white_text) 
                myWrite(lpDevice, WHITE_TEXT);
                
            str_out(lpDevice, x + strikes, y, lpFont, lpDrawMode, lpXform,
            count, lpString, lpClipRect, lpWidths);
            
            // Write out the escape sequence to turn off white text - sjc
            // only if we turned it on to begin with.
            if(white_text) 
                myWrite(lpDevice, NO_WHITE_TEXT);
        }
  
        if (lpDrawMode->bkMode == OPAQUE)
        {
        
            /*  Fix for MS Bug 14975.
                This white rule fixes the white_out used behind text
                in Excel.  The white rule is sent just before the text
                if an opaque rectangle is defined.
             */
            RECT backRect;

            /*  Pick up the dimensions of the string
             *  (but don't output it).
             */
            status = str_out(lpDevice, x, y, lpFont, lpDrawMode, lpXform,
                     count > 0 ? -count : count, 
                     lpString, lpClipRect,lpWidths) + overhang;
 
            /*  Set the dimensions of opaque rectangle using text dimensions
             * in status to give the right and bottom values.
             */
            backRect.left = x;
            backRect.top = y;
            backRect.right = x + (LWORD(status));
            backRect.bottom = y + (HWORD (status));
  
  
            WhiteOut(lpDevice, x, y, backRect.bottom - backRect.top,
                        backRect.right - backRect.left);
        }

        getlength:
        
            /*    Fix for Bug # 659, 801,& MS Bug #13140  jcs  1/92  */   
        if(white_text) 
            myWrite(lpDevice, WHITE_TEXT);
        
        status = str_out(lpDevice, x, y, lpFont, lpDrawMode, lpXform, 
                          count,lpString, lpClipRect, lpWidths) + overhang;
        // Write out the escape sequence to turn off white text - sjc
        // only if we turned it on to begin with.

        if(white_text) 
            myWrite(lpDevice, NO_WHITE_TEXT);

        return (status);
    }
  
    /*  Pick up the dimensions of the string
     *  (but don't output it).
     */
    status = str_out(lpDevice, x, y, lpFont, lpDrawMode, lpXform,
                     count > 0 ? -count : count, 
                     lpString, lpClipRect,lpWidths) + overhang;
  
    /*  In OPAQUE text mode we make sure that the background of the exact
     *  line of text is white'd out.
     */
    if (lpDrawMode->bkMode == OPAQUE)
    {
        RECT backRect;
  
        /*  Pick up dimensions of opaque rectangle.
         */
        backRect.left = x;
        backRect.top = y;
        backRect.right = x + (LWORD(status) >> lpDevice->epScaleFac);
        backRect.bottom = y + (HWORD (status) >> lpDevice->epScaleFac);
  
        /*  Clip.
         */
        if (lpClipRect)
            IntersectRect(&backRect, lpClipRect, &backRect);
  
        /*  Turn right and bottom into width and depth.
         */
        backRect.right -= backRect.left;
        backRect.bottom -= backRect.top;
  
        /*  Offset top left corner by current band rect.
         */
        backRect.left -= lpDevice->epXOffset >> lpDevice->epScaleFac;
        backRect.top -= lpDevice->epYOffset >> lpDevice->epScaleFac;
  
        /*  Output via GDI.
         */
        if(RealMode)
            lpDevice->epBmpHdr.bmBits = lpDevice->epBmp;

        OverLap(lpDevice, backRect.left, backRect.top,
                backRect.right, backRect.bottom);
    }
  
    justopaque:
    /*  If we received an OPAQUE rectangle, then we white out the
     *  region specified by the rectangle.
     */
    if (lpOpaqRect)
    {
        if(RealMode)
            lpDevice->epBmpHdr.bmBits = lpDevice->epBmp;

        OverLap(lpDevice, lpOpaqRect->left, lpOpaqRect->top,
                (lpOpaqRect->right - lpOpaqRect->left),
                (lpOpaqRect->bottom - lpOpaqRect->top));
    }
  
    return status;
  
}   // ExtTextOut()
  
/*************************************************************************
 *
 *   OverLap()
 *
 *   Output white rectangle.
 *
 *************************************************************************/
static void OverLap(lpDevice, x, y, xext, yext)
LPDEVICE lpDevice;
short x, y, xext, yext;
{
  
#ifdef DEBUG_FUNCT
    DB(("Entering OverLap\n"));
#endif
  
    DBGtrace(("OverLap(%lp,%d,%d,%d,%d)\n", lpDevice, x, y, xext, yext));
  
    if (x < 0)
    {
        xext += x;
        x = 0;
    }
  
    if (y < 0)
    {
        yext += y;
        y = 0;
    }
  
    if ((xext <= 0) || (yext <= 0))
        return;
  
    if (x + xext > lpDevice->epBmpHdr.bmWidth)
        xext = lpDevice->epBmpHdr.bmWidth - x;
  
    if (y + yext > lpDevice->epBmpHdr.bmHeight)
        yext = lpDevice->epBmpHdr.bmHeight - y;
  
    if ((xext <= 0) || (yext <= 0))
        return;
  
    DBGtrace(("...rect: x=%d,y=%d,width=%d,depth=%d\n", x, y, xext, yext));
  
    if(lpDevice->epNband>2)  /* This is a for-real band */
        if (xext > 0 && yext > 0) {
            if(RealMode) lpDevice->epBmpHdr.bmBits=lpDevice->epBmp;
            allocate_mem(lpDevice);
            dmBitblt((LPDEVICE) &lpDevice->epBmpHdr, x, y, 0, 0, 0, xext, yext,
            WHITENESS, (long)0L, (long)0L);
        }
  
#ifdef DEBUG_FUNCT
    DB(("Exiting OverLap\n"));
#endif
  
}   //OverLap()
  
/**************************************************************************/
/***************************   Text Utilities   ***************************/
  
  
/**************************************************************************
 *
 * str_out()
 *
 *  Low-level routine for getting widths and outputting text.
 *
 **************************************************************************/

static long str_out(lpDevice, xpos, ypos, lpFont, lpDrawMode, lpXform,
                    count, lpString, lpClipRect, lpWidths)
LPDEVICE lpDevice;
short xpos;
short ypos;
LPFONTINFO lpFont;
LPDRAWMODE lpDrawMode;
LPTEXTXFORM lpXform;
short count;
LPSTR lpString;
LPRECT lpClipRect;
short far *lpWidths;
{
    JUSTBREAKREC JustifyWB, JustifyLTR;
    JUSTBREAKTYPE JustType;
    short width, real;
    short prevwidth, nextwidth;
    short size;
    BYTE dfBreakChar;
    BYTE dfFirstChar;
    BYTE dfLastChar;
    BYTE dfDefaultChar;
    BYTE thisChar;
    BYTE overStrike;
    short far *widthptr;
    short far *lpdx;
    short far *lpw;
    short i, j;
    BOOL gotdeltax;
    BOOL updatex;
    register short ctl;
    BOOL PubChar;
    BOOL shiftCH;
    LPSTR lpTransTbl;
    HANDLE hTransData;
    long status = OEM_FAILED;
    int err = SUCCESS;
    RECT ZCARTrect;
    BOOL OutLeft;   
    BOOL OutRight;  
    BOOL clipL;

#ifdef WIN31  
    /* TrueType support var */
    LPTTFONTINFO lpttfi;
#endif
  
    /*** Tetra -- vars for char downloading on-demand ***/
    short char_ind = 0;             /* index into text string */
    BOOL already_down = TRUE;       /* TRUE if chars are already downloaded   */
    BOOL Softfont = FALSE;          /* TRUE if we're dealing with a soft font */
    LPFONTSUMMARYHDR lpFontSummary;
    LPFONTSUMMARY lpSummary;
    LPCHARDL lpCharDL;
  
    extern HANDLE hLibInst;
    short xydirection,outside;
    RECT cliprect;                        /* in case no cliprect exists */
  
    if (!lpClipRect)
        SetRect(lpClipRect = (LPRECT) &cliprect, 0, 0, lpDevice->epPF.xImage,
        lpDevice->epPF.yImage);


    /* update in case it moved in real mode
     */
    if (lpFont->dfType & TYPE_TRUETYPE)
        ((WORD FAR *)&lpFont->dfBitsOffset)[1] = HWORD(lpFont);
  
    real = (count > 0);
  
    if (count < 0)
        count = -count;
  
    /* temp vars for rotation - 0, 90, 180, 270
     */
    if (lpXform->ftOrientation >= 0)
        xydirection = (lpXform->ftOrientation);
    else 
        xydirection = 3600 + lpXform->ftOrientation;

    outside = FALSE;
  
        /* If the line is going to be output, check the logical 
         * vertical clipping of the text based on the direction 
         * of the printing - dtk 10-91
         */
    if (real && lpClipRect)

          /* Take the following check out since it used to be used for 
           * vertical binding with duplex printers - dtk 8-91
           */
//        && (lpDevice->epOptions & OPTIONS_VERTCLIP)) 
    {
        switch (xydirection) 
        {
            case POSX:  /* portrait - 0 */
                    if ((ypos + lpFont->dfPixHeight < lpClipRect->top) ||
                        (ypos > lpClipRect->bottom))
                        outside = TRUE;
                    break;

            case POSY:  /* rev. landscape - 90 */
                    if ((xpos + lpFont->dfPixHeight < lpClipRect->left) ||
                        (xpos > lpClipRect->right))
                        outside = TRUE;
                    break;

            case NEGX:  /* rev. portrait - 180 */
                    if ((ypos - lpFont->dfPixHeight > lpClipRect->bottom) ||
                        (ypos < lpClipRect->top))
                        outside = TRUE;
                    break;

            case NEGY:  /* landscape - 270 */
                    if ((xpos - lpFont->dfPixHeight > lpClipRect->right) ||
                        (xpos < lpClipRect->left))
                        outside = TRUE;
                    break;

            default:    /* just in case it's at some other angle */
                    outside = FALSE;
                    break;
        }

        /* if the character(s) fell outside the cliprect,
         * bail out now and don't print it.
         */
        if(outside == TRUE)
            return(status);
    }

//  Take this stuff out, since it was based on previous 
//  assumptions about how text lines should be clipped.
//  The corresponding routine, check_clip has also 
//  been commented out - dtk 10-91
//
//        /* portrate or rev. portrate 
//         */
//        if((xydirection==0) || (xydirection==2)) 
//        {
//            pos=check_clip(xpos,ypos+lpFont->dfInternalLeading,lpClipRect);
//            pos|=check_clip(xpos,ypos+lpFont->dfPixHeight,lpClipRect);
//            pos=(pos&5); /* 5=top or bottom (1 or 4)*/
//
//        }
//        else 
//        {
//            /* landscape or rev. landscape */
//            pos=check_clip(xpos,ypos,lpClipRect);
//            pos=(pos&10);  /* 10= left or right (8 or 2) */
//        }
//        if(pos)
//            return(status);
//    }

  
    /*  Make a copy of the array of widths.  This structure is used to
    *  hold the width of each character and the delta-x move passed in
    *  from ExtTextOut().  The structure starts out looking like this:
    *
    *      array-of-desired-widths[count];  (from ExtTextOut())
    *      unused[count];
    *
    *  Then, when we roll through the string picking up the true widths
    *  of the characters, the structure changes to look like this:
    *
    *      array-of-true-widths[count];
    *      array-of-deltax-moves[count];
    */
    gotdeltax = (lpWidths != 0L);
    lpWidths = (short far *)DoubleSizeCopy(&lpDevice->epHWidths,
                                           (LPSTR)lpWidths, count, 
                                           sizeof(short));
  
    if (!lpWidths)
    {
        DBMSG(("str_out(): could *not* alloc array of widths\n"));
        goto backout0;
    }
  
    dfFirstChar   = lpFont->dfFirstChar;
    dfLastChar    = lpFont->dfLastChar;
    dfBreakChar   = lpFont->dfBreakChar + lpFont->dfFirstChar;
    dfDefaultChar = lpFont->dfDefaultChar;
  
    /*  Send the information to the printer to change the font if
    *  we're actually outputting the string.
    */
    if (real)
    {
#ifdef WIN31  

        if (lpFont->dfType & TYPE_TRUETYPE)
        {
            lpttfi = (LPTTFONTINFO)lpFont->dfBitsOffset;
  
            /* If the lpFont data is new, we won't have the widths 
             */
            if (!(lpFont->dfType & TYPE_HAVEWIDTHS))
                LoadWidthTable(lpDevice, lpFont);
  
            /* TrueType font
             */
            SelectTTFont(lpDevice,lpFont);
            for (ctl = 0; ctl < count; ctl++)
            {
                if (!ISCHARDOWN(lpttfi,lpString[ctl]))
                {
                    DownloadCharacter(lpDevice,lpFont,lpString[ctl]);
                    SETCHARDOWN(lpttfi,lpString[ctl]);
                }
            }
        }
        else
#endif  

        {
            ctl = ((LPPRDFONTINFO)lpFont)->indFontSummary;
  
            DBGStrOut(("Str_out():About to lock fsum\n"));
  
            /* Lock down the font summary to get download info */
            if (lpFontSummary = lockFontSummary(lpDevice))
            {
                lpSummary = &lpFontSummary->f[ctl];
  
                DBGStrOut(("Str_out():Obtained lpSummary\n"));
  
                /* Check if it's even a soft font */
                if (lpSummary->indDLName != -1)
                {
                    Softfont = TRUE;
  
                    DBGStrOut(("Str_out():indDLName != -1\n"));
                    DBGStrOut(("lpSummary->hCharDL=%d\n", lpSummary->hCharDL));
  
                    /* If there's no download info, we haven't downloaded! */
                    if (lpSummary->hCharDL)
                    {
                        if (lpCharDL = (LPCHARDL)GlobalLock(lpSummary->hCharDL))
                        {
                            DBGStrOut(("lpCharDL=%lp\n", lpCharDL));
                            DBGStrOut(("count=%d\n", count));
  
                            /* Scan string until we find a char that's not down */
                            while ((char_ind < count) && (already_down == TRUE))
                            {
                                DBGStrOut(("Str_out(): char_ind=%d: ", char_ind));
                                DBGStrOut(("Str_out(): lpString[char_ind]=%d: ",
                                lpString[char_ind]));
  
                                if ((lpCharDL->LastCode > lpString[char_ind]) &&
                                    (!ISTRUE(lpCharDL->CharDown,lpString[char_ind])))
                                {
                                    already_down = FALSE;
  
                                    DBGStrOut(("!ISTRUE\n"));
                                }
                                else
                                {
                                    DBGStrOut(("ISTRUE\n"));
  
                                    char_ind++;
                                }
                            }
  
                            GlobalUnlock(lpSummary->hCharDL);
                        }
                        else
                            already_down = FALSE;
                    }
                    else
                        already_down = FALSE;
                }
#ifdef DEBUG
                else
                    DBGStrOut(("Str_out():indDLName == -1\n"));
#endif
  
                unlockFontSummary(lpDevice);
            }
            else
                already_down = FALSE;
  
#ifdef DEBUG
            if (already_down)
                DBGStrOut(("Str_out():already_down == TRUE\n"));
            else
                DBGStrOut(("Str_out():already_down == FALSE\n"));
  
            DBGStrOut(("str_out(): fontind to print is %d\n", ctl));
#endif
  
            /* If the select sequence has changed or the font is scalable and */
            /* the height has changed or some characters aren't downloaded... */
            if ((lpDevice->epECtl != ctl) ||
                ((((LPPRDFONTINFO)lpFont)->scaleInfo.scalable) &&
                (lpFont->dfPixHeight != lpDevice->epLastSize)) ||
                (!(already_down)))
  
            {
                char temp[80],
  
                /*** Some variables to use while splicing the correct size ***/
                /*** into the escape sequence.                             ***/
                temp_repl[80],
                size_str[80];
                long size;
                short adv_amt,
                pound_ind;
  
                /* If the font is scalable, then force the select sequence to */
                /* be sent again, since the size may have changed while       */
                /* remaining at the same font summary index.                  */
                if (already_down && LoadFontString(lpDevice, (LPSTR)temp,
                    sizeof (temp), fontescape, ctl, lpFont->dfPixHeight))
                {
                    DBGStrOut(("Str_out(): escape is %ls\n", (LPSTR)temp));
  
                    /*** If the escape sequence is for a scalable font, then ***/
                    /*** replace #HEIGHT or #PITCH with the correct value.   ***/
  
                    pound_ind = lstrind ((LPSTR) temp, '#');
                    if (pound_ind >= 0)
                    {
                        if (temp[pound_ind+1] == 'H')
                        {
                            /*** variable-pitch ***/
                            size = CalcPtSize ((long) (lpFont->dfPixHeight),
                            (long) (lpFont->dfVertRes));
                            adv_amt = 7;
                        }
                        else
                        {
                            /*** fixed pitch ***/
                            /*** Changed last argument from PixWidth to AvgWidth ***/
                            size = labdivc ((long) (lpFont->dfHorizRes),
                            (long) 100,
                            (long) (lpFont->dfAvgWidth));
                            adv_amt = 6;
                        }
  
                        /*** put the size into a string ***/
                        MakeEscSize (size, (LPSTR) size_str);
  
                        /*** copy the source sequence up to the # sign ***/
                        hplstrcpyn ((LPSTR) temp_repl, (LPSTR) temp, pound_ind);
  
                        /*** terminate copy sequence ***/
                        temp_repl[pound_ind] = '\0';
  
                        /*** add size string to the copy sequence ***/
                        lstrcat ((LPSTR) temp_repl, (LPSTR) size_str);
  
                        /*** skip over the #HEIGHT or #PITCH and copy rest of ***/
                        /*** source sequence.                                 ***/
                        lstrcat ((LPSTR) temp_repl, (LPSTR) &temp[pound_ind+adv_amt]);
  
                        /*** replace source sequence with new sequence ***/
                        lstrcpy ((LPSTR) temp, (LPSTR) temp_repl);
                    }
  
  
  
                    /*  Send escape sequence to set font.
                    */
                    err = myWrite(lpDevice, (LPSTR)temp, lstrlen((LPSTR)temp));
  
                    /* Apply publishing translation */
                    if (lpDevice->epPubTrans)
                    {
                        LPSTR temp2 = (LPSTR)temp;
  
                        /* Select secondary font */
                        while (*temp2)
                        {
                            if (*temp2 == '(')
                                *temp2 = ')';
  
                            temp2++;
                        }   /* while */
  
                        myWrite(lpDevice, (LPSTR)temp, lstrlen((LPSTR)temp));
  
                        /* Select CG Times (Desktop) if soft or fixed pitch */
                        /* but keep bold/italic from primary font           */
                        if ((Softfont) || (!(lpFont->dfPitchAndFamily & 0x1)))
                        {
                            myWrite(lpDevice, DT_CGT_SECOND);
  
                            /* If fixed pitch and pixheight is defined... */
                            if ((!(lpFont->dfPitchAndFamily & 0x1)) &&
                                (lpFont->dfPixHeight))
                            {
                                /*** CG Times is variable-pitch ***/
                                size = CalcPtSize ((long) (lpFont->dfPixHeight),
                                (long) (lpFont->dfVertRes));
  
                                /*** put the size into a string ***/
                                MakeEscSize (size, (LPSTR) size_str);
  
                                lstrcpy ((LPSTR) temp, (LPSTR) "\033)s");
  
                                /*** add size string to the copy sequence ***/
                                lstrcat ((LPSTR) temp, (LPSTR) size_str);
  
                                lstrcat ((LPSTR) temp, (LPSTR) "V");
  
                                myWrite(lpDevice, (LPSTR)temp,
                                lstrlen((LPSTR)temp));
                            }
                        }
                        else
                            /* Force symset to DeskTop */
                            myWrite(lpDevice, DESKTOP_SECOND);
                    }
  
                }
                /*** Added pixel height to arg list for size esc string ***/
                else if (!DownLoadSoft(lpDevice, ctl, lpFont->dfPixHeight,
                    (LPSTR)((long)lpString + char_ind),(count - char_ind)))
                {
                    if (!(lpDevice->epFontSub))
                    {
                        /* send wrning message
                        */
                        WarningMsg(lpDevice, SOFT_LIMIT);
                        lpDevice->epFontSub = TRUE;
                    }
  
                    /*send escape for default courier
                    */
                    err = myWrite(lpDevice, FONT_DEFAULT);
                }
  
                lpDevice->epLastSize = lpFont->dfPixHeight;
  
                lpDevice->epECtl = ctl;
                lpDevice->epCurTTFont = -1;
            }
        }
    }   /* if (real) ... */
  
    /*  Get width of string -- if it is variable pitch, then load
    *  the width table and build up the widths.  If it is fixed pitch,
    *  or we fail to load the width table, then use dfPixWidth.
    *  This code is repeated in GetCharWidth(), if you change it here,
    *  then change it there.
    *
    *  For ExtTextOut(), expand the array widths into two arrays -- an
    *  array of true character widths followed by an array of delta-x
    *  moves.
    */
  
    if ((lpFont->dfPitchAndFamily & 0x1) &&
        (widthptr = (short far *)LoadWidthTable(lpDevice, lpFont)))
    {
  
        /*  NOTE: The width table has been built in Windows ANSI order,
        *  so we get the widths by referencing the original string,
        *  not the translated string.
        */

        for (lpw=lpWidths, lpdx=&lpWidths[count], i=0; i < count;
            ++i, ++lpw, ++lpdx)
        {
            DBGtrace(("In str_out: width loop: scal= %d, wspac= %d, PH= %d, MU= %d\n",
            ((LPPRDFONTINFO)lpFont)->scaleInfo.scalable, widthptr[0],
            lpFont->dfPixHeight, ((LPPRDFONTINFO)lpFont)->scaleInfo.emMasterUnits));
  
            thisChar = (BYTE)lpString[i];
  
            if ((thisChar == (BYTE)0xA0) && (widthptr[(BYTE)' ' - dfFirstChar] == 0))
            {
                /*  Detect fixed space and return width of normal space.
                */
                /*** If scalable, assign scaled width, else raw width ***/
                width = (((LPPRDFONTINFO)lpFont)->scaleInfo.scalable &&
                ((!(lpFont->dfType & TYPE_TRUETYPE)))) ?
                ScaleWidth ((long) widthptr[(BYTE)' ' - dfFirstChar],
                (long) ((LPPRDFONTINFO)lpFont)->scaleInfo.emMasterUnits,
                (long) lpFont->dfPixHeight,
                (long) lpFont->dfVertRes) :
                widthptr[(BYTE)' ' - dfFirstChar];
  
            }
  
            else 
                if ((thisChar >= dfFirstChar) && (thisChar <= dfLastChar))
                {
                    if ((lpDevice->epPubTrans) && 
                        ((thisChar > 146) && (thisChar < 152)) && 
                        (lpFont->dfCharSet == 0))
                    {
                        switch (thisChar)
                        {
                            case 147: 
                                    width = ScaleWidth (4554L, 8782L,
                                    (long) lpFont->dfPixHeight,
                                    (long) lpFont->dfVertRes);
                                break;
                                    
                            case 148: 
                                    width = ScaleWidth (4554L, 8782L,
                                    (long) lpFont->dfPixHeight,
                                    (long) lpFont->dfVertRes);
                                break;
  
                            case 149: 
                                    width = ScaleWidth (4391L, 8782L,
                                    (long) lpFont->dfPixHeight,
                                    (long) lpFont->dfVertRes);
                                break;
  
                            case 150: 
                                    width = ScaleWidth (4391L, 8782L,
                                    (long) lpFont->dfPixHeight,
                                    (long) lpFont->dfVertRes);
                                break;
  
                            case 151: 
                                    width = ScaleWidth (7806L, 8782L,
                                    (long) lpFont->dfPixHeight,
                                    (long) lpFont->dfVertRes);
                                break;
                        }
                    }

                    else

                        width = (((LPPRDFONTINFO)lpFont)->scaleInfo.scalable &&
                        ((!(lpFont->dfType & TYPE_TRUETYPE)))) ?
                        ScaleWidth ((long) widthptr[thisChar - dfFirstChar],
                        (long) ((LPPRDFONTINFO)lpFont)->scaleInfo.emMasterUnits,
                        (long) lpFont->dfPixHeight,
                        (long) lpFont->dfVertRes) :
                        widthptr[thisChar - dfFirstChar];
                }
                else
  
                    width = (((LPPRDFONTINFO)lpFont)->scaleInfo.scalable &&
                    ((!(lpFont->dfType & TYPE_TRUETYPE)))) ?
                    ScaleWidth ((long) widthptr[dfDefaultChar],
                    (long) ((LPPRDFONTINFO)lpFont)->scaleInfo.emMasterUnits,
                    (long) lpFont->dfPixHeight,
                    (long) lpFont->dfVertRes) :
                    widthptr[dfDefaultChar];
  

            /*  Build an array of delta-x moves if ExtTextOut() passed
            *  in array of widths -- we subtract the true character
            *  width from the desired width (passed in) to get the
            *  amount by which we need to adjust.
            */
            if (gotdeltax)
            {
                *lpdx = *lpw;
                *lpdx -= width;
            }
            else
                *lpdx = 0;
  
            /*  Build an array of character widths used for clipping
            *  text.  This code MUST come AFTER we adjust the array
            *  of delta-x moves.
            */
            *lpw = width;
  
            #ifdef DBGdumpwidth
            DBMSG(("str_out(): width [%c%d] = %d\n",
            (char)thisChar, (WORD)thisChar, width));
            DBMSG(("str_out(): lpFont->dfPixHeight = %d\n", lpFont->dfPixHeight));
  
            #endif
        }
  
        if (!(lpFont->dfType & TYPE_TRUETYPE))
            UnloadWidthTable(lpDevice,
            ((LPPRDFONTINFO)lpFont)->indFontSummary);
    }
    else
    {
        /*** Added scaling for fixed pitch ***/
        short PixWidth;

        /* Changed the conditional to check TT first, so we can
         * use the Average Width for either case - dtk 11-91
         */
        if (!(lpFont->dfType & TYPE_TRUETYPE))
            PixWidth = ((LPPRDFONTINFO)lpFont)->scaleInfo.scalable ?
                        lpFont->dfAvgWidth :
                        lpFont->dfPixWidth;
        else  // it's TT
            PixWidth = lpFont->dfAvgWidth;
            
        size = PixWidth * count;
  
        /*  Set up the array of true character widths followed by an
         *  array of delta-x moves -- the width-getting code above for
         *  variable width characters describes what is happening.
         */
        for (lpw=lpWidths, lpdx=&lpWidths[count], i=0; i < count;
            ++i, ++lpw, ++lpdx)
        {
            if (gotdeltax)
            {
                *lpdx = *lpw;
                *lpdx -= PixWidth;
            }
            else
                *lpdx = 0;
  
            *lpw = PixWidth;
  
            #ifdef DBGdumpwidth
            DBMSG(("str_out(): fixed-pitch width = %d\n", (short)*lpw));
            #endif
        }
    }
  
    if (real)
    {
        /* Aldus change - The following line added to correct 
         * a problem with random bolding when simulating bolding.
         */

        lpDevice->epXerr = 0;
  
        /* HACK for the Z cartridge.  The fonts in the Z cartridge
         * are offset 0.017 inch to the right.  We detect if the
         * font is from the Z cartridge and adjust by shifting our
         * x position 0.017 inch to the left (this applies only to
         * the variable width fonts).
         *
         * But, we don't want to do this for the publishing 
         * chars we substitute - dtk 9-91
         */

        if ((!(lpFont->dfType & TYPE_TRUETYPE)) &&
            ((LPPRDFONTINFO)lpFont)->ZCART_hack &&
            ((thisChar < 145) || (thisChar > 151)) && 
            (lpFont->dfPitchAndFamily & 0x1))
        {
            /* Always subtract the 0.017 inches.  xmoveto() will fail if
             * xpos is < 0, but ClipRect prevents such call from being
             * made since ClipRect.left is (now) always >= 0.
             */
            if (xpos >= 6)
                xpos -= 6;
            else if (xpos > 0)
                xpos = 0;
  
            if (lpClipRect)
            {
                lmemcpy((LPSTR) &ZCARTrect, (LPSTR)lpClipRect, sizeof(RECT));
                lpClipRect = (LPRECT) &ZCARTrect;
  
                if ((lpClipRect->left -= 6) < 0)
                    lpClipRect->left = 0;
                lpClipRect->right -= 6;
            }
        }
    }
  
    SetJustification(lpDevice,lpDrawMode,&JustType,&JustifyWB,&JustifyLTR);
  
    hTransData = 0;
    lpTransTbl = 0L;
    updatex = TRUE;
    size = 0;
    shiftCH = FALSE;
    clipL = FALSE;
  

    /*  For each character...figure out the justification to be
    *  applied to the next character, then output the character
    *  provided it is within the clipping rectangle.
    */
    for (lpw=lpWidths, lpdx=&lpWidths[count], nextwidth=prevwidth=i=0;
        i < count;
        prevwidth=nextwidth, ++i, ++lpw, ++lpdx) 
    {
        thisChar = lpString[i];
        overStrike = 0;
        OutLeft = FALSE;    /* vars for breaking out of the loop if the char */
        OutRight = FALSE;   /* is outside of the clipping rect. - dtk 1-92   */
        PubChar = FALSE;

        if (lpFont->dfType & TYPE_TRUETYPE)
            goto NoTranslate;
  
        /*  Yet another HACK:  set a flag indicating that a
         *  shift-out and shift-in should embrace output of this
         *  character so as to get good looking quotes from the 
         *  ones in the ASCII set.
         *
         *  This happens in the case where the symbol set is
         *  ECMA-94 (Z1a and S2 cartridges).
         */
        shiftCH = (((LPPRDFONTINFO)lpFont)->QUOTE_hack &&
                   (thisChar == 145 || thisChar == 146) &&
                   (((LPPRDFONTINFO)lpFont)->symbolSet == epsymECMA94));
  

        if ((lpDevice->epPubTrans) && 
            ((thisChar > 146) && (thisChar < 152)) && 
            (lpFont->dfCharSet == 0))
        {
            shiftCH = TRUE;    /* Use DT symset */
            PubChar = TRUE;    /* It's a publishing char */ 

            /* Replace with appropriate desktop char and
             * Scale char widths according to CG Times DT-to-WN ratio 
             *
             * These suckers won't get downloaded because they don't 
             * exist in the WN symset font that's being "translated"             
             */
  
            switch (thisChar)
            {
                case 147: thisChar = 176;
  
                    if (lpFont->dfPitchAndFamily & 0x1)
                        lpWidths[i] = ScaleWidth (4554L, 8782L,
                        (long) lpFont->dfPixHeight,
                        (long) lpFont->dfVertRes);
                    break;
  
                case 148: thisChar = 177;
  
                    if (lpFont->dfPitchAndFamily & 0x1)
                        lpWidths[i] = ScaleWidth (4554L, 8782L,
                        (long) lpFont->dfPixHeight,
                        (long) lpFont->dfVertRes);
                    break;
  
                case 149: thisChar = 180;
  
                    if (lpFont->dfPitchAndFamily & 0x1)
                        lpWidths[i] = ScaleWidth (4391L, 8782L,
                        (long) lpFont->dfPixHeight,
                        (long) lpFont->dfVertRes);
                    break;
  
                case 150: thisChar = 170;
  
                    if (lpFont->dfPitchAndFamily & 0x1)
                        lpWidths[i] = ScaleWidth (4391L, 8782L,
                        (long) lpFont->dfPixHeight,
                        (long) lpFont->dfVertRes);
                    break;
  
                case 151: thisChar = 171;
  
                    if (lpFont->dfPitchAndFamily & 0x1)
                        lpWidths[i] = ScaleWidth (7806L, 8782L,
                        (long) lpFont->dfPixHeight,
                        (long) lpFont->dfVertRes);
                    break;
            }
        }
  
        if ((thisChar < dfFirstChar) || (thisChar > dfLastChar))
        {
            /*  Character out of range, use default.
             */
            if (thisChar != (BYTE)0xA0)
                thisChar = lpFont->dfDefaultChar + lpFont->dfFirstChar;
        }
        else if ((thisChar > (BYTE)0x7F) && 
                 (PubChar == FALSE))
        {
            /*  Only translate ECMA-94 or US-ASCII.     
             *  Reverse translate Roman-8, if necessary.
             *
             *  And only do it if it ain't a pubtrans char - dtk
             */

            /* Re-added support for the old bitmap cartridge fonts
             * for Europe customers.  Bitmap Roman8 cartridge
             * characters will be remapped to Windows symbol set
             * so the screen will closely match the printed characters. - jcs 
             * Made sure not to re-translate a SETCHARSET pblshing char - dtk
             */
            if ((((LPPRDFONTINFO)lpFont)->symbolSet == epsymUSASCII) ||
                (((LPPRDFONTINFO)lpFont)->symbolSet == epsymECMA94)  ||
                (((LPPRDFONTINFO)lpFont)->symbolSet == epsymGENERIC7)||
                ((((LPPRDFONTINFO)lpFont)->symbolSet == epsymRoman8) &&
                (((LPPRDFONTINFO)lpFont)->scaleInfo.scalable == 0)))
            {
               /*  We encountered a character which must be translated, so
                *  we'll load in the translation table from the resources.
                */
                if (!lpTransTbl)
                {
                    if (!(hTransData = GetTransTable(hLibInst,
                                        &lpTransTbl, 
                                        ((LPPRDFONTINFO)lpFont)->symbolSet)))
                    {
                        goto backout1;
                    }
                }
  
                /*  Pick up the replacement character and its overstrike.
                 */
                j = ((WORD)lpString[i] - TRANS_MIN) * 2;
                thisChar = lpTransTbl[j];
                overStrike = lpTransTbl[j+1];
  
                if (real)
                    DownLoadSoft(lpDevice, ctl, lpFont->dfPixHeight,
                    (LPSTR)&lpTransTbl[j], (lpTransTbl[j+1] ? 2 : 1));
            }
        }
  
        NoTranslate:
         
        if (JustType == justifyletters)
            nextwidth = GetJustBreak(&JustifyLTR, JustType);
        else
        {
            nextwidth = JustifyLTR.extra;
  
            if ((JustType != fromdrawmode) && JustifyLTR.count &&
                (++JustifyLTR.ccount > JustifyLTR.count))
            {
                /*  Justification should drop off after we've
                *  output count characters.
                */
                nextwidth = 0;
            }
        }
  
        if (thisChar == dfBreakChar)
            nextwidth += GetJustBreak(&JustifyWB, JustType);
  
        /*  Extended text out, add in the per-character
        *  adjustment.
        */
        nextwidth += *lpdx;
                                        
        if (real)
        {
            /* Moved this stuff to the switch stmt below 
             * and jcs added clipping on rotatoion. - dtk 1-92
             */

//            if (lpClipRect)
//            {
//              /*  Do not output any text outside of the
//               *  clipping rectangle.
//               *
//               *  added nextwidth to righthand test  5 Oct 1989  clarkc
//               */
//                      
//               if ((xpos + prevwidth) < lpClipRect->left)
//               {
//                   xpos+=*lpw + prevwidth;
//                   continue;
//               }
//               else if((xpos+ *lpw + nextwidth) > lpClipRect->right)
//                      break;
//            }
  

            /* Bug fix #54 - moved "switch (xydirection) ..." here.  It was previously
             * outside this for loop which caused two xmoveto's to occure in some
             * situations.  SJC
             *
             * Changed left and right edge clipping to include
             * text that will be rotated.  Fix for MS Bug #8783.
             * 12-12-91  jcs  
             */
            switch (xydirection) 
            {
                case POSX: 
                    if (lpClipRect)
                    {
                        if ((xpos + prevwidth) < lpClipRect->left)
                        {
                            xpos += *lpw + prevwidth;
                            OutLeft = TRUE;
                            break;
                        }
                        else if((xpos + *lpw + nextwidth) > lpClipRect->right)
                        {
                            OutRight = TRUE;
                            break;
                        }
                    }
                    if(updatex) 
                    {
                        xmoveto(lpDevice, xpos);
                        ymoveto(lpDevice, ypos + lpFont->dfAscent);
                    }
                    break;

                case POSY:
                    if (lpClipRect)
                    {
                        if ((ypos - prevwidth) > lpClipRect->bottom)
                        {
                            xpos += *lpw + prevwidth;
                            OutLeft = TRUE;
                            break;
                        }
                        else if((ypos - (*lpw + nextwidth)) < lpClipRect->top)
                        {
                            OutRight = TRUE;
                            break;
                        }
                    }
                    if(updatex) 
                    {
                        xmoveto(lpDevice, xpos + lpFont->dfAscent);
                        ymoveto(lpDevice, ypos);
                        myWrite(lpDevice, LANDSCAPE_DIR);
                    }
                    break;

                case NEGX: 
                    if (lpClipRect)
                    {
                        if ((xpos - prevwidth) > lpClipRect->right)
                        {
                            xpos += *lpw + prevwidth;
                            OutLeft = TRUE;
                            break;
                        }
                        else if((xpos - (*lpw + nextwidth)) < lpClipRect->left)
                        {
                            OutRight = TRUE;
                            break;
                        }
                    }
                    if(updatex) 
                    {
                        xmoveto(lpDevice, xpos);
                        ymoveto(lpDevice, ypos - lpFont->dfAscent);
                        myWrite(lpDevice, NEGPORT_DIR);
                    }
                    break;

                case NEGY: 
                    if (lpClipRect)
                    {
                        if ((ypos + prevwidth) < lpClipRect->top)
                        {
                            xpos += *lpw + prevwidth;
                            OutLeft = TRUE;
                            break;
                        }
                        else if((ypos + *lpw + nextwidth) > lpClipRect->bottom)
                        {
                            OutRight = TRUE;
                            break;
                        }
                    }
                    if(updatex) 
                    {
                        xmoveto(lpDevice, xpos - lpFont->dfAscent);
                        ymoveto(lpDevice, ypos);
                        myWrite(lpDevice, NEGLANSC_DIR);
                    }
                    break;

                default:
                    if(updatex) 
                    {
                        xmoveto(lpDevice, xpos);
                        ymoveto(lpDevice, ypos + lpFont->dfAscent);
                    }
                    break;

            } /* switch */

            /* if the character was outside to the left, loop until we get
             * a char inside the clip rect.  If it was outside to the right,
             * we're done for the line, so break out. - dtk 1-92
             */
            if (OutLeft)
            {
                clipL = TRUE;
                continue;
            }
            else if (OutRight)
                break;

            /*  Advance cursor if necessary.
             */
            if (prevwidth)
                err = RelXMove(lpDevice, prevwidth);

            /* Turn on underline & strikeout if necessary 
             */
            if (updatex)
            {
                err = StartStyle(lpDevice, lpXform, lpFont);
                updatex = FALSE;
            }
  
            /*  Output the character.
             */
            if (shiftCH)
            {
                myWrite(lpDevice, SHIFT_OUT);
                err = char_out(lpDevice, thisChar, prevwidth, overStrike);
                myWrite(lpDevice, SHIFT_IN);
            }
            else
                err = char_out(lpDevice, thisChar, prevwidth, overStrike);

        } /* if (real) */
  
        /* If we got this far, the character was actually output,
         * so adjust the length of the string.  If we had clipped the
         * char on the left, don't add in prevwidth so the underline and
         * strike out will line up correctly. - dtk 1-92
         */
         if (clipL)
         {
            size += *lpw;
            clipL = FALSE;
         }
        else
            size += *lpw + prevwidth;
  
        /* update current position
         */
        switch (xydirection) 
        {
            case POSX: 
                xpos += *lpw + prevwidth;
                break;
            case POSY: 
                ypos -= *lpw + prevwidth;
                break;
            case NEGX: 
                xpos -= *lpw +prevwidth;
                break;
            case NEGY: 
                ypos += *lpw + prevwidth;
                break;
            default: DBMSG(("found undefined xydirection\n"));
        }
    } /* for */
  
    /*  If we loaded up a translation table, unlock it now.  The
     *  table is compiled as discardable so Windows will free it
     *  if it needs the space.
     */
    if (hTransData)
    {
        GlobalUnlock(hTransData);
        lpTransTbl = 0L;
    }
  
    if (real)
    {
        /* We really output the line, end any special styles
         * and update our record of xpos.
         * Must always reset to portrate direction if not 
         * in portrate - sjc
         */
        err = EndStyle(lpDevice, lpXform, lpFont, size);

        if(xydirection)
            myWrite(lpDevice,PORTRATE_DIR);

        if((xydirection == POSY) || (xydirection == NEGY))
            lpDevice->epCury = ypos;
        else
            lpDevice->epCurx = xpos;
  
        /* Reset running error (04 aug 89 peterbe)
         */
        lpDevice->epXerr = 0;
    }
    else if (!isWhite(lpDrawMode->TextColor, lpDevice->epTxWhite))
    {
        /*  Justification information gets updated if we
        *  are getting the extent of the line, but not if
        *  we are outputting the line.
        *
        *  If the text color is white, then str_out was called
        *  just to return a valid width (count < 0), but this
        *  call originated as a text output, so do not update
        *  the justification parameters.
        */
        lpDrawMode->BreakErr = JustifyWB.err;
  
        if (JustType != fromdrawmode)
        {
            lpDevice->epJustWB.err = JustifyWB.err;
            lpDevice->epJustWB.ccount = JustifyWB.ccount;
            lpDevice->epJustLTR.err = JustifyLTR.err;
            lpDevice->epJustLTR.ccount = JustifyLTR.ccount;
        }
    }


#ifdef WIN31

    /* check if we need to do underline or strikeout for
     * truetype fonts only..
     */
    if ((lpFont->dfType & TYPE_TRUETYPE) && 
        ((lpXform->ftUnderline) || (lpXform->ftStrikeOut)) &&
        (real))
    {
        short int rule_pos;
        short int rule_height;
        short int rule_length = size;

        /* Calculate the width, height, and position of 
         * the rule and call RuleIt to print it. 
         * Note: the position is negated for underline since
         * we have to move down the page to print it.
         */
        if (lpXform->ftUnderline) 
        {
            rule_height = ((LPSCALABLEFONTINFO)lpFont)->erUnderlineThick;
            rule_pos = -(((LPSCALABLEFONTINFO)lpFont)->erUnderlinePos - 
                         lpFont->dfAscent);

            RuleIt(lpDevice, rule_height, rule_length, rule_pos);
        }

        if (lpXform->ftStrikeOut)
        {
            rule_height = ((LPSCALABLEFONTINFO)lpFont)->erStrikeoutThick;
            rule_pos = (lpFont->dfAscent -
                        ((LPSCALABLEFONTINFO)lpFont)->erStrikeoutPos); 

            RuleIt(lpDevice, rule_height, rule_length, rule_pos);
        }
    }

#endif


    /*  The "if" statement below has been moved to AFTER the call to
     *  EndStyle().  This helps synthesized strikeout work.
     *
     *  Add in the adjustment to the last character, so the
     *  length of the line will correctly reflect justification
     *  and ExtTextOut() adjustments.
     */
    if (nextwidth)
    {
        size += nextwidth;
  
        /*  If we're doing strikeout or underline, we have to actually
         *  move the cursor so the lines end up in the right place.
         *  Do this only for device fonts - dtk 9/91
         */
        if ((lpFont->dfType & TYPE_DEVICE) && 
             (real) && 
             (lpXform->ftStrikeOut || lpXform->ftUnderline))
            RelXMove(lpDevice, nextwidth);
    }
  
    status = MAKELONG(lpFont->dfPixHeight, size);
  
    backout1:
    /*  Unlock our work area for character widths -- we'll
    *  lock it down the next time we need it, and delete it
    *  when Disable() is called.
    */
    if (lpWidths && lpDevice->epHWidths)
    {
        GlobalUnlock(lpDevice->epHWidths);
        lpWidths = 0L;
    }
  
    backout0:
  
#ifdef DEBUG_FUNCT
    DB(("Exiting str_out\n"));
#endif
  
    return (status);
}   // str_out()

//  Took this out because it was based on an old assumption of 
//  how text is to be vertically clipped - dtk 10-91
//  
//  /* This function checks if we are within the clipping rectangle.
//      It is given the x and y coordinates, and the clipping rect. cordinates.
//      It returns a short int with bits 1, 2, 4, or 8 set indicating:
//
//      9        1         3
//          -------------
//      8   |           |  2
//          |           |
//          -------------
//      12       4         6
//
//  A combination of these bits (i.e. 1 & 2 would indicate x,y cordinate is
//  out of the clipping rect. on the top and the right - SJC 5/1/90.
//  */
//int check_clip(x,y,lpClipRect)
//short x,y;
//LPRECT lpClipRect;
//{
//    int err;
//  
//#ifdef DEBUG_FUNCT
//    DB(("Entering check_clip\n"));
//#endif
//  
//    err=0;
//  
//    if(y<lpClipRect->top)    err|=1;
//    if(x>lpClipRect->right)  err|=2;
//    if(y>lpClipRect->bottom) err|=4;
//    if(x<lpClipRect->left)   err|=8;
//    /*sjcDBMSG(("In check_clip, err=%d\n",err)); */
//  
//#ifdef DEBUG_FUNCT
//    DB(("Exiting check_clip\n"));
//#endif
//  
//    return(err);
//}
  
  
/*  char_out()
*/
static int char_out(lpDevice, c, width, overstrike)
LPDEVICE lpDevice;
char c;
short width;
BYTE overstrike;
{
    int err = SUCCESS;
    char buf[2];
  
#ifdef DEBUG_FUNCT
    DB(("Entering char_out\n"));
#endif
  
//    DBGtrace(("In char_out,c=%c\n", c));
  
    /*  Advance cursor if necessary.
    */
//    if (width)
//        err = RelXMove(lpDevice, width);
  
    /*  Trap fixed space (160) and turn into normal space.
     *  Only if there is no width for it - dtk
     */
    if (((BYTE)c == (BYTE)0xA0) && (width == 0))
        c = ' ';
  
    err = myWrite(lpDevice, (LPSTR) &c, 1);
  
    if (overstrike)
    {
        myWrite(lpDevice, PUSH_POSITION);
        buf[0] = '\010';
        buf[1] = overstrike;
        err = myWrite(lpDevice, (LPSTR)buf, 2);
        myWrite(lpDevice, POP_POSITION);
    }
  
#ifdef DEBUG_FUNCT
    DB(("Exiting char_out\n"));
#endif
  
    return (err);
  
}   // char_out()
  
/*  RelXMove()
*
*  Send out relative cursor positioning.
*/
static int RelXMove(lpDevice, xmove)
LPDEVICE lpDevice;
short xmove;
{
    register short n, m = 0;
    ESCtype escape;
    int err = SUCCESS;
  
#ifdef DEBUG_FUNCT
    DB(("Entering RelXMove\n"));
#endif
  
    xmove *= 12;
    xmove += lpDevice->epXerr;
    lpDevice->epXerr = xmove % 5;
    xmove /= 5;
  
    if (lpDevice->epXerr > 0)
    {
        ++xmove;
        lpDevice->epXerr -= 5;
    }
  
    if (xmove)
    {
        /* #define HP_HCP   '&', 'a', 'H'
        */
        escape.esc = '\033';
        escape.start1 = '&';
        escape.start2 = 'a';
        if (xmove >= 0)
            escape.num[m++] = '+';
  
        n = itoa(xmove, &escape.num[m]);
        escape.num[n+m] = 'H';
        err = myWrite(lpDevice, (LPSTR) &escape, n+m+4);
    }
  
#ifdef DEBUG_FUNCT
    DB(("Exiting RelXMove\n"));
#endif
  
    return (err);
  
}   // RelXMove()
  
/*  StartStyle()
*/
static int StartStyle(lpDevice, lpXform, lpFont)
LPDEVICE lpDevice;
LPTEXTXFORM lpXform;
LPFONTINFO lpFont;
{
    int err = SUCCESS;
  
#ifdef DEBUG_FUNCT
    DB(("Entering StartStyle\n"));
#endif
  
    if ((lpFont->dfType & TYPE_DEVICE) && (lpXform->ftUnderline))
        err = myWrite(lpDevice, (LPSTR)HP_UNDERLINE_ON);

#ifdef DEBUG_FUNCT
    DB(("Exiting StartStyle\n"));
#endif
  
    return (err);
  
}   // StartStyle()
  
/*  EndStyle()
*/
static int EndStyle(lpDevice, lpXform, lpFont, size)
LPDEVICE lpDevice;
LPTEXTXFORM lpXform;
LPFONTINFO lpFont;
short size;
{
    ESCtype escape;
    int err;
  
#ifdef DEBUG_FUNCT
    DB(("Entering EndStyle\n"));
#endif
  
    /* Strike-out the line.
     * added device font dependancy - dtk 9/91
     */
    if ((lpFont->dfType & TYPE_DEVICE) && 
        (lpXform->ftStrikeOut) && 
        (size > 0))
    {
        /* Took out the rule code and added a call to Ruleit - dtk 11/91
         */
        short int rule_height = STRIKEOUT_THICKNESS;
        short int rule_pos = lpXform->ftHeight / 5 + rule_height;
        short int rule_length = size;

        err = RuleIt(lpDevice, rule_height, rule_length, rule_pos);
    }

    /* added device font dependancy - dtk 9/91
     */
    if ((lpFont->dfType & TYPE_DEVICE) && (lpXform->ftUnderline))
        err = myWrite(lpDevice, (LPSTR)HP_UNDERLINE_OFF);

#ifdef DEBUG_FUNCT
    DB(("Exiting EndStyle\n"));
#endif
  
    return (err);
  
}   // EndStyle()
  
/*  SetJustification()
*
*  Set up the justification records and return TRUE if justification
*  will be required on the line.  The justification values can come
*  from two places:
*
*      1. Windows GDI SetTextCharacterExtra() and
*          SetTextJustification(), which handle only
*          positive justification.
*      2. The SETALLJUSTVALUES escape, which handles negative
*          and positive justification.
*
*  Windows' justification parameters are stored in the DRAWMODE struct,
*  while SETALLJUSTVALUES stuff comes from our DEVICE struct.
*/
static BOOL SetJustification(lpDevice, lpDrawMode, lpJustType,
lpJustifyWB, lpJustifyLTR)
LPDEVICE lpDevice;
LPDRAWMODE lpDrawMode;
LPJUSTBREAKTYPE lpJustType;
LPJUSTBREAKREC lpJustifyWB;
LPJUSTBREAKREC lpJustifyLTR;
{
  
#ifdef DEBUG_FUNCT
    DB(("Entering SetJustification\n"));
#endif
  
    if ((*lpJustType = lpDevice->epJust) == fromdrawmode)
    {
        /*  Normal Windows justification.
        */
        if (lpDrawMode->TBreakExtra)
        {
            lpJustifyWB->extra = lpDrawMode->BreakExtra;
            lpJustifyWB->rem = lpDrawMode->BreakRem;
            lpJustifyWB->err = lpDrawMode->BreakErr;
            lpJustifyWB->count = lpDrawMode->BreakCount;
            lpJustifyWB->ccount = 0;
        }
        else
        {
            lpJustifyWB->extra = 0;
            lpJustifyWB->rem = 0;
            lpJustifyWB->err = 1;
            lpJustifyWB->count = 0;
            lpJustifyWB->ccount = 0;
        }
  
        lpJustifyLTR->extra = lpDrawMode->CharExtra;
        lpJustifyLTR->rem = 0;
        lpJustifyLTR->err = 1;
        lpJustifyLTR->count = 0;
        lpJustifyLTR->ccount = 0;
    }
    else
    {
        /*  SETALLJUSTVALUES -- the records were filled when the
        *  escape was called, now make local copies.
        */
        lmemcpy((LPSTR)lpJustifyWB, (LPSTR) &lpDevice->epJustWB,
        sizeof(JUSTBREAKREC));
        lmemcpy((LPSTR)lpJustifyLTR, (LPSTR) &lpDevice->epJustLTR,
        sizeof(JUSTBREAKREC));
    }
  
#ifdef DEBUG_FUNCT
    DB(("Exiting SetJustification\n"));
#endif
  
    /*  Advise the caller as to whether or not justification
    *  adjustments will be required.
    */
    if (lpJustifyWB->extra || lpJustifyWB->rem || lpJustifyWB->count ||
        lpJustifyLTR->extra || lpJustifyLTR->rem || lpJustifyLTR->count)
        return TRUE;
    else
        return FALSE;
  
}   // SetJustification()
  
/*  GetJustBreak()
*
*  Calculate the additional pixels to add/subtract from the horizontal
*  position.
*/
static short GetJustBreak (lpJustBreak, justType)
LPJUSTBREAKREC lpJustBreak;
JUSTBREAKTYPE justType;
{
    short adjust = lpJustBreak->extra;
  
#ifdef DEBUG_FUNCT
    DB(("Entering GetJustBreak\n"));
#endif
  
    /*  Update the err value and add in the distributed adjustment.
    */
    if ((lpJustBreak->err -= lpJustBreak->rem) <= 0)
    {
        ++adjust;
        lpJustBreak->err += (short)lpJustBreak->count;
    }
  
    if ((justType != fromdrawmode) && lpJustBreak->count &&
        (++lpJustBreak->ccount > lpJustBreak->count))
    {
        /*  Not at a valid character position, return zero adjustment.
        */
        adjust = 0;
    }
  
#ifdef DEBUG_FUNCT
    DB(("Exiting GetJustBreak\n"));
#endif
  
    return (adjust);
}   // GetJustBreak()
  
/*  DoubleSizeCopy()
*
*  Allocate a structure twice the size of the passed-in struct, and
*  copy the passed-in struct into the first half.
*/
static LPSTR DoubleSizeCopy(lpHSrc, lpSrc, len, size)
LPHANDLE lpHSrc;
LPSTR lpSrc;
short len;
short size;
{
    LPSTR lpDst = 0L;
    DWORD allocSize = 0;
  
#ifdef DEBUG_FUNCT
    DB(("Entering DoubleSizeCopy\n"));
#endif
  
    /*  Size of array must be atleast MAXNUM_WIDTHS * 2.
    */
    if (len <= MAXNUM_WIDTHS)
        allocSize = MAXNUM_WIDTHS * 2 * size;
    else
    {
        allocSize = len * 2 * size;
  
        /*  Array needs to be longer than MAXNUM_WIDTHS * 2.
        *  If it already exists, attempt to lengthen it.
        */
        if (*lpHSrc && (GlobalSize(*lpHSrc) < allocSize))
        {
            GlobalFree(*lpHSrc);
            *lpHSrc = 0;
        }
    }
  
    /*  If the array does not exist, then allocate it.
    */
    if (!(*lpHSrc))
        *lpHSrc = GlobalAlloc(GMEM_MOVEABLE, allocSize);
  
    /*  Lock down the array if it exists.
    */
    if ((*lpHSrc) && (!(lpDst = GlobalLock(*lpHSrc))))
    {
        GlobalFree(*lpHSrc);
        *lpHSrc = 0;
    }
  
    /*  Initialize to all zeros.
    */
    if (lpDst)
        lmemset(lpDst, 0, len * size * 2);
  
    /*  Copy the array of widths.
    */
    if (lpDst && lpSrc)
        lmemcpy(lpDst, lpSrc, len * size);
  
#ifdef DEBUG_FUNCT
    DB(("Exiting DoubleSizeCopy\n"));
#endif
  
    return (lpDst);
}   // DoubleSizeCopy()
  
/*  isWhite()
*
*  Return TRUE if each component of pcolor has an intensity greater
*  than or equal to the value of white.
*/
static BOOL isWhite(pcolor, white)
long pcolor;
short white;
{
    BYTE r, g, b, cmp;
  
#ifdef DEBUG_FUNCT
    DB(("Entering isWhite \n"));
#endif
  
    if (white > 255)
    {
        DBGtrace(("isWhite(%ld,%d): color is not white\n", pcolor, white));
        return FALSE;
    }
  
    cmp = (BYTE)white;
    r = GetRValue(pcolor);
    g = GetGValue(pcolor);
    b = GetBValue(pcolor);
  
//    #ifdef DEBUG
//    if (r >= cmp && g >= cmp && b >= cmp)
//    { DBGtrace(("isWhite(r%d,g%d,b%d,%d): color is white\n",
//    (WORD)r, (WORD)g, (WORD)b, white)); }
//    else
//    { DBGtrace(("isWhite(r%d,g%d,b%d,%d): color is not white\n",
//    (WORD)r, (WORD)g, (WORD)b, white)); }
//    #endif
  
#ifdef DEBUG_FUNCT
    DB(("Exiting isWhite \n"));
#endif
  
    return (r >= cmp && g >= cmp && b >= cmp);
  
}   // isWhite()

/************************************************************************
 *  RuleIt() - dtk 9/91
 *
 *  This routine is called to draw the underline and/or strikeout lines
 *  on a TrueType font.  This is done using pcl rules.  
 *  We may want to look at using this for pcl fonts as well, if we can get 
 *  the position and thickness from the typeface???
 *
 *************************************************************************/

static BOOL RuleIt(lpDevice, rheight, rlen, vpos)
LPDEVICE lpDevice;
short int rheight;
short int rlen;
short int vpos;
{
    ESCtype escape;
    int err, esclen;

    /* save the current position
     */
    err = myWrite(lpDevice, (LPSTR)PUSH_POSITION);

    /* move to the beginning of the text line
     */
    err = myWrite(lpDevice, (LPSTR) &escape,
                    MakeEscape(&escape, DOT_HCP, -rlen));

    /* Move to the verical position.
     *
     * NOTE: since y is at the baseline, we need to move down the 
     * page (positive direction) to print an underline, so append 
     * a + sign to the escape sequence so the printer does not do 
     * an absolute move. Otherwise, we need to move up the page to 
     * print the strikeout, and MakeEscape will appene the minus.- dtk
     */
    if (vpos > 0)
        err = myWrite(lpDevice, (LPSTR) &escape,
                      MakeEscape(&escape, DOT_VCP, -vpos));
    else
    {
        esclen = MakeEscape(&escape, DOT_VCP, -vpos);
        err = myWrite(lpDevice, (LPSTR) &escape, 3);
        err = myWrite(lpDevice, (LPSTR) "+", 1);
        err = myWrite(lpDevice, (LPSTR) &(escape.num), esclen-3);
    }

    /* send the rule's dimensions and the pattern
     */
    err = myWrite(lpDevice, (LPSTR) &escape,
                    MakeEscape(&escape, DOT_HRPS, rlen));
    err = myWrite(lpDevice, (LPSTR) &escape,
                    MakeEscape(&escape, DOT_VRPS, rheight));
    err = myWrite(lpDevice, BLACK_PATTERN);

    /* restore the position and bail out
     */
    err = myWrite(lpDevice, (LPSTR)POP_POSITION);

    return(err);
  
}   // RuleIt


/************************************************************************
 *  WhiteOut() - jcs 11/91
 *
 *        This routine is used to solve MS Bug 14975.  White_out
 *    in excel does not print.  This routine sets the source as
 *    opaque then sends a white rule just before the text is sent
 *    during the textband.
 *
 *
 *************************************************************************/

static BOOL WhiteOut(lpDevice, xpos, ypos, rheight, rlength)
LPDEVICE lpDevice;
short int xpos;
short int ypos;
short int rheight;
short int rlength;
{
    short err;
    ESCtype escape;
    
    xmoveto(lpDevice, xpos);
    ymoveto(lpDevice, ypos);

    myWrite(lpDevice, WHITE_TEXT);
        
    err = myWrite(lpDevice, (LPSTR) &escape,
                    MakeEscape(&escape, DOT_HRPS, rlength));
    err = myWrite(lpDevice, (LPSTR) &escape,
                    MakeEscape(&escape, DOT_VRPS, rheight));
    err = myWrite(lpDevice, WHITE_PATTERN);
        
    err = myWrite(lpDevice, NO_WHITE_TEXT);
 
    return (err);
  
}   // WhiteOut



