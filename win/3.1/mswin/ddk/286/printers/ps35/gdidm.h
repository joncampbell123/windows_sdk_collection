

/*--------------- driver helper routines supplyed by GDI -----------------*/

int  FAR PASCAL dmRealizeObject(LPPDEVICE, int, LPSTR, LPSTR, LPTEXTXFORM);
int  FAR PASCAL dmOutput(LPPDEVICE, int, int, LPPOINT, LPPEN, LPBR, LPDRAWMODE, LPRECT);
long FAR PASCAL dmStrBlt(LPPDEVICE, int, int, LPRECT, LPSTR, int, LPFONTINFO, LPDRAWMODE, LPTEXTXFORM);
BOOL FAR PASCAL dmBitblt(LPPDEVICE, int, int, LPBITMAP, int, int, int, int, long, long, LPDRAWMODE);

CO  FAR PASCAL dmPixel(LPPDEVICE, int, int, CO, LPDRAWMODE);
int FAR PASCAL dmScanLR(LPPDEVICE, int, int, CO, int);
RGB FAR PASCAL dmColorInfo(LPPDEVICE, RGB, LPCO);


