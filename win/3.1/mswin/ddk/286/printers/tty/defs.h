/*
 +-- DEFS.H for TTY --------------------------------------------------------+
 |                                                                          |
 |  Copyright (c) 1989-1990, Microsoft Corporation.			    |
 |  Modificado en 1986 por Jaime Garza V. FRALC Consultores (W 1.03)        |
 |  Modificado en 1988 por Jaime Garza V. FRALC Consultores (W 2.03)        |
 |  Modificado en 1989 por Armando Rodri'guez M. FRALC Consultores (W 3.00) |
 |  Modificado en 1989 por Jaime Garza V. FRALC Consultores (W 3.00)        |
 |                                                                          |
 +--------------------------------------------------------------------------+
*/

//	Microsoft history
//	11 Sep 91	LinS		Include all definitions
//	20 oct 89	peterbe		checked in.

//  reset.c
short NEAR PASCAL heapinit(LPDV);
HANDLE NEAR PASCAL InitQueue(LPDV);
void NEAR PASCAL DeleteQueue(LPDV);

// control.c

short	NEAR PASCAL doFirstBand(LPDV);
short	NEAR PASCAL doMaxBand(LPDV);

//  tty.asm

VOID	FAR  PASCAL SetByteValue(LPSTR, WORD, WORD);

//  file.c
extern int   far pascal GetPrinterFileName(void);
extern int   far pascal GetNumPrinters(void);
extern int   far pascal SetNumPrinters(int);
extern int   far pascal GetCurPrinter(void);
extern int   far pascal SetCurPrinter(int);
extern int   far pascal GetPrinter(int);
extern int   far pascal SetPrinter(int);
extern int   far pascal LoadThePrinter(LPESCAPECODE);
extern int   far pascal BorrarImpresora(int editing);

short FAR PASCAL myWrite(LPDV, LPSTR, short);
short FAR PASCAL FlushSpoolBuf(LPDV);

void NEAR PASCAL dump(LPDV);
void NEAR PASCAL epsstrip(WORD far *, short);
short NEAR PASCAL fake(LPDV far *, short far *, short far *);
void NEAR PASCAL line_out(LPDV, LPSTR, short, short);
BOOL NEAR PASCAL ch_line_out(LPDV, short);
FAR PASCAL YMoveTo(LPDV, short);
short FAR PASCAL XMoveTo(LPDV, short, short);
void NEAR PASCAL FindDeviceMode(LPDV, LPSTR);
long FAR PASCAL StrBlt(LPDV, short, short, LPRECT, LPSTR, short, LPFONTINFO, LPDRAWMODE, LPTEXTXFORM);


long FAR PASCAL chStrBlt(LPDV, short, short, LPSTR, short, LPFONTINFO, LPDRAWMODE, LPTEXTXFORM, LPRECT);
short FAR PASCAL  chRealizeObject(LPDV, LPLOGFONT, LPFONTINFO, LPTEXTXFORM);
long NEAR PASCAL str_out(LPDV, LPSTR, short, LPFONTINFO, LPDRAWMODE, LPTEXTXFORM);
int  FAR PASCAL ExtWidths(LPDV, BYTE, BYTE, short far *, LPFONTINFO, LPTEXTXFORM);
DWORD FAR PASCAL ExtStrOut(LPDV, short, short, LPRECT, LPSTR, short, LPFONTINFO, LPTEXTXFORM, LPDRAWMODE, short far *);

FAR PASCAL DraftStrblt(LPDV,short, short, LPSTR, short, LPFONTINFO, LPDRAWMODE, LPTEXTXFORM, LPRECT);

unsigned NEAR PASCAL Translate(BYTE, BYTE[], short, BYTE);
short NEAR PASCAL GetSpecialWidth(BYTE, short far *, short);
short NEAR PASCAL findword(LPSTR, short);
void FAR PASCAL SetWidth(LPDV, BOOL, short);
short FAR PASCAL SelectWidth(short);
short NEAR PASCAL StartStyle(LPDV, LPTEXTXFORM);
short NEAR PASCAL EndStyle(LPDV, LPTEXTXFORM);

void FAR PASCAL TextDump(LPDV, BOOL);
void NEAR PASCAL Ep_Output_String(LPDV, LPSTR, short, BYTE);
short NEAR PASCAL InsertString(LPDV, short, short, short);

short FAR PASCAL GetFaceName(LPSTR);
void NEAR PASCAL char_out(LPDV, unsigned char, unsigned char);
NEAR PASCAL myFree(LPDV, short);
short NEAR PASCAL myAlloc(LPDV, short);
void NEAR PASCAL mixbits(LPDV, LPSTR, short);
void FAR PASCAL numconv(LPSTR, short);
void FAR PASCAL FillBuffer(LPSTR, WORD, WORD);

// devmode.c
void FAR PASCAL GetDevMode(LPSTR, LPDM, LPSTR);
void NEAR PASCAL BuildDevMode( LPDM, LPSTR);
BOOL FAR PASCAL CheckDevMode( LPDM );
short	FAR PASCAL PaperID2Index(short);




// other externals
void NEAR PASCAL SetDeviceMode(LPDV, LPDM);
void FAR PASCAL GetDevMode(LPSTR, LPDM, LPSTR);
