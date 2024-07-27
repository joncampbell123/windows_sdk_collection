#define RASTER
#define IBMCOLOR	1
#define FIXED_PITCH_FONTS_ONLY		1	/* no variable pitch fonts */
#define DEVMODE_NO_PRINT_QUALITY	0	/* has quality selection */
#define DRAFTMODE_HAS_ELITE_PRINT	0	/* no elite in draftmode */
#define DEVMODE_WIDEPAPER		1	
	/* printer supports wide carriage paper format */

#include "printer.h"
#include "gdidefs.inc"
#include "ibmcolor.h"
#include "device.h"

#define DEVICENAME  "ibmcolor.exe"

#define MODULENAME "ibmcolor"

/* in _TEXT */

void FAR PASCAL myWrite(LPDEVICE, LPSTR, short);

short NEAR PASCAL myWriteSpool(LPDEVICE);

void NEAR PASCAL dump(LPDEVICE);
void NEAR PASCAL epsstrip(WORD far *, short);
short NEAR PASCAL fake(LPDEVICE far *, short far *, short far *);
void NEAR PASCAL line_out(LPDEVICE, LPSTR, short, short);
BOOL NEAR PASCAL ch_line_out(LPDEVICE, short);
FAR PASCAL YMoveTo(LPDEVICE, short);
short FAR PASCAL XMoveTo(LPDEVICE, short, short);
void NEAR PASCAL FindDeviceMode(LPDEVICE, LPSTR);
long FAR PASCAL StrBlt(LPDEVICE, short, short, LPRECT, LPSTR, short, LPFONTINFO, LPDRAWMODE, LPTEXTXFORM);


/* in _CHAR segment */

long FAR PASCAL chStrBlt(LPDEVICE, short, short, LPSTR, short, LPFONTINFO, LPDRAWMODE, LPTEXTXFORM, LPRECT);
short FAR PASCAL  chRealizeObject(LPDEVICE, LPLOGFONT, LPFONTINFO, LPTEXTXFORM);
short FAR PASCAL heapinit(LPDEVICE);
long NEAR PASCAL str_out(LPDEVICE, LPSTR, short, LPFONTINFO, LPDRAWMODE, LPTEXTXFORM);
int  FAR PASCAL ExtWidths(LPDEVICE, BYTE, BYTE, short far *, LPFONTINFO, LPTEXTXFORM);
DWORD FAR PASCAL ExtStrOut(LPDEVICE, short, short, LPRECT, LPSTR, short, LPFONTINFO, LPTEXTXFORM, LPDRAWMODE, short far *);

FAR PASCAL DraftStrblt(LPDEVICE,short, short, LPSTR, short, LPFONTINFO, LPDRAWMODE, LPTEXTXFORM, LPRECT);
FAR PASCAL short_write(LPDEVICE, PCD *);

BYTE NEAR PASCAL Translate(BYTE, short);
short NEAR PASCAL GetSpecialWidth(BYTE, short far *, short);
short NEAR PASCAL findword(LPSTR, short);
void NEAR PASCAL SetMode(LPDEVICE, LPFONTINFO);
short NEAR PASCAL StartStyle(LPDEVICE, LPTEXTXFORM);
short NEAR PASCAL EndStyle(LPDEVICE, LPTEXTXFORM);

void NEAR PASCAL Ep_Output_String(LPDEVICE, LPSTR, short);
short NEAR PASCAL InsertString(LPDEVICE, short, short, short);

short FAR PASCAL GetFaceName(LPSTR);
void NEAR PASCAL char_out(LPDEVICE, unsigned char, short, short);
NEAR PASCAL long_write(LPDEVICE, LPCD *, char);
NEAR PASCAL myFree(LPDEVICE, short);
short NEAR PASCAL myAlloc(LPDEVICE, short);
void NEAR PASCAL mixbits(LPDEVICE, LPSTR, short);
void FAR PASCAL numconv(LPSTR, short);

typedef struct  {
        char esc;
        char code;
        short cnt;
        char length;
} DELX; 

extern  short       pcsbtab[];
extern  PAPERFORMAT PaperFormat[];
extern  BYTE        Trans[];
extern  ESCAPECODE  escapecode;
extern  DELX delx;
