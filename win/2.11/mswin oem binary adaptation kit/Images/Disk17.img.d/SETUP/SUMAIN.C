/* SUMAIN -- Setup main module
*/

/* This is the main module of SETUP for Windows 2.0 and Windows/386
**
** Errors in this module exit with return code 8
**
** History
**
**  5 jun 87    plb Made IsQuick() routine return value of Quick.
**  9 jun 87    plb Displaying [fonts] list.
** 13 jun 87    plb Testing DiskIsIncluded();
**  ........
** 19 jun 87    mp  Test version changed to final functionality
** 16 jul 87    mp  Add -d command line option for destination directory
** 18 jul 87    mp  Improve error message for missing Setup.inf file
**                  allowing pathname in lowercase
** 19 jul 87    mp  create subdirectory for command line option -q and -d
** 21 jul 87    mp  no reading of SETUP.DAT any more, eliminae ShowSetupMess()
**                  get default directory from SETUP.INF
** 22 jul 87    mp  append \ to WindowsPath for default if necessary
**  3 aug 87    mp  eliminate flashing message
**  4 aug 87    mp  don't write grabber for runtime version
** 20 aug 87    mp  change reinstallation process
** 31 jan 89    wch removed references to IsHarddiskSystem() (always TRUE)
** 31 jan 89    wch removed references to IsTwoDiskette, Is360K and
**			IsDiskette (always FALSE)
**  1 feb 89    wch added new global BOOL bCompatHiMem. default == TRUE,
**                      incompatibilities sets it FALSE.  Used to set 
**                      bSkinnyUser in SelectMachine().
*/

#include <stdlib.h>
#include <fcntl.h>
#include <io.h>
#include <dos.h>
#include <string.h>
#include <process.h>
#include <errno.h>
#include "id.h"
#include "setup.h"

#define GET_EXTENDED_INT			0x15
#define GET_EXTENDED_PARM			0x88
union REGS ir, or;


/* global flags, etc. */

BOOL IsRuntime;
BOOL IsQuick;
BOOL Is386;
/* Determines whether we run tests to identify the display adapter.
 * If SETUP is started with the /n or -n option this flag is reset
 * to FALSE so that no diagnosis tests are run and the user interactively
 * specifies the machine, display adpater etc, by examining the menus.
 */
BOOL bRunDiagnosisTests = TRUE;
BOOL bSkinnyUser = TRUE;
BOOL bCompatHiMem = TRUE;
BOOL bInsertHimemLine = TRUE;
BOOL bConfigBackedUp = FALSE;
BOOL IsFixedDestination;

char szBootDrive[2] = "";
CHAR *DosPath;

/* externals from SUDATA.C
*/
extern textline CopyMsg[];
extern textline ReinstallFinishedMsg1[];
extern textline FinishedMsg1[];
extern textline ReinstallFinishedMsg2[];
extern textline FinishedMsg2[];
extern CHAR smInfOpenErr[];
extern CHAR SetupInf[];
extern textline IncorrectDos20[];
extern textline IncorrectDos386[];
extern textline ExtMemMsg0[];
extern textline ExtMemMsg1[];
extern textline MemsetMsg286[];
extern textline MemsetMsg386[];
extern textline MemsetMsg[];
extern textline MemsetFailMsg[];
extern textline MemsetChoices[];
extern textline AutoexecMsg1[];
extern textline AutoexecMsg2[];
extern textline AutoexecChoices[];
extern textline YesNoChoices[];
extern textline DefaultExitChoices[];
extern CHAR WinIniBakFile[];
extern CHAR smConfigModfFail[];

/* externals from SUSOURCE.C
*/
extern CHAR DefaultSPath[PATHMAX+1];

/* externs from disptest.asm */
extern int get_ext(), A8086or8088(), CallINT15();
extern int IsHimemDrvPresent(), HMAreaPresent(), HMAAllocated();
extern int IsCEMMIn();

/* extern from susel.c */
extern union dfile * pselect[SSIZE];

/* extern form sudest.c */
extern int InsertHimemLine();

/* extern form suinf.c */
extern void StoreInsertText();

/* forward declarations of local procedures */

void ReadInfo();


main(argc, argv)
int argc;
CHAR * argv[];
{
int shift = 0;

    DefaultSPath[0] = '\0';

	 /* If /n option turn off the flag to Run Diagnosis Tests, to identify
       User Display Adapter. */
	 if ((argc > 1) && ((stricmp(argv[1], "/n") == 0) ||
								 (stricmp(argv[1], "-n") == 0)))
		 bRunDiagnosisTests = FALSE;

    IsQuick = ((argc > 1) && ((0 == stricmp(argv[1], "/q")) ||
                              (0 == stricmp(argv[1], "-q"))));
    IsFixedDestination = ((argc > 1) && ((0 == stricmp(argv[1], "/d")) ||
                                         (0 == stricmp(argv[1], "-d"))));

	 /* -d option given after -n ? */
	 if (!IsFixedDestination)	{
		IsFixedDestination = ((argc > 2) && ((0 == stricmp(argv[2], "/d")) ||
														 (0 == stricmp(argv[2], "-d"))));
		if (IsFixedDestination)
			shift = 1;
	 }


    DosPath = (CHAR*)getenv("PATH");

    ReadInfo();                             /* read SETUP.INF */

	 /* ReadInfo sets the Is386 flag. 
     * user should have 80386 based machine for Win/386. */
	
	 if (Is386 && A386machine() != 0x386)	{
      ScrClear();
      ScrDisplay(NotA386Machine);
		exit(8);
	 }

	 /* require DOS V3.10 or higher for WIN 386. */
	 if (Is386 && (_osmajor < 3 || (_osmajor == 3 && _osminor < 10)))	{
      ScrClear();
      ScrDisplay(IncorrectDOS386);
		exit(8);
	 }

	 /* require DOS V3.0 or higher for WINDOWS. */
	 if (!Is386 && _osmajor < 3)	{
      ScrClear();
      ScrDisplay(IncorrectDOS20);
		exit(8);
	 }


	 
    /* get path from argv[] or default it. */
    if (argc > (2+shift)) {
        strcpy(WindowsPath, strupr(argv[2+shift]));
        if (argc > (3+shift))
            strcpy(DefaultSPath, strupr(argv[3+shift]));
    } else
        strcpy(WindowsPath, inserttext[D_DIRECTORY]);

    if (strchr("\\:", WindowsPath[strlen(WindowsPath) - 1]) == NULL)
        strcat(WindowsPath, "\\");  /* append '\' if necessary */

    if (IsQuick)
        DoSetupAutomatic();             /* main function for normal setup*/
    else
        DoSetupInteractive();               /* main function for quick setup */

    exit (0);
} /* main */

DoSetupInteractive()
{
char *path;
int iChoice;

    if (!IsFixedDestination) {
        ScrClear();
        ScrDisplay(SetupBanner);
        ContinueOrQuit();
    }

	 if (Is386)
		bSkinnyUser = FALSE;

	 /* Check for VDISK.SYS, etc. in CONFIG.SYS */
	 CheckDriverCompat();

    if ( !Is386)	{
   	  /* WIN 286 - setup bSkinnyUser depending on the user H/W. 
           default is bSkinnyUser = TRUE
           AT&T machine returns bogus value on a INT 15 call.
           bSkinnyuser=TRUE, if user selects a AT&T machine. 
        */
	    DecideUser();	/* choose between skinny and fat user */
    }

    if (IsFixedDestination) 
        MakeWindowsPath();
    else 
        DoInitDisk();                       /* Get disk path */

    WinIniPrepare();                        /* load WIN.INI if necessary */

    GetWinFiles();                          /* store inf-data in statics */

    if (!IsQuick)            /* allow copying of apps */
        StoreStandardItem(S_APPS, pheads[N_APPS]);

    CopyWinCnf();                           /* copy WIN.CNF to WIN.COM */

    SelectMachine();                        /* select machine */

    BinOpen();
    ScrClear();
    ScrDisplay(CopyMsg);
    Copy(CD_STARTUP);                       /* copy and load files and */
    BinClose();                             /* write BIN file */
    CreateWinCom();                         /* create WIN.COM file */

           /* insert system diskette */

    OvlWrite();                             /* write OVL file */

    SelectPrinters();                       /* select printer */
    SelectCountry();                        /* select country */

    SelectFonts();                          /* set appropriate fonts */

    ScrClear();
    ScrDisplay(CopyMsg);
    Copy(CD_SYSTEM);                        /* copy all other files */
    if (!IsRuntime) {
        WriteGrabber();                     /* write grabber file */
        WriteMsDos();                       /* write MSDOS.EXE */
    }
    WinIniFonts();                          /* write WIN.INI */
    WinIniClose();

    if (!*DefaultSPath)
	getcwd(DefaultSPath, PATHMAX);      /* set source path to curr.path */

    if (!Is386 && bInsertHimemLine && IsHimemDrvPresent() == 0)	{	/* if HIMEM.SYS not installed */
    	if (InsertHimemLine() != 0)
                        FatalError(smConfigModfFail, 7);
    }

    ScrClear();
    ScrDisplay((CheckOldWin()) ? AutoexecMsg1: AutoexecMsg2);
    if (ScrSelectChar(AutoexecChoices, 0) == 0)
    	ModifyAutoexec();

    DisplayReadmes();
    ChangeToWindowsPath();

    ScrClear();

    if (Is386)
        ScrDisplay(MemsetMsg386);
    else
        ScrDisplay(MemsetMsg286);

    ScrDisplay(MemsetMsg);
    if (ScrSelectChar(MemsetChoices, 0) == 0)
        RunMemset();

    if (*(inserttext[D_EXECPGM])) {
        path = FileName(WindowsPath, inserttext[D_EXECPGM]);
        execl(path, path, DefaultSPath, NULL);
    }

   ScrClear();

	if (IsReinstall)
	     PutFileName(WinIniBakFile);

   ScrDisplay( IsReinstall ? ReinstallFinishedMsg1 : FinishedMsg1);
} /* SetupInteractive() */

/* Do quick setup */
DoSetupAutomatic()
{
    MakeWindowsPath();                      /* make subdir if necessary */

    GetWinFiles();                          /* store inf-data in statics */

    CopyWinCnf();                           /* copy WIN.CNF to WIN.COM */

    GetStandardSys(0);                      /* select first [machine] entry */

    BinOpen();
    Copy(CD_STARTUP);                       /* copy and load files and */
    BinClose();                             /* write BIN file */
    CreateWinCom();                         /* create WIN.COM file */

    OvlWrite();                             /* write OVL file */

    Copy(CD_SYSTEM);                        /* copy all other files */

    if (!IsRuntime) {
        WriteGrabber();                     /* write grabber file */
        WriteMsDos();                       /* write MSDOS.EXE */
    }
} /* SetupAutomatic() */



/*  ReadInfo() -- read SETUP.INF
**
*/
void ReadInfo()
{
    if (!Open_Info(SetupInf)) {
        FatalError(smInfOpenErr, 8);
    }
    GetWinInfo();
/* BILLHU
    Close_Info();
*/
    free(buffer);
    close(fhInfo);
    StoreInsertTexts();
/* Close_Info() */

    IsRuntime = (strlen(inserttext[D_EXEFILE]) != 0);
    Is386 = (strcmp(inserttext[D_COMFILE], "WIN86.COM") == 0);
} /*  ReadInfo() */


RunMemset()
{
char arg0[PATHMAX];
int errcode;
static char *args[6] = {"n", "n", "0", "n", "", ""};
char PrgName[PATHMAX];

/* args[0] = "n" ==> skip the first screen,
   args[1] = "y" ==> win/386
	args[2] = Boot Drive e.g "C" or "0"
	args[3] = "y" if CONFIG.SYS backed up by Setup
	args[4] = string representing the program to be invoked
		at the end of Memset.
		"" (blank) ==> no program needs to be invoked.
	args[5] = string parameter for args[4]
 */

	strcpy(arg0, WindowsPath);
	strcat(arg0, "MEMSET.EXE");

	ScrClear();

	if (Is386)
		args[1] = "y";

	if (*szBootDrive)
		*args[2] = *szBootDrive; 

	if (!Is386 && bConfigBackedUp)
		args[3] = "y";

	if (*(inserttext[D_EXECPGM])) {
		strcpy(PrgName, WindowsPath);
		strcat(PrgName, inserttext[D_EXECPGM]);
		args[4] = PrgName;
		args[5] = DefaultSPath;
	}
	
	/* closing messages for Setup, displayed only if it is not runtime Setup */

	if (! *(inserttext[D_EXECPGM])) {
	   ScrClear();

		if (IsReinstall)
	   	PutFileName(WinIniBakFile);

/* BILLHU
	   ScrDisplay(IsTwoDiskette ? DisketteFinishedMsg :
			  IsReinstall ? ReinstallFinishedMsg2 : FinishedMsg2);
*/
	   ScrDisplay( IsReinstall ? ReinstallFinishedMsg2 : FinishedMsg2);
		getchw();
		
	}

	errcode = spawnl(P_OVERLAY, arg0, arg0, args[0], args[1], args[2], args[3], args[4], args[5], NULL);

	/* we never come back to this place.*/


	if (errcode != 0)	{
			ScrClear();
			ScrDisplay(MemsetFailMsg);
			ContinueOrQuit();
	}			 
	
}	

/* depending on the user system configuration decide which user to 
   use - UserS or userF */
DecideUser()
{

	if (IsCEMMIn())	/* CEMM.SYS is incompatible with HIMEM.SYS */
		bSkinnyUser = TRUE;
	else if (IsHimemDrvPresent())	{	/* HIMEM.DRV present ? */
		if (HMAreaPresent() && HMAAllocated())	{
			ScrClear();
			ScrDisplay(ExtMemMsg0);
			if (ScrSelectChar(DefaultExitChoices, 0) == 0)	{
				textpos(23, 1);
				exit(8);
			}
			/* if user continues, he will get SkinnyUser */
		}
		else if (HMAreaPresent() && !HMAAllocated())
			bSkinnyUser = FALSE;
	}
	else	{	/* HIMEM.DRV absent */
		/* check to see if we have a 80286 or 80386 processor */

		if (!A8086or8088())	{	/* may have a CMOS RAM */
			/* if PC or XT with an accelerator board look at INT 15 */
			if (Ibm() == 1 || Ibm() == 2)	{
				if (CallINT15() == -1)	/* INT 15 call was invalid */
					;		/* bSkinnUser = TRUE, the default value is correct. */
				else if (CallINT15() >= 64)
					bSkinnyUser = FALSE;
				else	{	/* valid INT 15/88 call, but no ext memory present. */
					/* it may be absent, or it is being used by some body else */
					;	/* bSkinnyUser = TRUE, default value is correct */
				}
			}
			else	{	/* can read CMOS value */
				if (get_ext() > 0 && CallINT15() < 64)	{
					ScrClear();
					ScrDisplay(ExtMemMsg1);
					if (ScrSelectChar(DefaultExitChoices, 0) == 0)	{
						textpos(23, 1);
						exit(8);
					}
					/* if user continues, he will get SkinnyUser */
				}
				else if (get_ext() > 0 && CallINT15() >= 64)
					bSkinnyUser = FALSE;
			}
		}
	}
}


