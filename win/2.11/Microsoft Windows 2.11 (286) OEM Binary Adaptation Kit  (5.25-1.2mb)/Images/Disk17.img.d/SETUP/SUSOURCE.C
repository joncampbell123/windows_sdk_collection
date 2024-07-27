/* SUSOURCE.C -- functions for selecting diskettes
**
** This contains functions for determining the source path for standard
** and OEM drivers, and other files.
**
** History
**
** 15 jun 87	plb	Define InsertDisk() and pSourcePath
**			also InsertOemDisk().
** 16 jun 87	plb	Made pSourcePath unsigned.
**			InsertOemDisk() now returns the length of
**			the string that is input.
** 13 aug 87    mp      if A: is destination drive, B: is default source drive
** 14 aug 87    mp      default source drive is current drive
** 31 jan 89    wch     removed references to GetCntFloppyDrives()
*/

#include "setup.h"
#include "direct.h"

/* externals from sudest.c
*/
extern BOOL DriveIsHarddisk(int);

/* global (public) variables */

CHAR * pSourcePath;    /* this points to whatever the path is used
		       ** for input of files.
		       */
/* These are changed when system diskette configuration is determined */

CHAR DefaultSPath[PATHMAX+1];
CHAR DefaultOemSPath[PATHMAX+1] = "A:";


/* InsertDisk(d) -- prompt for insertion of diskette.
**
** The user is prompted with the path or drive designation in
** DefaultSPath -- and then types ENTER, or may edit the path,
** which is returned in SPath, with the pointer pSourcePath
** set to it.
*/
InsertDisk(d)
int d;
    {
    CHAR SPath[PATHMAX+1];
    char cDrive;                        /* current drive */

    if (strlen(DefaultSPath) == 0)
	{
	cDrive = *getcwd(DefaultSPath, PATHMAX);
	if (DriveIsHarddisk(cDrive))
	    strcpy(DefaultSPath, WindowsPath[0] == 'A' ? "B:" : "A:");
	else
	    DefaultSPath[2] = 0;        /* cut path from drive letter */
	}

    PutNumber(d);
    ScrClear();
    ScrDisplay(InsertMenu);
    ScrInputPath(DefaultSPath, SPath, PATHMAX);
    strcpy(DefaultSPath, SPath);
    pSourcePath = DefaultSPath;

    } /* InsertDisk(d) */

/* InsertOemDisk(d) -- prompt for insertion of diskette.
**
** driverkind is one of the strings sInsertDisplay, sInsertKB, sInsertMouse,
** or sInsertPrinter.
**
** The user is prompted with the default path in DefaultOemSPath --
** this may be edited.
**
** Returns: length of drive or path name string.  If length is NOT 0, the
** pointer pSourcePath is set to the address of the string.
*/
int InsertOemDisk(driverkind)
CHAR * driverkind;
    {
    int len;
    CHAR OemSPath[PATHMAX+1];

    if (strncmp(WindowsPath, DefaultOemSPath, 2) == 0)
	strcpy(DefaultOemSPath, "B:");
    PutSecName(driverkind); 	/* use same insertstring ptr. as */
				/* for INF section */
    ScrClear();
    ScrDisplay(InsertOemMenu);
    ScrInputPath(DefaultOemSPath, OemSPath, PATHMAX);
    len = strlen(OemSPath);
    if (len > 0) {
	strcpy(DefaultOemSPath, OemSPath);
	pSourcePath = DefaultOemSPath;
    }
    return(len);

    } /* InsertOemDisk(d) */

