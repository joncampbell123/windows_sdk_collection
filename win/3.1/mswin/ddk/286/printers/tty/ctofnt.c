/*/  CTOFNT.C for TTY
 +-----------------------------------------+
 | Fralc Consultores (c), Mexico 1986-1989 |
 +-----------------------------------------+
/*/

// ------------------------------------------------------------------------
// Copyright (C) 1989-1990, Microsoft Corporation. All rights reserved.
// ------------------------------------------------------------------------
//	Microsoft History (latest first)
//	20 oct 89	peterbe		Checked in.
// ------------------------------------------------------------------------

/*
 * ligar con un "c data file" y crear un "fnt file".
 * Uso: crearfnt
 *
 * This links with a C font data file, and creates a binary font
 * resource file.  It is a DOS program, not a Windows program.
 */

#include "printer.h"
#include <gdidefs.inc>
#include "tty.h"
#include "devmode.h"
#include "device.h"
#include <stdio.h>
#include <process.h>


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

void error(char *ecode)
{
    printf("%s", ecode);
    printf("creatfnt failed");
    fcloseall();
    exit(0);
}

void main(short argc, char *argv[])
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

    fclose(newfd);
}
