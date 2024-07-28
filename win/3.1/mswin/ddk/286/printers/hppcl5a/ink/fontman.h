/**[f******************************************************************
* fontman.h -
*
* Copyright (C) 1988,1989 Aldus Corporation
* Copyright (C) 1988-1990 Microsoft Corporation.
* Copyright (C) 1989,1990, 1991 Hewlett-Packard Company.
* All rights reserved.  Company confidential.
*
**f]*****************************************************************/
  
/*
* $Header: fontman.h,v 3.890 92/02/06 16:13:29 dtk FREEZE $
*/
  
/*
* $Log:	fontman.h,v $
 * Revision 3.890  92/02/06  16:13:29  16:13:29  dtk (Doug Kaltenecker)
 * Win3.1 Freeze
 * 
 * Revision 3.870  91/11/08  11:45:18  11:45:18  dtk (Doug Kaltenecker)
 * 3.1 Release Candidate 1
 * 
 * Revision 3.865  91/11/01  13:53:16  13:53:16  dtk (Doug Kaltenecker)
 * Beta release for Windows 3.1
 * 
 * Revision 3.862  91/10/25  13:48:35  13:48:35  dtk (Doug Kaltenecker)
 * BETA
 * 
 * Revision 3.860  91/10/23  09:50:11  09:50:11  dtk (Doug Kaltenecker)
 * WinWird Release
 * 
 * Revision 3.852  91/10/09  15:01:03  15:01:03  dtk (Doug Kaltenecker)
 * BETA
 * 
 * Revision 3.850  91/10/04  16:51:20  16:51:20  dtk (Doug Kaltenecker)
 * WINWORD RELEASE CANDIDATE
 * 
 * Revision 3.840  91/09/28  14:18:33  14:18:33  dtk (Doug Kaltenecker)
 * RELEASE CANDIDATE 3 FOR WINWORD
 * 
 * Revision 3.830  91/09/18  16:34:53  16:34:53  dtk (Doug Kaltenecker)
 * RELEASE
 * 
 * Revision 3.822  91/09/16  10:36:05  10:36:05  dtk (Doug Kaltenecker)
 * WINWORD RELEASE CANDIDATE 2 FOR WIN30.
 * 
 * Revision 3.820  91/09/06  14:13:34  14:13:34  dtk (Doug Kaltenecker)
 * RELEASE CANDIDATE FOR WINWORD 2.0
 * 
 * Revision 3.812  91/08/22  14:33:27  14:33:27  dtk (Doug Kaltenecker)
 * BETA
 * 
 * Revision 3.807  91/08/08  10:32:42  10:32:42  dtk (Doug Kaltenecker)
 * PREBETA3
 * 
 * Revision 3.802  91/07/22  12:55:51  12:55:51  oakeson (Ken Oakeson)
 * BETA
 * 
 * Revision 3.800  91/07/21  13:04:37  13:04:37  dtk (Doug Kaltenecker)
 * added type_device for device realized fonts
 * 
 * Revision 3.799  91/07/02  11:53:13  11:53:13  daniels (Susan Daniels)
 * Beta
 * 
 * Revision 3.797  91/07/01  16:26:46  16:26:46  dtk (Doug Kaltenecker)
 * added structs for TT memory mgmt, move to truetype.h later
 * 
 * Revision 3.790  91/06/11  16:04:47  16:04:47  stevec (Steve Claiborne)
 * Freeze
 * 
 * Revision 3.786  91/06/11  15:46:07  15:46:07  dtk (Doug Kaltenecker)
 * Prettified files!
 * 
* Revision 3.785  91/05/22  14:58:23  14:58:23  stevec (Steve Claiborne)
* Beta version to MS
*
* Revision 3.780  91/05/15  15:58:32  15:58:32  stevec (Steve Claiborne)
* Beta
*
* Revision 3.775  91/04/05  14:32:24  14:32:24  stevec (Steve Claiborne)
* Beta release to MS
*
* Revision 3.770  91/03/25  15:37:23  15:37:23  stevec (Steve Claiborne)
* maintance release
*
* Revision 3.760  91/03/12  07:54:12  07:54:12  stevec (Steve Claiborne)
* Maintance release
*
* Revision 3.755  91/03/03  07:47:28  07:47:28  stevec (Steve Claiborne)
* March 3 Freeze
*
* Revision 3.720  91/02/11  09:16:46  09:16:46  stevec (Steve Claiborne)
* Aldus version
*
* Revision 3.710  91/02/04  15:48:59  15:48:59  stevec (Steve Claiborne)
* Aldus freeze
*
* Revision 3.700  91/01/19  09:01:41  09:01:41  stevec (Steve Claiborne)
* Release
*
* Revision 3.685  91/01/14  15:44:39  15:44:39  stevec (Steve Claiborne)
* Freeze
*
* Revision 3.681  91/01/14  10:18:52  10:18:52  stevec (Steve Claiborne)
* Updated the copy right stmt.
*
* Revision 3.680  91/01/10  16:17:59  16:17:59  stevec (Steve Claiborne)
* Freeze
*
* Revision 3.670  90/12/14  14:55:31  14:55:31  stevec (Steve Claiborne)
* freeze for 12-14-90 ver. 3.670
*
* Revision 3.665  90/12/10  15:37:10  15:37:10  stevec (Steve Claiborne)
* Freeze
*
* Revision 3.660  90/12/07  14:51:40  14:51:40  stevec (Steve Claiborne)
* Freeze 12-7-90
*
* Revision 3.650  90/11/30  08:13:30  08:13:30  stevec (Steve Claiborne)
* Freeze 3.650, 11-30-90
*
* Revision 3.604  90/09/06  14:58:52  14:58:52  oakeson (Ken Oakeson)
* Changed char download structure to dynamic format.
*
* Revision 3.603  90/08/29  17:42:47  17:42:47  oakeson (Ken Oakeson)
* Added CHARDL struct for downloading characters on demand
*
* Revision 3.602  90/08/24  13:22:00  13:22:00  daniels ()
* ../message.txt
*
* Revision 3.601  90/08/14  15:25:47  15:25:47  oakeson (Ken Oakeson)
* Changed function param from font index to lpfont
*
* Revision 3.600  90/08/03  11:11:15  11:11:15  stevec (Steve Claiborne)
* This is the Aug. 3 release ver. 3.600
*
* Revision 3.550  90/07/27  11:32:32  11:32:32  root ()
* Experimental freeze 3.55
*
* Revision 3.540  90/07/25  12:36:59  12:36:59  stevec (Steve Claiborne)
* Experimental freeze 3.54
*
* Revision 3.520  90/06/13  16:54:20  16:54:20  root ()
* 5_2_release
*
*
*    Rev 1.1   20 Feb 1990 15:48:52   vordaz
* Support for downloadables.
*/
  
#define ISTRUE(BitField,ch) ((BitField[(ch)/16]) & (1 << ((ch)%16)))
#define SETTRUE(BitField,ch) ((BitField[(ch)/16]) |= (1 << ((ch)%16)))
  
typedef struct {
    WORD LastCode;      /* May have > 255 chars w/ compound contours */
    WORD CharDown[1];   /* Array of characters currently downloaded. */
    /* We'll also allocate a BitField for Class */
    /* 4 characters, a long array for character */
    /* offsets into the file, and a word array  */
    /* for recording the next character.        */
} CHARDL;
  
typedef CHARDL far *LPCHARDL;
  
typedef enum { fontname, fontescape, fontpfmfile, fontdlfile } stringType;
  
typedef struct {
    short       offset;         /* offset to font in resource file */
    SYMBOLSET   symbolSet;      /* symbol set (USASCII, Roman8) */
    short       fontID;         /* ID of soft font (-1 if not soft) */
    short       indName;        /* Index to Font name */
    short       indEscape;      /* Index to escape invoking font */
                                /* -1 if soft, downloadable font */
    short       indPFMPath;     /* Index to path name (soft font) */
                                /* -1 if ROM/cartridge font */
    short       indDLPath;      /* Index to path name (download soft font) */
                                /* -1 if permanent downloaded font */
    short       indPFMName;     /* Index to file name (soft font) */
                                /* -1 if ROM/cartridge font */
    short       indDLName;      /* Index to file name (download soft font) */
                                /* -1 if permanent downloaded font */
    long        lPCMOffset;     /* offset of PFM in PCM file */
                                /* 0 if soft font PFM file (ie, no offset) */
    BOOL        ZCART_hack;     /* Z cartridge hack */
    BOOL        QUOTE_hack;     /* Typographic quotes hack */
  
/*** Tetra begin ***/
    SCALEINFO   scaleInfo;      /* scalable font flag structure */
/*** Tetra end ***/
  
    short LRUcount;             /* Least Recently used count */
    long memUsage;              /* Memory use of this fonts' bitmaps */
  
    HANDLE hCharDL;             /* Handle to char download information */
  
    BYTE  onPage;                /* TRUE if font on page */
    short       indPrevSoft;       /* Index to prev soft font in list */
    short       indNextSoft;       /* Index to next soft font in list */
    short int   dfType;         /* Font Metrics that we need ... */
    short int   dfPoints;
    short int   dfVertRes;
    short int   dfHorizRes;
    short int   dfAscent;
    short int   dfInternalLeading;
    short int   dfExternalLeading;
    BYTE        dfItalic;
    BYTE        dfUnderline;
    BYTE        dfStrikeOut;
    short int   dfWeight;
    BYTE        dfCharSet;
    short int   dfPixWidth;
    short int   dfPixHeight;
    BYTE        dfPitchAndFamily;
    short int   dfAvgWidth;
    short int   dfMaxWidth;
    BYTE        dfFirstChar;
    BYTE        dfLastChar;
    BYTE        dfDefaultChar;
    BYTE        dfBreakChar;
    HANDLE      hExtMetrics;    /* Handles to tables we may load ... */
    HANDLE      hWidthTable;
    HANDLE      hPairKernTable;
    HANDLE      hTrackKernTable;
} FONTSUMMARY;
  
typedef struct {
    short       numOpenDC;      /* # DC's using this struct */
    short       len;            /* Number of fontSummary items */
    short       firstSoft;      /* First soft font in list */
    BOOL        newFS;          /* TRUE if first time struct created */
    WORD        softfonts;      /* Number of key words listed in win.ini */
    PCLDEVMODE  environ;        /* Environment used to create fontSum */
    FONTSUMMARY f[1];           /* Array of fonts */
} FONTSUMMARYHDR;
  
typedef FONTSUMMARY far *LPFONTSUMMARY;
typedef FONTSUMMARYHDR far *LPFONTSUMMARYHDR;
  
  
#define TYPE_DEVICE       0x0080  // font is a PCL device font


#ifdef FONTMAN_UTILS
/*  Constants used by LoadPFMStruct
*/
#define FNTLD_WIDTHS        1
#define FNTLD_EXTMETRICS    2
#define FNTLD_PAIRKERN      3
#define FNTLD_TRACKKERN     4
  
/*** Tetra II begin ***/
/*** Added a short for the pixel height ***/
BOOL far PASCAL LoadFontString(LPDEVICE, LPSTR, short, stringType, short, short);
/*** Tetra II end ***/
LPSTR far PASCAL LoadWidthTable(LPDEVICE, LPFONTINFO);
void far PASCAL UnloadWidthTable(LPDEVICE, short);
#endif
  
#ifdef FONTMAN_ENABLE
HANDLE far PASCAL GetFontSummary(LPSTR, LPSTR, LPPCLDEVMODE, HANDLE);
#endif
  
#ifdef FONTMAN_DISABLE
HANDLE far PASCAL FreeFontSummary(LPDEVICE);
#endif
  
#ifdef DEBUG
void FAR PASCAL DBGdumpFontSummary(LPFONTSUMMARYHDR, short);
#endif
