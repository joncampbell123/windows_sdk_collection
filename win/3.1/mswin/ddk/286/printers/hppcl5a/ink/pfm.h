/**[f******************************************************************
* pfm.h -
*
* Copyright (C) 1988,1989 Aldus Corporation
* Copyright (C) 1989-1990 Microsoft Corporation.
* Copyright (C) 1989,1990, 1991 Hewlett-Packard Company.
* All rights reserved.  Company confidential.
*
**f]*****************************************************************/
  
/*
* $Header: pfm.h,v 3.890 92/02/06 16:13:20 dtk FREEZE $
*/
  
/*
* $Log:	pfm.h,v $
 * Revision 3.890  92/02/06  16:13:20  16:13:20  dtk (Doug Kaltenecker)
 * Win3.1 Freeze
 * 
 * Revision 3.870  91/11/08  11:45:08  11:45:08  dtk (Doug Kaltenecker)
 * 3.1 Release Candidate 1
 * 
 * Revision 3.865  91/11/01  13:53:07  13:53:07  dtk (Doug Kaltenecker)
 * Beta release for Windows 3.1
 * 
 * Revision 3.862  91/10/25  13:48:26  13:48:26  dtk (Doug Kaltenecker)
 * BETA
 * 
 * Revision 3.860  91/10/23  09:50:00  09:50:00  dtk (Doug Kaltenecker)
 * WinWird Release
 * 
 * Revision 3.852  91/10/09  15:00:53  15:00:53  dtk (Doug Kaltenecker)
 * BETA
 * 
 * Revision 3.850  91/10/04  16:51:10  16:51:10  dtk (Doug Kaltenecker)
 * WINWORD RELEASE CANDIDATE
 * 
 * Revision 3.840  91/09/28  14:18:23  14:18:23  dtk (Doug Kaltenecker)
 * RELEASE CANDIDATE 3 FOR WINWORD
 * 
 * Revision 3.830  91/09/18  16:34:43  16:34:43  dtk (Doug Kaltenecker)
 * RELEASE
 * 
 * Revision 3.822  91/09/16  10:35:51  10:35:51  dtk (Doug Kaltenecker)
 * WINWORD RELEASE CANDIDATE 2 FOR WIN30.
 * 
 * Revision 3.820  91/09/06  14:13:24  14:13:24  dtk (Doug Kaltenecker)
 * RELEASE CANDIDATE FOR WINWORD 2.0
 * 
 * Revision 3.812  91/08/22  14:33:17  14:33:17  dtk (Doug Kaltenecker)
 * BETA
 * 
 * Revision 3.807  91/08/08  10:32:32  10:32:32  dtk (Doug Kaltenecker)
 * PREBETA3
 * 
 * Revision 3.802  91/07/22  12:55:41  12:55:41  oakeson (Ken Oakeson)
 * BETA
 * 
 * Revision 3.799  91/07/02  11:53:02  11:53:02  daniels (Susan Daniels)
 * Beta
 * 
 * Revision 3.796  91/06/26  11:27:23  11:27:23  stevec (Steve Claiborne)
 * BETA
 * 
 * Revision 3.790  91/06/11  16:04:36  16:04:36  stevec (Steve Claiborne)
 * Freeze
 * 
 * Revision 3.786  91/06/11  15:45:53  15:45:53  dtk (Doug Kaltenecker)
 * Prettified files!
 * 
* Revision 3.785  91/05/22  14:58:12  14:58:12  stevec (Steve Claiborne)
* Beta version to MS
*
* Revision 3.780  91/05/15  15:58:21  15:58:21  stevec (Steve Claiborne)
* Beta
*
* Revision 3.775  91/04/05  14:32:14  14:32:14  stevec (Steve Claiborne)
* Beta release to MS
*
* Revision 3.770  91/03/25  15:37:13  15:37:13  stevec (Steve Claiborne)
* maintance release
*
* Revision 3.760  91/03/12  07:54:02  07:54:02  stevec (Steve Claiborne)
* Maintance release
*
* Revision 3.755  91/03/03  07:47:18  07:47:18  stevec (Steve Claiborne)
* March 3 Freeze
*
* Revision 3.720  91/02/11  09:16:36  09:16:36  stevec (Steve Claiborne)
* Aldus version
*
* Revision 3.710  91/02/04  15:48:49  15:48:49  stevec (Steve Claiborne)
* Aldus freeze
*
* Revision 3.700  91/01/19  09:01:31  09:01:31  stevec (Steve Claiborne)
* Release
*
* Revision 3.685  91/01/14  15:44:29  15:44:29  stevec (Steve Claiborne)
* Freeze
*
* Revision 3.681  91/01/14  10:18:42  10:18:42  stevec (Steve Claiborne)
* Updated the copy right stmt.
*
* Revision 3.680  91/01/10  16:17:48  16:17:48  stevec (Steve Claiborne)
* Freeze
*
* Revision 3.670  90/12/14  14:55:21  14:55:21  stevec (Steve Claiborne)
* freeze for 12-14-90 ver. 3.670
*
* Revision 3.665  90/12/10  15:36:59  15:36:59  stevec (Steve Claiborne)
* Freeze
*
* Revision 3.660  90/12/07  14:51:30  14:51:30  stevec (Steve Claiborne)
* Freeze 12-7-90
*
* Revision 3.650  90/11/30  08:13:20  08:13:20  stevec (Steve Claiborne)
* Freeze 3.650, 11-30-90
*
* Revision 3.601  90/08/24  13:24:23  13:24:23  daniels (Susan Daniels)
* ../message.txt
*
* Revision 3.600  90/08/03  11:11:03  11:11:03  stevec (Steve Claiborne)
* This is the Aug. 3 release ver. 3.600
*
* Revision 3.550  90/07/27  11:32:20  11:32:20  root ()
* Experimental freeze 3.55
*
* Revision 3.540  90/07/25  12:36:42  12:36:42  stevec (Steve Claiborne)
* Experimental freeze 3.54
*
* Revision 3.520  90/06/13  16:54:05  16:54:05  root ()
* 5_2_release
*
*
*    Rev 1.1   20 Feb 1990 15:49:36   vordaz
* Support for downloadables.
*/
  
/*  HP Default Character -- this is used EVERYWHERE, all
*  driver code, all translation tables, all pfm makers.
*/
#define HP_DF_CH    ((BYTE) 0x7F)
  
  
/*  HP Character sets
*/
#define MATH8_CHARSET       180
#define PIFONT_CHARSET      181
#define LINEDRAW_CHARSET    182
#define PCLINE_CHARSET      183
#define TAXLINE_CHARSET     184
#define USLEGAL_CHARSET     185
  
  
/*  DRIVERINFO version number (i.e., version of the structure).
*/
#define DRIVERINFO_VERSION  1
  
#define PCM_MAGIC   0xCAC
#define PCM_VERSION 0x310
  
typedef struct _pcmheader {
    WORD pcmMagic;
    WORD pcmVersion;
    DWORD pcmSize;
    DWORD pcmTitle;
    DWORD pcmPFMList;
} PCMHEADER, FAR * LPPCMHEADER;
  
  
typedef struct
{
    WORD    dfVersion;
    DWORD   dfSize;
    char    dfCopyright[60];
    WORD    dfType;
    WORD    dfPoints;
    WORD    dfVertRes;
    WORD    dfHorizRes;
    WORD    dfAscent;
    WORD    dfInternalLeading, dfExternalLeading;
    BYTE    dfItalic, dfUnderline, dfStrikeOut;
    WORD    dfWeight;
    BYTE    dfCharSet;
    WORD    dfPixWidth;
    WORD    dfPixHeight;
    BYTE    dfPitchAndFamily;
    WORD    dfAvgWidth;
    WORD    dfMaxWidth;
    BYTE    dfFirstChar, dfLastChar, dfDefaultChar, dfBreakChar;
    WORD    dfWidthBytes;
    DWORD   dfDevice;
    DWORD   dfFace;
    DWORD   dfBitsPointer;
    DWORD   dfBitsOffset;
    WORD    dfCharOffset[1];    /* size is dfLastChar-dfFirstChar+2 */
} PFMHEADER;
  
typedef struct
{
    WORD    dfSizeFields;
    DWORD   dfExtMetricsOffset;
    DWORD   dfExtentTable;
    DWORD   dfOriginTable;
    DWORD   dfPairKernTable;
    DWORD   dfTrackKernTable;
    DWORD   dfDriverInfo;
    DWORD   dfReserved;
} PFMEXTENSION;
  
typedef struct
{
    short   emSize;
    short   emPointSize;
    short   emOrientation;
    short   emMasterHeight;
    short   emMinScale;
    short   emMaxScale;
    short   emMasterUnits;
    short   emCapHeight;
    short   emXHeight;
    short   emLowerCaseAscent;
    short   emLowerCaseDescent;
    short   emSlant;
    short   emSuperScript;
    short   emSubScript;
    short   emSuperScriptSize;
    short   emSubScriptSize;
    short   emUnderlineOffset;
    short   emUnderlineWidth;
    short   emDoubleUpperUnderlineOffset;
    short   emDoubleLowerUnderlineOffset;
    short   emDoubleUpperUnderlineWidth;
    short   emDoubleLowerUnderlineWidth;
    short   emStrikeOutOffset;
    short   emStrikeOutWidth;
    WORD    emKernPairs;
    WORD    emKernTracks;
} EXTTEXTMETRIC;
  
typedef struct
{
    union {
        BYTE each[2];
        WORD both;
    } kpPair;
    short kpKernAmount;
} KERNPAIR;
  
typedef struct
{
    short ktDegree;
    short ktMinSize;
    short ktMinAmount;
    short ktMaxSize;
    short ktMaxAmount;
} KERNTRACK;
  
typedef enum
{
    epsymUserDefined,
    epsymRoman8,
    epsymKana8,
    epsymMath8,
    epsymUSASCII,
    epsymLineDraw,
    epsymMathSymbols,
    epsymUSLegal,
    epsymRomanExt,
    epsymISO_DenNor,
    epsymISO_UK,
    epsymISO_France,
    epsymISO_German,
    epsymISO_Italy,
    epsymISO_SwedFin,
    epsymISO_Spain,
    epsymGENERIC7,
    epsymGENERIC8,
    epsymECMA94
} SYMBOLSET;
  
typedef struct
{
    SYMBOLSET symbolSet;        /* kind of translation table */
    DWORD offset;               /* location of user-defined table */
    WORD len;                   /* length (in bytes) of table */
    BYTE firstchar, lastchar;   /* table ranges from firstchar to lastchar */
} TRANSTABLE;
  
/*** Tetra begin ***/
typedef struct
{
    BOOL scalable;          /* true if scalable font                       */
    short emMasterUnits;    /* from extended text metrics structure in pfm */
} SCALEINFO;
/*** Tetra end ***/
  
typedef struct
{
    WORD epSize;                /* size of this data structure */
    WORD epVersion;             /* number indicating version of struct */
    DWORD epMemUsage;           /* amt of memory font takes up in printer */
    DWORD epEscape;             /* pointer to escape that selects the font */
    TRANSTABLE xtbl;            /* character set translation info */
} DRIVERINFO;
  
typedef PFMHEADER far * LPPFMHEADER;
typedef PFMEXTENSION far * LPPFMEXTENSION;
typedef EXTTEXTMETRIC far * LPEXTTEXTMETRIC;
/*** Tetra II begin ***/
typedef KERNTRACK far * LPKERNTRACK;
/*** Tetra II end ***/
typedef KERNPAIR far * LPKERNPAIR;
typedef DRIVERINFO far * LPDRIVERINFO;
