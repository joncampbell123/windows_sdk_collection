/* SUSEL.C -- make system selections from SETUP.INF information
**
** It is assumed that the routines in SUINF.C have been called to
** read in the information in SETUP.INF, and store it as an array
** of lists.
** Errors in this module exit with return code 7.
**
** History
**
** 28 may 87	plb	Begun
** 29 may 87    plb     Added GetLogAndGrb(), etc.
**			Added InitHardDisk(), DiskToHardPath().
**  1 jun 87	plb	Added BuildMenu(), SelectMachine().
**  3 jun 87	plb	Moved gdiskno[], hdindex[], menu[], etc. here.
**			Added FileNName(), FileNDisk().
**  4 jun 87	plb	removed InitHardDisk() and static arrays for
**			hard disk information.   DiskToHardPath()
**			is rewritten, assuming dynamic data stays.
**			It now has a 'quick' parameter.
**  5 jun 87	plb	Took away 'quick' param, call IsQuick() instead.
**			StoreStandardItem() now available for saving
**			filename and disk no. from a 'dfile' struct.
**  9 jun 87	plb	Meaning of 2 filenames in logo/grabber records
**			interchanged, so that the fname field is always
**			the actual filename.  The FindLGRecord() function
**			is renamed.
**			Information about selected files is now in the
**			form of pointers to the selected files.
** 10 jun 87	plb	BuildMenu() returns value now. Extending
**			SelectMachine() to allow driver selection.
** 12 jun 87	plb	Modifed OEM font menu building in SelectMachine()
**			so that if keyboard charset is 0, all OEM fonts
**			are listed in menu.
** 13 jun 87	plb	IsQuick is now global variable.
** 14 jun 87	plb	Combining all .H files into SETUP2.H
**			Changed init. in GetWinFiles() of pselect[] for
**			S_FONT, S_APPS.
** 16 jun 87	plb	Making changes in SelectMachine for OEM drivers.
**			Changed many declarations to 'unsigned char'.
** 17 jun 87	plb	Factored MakeLogGrbNames() out of GetLogAndGrb()
** 18 jun 87	plb	Don't allow selection of a new driver in
**			SelectMachine() if prev. one is OEM drv. in memory.
** 			Moved SelectPath() and SetDefaultPath() to SUPATH.C
** 19 jun 87	plb	Added AddStandardItem() function.
** [11:18 pm]   plb     Changed init. of pselect[S_APPS] to NULL.
** 16 jul 87    mp      SelectMachine() asks first for all drivers
** 18 jul 87    mp      Rename GetLogAndGrb() to GetDispFiles() and
**                      FindLGRecord() to FindDispRecord, add 386 files
**                      and include MakeLogBrbNames() in that function
**                      Added error messages to SelectMachine(),GetDispFiles()
**                      and FindDispRecord()
** 18 jul 87    mp      Handling of OEM terminal fonts with charset 0
** 21 jul 87    mp      eliminate ShowSetupMessage()
**  1 aug 87    mp      Add "to install OEM driver" selection at end of list
**                      Add "no selection" line at top of list
**                      no font selection in normal case, only if OEM install.
**  3 aug 87    mp      aliasing for 386 files
** 23 aug 87    mp      no installing of Terminal font for runtime
**  1 feb 89    wch     sets bSkinnyUser TRUE if bCompatHiMem == FALSE.
**  3 feb 89    wch     removed FileNDisk(), FileNName().
*/

#include <stdio.h>
#include <string.h>

#include "setup.h"
#include "id.h"


/* external from disptest.asm */
extern int IsHimemDrvPresent();


/* externals from SUMAIN.C
*/
extern BOOL Is386;
extern BOOL bRunDiagnosisTests;
extern BOOL bSkinnyUser;
extern BOOL bCompatHiMem;
extern BOOL bInsertHimemLine;

/* externals from SUDATA.C
*/
extern CHAR smInfMissingErr[];
extern CHAR smNoSelection[];
extern CHAR smSelectionInsertDisk[];
extern textline ExtMemMsg2[], ExtMemChoices2[];
extern CHAR smUserSMissing[];
extern CHAR smConfigModfFail[];

/* number of entries in the machine selection
*/
#define MACHINE_E_NORMAL 3             /* if it is a normal installation */
#define MACHINE_E_OEM    5              /* if you install OEM displays/kbds */
#define MACHINE_E_OEM_RUNTIME 4         /* .. and it is a runtime or /386 */
static int iMachineEntries = MACHINE_E_NORMAL; /* current value */
extern BOOL bFontSelection;             /* external from SUDATA.C */


/* pointers to selected records for KERNEL, GDI, USER, MSDOS,
** SYSTEM, KEYBOARD, MOUSE, DISPLAY, LOGO, OEMLOGO, 386, 3EX, SOUND, COMM,
** SYSFONTS, OEMFONTS, MSDOSD, WINOLDAP, and SPOOLER.
*/
union dfile * pselect[SSIZE];


/* Mapping tables for  SelectMachine().
** Standard drivers & fonts which can be changed after machine selection.
** Messages to indicate OEM drivers are loaded.
*/
static int i2driver[] = { S_DISPLAY, S_KEYBOARD, S_MOUSE,
			  S_SYSFONT, S_OEMFONT };
static int i2kind[] = { N_DISPLAY, N_KEYBOARD, N_POINTING_DEVICE,
			  N_SYSFONTS, N_OEMFONTS };

static unsigned char * i2OemMsg[] =
	{ sOemDisplayLoaded, sOemKBLoaded, sOemMouseLoaded,
	  sOemSysFontLoaded, sOemOemFontLoaded};

static unsigned char * i2insert[] =
	{ sInsertDisplay, sInsertKB, sInsertMouse,
	  sInsertSysFont, sInsertOemFont };

static textline * i2menu[] = {	DisplayMenu,
				KeyboardMenu,
				MouseMenu,
				SysFontMenu,
				OemFontMenu };

/* storage for path names for 'aliased' diskette numbers in [harddisk] */

static unsigned char HdNull[] = "";	/* 0-length string */



/* arrays for current menu for ScrSelectList() function.
** menu[] is always used.
** menu[], menumap[] and menuptr[] are set up by BuildMenu().
*/
					/* these map menu line number to .. */
unsigned char * menu[MENUSIZE];		/* pointers to strings for menu */
int menumap[MENUSIZE];			/* item number in list */
union dfile * menuptr[MENUSIZE];	/* record for selected file */


/* Extensions for display dependent files */
/* (extensions for logo data files will be ".LGD" */

unsigned char logext[] = ".LGO";
unsigned char grbext[] = ".GRB";
unsigned char d386ext[] = ".386";
unsigned char d3exext[] = ".3EX";

/* forward declarations */

union dfile * FindDispRecord(unsigned char *, int);
void GetDispFiles();
void GetFileRoot();
void BuildMenu(int, union dfile *, BOOL, int);

/* ************ code *************** */

/* GetWinFiles() -- save filenames from [windows] in static data.
**
** Get the records of the KERNEL, USER, GDI, and MSDOS files from
** the lists which were set up in SUINF.
** Pointers to these records are set up in pselect[].
** Also, clears all the remaining pointers in pselect[].
** The S_FONT entry is initialized (for later 'pruning').
** Returns TRUE if all 4 files are in the list.
*/

BOOL GetWinFiles()
{
    int i;
    union dfile * p;

    /* clear pointer array of selected drivers */
    for (i = 0; i < SSIZE; i++)
    	pselect[i] = NULL;

    /* transfer pointers from list for pheads[N_WINDOWS] to the array
    ** pselect[].
    */
    for (i = 0, p = pheads[N_WINDOWS]; (i <= S_RAMDRIVE) && (p != NULL);
    		p = p->n.p)
	{
	pselect[i] = p;
	i++;
	}

	/* initialize list of 'additional' fonts */
	pselect[S_FONT] = NULL;

	/* initialize list of application files */
	pselect[S_APPS] = NULL;

    return(i == 5);

} /* GetWinFiles() */

/* GetStandardSys() -- save pointers for entries for a standard system in
** the static array pselect[].
**
** Input	sysno	the number of the system entry.
**
*/
BOOL GetStandardSys(sysno)
int sysno;
{
union dfile * psys;
union dfile * pf;
int sn;
int i;
int j;
BOOL valid;

	for (sn = 0, psys = pheads[N_MACHINE]; sn < sysno; sn++)	{
		if (psys == NULL)
	   	return(FALSE);
		psys = psys->n.p;
	}

   /* copy entries from this machine to static data
    * j is index to machine.f[] array in record 
    * i is index to pselect[] array.
    * It is 4 greater than j. */

   for (valid = TRUE, j = 0, i = S_SYSTEM; j < NMFILES; i++, j++)	{
		if (NULL != (pf = psys->machine.f[j]))	{
			/* StoreStandardItem(i, pf) */
			pselect[i] = pf;
		}
	   else
			valid = FALSE;
	}
   GetDispFiles();

   return(valid);

} /* GetStandardSys() */


/* StoreStandardItem(sno, pf) -- stores a pointer to the record for a selected
** file in the pselect[] array in this module.
**
** Inputs:	sno	index to file/disk number lists. Typical values are
**			S_KEYBOARD for keyboard driver
**			S_MOUSE for pointing device driver
**			S_DISPLAY for display driver
**			S_SYSFONT for System font file
**			S_OEMFONT for OEM font file
**			S_PRINTER for printers
**                      S_FONT for printer and other fonts in [fonts]  ...
**                      (see "setup.h")
**
**		pf	pointer to selected dfile structure.
*/

StoreStandardItem(sno, pdfile)
int sno;			/* S_KEYBOARD, etc. */
union dfile * pdfile;		/* -> a record for some driver file, etc. */
{
    pselect[sno] = pdfile;

} /* StoreStandardItem() */


/* BOOL AddStandardItem(sno, pdfile)
**
** This adds a record to the end of a list beginning with a pointer
** in pselect[], such as a list of printers or fonts.
**
** NOTE: The record is added ONLY if no record with the same filename is
** already in the list.
**
** The record is added to the end of the list, with its 'next' pointer
** (n.p) set to NULL.
**
** The input parameters are the same as for StoreStandardItem().
**
** This returns TRUE iff the item was new.
*/
BOOL AddStandardItem(sno, pdfile)
int sno;		/* S_PRINTER, etc. */
union dfile * pdfile;	/* -> a record for some driver file, etc. */
{
    union dfile * pd;
    union dfile * pdNext;

    /* find pointer (pd) to first item in list. if none, put new record
    ** at end of list.
    */
    if (NULL == (pd = pselect[sno]))
	/* it's an empty list */
	StoreStandardItem(sno, pdfile);
    else
	{
	/* find end of list and link new record to it */
	while (NULL != (pdNext = pd->n.p)) 
	    {
	    /* while searching, exit if filenames match */
	    if (0 == strcmp(pdfile->n.fname, pd->n.fname))
		return(FALSE);
	    pd = pdNext;
	    }
	pd->n.p = pdfile;
	}

    /* make sure NULL is in next field of new record */
    pdfile->n.p = NULL;
    return(TRUE);

} /* AddStandardItem(sno, pdfile) */


/* GetDispFiles() -- find filenames and diskette numbers for
**                      display dependent files.
**
** The filename of the display driver is obtained from
** pselect[S_DISPLAY].fname and pointers to the display dependent file
** records are stored back in pselect[s].
** Display dependent files are: logo, grabber, .386 and .3ex
*/
void GetDispFiles()
{
    unsigned char Name[13];
    unsigned char NameBuf[13];

    GetFileRoot(pselect[S_DISPLAY]->n.fname, Name);

	 /* CAUTION:
	  * FindDispRec uses the name of the display driver to fetch
	  * the other display dependent files like .lgo etc.
	  * Since IBM VGA and C&T VGA use the same driver but different
	  * .3ex and .386 files this method fails. The following line
	  * looks at the Id field in the display record to detect a C&T VGA
	  * and alters the base name (or root name) supplied to FindDispRec
	  * so that the correct files are retrieved.
	  *
	  * A clean solution would have to create a driver ctvga.drv which 
	  * which is a copy of vga.drv 
     */

	 if (pselect[S_DISPLAY]->display.Id == CTVGA_640X450 )
		strcpy(Name, "CTVGA");


    /* search for records */
    pselect[S_LOGO] = FindDispRecord(strcat(strcpy(NameBuf, Name), logext),
				     N_LOGOS);
    pselect[S_GRABBER] = FindDispRecord(strcat(strcpy(NameBuf, Name), grbext),
					N_GRABBER);
    if (Is386) {
	pselect[S_386] = FindDispRecord(strcat(strcpy(NameBuf, Name), d386ext),
					N_386);
	pselect[S_3EX] = FindDispRecord(strcat(strcpy(NameBuf, Name), d3exext),
					N_386EXE);
    }
    return;

} /* GetDispFiles() */

/* GetFileRoot() -- get root of filename (delete extension)
**
** Parameters	fnam	pointer to filename (input)
**		root	pointer to root of filename (output)
*/

void GetFileRoot(fnam, root)
unsigned char * fnam;
unsigned char * root;
{
    int i;
    unsigned char ch;

    for (i = 0; (i < 8) && ('.' != (ch = fnam[i])) && (ch != 0); )
	    root[i++] = ch;
    root[i] = 0;
} /* GetFileRoot() */



/* FindDispRecord() -- find display dependent file record
**                   with a specific alias string
**
** Input string is assumed to be upper-case.
**
** Inputs:      fn      pointer to filename based on
**			display driver name.
**		kind	index to pheads for this kind of record
**                      (like N_LOGOS, N_GRABBER, N_SYSTEM)
** Returns		pointer to record with this filename, or NULL
**			if not found.
*/

union dfile * FindDispRecord(fn, kind)
unsigned char * fn;
int kind;
{
    union dfile * p;

    for (p = pheads[kind]; p != NULL; p = p->loggrb.p)
	{
	if (0 == strcmp(fn, kind == N_SYSTEM ?  p->loggrb.fname :
						p->loggrb.aname))
		return(p);
	}

    FatalError(smInfMissingErr, 7);

} /* FindDispRecord() */



/* DiskToHardPath(diskno,quick) -- get path name from [harddisk] data.
*
*/

unsigned char * DiskToHardPath(diskno)
int diskno;
{
    int i;
    union dfile * p;
    unsigned char * s;
    int disk;

    /* search list of [harddisk] records, finding the one with this
    ** disk number */
    for (s = NULL, i = 0, p = pheads[N_HARDDISK]; p != NULL; p = p->n.p)
    	{
	if (diskno == p->n.d)
	    {
	    s = p->n.fname;
	    break;
	    }
	}

    /* return pointer to string.  If NULL, return NULL, except in
    ** the case of a quick setup -- then, return pointer to "".
    */
    return( ((NULL != s) || (!IsQuick )) ? s : HdNull);

} /* DiskToHardPath() */


/* BuildMenu() -- build menu vector from a file list
**
** The list of file entries starting at pheads[kind] is searched,
** and non-null description string entries are put in the menu[]
** array.  If an entry in the linked list has a NULL description
** pointer, a pointer to the string nullfile is inserted instead.
**
** If 'pfirst' is NULL, an entry "no selection" is added at the top
** (menumap[0] = -1),  otherwise it points to a selected entry
** in the list, which will be displayed first.
**
** 'bOem' must be true, if an entry "to install oem driver.." has to be added.
** (menumap[end] = -2)
**
** If 'kind' is N_OEMFONTS, the character set must match.
**
*/

void BuildMenu(kind, pfirst, bOem, chset)
int kind;
union dfile * pfirst;
BOOL bOem;
int chset;
{
    union dfile * p;
    unsigned char * s;
    int i;	/* index in menu */
    int im;	/* index in list */

    /* place selected item first in list */
    i = 0;
    if (pfirst)
      {
      if (NULL != (s = pfirst->n.descr))
	{
	menuptr[0] = pfirst;
	menu[0] = s;
	menumap[0] = 0;
	i++;
	}
      }
    else
	{
	menuptr[0] = NULL;
	menu[0] = smNoSelection;
	menumap[0] = -1;
	i++;
	}

    for (im = 0, p = pheads[kind]; i < MENUSIZE - 1, p; im++, p = p->n.p)
      if (p == pfirst)
      	{
	menumap[0] = im;
	}
      else if((kind != N_OEMFONTS) ||
		/* OEM font must map keyboard charset, ... */
		(chset == p->oemfon.charset) ||
		/* .. unless it's an OEM keyboard driver with charset 0,
		** which means "we don't know the keyboard charset" */
		(0 == chset) ||
		/* .. unless it's an OEM terminal font with charset 0,
		** which means "we don't know the terminal font charset" */
		(0 == p->oemfon.charset))
	{
	s = p->n.descr;		/* get ptr. to description string */
	menu[i] = (s != NULL) ? s : nullfile;
	menumap[i] = im;
	menuptr[i] = p;
	i++;
	}

    if (bOem)
	{
	menuptr[i] = NULL;
	menu[i] = smSelectionInsertDisk;
	menumap[i] = -2;
	i++;
	}

    menu[i] = NULL;

} /* BuildMenu() */

/* GetNoMouseRec() - return pointing device record corresponding to nomouse.drv.
 * Flag error if it is absent. */
/*	Not used currently - 3/30/88, may be useful later.
union dfile * GetNoMouseRec()
{
union dfile *p;

	p = pheads[N_POINTING_DEVICE];	
	for (; p != NULL; p = p->n.p)	{
		if (stricmp(p->n.fname, "NOMOUSE.DRV") == 0)
			return p;
	}

	PutSecName("pointing.device");
	FatalError(smMissingNoMouseRec, 7);
}
*/

/*
 * input - Display Id 
 * Scans pheads[N_DISPLAY] for a display record with a matching display Id.
 * On success returns a pointer to the display record, NULL on failure.
 */
union dfile * GetDisplayRecFromId(Id)
int Id;
{
union dfile * p;
	
	for (p = pheads[N_DISPLAY]; p != NULL ; p = p->display.p)	{
		if (p->display.Id == Id)
			return p;
	}

	/* couldn't find the display record with this ID. */
	PutSecName("display");
	PutNumber(Id);
	FatalError(smNoDisplayRec, 7);
}

/* returns the n th Machine Rec from SETUP.INF */
union dfile * GetMachineRec(n)
int n;
{
union dfile * p;
int i;

	for (i = 0, p = pheads[N_MACHINE]; i < n; i++)	{
		if (p == NULL)
	   	return NULL;
		p = p->machine.p;
	}

	return p;
}


/* SelectMachine() -- display menu, select [machine] entry.
**
** First, a selection is made from among standard machines.
** pselect[] is set to the default values in SETUP.INF
 * If Display Adapter is identifiable it is updated. IF no mouse is present
 * this is updated too. If we identify the display adapter we go to a final 
 * screen for the users confirmation or revision of his selection.
 * If we cannot identify the display adapter the user has to go through 
 * the different menus.
 * Then, a screen to validate the default selection is displayed for each
** driver. Then, the selected drivers are listed.
** The user is allowed to make selections of any which are to be
** changed.
*/


SelectMachine()
{
BOOL GoThroughList = TRUE;  /* go through list of drivers */
int m, c, cd, i;
int idriver;		/* ranges over S_DISPLAY, S_KEYBOARD, etc. */
int ikind;			/* ranges over N_DISPLAY, etc. */
union dfile * pdriver;	/* pointer to driver record */
char *s;
int nOem;
int MachineId = 0, DisplayId = 0;
union dfile *p;
int iChoice;

/* After identifying the user adapter, bFinalMenu is set to TRUE.
 * It ensures that we go to the final menu, the first time and
 * subsequently too if the user revises the defaults.*/
int bFinalMenu = FALSE;


	iMachineEntries = bFontSelection ?
		(IsRuntime||Is386 ? MACHINE_E_OEM_RUNTIME :
					    MACHINE_E_OEM) : MACHINE_E_NORMAL;

    /* Select a system and its drivers from the [machine] selections */
	BuildMenu(N_MACHINE, pheads[N_MACHINE], FALSE, -1);
   do	{
		ScrClear();
		ScrDisplay(MachineMenu);
		m = ScrSelectList(menu);
	} while(m < 0);

   if (!GetStandardSys(m))
		FatalError(smInfMissingErr, 7);

	p = GetMachineRec(m);
	if (p == NULL)
		FatalError(smInfMissingErr, 7);

	MachineId = p->machine.Id;				

	if (!Is386 && MachineId == ATT_PC) /* WIN 286 and AT&T machine, can't trust*/
		bSkinnyUser = TRUE;				  /* INT 15, always give USERS.EXE */
 
	if (bRunDiagnosisTests)	{
		DisplayId = ( (Is386) ? Get386Display(MachineId) 
											: Get20Display(MachineId));
		if (DisplayId != 0)	{
			bFinalMenu = TRUE;
			pselect[S_DISPLAY] = GetDisplayRecFromId(DisplayId);
			/* change display dependent files, fonts */
			GetDispFiles();
			SetSysFont(pselect[S_DISPLAY]->fon.res);
			SetOemFont(pselect[S_DISPLAY]->fon.res,
				pselect[S_KEYBOARD]->kb.charset);
						
/*	Skip detecting absence of the mouse for the present.
			if (NoMouse(MachineId))
				pselect[S_MOUSE] = GetNoMouseRec();
 */
		}
	}


   /* Display the selected DISPLAY, KEYBOARD, MOUSE drivers, and FONTS
    * and ask for changes */

   for (c = 1; c > 0; )	{
		if (!GoThroughList || bFinalMenu)	{
	   	menu[0] = MsgNoChange;
	   	for (i = 0; i <= iMachineEntries; i++)	{
				pdriver = pselect[idriver = i2driver[i]];
				/* if an OEM driver is installed, label it as unchangable */
				menu[i+1] = ((pdriver->n.d) < 1) ? i2OemMsg[i] :
				pdriver->n.descr;
			}
			menu[i] = NULL;
			ScrClear();
			ScrDisplay(bFinalMenu ? FinalMenu : DriversMenu);
			c = ScrSelectList(menu);
		}
		if (c == 0)		/* user choose "No Change" from Menu. */
			break;

	if (c > 0) {
		/* check for OEM driver, disk no. < 1. Don't allow reselection */
		if (0 >= (pdriver = pselect[idriver = i2driver[c-1]])->n.d )
/* BILLHU
	   	beep();
*/
			ttyout(7);
		else	{
			/* Previously selected driver isn't OEM driver, so setup for
			 * selection of a new one now 
			 * change selected driver 
			 * idriver = i2driver[c-1]; calculated above 
	       * pdriver = pselect[idriver]; */
			ikind = i2kind[c-1];
			switch(idriver)	{
				case S_OEMFONT:
				/* select among OEM fonts with same charset as
				** keyboard driver. */
				BuildMenu(ikind, pdriver, TRUE,
					pselect[S_KEYBOARD]->kb.charset);
				break;

			default:
				BuildMenu(ikind, pdriver, TRUE, -1);
				break;
	    	}
			ScrClear();
			ScrDisplay(i2menu[c-1]);
			/* select new driver/file and store pointer to it */
			cd = ScrSelectList(menu);
			if (menumap[cd] >= 0)	{
	      /* Make selection among drivers from SETUP.INF */
		      pdriver = menuptr[cd];
		      if (pdriver != pselect[idriver])	{
					pselect[idriver] = pdriver;
				switch(idriver) 	{
					case S_DISPLAY:
					/* change display dependent files, fonts */
					GetDispFiles();
					SetSysFont(pdriver->fon.res);
					SetOemFont(pdriver->fon.res,
						pselect[S_KEYBOARD]->kb.charset);
					break;

				case S_KEYBOARD:
					/* change OEM font, keeping resolution. */
					SetOemFont(pselect[S_OEMFONT]->oemfon.res,
						pdriver->kb.charset);
					break;

				default:
					break;
				}
				}	/* if(pdriver ..) */

	      }  /* if menumap[cd] >= 0 */
			else  {
		      /* make selection from among OEM drivers, maybe */
		      if (0 < InsertOemDisk(i2insert[c-1]) )	{
					if (c <= 2)     /* if display or keyboard driver:
										  * allow font selection */
						iMachineEntries = IsRuntime || Is386 ?
							MACHINE_E_OEM_RUNTIME : MACHINE_E_OEM;
					/* read diskettes, build a menu */
					GetOemDriverList(idriver, pSourcePath);
					if (NULL != menu[0])	{
						/* there are drivers of this kind on the diskette... */
						ScrClear();
						ScrDisplay(SelectOemMenu);
						nOem = ScrSelectList(menu);
						if (nOem == 0)
							DeleteOemRecords(NULL);
						else
							SetOemDriver(idriver, menuptr[menumap[nOem]]);
					}
			
				}	/* if (0 < ...) */
			}  /* if cd >= 0 .. else .. */
		} /* .. prev driver not OEM */
	}     /* if (c > 0) */
		if (GoThroughList)	{
			if (++c > iMachineEntries)	{
				GoThroughList = FALSE;
				c = 1;
			}
		}

	} /* for(...) */

	if (!Is386)	{
		ScrClear();
		ScrDisplay(ExtMemMsg2);
			  /* BILLHU incompatible EMM driver, suggest USERS */
		if ( !bCompatHiMem )
			bSkinnyUser = TRUE;
		iChoice = ScrSelectChar(ExtMemChoices2, bSkinnyUser ? 1 : 0);
		if (iChoice == 0)
			bSkinnyUser = FALSE;
		else
			bSkinnyUser = TRUE;
	
		SetUserPtr();
	}

} /* SelectMachine() */


/* WIN 286 only, set pselect[S_USER] to USERF.EXE or USERS.EXE 
 * depending on bSkinnyUser */

SetUserPtr()
{
int i;
union dfile *p;

	for (i = 0, p = pheads[N_WINDOWS]; (i <= S_RAMDRIVE) && (p != NULL);
    		p = p->n.p)
		i++;

	/* if bSkinnyUser is TRUE and (i != S_RAMDRIVE+1 || p == NULL)
	   it is an error in SETUP.INF entry for skinny user. */
	
	if (bSkinnyUser)	{
		if (p == NULL) 	{
			PutSecName("windows");
			FatalError(smUserSMissing, 7);
		}
		pselect[S_USER] = p;
		bInsertHimemLine = FALSE;	/* No need for Himem Line in CONFIG.SYS */
	}

	/* KLUDGE! 
		After setting the user ptr to userf.exe or users.exe record
	   set pheads[.... RAMDRIVE ptr]->n.p = NULL to prevent users.exe 
      from getting copied to ramdrive directory */

	for (i = 0, p = pheads[N_WINDOWS]; (i < S_RAMDRIVE) && (p != NULL);
   		p = p->n.p)
		i++;
	p->n.p = NULL;

}


/* SetSysFont() -- set system font after display selection.
** Will only make change if exact match.
*/
SetSysFont(resolution)
int resolution[];
{
    union dfile * p;

    if (pselect[S_SYSFONT]->fon.d < 1)  /* OEM system font chosen */
	return;

    for (p = pheads[N_SYSFONTS]; p != NULL; p = p->fon.p)
      {
      if ((p->fon.res[0] == resolution[0]) &&
          (p->fon.res[1] == resolution[1]) &&
          (p->fon.res[2] == resolution[2]) )
	{
	pselect[S_SYSFONT] = p;
	break;
	}
      }

} /* SetSysFont() */

/* SetOemFont() -- set OEM font after display or keyboard change.
*/
SetOemFont(resolution, charset)
int resolution[];
int charset;
{
    union dfile * p;

    if (pselect[S_OEMFONT]->oemfon.d < 1)  /* OEM terminal font chosen */
	return;

    for (p = pheads[N_OEMFONTS]; p != NULL; p = p->fon.p)
      {
      if ((p->oemfon.res[0] == resolution[0]) &&
          (p->oemfon.res[1] == resolution[1]) &&
          (p->oemfon.res[2] == resolution[2]) &&
	  (p->oemfon.charset == charset || p->oemfon.charset == 0) )
	{
	pselect[S_OEMFONT] = p;
	break;
	}
      }

} /* SetOemFont() */



/* FileNDisk() -- get disk number of a driver or other selected file.
**
** input: n is integer in range S_KERNEL..S_GRABBER
**
** To see if the returned disk number is actually a hard-disk
** path, call DiskToHardPath().
*/

/* BILLHU
int FileNDisk(n)
int n;
{
    return((n < SSIZE) ? pselect[n]->n.d : 0);

}
*/




/* FileNName() -- get filename of a driver.
**
** input: n is integer in range S_KERNEL..S_GRABBER
*/

/* BILLHU
unsigned char * FileNName(n)
int n;
{
    return((n < SSIZE) ? (unsigned char *) pselect[n]->n.fname : NULL);

}
*/



/* RecNPtr() -- gets pointer to record for select file or list of
** files.  In some cases, this is the pointer to the first record in
** a list (e.g. [fonts]).
**
** input: n is integer in range S_KERNEL..S_GRABBER
*/
/* BILLHU
union dfile * RecNPtr(n)
int n;
{
    return((n < SSIZE) ? pselect[n] : NULL);

} */ /* RecNPtr() */


