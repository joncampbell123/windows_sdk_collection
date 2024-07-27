/****************************************************************************

	PROGRAM: memset

	MODULE: memset.c

See below for program and parameter description:

****************************************************************************


	    PLEASE PUT NOTES in HISTORY (below) WHEN YOU EDIT
	    THIS FILE.


****************************************************************************
First, some history (most recent first)..

 27 jun 88	peterbe	Fixed calculation of free expanded memory size
			(* 16).  Added DEBUG parameter for FREE expanded.
 17 jun 88	peterbe	When ModifyConfig() returns, the program whose
			name was passed from SETUP is executed (new
			ExecProgram() function).
 16 jun 88	peterbe	DEVICE= and EMM driver names are now all UpperCase
			Loading smartdrive is determined by the size
			of free_expanded instead of total_expanded,
			if there is not enough extended memory.
			For test purposes (DEBUG set, parameter exists),
			free_expanded = total_expanded / 2.

 14 jun 88	peterbe	Added some more code for handling defaults
			for EXEC parameters.
			DriveIsHarddisk(), LocateBootDrive() are
			no longer used -- boot just defaults to C:
			or what is passed from SETUP.

****************************************************************************

	For execution from Setup, and for testing, this program has some
	optional arguments.

	Arg 1   'y'		indicates the program is running standalone.
		'n'		indicates it's running from SETUP.
				If it's running from SETUP, the program MUST
				have at least 4 arguments.

	Arg 2   'y' or 'n'  to indicate Windows 386

	Arg 3   Boot drive	'0' if not specified, letter otherwise.

	Arg 4   'y' or 'n'	to indicate that Setup backed up CONFIG.SYS

	Arg 5	string		program name to be EXEC'd from MEMSET.
				If this arg exists and is not "", then
				the program will be EXEC'd.

	Arg 6	string		argument of program to be EXEC'd

	The following arguments are defined if DEBUG is:

	Arg 7   numeric		Total Expanded memory (Kbytes)

	Arg 8   numeric		Free Expanded memory (Kbytes)

	Arg 9   numeric		Total Extended memory (Kbytes)

	Arg 10  numeric		Free Extended memory (Kbytes)

				(see definitions PSTANDALONE... below)

****************************************************************************/

/* definitions of parameter numbers in arg list */

#define PSTANDALONE	 1
#define P386		 2
#define PBOOTDRIVE	 3
#define PBACKUP		 4
#define PEXECFUN	 5
#define PEXECPAR	 6
#define PEXP		 7
#define PEXPFREE	 8
#define PEXT		 9
#define PEXTFREE	10


#include <stdio.h>
#include <string.h>
#include <dos.h>
#include <fcntl.h>
#include <process.h>
#include <stddef.h>
#include <errno.h>

#include "setup.h"

#define	GET_UNALLOC_PAGE_COUNT	0x42
#define	GET_STATUS		0x40
#define	GET_VERSION		0x46
#define	GET_CONVENTIONAL	0x12
#define	GET_EXTENDED_INT	0x15
#define	GET_EXTENDED_PARM	0x88
#define	GET_MODEL_PARM		0xC0
#define	EMM_INT			0x67
#define	DEVICE_NAME_LENGTH	0x08
#define	GET_VECTOR		0x35

typedef	unsigned short USHORT;	/* us  */
typedef	unsigned long ULONG;	/* ul  */
typedef	void far *PVOID;

#define	MAKEULONG(l, h)	 ((ULONG)(((USHORT)(l))	| ((ULONG)((USHORT)(h))) << 16))
#define	MAKEP(sel, off)		((PVOID)MAKEULONG(off, sel))

/* externs from sudata.c */

extern textline	IntroMsg[];
extern textline	IsWin386[];
extern textline	NoExtOrExp386[];
extern textline	NoExtOrExp286[];
extern textline	ExpOnly386[];
extern textline	ExpAndExt386[];
extern textline	InsuffExtMem1[];
extern textline	InsuffExtMem2[];
extern textline	InsuffFreeExtMem1[];
extern textline	InsuffFreeExtMem2[];
extern textline	ExpMemBoardsMsg[];
extern char *BoardsList[];
extern textline	YesNoChoices[];
extern textline	UnknownExpMemBoard[];
extern textline	NY286[];
extern textline	InsuffExt286[];
extern textline	InsuffExp286[];
extern textline	InsuffExtOrExp286[];
extern textline	SmartDriveInMsg[];

union REGS ir, or;
struct SREGS sr;

extern Ibm();
int emm_installed();
int get_free_pages();

typedef	char far  *PCH;
typedef	char near *NPCH;
int lmemcmp(PCH, PCH, int);

extern int IsHimemDrvPresent(), QueryHimemExtMem();
int get_conventional();
int get_ext();
int get_model();
int get_cs();
int cpu_type();
char LocateBootDrive();

int fSetup = FALSE;
int ext_avail = 0;
int total_expanded = 0;
int free_expanded = 0;
int conventional = 0;
ULONG conv_bytes;
int free_extended = 0;
int total_extended = 0;
int EMM_version;
USHORT cs_val;
ULONG cs_addr;

int bPS2Emm = FALSE;
char SmName[13]	= "";
char SmLine[81]	= "";
char EmmName[13] = "";
char EmmLine[81];

static char * pExecFun = "";
static char * pExecPar = "";

main(argc, argv)
int argc;
char *argv[];
{
int model;
int fBackedUp =	FALSE;
char BootDrive[2], Drive;
int Is386 = FALSE;
static char sSize[5];
int fh;

	strcpy(BootDrive, "0");

	/* at least PBACKUP parameters and NOT standalone => run from SETUP. */
	if (argc > PBACKUP && *argv[PSTANDALONE] ==	'n') {
		fSetup = TRUE;
		if (argc < PEXECFUN)	/* missing args from SETUP. */
			exit(1);

		/* use args from SETUP to set various flags */

		if (*argv[P386] ==	'y')
			Is386 = TRUE;

		BootDrive[0] = *argv[PBOOTDRIVE];
		BootDrive[1] = '\0';
		
		if (*argv[PBACKUP] ==	'y')
			fBackedUp =	TRUE;
	}


#ifdef FINDBOOTDRIVE
	Drive = LocateBootDrive(); 
#endif

	/* if Setup didn't give us the boot drive, and we could locate one */

	if (BootDrive[0] == '0')	/* && Drive != ' ') */
		BootDrive[0] = 'C'; 


	/* Set up EXEC function and parameter.  If no EXEC parameters,
	** or if they are 0-length, leave blank ("").  The program will
	** EXEC pExecFun on exit, if (*pExecFun != 0) */

	if (argc > PEXECFUN)
	    pExecFun = argv[PEXECFUN];

	if (argc > PEXECPAR)
	    pExecPar = argv[PEXECPAR];

	/* Get memory sizes */

	conventional = get_conventional();
	conv_bytes = (ULONG) conventional * 1024L;
	cs_val = get_cs();
	cs_addr = (ULONG) cs_val * 16L;
	conv_bytes -= cs_addr;

	total_extended = free_extended = 0;

	if (!A8086or8088())	{	/* may have a CMOS RAM */
		if (Ibm() == 1 || Ibm() == 2)	{
			/* PC or XT with an accelerator board look at INT 15 */
			total_extended = free_extended = CallINT15();

			/* -1 returned if call  was invalid. */

			if (free_extended < 64)
				total_extended = free_extended = 0;
		}
		else	{	/* CMOS RAM exists, read from CMOS */
			total_extended = get_ext();
			free_extended = CallINT15();
			
			if (total_extended < 0 || free_extended < 0)
				total_extended = free_extended = 0;

			if (IsHimemDrvPresent() && free_extended == 0)
				free_extended = QueryHimemExtMem() + 64;
		}
	}
	


/*
**	default:
**		free_extended  = get_free_extended();
**		if (IsHimemDrvPresent() && free_extended == 0)
**			free_extended = QueryHimemExtMem() + 64;
**		total_extended = get_ext();
**		break;
**	}
*/
	if (emm_installed())
		get_free_pages();

#ifdef DEBUG

    /* Defining DEBUG means the 4th paramater can force the amount of
     * extended memory, and the 5th paramater can force the amount of expanded
     * memory.
     */

	if (argc > PEXP)
		/* put total_expanded in units of 16 K */
		total_expanded = atoi(argv[PEXP]) / 16;

	if (argc > PEXPFREE)
		/* free_expanded is also in units of 16 K. */
		free_expanded = atoi(argv[PEXPFREE]) / 16;

	if (argc > PEXT)
		total_extended = atoi(argv[PEXT]);

	if (argc > PEXTFREE)
		free_extended  = atoi(argv[PEXTFREE]);

#endif

	if (total_expanded < 0)
		total_expanded = 0;

	if (free_expanded < 0)
		free_expanded = 0;

	if (free_expanded > total_expanded)
		free_expanded = total_expanded;

	/* change Expanded memory sizes from 16K pages to Kbytes */
	total_expanded *= 16;
	free_expanded  *= 16;


	if (total_extended < 0)
		total_extended = 0;

	if (free_extended < 0)
		free_extended = 0;

	if (free_extended > total_extended)
		free_extended = total_extended;



	if (!fSetup)
		Pause(IntroMsg);

	fh = open("SMARTAAR", O_RDONLY);
	if (fh != -1)	{
		close(fh);
		ExitMemset(SmartDriveInMsg);
	}

	if (cpu_type() == 386 && (Is386 || (!fSetup && YesNo(IsWin386)))) {
		/*  a 80386 machine, setup says user has WIN/386 or 
			run from DOS and user has WIN/386. */
		if (!total_expanded && !total_extended)
			ExitMemset(NoExtOrExp386);	
		else if (total_expanded && !total_extended)	{
			PutExp(total_expanded);
			ExitMemset(ExpOnly386);
		}

		if (total_extended && total_expanded)  {
			/* both exp and ext memory present. */
			PutExt(total_extended);
			PutExp(total_expanded);
			Pause(ExpAndExt386); /* ask user: change exp to ext ? */
		}

		/* user has extended memory. he may have expanded memory
		** which he doesn't want to change to extended memory. */
		
		/* total_extended memory has a useful value ? */

		if (!total_expanded && total_extended < 1024)  {

			/* if EMM present, ext and exp may be overlapping. 
			** can advise user how much memory to buy only
			** if no EMM is present and total_ext < 1024 */

			PutExt(total_extended);
			if ( ((1024-total_extended) % 512) != 0)
				PutExp( ((1024-total_extended)/512 + 1) * 512);
			else
				PutExp(1024-total_extended);
				/* not really expanded memory */
				/* round the extra memory needed to the
				** nearest 512K */
			ExitMemset(InsuffExtMem1);
		}
		else if (total_expanded && total_extended < 1024)	{
			PutExt(total_extended);
			PutExp(total_expanded);
			ExitMemset(InsuffExtMem2);
		}


		/* sufficient free extended memory ? */

		if (!total_expanded && free_extended < 1024)	{
			PutExt(total_extended);
			PutFreeExt(free_extended);
			ScrClear();
			/* should free up some ext memory or buy some memory
			** so that at least 1 MB is avalilable for WIN/386
			*/
			ExitMemset(InsuffFreeExtMem1);
		}
		else if (free_extended < 1024)	{
			PutExt(total_extended);
			PutExp(total_expanded);
			PutFreeExt(free_extended);
			ScrClear();
			/* should conv exp to ext, free up some ext memory,
			** or buy some memory
			** so that at least 1 MB is avalilable for WIN/386
			*/

			ExitMemset(InsuffFreeExtMem2);
		}

		/* free_extended >=1024 */

		ext_avail = free_extended;
		free_extended -= 1024;
		free_extended = free_extended/4;

		if (free_extended < 128)   /* smartdrv needs atleast 128K */
			free_extended = 128;

		LineForSmartDrv(0, free_extended);

		ScrClear();
#ifdef DEBUG
		/* use free_expanded now -- peterbe 16jun88 */
		if (argc > PEXTFREE)
		  ModifyConfig(BootDrive, fBackedUp, fSetup,
			conventional, atoi(argv[8]), free_expanded);
		else
		  ModifyConfig(BootDrive, fBackedUp, fSetup,
			conventional, ext_avail, free_expanded);
#else
		ModifyConfig(BootDrive, fBackedUp, fSetup,
			conventional, ext_avail, free_expanded);
#endif
		ExecProgram();
		exit(0);
	}


	/* WIN/286 */

	if (!total_expanded) {
		ScrClear();
		ScrDisplay(NY286);
		LineForEmm();
	}
		
	if (*EmmLine)	{
		ext_avail = free_extended;
#ifdef DEBUG
		/* use free_expanded now -- peterbe 16jun88 */
		if (argc > PEXTFREE)
		  ModifyConfig(BootDrive, fBackedUp, fSetup,
			conventional, atoi(argv[PEXTFREE]), free_expanded);
		else
		  ModifyConfig(BootDrive, fBackedUp, fSetup,
			conventional, ext_avail, free_expanded);
#else
		ModifyConfig(BootDrive, fBackedUp, fSetup,
			conventional, ext_avail, free_expanded);
#endif
		ExecProgram();
		exit(0);
	}

	if (!total_extended && !total_expanded)
	   ExitMemset(NoExtOrExp286);

	/* user has either extended or expanded memory or both.
	 */
		
	if (total_extended && (free_extended-64 >= 128) )
		LineForSmartDrv(0, free_extended-64);
	else if ((free_expanded > 512) &&
		 (free_expanded <= 1024))	/* 16jun88peterbe*/
		LineForSmartDrv(1, 256);
	else if (free_expanded > 1024)		/* 16jun88peterbe*/
		LineForSmartDrv(1, 512);

	else {
		/* couldn't install Smartdrive */
	
		if (total_expanded <= 0)	{
			ScrClear();
			PutExt(total_extended);
			PutFreeExt(free_extended);
			ExitMemset(InsuffExt286);
		}
		else if (total_extended <= 0) {
			ScrClear();
			PutExp(free_expanded);	/* 16jun88 peterbe */
			ExitMemset(InsuffExp286);
		}
		else	{				
			ScrClear();
			PutExt(total_extended);
			PutExp(free_expanded);	/* 16jun88 peterbe */
			PutFreeExt(free_extended);
			ExitMemset(InsuffExtOrExp286);
		}
	}

   ScrClear();

	ext_avail = free_extended-64;
	if (ext_avail < 0)
		ext_avail = 0;

#ifdef DEBUG
	/* use free_expanded: 16jun88 peterbe */
	ModifyConfig(BootDrive, fBackedUp, fSetup,
		conventional, atoi(argv[PEXTFREE]), free_expanded);
#else
	ModifyConfig(BootDrive, fBackedUp, fSetup,
	conventional, ext_avail, free_expanded);
#endif
	ExecProgram();
	exit(0);

}	/* end main() */

LineForEmm()
{
int iChoice;
int dspChoice;

	ScrDisplay(ExpMemBoardsMsg);
	iChoice = ScrSelectList(BoardsList);

	switch  (iChoice) {
	case 0:		/* NONE */
		EmmLine[0] = 0;
		EmmName[0] = 0;
		break;
	case 1:		/* AST RamPage */
		strcpy(EmmLine, "REMM.SYS");
		strcpy(EmmName, "REMM.SYS");
		break;
	
	case 2:		/* PS2EMM.SYS */
		strcpy(EmmLine, "PS2EMM.SYS /E");
		strcpy(EmmName, "PS2EMM.SYS");
		break;
	case 3:
		strcpy(EmmLine, "EMM.SYS");
		strcpy(EmmName, "EMM.SYS");

		if (Ibm() == 4)		/* PS2 Model 30, start address,
					** port address */
			strcat(EmmLine, " mod30 D000 208");		

		else if (Ibm() == 5) {/* PS2 model 50 or 60 */
			strcpy(EmmLine, "EMM2.SYS");
			strcpy(EmmName, "EMM2.SYS");
		}
		else if (cpu_type() == 8086)	
			strcat(EmmLine, " pc D000 208");	  
		else	
			strcat(EmmLine,	" at D000 208");	  

		break;
	default:
		ExitMemset(UnknownExpMemBoard);
		break;
	}

}	/* end LineForEmm() */

Pause(Msg)
textline Msg[];
{
	ScrClear();
	ScrDisplay(Msg);
	ContinueOrQuit();
}

ExitMemset(Msg)
textline Msg[];
{
	ScrClear();
	ScrDisplay(Msg);
	textpos(23,1);
	exit(0);
	
}

YesNo(Msg)
textline Msg[];
{
	ScrClear();
	ScrDisplay(Msg);
	if (ScrSelectChar(YesNoChoices, 0) == 0)
	return TRUE;
	else
	return FALSE;

}

/* The following functions were taken from Aldus code for determining the
 * amount of extended and expanded memory available.
 */

int emm_installed() {
	PCH	EMM_device_name="EMMXXXX0";
	PCH	int67_device_name;

	ir.h.ah = GET_VECTOR;
	ir.h.al = EMM_INT;
	intdosx(&ir, &or, &sr);

	int67_device_name = MAKEP(sr.es, 0xa);

	if (lmemcmp(EMM_device_name, int67_device_name,
		DEVICE_NAME_LENGTH) != 0)
	return(FALSE);

	ir.h.ah = GET_STATUS;
	int86(EMM_INT, &ir, &or);
	if (or.h.ah != 0)
	return(FALSE);
	ir.h.ah = GET_VERSION;
	int86(EMM_INT, &ir, &or);
	EMM_version = or.x.ax;
	return(TRUE);

}	/* end emm_installed() */

int get_free_pages() {
	ir.h.ah = GET_UNALLOC_PAGE_COUNT;
	int86(EMM_INT, &ir, &or);
	if (or.h.ah == 0) {
	total_expanded = or.x.dx;
	free_expanded =  or.x.bx;
	return(TRUE);
	}
	else {
	total_expanded = free_expanded = 0;
	return(FALSE);
	}

} /* end get_free_pages() */

int get_conventional() {
	int86(GET_CONVENTIONAL, &ir, &or);
	return(or.x.ax);
}

/*
** get_free_extended() {
**	ir.h.ah = GET_EXTENDED_PARM;
**	int86(GET_EXTENDED_INT, &ir, &or);
**	return(or.x.ax);
**}
*/

int get_model() {
	unsigned char far *ptr;

	ptr = MAKEP(0xf000, 0xfffe);
	return (int) *ptr;
}


/*  LineForSmartDrv()
*
*constructs a CONFIG.SYS command line and filename for
*SMARTDRV.SYS from memory size information passed to it.
*
*   INPUT
*	flag = 0 use extended memory of specified size.
*		  = 1 use expanded memory of specified size.
*		  = 2 use all expanded memory. never used.
*
*	nK	  Amount of extended/expanded memory to
*			be used by SmartDrv in KBytes.
*
*   OUTPUT
*
*	The part of the CONFIG.SYS command line for SmartDrv
*	which begins with the filename (exclusive of drive
*	and path) is copied into SmLine[].
*
*/

LineForSmartDrv(flag, nK)
int flag;
int nK;
{

	/* create command line with filename, memory size, and
	** expanded memory switch, if specified. */

	strcpy(SmName, "SMARTDRV.SYS");
	if (flag == 0)
	sprintf(SmLine, "%s %d", SmName, nK);
	else if (flag == 1)
	sprintf(SmLine, "%s %d /a", SmName, nK);
	else	/* flag == 2, use all expanded memory. */
	sprintf(SmLine, "%s /a", SmName);

} /* end LineForSmartDrv() */

/* ExecProgram()
**
** If pExecFun is not blank, execute it when the
** program is done.
** otherwise position the cursor and exit.
*/
ExecProgram()
{
    char * pS;

    /* skip past blanks */
    for (pS = pExecFun; (*pS) && (*pS == ' '); )
	pS++;

    /* don't bother to exec if filename is blank */
    if(*pS)
	{
	ContinueOrQuit();
	ScrClear();
	execl(pS, pS, pExecPar, NULL);

	/* if the execl() succeded, we never return! */
#if DEBUG
	/* otherwise, if DEBUG is defined, we tell you why it failed. */
	printf("\nCould not execute \"%s\"", pS);
	if (errno == ENOENT)
	    printf(" -- File not found");
	if (errno == ENOEXEC)
	    printf(" -- File not executable");
#endif
	}

    /* exit if we didn't exec anything */
    textpos(23, 1);
    exit(0);

} /* end ExecProgram() */

