/**[f******************************************************************
 * control.c - 
 *
 * Copyright (C) 1988 Aldus Corporation.  All rights reserved.
 * Copyright (C) 1989 Microsoft Corporation.
 * Company confidential.
 *
 **f]*****************************************************************/

/*********************************************************************
 * CONTROL.C
 *
 * 87-02-11	sec	"letter" -> a4,b5 for apple printers
 * 9Mar87	sjp	IBM printer support
 * 7Apr87	sjp	added DataProducts LZR 2665 (1st cut)
 * 8Apr87	sjp	added DECLN03R ScriptPrinter
 * 17Apr87	sjp	disabled SELECTPAPERSOURCE Escape,added PrintServer40
 *			paper selection.
 * 3Jun87	sjp	disabled ENABLERELATIVEWIDTHS escape.
 * 3Jun87	sjp	added GETSETPRINTERORIENTATION escape.
 * 4Jun87	sjp	added GETSETPAPERBINS escape, and escape numbers
 *			for above 2 escapes and ENABLEDUPLEX
 * 18Jun87	sjp	Added initialization of "spare" output data fields
 *			for GETSETPAPERBINS escape.
 * 20Jul87	sjp	Bug fix for Digital LPS40 (PRINTSERVER40).
 * 26Oct88	msd	added support for OLIVETTI_CHARSET
 * 26Mar91	msd	added duplex support, renamed Reserved to iDeviceRes
 *
 * note:
 *   escapes in the range 256 (AldusMin) to 767 (AldusPureOutMax) lpInData
 *   get modified to point to EXTTEXTDATA structure that has info
 *   on the currently selected font (and other stuff).  thus, the lpInData
 *   param that the app passes is ignored. (also note that some ranges
 *   of escapes only go to metafiles, and some only go to real devices).
 *   
 *********************************************************************/

#include "pscript.h"
#include <winexp.h>
#include "driver.h"
#include "pserrors.h"
#include "psdata.h"
#include "utils.h"
#include "debug.h"
#include "channel.h"
#include "pss.h"
#include "resource.h"
#include "atprocs.h"
#include "atstuff.h"
#include "profile.h"
#include "etm.h"
#include "text.h"
#include "exttext.h"
#include "getdata.h"
#include "spooler.h"
#include "control2.h"
#include "escapes.h"
#include "mgesc.h"
#include "graph.h"
#include "truetype.h"
#include "windict.h"
#include "psoption.h"
#include "enum.h"

#define RESETDC 128

#ifdef PROFILE
#define VOID		    void
/* function declarations for profiler routines contained in Windows libraries */
int	API ProfInsChk(VOID);
VOID	API ProfSetup(int,int);
VOID	API ProfSampRate(int,int);
VOID	API ProfStart(VOID);
VOID	API ProfStop(VOID);
VOID	API ProfClear(VOID);
VOID	API ProfFlush(VOID);
VOID	API ProfFinish(VOID);
#endif

/* from mgesc.h */
int	FAR PASCAL MGControl(LPDV, int, LPSTR, LPSTR);

typedef struct {
	short ScaleMode;
	short dx, dy;
} DIBPARAMS;

typedef DIBPARAMS FAR *LPDIBPARAMS;

/*---------------------------- local data ----------------------------------*/

char clip_box[] = "%d %d 0 0 CB\n";

/*------------------------- local functions --------------------------------*/

int PASCAL MakeBinList(PPRINTER, LPSHORT);
void PASCAL CalcBreaks(LPJUSTBREAKREC, short, WORD);
BOOL PASCAL PaperRef(LPDV);
BOOL FAR PASCAL DumpPSSSafe(LPDV, short, short, short);


/* these functions are placed in a different segment to reduce the size of
 * _CNTRL (make it < 4k) */

BOOL FAR PASCAL OlivettiPrinter(LPDV);	/* in segment _STUBS */
BOOL FAR PASCAL InitDev(LPDV, LPSTR);	/* in segment _INITDEV */
void FAR PASCAL TermDev(LPDV);		/* in segment _INITDEV */

BOOL NEAR PASCAL DoResetDC(LPDV lpNewDV, LPDV lpOldDV);
BOOL NEAR PASCAL ResetDev(LPDV lpdvNew, LPDV lpdvOld);

void FAR PASCAL WriteResourceComments(LPDV lpdv);

#pragma alloc_text(_INITDEV, InitDev, TermDev)
#pragma alloc_text(_STUBS, OlivettiPrinter)


#define ESCAPE_PORTRAIT		1
#define ESCAPE_LANDSCAPE	2



#if 0 /* DISABLED */

 /* The paper metrics structure for the SELECTPAPERSOURCE escape */
typedef struct  {
	WORD x, y;
	RECT rcPage;
	WORD orientation;
} PM;
typedef PM FAR *LPPM;

#endif

/* this structure is for the SETALLJUSTVALUES escape...
 * added by Aldus Corporation--19Jan87--sjp
 */
typedef struct {
	short	nCharExtra;
	WORD	nCharCount;
	short	nBreakExtra;
	WORD	nBreakCount;
} ALLJUSTREC;
typedef ALLJUSTREC FAR *LPALLJUSTREC;



/* These next 2 typedefs are specially for
 * the GETSETPAPERBINS output data structures.
 * The 2nd one is only used for debug purposes
 * since the escape doesn't make assumptions on
 * the actual number of elements in the structure.
 */
typedef struct {
	short	binNum;
	short	numOfBins;
	short	spare1, spare2, spare3, spare4;
} GSPBOD;
typedef GSPBOD FAR *LPGSPBOD;

#define GETSETSCREENPARAMS 3072


BOOL FAR PASCAL DumpPSSSafe(LPDV lpdv, short iPrinter, short dirID, short pssID)
{
	register BOOL ret;

	PrintChannel(lpdv, "/oldDictCnt countdictstack def {");
	ret = DumpPSS(lpdv, iPrinter, dirID, pssID);
	PrintChannel(lpdv, "}stopped \n");
    PrintChannel(lpdv, "{ countdictstack oldDictCnt ");
    PrintChannel(lpdv, "lt { " WINDICT " begin } \n");
    PrintChannel(lpdv, "{1 1 countdictstack oldDictCnt sub ");
    PrintChannel(lpdv, "{pop end } for } ifelse } if \n");

//    This postscript code attempts to detect an error
//    in the execution of printer specific code.
//    If an error occurs, the dictionary stack is checked.
//    If we now have more dictionaries than what we started
//    with, Pop off excess dictionaries.
//    If we have fewer, then push PSWINDICT onto the stack.
//    Otherwise, assume the dictionary stack is ok.
//
//    /oldDictCnt countdictstack def  % save number of dictionaries
//    { some printer specific routine executed here }
//    stopped
//
//    {   % do this block if stopped returns true.
//        countdictstack oldDictCnt lt
//        {
//            WINDICT begin  % load WINDICT if dict stack has shrunk
//        } 
//        {       %  otherwise pop off excess dictionaries from dict stack
//            1 1 countdictstack oldDictCnt sub
//            {
//                pop end 
//            } for
//        } ifelse
//    } if
    
    
    return ret;
}


/*********************************************************************
 * Name: PaperRef()
 *
 * Action: This routine is called to refresh the paper-
 * handling characteristics for each output page.
 *
 *********************************************************************/

BOOL PASCAL PaperRef(LPDV lpdv)
{
	PPRINTER pPrinter;

	DBMSG1((">PaperRef(): iP=%d\n", lpdv->iPrinter));

	if (!(pPrinter = GetPrinter(lpdv->iPrinter)))
		goto ERROR;
	 
	/* Is manual feed supported by this printer? ... if so then
	 * make sure that manual feed is turned off or on depending
	 * on the state of iPaperSource, i.e. if manual feed is selected. */

	// should be do a %%BeginPageSetup / %%EndPageSetup here?

	if (pPrinter->feed[DMBIN_MANUAL - DMBIN_FIRST]) {
		DumpPSS(lpdv, lpdv->iPrinter, PSS_MANUALSWITCH, (lpdv->iPaperSource == DMBIN_MANUAL));
	}

#if 0
	/* this sets the paper source */
	/* right now we do this within the escape */

	DumpPSS(lpdv, lpdv->iPrinter, PSS_INPUTSLOT, lpdv->iPaperSource - DMBIN_FIRST);
#endif

	FreePrinter(pPrinter);

	DBMSG1(("<PaperRef()\n"));
	return TRUE;

ERROR:
	DBMSG1(("<PaperRef():  ERROR\n"));
	return FALSE;
}


/*********************************************************************
* Name: InitDev()
*
* Action: This routine is called to initialize the output
*	  device when the printer is enabled.
*
* this dumps the header
*
*  sept 2, 91: peterwo - initialize iPageNumber and DLState
*  at startdoc time not just at enable time.
**********************************************************************/

BOOL FAR PASCAL InitDev(lpdv, lszJob)
LPDV lpdv;	    /* Far ptr to the device descriptor */
LPSTR lszJob;	    /* Far ptr to the print job name */
{
#if 0
	int	paperID;
#endif
	int	iPrinter, i;
	PPRINTER pPrinter;
	char	buf[80];
	short	resID;
#ifdef APPLETALK
	char	userName[40];		/* used with AppleTalk */
#endif
   BOOL  fEOF;
   char  szCtrlDKey[20];

	DBMSG1((">InitDev()\n"));

	ASSERT(lpdv);
	ASSERT(ghInst);

	if (!(pPrinter = GetPrinter(lpdv->iPrinter)))
		return FALSE;

	iPrinter = lpdv->iPrinter;
    lpdv->iPageNumber = 0;       
    lpdv->DLState = DL_NOTHING;

        /* assume first page is dirty so nothing is output before header */
        lpdv->fDirty = TRUE;

        /* special hack for ELI printer: It needs a string output to tell
         * it this is a PostScript file.  Blecho!
         */
        if (!LoadString(ghInst, IDS_ELINAME, buf, sizeof(buf)))
            return FALSE;
        if (!(lpdv->fDoEps  || lstrcmpi(pPrinter->Name, buf)))
        {
            if (!LoadString(ghInst, IDS_ELICMD, buf, sizeof(buf)))
                return FALSE;
            WriteChannel(lpdv, buf, lstrlen(buf));
            PrintChannel(lpdv, newline);
        }

	/* Terminate any previous job before the new one,
	 * however, a few printers do not allow EOF! */

    SetKey(buf, pPrinter->Name, gPort);
    if (!LoadString(ghInst, IDS_CTRLD, szCtrlDKey, sizeof(szCtrlDKey)))
       return FALSE;
    fEOF = GetProfileInt(buf, szCtrlDKey, pPrinter->fEOF) && !lpdv->fDoEps;

#ifdef APPLE_TALK
	if (fEOF && !lpdv->fDoEps && !ATState())
#else
	if (fEOF && !lpdv->fDoEps)
#endif
		WriteChannelChar(lpdv, EOF);

	/* Print minimum Adobe Header Comments */
	if (lpdv->fDoEps) {
		LoadString(ghInst, IDS_EPSHEAD, buf, sizeof(buf));
		PrintChannel(lpdv, buf);
	} else {
		LoadString(ghInst, IDS_PSHEAD, buf, sizeof(buf));
		PrintChannel(lpdv, buf);
	}

	LoadString(ghInst, IDS_PSTITLE, buf, sizeof(buf));
	PrintChannel(lpdv, buf, (LPSTR)lszJob);

	LoadString(ghInst, IDS_EPSBBOX, buf, sizeof(buf));

	/* EPS bounding rect is already in default Postscript
	 * coords (1/72 of an inch) and  (0, 0 == lower left) */


	PrintChannel(lpdv, buf, lpdv->EpsRect.left,
		lpdv->EpsRect.bottom, lpdv->EpsRect.right, lpdv->EpsRect.top);

    /* Output resource comments */
    PrintChannel(lpdv, "%%%%DocumentNeededResources: (atend)\n%%%%DocumentSuppliedResources: (atend)\n");

    /* Output page comment */
    PrintChannel(lpdv, "%%%%Pages: ");
    if (lpdv->fDoEps)
        PrintChannel(lpdv, "0\n");
    else
        PrintChannel(lpdv, "(atend)\n");

	/*
	 * #ifndef ADOBE_11_1_88
	 * Make a decision here as to what type of header to download.
	 * There will be one of three choices:
	 *
	 * - if fHeader = TRUE download the PS header.
	 * - if fHeader = FALSE don't download the PS header just send code
	 *   to make sure its already there outside the exitserver loop.
	 * - if fEps is set download only the control portions of the header
	 *   setting the CTM and a clip rect to the imageable area.
	 */
	DBMSG1((" InitDev: fHeader=%d\n", lpdv->fHeader));
	DBMSG1((" InitDev: fEps=%d\n", lpdv->fEps));

	/* 03Apr91 msd Added support for the TrueImage header -- The
	 * TrueImage header takes on the same name as the normal PS
	 * header (WINDICT), so the permanent header test is the
	 * same.  For EPS files, TrueImage uses the same header.
	 *
	 * resID = (lpdv->fEps) ? PS_2 : (lpdv->fHeader) ? PS_HEADER : PS_1;
	 */
	if (lpdv->fEps)
		resID = PS_2;
	else if (lpdv->fHeader) {
		if (IS_TRUEIMAGE(pPrinter))
			resID = TI_HEADER;
		else
			resID = PS_HEADER;
	} else
		resID = PS_1;

	DBMSG((" InitDev: resID=%d\n", resID));

        if (lpdv->fHeader)
            PrintChannel(lpdv, "%%%%BeginResource: procset " WINDICT " 3 1\n");
        else
            PrintChannel(lpdv, "%%%%IncludeResource: procset " WINDICT " 3 1\n");

	DumpResourceString(lpdv, PS_DATA, resID);

        if (lpdv->fHeader)
            PrintChannel(lpdv, "%%%%EndResource\n");

        if (!lpdv->bDSC)
            PrintChannel(lpdv, "/SVDoc save def\n");

	PrintChannel(lpdv, "%%%%EndProlog\n");

        PrintChannel(lpdv, "%%%%BeginSetup\n");

        PrintChannel(lpdv, WINDICT " begin\n");

   /* if we are not downloading minimal header enable error handler */
   if (lpdv->bErrHandler  &&  resID != PS_2)
      PrintChannel(lpdv, "ErrHandler\n");

	/*
	 * #ifdef OLIVETTI_CHARSET
	 *
	 *	msd - 10/26/88:  If the printer is an Olivetti printer, then
	 *	download the PostScript stub which appends Olivetti's special
	 *	characters to the Windows ANSI character set.
	 */
	if (OlivettiPrinter(lpdv))
	    DumpResourceString(lpdv, PS_DATA, PS_OLIVCHSET);


	if (!lpdv->fDoEps) {

	        LoadString(ghInst, IDS_PSTIMEOUT, buf, sizeof(buf));
	        PrintChannel(lpdv, buf, lpdv->iJobTimeout);

	        if (lszJob) {

		        LoadString(ghInst, IDS_PSJOB, buf, sizeof(buf));
		        PrintChannel(lpdv, buf, (LPSTR)lszJob);

        #ifdef APPLE_TALK
		        if (ATState()) {
			        LoadString(ghInst, IDS_PREPARE, userName, sizeof(userName));
			        ATMessage(-1, 0, (LPSTR)userName);
		        }
        #endif
	        }

		// set the input slot.  this stuff comes from the
		// *InputSlot line in the PPD file

		DumpPSSSafe(lpdv, iPrinter, PSS_INPUTSLOT, lpdv->iPaperSource - DMBIN_FIRST);

		DBMSG1((" InitDev: tP.p [ (iP=%d)-(MIN=%d)=%d ] = %d pID=%d\n",
		    	lpdv->paper.iPaper, DMPAPER_FIRST, lpdv->paper.iPaper - DMPAPER_LAST,
		    	PaperSupported(pPrinter, lpdv->paper.iPaper), paperID));

		DBMSG((" InitDev: iPaper:%d\n", lpdv->paper.iPaper));


		/* only try to send a PostScript image/tray setup if
		 * the paper is available to the printer */

		if (PaperSupported(pPrinter, &lpdv->DevMode, lpdv->paper.iPaper)) {

			/* search here to get the index of the PSS table entry
		 	* that matches the index of the Paper[] array for the
		 	* current printer */
		 
                        i = FindPaperType(pPrinter, &lpdv->DevMode, lpdv->paper.iPaper, NULL);
                        if (i < 0) return FALSE;

            if (lpdv->iPaperSource == DMBIN_AUTO &&
                pPrinter->feed[DMBIN_AUTO - DMBIN_FIRST]) {
                  PrintChannel(lpdv, "{");
                  DumpPSS(lpdv, iPrinter, PSS_NORMALIMAGE, i);
                  PrintChannel(lpdv, "}stopped\n{");
                  DumpPSS(lpdv, iPrinter, PSS_MANUALIMAGE, i);
                  PrintChannel(lpdv, "}if\n");
            } else {

			      // the PSS_MANUALIMAGE coorisponds with the
			      // *PageRegion PPD file command.

			      DumpPSSSafe(lpdv, iPrinter, PSS_MANUALIMAGE, i);
            }
		}

		// set duplex mode
		if (IS_DUPLEX(pPrinter) && lpdv->iDuplex != -1) {
			DumpPSS(lpdv, lpdv->iPrinter, PSS_DUPLEX, lpdv->iDuplex);
		}

		// this will concatinate the PPD "*Transfer Normalized"
		// command to the current transfer, thus preserving
		// any transfer functions that have already been set

		PrintChannel(lpdv, "[");
		DumpPSS(lpdv, iPrinter, PSS_TRANSFER, -1);
		PrintChannel(lpdv, "/exec load currenttransfer /exec load] cvx settransfer\n");
	}

        // output screen parameters IF they are not the default AND we are
        // NOT generating DSC output
        if (!lpdv->bDSC && (lpdv->angle != pPrinter->ScreenAngle
                             || lpdv->freq != pPrinter->ScreenFreq)) {
	    LoadString(ghInst, IDS_SETSCREEN, buf, sizeof(buf));
	    PrintChannel(lpdv, buf, lpdv->freq, lpdv->angle);
        }

        // if negative image requested invert current transfer function
        if (lpdv->bNegImage) {
	    LoadString(ghInst, IDS_NEGIMAGE, buf, sizeof(buf));
	    PrintChannel(lpdv, buf);
        }

        // if per page downloading requested set the flag
        if (lpdv->bPerPage)
            PrintChannel(lpdv, "/fPP true def\n");

        // output resolution control sequence (does nothing if setresolution doesn't exist)
        if (!lpdv->fDoEps) {
            BOOL bSetResolution;
            
            /* assume we will be setting the resolution */
            bSetResolution = TRUE;

            /* check to see if resolution matches (a) physical resolution */
	    if (lpdv->iRealRes != pPrinter->defRes) {
                for (i = 0; i < NUMDEVICERES; ++i) {
                    int n;

                    n = pPrinter->iDeviceRes[i];

		    if (!n || n == lpdv->iRealRes)
                        break;
                }

                if (i == NUMDEVICERES
		    || pPrinter->iDeviceRes[i] != lpdv->iRealRes)
                    bSetResolution = FALSE;
            }

            if (bSetResolution) {
	        LoadString(ghInst, IDS_SETRESOLUTION, buf, sizeof(buf));
		PrintChannel(lpdv, buf, lpdv->iRealRes, lpdv->iRealRes);
            }
        }

        PrintChannel(lpdv, "%%%%EndSetup\n");

	DBMSG1(("<InitDev()\n"));

	FreePrinter(pPrinter);

        /* now that header is written assume clean so that the next 
         * output call generates the page initialization code.
         */
        lpdv->fDirty = FALSE;

#if 0
        /* initialize the string lists to empty */
        if (!(lpdv->slNeeded = CreateStringList())
            || !(lpdv->slSupplied = CreateStringList())
            || !(lpdv->slPageResources = CreateStringList())
            || !(lpdv->slTTFonts = CreateStringList())
           )
           return FALSE;
#endif
    if (lpdv->slNeeded = CreateStringList())
    {
        if (lpdv->slSupplied = CreateStringList())
        {
            if (lpdv->slPageResources = CreateStringList())
            {
        	    return lpdv->fh >= 0;
            }
            else
    	        goto StringListFailed2;
    	}
        else
            goto StringListFailed1;
    }
	else
	    goto Exit;


StringListFailed2:
    DeleteStringList(lpdv->slSupplied);

StringListFailed1:
    DeleteStringList(lpdv->slNeeded);

Exit:
    return FALSE;
}


/*********************************************************************
 * Name: TermDev()
 *
 * Action: This routine is called at the end of a print job.
 *
 *********************************************************************/

void FAR PASCAL TermDev(lpdv)
LPDV lpdv;
{
	PPRINTER pPrinter;
        char buf[80];

	DBMSG1((">TermDev()\n"));
	ASSERT(lpdv);

        lpdv->fDirty = TRUE; /* don't force new page */

	PrintChannel(lpdv, "%%%%Trailer\n");	/* match the WINDICT begin */


         if (!lpdv->bDSC)
            PrintChannel(lpdv, "SVDoc restore\n");

        /* close WINDICT */
        PrintChannel(lpdv, "end\n");

        /* write out page count */
        if (!lpdv->fDoEps)
            PrintChannel(lpdv, "%%%%Pages: %d\n", lpdv->iPageNumber);

        /* write out the Resource Comments */
        WriteResourceComments(lpdv);

        PrintChannel(lpdv, "%%%%EOF\n");

	/* see if we should dump EOF */

	if (!(pPrinter = GetPrinter(lpdv->iPrinter)))
		return;


#ifdef APPLE_TALK
	if (pPrinter->fEOF && !lpdv->fDoEps && !ATState()) {
#else
	if (pPrinter->fEOF && !lpdv->fDoEps) {
#endif
		WriteChannelChar(lpdv, EOF);	/* yes */
            /* special hack for ELI printer: It needs a string output to tell
             * it the job is done.  Blecho!
             */
            if (!LoadString(ghInst, IDS_ELINAME, buf, sizeof(buf)))
                return;
            if (!(lpdv->fDoEps  || lstrcmpi(pPrinter->Name, buf)))
            {
                if (!LoadString(ghInst, IDS_ELICMD, buf, sizeof(buf)))
                    return;
                WriteChannel(lpdv, buf, 9);
            }
        }

	CloseChannel(lpdv);
#ifdef APPLE_TALK
	KillAT();
#endif

        /* empty font list */
        /* this function now only clears flags  */
        TTFlushFonts(lpdv);

        /* Destroy string lists */
        DeleteStringList(lpdv->slNeeded);
        DeleteStringList(lpdv->slSupplied);
        DeleteStringList(lpdv->slPageResources);

        /* delete TT atom list */
        /*  perform this only at Disable() FreeTTFontTable(lpdv); */

	FreePrinter(pPrinter);

	DBMSG1(("<TermDev()\n"));

	return;
}


/*********************************************************************
 * CalcBreaks
 *
 * fill in the JUSTBREAKREC according to BreakExtra and Count
 *
 * note: BreakExtra can be negative.
 *
 * Aldus Corporation--19January87
 *********************************************************************/

void PASCAL CalcBreaks (lpJustBreak, BreakExtra, Count)
LPJUSTBREAKREC	lpJustBreak;
short		BreakExtra;
WORD		Count;
{
	DBMSG(("***CalcBreaks(): Count=%d,BreakExtra=%d\n",
	    Count, BreakExtra));

	if (Count > 0) {
		/* Fill in JustBreak values.  May be positive or negative.
		 */
		lpJustBreak->extra = BreakExtra / (short)Count;
		lpJustBreak->rem = BreakExtra % (short)Count;
		lpJustBreak->err = (short)Count / 2 + 1;
		lpJustBreak->count = Count;
		lpJustBreak->ccount = 0;

		DBMSG(((LPSTR)">0 e=%d,r=%d,e=%d,c=%d,cc=%d\n",
		    lpJustBreak->extra, lpJustBreak->rem, lpJustBreak->err,
		    lpJustBreak->count, lpJustBreak->ccount));

		/* Negative justification:  invert rem so the justification
		 * algorithm works properly.
		 */
		if (lpJustBreak->rem < 0) {
			--lpJustBreak->extra;
			lpJustBreak->rem += (short)Count;

			DBMSG(((LPSTR)"Neg.Just. e=%d,r=%d\n",
			    lpJustBreak->extra, lpJustBreak->rem));
		}
	} else {
		/* Count = zero, set up justification rec so the algorithm
		 * always returns zero adjustment.
		 */
		lpJustBreak->extra = 0;
		lpJustBreak->rem = 0;
		lpJustBreak->err = 1;
		lpJustBreak->count = 0;
		lpJustBreak->ccount = 0;

		DBMSG(((LPSTR)"=0 e=%d,r=%d,e=%d,c=%d,cc=%d\n",
		    lpJustBreak->extra, lpJustBreak->rem, lpJustBreak->err,
		    lpJustBreak->count, lpJustBreak->ccount));
	}
}



/*********************************************************************/

int PASCAL MakeBinList(PPRINTER pPrCaps, LPSHORT lpBinList)
{
	short	i;
	short	num_in_list = 0;

	DBMSG2((">MakeBinList()\n"));

	for (i = 0; i < NUMFEEDS; i++) {

		if (pPrCaps->feed[i]) {

			*lpBinList++ = i + DMBIN_FIRST;

			num_in_list++;
		}
	}

	return num_in_list;
}


/*********************************************************************
 * Name: Control()
 *
 * Action: This routine is the low-level version of the
 *	  device Escape function.  It handles a hodgepodge
 *	  of somewhat unrelated functions.  Its main purpose
 *	  is to allow enhancements to the printer driver
 *	  functionality without altering the GDI interface.
 *
 * some of these escapes modify the current enviornment (defined by
 * the PSDEVMODE struct).  these changes are for the most part saved
 * permenently (in win.ini).  these changes are also updated in the
 * PDEVICE (DV) struct when necessary.  this routine and Enable() are
 * the only places where data is copied from PSDEVMODE to PDEVICE.
 *
 * note: many of these routines were added by many different people
 *	 (not all of them having a good understanding for this code).
 *	 much of this code should be reviewd for correctness and
 *	 efficiency.
 *
 *********************************************************************/

int FAR PASCAL Control(lpdv, ifn, lpbIn, lpbOut)
LPDV	lpdv;	    /* ptr to the device descriptor */
int	ifn;	    /* the escape function to execute */
LPSTR	lpbIn;	    /* ptr to input data for the escape function */
LPSTR	lpbOut;	    /* ptr to the output buffer for the esc function*/
{
	register int	iResult;
	short	i;
        char szBuf[80];

	DBMSG((">Control():  escFunc=%d\n", ifn));

	ASSERT(lpdv);

	if (ifn >= 4096 && ifn <= 4109)
		return MGControl(lpdv, ifn, lpbIn, lpbOut);

	switch (ifn) {

	case QUERYESCSUPPORT:
		switch (*(LPSHORT)lpbIn) {
		case NEXTBAND:
		case NEWFRAME:
		case ABORTDOC:
		case QUERYESCSUPPORT:
		case FLUSHOUTPUT:
		case STARTDOC:
		case ENDDOC:
		case GETPRINTINGOFFSET:
		case GETPHYSPAGESIZE:
	        case GETSCALINGFACTOR:
		case SETCOPYCOUNT:
		case PASSTHROUGH:
		case GETEXTENDEDTEXTMETRICS:
//		case GETEXTENTTABLE:
		case GETPAIRKERNTABLE:
		case SETKERNTRACK:
		case SETCHARSET:
		case ENABLEPAIRKERNING:
		case GETTECHNOLOGY:
		case SETLINECAP:
		case SETLINEJOIN:
		case SETMITERLIMIT:
		case GETSETPRINTERORIENTATION:
		case GETSETPAPERBINS:
      case GETSETSCREENPARAMS:
		case ENUMPAPERBINS:
		case SETALLJUSTVALUES:	/* Aldus Corporation--19Jan87--sjp */
		case GETFACENAME:	/* added by ADOBE_11_1_88 */
		case DOWNLOADFACE:
		case EPSPRINTING:
		case ENUMPAPERMETRICS:
		case GETSETPAPERMETRICS:
		case GETVERSION:
		case SETDIBSCALING:

		case BEGIN_PATH:
		case CLIP_TO_PATH:
		case END_PATH:
		case EXT_DEVICE_CAPS:
		case SET_ARC_DIRECTION:
		case SET_POLY_MODE:
		case RESTORE_CTM:
		case SAVE_CTM:
		case TRANSFORM_CTM:
		case SET_CLIP_BOX:
		case SET_BOUNDS:
		case SET_SCREEN_ANGLE:
		case SETABORTPROC:
                case RESETDC:

#ifdef PS_IGNORE
		case POSTSCRIPT_DATA:
		case POSTSCRIPT_IGNORE:
#endif

			DBMSG(("Query # %d supported\n", *(LPSHORT)lpbIn));
			return TRUE;
			break;

	        case ENABLEDUPLEX:
		        if (lpdv->iDuplex != -1)
			        return TRUE;
		        else
			        return FALSE;
		        break;

		default:
			DBMSG(("Query # %d not supported\n", *(LPSHORT)lpbIn));
			return FALSE;
			break;
		}
		break;


	case SETDIBSCALING:
		i = ((LPDIBPARAMS)lpbIn)->ScaleMode;

		if (i >= 0 && i <= 2) {

			DBMSG(("SETDIBSCALING mode:%d dx:%d dy:%d\n", 
				((LPDIBPARAMS)lpbIn)->ScaleMode,
				((LPDIBPARAMS)lpbIn)->dx,
				((LPDIBPARAMS)lpbIn)->dy));

			iResult = lpdv->ScaleMode;

			lpdv->ScaleMode = (char)i;
			lpdv->dx = ((LPDIBPARAMS)lpbIn)->dx;
			lpdv->dy = ((LPDIBPARAMS)lpbIn)->dy;

		} else
			iResult = -1;


		return iResult;

	/* this escape is required by many apps that call next band without
	 * first QUERYESCSUPPORTing it eventhough we are not a banding device */

	case NEXTBAND:

		SetRectEmpty((LPRECT)lpbOut);
		if (lpdv->fh >= 0) {
			if (lpdv->iBand == 0) {
				((LPRECT) lpbOut)->bottom = lpdv->paper.cyPaper;
				((LPRECT) lpbOut)->right = lpdv->paper.cxPaper;
				lpdv->iBand = 1;	/* we've done a band */
			} else {
				lpdv->iBand = 0;

				Control(lpdv, NEWFRAME, 0L, 0L);
			}
		}


		DBMSG(("NEXTBAND %d %d %d %d result:%d\n", 
			((LPRECT)lpbOut)->left, 
			((LPRECT)lpbOut)->top,
			((LPRECT)lpbOut)->right,
			((LPRECT)lpbOut)->bottom, iResult));

		return lpdv->fh;
		break;

        case 0x33:
	case NEWFRAME:
		/* invalidate pens and brushes and fonts */

		lpdv->fPenSelected = FALSE;
		lpdv->GraphData = GD_NONE;
		lpdv->lidFont = -1L;

                if (lpdv->bPerPage) {
                    lpdv->nextSlot = -1;
		    lpdv->DLState = DL_NOTHING;	/* reset download state */
                }

		lpdv->FillMode = 0;	/* reset fill mode */
		lpdv->FillColor = -1L;

		// get all this stuff out before we EndSpoolPage()
		// so that we can begin printing.

		if (!lpdv->fDoEps) {

			/* # of copies */
			PrintChannel(lpdv, "%d #C\n", lpdv->iCopies);

			/* refresh paper source */
			PaperRef(lpdv);
		}

		/* EJ=eject, RS=RestoreState */

		PrintChannel(lpdv, "EJ RS\n%%%%PageTrailer\n");

                /* write page resource comments */
		if (lpdv->fh >= 0 && !lpdv->fDoEps) {
                    PrintChannel(lpdv, "%%%%PageResources: ");
                    if (EnumStrings(lpdv->slPageResources, szBuf, 80)) {
                        PrintChannel(lpdv, "font %s\n", (LPSTR) szBuf);

                        while (EnumStrings(NULL, szBuf, 80))
                            PrintChannel(lpdv, "%%%%+ font %s\n", (LPSTR) szBuf);
                    } else
                        PrintChannel(lpdv, newline);
                }

		/* ADOBE_11_1_88 */

		if (lpdv->fEps) {
			PrintChannel(lpdv, clip_box,
			    lpdv->paper.cxPage,
			    lpdv->paper.cyPage);
		}

		lpdv->fDirty = FALSE;		/* current page is empty */

		if (lpdv->fh >= 0) {

			/* let the spooler start printing */
			if ((iResult = EndSpoolPage(lpdv->fh)) < 0)
				return iResult;

			if ((iResult = StartSpoolPage(lpdv->fh)) < 0)
				return iResult;
		}

		return lpdv->fh;	// this will < 0 on SP_ error condition
		break;

	case ABORTDOC:
		if (lpdv->fh >= 0)
		    TermDev(lpdv);
		break;

	case GETEXTENTTABLE:
		return(GetExtTable(lpdv, (LPEXTTEXTDATA)lpbIn, (LPSHORT)lpbOut));
		break;

/* shut this escape off for now...
 *	case ENABLERELATIVEWIDTHS:
 *		if (lpbIn)
 *			lpdv->fIntWidths = !*lpbIn;
 *		break;
 */
	case GETEXTENDEDTEXTMETRICS:
		return(GetEtm(lpdv, (LPEXTTEXTDATA)lpbIn, lpbOut));
		break;

	/* ADOBE_11_1_88 */
	case GETFACENAME:
		return(GetPSName(lpdv, (LPEXTTEXTDATA)lpbIn, lpbOut));
		break;

	case DOWNLOADFACE:
		if ((*((short far * )lpbOut) == 1) && (lpdv->fh <= 0))
			return(-1);
		else
			return(PutFontInline(lpdv, (LPEXTTEXTDATA)lpbIn, *((short far * )lpbOut)));
		break;

	/* this escape makes the driver output a minimal header.  this
	 * is intended for apps that produce their own postscript */

	case EPSPRINTING:
		/* check to see if we need to toggle */
		if (iResult = lpdv->fEps != *(LPSHORT)lpbIn)
			lpdv->fEps = *(LPSHORT)lpbIn;

		return iResult ? 1 : -1;
		break;

	case ENUMPAPERMETRICS:
		{
		short	rc;

		DBMSG(("ENUMPAPERMETRICS in:%lx out:%lx\n", lpbIn, lpbOut));

		rc = EnumPaperMetrics(lpdv, (LPRECT)lpbOut, *((short far * )lpbIn), 0);
		if (rc == -1)
			rc = EnumPaperMetrics(lpdv, (LPRECT)lpbOut, *((short far * )lpbIn), 1);
		DBMSG(("    return %d\n", rc));
		return rc;
		}
		break;

	case ENABLEDUPLEX:
		{
		if (lpdv->iDuplex != -1) {
			short duplex = *(short far *)lpbIn;
			if (duplex == 1)
				lpdv->iDuplex = DMDUP_VERTICAL - DMDUP_SIMPLEX;
			else if (duplex == 2)
				lpdv->iDuplex == DMDUP_HORIZONTAL - DMDUP_SIMPLEX;
			else
				lpdv->iDuplex == 0;
			return 1;
		} else
			return 0;
		}
		break;

	case GETSETPAPERMETRICS:
		{
		short	rc;
		PPSDEVMODE poriginalDM;
		PPSDEVMODE pnewDM;

		DBMSG(("GETSETPAPERMETRICS in:%lx out:%lx\n", lpbIn, lpbOut));

		if (lpbOut) {
			SetRect((LPRECT)lpbOut,
				lpdv->paper.cxMargin,
				lpdv->paper.cyMargin,
				lpdv->paper.cxMargin + lpdv->paper.cxPage,
				lpdv->paper.cyMargin + lpdv->paper.cyPage);

			DBMSG(("    GET rc: %d %d %d %d\n", 
				((LPRECT)lpbOut)->left, ((LPRECT)lpbOut)->top, ((LPRECT)lpbOut)->right, ((LPRECT)lpbOut)->bottom));

			rc = 1;
		}

		if (lpbIn) {

         /* allocate the devmode structures */
         rc = -1;  /* assume failure... */
         poriginalDM = AllocDevMode();
         if (!poriginalDM) goto cleanup_none;
         pnewDM = AllocDevMode();
         if (!pnewDM) goto cleanup_original;

			/* get the current env */

			MakeEnvironment(lpdv->szDevType,
			    lpdv->szFile, poriginalDM, NULL);

			*pnewDM = *poriginalDM;	/* make new copy */

			DBMSG(("    SET rc: %d %d %d %d\n", ((LPRECT)lpbIn)->left, ((LPRECT)lpbIn)->top, ((LPRECT)lpbIn)->right, ((LPRECT)lpbIn)->bottom));

			rc = SetPaperMetrics(lpdv, pnewDM, (LPRECT)lpbIn);

			if (rc != -1) {

				// save changes to newDM and lpdv in win.ini

				SaveEnvironment(lpdv->szDevType,
				    	lpdv->szFile, pnewDM, 
				    	poriginalDM, NULL, TRUE, TRUE);
			}

         FreeDevMode(pnewDM);
cleanup_original:
         FreeDevMode(poriginalDM);
cleanup_none:
         ;
		}

		DBMSG(("    return %d\n", rc));

		return rc;
		}
		break;

	/* Adobe added this.  this probally isn't that good of an idea.
	 * if apps querey a driver for a version # they usualy are going
	 * to assume something.  be careful here */

	case GETVERSION:		/* from Adobe 3.1+ */
		return 100;


	case GETPAIRKERNTABLE:
		return(GetPairKern(lpdv, (LPEXTTEXTDATA)lpbIn, lpbOut));
		break;

	case FLUSHOUTPUT:
		if (lpdv->fh >= 0)
			FlushChannel(lpdv);
		break;

	case STARTDOC:
		DBMSG2((" Control(): STARTDOC\n"));

      ConfigureCommPort(); /* configure the comm port if appropriate */

		iResult = SP_ERROR;	/* general error -1 */

		/* Do an explicit ENDDOC if the user forgot to do it */
		if (lpdv->fh >= 0) 
			Control(lpdv, ENDDOC, 0L, 0L);

#if 0
		// Explicitly set all StringList handles to 0

		lpdv->slPageResources =
		lpdv->slSupplied =
		lpdv->slNeeded =
		lpdv->slTTFonts = 0;
#endif

#ifdef APPLE_TALK
		ATQuery(lpdv->szFile);

		if (lpdv->fDoEps) {

			ATChangeState(FALSE);	/* turn AppleTalk off if
			 			 * doing EPS output */
		}

		if (iResult = TryAT()) {
			if (iResult > 0) 
				iResult = SP_USERABORT;
			goto END_STARTDOC;
		}
#endif

		/* Open the output channel: note lpbIn is the document name */
		if ((iResult = OpenChannel(lpdv, lpbIn, (LPDOCINFO)lpbOut)) < 0)
			goto END_STARTDOC;

#ifdef PROFILE
		ProfSetup(512, 0);
		ProfSampRate(4, 1);
		ProfStart();
#endif

		/* Initialize the device */
		if (!InitDev(lpdv, lpbIn)) {
			CloseChannel(lpdv);
			iResult = SP_ERROR;
			goto END_STARTDOC;
		}

		/* ADOBE_11_1_88 */
		if (lpdv->fEps) {
			PrintChannel(lpdv, clip_box, lpdv->paper.cxPage, lpdv->paper.cyPage);
		}

		/* clear the temporary softfonts array */
		lpdv->nextSlot = -1;


		/* everything is A.O.K. */
		iResult = 1;

END_STARTDOC:
		DBMSG2((" Control(): STARTDOC exit\n"));
		return(iResult);
		break;

	case ENDDOC:
		if (lpdv->fh >= 0)
		    {
		    if (lpdv->fDirty)
			    Control(lpdv, NEWFRAME, 0L, 0L);

		    TermDev(lpdv);
		    }

#ifdef PROFILE
	    ProfStop();
	    ProfFinish();
#endif
		break;

	case SETABORTPROC:
		lpdv->hdc = *(HDC FAR * )lpbIn;
		break;

	case GETPHYSPAGESIZE:
		((LPPOINT)lpbOut)->xcoord = lpdv->paper.cxPaper;
		((LPPOINT)lpbOut)->ycoord = lpdv->paper.cyPaper;

		DBMSG(("GETPHYSPAGESIZE %d %d\n", lpdv->paper.cxPaper,
						 lpdv->paper.cyPaper));
		break;

	case GETPRINTINGOFFSET:
		((LPPOINT)lpbOut)->xcoord = lpdv->paper.cxMargin;
		((LPPOINT)lpbOut)->ycoord = lpdv->paper.cyMargin;
		DBMSG(("GETPRINTINGOFFSET %d %d size %d %d\n",
			lpdv->paper.cxMargin, lpdv->paper.cyMargin,
			lpdv->paper.cxPage, lpdv->paper.cyPage));
		break;

	case GETSCALINGFACTOR:
		((LPPOINT)lpbOut)->xcoord = ((LPPOINT)lpbOut)->ycoord = 0;
		break;

	case SETCOPYCOUNT:
		if (lpbIn)
			lpdv->iCopies = *((LPSHORT)lpbIn);

		if (lpbOut)
			*((LPSHORT)lpbOut) = lpdv->iCopies;

		iResult = TRUE;
		break;

#if 0	/* DISABLED */
	case SELECTPAPERSOURCE:
		DBMSG1(((LPSTR)"***SELECTPAPERSOURCE\n"));
		if (!SetPaperSource(lpdv, *((short far * )lpbIn))) 
			return(-1);

		if (lpbOut) {
			((LPPM)lpbOut)->x = lpdv->paper.cxPaper;
			((LPPM)lpbOut)->y = lpdv->paper.cyPaper;

#if 0
			/* big lie: */
			((LPPM)lpbOut)->rcPage.top = 0;
			((LPPM)lpbOut)->rcPage.left = 0;
			((LPPM)lpbOut)->rcPage.bottom = lpdv->paper.cyPage;
#endif
			((LPPM)lpbOut)->rcPage.top = lpdv->paper.cyMargin;
			((LPPM)lpbOut)->rcPage.left = lpdv->paper.cxMargin;
			((LPPM)lpbOut)->rcPage.bottom = lpdv->paper.cyMargin + 
			    lpdv->paper.cyPage;

			((LPPM)lpbOut)->orientation = lpdv->fLandscape ? 2 : 1;
		}
		DBMSG1(("SELECTPAPERSOURCE***\n"));
		break;
#endif

#ifdef PS_IGNORE
	case POSTSCRIPT_IGNORE:

		i = lpdv->fSupressOutput;
		lpdv->fSupressOutput = (BOOL)*lpbIn;

		return i;	// return previous value
		break;

	case POSTSCRIPT_DATA:
#endif
	case PASSTHROUGH:
		{
		unsigned int cb;

		if (lpbIn) {
			cb = *((unsigned int far * )lpbIn)++;

			WriteChannel(lpdv, lpbIn, cb);

			if (lpdv->fh >= 0) 
				return cb;
			else 
				return lpdv->fh;
		}
		return lpdv->fh;
		break;
		}

	case SETKERNTRACK:
		i = lpdv->iTrack;
		lpbIn = (LPSTR) ((LPETD)lpbIn)->lpInData;
		if (lpbIn) 
			lpdv->iTrack = *((short far * )lpbIn);
		if (lpbOut) 
			*((short far * )lpbOut) = i;
		break;

	case SETCHARSET:
		// this is a bogus undocumented escape that is used
		// by various unnamed apps to "switch" between the standard
		// ANSI char set and a special publishing char set.
		// if *lpbIn == 1 we use the publishing set
		//	char 145 single open quote
		//	char 146 single close quote
		//	char 147 double open quote
		//	char 148 double close quote
		//	char 149 bullet
		//	char 150 En dash
		//	char 151 Em dash
		//	char 160 non breaking space
		// if *lpbIn == 0 we use standard ANSI.
		//
		// the way this works is these chars (in the publishing set)
		// are always their.  so this just serves as a means
		// to inform the app that we support this stuff

		DBMSG(("SETCHARSET %d\n", *((WORD FAR *)lpbIn)));

		// instead of actually checking for valid parameters
		// we assume the caller knows what he is doing (this is
		// undocumented anyway) and just say, hey! cool man, go
		// for it.

#if 0
		// valid params?

		if (*((WORD FAR *)lpbIn) < 2) 
			return TRUE;
		else 
			return FALSE;
#else
		return TRUE;	// what I said above
#endif
		break;

		/* added by Aldus Corporation--19Jan87--sjp */
	case SETALLJUSTVALUES:
		{
		LPEXTTEXTDATA lpExtText = (LPEXTTEXTDATA)lpbIn;
		LPALLJUSTREC lpAllJust = (LPALLJUSTREC)lpExtText->lpInData;
		LPDRAWMODE lpDrawMode = lpExtText->lpDrawMode;
		LPFONTINFO lpFont = lpExtText->lpFont;

		DBMSG(((LPSTR)"SETALLJUSTVALUES\n"));
		DBMSG(((LPSTR)"TBreakExtra=%d\n", lpDrawMode->TBreakExtra));
		DBMSG(((LPSTR)"BreakExtra=%d\n", lpDrawMode->BreakExtra));
		DBMSG(((LPSTR)"BreakErr=%d\n", lpDrawMode->BreakErr));
		DBMSG(((LPSTR)"BreakRem=%d\n", lpDrawMode->BreakRem));
		DBMSG(((LPSTR)"BreakCount=%d\n", lpDrawMode->BreakCount));
		DBMSG(((LPSTR)"CharExtra=%d\n", lpDrawMode->CharExtra));

		lpDrawMode->BreakErr = 1;
		lpDrawMode->TBreakExtra = 0;
		lpDrawMode->BreakExtra = 0;
		lpDrawMode->BreakRem = 0;
		lpDrawMode->BreakCount = 0;
		lpDrawMode->CharExtra = 0;

		DBMSG(((LPSTR)"TBreakExtra=%d\n", lpDrawMode->TBreakExtra));
		DBMSG(((LPSTR)"BreakExtra=%d\n",  lpDrawMode->BreakExtra));
		DBMSG(((LPSTR)"BreakErr=%d\n",    lpDrawMode->BreakErr));
		DBMSG(((LPSTR)"BreakRem=%d\n",    lpDrawMode->BreakRem));
		DBMSG(((LPSTR)"BreakCount=%d\n",  lpDrawMode->BreakCount));
		DBMSG(((LPSTR)"CharExtra=%d\n",   lpDrawMode->CharExtra));

		if (lpFont->dfCharSet == OEM_CHARSET) {
		 /*	Vector font: disable ALLJUSTVALUES and
		  *	return false.
		  */
			lpdv->epJust = fromdrawmode;
			DBMSG(((LPSTR)" Control(): vector font disable and return\n"));
			return(0);
		}

		if (lpbIn) {

			CalcBreaks (&lpdv->epJustWB, lpAllJust->nBreakExtra,
			    lpAllJust->nBreakCount);
			CalcBreaks (&lpdv->epJustLTR, lpAllJust->nCharExtra,
			    lpAllJust->nCharCount);

			if (lpdv->epJustWB.extra || lpdv->epJustWB.rem || 
			    lpdv->epJustLTR.extra || lpdv->epJustLTR.rem
			    ) {
				if (lpdv->epJustLTR.rem) {
					lpdv->epJust = justifyletters;
				} else {
					lpdv->epJust = justifywordbreaks;
				}
			} else {
				/* Zero justification == shut off ALLJUSTVALUES */
				lpdv->epJust = fromdrawmode;
				DBMSG(((LPSTR)"zero just.\n"));
			}
		}

		}
		break;

	case ENABLEPAIRKERNING:
		i = lpdv->fPairKern;
		lpbIn = (LPSTR) ((LPETD)lpbIn)->lpInData;
		if (lpbIn) 
			lpdv->fPairKern = *((short far * )lpbIn);
		if (lpbOut) 
			*((short far * )lpbOut) = i;
		break;

	case GETTECHNOLOGY:

		lstrcpy(lpbOut, "PostScript");

#if 0

		/* is this a valid binary port? */
		if (lpdv->fBinary) {
			lstrcat(lpbOut, "binary");
		}
#endif

		/* always a double NULL at end of string */
		lpbOut[lstrlen(lpbOut)+1] = '\0';
		break;

	case SETLINECAP:
		i = lpdv->iNewLineCap;
		if (lpbIn)
			lpdv->iNewLineCap = *((short far * ) lpbIn);
		if (lpbOut)
			*((short far * ) lpbOut) = i;
		break;

	case SETLINEJOIN:
		i = lpdv->iNewLineJoin;
		if (lpbIn)
			lpdv->iNewLineJoin = *((short far * ) lpbIn);
		if (lpbOut)
			*((short far * ) lpbOut) = i;
		break;

	case SETMITERLIMIT:
		i = lpdv->iNewMiterLimit;
		if (lpbIn)
			lpdv->iNewMiterLimit = *((short far * ) lpbIn);
		if (lpbOut)
			*((short far * ) lpbOut) = i;
		break;

	case GETSETPRINTERORIENTATION:
		 {
		/* lpInData is set up in the following way...
		 * struct { short orientation,spare1,spare2,spare3; } lpInData;
		 */
		PPSDEVMODE poldDM;
		short	orientation;
		short	rc = ESCAPE_PORTRAIT;

		DBMSG2(((LPSTR)" Control(): GETSETPRINTERORIENTATION rc=%d\n", rc));

		/* always return the present value of the orientation...
		 * unless there is an error
		 */
		if (lpdv->fLandscape)
			rc = ESCAPE_LANDSCAPE;
		DBMSG2(((LPSTR)" Control(): rc=%d\n", rc));

		/* if NULL then return the current orientation
		 * (previously set)...if NOT NULL process...
		 */
		if (lpbIn) {
			orientation = *((short far * )lpbIn);

			DBMSG2(((LPSTR)" Control(): CHANGE o=%d\n", orientation));

			/* check to make sure the orientation is withing the
			 * available bounds
			 */
			if (orientation <= ESCAPE_LANDSCAPE && orientation >= ESCAPE_PORTRAIT) {

            /* allocate the devmode structure */
            poldDM = AllocDevMode();
            if (poldDM) {
				   /* make the environment...doesn't fail */
				   MakeEnvironment(lpdv->szDevType,
				      lpdv->szFile, poldDM, NULL);

				   if (orientation == ESCAPE_LANDSCAPE) 
					   poldDM->dm.dmOrientation = DMORIENT_LANDSCAPE;
				   else 
					   poldDM->dm.dmOrientation = DMORIENT_PORTRAIT;

				   /* Update the environment with the new info and inform
			 	   * everyone.
				   */
				   SaveEnvironment(lpdv->szDevType,
					   lpdv->szFile, poldDM,
					   NULL, NULL, FALSE, TRUE);

               /* get rid of devmode structure */
               FreeDevMode(poldDM);
            } else {
               /* devmode allocation failed */
               rc = -1;
            }

			} else {
				rc = -1; /* invalid selection */
				DBMSG2(((LPSTR)" Control(): ERROR rc=%d\n", rc));
			}
		}
		DBMSG2(("<Control(): GETSETPRINTERORIENTATION exit rc=%d\n",
		    rc));
		return rc;
		}
		break;

	/* 88Jan10 chrisg added support for change of paper bin while
	 * printing */

	case GETSETPAPERBINS:
		{
//		PPSDEVMODE poldDM;	/* used to get/save env */
//		PPSDEVMODE pnewDM;	/* used to get/save env */
		PPRINTER pPrinter;      /* temp printer struct */
		short	bin[NUMFEEDS];	/* bin array */
		short	num, i, num_bins;
		BOOL	fSave, fSaveDirty;

		DBMSG((" Control():>GETSETPAPERBINS\n"));


		/* get the bin data for this printer */

		if (!(pPrinter = GetPrinter(lpdv->iPrinter)))
			return FALSE;

		num_bins = MakeBinList(pPrinter, bin);

		if (lpbOut) {	/* GET */

#if 0	// fix bug #15167
	// should enum paper bin indices instead of id's, as required by
	// AmiPro. (ZhanW 2/21/92)
	// lpdv->iPaperSource still stores the bin id in order to minimize
	// the changes necessary.

			((LPGSPBOD)lpbOut)->binNum = lpdv->iPaperSource;
#else
			for (i = 0; i < num_bins; i++) {
			    if (lpdv->iPaperSource == bin[i]) {
				((LPGSPBOD)lpbOut)->binNum = i;
				break;
				}
			}
#endif
			((LPGSPBOD)lpbOut)->numOfBins = num_bins;

			DBMSG((" Control():>GETSETPAPERBINS GET bin num:%d  num bins:%d\n",
				((LPGSPBOD)lpbOut)->binNum,
				((LPGSPBOD)lpbOut)->numOfBins));
		}


		if (lpbIn) {	/* SET */

#if 0	// fix bug #15167 (part 2)
	// do NOT update the global printer settings! The change of paper
	// bin is on per job basis.  (ZhanW 2/21/92)

         /* alloc devmode structures */
         poldDM = AllocDevMode();
         if (!poldDM) goto cleanup2_none;
         pnewDM = AllocDevMode();
         if (!pnewDM) goto cleanup2_old;
#endif

			num = *(LPSHORT)lpbIn;	/* bin number to set to */

			fSave = !(num & 0x8000);/* high bit means temp change */

		      	num &= 0x7FFF;	/* keep low bits */

			DBMSG((" Control():>GETSETPAPERBINS SET bin = %d Save = %d\n", num, fSave));

			/* make sure bin # exists before setting */
#if 0	// fix bug #15167
	// 'num' is the index to the bin list. It's NOT the bin id (per AmiPro)
			for (i = 0; i < num_bins; i++) {

				if (num == bin[i]) {
					lpdv->iPaperSource = num;
					DBMSG(("bin found and set\n"));
					break;
				}
			}
#else
			if (num < num_bins)
			    lpdv->iPaperSource = bin[num];
			// otherwise, do not change the current bin.
#endif

#if 0	// fix bug #15167 (part 2)
	// do NOT update either the Windows environment or WIN.INI profile.
	// Besides, 'fSave' is misunderstood here: it should mean that the
	// bin change takes effect immediately. In fact, apps always sets
	// this bit.
			MakeEnvironment(lpdv->szDevType, lpdv->szFile, poldDM, NULL);
			*pnewDM = *poldDM;
			pnewDM->dm.dmDefaultSource = lpdv->iPaperSource;

			DBMSG((" GETSETPAPERBINS dmDefaultSource %d\n", pnewDM->dm.dmDefaultSource));

			SaveEnvironment(lpdv->szDevType, lpdv->szFile, pnewDM,
				poldDM, NULL, fSave, fSave);
#endif

			// to make sure the paper bin selection ends up outside
			// the first SS we set this flag.  this should
			// solve problems with postscript code that
			// selects the input tray AND resets the graphics
			// state (our graphics state setup comes just
			// after the initial SS command)

			fSaveDirty = lpdv->fDirty;
			lpdv->fDirty = TRUE;

			DumpPSSSafe(lpdv, lpdv->iPrinter, PSS_INPUTSLOT, 
				lpdv->iPaperSource - DMBIN_FIRST);
			lpdv->fDirty = fSaveDirty;

// fix bug #15167 (part 2)
// !!!! Lin or Peter: does the above code commands the printer to change to
//		      the new bin? I'm not sure. Please check out.

#if 0	// fix bug #15167 (part 2)

         FreeDevMode(pnewDM);
cleanup2_old:
         FreeDevMode(poldDM);
cleanup2_none:
	 ;

#endif
		}

		FreePrinter(pPrinter);

		}
		return TRUE;
		break;

	case ENUMPAPERBINS:
		{
		short	binList[NUMFEEDS];
		short	numOfBins;
		short	offset;/* for indexing into the array of bins names */
		short	i;
		short	numOfElements;
		LPSHORT lpBinList;
		char	paperBin[BINSTRLEN];
		PPRINTER pPrinter;

		if (!lpbIn || !lpbOut)
			return FALSE;

		if (!(pPrinter = GetPrinter(lpdv->iPrinter)))
			return -1;

		/* generate the list of bins supported by this printer */
		numOfBins = MakeBinList(pPrinter, binList);

		/* get the number of elements in the ouput data structure */

		numOfElements = *(LPSHORT)lpbIn;
		numOfBins = min(numOfBins, numOfElements);

		DBMSG((" Control(): ENUMPAPERBINS numOfElements=%d\n", numOfElements));

		/* get 1st array element */
		lpBinList = (LPSHORT)lpbOut;

		/* offset to 1st paper name position */
		offset = numOfElements  * sizeof(short);

		for (i = 0; i < numOfBins; i++) {
			/* add the bin number to the list */
#if 0	// fix bug #15167 (2/21/92)
			lpBinList[i] = binList[i];
#else
			lpBinList[i] = i;
#endif
			/* add the bin string to the list */
			LoadString(ghInst, binList[i] + DMBIN_BASE, paperBin, BINSTRLEN);
			lstrcpy(lpbOut + offset, paperBin);

			DBMSG((" ENUMPAPERBINS: lpBinList[%d]=%d", i, lpBinList[i]));
			DBMSG((" lpbOut + %d=%ls\n", offset, lpbOut + offset));

			offset += BINSTRLEN;
		}

		FreePrinter(pPrinter);

		return TRUE;
		}
		break;

        case GETSETSCREENPARAMS:
        {
                int freq, angle;
                
                /* if user wants old parameters keep em */
                if (lpbOut) {
                        freq = lpdv->freq;
                        angle = lpdv->angle;
                }

                /* if user wants to change parameters do it */
                if (lpbIn) {
                    lpdv->freq = ((SCREENPARAMS FAR *)lpbIn)->frequency;
                    lpdv->angle = ((SCREENPARAMS FAR *)lpbIn)->angle;
                }

                /* fill in output structure if given */
                if (lpbOut) {
                    ((SCREENPARAMS FAR *)lpbOut)->angle = angle;
                    ((SCREENPARAMS FAR *)lpbOut)->frequency = freq;
                }
        }
                break;

        case RESETDC:
                return DoResetDC(lpdv, (LPDV)lpbIn);

	default:
		return FALSE;
		break;
	}

	DBMSG2(("<Control(): Normal exit\n"));

	return TRUE;
}



/*
 * #ifdef OLIVETTI_CHARSET
 */

/*
 * OlivettiPrinter()
 *
 * msd - 10/26/88:  
 * special case hack.  check to see if this is an Olivetti LP 5000
 * to support it's unique charaterset.  note, Olivetti does not
 * support this printer anymore.
 */

BOOL FAR PASCAL OlivettiPrinter(lpdv)
LPDV lpdv;
{
	PPRINTER pPrinter;
	char buf[60];
	BOOL res = FALSE;

	if (pPrinter = GetPrinter(lpdv->iPrinter)) {

		LoadString(ghInst, IDS_OLIV, buf, sizeof(buf));

		res = !lstrcmpi(pPrinter->Name, buf);

		FreePrinter(pPrinter);
	}

	return res;
}

/*
 * WriteResourceComments()
 *
 * Output the resource summary comments.  This includes all proc sets and 
 * fonts used / downloaded in the print job.
 *
 */
void FAR PASCAL WriteResourceComments(LPDV lpdv)
{
    char szBuf[80];
    int i;
    BOOL f;

    /* print the decoding table for the font names */
    if (lpdv->cTTFontList) {
        PrintChannel(lpdv, "%% TrueType font name key:\n");
        for (i = 0; i < lpdv->cTTFontList; ++i) {
           GlobalGetAtomName(lpdv->lpTTFontList[i], szBuf, sizeof(szBuf));
           PrintChannel(lpdv, "%%    MSTT31%04x = %s\n", lpdv->lpTTFontList[i],
                        (LPSTR)szBuf);
        }
    }

    PrintChannel(lpdv, "%%%%DocumentSuppliedResources: ");

    f = EnumStrings(lpdv->slSupplied, szBuf, 80);

    if (lpdv->fHeader) {
        PrintChannel(lpdv, "procset " WINDICT " 3 1\n");
        if (f)
            PrintChannel(lpdv, "%%%%+ ");
    }
    
    if (f) {
        PrintChannel(lpdv, "font %s\n", (LPSTR) szBuf);
    
        while (EnumStrings(NULL, szBuf, 80)) {
            PrintChannel(lpdv, "%%%%+ font %s\n", (LPSTR) szBuf);
        }
    }

    PrintChannel(lpdv, "\n%%%%DocumentNeededResources: ");

    f = EnumStrings(lpdv->slNeeded, szBuf, 80);

    if (!lpdv->fHeader) {
        PrintChannel(lpdv, "procset " WINDICT " 3 1\n");
        if (f)
            PrintChannel(lpdv, "%%%%+ ");
    }

    if (f) {
        PrintChannel(lpdv, "font %s\n", (LPSTR) szBuf);

        while (EnumStrings(NULL, szBuf, 80))
            PrintChannel(lpdv, "%%%%+ font %s\n", (LPSTR) szBuf);
    }

    PrintChannel(lpdv, newline);
}


BOOL NEAR PASCAL DoResetDC(LPDV lpNewDV, LPDV lpOldDV)
{
    int i, newAtomCount;
    unsigned u;
    LPWORD  newAtomList;

    /* make sure this is our PDEVICE */
 	 if(lpOldDV->iType != 0x5750)
 	     return(FALSE);

    newAtomList = lpNewDV->lpTTFontList;
    newAtomCount = lpNewDV->cTTFontList;

    /*  disable will now inadvertantly destroy the default
        atom list for the new lpdv while the old atom table
        is preserved for use with the new lpdv.  */

    /* Copy common fields */

    lpNewDV->cTTFontList = lpOldDV->cTTFontList;
    lpNewDV->lpTTFontList = lpOldDV->lpTTFontList;
    lpNewDV->fh = lpOldDV->fh;
    lpNewDV->hdc = lpOldDV->hdc;
    lpNewDV->br = lpOldDV->br;
    lpNewDV->pen = lpOldDV->pen;
    lpNewDV->iCurLineJoin = lpOldDV->iCurLineJoin;
    lpNewDV->iCurLineCap = lpOldDV->iCurLineCap;
    lpNewDV->iCurMiterLimit = lpOldDV->iCurMiterLimit;
    lpNewDV->iNewLineJoin = lpOldDV->iNewLineJoin;
    lpNewDV->iNewLineCap = lpOldDV->iNewLineCap;
    lpNewDV->iNewMiterLimit = lpOldDV->iNewMiterLimit;
    lpNewDV->lid = lpOldDV->lid;
    lpNewDV->lidFont = lpOldDV->lidFont;
    lpNewDV->fPenSelected = lpOldDV->fPenSelected;
    lpNewDV->GraphData = lpOldDV->GraphData;
    lpNewDV->DLState = lpOldDV->DLState;
    lpNewDV->HatchColor = lpOldDV->HatchColor;
    lpNewDV->FillColor = lpOldDV->FillColor;
    lpNewDV->FillMode = lpOldDV->FillMode;
    lpNewDV->PolyMode = lpOldDV->PolyMode;
    lpNewDV->PathLevel = lpOldDV->PathLevel;
    lpNewDV->nextSlot = lpOldDV->nextSlot;
    lpNewDV->dx = lpOldDV->dx;
    lpNewDV->dy = lpOldDV->dy;
    lpNewDV->ScaleMode = lpOldDV->ScaleMode;
//    lpNewDV->angle = lpOldDV->angle;
//    lpNewDV->freq = lpOldDV->freq;
    lpNewDV->dwMaxVM = lpOldDV->dwMaxVM;
    lpNewDV->dwCurVM = lpOldDV->dwCurVM;
    lpNewDV->iPageNumber = lpOldDV->iPageNumber;

    /* copy external storage */
    lpNewDV->slSupplied = lpOldDV->slSupplied;
    lpNewDV->slNeeded = lpOldDV->slNeeded;
    lpNewDV->slPageResources = lpOldDV->slPageResources;
    lpNewDV->slTTFonts = lpOldDV->slTTFonts;
    for (i = 0; i < lpOldDV->nextSlot; ++i)
        lpNewDV->slotsArray[i] = lpOldDV->slotsArray[i];

    /* blank out string fields so they don't get deleted 
     * NOTE: This is not currently necessary since string lists are deleted
     * at ENDDOC time but is here for future use.
     */
    lpOldDV->slSupplied         =
    lpOldDV->slNeeded           =
    lpOldDV->slPageResources    =
    lpOldDV->slTTFonts          = NULL;

    /*  this causes the default atom list to be destroyed */
        
    lpOldDV->cTTFontList = newAtomCount;
    lpOldDV->lpTTFontList = newAtomList;

    /* copy the spool buffer */
    lpNewDV->cbSpool = lpOldDV->cbSpool;
    lpNewDV->cbSpoolMax = lpOldDV->cbSpoolMax;
    for (u = 0; u < lpOldDV->cbSpool; ++u)
        lpNewDV->rgbSpool[u] = lpOldDV->rgbSpool[u];
    lpNewDV->fDirty = lpOldDV->fDirty;

    return (ResetDev(lpNewDV, lpOldDV));
}

/*********************************************************************
* Name: ResetDev()
*
* Action: This routine is called to reinitialize the output
*	  device during a resetDC.
* 
*
*  created Feb 22 92: peterwo 
*  
**********************************************************************/

BOOL NEAR PASCAL ResetDev(lpdvNew, lpdvOld)
LPDV lpdvNew, lpdvOld;	    /* Far ptr to the device descriptor */
{
	int	        iPrinter,   i;
	PPRINTER    pPrinter;
	char	    buf[80];


	if (!(pPrinter = GetPrinter(lpdvNew->iPrinter)))
		return FALSE;

    // Fail call if app makes unreasonable changes

    if(lpdvNew->fDoEps != lpdvOld->fDoEps)
		return FALSE;
    if(lpdvNew->bDSC != lpdvOld->bDSC)
		return FALSE;
    if(lpdvNew->iRealRes != lpdvOld->iRealRes)
		return FALSE;   // this may not be allowed on a per page basis


	iPrinter = lpdvNew->iPrinter;


    PrintChannel(lpdvNew, "%%%%BeginPageSetup\n");
//  PrintChannel(lpdvNew, WINDICT " begin\n");

	if (!lpdvNew->fDoEps) 
    {

		// set the input slot.  this stuff comes from the
		// *InputSlot line in the PPD file

        if(lpdvNew->iPaperSource != lpdvOld->iPaperSource)
    		DumpPSSSafe(lpdvNew, iPrinter, PSS_INPUTSLOT, 
                lpdvNew->iPaperSource - DMBIN_FIRST);


		/* only try to send a PostScript image/tray setup if
		 * the paper is available to the printer */

		if ((lpdvNew->paper.iPaper != lpdvOld->paper.iPaper) &&
            PaperSupported(pPrinter, &lpdvNew->DevMode, lpdvNew->paper.iPaper))
        {
			/* search here to get the index of the PSS table entry
		 	* that matches the index of the Paper[] array for the
		 	* current printer */
		 
            i = FindPaperType(pPrinter, &lpdvNew->DevMode, lpdvNew->paper.iPaper, NULL);
            if (i < 0) 
                return (FALSE);

            if (lpdvNew->iPaperSource == DMBIN_AUTO &&
                pPrinter->feed[DMBIN_AUTO - DMBIN_FIRST]) 
            {
                  PrintChannel(lpdvNew, "{");
                  DumpPSS(lpdvNew, iPrinter, PSS_NORMALIMAGE, i);
                  PrintChannel(lpdvNew, "}stopped\n{");
                  DumpPSS(lpdvNew, iPrinter, PSS_MANUALIMAGE, i);
                  PrintChannel(lpdvNew, "}if\n");
            } else {

			      // the PSS_MANUALIMAGE coorisponds with the
			      // *PageRegion PPD file command.

			      DumpPSSSafe(lpdvNew, iPrinter, PSS_MANUALIMAGE, i);
            }
		}

		// set duplex mode
		if (lpdvNew->iDuplex != lpdvOld->iDuplex  &&
            IS_DUPLEX(pPrinter) && lpdvNew->iDuplex != -1) 
        {
			DumpPSS(lpdvNew, lpdvNew->iPrinter, PSS_DUPLEX, lpdvNew->iDuplex);
		}

        // output resolution control sequence 
        // (does nothing if setresolution doesn't exist)
        //  No! you are not allowed to change the resolution
        //  in the middle of a job.
	}

    // output screen parameters IF they are not the default AND we are
    // NOT generating DSC output
    if (!lpdvNew->bDSC &&   (lpdvNew->angle != lpdvOld->angle
        || lpdvNew->freq != lpdvOld->freq)) 
    {
	    LoadString(ghInst, IDS_SETSCREEN, buf, sizeof(buf));
	    PrintChannel(lpdvNew, buf, lpdvNew->freq, lpdvNew->angle);
    }

    // if negative image requested invert current transfer function
    if (lpdvNew->bNegImage != lpdvOld->bNegImage) 
    {
	    LoadString(ghInst, IDS_NEGIMAGE, buf, sizeof(buf));
	    PrintChannel(lpdvNew, buf);
    }

    // if per page downloading requested set or clear the flag
    if (lpdvNew->bPerPage != lpdvOld->bPerPage)
    {
        if(lpdvNew->bPerPage == TRUE)
            PrintChannel(lpdvNew, "/fPP true def\n");
        else
            PrintChannel(lpdvNew, "/fPP false def\n");

    }

    PrintChannel(lpdvNew, "%%%%EndPageSetup\n");
	FreePrinter(pPrinter);
    return(TRUE);
}



