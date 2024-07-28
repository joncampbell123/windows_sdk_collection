/*
 * adobe.h
 *
 */

BOOL FAR PASCAL DumpNormalizedTransfer(LPDV);

void FAR PASCAL DoAdobeFont(LPDV, int, LPSTR,int);
BOOL PSWriteSpool(LPDV,int,long,LPSTR,int,BOOL);

void FAR PASCAL Bin2Ascii(LPSTR,int);
unsigned char Nibble2HexChar(unsigned char);


