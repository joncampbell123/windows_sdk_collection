/*
 * link with a c data file and create fnt file.
 * Usage: creatfnt
 */

#include <printer.h>
#include <gdidefs.inc>
#include "epson.h"
#include "device.h"
#include <stdio.h>

typedef struct {
	short version;
	long size;
	char copyright[60];
} FNTHEADER;

FNTHEADER fntHeader;

extern char FntFile[];
extern FONTINFO FontInfo;
extern PRDFONTINFO PrdFontInfo;
extern unsigned char WidthFirstChar;
extern unsigned char WidthLastChar;
extern short WidthTable[];
extern char DeviceName[];
extern char FaceName[];
extern int bWidthTable;

error(ecode)
char *ecode;
{
	printf("%s", ecode);
	printf("creatfnt failed");
	fcloseall();
	exit();
}

main(argc, argv)
short argc;
char *argv[];
{
        FILE *fdwidth, *oldfd, *newfd, *headerfd;
        short width, i, j;
        short oldsize, *temp;
        short newsize;
        short short1;
	long widthptr;
	char widthfile[30];

	if (argc != 1)
		error("Usage: creatfnt\n");

        if ((newfd = fopen((char *)FntFile, "wb")) == NULL) {
                error("cannot create fnt file\n");
        }

/*****************
	if (fread((char *) &fntHeader, 1, 66, headerfd) < 66)
		error("cannot read header\n");
*****************/

	FontInfo.dfDefaultChar -= FontInfo.dfFirstChar;
	FontInfo.dfBreakChar -= FontInfo.dfFirstChar;

	if (DeviceName[0]) {
		newsize = FontInfo.dfDevice = (long) sizeof(PRDFONTINFO);
	} else {
		error ("No Device Name\n");
	}
	if (FaceName[0]) {
		newsize = FontInfo.dfFace = newsize + strlen(DeviceName) + 1;
	} else {
		error ("No Face Name\n");
	}

	if (bWidthTable) {
		newsize = PrdFontInfo.widthtable = newsize + strlen(FaceName) + 1;
		newsize = newsize + (WidthLastChar - WidthFirstChar + 1) * sizeof(short);
	} else
		PrdFontInfo.widthtable = 0;

	if (fwrite((char *)&FontInfo, 1, sizeof(FONTINFO) - 1, newfd) != sizeof(FONTINFO) -1 ||
	    fwrite((char *)&PrdFontInfo.wheel, 1, sizeof(PRDFONTINFO)-sizeof(FONTINFO)+1, newfd) != sizeof(PRDFONTINFO)-sizeof(FONTINFO)+1 ||
	    fwrite((char *)DeviceName, 1, strlen(DeviceName)+1, newfd) != strlen(DeviceName) + 1 ||
	    fwrite((char *)FaceName, 1, strlen(FaceName)+1, newfd) != strlen(FaceName) + 1 ||
	    (bWidthTable ?
	    	(fwrite((char *)WidthTable, sizeof(short), WidthLastChar-WidthFirstChar+1, newfd) != WidthLastChar-WidthFirstChar+1) :
	    	0)
	    )
	       error("cannot write to fnt file\n");

/**************
	FontInfo.dfFace += 66;
	FontInfo.dfDevice += 66;
	fntHeader.size = (long)newsize + 66;
	if (fwrite((char *)&fntHeader, 1, 66, newfd) != 66 ||
	    fwrite(newbuffer, 1, newsize, newfd) != newsize)
	       error("cannot write to new font file\n");
	newsize += 66;
**************/
	fclose(newfd);
}
