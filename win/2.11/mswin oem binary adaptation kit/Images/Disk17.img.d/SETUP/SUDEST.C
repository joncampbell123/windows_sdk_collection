/* SUDEST.C -- entering Windows destination, creating subdir,
**             everything special for diskette setup
**
** Errors in this module exit with return code 17
*/
/*  History to oct 1987 .. for more, see slm update log.
**
**   1 may 87	plb SUFLOP.C written
**  17 jun 87	mp  SUFLOP renamed to SUDEST and main functions written
**  18 jun 87	mp  debugged, WindowsPath moved from SUSEL.C
**  19 jun 87	mp  ReinstallMsg and MakeWindowsPath() added
**  20 jun 87   mp  move texts to SUDATA.C
**  17 jul 87   mp  no ScrClear() at the end of SetPath()
**  19 jul 87   mp  allow fomatting on default destination drive
**  21 jul 87   mp  only harddisk setup for Windows/386
**  22 jul 87   mp  improve error message for "cannot create subdir"
**  28 jul 87   mp  IsHarddiskSystem() added for default destination medium
**                  create subdirectory for pifs
**  31 jul 87   mp  formatting of system and startup disk
**   3 aug 87   mp  eliminate flashing messages by removing a ScrClear()
**   4 aug 87   mp  no creation of pif directory for runtime version
**  14 aug 87   mp  GetCntFloppyDrive() is now global function
**  19 aug 87   mp  add removing of pif files
**  24 aug 87   mp  fix bug in make pif directory
**  21 sep 87   mp  add CheckOldVersion()
**   1 oct 87   mp  add CheckCurrentDir()
**  31 jan 89   wch removed references to IsHarddiskSystem() because it was
**			always returning TRUE
**  31 jan 89   wch removed references to GetCntFloppyDrives() because it was
**			never called
**  31 jan 89   wch removed references to IsTwoDiskette, Is360K and
** 			IsDiskette because they were always FALSE
*/
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <io.h>
#include <string.h>
#include <dos.h>
#include <newexe.h>
#include <sys\types.h>
#include <sys\stat.h>
#include <direct.h>
#include "setup.h"

#define BUFLENGTH 512  /* buffer length for config.sys file */

/* external from SUMAIN.C */
extern BOOL Is386;
extern BOOL bConfigBackedUp;
extern char szBootDrive[];

/* global variable */
CHAR WindowsPath[PATHMAX];

/* This path is used while copying the new RAMDRIVE.SYS */
CHAR RamDrivePath[PATHMAX];
BOOL bCopyRamDrive = FALSE;	/* indicates users choice whether to update the 
											ramdrive.sys or not*/


/* externals from SUDATA.C */
extern CHAR smDirErr[];
extern CHAR smCurrentDir[];
extern CHAR smRAMDRVBackUpFail[];
extern CHAR smCONFIGBackUpFail[];
extern CHAR smCopyHimemFail[];
extern CHAR smHimemExists[];
extern textline DirErrMsg[];
extern textline CopyHimemMsg[];
extern textline PathMenu[];
extern textline RamDriveMsg[];
extern textline RamDriveChoices[];
extern textline ConfigMsg[];
extern textline YesNoChoices[];
extern textline BootDriveMsg[];	/* for config.sys */
extern textline BootDriveMsg1[]; /* for autoexec.bat */
extern char smAutoExecModfFail[];

/* statics */
static CHAR ConfigDelimiter[] = "\\=:\x0d/ ";
static CHAR PathDelimiter[] = ";";

static CHAR DirDelimiter[] = "\\/";

/* forward declaration */
static void MakePifPath();
static void CheckCurrentDir();
BOOL DriveIsHarddisk(int);
char LocateBootDrive();



/*  GetCntFloppyDrives() -- get count of floppy drives.
**
**
**  Output: returns number of floppy drives.
*/

/* BILLHU removed 1/31/89 because never called
int GetCntFloppyDrives()
{
    union REGS inregs;
    union REGS outregs;

*/
    /* Call ROM BIOS (equiment check) to get floppy information */
/*
    int86(0x11, &inregs, &outregs);
    return( ((outregs.h.al >> 6) & 3 ) + 1);
}
*/


/*  DriveIsHarddisk() -- test whether drive letter specifies harddisk/netdrive
**
**  Input:  drive letter
*/

int DriveIsHarddisk(Drive)
int Drive;		/* upper-case letter, actually */
{
    union REGS inregs;
    union REGS outregs;
    struct SREGS segregs;
    unsigned char far *fpc;

    inregs.x.ax = 0x4408;           /* DOS: IOCTL -- */
				    /* Check if Block device is removable */
    inregs.h.bl = Drive - 'A' + 1;	/* 1 == drive A:, etc. */

    intdosx(&inregs, &outregs, &segregs);

    if (0 != outregs.x.cflag)
	return(FALSE);			/* invalid drive */
    else
	return(1 == outregs.x.ax);	/* 1 if medium is not removable */
					/* 0 if medium is removable */

} /* end DriveIsHarddisk() */



/*  IsHarddiskSystem() -- test whether C: is a harddisk
*/

/* This was removed by bobgu on 8/30/88.  We no longer do any testing for
 * hard drives.  This was required to run on things like a Novel network
 * system w/o a disk since they report the virtual drives as floppies
 */
/* BILLHU totally removed on 1/31/89 here and elsewhere
BOOL IsHarddiskSystem()
{
#if 0
    char Drive;

    for (Drive = 'C'; Drive <= 'Z'; Drive++) {
	if (DriveIsHarddisk(Drive)) {
	    return(TRUE);
	}
    }
    return(FALSE);
#else
    return(TRUE);
#endif
}
*/


/*  DoInitDisk() -- prompt for diskette/harddisk, formatting, destination path
**
*/

void DoInitDisk()
{
	CHAR cDrive;
    CHAR CurrentPath[PATHMAX];          /* current working dir */

    cDrive = *getcwd(CurrentPath, PATHMAX);
					/* compute default path */
    strcpy(WindowsPath, inserttext[D_DIRECTORY]);
    SetPath(PathMenu);
}


/* SetPath () -- input pathname for WINDOWS directory, create subdirectory
**		 if necessary, test whether drive is accessible, clear screen
**
** Input:  pointer to a menu which has to be displayed
*/

void SetPath(Menu)
textline *Menu;
{
    CHAR Path[PATHMAX];
    FINDTYPE findbuf;
    int iLen;

again:
    ScrClear();
    ScrDisplay(Menu);
    ScrInputPath(WindowsPath, WindowsPath, 60);

    CheckCurrentDir();

    strcat(strcpy(Path, WindowsPath), "*.*");

    if (ffirst(Path, 0, &findbuf) != 3) {           /* try to find subdir */
	MakePifPath();
	return;					    /* subdirectory exists */
    }
    if ((iLen = strlen(WindowsPath)) > 2) {  /* not only a drive letter */
	strcpy(Path, WindowsPath);
	Path[iLen - 1] = '\0';			       /* eliminate '\' */

	if (mkdir(Path)) {                  /* try to make new directory */
		ScrClear();
	   PutDestName(Path);
		ScrDisplay(DirErrMsg);
		ContinueOrQuit();
		strcpy(WindowsPath, "C:\\WINDOWS");
		goto again;
	}
	MakePifPath();
    }
} /* SetPath */


/* MakeWindowsPath () -- makes subdirectory if necessary
**
*/

void MakeWindowsPath()
{
    CHAR Path[PATHMAX];
    FINDTYPE findbuf;
    int iLen;

    strcat(strcpy(Path, WindowsPath), "*.*");

    if (ffirst(Path, 0, &findbuf) != 3) {           /* try to find subdir */
	MakePifPath();
	return;					    /* subdirectory exists */
    }
    if ((iLen = strlen(WindowsPath)) > 2) {  /* not only a drive letter */
	strcpy(Path, WindowsPath);
	Path[iLen - 1] = '\0';			       /* eliminate '\' */

	if (mkdir(Path)) {                  /* try to make new directory */
	    PutDestName(Path);
	    FatalError(smDirErr, 1);
	}
	MakePifPath();
    }
}

/* MakePifPath() -- makes pif subdirectory if necessary
**
*/

static void MakePifPath()
{
    CHAR Path[PATHMAX];
    FINDTYPE findbuf;
    int status;

    if (!IsQuick && !IsRuntime) {
	strcat(strcpy(Path, WindowsPath), "PIF");
						      /* try to find subdir */
	if ((status = ffirst(FileName(Path, "\\*.pif"), 0, &findbuf)) != 3) {
	    strcat(Path, "\\");
		         /* delete PIFs for Win386 */
	    while (!status && Is386) {
		remove(FileName(Path, findbuf.name));
		status = fnext(&findbuf);
	    }
	} else if (mkdir(Path)) {              /* try to make new directory */
	    PutDestName(Path);
	    FatalError(smDirErr, 1);
	}
    }
}

/* ChangeToWindowsPath () -- change current directory to WindowsPath
**
*/

void ChangeToWindowsPath()
{
    CHAR Path[PATHMAX];
    int iLen;

    strcpy(Path, WindowsPath);

    if ((iLen = strlen(WindowsPath)) > 2) {  /* not only a drive letter */
	Path[iLen - 1] = '\0';			       /* eliminate '\' */
    }
    chdir(Path);

    bdos(0x0E, Path[0] - 'A', 0);		/* select drive */
}


/* CheckOldVersion() -- Check for Ramdrive in Config.sys 
  provide the option of updating to the new one. 
 */
void CheckOldVersion()
{
CHAR buffer[BUFLENGTH];
CHAR *token;
CHAR Path[PATHMAX];
FINDTYPE findbuf;
BOOL oldwin = FALSE;
int iChoice, n;
char file1[PATHMAX], file2[PATHMAX];


		/* no need to detect old ramdrive or old Windows version. */
	if (IsQuick)
		return;

	if (LocateRamDrive(RamDrivePath) == 1)	{
		ScrClear();
		ScrDisplay(RamDriveMsg);
		iChoice = ScrSelectChar(RamDriveChoices, 0);
		if (iChoice == 0) {
			bCopyRamDrive = TRUE;
			strcpy(file1, RamDrivePath);
			strcpy(file2, RamDrivePath);
			/* create a back up of old Ramdrive.sys under the name ramdrive.old */
			n = CreateBackup(strcat(file1, "RAMDRIVE.SYS"), 
								strcat(file2, "RAMDRIVE.OLD"));
			if (n == -1)
				FatalError(smRAMDRVBackUpFail, 1);
		}
	}

}


/* CheckOldWin() -- Check for Old Windows in the path.
 */
int CheckOldWin()
{
CHAR buffer[BUFLENGTH];
CHAR *token;
CHAR Path[PATHMAX];
FINDTYPE findbuf;
BOOL oldwin = FALSE;

	if (IsQuick)
		/* no need to detect old Windows version */
		return;				

	strcpy(buffer, DosPath);
	token = strtok(buffer, PathDelimiter);
	while (token) {
   	strcpy(Path, token);
		if (Path[strlen(Path) - 1] != '\\')
			strcat(Path, "\\");
	   	if (oldwin = (ffirst(FileName(Path, "WIN?00.BIN"), 0,
			 &findbuf) == 0))
			break;
	   	token = strtok(NULL, PathDelimiter);
	}

	if (oldwin) 
		return TRUE;
	else
		return FALSE;
}

/* Creates a backup of the srcfname under the dstfname.
 * returns -1 on ERROR. */
CreateBackup(srcfname, dstfname)
char *srcfname, *dstfname;
{
FILE *in, *out;
size_t  r;
unsigned date,time;
int in_time, out_time, out_flush;
char buffer[BUFLENGTH];

	/* Open the source and dest files. */

	in = fopen(srcfname,"rb");
	out = fopen(dstfname,"wb");
	if (in == NULL || out == NULL) 
		return -1;
	
	/* Copy from source to dest. */

	for(r=1; r!=0;) {
		r = fread ((char *)buffer, 1, sizeof(buffer), in);
		if (fwrite((char *)buffer, 1, r,  out) != r)	
			return -1;
	}

/* 
 * Set the destination file's modification time to match the source.  Note
 * that we must flush the dest file before setting the time in order to
 * ensure that a subsequent write does not occur which would overwrite the
 * modification time. 
 */
	in_time = _dos_getftime( fileno(in), &date, &time);
	out_flush = fflush(out);
	out_time = _dos_setftime( fileno(out), date, time);

	if ((in_time) || (out_time) || (out_flush))
		return -1;

	fclose(in);
	fclose(out);
	return 0;

}



/* Locate the RAMDRIVE.SYS if any on the user machine. 
 * returns -1 on ERROR, 
 * 			0 if noRAMDRIVE.SYS, 
 *				1 if an Old Ramdrive.sys exists */
LocateRamDrive(path)
char path[];
{

char bootdrive = ' ', file[20];
char line[101];
int found = FALSE, i;
char *p, *p1,*p2;
FILE *fp;

	/* Locate a Hard Disk Drive with CONFIG.SYS at it's root. */
	if ((bootdrive = LocateBootDrive()) == ' ')
		return 0;

	file[0] = bootdrive;
	file[1] = '\0';
	strcat(file, ":\\CONFIG.SYS");

	/* read the config.sys file from bootdrive. */
	fp = fopen(file, "r");
	if (fp == NULL)	/* cannot open config.sys file */
		return -1;
	
	while (fgets(line, 100, fp))	{
		if (strstr(strupr(line), "RAMDRIVE.SYS"))	{
			found = TRUE;
			break;
		}
	}
   fclose(fp);

	if (!found)		/* No RAMDRIVE.SYS entry in config.sys */
		return 0;

	p = strchr(line, '=');
	if (p == NULL)
		return 0;
	p++; /* step over the '=' */	
	/* skip blanks */
	while (*p == ' ')
		p++;

	p1 = p;
	
	while (*p != '\0' && *p != '\n' && *p != ' ' && *p != 0x0d)
		p++;

	p2 = p;

	for (p = p1, i = 0; p != p2; p++)	
		path[i++] = *p;
		
	path[i] = '\0';

	/* Ensure RAMDRIVE.SYS exists in path */
	if (!strstr(path, "RAMDRIVE.SYS"))	
		return 0;

	if (path[1] != ':')	{	/* no drive letter */
		InsertC(':', path);
		InsertC(bootdrive, path);
	}

	if (path[2] != '\\')	 /* path doesn't begin with a backslash. */
		InsertC('\\', &path[2]);
			
	/* In the final piece of code, add more code to see if ramdrive.sys 
	 * exists at the finally constructed path. This should be easy	to
    * do (access(path or newpath, 0 ) == 0) ? */

	if (access(path, 0) != 0)
		return 0;

	p = strrchr(path, '\\');
	if (p == NULL)
		return 0;
	/* truncate the path at beginning of filename. */
	*(p+1) = '\0';

	
	return 1;
}

InsertC(c, str)
char c;
char *str;
{
char newstr[PATHMAX];

	newstr[0] = c;
	strcpy(&newstr[1], str);
	strcpy(str, newstr);
}

/* returns the hard disk drive name which has a config.sys at it's root.
 *  if not able to find a hard disk drive with config.sys at the root 
 *  it returns a ' '
 */

char LocateBootDrive()
{
char drive, file[20];

	/* Locate a Hard Disk Drive with CONFIG.SYS at it's root. */
	for (drive = 'C'; drive < 'Z'; drive++)	{
		if (DriveIsHarddisk(drive))	{
			file[0] = drive;
			file[1] = '\0';
			strcat(file, ":\\CONFIG.SYS");
			if (access(file, 0) == 0)	{
				return drive;
			}
		}
	}

	/* couldn't locate a hard disk drive with config.sys at it's root */
	return ' ';
}

/* returns 0 on success only. */

InsertHimemLine()
{
char bootdrive, file[20], buffer[5];
int iChoice;
FILE *fp;
char path[PATHMAX];
char srcpath[PATHMAX];

		bootdrive = LocateBootDrive();
		if (bootdrive != ' ')	{
			szBootDrive[0] = bootdrive;
			szBootDrive[1] = '\0';
			ScrClear();
			ScrDisplay(ConfigMsg);
			iChoice = ScrSelectChar(YesNoChoices, 0);
			if (iChoice == 0)
				return (ModifyConfig(bootdrive));
			else
				return 0;
		}
		else	{
			ScrClear();
			ScrDisplay(BootDriveMsg);
			ScrInputPath("C:", buffer, 4);
			file[0] = buffer[0];
			file[1] = '\0';

			strcpy(szBootDrive, file);

			strcat(file, ":\\CONFIG.SYS");
 
			if (access(file, 0) == 0)	{
				ScrClear();
				ScrDisplay(ConfigMsg);
				iChoice = ScrSelectChar(YesNoChoices, 0);
				if (iChoice == 0)
					return (ModifyConfig(buffer[0]));
				else
					return 0;
			}
			else {

				PutHimemAtRoot(*buffer);

				strcpy(path, "DEVICE=");
				path[7] = buffer[0];
				path[8] = '\0';
				strcat(path,":\\HIMEM.SYS\n");
 
				if (creat(file, S_IWRITE) == -1)
					return -1;
				if (! (fp = fopen(file, "w")))
					return -1;
				if (fputs(path, fp))
					return -1;
				fclose(fp);
				
			}
		}

}

PutHimemAtRoot(drive)
char drive;
{
char spath[PATHMAX];
char dpath[PATHMAX];
	
	strcpy(spath, WindowsPath);
	strcat(spath, "HIMEM.SYS");
	
	dpath[0] = drive;
	dpath[1] = '\0';
	strcat(dpath, ":\\HIMEM.SYS");

/*	if (access(dpath, 0) == 0)	{
		PutFileName(dpath);
		FatalError(smHimemExists, 1);
	}
*/

	if (CreateBackUp(spath, dpath) == -1)	{
		PutFileName(dpath);
		FatalError(smCopyHimemFail, 1);
	}

	remove(spath);
	return;
}

ModifyConfig(drive)
char drive;
{
FILE *fp1, *fp2;
char line[100], temp[100], path[100];
int found = FALSE;
char file1[20], file2[20];

	if (drive < 'A' || drive > 'Z')
		return -1;

	PutHimemAtRoot(drive);

	file1[0] = file2[0] = drive;
	file1[1] = file2[1] = '\0';
	strcat(file1, ":\\CONFIG.BAK");
	strcat(file2, ":\\CONFIG.SYS");

	remove(file1);

	if (CreateBackup(file2, file1) == -1)
		FatalError(smCONFIGBackUpFail, 1);
	else
		bConfigBackedUp = TRUE;

	if(!(fp1 = fopen(file1, "r")) )
		return -1;

	if(!(fp2 = fopen(file2, "w")) )
		return -1;

	strcpy(path, "DEVICE=");
	path[7] = drive;
	path[8] = '\0';
	strcat(path,":\\HIMEM.SYS\n");

	while (fgets(line, 100, fp1))	{
		strcpy(temp, line);
		if (!found && strstr(strupr(temp), "DEVICE"))	{
			found = TRUE;
			if (fputs(path, fp2))
				return -1;
		}
		if (fputs(line, fp2))
			return -1;
	}

	if (!found) {
		if (fputs(path, fp2))
			return -1;
	}

   fclose(fp1);
   fclose(fp2);

	return 0;
}


ModifyAutoexec()
{
char bootdrive, buffer[5];
int iChoice;
char path[PATHMAX];

	bootdrive = szBootDrive[0];

	if (!bootdrive)	{			/* if we do not know the boot drive */
		bootdrive = LocateBootDrive();
		if (bootdrive == ' ')	{	/* ask the user, SETUP can't locate it. */
			ScrClear();
			ScrDisplay(BootDriveMsg1);
			ScrInputPath("C:", buffer, 4);
			bootdrive = buffer[0];
		}
	}

	strcpy(path, WindowsPath);

	if (InsertWinPath(bootdrive, path) == -1)
		FatalError(smAutoExecModfFail, 17);

}


/* ===========================================================================

    InsertWinPath(sBoot,sWinPath);

    This version of InsertWinPath copies AutoExec.Bat to a temporary
    file, scanning for PATH, PATH =, and SET PATH = statements.
    The last path scanned is saved.  Finally, a PATH statement
    of the form

	PATH c:\NewWindows;<old path>

    is appended to the copy of AUTOEXEC.BAT file, and the copy and
    the original are renamed so that the original is AUTOEXEC.BAK
    and the new file is AUTOEXEC.BAT.  The input file is read
    in binary mode so that control-z's and dangling (EOL-less) lines
    can be handled.  The output file is opened in text mode.

    If no AUTOEXEC.BAT file is found, a new one containing
    the line

	PATH c:\;c:\NewWindows

    is created.

=========================================================================== */
#define LINESIZE 257

InsertWinPath(cBoot, sWinPath)
char cBoot;			/* Boot drive */
char * sWinPath;		/* full path of Windows */
{
    FILE *fileIn;
    FILE *fileOut;
    int Count;
    char sBootPath[5];		/* e.g. "C:\\" */

    char sPathIn[20];		/* e.g. "C:\\autoexec.bat" */
    char sPathOut[20];		/* e.g. "C:\\autoexec.000" */
    char sPathBak[20];		/* e.g. "C:\\autoexec.old" */
    char linebuf[LINESIZE];	/* line buffer */
    char oldpath[LINESIZE];	/* Path from old PATH statement */
    char ch;
    int iLen;

    /* strip trailing backslash from Windows path */

    if (strlen(sWinPath) > 3)
	if (sWinPath[strlen(sWinPath)-1] == '\\')
	    sWinPath[strlen(sWinPath)-1] = '\0';

    /* Create pathname for AUTOEXEC.BAT */
    strcpy(sBootPath, "C:\\");
    * sBootPath = cBoot;

    strcpy(sPathIn, sBootPath);
    strcpy(sPathOut, sBootPath);
    strcpy(sPathBak, sBootPath);

    strcat(sPathIn, "AutoExec.Bat");
    strcat(sPathOut, "AutoExec.000");
    strcat(sPathBak, "AutoExec.BAK");

    *oldpath = '\0';

    if (NULL == (fileIn = fopen(sPathIn, "rb")))
	{
	/* can't open OLD Autoexec.Bat .. so try to create a new one. */
	if (NULL == (fileOut = fopen(sPathIn, "w")))
	    /* can't create for some foolish reason */
	    return(FALSE);
	else
	    {
	    /* Write path statement to completely new AutoExec.Bat */
	    strcpy(linebuf, "PATH ");
	    strcat(linebuf, sBootPath);
	    strcat(linebuf, ";");
	    strcat(linebuf, sWinPath);
	    fputs(linebuf, fileOut);
	    fclose(fileOut);
	    }
	}
    else
	{
	/* We COULD open the old AutoExec.Bat.
	** open 'AutoExec.000' in text mode for write.
	*/
	if (NULL == (fileOut = fopen(sPathOut, "w")))
	    {
	    fclose(fileIn);
	    return(FALSE);
	    }
	else
	    {
	    iLen = 0;
	    do	{
		/* input a line */
		while (((ch = fgetc(fileIn)) != EOF) && (ch != '\n'))
		    {
		    /* skip carriage returns and control-Z's */
		    if ((ch != '\015') && (ch != 26))
			linebuf[iLen++] = ch;
		    }
		linebuf[iLen] = '\000';
		/* if this is a path statement, save the path list */
		FindPath(linebuf, oldpath);

		/* write the line out */
		if ((ch != EOF) || (iLen > 0))
		    {
		    fputs(linebuf, fileOut);
		    fputs("\n", fileOut);
		    }

		iLen = 0;
		} while (ch != EOF);

	    /* append new path statement to end of file */

	    strcpy(linebuf, "PATH ");
	    strcat(linebuf, sWinPath);
	    strcat(linebuf, ";");
	    strcat(linebuf, oldpath);
	    strcat(linebuf, "\n");

	    fputs(linebuf, fileOut);

	    fclose(fileOut);
	    fclose(fileIn);

	    /* now remove any old backup file, and rename the 2 files ... */

	    remove(sPathBak);
	    if (0 == rename(sPathIn, sPathBak))
		rename(sPathOut, sPathIn);

	    }
	}

    return(TRUE);

}	/* end InsertWinPath() */

/* ========== FindPath() ==============================================
**
** scan for 'PATH ', 'SET PATH=', or 'PATH=' statement,
** copy actual path parameter into string 'path'.
**
======================================================================*/

FindPath(buf, path)
char * buf;
char * path;
{
    char * p;
    int len;
    int siz;
    char *pS;
    char *pD;

    /* find first nonblank character (not space or tab) in buf */
    p = buf + strspn(buf, " \t");

    /* look for "path " or "path=" or "set ".  Ignore case. */
    if ((0 == strnicmp(p, "path ", 5) ) ||
	(0 == strnicmp(p, "path=", 5) ) )
	{ 
	/* scan past blanks or equals sign */
	p += 5 + strspn(p + 5, "= \t");
	}
    else if (0 == strnicmp(p, "set ", 4))
	{
	/* this may be "set path", so look for "path" and '=' */
	/* scan past blanks */
	p += 4 + strspn(p + 4, " \t");

	/* we want "path" */
	if (0 == strnicmp(p, "path", 4))
	    {
	    p += 4 + strspn(p + 4, " \t");
	    /* we want '=' */
	    if (*p++ != '=')
		return(0);
	    }
	else
	    return(0);
	}
    else
	return(0);

    p += strspn(p, " \t");

    /* now just return the pointer to the path list */

    strcpy(path, p);

    return(1);

}	/* end FindPath */



/* CheckCurrentDir() -- check whether the destination path (WindowsPath)
**                      is the current path and abort in that case
**                      (WindowsPath starts with drive letter in any case
**                      and ends with '\')
*/						 
static void CheckCurrentDir()
{
    CHAR CurrentPath[PATHMAX];          /* current working dir */
    CHAR RelativePath[PATHMAX];
    CHAR AbsolutePath[PATHMAX + 40];
    CHAR *token;

    getcwd(CurrentPath, PATHMAX);
    if (strlen(CurrentPath) == 3)       /* root dir */
	CurrentPath[2] = '\0';              /* skip \ */

    if (*WindowsPath != *CurrentPath)   /* not on the same drive? */
	return;

    strcpy(RelativePath, WindowsPath);
    strncpy(AbsolutePath, RelativePath, 2);     /* copy drive letter */
    AbsolutePath[2] = '\0';

    if (RelativePath[2] != '\\')        /* does it start relative or absolut*/
	strcpy(AbsolutePath, CurrentPath);

    token = strtok(RelativePath + 2, DirDelimiter);
    while (token) {
	if (strcmp(token, ".") == 0)
	    strcpy(AbsolutePath, CurrentPath);
	else if (strcmp(token, "..") == 0)
	    *strrchr(AbsolutePath, '\\') = '\0';
	else
	    strcat(strcat(AbsolutePath, "\\"), token);

	token = strtok(NULL, DirDelimiter);
    }

    if (strcmp(AbsolutePath, CurrentPath) == 0)
	FatalError(smCurrentDir, 17);
}
