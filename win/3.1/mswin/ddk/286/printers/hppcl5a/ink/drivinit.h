/*$Header: drivinit.h,v 3.890 92/02/06 16:13:32 dtk FREEZE $*/
/*
*  drivinit.h
*
* Copyright (C) 1989 Microsoft Corporation. All rights reserved.
* Copyright (C) 1989,1990, 1991 Hewlett-Packard Company.
*
*  Header file for printer driver initialization.
*/
/*$Log:	drivinit.h,v $
 * Revision 3.890  92/02/06  16:13:32  16:13:32  dtk (Doug Kaltenecker)
 * Win3.1 Freeze
 * 
 * Revision 3.870  91/11/08  11:45:21  11:45:21  dtk (Doug Kaltenecker)
 * 3.1 Release Candidate 1
 * 
 * Revision 3.865  91/11/01  13:53:20  13:53:20  dtk (Doug Kaltenecker)
 * Beta release for Windows 3.1
 * 
 * Revision 3.862  91/10/25  13:48:39  13:48:39  dtk (Doug Kaltenecker)
 * BETA
 * 
 * Revision 3.860  91/10/23  09:50:15  09:50:15  dtk (Doug Kaltenecker)
 * WinWird Release
 * 
 * Revision 3.852  91/10/09  15:01:06  15:01:06  dtk (Doug Kaltenecker)
 * BETA
 * 
 * Revision 3.850  91/10/04  16:51:23  16:51:23  dtk (Doug Kaltenecker)
 * WINWORD RELEASE CANDIDATE
 * 
 * Revision 3.840  91/09/28  14:18:36  14:18:36  dtk (Doug Kaltenecker)
 * RELEASE CANDIDATE 3 FOR WINWORD
 * 
 * Revision 3.830  91/09/18  16:34:56  16:34:56  dtk (Doug Kaltenecker)
 * RELEASE
 * 
 * Revision 3.822  91/09/16  10:36:09  10:36:09  dtk (Doug Kaltenecker)
 * WINWORD RELEASE CANDIDATE 2 FOR WIN30.
 * 
 * Revision 3.820  91/09/06  14:13:37  14:13:37  dtk (Doug Kaltenecker)
 * RELEASE CANDIDATE FOR WINWORD 2.0
 * 
 * Revision 3.813  91/09/06  10:45:40  10:45:40  daniels (Susan Daniels)
 * Put in WIN31 changes to DEVMODE: added two new fields and removed
 * dmDriverData.
 * 
 * Revision 3.812  91/08/22  14:33:30  14:33:30  dtk (Doug Kaltenecker)
 * BETA
 * 
 * Revision 3.807  91/08/08  10:32:45  10:32:45  dtk (Doug Kaltenecker)
 * PREBETA3
 * 
 * Revision 3.803  91/08/02  09:56:05  09:56:05  daniels (Susan Daniels)
 * Added new defines and DM_SPECVERSION as specified by MS for Win 31 
 * build 42.
 * 
 * Revision 3.802  91/07/22  12:55:55  12:55:55  oakeson (Ken Oakeson)
 * BETA
 * 
 * Revision 3.800  91/07/19  12:26:40  12:26:40  daniels (Susan Daniels)
 * Bug #398:  update masks for windows 3x.
 * 
 * Revision 3.799  91/07/02  11:53:17  11:53:17  daniels (Susan Daniels)
 * Beta
 * 
 * Revision 3.796  91/06/26  11:27:38  11:27:38  stevec (Steve Claiborne)
 * BETA
 * 
 * Revision 3.790  91/06/11  16:04:50  16:04:50  stevec (Steve Claiborne)
 * Freeze
 * 
 * Revision 3.787  91/06/11  15:46:11  15:46:11  dtk (Doug Kaltenecker)
 * Prettified files!
 * 
* Revision 3.786  91/05/28  11:03:44  11:03:44  daniels (Susan Daniels)
* Fix BUG #213:  Added new defs from MS for DC_BINNAMES etc.
*
* Revision 3.785  91/05/22  14:58:26  14:58:26  stevec (Steve Claiborne)
* Beta version to MS
*
* Revision 3.780  91/05/15  15:58:35  15:58:35  stevec (Steve Claiborne)
* Beta
*
* Revision 3.775  91/04/05  14:32:28  14:32:28  stevec (Steve Claiborne)
* Beta release to MS
*
* Revision 3.770  91/03/25  15:37:27  15:37:27  stevec (Steve Claiborne)
* maintance release
*
* Revision 3.760  91/03/12  07:54:15  07:54:15  stevec (Steve Claiborne)
* Maintance release
*
* Revision 3.755  91/03/03  07:47:31  07:47:31  stevec (Steve Claiborne)
* March 3 Freeze
*
* Revision 3.722  91/02/26  11:13:01  11:13:01  daniels (Susan Daniels)
* Correct description of dmSize field in DEVMODE structure.
*
* Revision 3.700.1.5  91/02/05  10:49:13  10:49:13  stevec (Steve Claiborne)
* Freeze
*
* Revision 3.700.1.1  91/02/05  10:31:11  10:31:11  stevec (Steve Claiborne)
* textfile
*
* Revision 3.700  91/01/19  09:01:44  09:01:44  stevec (Steve Claiborne)
* Release
*
* Revision 3.685  91/01/14  15:44:42  15:44:42  stevec (Steve Claiborne)
* Freeze
*
* Revision 3.681  91/01/14  10:18:55  10:18:55  stevec (Steve Claiborne)
* Updated the copy right stmt.
*
* Revision 3.680  91/01/10  16:18:03  16:18:03  stevec (Steve Claiborne)
* Freeze
*
* Revision 3.670  90/12/14  14:55:35  14:55:35  stevec (Steve Claiborne)
* freeze for 12-14-90 ver. 3.670
*
* Revision 3.665  90/12/10  15:37:13  15:37:13  stevec (Steve Claiborne)
* Freeze
*
* Revision 3.660  90/12/07  14:51:44  14:51:44  stevec (Steve Claiborne)
* Freeze 12-7-90
*
* Revision 3.651  90/12/06  08:38:17  08:38:17  stevec (Steve Claiborne)
* Modified DEVMODE structure to correspond to the def'n in SDK.  This fixes
* bug #76 - PM couldn't change duplex on a per document bases.  SJC
*
* Revision 3.650  90/11/30  08:13:33  08:13:33  stevec (Steve Claiborne)
* Freeze 3.650, 11-30-90
*
* Revision 3.601  90/08/24  13:21:53  13:21:53  daniels (Susan Daniels)
* ../message.txt
*
* Revision 3.600  90/08/03  11:11:19  11:11:19  stevec (Steve Claiborne)
* This is the Aug. 3 release ver. 3.600
*
* Revision 3.550  90/07/27  11:32:35  11:32:35  root ()
* Experimental freeze 3.55
*
* Revision 3.540  90/07/25  12:37:05  12:37:05  stevec (Steve Claiborne)
* Experimental freeze 3.54
*
* Revision 3.521  90/07/09  17:10:15  17:10:15  daniels ()
* Added support for envelopes and removed unsupported papers:
*     Added DMPAPER_MONARCH, etc.
* */
  
/*  History:
* 09-05-91     SD      Added compiler switches for Win31 stuff.  Include
*                      build.h in resource.h ahead of the include for drivint.h.
* 08-01-91     SD      Added DM_SPECVERSION and defines for Win31 build 42.
* 07-17-91     SD      Added #define for DC_SCALE for BUG #398.
* 05-28-91     SD      Added new #defines; DC_BINNAMES for Bug #213.
* 02-15-91     SD      Changed #defines for paper sizes to agree with
*                      SDK.  Fixes bug #130.
* 12-5-90      SJC     Changed DEVMODE struct to correspond to the
*                      def'n in the SDK - fixes bug #76.
* 9-7-90       SD      Added envelopes; removed unsupported papers.
*  2-6-89      craigc  Initial
*  2-7-89      jimmat  Added DM*_FIRST/LAST defines
*
*/
  
/* size of a device name string */
#define CCHDEVICENAME 32

/* current version of specification */
#if defined (WIN31)  
#define DM_SPECVERSION 0x30A
#else
#define DM_SPECVERSION 0x300
#endif
  
/* field selection bits */
#define DM_ORIENTATION      0x0000001L
#define DM_PAPERSIZE        0x0000002L
#define DM_PAPERLENGTH      0x0000004L
#define DM_PAPERWIDTH       0x0000008L
#define DM_SCALE        0x0000010L
#define DM_COPIES       0x0000100L
#define DM_DEFAULTSOURCE    0x0000200L
#define DM_PRINTQUALITY     0x0000400L
#define DM_COLOR        0x0000800L
#define DM_DUPLEX       0x0001000L
#define DM_YRESOLUTION  0x0002000L      /* Added for Win 31 build 42 */
#define DM_TTOPTION     0x0004000L      /* Ditto */

  
/* orientation selections */
#define DMORIENT_PORTRAIT   1
#define DMORIENT_LANDSCAPE  2
  
/* paper selections */
#define DMPAPER_FIRST       DMPAPER_LETTER
#define DMPAPER_LETTER      1       // Letter 8 1/2 x 11 in
#define DMPAPER_LETTERSMALL 2       // Letter Small 8 1/2 x 11 in
#define DMPAPER_TABLOID     3       // Tabloid 11 x 17 in
#define DMPAPER_LEDGER      4       // Ledger 17 x 11 in
#define DMPAPER_LEGAL       5       // Legal 8 1/2 x 14 in
#define DMPAPER_STATEMENT   6       // Statement 5 1/2 x 8 1/2 in
#define DMPAPER_EXECUTIVE   7       // Executive"7 1/2 x 10 in
#define DMPAPER_A3      8       // A3 297 x 420 mm
#define DMPAPER_A4      9       // A4 210 x 297 mm
#define DMPAPER_A4SMALL     10      // A4 Small 210 x 297 mm
#define DMPAPER_A5      11      // A5 148 x 210 mm
#define DMPAPER_B4      12      // B4 250 x 354
#define DMPAPER_B5      13      // B5 182 x 257 mm
#define DMPAPER_FOLIO       14      // Folio 8 1/2 x 13 in
#define DMPAPER_QUARTO      15      // Quarto 215 x 275 mm
#define DMPAPER_10X14       16      // 10x14 in
#define DMPAPER_11X17       17      // 11x17 in
#define DMPAPER_NOTE        18      // Note 8 1/2 x 11 in
#define DMPAPER_ENV_9       19      // Envelope #9 3 7/8 x 8 7/8
#define DMPAPER_COM10       20      // Com-10 Envelope #10 4 1/8 x 9 1/2
#define DMPAPER_ENV_11      21      // Envelope #11 4 1/2 x 10 3/8
#define DMPAPER_ENV_12      22      // Envelope #12 4 \276 x 11
#define DMPAPER_ENV_14      23      // Envelope #14 5 x 11 1/2
#define DMPAPER_CSHEET      24      // C size sheet
#define DMPAPER_DSHEET      25      // D size sheet
#define DMPAPER_ESHEET      26      // E size sheet
#define DMPAPER_DL          27      // DL Envelope 110mm x 220mm or 4.331" x 8.661"
#define DMPAPER_C5          28      // C5 Envelope 162mm x 229mm or 6.378" x 9.016"
/* Start of additions for Win 31 build 42 */
#define DMPAPER_ENV_C3	    29		// Envelope C3	324 x 458 mm
#define DMPAPER_ENV_C4	    30		// Envelope C4	229 x 324 mm
#define DMPAPER_ENV_C6	    31		// Envelope C6	114 x 162 mm
#define DMPAPER_ENV_C65     32		// Envelope C65 114 x 229 mm
#define DMPAPER_ENV_B4	    33		// Envelope B4	250 x 353 mm
#define DMPAPER_ENV_B5	    34		// Envelope B5	176 x 250 mm
#define DMPAPER_ENV_B6	    35		// Envelope B6	176 x 125 mm
#define DMPAPER_ENV_ITALY   36		// Envelope 110 x 230 mm
#define DMPAPER_MONARCH     37		// Envelope Monarch 3.875 x 7.5 in
#define DMPAPER_ENV_PERSONAL 38 	 // 6 3/4 Envelope 3 5/8 x 6 1/2 in
#define DMPAPER_FANFOLD_US   39 	 // US Std Fanfold 14 7/8 x 11 in
#define DMPAPER_FANFOLD_STD_GERMAN  40	// German Std Fanfold 8 1/2 x 12 in
#define DMPAPER_FANFOLD_LGL_GERMAN  41	// German Legal Fanfold 8 1/2 x 13 in

#define DMPAPER_LETTER_EXTRA		 50    // Letter Extra 9 \275 x 12 in
#define DMPAPER_LEGAL_EXTRA		 51    // Legal Extra 9 \275 x 15 in
#define DMPAPER_TABLOID_EXTRA		 52    // Tabloid Extra 11.69 x 18 in
#define DMPAPER_A4_EXTRA		 53    // A4 Extra 9.27 x 12.69 in
#define DMPAPER_LETTER_TRANSVERSE	 54    // Letter Transverse 11 x 8 \275 in
#define DMPAPER_A4_TRANSVERSE		 55    // Transverse 297 x 210 mm
#define DMPAPER_LETTER_EXTRA_TRANSVERSE  56    // Letter Extra Transverse 12 x 9\275 in

#define DMPAPER_LAST	    DMPAPER_LETTER_EXTRA_TRANSVERSE

#define DMPAPER_USER    256
/* End of additions for Win 31 build 42 */  

/* bin selections */
#define DMBIN_FIRST     DMBIN_UPPER
#define DMBIN_UPPER     1
#define DMBIN_ONLYONE       1
#define DMBIN_LOWER     2
#define DMBIN_MIDDLE        3
#define DMBIN_MANUAL        4
#define DMBIN_ENVELOPE      5
#define DMBIN_ENVMANUAL     6
#define DMBIN_AUTO      7
#define DMBIN_TRACTOR       8
#define DMBIN_SMALLFMT      9
#define DMBIN_LARGEFMT      10
#define DMBIN_LARGECAPACITY 11
#define DMBIN_CASSETTE      14            /* Added for Win 31 build 42 */
#define DMBIN_LAST      DMBIN_CASSETTE
  
#define DMBIN_USER      256     /* device specific bins start here */
  
/* print qualities */
#define DMRES_DRAFT     (-1)
#define DMRES_LOW       (-2)
#define DMRES_MEDIUM        (-3)
#define DMRES_HIGH      (-4)
  
/* color enable/disable for color printers */
#define DMCOLOR_MONOCHROME  1
#define DMCOLOR_COLOR       2
  
/* duplex enable */
#define DMDUP_SIMPLEX    1
#define DMDUP_VERTICAL   2
#define DMDUP_HORIZONTAL 3

#if defined(WIN31)
/* TrueType options  -  Added for Win 31 build 42 */
#define DMTT_BITMAP	1	// print TT fonts as graphics
#define DMTT_DOWNLOAD	2	// download TT fonts as soft fonts
#define DMTT_SUBDEV	3	// substitute device fonts for TT fonts
#endif

#if defined (WIN31)

typedef struct _devicemode {
    char dmDeviceName[CCHDEVICENAME]; // i.e. "PCL/HP LaserJet"
    WORD dmSpecVersion;               // Ver # of init data specification
    WORD dmDriverVersion;             // driver version
    WORD dmSize;                      // Size in bytes of DEVMODE struct not including
                                      // the dmDriverData field.
    WORD dmDriverExtra;               // size of dmDriverData
    DWORD dmFields;                   // bit fields that specify which fields are
                                      // initialized in DEVMODE
    short dmOrientation;              // paper orientation
    short dmPaperSize;                // paper size
    short dmPaperLength;              // paper length, overrides dmPaperSize
    short dmPaperWidth;               // paper width, overrides dmPaperSize
    short dmScale;                    // scale of output, by dmScale/100
    short dmCopies;                   // # of copies to print
    short dmDefaultSource;            // default paper bin
    short dmPrintQuality;             // printer resolution; or x-res in dpi if y-res is set.
    short dmColor;                    // color/monochrome
    short dmDuplex;                   // duplex or double-sided printing
    short dmYResolution;              // New in WIN31. y-res in dpi.
    short dmTTOption;                 // New in WIN31.  How to print TT fonts.
} DEVMODE;

#else /* End WIN31; Start Win 30 */  

// For more detailed info on DEVMODE see SDK manual (vol. 2)
typedef struct _devicemode {
    char dmDeviceName[CCHDEVICENAME]; // i.e. "PCL/HP LaserJet"
    WORD dmSpecVersion;               // Ver # of init data specification
    WORD dmDriverVersion;             // driver version
    WORD dmSize;                      // Size in bytes of DEVMODE struct not including
                                      // the dmDriverData field.
    WORD dmDriverExtra;               // size of dmDriverData
    DWORD dmFields;                   // bit fields that specify which fields are
                                      // initialized in DEVMODE
    short dmOrientation;              // paper orientation
    short dmPaperSize;                // paper size
    short dmPaperLength;              // paper length, overrides dmPaperSize
    short dmPaperWidth;               // paper width, overrides dmPaperSize
    short dmScale;                    // scale of output, by dmScale/100
    short dmCopies;                   // # of copies to print
    short dmDefaultSource;            // default paper bin
    short dmPrintQuality;             // printer resolution
    short dmColor;                    // color/monochrome
    short dmDuplex;                   // duplex or double-sided printing
    BYTE dmDriverData[1];             // device specific data
} DEVMODE;

#endif /* End Win30 */
  
typedef DEVMODE * PDEVMODE, NEAR * NPDEVMODE, FAR * LPDEVMODE;
  
/* mode selections for the device mode function */
#define DM_UPDATE       1
#define DM_COPY         2
#define DM_PROMPT       4
#define DM_MODIFY       8
  
/* device capabilities indices */
#define DC_FIELDS       1
#define DC_PAPERS       2
#define DC_PAPERSIZE        3
#define DC_MINEXTENT        4
#define DC_MAXEXTENT        5
#define DC_BINS         6
#define DC_DUPLEX       7
#define DC_SIZE         8
#define DC_EXTRA        9
#define DC_VERSION      10
#define DC_DRIVER       11
#define DC_BINNAMES   12            /* BUG #213 */

#if defined (WIN31)
#define DC_ENUMRESOLUTIONS   13     
#define DC_FILEDEPENDENCIES  14     
#define DC_TRUETYPE          15     
#define DC_PAPERNAMES        16     
#define DC_ORIENTATION       17     
#define DC_COPIES            18
#endif

#if defined (WIN31)
/* bit fields of the return value (DWORD) for DC_TRUETYPE */
#define DCTT_BITMAP	    0x0000001L       
#define DCTT_DOWNLOAD	    0x0000002L 
#define DCTT_SUBDEV	    0x0000004L     
#endif

  
/* export ordinal definitions */
#define GPA_EXTDEVICEMODE   MAKEINTRESOURCE(19)
#define GPA_DEVICECAPABILITIES MAKEINTRESOURCE(20)
#define GPA_OLDDEVICEMODE   MAKEINTRESOURCE(13)
  
/* type of pointer returned by GetProcAddress() for ExtDeviceMode */
typedef WORD FAR PASCAL FNDEVMODE(HWND, HANDLE, LPDEVMODE, LPSTR, LPSTR,
LPDEVMODE, LPSTR, WORD);
  
typedef FNDEVMODE FAR * LPFNDEVMODE;
  
/* type of pointer returned for DeviceCapabilities */
typedef DWORD FAR PASCAL FNDEVCAPS(LPSTR, LPSTR, WORD, LPSTR, LPDEVMODE);
  
typedef FNDEVCAPS FAR * LPFNDEVCAPS;
