/*
*  truetype.h
*
*  prototypes, defines and typedefs for TrueType support
*/
  
/*
* $Header: truetype.h,v 3.890 92/02/06 16:13:17 dtk FREEZE $
*/
  
/*
* $Log:	truetype.h,v $
 * Revision 3.890  92/02/06  16:13:17  16:13:17  dtk (Doug Kaltenecker)
 * Win3.1 Freeze
 * 
 * Revision 3.872  92/01/29  16:27:08  16:27:08  dtk (Doug Kaltenecker)
 * *** empty log message ***
 * 
 * Revision 3.871  91/12/02  16:46:44  16:46:44  dtk (Doug Kaltenecker)
 * Changed the ifdef TTs to WIN31s.
 * 
 * Revision 3.871  91/11/22  13:20:37  13:20:37  dtk (Doug Kaltenecker)
 * Win 3.1 Post Beta 3 version.
 * 
 * Revision 3.870  91/11/08  11:45:05  11:45:05  dtk (Doug Kaltenecker)
 * 3.1 Release Candidate 1
 * 
 * Revision 3.865  91/11/01  13:53:04  13:53:04  dtk (Doug Kaltenecker)
 * Beta release for Windows 3.1
 * 
 * Revision 3.862  91/10/25  13:48:22  13:48:22  dtk (Doug Kaltenecker)
 * BETA
 * 
 * Revision 3.860  91/10/23  09:49:57  09:49:57  dtk (Doug Kaltenecker)
 * WinWird Release
 * 
 * Revision 3.852  91/10/09  15:00:50  15:00:50  dtk (Doug Kaltenecker)
 * BETA
 * 
 * Revision 3.850  91/10/04  16:51:07  16:51:07  dtk (Doug Kaltenecker)
 * WINWORD RELEASE CANDIDATE
 * 
 * Revision 3.840  91/09/28  14:18:20  14:18:20  dtk (Doug Kaltenecker)
 * RELEASE CANDIDATE 3 FOR WINWORD
 * 
 * Revision 3.830  91/09/18  16:34:40  16:34:40  dtk (Doug Kaltenecker)
 * RELEASE
 * 
 * Revision 3.822  91/09/16  10:35:47  10:35:47  dtk (Doug Kaltenecker)
 * WINWORD RELEASE CANDIDATE 2 FOR WIN30.
 * 
 * Revision 3.820  91/09/06  14:13:21  14:13:21  dtk (Doug Kaltenecker)
 * RELEASE CANDIDATE FOR WINWORD 2.0
 * 
 * Revision 3.813  91/09/04  11:46:08  11:46:08  dtk (Doug Kaltenecker)
 * Moved the #define USE_GRC to build.h and included it.
 * 
 * Revision 3.812  91/08/22  14:33:13  14:33:13  dtk (Doug Kaltenecker)
 * BETA
 * 
 * Revision 3.807  91/08/08  10:32:29  10:32:29  dtk (Doug Kaltenecker)
 * PREBETA3
 * 
 * Revision 3.802  91/07/22  12:55:37  12:55:37  oakeson (Ken Oakeson)
 * BETA
 * 
 * Revision 3.799  91/07/02  11:52:59  11:52:59  daniels (Susan Daniels)
 * Beta
 * 
 * Revision 3.797  91/07/01  16:28:56  16:28:56  dtk (Doug Kaltenecker)
 * added structs for TT memory managment
 * 
 * Revision 3.790  91/06/11  16:04:33  16:04:33  stevec (Steve Claiborne)
 * Freeze
 * 
 * Revision 3.786  91/06/11  15:45:49  15:45:49  dtk (Doug Kaltenecker)
 * Prettified files!
 * 
* Revision 3.785  91/05/22  14:58:09  14:58:09  stevec (Steve Claiborne)
* Beta version to MS
*
* Revision 3.781  91/05/22  13:48:49  13:48:49  oakeson (Ken Oakeson)
* Added GetRasterizerCaps functionality
*
*
* Revision 1.3  90/09/06  15:02:26  15:02:26  oakeson (Ken Oakeson)
* moved WHACKWORD macro before #ifdef, so that it will always be available,
* whether we're compiling with TrueType support or not
*
* Revision 1.2  90/08/24  13:24:33  13:24:33  daniels ()
* ../message.txt
*
* Revision 1.1  90/08/14  15:37:09  15:37:09  oakeson (Ken Oakeson)
* Initial revision
*
*/

#include "build.h"
  
/* hppcl is a 68000 based thing and it uses that foul backwards word garbage
* so here's a macro to take a correctly formatted number and hash it into
* nonsense (ie, swap bytes)
*/
#define WHACKWORD(x)  (((WORD)(x)<<8) | ((WORD)(x)>>8))
  
// high byte of dfType defines
  
#define TYPE_TRUETYPE       0x0100  // font is truetype
#define TYPE_HEADERDOWN     0x0200  // header has been downloaded
#define TYPE_HAVEWIDTHS     0x0400  // width table has been generated
#define TYPE_SETCONTEXT     0x0800  // remember to set font context
  
#define CAPS_TTFNTINST      0x0001  // at least 1 truetype font installed
#define CAPS_TTENABLED      0x0002  // truetype is currently enabled
  
#define FD_QUERY_CHARIMAGE  1       // get a bitmap
#define FD_QUERY_OUTLINE    2       // get an outline (polyline)
  
#define NCHARS   256
#define TTHDRSZ  64                 // size (in bytes) of TT font header
#define DEFTTNUM 10                 // default number of tt fonts per page
#define TTBASE   16384               // base number for tt font id's
  
#define CWGLYPHFLAGS (NCHARS/(8*sizeof(WORD)))
  
#define MAKEFIXED(n) ((LONG)(n)<<16)
#define FROMFIXED(n) HWORD(n)
  
#define ROT_PORT MAKEFIXED(0)       // rotation for portrait
#define ROT_LAND MAKEFIXED(270)     // rotation for landscape
  
#define LVHWORD(l) (((WORD FAR *)&l)[1])
#define LVLWORD(l) (((WORD FAR *)&l)[0])
  
typedef LONG FIXED;
  
typedef WORD FAR * LPINT;
  
typedef BYTE huge * LPDIBITS;
  
/* structs used for TrueType memory usage
 */
typedef struct {
    DWORD TTMem;            /* memory used by current font */
    WORD  TTUsage;          /* last time font was used, LRU = 0 */
} TTFSUM;


//typedef struct {
//  WORD  TTnum;            /* number of downloaded TrueType fonts */
//    DWORD TotMem;           /* total memory used by downloaded fonts */
//  WORD  Page;             /* the current page being printed */
//    BYTE  TTfs[1];
//} TTFSUMHDR;

typedef TTFSUM far *LPTTFSUM;
//typedef TTFSUMHDR far *LPTTFSUMHDR;

// default size of tt font struct
#define DEFTTSZ  DEFTTNUM * sizeof(TTFSUM)


typedef struct _rastcapinfo
{
    WORD nsize;
    WORD nFlags;
} RASTCAPINFO, FAR * LPRASTCAPINFO;
  
typedef struct _pointfx
{
    FIXED x;
    FIXED y;
} POINTFX;
  
typedef struct _sizel
{
    DWORD x;
    DWORD y;
} SIZEL;
  
typedef struct _bitmapmetrics
{
    SIZEL sizlExtent;
    POINTFX pfxOrigin;
    POINTFX pfxCharInc;
} BITMAPMETRICS, FAR * LPBITMAPMETRICS;
  
typedef struct _ttfontinfo
{
    WORD idFont;
    WORD rgfGlyphDown[CWGLYPHFLAGS];
    WORD rgwWidths[256];
} TTFONTINFO, FAR * LPTTFONTINFO;
  
#define ISCHARDOWN(lpttfi,ch) (((lpttfi)->rgfGlyphDown[(ch)/16]) & (1 << ((ch)%16)))
#define SETCHARDOWN(lpttfi,ch) (((lpttfi)->rgfGlyphDown[(ch)/16]) |= (1 << ((ch)%16)))
#define CLRCHARDOWN(lpttfi,ch) (((lpttfi)->rgfGlyphDown[(ch)/16]) &= ~(1 << ((ch)%16)))
  
typedef struct _pclheader
{
    WORD    wHeaderSize;
    BYTE    bDummy1;
    BYTE    bFontType;
    WORD    wDummy2;
    WORD    wBaseLine;
    WORD    wCellWidth;
    WORD    wCellHeight;
    BYTE     bOrientation;
    BYTE    bSpacing;
    WORD    wSymbolSet;
    WORD    wHMI;
    WORD    wHeight;
    WORD    wHeightX;
    BYTE    bWidthType;
    BYTE    bStyle;
    BYTE    bStrokeWeight;
    BYTE    bTypeFace;
    BYTE    bDummy3;
    BYTE    bSerifStyle;
    WORD    wDummy4;
    BYTE    bUnderlineDistance;
    BYTE    bUnderlineHeight;
    WORD    wTextHeight;
    WORD    wTextWidth;
    WORD    wFirstChar;
    WORD    wLastChar;
    BYTE    bPitchExtended;
    BYTE    bHeightExtended;
    WORD    wDummy7;
    WORD    wDummy8;
    WORD    wDummy9;
    char    cName [16];
} PCLHEADER;
  
typedef struct _charheader
{
    BYTE    bFormat;
    BYTE    bContinuation;
    BYTE    bDescriptorSize;
    BYTE    bClass;
    BYTE    bOrientation;
    BYTE    bDummy1;
    WORD    wLeftOffset;
    WORD    wTopOffset;
    WORD    wWidth;
    WORD    wHeight;
    WORD    wDeltaX;
} CHARHEADER;
  
/*
*  GDI Engine exports
*/
  
WORD FAR PASCAL EngineRealizeFont(LPLOGFONT, LPTEXTXFORM, LPFONTINFO);
WORD FAR PASCAL EngineDeleteFont(LPFONTINFO);
WORD FAR PASCAL EngineEnumerateFont(LPSTR, FARPROC, DWORD);
WORD FAR PASCAL EngineGetCharWidth(LPFONTINFO,BYTE,BYTE,LPINT);
WORD FAR PASCAL EngineSetFontContext(LPFONTINFO,WORD);
WORD FAR PASCAL EngineGetGlyphBmp(HANDLE,LPFONTINFO,WORD,WORD,LPSTR,DWORD,LPBITMAPMETRICS);

  
#ifdef WIN31

int FAR PASCAL GetRasterizerCaps(LPRASTCAPINFO,WORD);

#endif
  
/* internal TrueType utility functions
*/
  
void FAR PASCAL SelectTTFont(LPDEVICE, LPFONTINFO);
void FAR PASCAL DownloadCharacter(LPDEVICE, LPFONTINFO, BYTE);
