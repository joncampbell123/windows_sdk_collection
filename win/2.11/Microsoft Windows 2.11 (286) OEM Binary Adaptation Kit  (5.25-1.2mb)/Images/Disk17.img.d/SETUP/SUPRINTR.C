/* SUPRINTR.C -- handle port and printer selection, and font selection.
**
** This provides menu selection of standard and OEM printers (output
** devices), copying of the drivers (by calling CopyDisk() ),
** and writing printer and port selection information to the
** output WIN.INI. Dependent files for selected printer are added to the
** pselect[S_APPS] chain.
**
**
** History
**
** 18 jun 87	plb	Wrote GetPorts(). 
** 19 jun 87	plb	Began SelectPrinter(), with subsidiary routines.
**			(also added AddStandardItem() to SUSEL.C).
** (10:20pm)	plb	Changed some strings, prepared for move of some
**			declarations to SUDATA and SETUP2.H.
** (10:41pm)	plb	Moving declarations to SUDATA.  Didn't move
**			anything to SETUP2.H.
** 20 jun 87    plb     Need to allocate enough space for string terminating
**			byte, in SelectOemPrinter(), about line 359:
**                      ... (pDesc = malloc(1 + strlen(...
** 19 jul 87    mp      Remove WantOemPrinter(), add copying without apps
** 22 jul 87    mp      Printer installation is default for first printer
**  1 aug 87    mp      Allow mutiple installation of a printer driver
**                      add "no selection" and "OEM driver" in selection list
**  3 aug 87    mp      Move "None"-port to end of list
**  4 aug 87    mp      Remove feature for not copying apps
**                      Bug fixed: now drivers are copied
*/

#include <string.h>
#include <malloc.h>

#include "setup.h"


/* defined in SUSEL.C
*/
BOOL AddStandardItem(int /* sno */, union dfile * /* pdfile */);

/* defined in SUFONT.C
*/
void CopyRes(int *, int *);

/* externals from SUDATA.C
*/
extern textline CopyMsg[];
extern unsigned char sNullPort[];
extern textline MenuWantPrinter[];
extern textline MenuWantAnotherPrinter[];
extern textline PrinterYesNo[];
extern textline MenuInstallPrinter[];
extern unsigned char sInsertPrinter[];
extern textline MenuSelectPort[];

/* ******* Local definitions and data *********** */

#define MAXPORTS	10
#define MAXPRINTERS	10
#define PORTBUFSIZE     50

/* if there are statements in the [ports] section of WIN.INI, this
** will contain pointers to strings in PortBuf[]:
*/
static unsigned char * PortList[MAXPORTS] = { "LPT1:" };
static nPorts = 1;

static char PortBuf[PORTBUFSIZE];

/* Variables for keeping track of printer selection. PortToPrinter[] is
** indexed by the port number, and the other arrays are indexed by
** the printer entry number.
** PrinterList may contain pointers to more than one record for the
** same printer file.  In this case, only one such record is linked
** into the list of selected printers.
** However, there may be several description strings in DeviceNames[]
** for a single printer driver.  Appropriate information is written
** to WIN.INI in such a case.
*/

static int PortToPrinter[MAXPORTS];		/* maps ports to printers */
static int PrinterToPort[MAXPRINTERS];		/* maps printers to ports */
						/* -1 entry means "None" */
static union dfile * PrinterList[MAXPRINTERS];	/* lists files */
static unsigned char * DeviceNames[MAXPRINTERS]; /* description strings */
int nPrinters = 0;

/* **************** Forward declarations ******************* */

static BOOL WantPrinter();
static void ChoosePrinter();
static void AddPrinter(union dfile *, CHAR *);

/* **************** Code ******************* */

/*  GetPorts() -- get port list from WIN.INI
**  This builds an array of pointers to portnames in PortList[], with
**  a count in nPorts.  The value of nPorts will then be the maximum
**  number of ports selected. Entry nPorts+1 is NullPort.
**  If WIN.INI information is unavailable, default it to {"LPT1:"}
*/
int GetPorts()
{
    int i;
    int l;
    int ls;
    char portvalue[80];

    GetProfileString("ports", PortBuf);
    if (0 < strlen(PortBuf))
	{
	for (i = 0, l = 0; i < MAXPORTS - 1; i++)
	    {
	    if (0 == (ls = strlen(PortBuf + l)))
		break;
	    PortList[i] = PortBuf + l;
	    /* force port name to be upper-case */
	    strupr(PortList[i]);
	    l += ls + 1;
	    }
	nPorts = i;
	}

    PortList[nPorts] = sNullPort;

} /* int GetPorts() */


/* FindDependentRecord() -- find dependent file record
**                   if successful add file name to printer file chain
**
** Inputs:      fn      pointer to io device filename
*/

static void FindDependentRecord(fn)
unsigned char * fn;
{
    union dfile *pd;
    union dfile *pdLast;

    for (pd = (pdLast = (union dfile *)&pheads[N_IO_DEPENDENT])->loggrb.p;
	 pd != NULL; pd = (pdLast = pd)->loggrb.p)
	{
	if (0 == stricmp(fn, pd->loggrb.aname))
	    {
	    pdLast->loggrb.p = pd->loggrb.p;    /* remove from phead chain */
	    AddStandardItem(S_APPS, pd);        /* add to pselect chain */
	    pd = pdLast;
	    }
	}
} /* FindDependentRecord() */


/* SelectPrinters() -- select standard or OEM printers.
*/
SelectPrinters()
{
    int i;
    int j;
    unsigned char * pPort;	/* port name */
    unsigned char * pFile;
    unsigned char * pDevice;
    unsigned char valbuf[80];
    unsigned char fileroot[10];
    unsigned char *cp;

    /* set up the list of ports */
    GetPorts();

    /* initialize PrinterList[] and DeviceNames[] */
    for (i = 0; i < MAXPRINTERS; i++)
	{
	PrinterToPort[i] = -1;		/* Init port to "None" */
	PrinterList[i] = NULL;
	DeviceNames[i] = NULL;
	}
    for (i = 0; i <= nPorts; i++)
	PortToPrinter[i] = -1;		/* No printer, this port. */


    /* Main loop for selecting printers -- only MAXPRINTERS driver
    ** installations can be made.
    */
    while ((nPrinters < MAXPRINTERS ) && WantPrinter())
	{
	ChoosePrinter();
	}

    /* Write out port and printer selections */
    for (i = 0; i < nPrinters; i++)
	{
	/* i is printer number, j is port number */
	j = PrinterToPort[i];
	pPort = PortList[j];

	pFile = PrinterList[i]->n.fname;
	pDevice = DeviceNames[i];

	if (cp = strchr(pDevice, '['))  /* treat printer driver name */
	    {
	    *cp = '\0';
	    *strchr(pDevice = cp + 1, ']') = '\0';
	    }

	/* ******* Now write printer entries in WIN.INI ******* */

	/* Create root of filename */
	GetFileRoot(pFile, fileroot);

	/* for first printer, write DEVICE= entry in [windows] section */
	if (i == 0)
	    {
	    strcpy(valbuf, pDevice);
	    strcat(valbuf, ",");
	    strcat(valbuf, fileroot);
	    strcat(valbuf, ",");
	    strcat(valbuf, pPort);
	    WriteProfileString("windows", "DEVICE", valbuf);
	    }

	/* for all printers, write an entry in the [devices] section */
	strcpy(valbuf, fileroot);
	strcat(valbuf, ",");
	strcat(valbuf, pPort);
	WriteProfileString("devices", pDevice, valbuf);
	}

} /* SelectPrinters() */

/* WantPrinter() -- does the user want to install a printer.
**
** This asks the user if the installation of another printer is
** desired.
*/
static BOOL WantPrinter()
{
    int result;

    ScrClear();
    ScrDisplay((nPrinters  == 0) ? MenuWantPrinter : MenuWantAnotherPrinter);
    /* default to "install" for first time, after that to "copy files" */
    result = ScrSelectChar(PrinterYesNo, (nPrinters == 0) ? 0 : 1);

    return (0 == result);

} /* WantPrinter() */

/* ChoosePrinter()
*/
static void ChoosePrinter()
{
    int s;
    union dfile * dfSel0;
    union dfile * dfSel;

    BuildMenu(N_IO_DEVICE, NULL, TRUE, -1);
    if (menu[0] == NULL)
	return;
    ScrClear();
    ScrDisplay(MenuInstallPrinter);
    s = ScrSelectList(menu);
    if (menumap[s] == -2)
	{
	SelectOemPrinter();
	return;
	}
    if (menumap[s] == -1)
	{
	return;
	}
    /* Add selected printer to end of list */
    dfSel0 = menuptr[s];

    /* create a copy of the record , since we must leave this one in the
    ** original list -- we may still need the same driver for another
    ** printer. */
    if (NULL == (dfSel = (union dfile *) malloc(DFIODEVSIZE) ))
	OutOfMemory();

    /* set next pointer to NULL and copy disk no., file name and resolution */
    dfSel->n.p = NULL;
    dfSel->n.d = dfSel0->n.d;
    strcpy(dfSel->n.fname, dfSel0->n.fname);
    CopyRes(dfSel->iodev.res1, dfSel0->iodev.res1);
    CopyRes(dfSel->iodev.res2, dfSel0->iodev.res2);

    /* Finally, put the selection information in the printer-select
    ** arrays, etc.
    */
    AddPrinter(dfSel, menu[s]);

    return;

} /* ChoosePrinter() */

/* AddPrinter() -- Add printer/description to lists.
**
** Input	dfSel	pointer to record containing filename.
**		pDesc	pointer to description string.
**
**			Both pointers must point to data which is
**			not going to be freed during the execution
**			of SelectPrinter(), since this data must be
**			accessed throughout the execution of AddPrinter.
*/
static void AddPrinter(dfSel, pDescr)
union dfile * dfSel;
unsigned char * pDescr;
{
    int i, s;

    AddStandardItem(S_PRINTER, dfSel);
    FindDependentRecord(dfSel->n.fname);
    PrinterList[nPrinters] = dfSel;
					/* save pointer to descr. string */
    DeviceNames[nPrinters] = pDescr; 

    /* now select port connection. First, create a menu.
    ** (this trashes the menu arrays!) */
    for (i = 0, s = 0; i <= nPorts; i++)
	{
	if (PortToPrinter[i] == -1  ||  /* port is available or */
	    i == nPorts)                /* is NullPort */
	    {
	    menu[s] = PortList[i];      /* put port name in menu */
	    menumap[s] = i;             /* map to port number */
	    s++;
	    }
	}
    menu[s] = NULL;

    ScrClear();
    ScrDisplay(MenuSelectPort);
    if (0 > (i = ScrSelectList(menu)))
	i = 0;

    PortToPrinter[menumap[i]] = nPrinters;
    PrinterToPort[nPrinters] = menumap[i];

    nPrinters++;

} /* AddPrinter() */

/* SelectOemPrinter()
*/
SelectOemPrinter()
{
    int s;
    union dfile * dfSel;
    char * pDesc;

    InsertOemDisk(sInsertPrinter);
    GetOemDriverList(S_PRINTER, pSourcePath);
    if (NULL != menu[0])
	{
	ScrClear();
	ScrDisplay(MenuInstallPrinter);
	if (0 < (s = ScrSelectList(menu)))
	    {
	    /* selection was made, so get pointer to selected record. */
	    dfSel = menuptr[menumap[s]];

	    /* make a copy of the description string. We need to make a
	    ** copy since we're going to free the menu stuff, and the
	    ** file records except for the one at pDesc.
	    */
	    if (NULL == (pDesc = malloc(1 + strlen(menu[s])) ))
		OutOfMemory();
	
	    strcpy(pDesc, menu[s]);

	    /* add the printer to the file list, etc. */
	    AddPrinter(dfSel, pDesc);

	    /* Inform the disk copying code that we must copy the
	    ** file right now.
	    */
	    ScrClear();
	    ScrDisplay(CopyMsg);
	    CopyDisk(-1, CD_OEMDRIVER);

	    /* finally, dispose of the memory taken up by the data created
	    ** by GetOemDriverList() (except the record at dfSel).
	    */
	    DeleteOemRecords(dfSel);
	    }
	}

} /* SelectOemPrinter() */

