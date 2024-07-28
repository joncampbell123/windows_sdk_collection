/**[f******************************************************************
 * adobe.c - 
 *
 * Copyright (C) 1988 Aldus Corporation.  All rights reserved.
 * Copyright (C) 1989 Microsoft Corporation.
 * Company confidential.
 *
 * msd 21-Mar-91  Made Bin2Ascii explicitly far pascal so other modules
 *                can call it.
 *
 **f]*****************************************************************/

#include "pscript.h"
#include <winexp.h>
#include "adobe.h"
#include "utils.h"
#include "debug.h"
#include "channel.h"
#include "printers.h"
#include "psdata.h"

typedef struct{
	unsigned char flag;
	char type;
	long length;
} HDR;


/****************************************************************************/

void FAR PASCAL DoAdobeFont(lpdv,fh,buf,bufSize)
	LPDV lpdv;
	int fh;
	LPSTR buf;
	int bufSize;
{
	HDR hdr;

	DBMSG(((LPSTR)">DoAdobeFont(): Converting Adobe font file HDR=%d\n",
		sizeof(HDR)));

	do{
		if (_lread(fh, (LPSTR)&hdr, sizeof(HDR))!=sizeof(HDR))
		    goto ERR;

		DBMSG(((LPSTR)" DoAdobeFont(): flag=%d, type=%d, length=%ld\n",
			(int)hdr.flag, (int)hdr.type, hdr.length));

		/* type=1 --> the data is ASCII */
		if(hdr.type==1){
			DBMSG(((LPSTR)" DoAdobeFont(): ASCII data\n"));
			if(!PSWriteSpool(lpdv,fh,hdr.length,buf,bufSize,FALSE)){
				goto ERR;
			}
		}

		/* type=2 --> the data is binary, convert to ASCII */
		else if(hdr.type==2){
			DBMSG(((LPSTR)" DoAdobeFont(): binary data\n"));
			if(!PSWriteSpool(lpdv,fh,hdr.length,buf,bufSize,TRUE)){
				goto ERR;
			}
		}
#ifdef DEBUG_ON
		else DBMSG(("EOF encountered\n"));
#endif
	} while(hdr.type<3);

	_lclose(fh);

goto END;

ERR:
	DBMSG(("READ ERROR\n"));
	goto END;

END:;

	DBMSG(((LPSTR)"<DoAdobeFont():\n"));
}


/****************************************************************************/

BOOL PSWriteSpool(lpdv,fh,length,buf,bufSize,fAscii)
	LPDV lpdv;
	int fh;
	long length;
	LPSTR buf;
	int bufSize;
	BOOL fAscii;	/* TRUE-->convert to 2 ASCII chars */
{
	long nBuffers;
	int nRemaining;
	long i;

	/* If we are going to convert the data into ASCII format
	 * then shrink the buffer size to accomodate the resulting
	 * increase in data size.
	 */
	if(fAscii) bufSize /= 2;

	nBuffers=ldiv(length,(long)bufSize);
	nRemaining=(int)lmod(length,(long)bufSize);

	DBMSG(((LPSTR)">PSWriteSpool(): l=%ld,bS=%d,nB=%ld,nR=%d\n",
		length,bufSize,nBuffers,nRemaining));

	for(i=0;i<nBuffers;i++){
		DBMSG(((LPSTR)" PSWriteSpool(): i=%ld\n",i));
		if((int)_lread(fh, buf, bufSize)!=bufSize) goto ERR;
#ifdef DEBUG_ON
DBMSG(((LPSTR)" PSWriteSpool():"));
for(j=0;j<bufSize;j++) DBMSG(((LPSTR)" %d",buf[j]));
DBMSG(((LPSTR)"\n"));
#endif
		if(fAscii){
			Bin2Ascii(buf,bufSize);
			bufSize *= 2;
		}
#ifdef DEBUG_ON
DBMSG(((LPSTR)" PSWriteSpool():"));
for(j=0;j<bufSize;j++) DBMSG(((LPSTR)" %d",buf[j]));
DBMSG(((LPSTR)"\n"));
#endif
		WriteChannel(lpdv,buf,bufSize);
		if(fAscii){
			bufSize /=2;
/*			PrintChannel(lpdv,(LPSTR)"\n"); */
		}
	}
	if(nRemaining>0){
		DBMSG(((LPSTR)" PSWriteSpool(): remaining bytes\n"));
		if((int)_lread(fh, buf, nRemaining)!=nRemaining) goto ERR;
#ifdef DEBUG_ON
DBMSG(((LPSTR)" PSWriteSpool():"));
for(j=0;j<nRemaining;j++) DBMSG(((LPSTR)" %d",buf[j]));
DBMSG(((LPSTR)"\n"));
#endif
		if(fAscii){
			Bin2Ascii(buf,nRemaining);
			nRemaining *= 2;
		}
#ifdef DEBUG_ON
DBMSG(((LPSTR)" PSWriteSpool():"));
for(j=0;j<nRemaining;j++) DBMSG(((LPSTR)" %d",buf[j]));
DBMSG(((LPSTR)"\n"));
#endif
		WriteChannel(lpdv,buf,nRemaining);
		PrintChannel(lpdv,newline);
	}
	DBMSG(((LPSTR)"<PSWriteSpool():\n"));
	return(1);

ERR:
	DBMSG(((LPSTR)"<PSWriteSpool(): ERROR\n"));
	return(0);
}


/****************************************************************************/
/* Convert an array of Binary characters to an array of ASCII characters
 */
void FAR PASCAL Bin2Ascii(buf,bufSize)
	LPSTR buf;
	int bufSize;
{
	int i,j;
	unsigned char temp;

	DBMSG(((LPSTR)"Bin2Ascii():\n"));
	/* Expand the contents of the buffer so that 1 byte Binary
	 * will become 2 bytes ASCII.  This is done from the end of
	 * the array so that the initial data is not over written.
	 */
	j=bufSize<<1;
	for(i=bufSize-1;i>=0;i--){
		temp=(unsigned char)buf[i];
		DBMSG(((LPSTR)"[%d] %d",i,(int)temp));

		buf[--j]=(char)Nibble2HexChar((unsigned char)(temp & 0x0f));
		buf[--j]=(char)Nibble2HexChar((unsigned char)(temp>>4));
		DBMSG(((LPSTR)"  [%d %d]: %d %d\n",j,j+1,(int)buf[j],(int)buf[j+1]));
	}
}


/****************************************************************************/
/* Convert a Binary Nibble to a single Hex (ASCII) character.
 */
unsigned char Nibble2HexChar(c)
	unsigned char c;
{
	unsigned char nibble;

	if (c < 10) 
		nibble = (unsigned char)('0'+c);
	else 
		nibble = (unsigned char)('A'+c-10);

	return nibble;
}


