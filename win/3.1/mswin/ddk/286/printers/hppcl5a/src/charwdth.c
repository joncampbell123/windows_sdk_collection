/**[f******************************************************************
* charwdth.c -
*
* Copyright (C) 1988,1989 Aldus Corporation
* Copyright (C) 1988-1990 Microsoft Corporation.
* Copyright (C) 1989,1990, 1991, Hewlett-Packard Company.
*             All rights reserved.
* Company confidential.
*
**f]*****************************************************************/
  
/**********************************************************************
*
*  20 Jul 90  SJC      Removed PCL_* - don't need to lock segments.
*  08 feb 90  VO       Added scaling for fixed pitch scalables.
*  10 jan 90  VO       Added scaling for variable pitch scalables.
*  16 oct 89   peterbe Minor change to debug #ifdef's to facilitate dumping
*          of character widths.
*  27 apr 89   peterbe Tabs are 8 spaces now.
*   1-17-89    jimmat  Added PCL_* entry points to lock/unlock data seg.
*/
  
/*
* $Header: charwdth.c,v 3.890 92/02/06 16:12:20 dtk FREEZE $
*/
  
/*
* $Log:	charwdth.c,v $
 * Revision 3.890  92/02/06  16:12:20  16:12:20  dtk (Doug Kaltenecker)
 * Win3.1 Freeze
 * 
 * Revision 3.873  92/01/10  11:26:26  11:26:26  dtk (Doug Kaltenecker)
 * Fixed char 160 remapping.
 * 
 * Revision 3.872  91/12/02  16:43:02  16:43:02  dtk (Doug Kaltenecker)
 * Changed the ifdef TT build variables to ifdef WIN31.
 * 
 * Revision 3.871  91/11/22  13:19:26  13:19:26  dtk (Doug Kaltenecker)
 * Win 3.1 Post Beta 3 version.
 * 
 * Revision 3.870  91/11/08  11:44:04  11:44:04  dtk (Doug Kaltenecker)
 * 3.1 Release Candidate 1
 * 
 * Revision 3.865  91/11/01  13:52:02  13:52:02  dtk (Doug Kaltenecker)
 * Beta release for Windows 3.1
 * 
 * Revision 3.862  91/10/25  13:47:22  13:47:22  dtk (Doug Kaltenecker)
 * BETA
 * 
 * Revision 3.860  91/10/23  09:48:50  09:48:50  dtk (Doug Kaltenecker)
 * WinWird Release
 * 
 * Revision 3.852  91/10/09  14:59:50  14:59:50  dtk (Doug Kaltenecker)
 * BETA
 * 
 * Revision 3.850  91/10/04  16:50:04  16:50:04  dtk (Doug Kaltenecker)
 * WINWORD RELEASE CANDIDATE
 * 
 * Revision 3.840  91/09/28  14:17:21  14:17:21  dtk (Doug Kaltenecker)
 * RELEASE CANDIDATE 3 FOR WINWORD
 * 
 * Revision 3.830  91/09/18  16:33:38  16:33:38  dtk (Doug Kaltenecker)
 * RELEASE
 * 
 * Revision 3.823  91/09/18  16:21:10  16:21:10  dtk (Doug Kaltenecker)
 * Changed getcharwidth for publishing chars.
 * 
 * Revision 3.822  91/09/16  10:34:17  10:34:17  dtk (Doug Kaltenecker)
 * WINWORD RELEASE CANDIDATE 2 FOR WIN30.
 * 
 * Revision 3.821  91/09/16  10:14:02  10:14:02  dtk (Doug Kaltenecker)
 * Added code to recalculate widths for publishing chars when SETCHARSET
 * is called by the application.
 * .,
 * 
 * Revision 3.820  91/09/06  14:12:17  14:12:17  dtk (Doug Kaltenecker)
 * RELEASE CANDIDATE FOR WINWORD 2.0
 * 
 * Revision 3.812  91/08/22  14:32:13  14:32:13  dtk (Doug Kaltenecker)
 * BETA
 * 
 * Revision 3.807  91/08/08  10:31:29  10:31:29  dtk (Doug Kaltenecker)
 * PREBETA3
 * 
 * Revision 3.802  91/07/22  12:54:30  12:54:30  oakeson (Ken Oakeson)
 * BETA
 * 
 * Revision 3.799  91/07/02  11:51:55  11:51:55  daniels (Susan Daniels)
 * Beta
 * 
 * Revision 3.796  91/06/26  11:26:14  11:26:14  stevec (Steve Claiborne)
 * BETA
 * 
 * Revision 3.790  91/06/11  16:03:29  16:03:29  stevec (Steve Claiborne)
 * Freeze
 * 
 * Revision 3.786  91/06/11  15:44:32  15:44:32  dtk (Doug Kaltenecker)
 * Prettified files!
 * 
* Revision 3.785  91/05/22  14:57:07  14:57:07  stevec (Steve Claiborne)
* Beta version to MS
*
* Revision 3.780  91/05/15  15:57:16  15:57:16  stevec (Steve Claiborne)
* Beta
*
* Revision 3.775  91/04/05  14:31:09  14:31:09  stevec (Steve Claiborne)
* Beta release to MS
*
* Revision 3.770  91/03/25  15:36:08  15:36:08  stevec (Steve Claiborne)
* maintance release
*
* Revision 3.760  91/03/12  07:52:53  07:52:53  stevec (Steve Claiborne)
* Maintance release
*
* Revision 3.755  91/03/03  07:46:13  07:46:13  stevec (Steve Claiborne)
* March 3 Freeze
*
* Revision 3.720  91/02/11  09:15:31  09:15:31  stevec (Steve Claiborne)
* Aldus version
*
* Revision 3.711  91/02/08  16:24:17  16:24:17  stevec (Steve Claiborne)
* Added debuging
*
* Revision 3.710  91/02/04  15:47:49  15:47:49  stevec (Steve Claiborne)
* Aldus freeze
*
* Revision 3.700  91/01/19  09:00:30  09:00:30  stevec (Steve Claiborne)
* Release
*
* Revision 3.685  91/01/14  15:43:24  15:43:24  stevec (Steve Claiborne)
* Freeze
*
* Revision 3.681  91/01/14  10:17:41  10:17:41  stevec (Steve Claiborne)
* Updated the copy right stmt.
*
* Revision 3.680  91/01/10  16:16:47  16:16:47  stevec (Steve Claiborne)
* Freeze
*
* Revision 3.670  90/12/14  14:54:14  14:54:14  stevec (Steve Claiborne)
* freeze for 12-14-90 ver. 3.670
*
* Revision 3.665  90/12/10  15:35:57  15:35:57  stevec (Steve Claiborne)
* Freeze
*
* Revision 3.660  90/12/07  14:50:26  14:50:26  stevec (Steve Claiborne)
* Freeze 12-7-90
*
* Revision 3.650  90/11/30  08:12:20  08:12:20  stevec (Steve Claiborne)
* Freeze 3.650, 11-30-90
*
* Revision 3.604  90/10/25  17:01:51  17:01:51  oakeson (Ken Oakeson)
* Replaced "ifdef"s with GetVersion calls
*
* Revision 3.603  90/08/29  17:41:51  17:41:51  oakeson (Ken Oakeson)
* Avoided scaling widths of TrueType fonts
*
* Revision 3.602  90/08/24  11:36:55  11:36:55  daniels ()
* message.txt
*
* Revision 3.601  90/08/14  15:23:50  15:23:50  oakeson (Ken Oakeson)
* Added TrueType support
*
* Revision 3.600  90/08/03  11:09:55  11:09:55  stevec (Steve Claiborne)
* This is the Aug. 3 release ver. 3.600
*
* Revision 3.550  90/07/27  11:31:09  11:31:09  root ()
* Experimental freeze 3.55
*
* Revision 3.540  90/07/25  12:34:40  12:34:40  stevec (Steve Claiborne)
* Experimental freeze 3.54
*
* Revision 3.521  90/07/21  10:33:18  10:33:18  stevec (Steve Claiborne)
* Removed PCL_* lock functions
*
* Revision 3.520  90/06/13  16:52:41  16:52:41  root ()
* 5_2_release
*
*
*    Rev 1.1   20 Feb 1990 15:46:26   vordaz
* Support for downloadables.
*/
  
//#define DEBUG
  
#include "generic.h"
#include "resource.h"
#define FONTMAN_UTILS
#include "fontman.h"
#include "utils.h"
#include "truetype.h"
  
  
#ifdef DEBUG
#define DBGdumpwidth
#else
#undef DBGdumpwidth
#endif
  
int far PASCAL GetCharWidth(LPDEVICE, short far *, WORD, WORD, LPFONTINFO, LPDRAWMODE, LPTEXTXFORM);
  
  
/*  GetCharWidth
*
*  Return the character widths from firstChar to lastChar.  This procedure
*  is the same as the width-getting code in str_out(), if you change it
*  there, you should change it here.
*/
int far PASCAL GetCharWidth(lpDevice, lpBuffer, firstChar, lastChar, lpFont,
lpDrawMode, lpXform)
LPDEVICE lpDevice;
short far *lpBuffer;
WORD firstChar;
WORD lastChar;
LPFONTINFO lpFont;
LPDRAWMODE lpDrawMode;
LPTEXTXFORM lpXform;
{
    short far *widthptr;
    short overhang;
    WORD dfFirstChar = lpFont->dfFirstChar;
    WORD dfLastChar = lpFont->dfLastChar;
#ifdef DEBUG_FUNCT
    DB(("Entering GetCharWidth\n"));
#endif
  
    DBMSG(("GetCharWidth(%lp): first = %c%d, last = %c%d\n",
    lpBuffer, firstChar, (WORD)firstChar, lastChar, (WORD)lastChar));
  
    if (!lpBuffer)
        return FALSE;
  
    /*  Synthesized bold.
    */
    if (lpFont->dfWeight < lpXform->ftWeight)
    {
        overhang = lpXform->ftOverhang;
        DBMSG(("GetCharWidth(): overhang for synthesized bold=%d\n", overhang));
    }
    else
        overhang = 0;
  
    /*  Get width of string -- if it is variable pitch, then load
    *  the width table and build up the widths.  If it is fixed pitch,
    *  or we fail to load the width table, then use dfPixWidth.
    */
    if ((lpFont->dfPitchAndFamily & 0x1) &&
        (widthptr = (short far *)LoadWidthTable(lpDevice,lpFont)))
    {
        for (; firstChar <= lastChar; ++firstChar, ++lpBuffer)
        {
            if ((firstChar == (BYTE)0xA0) && 
                (widthptr[(BYTE)' ' - dfFirstChar] == 0))
            {
                /*  Detect fixed space and return width of normal space.
                 *  Only if it doesn't have an associated width - dtk 1-92
                 */
  
                *lpBuffer = (((LPPRDFONTINFO)lpFont)->scaleInfo.scalable &&
                (!(lpFont->dfType & TYPE_TRUETYPE))) ?
  
                ScaleWidth ((long) widthptr[' ' - dfFirstChar],
                (long) ((LPPRDFONTINFO)lpFont)->scaleInfo.emMasterUnits,
                (long) lpFont->dfPixHeight,
                (long) lpFont->dfVertRes) :
  
                widthptr[' ' - dfFirstChar];
            }
            else 
                if ((firstChar >= dfFirstChar) && (firstChar <= dfLastChar))
                {
                    if ((lpDevice->epPubTrans) && (firstChar > 146) && 
                        (firstChar < 152) &&
                        (lpFont->dfCharSet == 0))
                    {
                        switch (firstChar)
                        {
                            case 147: 
                                    *lpBuffer = ScaleWidth (4554L, 8782L,
                                    (long) lpFont->dfPixHeight,
                                    (long) lpFont->dfVertRes);
                                break;
  
                            case 148: 
                                    *lpBuffer = ScaleWidth (4554L, 8782L,
                                    (long) lpFont->dfPixHeight,
                                    (long) lpFont->dfVertRes);
                                break;
  
                            case 149: 
                                    *lpBuffer = ScaleWidth (4391L, 8782L,
                                    (long) lpFont->dfPixHeight,
                                    (long) lpFont->dfVertRes);
                                break;
  
                            case 150: 
                                    *lpBuffer = ScaleWidth (4391L, 8782L,
                                    (long) lpFont->dfPixHeight,
                                    (long) lpFont->dfVertRes);
                                break;
  
                            case 151: 
                                    *lpBuffer = ScaleWidth (7806L, 8782L,
                                    (long) lpFont->dfPixHeight,
                                    (long) lpFont->dfVertRes);
                                break;
                        }
                    }

                    else
  
                        /*** Tetra begin ***/
                        *lpBuffer = (((LPPRDFONTINFO)lpFont)->scaleInfo.scalable &&
                        (!(lpFont->dfType & TYPE_TRUETYPE))) ?
  
                        ScaleWidth ((long) widthptr[firstChar - dfFirstChar],
                        (long) ((LPPRDFONTINFO)lpFont)->scaleInfo.emMasterUnits,
                        (long) lpFont->dfPixHeight,
                        (long) lpFont->dfVertRes) :
                        /*** Tetra end ***/
  
                        widthptr[firstChar - dfFirstChar];
                }
                else
  
                    /*** Tetra begin ***/
                    *lpBuffer = (((LPPRDFONTINFO)lpFont)->scaleInfo.scalable &&
                    (!(lpFont->dfType & TYPE_TRUETYPE))) ?
  
                    ScaleWidth ((long) widthptr[lpFont->dfDefaultChar],
                    (long) ((LPPRDFONTINFO)lpFont)->scaleInfo.emMasterUnits,
                    (long) lpFont->dfPixHeight,
                    (long) lpFont->dfVertRes) :
                    /*** Tetra end ***/
  
                widthptr[lpFont->dfDefaultChar];


            /*  Add in overhang for synthesized bold.
            */
            *lpBuffer += overhang;
  
        #ifdef DBGdumpwidth
  
            if (firstChar < 127)
                DBMSG(("<'%c',%d,%d>", firstChar,
                (WORD)firstChar, (short)*lpBuffer));
            else
                DBMSG(("<%d,%d>", (WORD)firstChar, (short)*lpBuffer));
  
            if (firstChar == lastChar)
                DBMSG(("\n"));
        #endif
        }
  
        if (!(lpFont->dfType & TYPE_TRUETYPE))
            UnloadWidthTable(lpDevice, ((LPPRDFONTINFO)lpFont)->indFontSummary);
    }
    else
    {
        /* Added scaling for fixed-pitch.
         * Overhang included in fixed pitch fonts.  
         *  - 5 Dec 1989  Clark R. Cyr 
         */

//        overhang += ((((LPPRDFONTINFO)lpFont)->scaleInfo.scalable) &&
//        (!(lpFont->dfType & TYPE_TRUETYPE))) ?
//        lpFont->dfAvgWidth :
//        lpFont->dfPixWidth;


        if (!(lpFont->dfType & TYPE_TRUETYPE))
            overhang += ((LPPRDFONTINFO)lpFont)->scaleInfo.scalable ?
                        lpFont->dfAvgWidth :
                        lpFont->dfPixWidth;
        else  // it's TT
            overhang += lpFont->dfAvgWidth;

        for (; firstChar <= lastChar; ++firstChar, ++lpBuffer)
        {
            *lpBuffer = overhang;
        }
  
    #ifdef DBGdumpwidth
        DBMSG(("GetCharWidth(): fixed-pitch width = %d\n",
        overhang));
    #endif
        /*** Tetra II end ***/
    }
  
#ifdef DEBUG_FUNCT
    DB(("Exiting GetCharWidth\n"));
#endif
    return TRUE;
}
