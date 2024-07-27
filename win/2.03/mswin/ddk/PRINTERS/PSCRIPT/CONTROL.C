/*********************************************************************
 * CONTROL.C
 *
 * 87-02-11	sec		"letter" -> a4,b5 for apple printers
 * 9Mar87	sjp		IBM printer support
 * 7Apr87	sjp		added DataProducts LZR 2665 (1st cut)
 * 8Apr87	sjp		added DEC LN03R ScriptPrinter
 * 17Apr87	sjp		disabled SELECTPAPERSOURCE Escape,added PrintServer40
 *					paper selection.
 * 3Jun87	sjp		disabled ENABLERELATIVEWIDTHS escape.
 * 3Jun87	sjp		added GETSETPRINTERORIENTATION escape.
 * 4Jun87	sjp		added GETSETPAPERBINS escape, and escape numbers
 *					for above 2 escapes and ENABLEDUPLEX
 * 18Jun87	sjp		Added initialization of "spare" output data fields
 *					for GETSETPAPERBINS escape.
 * 20Jul87	sjp		Bug fix for Digital LPS40 (PRINTSERVER40).
 *********************************************************************
 */

#include "pscript.h"
#include "printers.h"

#define EOF	4	/* Control-D = end of file */

void FAR PASCAL StretchBlt(LPDV, LPSTR);
short FAR PASCAL GetEtm(LPDV, LPSTR, LPSTR);
short FAR PASCAL GetPairKern(LPDV, LPSTR, LPSTR);
BOOL PASCAL SetPaperSource(LPDV, int);
BOOL FAR PASCAL GetExtTable(LPDV, LPSTR, LPSTR);
void FAR PASCAL lstrcpy(LPSTR, LPSTR);
void FAR PASCAL MakeEnvironment(HINST,LPSTR,LPSTR,LPDEVMODE);
void FAR PASCAL SaveEnvironment(HINST,LPSTR,LPSTR,LPDEVMODE,short,short);


extern BOOL gl_dloaded;
char gBinToFeedMap[NUMBINS]={ 0, 1, 2, 1, 1 };


#define SETCOPYCOUNT		 17
#define SELECTPAPERSOURCE	 18
#define PASSTHROUGH		 19
#define GETTECHNOLOGY            20
#define SETLINECAP               21
#define SETLINEJOIN              22
#define SETMITERLIMIT            23

#define GETEXTENDEDTEXTMETRICS	256
#define GETEXTENTTABLE		257
#define GETPAIRKERNTABLE	258
#define GETTRACKKERNTABLE	259

#define EXTTEXTOUT		512

/* shut this escape off for now...
 * #define ENABLERELATIVEWIDTHS	768
 */
#define ENABLEPAIRKERNING	769
#define SETKERNTRACK		770
#define SETALLJUSTVALUES	771
#define SETCHARSET			772

/* added 4 June 1987 -- sjp */
#define ENABLEDUPLEX				28
#define GETSETPRINTERORIENTATION	29
#define ESCAPE_PORTRAIT 1
#define ESCAPE_LANDSCAPE 2

#define GETSETPAPERBINS				30

/* temporary !!! */
#define ENUMPAPERBINS	31

#define STRETCHBLT		2048

/****************************************************************************/
#ifdef DEBUG_ON
#define DBMSG(msg) printf msg
#else
#define DBMSG(msg)
#endif

#ifdef DEBUG1_ON
#define DBMSG1(msg) printf msg
#else
#define DBMSG1(msg)
#endif

/*#define DEBUG_CONTROL*/
#ifdef DEBUG_CONTROL
#define DBMSG2(msg) printf msg
#else
#define DBMSG2(msg)
#endif
/****************************************************************************/

#if 0 /* DISABLED */
/* The paper metrics structure for the SELECTPAPERSOURCE escape */
typedef struct
    {
    WORD x, y;
    RECT rcPage;
    WORD orientation;
    }PM;
typedef PM FAR * LPPM;
#endif

/* this structure is for the SETALLJUSTVALUES escape...
 * added by Aldus Corporation--19Jan87--sjp
 */
typedef struct {
	short nCharExtra;
	WORD nCharCount;
	short nBreakExtra;
	WORD nBreakCount;
	} ALLJUSTREC;
typedef ALLJUSTREC FAR * LPALLJUSTREC;



/* These next 2 typedefs are specially for
 * the GETSETPAPERBINS output data structures.
 * The 2nd one is only used for debug purposes
 * since the escape doesn't make assumptions on
 * the actual number of elements in the structure.
 */
typedef struct{
	short binNum;
	short numOfBins;
	short spare1, spare2, spare3, spare4;
} GSPBOD;
typedef GSPBOD FAR * LPGSPBOD;

#ifdef DEBUG_CONTROL
/* DEBUG DEBUG DEBUG DEBUG DEBUG DEBUG DEBUG DEBUG DEBUG DEBUG DEBUG */

#define MAXELEMENTS 10
typedef struct{
	short binList[MAXELEMENTS];
	char paperNames[MAXELEMENTS][BINSTRLEN];
} EPBOD;
typedef EPBOD FAR * LPEPBOD;

/* DEBUG DEBUG DEBUG DEBUG DEBUG DEBUG DEBUG DEBUG DEBUG DEBUG DEBUG */
#endif


/*********************************************************************
* Name: PaperRef()
*
* Action: This routine is called to refresh the paper-
*  handling characteristics for each output page.
*
**********************************************************************
*/
static BOOL PaperRef(lpdv)
DV FAR *lpdv;
{
	PRINTER thePrinter;

	DBMSG1(((LPSTR)">PaperRef()\r"));

	if(!GetPrinterCaps(lpdv->dh.hmod,lpdv->dh.iPrinter,
		(LPPRINTER)&thePrinter)
	){
		goto ERROR;
	}

	/* Is manual feed supported by this printer? ... if so then
	 * make sure that manual feed is turned off or on depending
	 * on the state of iPaperSource, i.e. if manual feed is selected.
	 * Assumes that "manualfeed" operator is in "statusdict".
	 */
	if(thePrinter.feed[MANUALFEED-IFEEDMIN]){

		PrintChannel(lpdv, (LPSTR) "statusdict begin /manualfeed ");

		/* is manual feed selected? */
		if(lpdv->dh.iPaperSource == (MANUALFEED-IFEEDMIN)){
			PrintChannel(lpdv, (LPSTR) "true");
		}else{
			PrintChannel(lpdv, (LPSTR) "false");
		}

		PrintChannel(lpdv, (LPSTR) " def end\n");
	}

	if ((lpdv->dh.iPrinter == QMSPS800) ||
		(lpdv->dh.iPrinter == APPLEPLUS) ||
		(lpdv->dh.iPrinter == APPLE))
		{	
		PrintChannel(lpdv, (LPSTR)"statusdict begin 0 setjobtimeout end\n");
		}

#if 0
	/* paper tray / manual feed
	 */
	switch(lpdv->dh.iPrinter){
		case APPLE:
		case APPLEPLUS:
			DBMSG1(((LPSTR)"APPLE"));
			if (lpdv->dh.iPaperSource < 2) {
				PrintChannel(lpdv, (LPSTR) "statusdict begin /manualfeed ");
				if (lpdv->dh.iPaperSource == 0)
					PrintChannel(lpdv, (LPSTR) "true");
				else if (lpdv->dh.iPaperSource == 1)
					PrintChannel(lpdv, (LPSTR) "false");
				PrintChannel(lpdv, (LPSTR) " def end\n");
			}
		break;

      case OMNILASER2115:
      case PRINTSERVER40:
      case OMNILASER2108:
            if ((lpdv->dh.iPaperSource > 0) && (lpdv->dh.iPaperSource < 3)){
                PrintChannel(lpdv, 
				 (LPSTR) "statusdict begin %d setpapertray end\n",
				 lpdv->dh.iPaperSource - 1);
			}
		break;

		case DPLZR2665:
			DBMSG1(((LPSTR)"DPLZR2665:  iPS=%d\r",lpdv->dh.iPaperSource));
			DBMSG1(((LPSTR)"DPLZR2665:  p.iP=%d\r",lpdv->dh.paper.iPaper));
			DBMSG1(((LPSTR)"DPLZR2665:  fL=%d\r",lpdv->dh.fLandscape));
            switch(lpdv->dh.iPaperSource){
				case 1:	/* upper tray */
				case 2:	/* lower tray */
				default:
					PrintChannel(lpdv,(LPSTR)
						"statusdict begin /manualfeed false def end\n");
				break;
				case 0:	/* manual */
					PrintChannel(lpdv,(LPSTR)
						"statusdict begin /manualfeed true def end\n");
				break;
			}
		break;

		case WANGLCS15:
		case WANGLCS15FP:
			DBMSG1(((LPSTR)"WANG"));
			/* setbin: 1 is lower tray, 2 is upper tray, 0 is either */
            if ((lpdv->dh.iPaperSource > 0) && (lpdv->dh.iPaperSource < 3)){
				int tray;
				switch (lpdv->dh.iPaperSource) {
				case 1:		/* driver's notion of upper tray */
					tray = 2;
					break;
				case 2:		/* driver's notion of lower tray */
					tray = 1;
					break;
				default:
					tray = 0;
					break;
				}
                PrintChannel(lpdv,
				 (LPSTR) "statusdict begin %d setbin end\n", tray);
	        }
        break;

		/* no paper refresh is possible...only one source OR the
		 * printer does not do manual feed
		 */
		case LN03R:
		case LINOTYPE:
		case IBM1:
		default:
			DBMSG1(((LPSTR)"No tray refresh needed.\r"));
		break;

	}
#endif

	/* # of copies
	 */
	PrintChannel(lpdv, (LPSTR) "userdict begin /#copies %d def end\n",
		lpdv->dh.iCopies);

	DBMSG1(((LPSTR)"<PaperRef()\r"));
	return(TRUE);

ERROR:
	DBMSG1(((LPSTR)"<PaperRef():  ERROR\r"));
	return(FALSE);
}

/*********************************************************************
* Name: InitDev()
*
* Action: This routine is called to initialize the output
*	  device when the printer is enabled.
*
**********************************************************************
*/
void PASCAL InitDev(lpdv, lszJob)
DV FAR *lpdv;	    /* Far ptr to the device descriptor */
LPSTR lszJob;	    /* Far ptr to the print job name */
    {
    HANDLE hres;
    int fh;
    short cbFile;
    int cbRead;
    int iPrinter;

    char rgb[128];

	DBMSG1(((LPSTR)"***InitDev()\r"));

    ASSERT(lpdv!=(LPSTR) (long) NULL);
    ASSERT(lpdv->dh.paper.iPaper >= IPAPERMIN &&
	 lpdv->dh.paper.iPaper <= IPAPERMAX);
    ASSERT(lpdv->dh.hmod!=(LPSTR) (long) NULL);


#ifdef DIGBY
	/* THIS STUFF MUST BE MODIFIED TO WORK IN THIS VERSION OF THE DRIVER */
    /* This code loads the PostScript header dirctly from an external file */
    /* instead of a resource so that Adobe could debug it */

    fh = _lopen((LPSTR) "header.ps", READ);
    if (fh > 0){
		/* Terminate any previous job before the new one */
		if(iPrinter!=PRINTSERVER40 && iPrinter!=IBM1) /* printer is invalid */
		    PrintChannel(lpdv, (LPSTR) "%c", EOF);

		if (lszJob!=(LPSTR) (long) NULL)
		    PrintChannel(lpdv, (LPSTR) "%% %s\n", lszJob);
		else
		    PrintChannel(lpdv, (LPSTR) "\n");

		/* Download the PostScript header */
		while (TRUE){
		    cbRead = _lread(fh, (LPSTR) rgb, sizeof(rgb));
		    if (cbRead > 0)
				WriteChannel(lpdv, (LPSTR) rgb, cbRead);
		    else
				break;
		}
		_lclose(fh);
	}else{
		return;
	}


#else
    if (!(hres = FindResource(lpdv->dh.hmod, (LPSTR)(long) 1,
		(LPSTR)(long)PSHEADER))
	){
PrintChannel(lpdv, (LPSTR) "Could not Find header.\n");
		return;
	}

    iPrinter = lpdv->dh.iPrinter;

    if ((fh = AccessResource(lpdv->dh.hmod, hres))>=0){
		/* Terminate any previous job before the new one */
		if (iPrinter!=PRINTSERVER40 && iPrinter!=IBM1)
		    PrintChannel(lpdv, (LPSTR) "%c", EOF);

		if (lszJob!=(LPSTR) (long) NULL)
		    PrintChannel(lpdv, (LPSTR) "%% %s\n", lszJob);
		else
		    PrintChannel(lpdv, (LPSTR) "\n");

		/* Download the PostScript header */
		DBMSG (((LPSTR)"gl_dloaded=%d\n", gl_dloaded));	/* DEBUG */
		if (gl_dloaded){
			PrintChannel(lpdv,(LPSTR)" /WinDict where\n");
			PrintChannel(lpdv,(LPSTR)"{pop}\n");
			PrintChannel(lpdv,(LPSTR)
				"{(PS Driver Header is missing) print flush\n");
			PrintChannel(lpdv,(LPSTR)
				" 100 100 moveto /Helvetica findfont 12 scalefont setfont\n");
			PrintChannel(lpdv,(LPSTR)
				"(PS Driver Header has not been downloaded) show\n");
			PrintChannel(lpdv,(LPSTR)"100 84 moveto\n");
			PrintChannel(lpdv,(LPSTR)
"(Please download psprep.txt, or change win.ini to: Header Downloaded=no)\n");
			PrintChannel(lpdv,(LPSTR) " show showpage stop}  ifelse\n\n\n");

			PrintChannel(lpdv, (LPSTR) "WinDict begin\n");
		}else{
			if (_lread(fh, (LPSTR) &cbFile, sizeof(cbFile))==sizeof(cbFile)){
		    	while (cbFile>0){
					cbRead = cbFile > sizeof(rgb) ? sizeof(rgb) : cbFile;
					_lread(fh, (LPSTR) rgb, cbRead);
					WriteChannel(lpdv, (LPSTR) rgb, cbRead);
					cbFile -= cbRead;
				}
		    }
		}
		_lclose(fh);
	}else{
PrintChannel(lpdv, (LPSTR) "Could not access header.\n");
		return;
	}

#endif

    /* Select the paper type */
	switch(iPrinter){
		case APPLE:
		case APPLEPLUS:
		case IBM1:
		case QMSPS800:
		default:
			DBMSG1(((LPSTR)"APPLE\r"));
			PrintChannel(lpdv,(LPSTR)"statusdict begin ");

			switch(lpdv->dh.paper.iPaper){
				case LETTER:
				default:
					PrintChannel (lpdv, (LPSTR) "letter");
				break;
				case LEGAL:
					PrintChannel(lpdv, (LPSTR) "legal");
				break;
				case STATEMENT:
					PrintChannel (lpdv,(LPSTR)
						"/statement where {pop statement} {letter} ifelse");
				break;
				case DINA4:
					PrintChannel (lpdv,
						(LPSTR) "/a4 where {pop a4} {letter} ifelse");
				break;
				case DINA5:
					PrintChannel (lpdv,
						(LPSTR) "/a5 where {pop a5} {letter} ifelse");
				break;
				case DINB5:
					PrintChannel (lpdv,
						(LPSTR) "/b5 where {pop b5} {letter} ifelse");
				break;
			}
			PrintChannel(lpdv,(LPSTR)" end\n");
		break;

		case LINOTYPE:
			DBMSG1(((LPSTR)"LINOTYPE\r"));
			PrintChannel(lpdv,(LPSTR)"statusdict begin ");

			switch(lpdv->dh.paper.iPaper){
				case LETTER:
				default:
					PrintChannel (lpdv, (LPSTR) "letter");
				break;
				case LEGAL:
					PrintChannel(lpdv, (LPSTR) "legal");
				break;
				case TABLOID:	/* 11" x 17" */
					PrintChannel(lpdv,(LPSTR)"792 1224 0 1 setpageparams");
				break;
				case STATEMENT:	/* 5.5" x 8.5" */
					PrintChannel(lpdv,(LPSTR)"396 612 0 1 setpageparams");
				break;
				case DINA3:	/* 297mm x 420mm */
					PrintChannel(lpdv,(LPSTR) "842 1191 0 1 setpageparams");
				break;
				case DINA4:	/* 210mm x 297mm */
					PrintChannel(lpdv,(LPSTR) "595 842 0 1 setpageparams");
				break;
				case DINA5:	/* 136.65mm x 210mm */
					PrintChannel(lpdv,(LPSTR) "387 595 0 1 setpageparams");
				break;
				case DINB4:	/* 9.84" x 13.9" -- should be in mm */
					PrintChannel(lpdv,(LPSTR) "708 1001 0 1 setpageparams");
				break;
				case DINB5:	/* 6.93" x 9.84" -- should be in mm */
					PrintChannel(lpdv,(LPSTR) "499 708 0 1 setpageparams");
				break;
			}
			PrintChannel(lpdv,(LPSTR)" end\n");
		break;

		case PRINTSERVER40:
		case DPLZR2665:
		{
			char p[7];	/* holds paper type */

			DBMSG1(((LPSTR)"DPLZR2665:  %d\r",lpdv->dh.paper.iPaper));
			DBMSG1(((LPSTR)"DPLZR2665:  %d\r",lpdv->dh.fLandscape));
			switch(lpdv->dh.paper.iPaper) {
				case LETTER:
				default:
					lstrcpy((LPSTR)p,(LPSTR)"letter");
				break;
				case LEGAL:
					lstrcpy((LPSTR)p,(LPSTR)"legal");
				break;
				case TABLOID:
					lstrcpy((LPSTR)p,(LPSTR)"11x17");
				break;
				case STATEMENT:
					lstrcpy((LPSTR)p,(LPSTR)"statement");
				break;
				case DINA3:
					lstrcpy((LPSTR)p,(LPSTR)"a3");
				break;
				case DINA4:
					lstrcpy((LPSTR)p,(LPSTR)"a4");
				break;
				case DINA5:
					lstrcpy((LPSTR)p,(LPSTR)"a5");
				break;
				case DINB4:
					lstrcpy((LPSTR)p,(LPSTR)"b4");
				break;
				case DINB5:
					lstrcpy((LPSTR)p,(LPSTR)"b5");
				break;
			}
			/* only do this if NOT manual feed
			 */
			if(lpdv->dh.iPaperSource){
				DBMSG1(((LPSTR)"NOT manual feed\r"));
				PrintChannel(lpdv,(LPSTR)
					"/Helvetica findfont 12 scalefont setfont\n");
				PrintChannel(lpdv,(LPSTR)
					"statusdict begin { %stray } stopped {\n",(LPSTR)p);
				PrintChannel(lpdv,(LPSTR)"  72 100 moveto\n");
				PrintChannel(lpdv,(LPSTR)
					"  (ERROR in Windows print job.) show\n");
				PrintChannel(lpdv,(LPSTR)"  72 84 moveto\n");
				PrintChannel(lpdv,(LPSTR)
			"  (You selected %s paper, but the %s tray is NOT installed.)",
					(LPSTR)p,(LPSTR)p);
				PrintChannel(lpdv,(LPSTR)"  show showpage quit\n} if\n");
				PrintChannel(lpdv,(LPSTR)"end\n");
			}
			if(iPrinter!=PRINTSERVER40){
				PrintChannel(lpdv,(LPSTR)"userdict begin %s end\n",(LPSTR)p);
			}
		}
		break;

		case WANGLCS15:
		case WANGLCS15FP:
		{
			char p[7];	/* holds paper type */

			DBMSG1(((LPSTR)"WANG:  %d\r",lpdv->dh.paper.iPaper));
			DBMSG1(((LPSTR)"WANG:  %d\r",lpdv->dh.fLandscape));
			switch(lpdv->dh.paper.iPaper) {
				case LETTER:
				default:
					lstrcpy((LPSTR)p,(LPSTR)"letter");
				break;
				case LEGAL:
					lstrcpy((LPSTR)p,(LPSTR)"legal");
				break;
				case DINA4:
					lstrcpy((LPSTR)p,(LPSTR)"a4");
				break;
			}
			/* set up the printer to operate in auto tray select mode */
			PrintChannel(lpdv,(LPSTR)"statusdict begin 0 setbin end\n");

			/* request the particular paper type */
			PrintChannel(lpdv,(LPSTR)"userdict begin %s end\n",(LPSTR)p);
		}
		break;

		/* no paper select is possible...uses switch */
		case LN03R:
			DBMSG1(((LPSTR)"LN03R\r"));
		break;
	}


    PrintChannel(lpdv, (LPSTR) "%d %d %d %d %d %d SetMetrics\n",
		lpdv->dh.paper.cxMargin,
		lpdv->dh.paper.cyMargin,
        lpdv->dh.paper.cxPaper,
        lpdv->dh.paper.cyPaper,
        lpdv->dh.iRes,
        lpdv->dh.fLandscape
	);

#if 0	/* disabled by sec, 87-1-13, due to apparent obsolescence */
    PrintChannel(lpdv, (LPSTR)
		"userdict begin /#copies %d def end\n", lpdv->dh.iCopies);
    SetPaperSource(lpdv, lpdv->dh.iPaperSource);
    PrintChannel(lpdv, (LPSTR) "/svPat save def\n");
#endif

	DBMSG1(((LPSTR)"InitDev()***\r"));
}


/*********************************************************************
* Name: TermDev()
*
* Action: This routine is called at the end of a print job.
*
**********************************************************************
*/
void PASCAL TermDev(lpdv)
DV FAR *lpdv;
{
	DBMSG1(((LPSTR)"***TermDev()\r"));
    ASSERT(lpdv!= (LPSTR) (long) NULL);

    PrintChannel(lpdv, (LPSTR) "end\n");

/*  This is NOT necessary ... 9Apr87--sjp
 *	if (lpdv->dh.iPaperSource > 0) SetPaperSource(lpdv, 0);
 */

    if (lpdv->dh.iPrinter!=PRINTSERVER40 && lpdv->dh.iPrinter!=IBM1){
		PrintChannel(lpdv, (LPSTR) "%c", EOF);
	}
	DBMSG1(((LPSTR)"TermDev()***\r"));
}


#if 0
/*********************************************************************
* Name: SetPaperSource()
*
* Action: This function selects the paper source.  Note that
*	  it is printer specific.
*
**********************************************************************
*/
BOOL PASCAL SetPaperSource(lpdv, iPaperSource)
LPDV lpdv;
int iPaperSource;
{
    BOOL fAvailable=FALSE;

	DBMSG1(((LPSTR)"***SetPaperSource()\r"));

    switch(lpdv->dh.iPrinter){
		case APPLE:
		case APPLEPLUS:
		    if (iPaperSource < 2){
				lpdv->dh.iPaperSource = iPaperSource;
				fAvailable = TRUE;

				/* Just save paper source number if file is not open yet */
				if (lpdv->dh.fh == -1)
				    break;
				PrintChannel(lpdv, (LPSTR) "statusdict begin /manualfeed ");

				if (iPaperSource == 0){
					PrintChannel(lpdv, (LPSTR) "true ");
				}else if (iPaperSource == 1){
					PrintChannel(lpdv,(LPSTR) "false ");
				}

				PrintChannel(lpdv, (LPSTR) "def end\n");
			}
		break;
        case OMNILASER2115:
        case PRINTSERVER40:
		case OMNILASER2108:
            if ((iPaperSource > 0) && (iPaperSource < 3)){
                PrintChannel(lpdv, (LPSTR) 
				  "statusdict begin %d setpapertray end\n", iPaperSource - 1);
                fAvailable = TRUE;
            }
        break;
		case WANGLCS15:
		case WANGLCS15FP:
			/* setbin: 1 is lower tray, 2 is upper tray, 0 is either */
            if ((iPaperSource > 0) && (iPaperSource < 3)){
				int tray;
				switch (iPaperSource) {
					case 1:		/* driver's notion of upper tray */
						tray = 2;
					break;
					case 2:		/* driver's notion of lower tray */
						tray = 1;
					break;
					default:
						tray = 0;
					break;
				}
                PrintChannel(lpdv,
				 (LPSTR) "statusdict begin %d setbin end\n", tray);

                fAvailable = TRUE;
            }
        break;
		case DPLZR2665:
		case LN03R:
		case IBM1:
		case LINOTYPE:
			/* Not exactly sure what to do with this routine... is it needed
			 * at all?  Just to be sure will do the bare minimum.
			 */
			fAvailable = TRUE;
		break;
	}
	DBMSG1(((LPSTR)"SetPaperSource()***\r"));
    return(fAvailable);
}
#endif


/*********************************************************************
 * CalcBreaks
 * Aldus Corporation--19January87
 *********************************************************************
 */
static void CalcBreaks (lpJustBreak, BreakExtra, Count)
	LPJUSTBREAKREC lpJustBreak;
	short BreakExtra;
	WORD Count;
{
	DBMSG(((LPSTR)"***CalcBreaks(): Count=%d,BreakExtra=%d\r",
		Count,BreakExtra));
	if (Count > 0){
		/*	Fill in JustBreak values.  May be positive or negative.
		 */
		lpJustBreak->extra = BreakExtra / (short)Count;
		lpJustBreak->rem = BreakExtra % (short)Count;
		lpJustBreak->err = (short)Count / 2 + 1;
		lpJustBreak->count = Count;
		lpJustBreak->ccount = 0;
		DBMSG(((LPSTR)">0 e=%d,r=%d,e=%d,c=%d,cc=%d\r",
			lpJustBreak->extra,lpJustBreak->rem,lpJustBreak->err,
			lpJustBreak->count,lpJustBreak->ccount));

		/*	Negative justification:  invert rem so the justification algorithm
		 *	works properly.
		 */
		if (lpJustBreak->rem < 0){
			--lpJustBreak->extra;
			lpJustBreak->rem += (short)Count;
			DBMSG(((LPSTR)"Neg.Just. e=%d,r=%d\r",
				lpJustBreak->extra,lpJustBreak->rem));
		}
	}else{
		/*	Count = zero, set up justification rec so the algorithm
		 *	always returns zero adjustment.
		 */
		lpJustBreak->extra = 0;
		lpJustBreak->rem = 0;
		lpJustBreak->err = 1;
		lpJustBreak->count = 0;
		lpJustBreak->ccount = 0;
		DBMSG(((LPSTR)"=0 e=%d,r=%d,e=%d,c=%d,cc=%d\r",
			lpJustBreak->extra,lpJustBreak->rem,lpJustBreak->err,
			lpJustBreak->count,lpJustBreak->ccount));
	}
}



/*********************************************************************/
/* Since there are NUMBINS=5 bins and NUMFEEDS=3 feeds, this routine
 * provides the means to produce a list of bin numbers that a given
 * printer supports based 2 items in gPrCaps:
 *		1) the list of feeds it supports, and
 *		2) the condition of the "special" field--see PRINTERS.H
 */
void MakeBinList(LPPRINTER,LPSHORT);
void MakeBinList(lpPrCaps,lpBinList)
	LPPRINTER lpPrCaps;
	LPSHORT lpBinList;
{
	short tempBin;
	short i;
	short j=0;

	DBMSG2(((LPSTR)">MakeBinList()\r"));
	for(i=0;i<NUMFEEDS;i++){

		if(lpPrCaps->feed[i]){
			DBMSG2(((LPSTR)" MakeBinList(): i=%d, j=%d,feed=%d\r",
				i,j,lpPrCaps->feed[i]));

			/* check to see if this is the upper tray
			 * since this dialog position can have more
			 * than one possible tray type
			 */
			if(i==1){
				/* upper tray */
				DBMSG2(((LPSTR)" MakeBinList(): UPPERTRAY,special=%d\r",
					lpPrCaps->special));
				switch(lpPrCaps->special){
					case IDS_UPPER:
					default:
						tempBin=1;
					break;

					case IDS_AUTO:
						tempBin=3;
					break;

					case IDS_CASSETTE:
						tempBin=4;
					break;
				}
				DBMSG2(((LPSTR)" MakeBinList(): tB=%d\r",tempBin));

			}else{
				/* NOT upper tray */
				DBMSG2(((LPSTR)" MakeBinList(): NOT UPPERTRAY\r"));
				tempBin=i;
				DBMSG2(((LPSTR)" MakeBinList(): tB=%d\r",tempBin));
			}
			/* add the bin number to the list */
			*(lpBinList+j)=tempBin;
			DBMSG2(((LPSTR)" MakeBinList(): bL[%d]=%d\r",j,*(lpBinList+j)));

			/* get ready for the next one */
			j++;
		}
	}
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
 *********************************************************************
 */
int FAR PASCAL Control(lpdv, ifn, lpbIn, lpbOut)
DV FAR *lpdv;	    /* Far ptr to the device descriptor */
int ifn;	    /* The escape function to execute */
LPSTR lpbIn;	    /* Far ptr to input data for the escape function */
LPSTR lpbOut;	    /* Far ptr to the output buffer for the esc function*/
    {
    register int iResult;
    short cb;
    short i;

	DBMSG2(((LPSTR)">Control():  escFunc=%d\r",ifn));
    ASSERT(lpdv!=(LPSTR) (long) NULL);

    switch(ifn)
	{
	case NEXTBAND:
	    SetRectEmpty((LPRECT) lpbOut);
	    if (lpdv->dh.fh>=0){
			if (lpdv->dh.iBand==0){
			    ((LPRECT) lpbOut)->bottom = lpdv->dh.paper.cyPaper;
			    ((LPRECT) lpbOut)->right = lpdv->dh.paper.cxPaper;
			    lpdv->dh.iBand = 1;
		    }else{
			    lpdv->dh.iBand = 0;
			    Control(lpdv, NEWFRAME, 0L, 0L);
		    }
			iResult = 1;
		}else iResult = -1;
    break;

	case NEWFRAME:
	    PrintChannel(lpdv, (LPSTR) "eject\n");
		/* 87-1-13 sec: */
	    iResult = lpdv->dh.fh > 0 ? 1 : -1;
		/* 87-1-13 sec: */
		lpdv->dh.br.lid = -1L;
		lpdv->dh.pen.lid = -1L;
		lpdv->dh.lidFont = -1L;
	    PrintChannel(lpdv, (LPSTR) "RestoreState\n\nSaveState\n");

		/* refresh paper source */
		if(!PaperRef(lpdv)) iResult=-1;

		lpdv->dh.fDirty = FALSE;
    break;

	case ABORTDOC:
	    TermDev(lpdv);
	    CloseChannel(lpdv);
	    iResult = TRUE;
    break;

	case QUERYESCSUPPORT:
	    switch(*(short far *)lpbIn){
			case NEXTBAND:
			case NEWFRAME:
			case ABORTDOC:
			case QUERYESCSUPPORT:
			case FLUSHOUTPUT:
			case STARTDOC:
			case ENDDOC:
			case GETPRINTINGOFFSET:
			case GETPHYSPAGESIZE:
			case SETCOPYCOUNT:
			/*	case SELECTPAPERSOURCE: *** disabled 17Apr87--sjp */
			case PASSTHROUGH:
			case GETEXTENDEDTEXTMETRICS:
			case GETEXTENTTABLE:
			case GETPAIRKERNTABLE:

		/* shut this escape off for now...
		 *	case ENABLERELATIVEWIDTHS:
		 */
			case STRETCHBLT:
			case SETKERNTRACK:
			case SETCHARSET:

			/* added by Aldus Corporation--19Jan87--sjp */
			case SETALLJUSTVALUES:

			case ENABLEPAIRKERNING:
	        case GETTECHNOLOGY:
	        case SETLINECAP:
	        case SETLINEJOIN:
	        case SETMITERLIMIT:

			case GETSETPRINTERORIENTATION:
			case GETSETPAPERBINS:
			    return(TRUE);
			break;

			case GETSCALINGFACTOR:

			/* disabled 17Apr87--sjp */
			case SELECTPAPERSOURCE:
			default:
			    return(FALSE);
			break;
		}
    break;

	case GETEXTENTTABLE:
        return(GetExtTable(lpdv, lpbIn, lpbOut));
    break;

/* shut this escape off for now...
 *	case ENABLERELATIVEWIDTHS:
 *	    if (lpbIn) lpdv->dh.fIntWidths = !*lpbIn;
 *	break;
 */
	case GETEXTENDEDTEXTMETRICS:
	    return(GetEtm(lpdv, lpbIn, lpbOut));
	break;

    case GETPAIRKERNTABLE:
	    return(GetPairKern(lpdv, lpbIn, lpbOut));
	break;

	case FLUSHOUTPUT:
	    FlushChannel(lpdv);
    break;

	case STARTDOC:
		DBMSG2(((LPSTR)" Control(): STARTDOC\r"));

	    iResult = 1;

	    /* Do an explicit ENDDOC if the user forgot to do it */
	    if (lpdv->dh.fh>=0) Control(lpdv, ENDDOC, 0L, 0L);

	    /* Open the output channel: note lpbIn is the document name*/
	    if (OpenChannel(lpdv, lpbIn)<0) iResult = -1;

	    /* Initialize the device */
	    InitDev(lpdv, lpbIn);

		/* 87-1-13 sec: save at the beginning of the 1st page
		 */
	    PrintChannel(lpdv, (LPSTR) "\nSaveState\n");

		if(!PaperRef (lpdv)) iResult=-1;

		DBMSG2(((LPSTR)" Control(): STARTDOC exit\r"));
    break;

	case ENDDOC:
	    if (lpdv->dh.fDirty) Control(lpdv, NEWFRAME, 0L, 0L);
		PrintChannel(lpdv, (LPSTR) "RestoreState\n");	/* 87-1-13 sec */
	    TermDev(lpdv);
	    CloseChannel(lpdv);
	    iResult = 1;
    break;

	case SETABORTPROC:
	    lpdv->dh.hdc = *(HDC FAR *)lpbIn;
	    iResult = 1;
	break;

	case GETPHYSPAGESIZE:
	    ((LPPOINT)lpbOut)->x = lpdv->dh.paper.cxPaper;
	    ((LPPOINT)lpbOut)->y = lpdv->dh.paper.cyPaper;

#ifdef DEBUG_CONTROL
/* DEBUG DEBUG DEBUG DEBUG DEBUG DEBUG DEBUG DEBUG DEBUG DEBUG DEBUG */
	{
		short numOfBins;
		short rc;
		GSPBOD oD1;
		EPBOD oD2;
		struct { short d,x,y,z } iD;

		iD.x=iD.y=iD.z=0;

#if 0
		DBMSG2(((LPSTR)"\rDEBUG: GETSETPRINTERORIENTATION\r"));

		/* return printer orientation */
		DBMSG2(((LPSTR)"\rDEBUG: getting orientation\r"));
		rc=Control(lpdv, GETSETPRINTERORIENTATION, 0L, 0L);
		DBMSG2(((LPSTR)"DEBUG: rc=%d\r",rc));
		if(rc<0){
			DBMSG2(((LPSTR)"DEBUG: ERROR!\r"));
		}else if(rc>0){
			DBMSG2(((LPSTR)"DEBUG: SUCCESS\r"));
		}else{
			DBMSG2(((LPSTR)"DEBUG: NOT IMPLEMENTED\r"));
		}

		/* set printer orientation */
		DBMSG2(((LPSTR)"\rDEBUG: setting landscape\r"));
		iD.d=ESCAPE_LANDSCAPE;
		rc=Control(lpdv, GETSETPRINTERORIENTATION, (LPSTR)&iD, 0L);
		DBMSG2(((LPSTR)"DEBUG: rc=%d\r",rc));
		if(rc<0){
			DBMSG2(((LPSTR)"DEBUG: ERROR!\r"));
		}else if(rc>0){
			DBMSG2(((LPSTR)"DEBUG: SUCCESS\r"));
		}else{
			DBMSG2(((LPSTR)"DEBUG: NOT IMPLEMENTED\r"));
		}

		/* set printer orientation */
		DBMSG2(((LPSTR)"\rDEBUG: setting portrait\r"));
		iD.d=ESCAPE_PORTRAIT;
		rc=Control(lpdv, GETSETPRINTERORIENTATION, (LPSTR)&iD, 0L);
		DBMSG2(((LPSTR)"DEBUG: rc=%d\r",rc));
		if(rc<0){
			DBMSG2(((LPSTR)"DEBUG: ERROR!\r"));
		}else if(rc>0){
			DBMSG2(((LPSTR)"DEBUG: SUCCESS\r"));
		}else{
			DBMSG2(((LPSTR)"DEBUG: NOT IMPLEMENTED\r"));
		}
#endif

		DBMSG2(((LPSTR)"\rDEBUG: GETSETPAPERBINS\r"));

		/* get the number of available bins */
		oD1.binNum=oD1.numOfBins=-999;
		DBMSG2(((LPSTR)"\rDEBUG: get the number of available bins lpIn=0\r"));
		rc=Control(lpdv, GETSETPAPERBINS, 0L, (LPSTR)&oD1);
		DBMSG2(((LPSTR)"DEBUG: oD1.bN=%d, oD1.nOB=%d\r",
			oD1.binNum,oD1.numOfBins));
		DBMSG2(((LPSTR)"DEBUG: rc=%d\r",rc));
		if(rc<0){
			DBMSG2(((LPSTR)"DEBUG: ERROR!\r"));
		}else if(rc>0){
			DBMSG2(((LPSTR)"DEBUG: SUCCESS\r"));
		}else{
			DBMSG2(((LPSTR)"DEBUG: NOT IMPLEMENTED\r"));
		}

		/* try to set a new bin */
		oD1.binNum=oD1.numOfBins=-999;
		iD.d=1;	/* upper tray */
		DBMSG2(((LPSTR)"\rDEBUG: set a new bin...UPPER iD.d=%d\r",iD.d));
		rc=Control(lpdv, GETSETPAPERBINS, (LPSTR)&iD, (LPSTR)&oD1);
		DBMSG2(((LPSTR)"DEBUG: oD1.bN=%d, oD1.nOB=%d\r",
			oD1.binNum,oD1.numOfBins));
		DBMSG2(((LPSTR)"DEBUG: rc=%d\r",rc));
		if(rc<0){
			DBMSG2(((LPSTR)"DEBUG: ERROR!\r"));
		}else if(rc>0){
			DBMSG2(((LPSTR)"DEBUG: SUCCESS\r"));
		}else{
			DBMSG2(((LPSTR)"DEBUG: NOT IMPLEMENTED\r"));
		}

		/* try to set a new bin */
		oD1.binNum=oD1.numOfBins=-999;
		iD.d=3;	/* auto tray select */
		DBMSG2(((LPSTR)"\rDEBUG: set a new bin...AUTO iD.d=%d\r",iD.d));
		rc=Control(lpdv, GETSETPAPERBINS, (LPSTR)&iD, (LPSTR)&oD1);
		DBMSG2(((LPSTR)"DEBUG: oD1.bN=%d, oD1.nOB=%d\r",
			oD1.binNum,oD1.numOfBins));
		DBMSG2(((LPSTR)"DEBUG: rc=%d\r",rc));
		if(rc<0){
			DBMSG2(((LPSTR)"DEBUG: ERROR!\r"));
		}else if(rc>0){
			DBMSG2(((LPSTR)"DEBUG: SUCCESS\r"));
		}else{
			DBMSG2(((LPSTR)"DEBUG: NOT IMPLEMENTED\r"));
		}

		/* try to set a new bin */
		oD1.binNum=oD1.numOfBins=-999;
		iD.d=0;	/* manual feed */
		DBMSG2(((LPSTR)"\rDEBUG: set a new bin...MANUAL iD.d=%d\r",iD.d));
		rc=Control(lpdv, GETSETPAPERBINS, (LPSTR)&iD, (LPSTR)&oD1);
		DBMSG2(((LPSTR)"DEBUG: oD1.bN=%d, oD1.nOB=%d\r",
			oD1.binNum,oD1.numOfBins));
		DBMSG2(((LPSTR)"DEBUG: rc=%d\r",rc));
		if(rc<0){
			DBMSG2(((LPSTR)"DEBUG: ERROR!\r"));
		}else if(rc>0){
			DBMSG2(((LPSTR)"DEBUG: SUCCESS\r"));
		}else{
			DBMSG2(((LPSTR)"DEBUG: NOT IMPLEMENTED\r"));
		}

		/* save for use by ENUMPAPERBINS */
		numOfBins=oD1.numOfBins;

		/* try to set a new bin */
		oD1.binNum=oD1.numOfBins=-999;
		iD.d=2;	/* lower tray */
		DBMSG2(((LPSTR)"\rDEBUG: set a new bin...LOWER iD.d=%d\r",iD.d));
		rc=Control(lpdv, GETSETPAPERBINS, (LPSTR)&iD, (LPSTR)&oD1);
		DBMSG2(((LPSTR)"DEBUG: oD1.bN=%d, oD1.nOB=%d\r",
			oD1.binNum,oD1.numOfBins));
		DBMSG2(((LPSTR)"DEBUG: rc=%d\r",rc));
		if(rc<0){
			DBMSG2(((LPSTR)"DEBUG: ERROR!\r"));
		}else if(rc>0){
			DBMSG2(((LPSTR)"DEBUG: SUCCESS\r"));
		}else{
			DBMSG2(((LPSTR)"DEBUG: NOT IMPLEMENTED\r"));
		}

		/* try to set a new bin */
		oD1.binNum=oD1.numOfBins=-999;
		iD.d=4;	/* cassette */
		DBMSG2(((LPSTR)"\rDEBUG: set a new bin...CASSETTE iD.d=%d\r",iD.d));
		rc=Control(lpdv, GETSETPAPERBINS, (LPSTR)&iD, (LPSTR)&oD1);
		DBMSG2(((LPSTR)"DEBUG: oD1.bN=%d, oD1.nOB=%d\r",
			oD1.binNum,oD1.numOfBins));
		DBMSG2(((LPSTR)"DEBUG: rc=%d\r",rc));
		if(rc<0){
			DBMSG2(((LPSTR)"DEBUG: ERROR!\r"));
		}else if(rc>0){
			DBMSG2(((LPSTR)"DEBUG: SUCCESS\r"));
		}else{
			DBMSG2(((LPSTR)"DEBUG: NOT IMPLEMENTED\r"));
		}

		/* try to set a new bin */
		oD1.binNum=oD1.numOfBins=-999;
		iD.d=-1;	/* upper tray */
		DBMSG2(((LPSTR)"\rDEBUG: negative bin...iD.d=%d\r",iD.d));
		rc=Control(lpdv, GETSETPAPERBINS, (LPSTR)&iD, (LPSTR)&oD1);
		DBMSG2(((LPSTR)"DEBUG: oD1.bN=%d, oD1.nOB=%d\r",
			oD1.binNum,oD1.numOfBins));
		DBMSG2(((LPSTR)"DEBUG: rc=%d\r",rc));
		if(rc<0){
			DBMSG2(((LPSTR)"DEBUG: ERROR!\r"));
		}else if(rc>0){
			DBMSG2(((LPSTR)"DEBUG: SUCCESS\r"));
		}else{
			DBMSG2(((LPSTR)"DEBUG: NOT IMPLEMENTED\r"));
		}

		/* try to set a new bin */
		oD1.binNum=oD1.numOfBins=-999;
		iD.d=1;	/* upper tray */
		DBMSG2(((LPSTR)"\rDEBUG: set UPPER-to prep for next iD.d=%d\r",iD.d));
		rc=Control(lpdv, GETSETPAPERBINS, (LPSTR)&iD, (LPSTR)&oD1);
		DBMSG2(((LPSTR)"DEBUG: oD1.bN=%d, oD1.nOB=%d\r",
			oD1.binNum,oD1.numOfBins));
		DBMSG2(((LPSTR)"DEBUG: rc=%d\r",rc));
		if(rc<0){
			DBMSG2(((LPSTR)"DEBUG: ERROR!\r"));
		}else if(rc>0){
			DBMSG2(((LPSTR)"DEBUG: SUCCESS\r"));
		}else{
			DBMSG2(((LPSTR)"DEBUG: NOT IMPLEMENTED\r"));
		}

		/* try to set a new bin */
		oD1.binNum=oD1.numOfBins=-999;
		iD.d=0;	/* manual feed */
		DBMSG2(((LPSTR)"\rDEBUG: select NO data...MANUAL iD.d=%d\r",iD.d));
		rc=Control(lpdv, GETSETPAPERBINS, (LPSTR)&iD, 0L);
		DBMSG2(((LPSTR)"DEBUG: oD1.bN=%d, oD1.nOB=%d\r",
			oD1.binNum,oD1.numOfBins));
		DBMSG2(((LPSTR)"DEBUG: rc=%d\r",rc));
		if(rc<0){
			DBMSG2(((LPSTR)"DEBUG: ERROR!\r"));
		}else if(rc>0){
			DBMSG2(((LPSTR)"DEBUG: SUCCESS\r"));
		}else{
			DBMSG2(((LPSTR)"DEBUG: NOT IMPLEMENTED\r"));
		}

		/* get the number of available bins */
		oD1.binNum=oD1.numOfBins=-999;
		DBMSG2(((LPSTR)"\rDEBUG: get avail bins--test for prev. lpIn=0\r"));
		rc=Control(lpdv, GETSETPAPERBINS, 0L, (LPSTR)&oD1);
		DBMSG2(((LPSTR)"DEBUG: oD1.bN=%d, oD1.nOB=%d\r",
			oD1.binNum,oD1.numOfBins));
		DBMSG2(((LPSTR)"DEBUG: rc=%d\r",rc));
		if(rc<0){
			DBMSG2(((LPSTR)"DEBUG: ERROR!\r"));
		}else if(rc>0){
			DBMSG2(((LPSTR)"DEBUG: SUCCESS\r"));
		}else{
			DBMSG2(((LPSTR)"DEBUG: NOT IMPLEMENTED\r"));
		}

		/* try to set a new bin */
		oD1.binNum=oD1.numOfBins=-999;
		iD.d=1;	/* upper tray */
		DBMSG2(((LPSTR)"\rDEBUG: lpIn=lpOut=0...iD.d=%d\r",iD.d));
		rc=Control(lpdv, GETSETPAPERBINS, 0L, 0L);
		DBMSG2(((LPSTR)"DEBUG: oD1.bN=%d, oD1.nOB=%d\r",
			oD1.binNum,oD1.numOfBins));
		DBMSG2(((LPSTR)"DEBUG: rc=%d\r",rc));
		if(rc<0){
			DBMSG2(((LPSTR)"DEBUG: ERROR!\r"));
		}else if(rc>0){
			DBMSG2(((LPSTR)"DEBUG: SUCCESS\r"));
		}else{
			DBMSG2(((LPSTR)"DEBUG: NOT IMPLEMENTED\r"));
		}

		/* try to set a new bin */
		oD1.binNum=oD1.numOfBins=-999;
		iD.d=1;	/* upper tray */
		DBMSG2(((LPSTR)"\rDEBUG: set a new bin...UPPER iD.d=%d\r",iD.d));
		rc=Control(lpdv, GETSETPAPERBINS, (LPSTR)&iD, (LPSTR)&oD1);
		DBMSG2(((LPSTR)"DEBUG: oD1.bN=%d, oD1.nOB=%d\r",
			oD1.binNum,oD1.numOfBins));
		DBMSG2(((LPSTR)"DEBUG: rc=%d\r",rc));
		if(rc<0){
			DBMSG2(((LPSTR)"DEBUG: ERROR!\r"));
		}else if(rc>0){
			DBMSG2(((LPSTR)"DEBUG: SUCCESS\r"));
		}else{
			DBMSG2(((LPSTR)"DEBUG: NOT IMPLEMENTED\r"));
		}

		DBMSG2(((LPSTR)"\rDEBUG: ENUMPAPERBINS\r"));


		/* enumerate available bins--should be an error */
		DBMSG2(((LPSTR)"\rDEBUG: #elements < #bins\r"));
		iD.d=1;
		rc=Control(lpdv, ENUMPAPERBINS, (LPSTR)&iD, (LPSTR)&oD2);
		for(i=0;i<oD1.numOfBins && rc>0;i++){
			DBMSG2(((LPSTR)"DEBUG: oD2.bL[%d]=%d, oD2.pN[%d][]=%ls\r",
				i,oD2.binList[i],i,(LPSTR)&oD2.paperNames[i][0]));
		}
		DBMSG2(((LPSTR)"DEBUG: rc=%d\r",rc));
		if(rc<0){
			DBMSG2(((LPSTR)"DEBUG: ERROR!\r"));
		}else if(rc>0){
			DBMSG2(((LPSTR)"DEBUG: SUCCESS\r"));
		}else{
			DBMSG2(((LPSTR)"DEBUG: NOT IMPLEMENTED\r"));
		}

		/* enumerate available bins */
		DBMSG2(((LPSTR)"\rDEBUG: enumerate the available bins\r"));
		iD.d=MAXELEMENTS;
		rc=Control(lpdv, ENUMPAPERBINS, (LPSTR)&iD, (LPSTR)&oD2);
		for(i=0;i<oD1.numOfBins;i++){
			DBMSG2(((LPSTR)"DEBUG: oD2.bL[%d]=%d, oD2.pN[%d][]=%ls\r",
				i,oD2.binList[i],i,(LPSTR)&oD2.paperNames[i][0]));
		}
		DBMSG2(((LPSTR)"DEBUG: rc=%d\r",rc));
		if(rc<0){
			DBMSG2(((LPSTR)"DEBUG: ERROR!\r"));
		}else if(rc>0){
			DBMSG2(((LPSTR)"DEBUG: SUCCESS\r"));
		}else{
			DBMSG2(((LPSTR)"DEBUG: NOT IMPLEMENTED\r"));
		}

		/* enumerate available bins--should be an error */
		DBMSG2(((LPSTR)"\rDEBUG: lpIn=0\r"));
		iD.d=1;
		rc=Control(lpdv, ENUMPAPERBINS, 0L, (LPSTR)&oD2);
		for(i=0;i<oD1.numOfBins && rc>0;i++){
			DBMSG2(((LPSTR)"DEBUG: oD2.bL[%d]=%d, oD2.pN[%d][]=%ls\r",
				i,oD2.binList[i],i,(LPSTR)&oD2.paperNames[i][0]));
		}
		DBMSG2(((LPSTR)"DEBUG: rc=%d\r",rc));
		if(rc<0){
			DBMSG2(((LPSTR)"DEBUG: ERROR!\r"));
		}else if(rc>0){
			DBMSG2(((LPSTR)"DEBUG: SUCCESS\r"));
		}else{
			DBMSG2(((LPSTR)"DEBUG: NOT IMPLEMENTED\r"));
		}

		/* enumerate available bins--should be an error */
		DBMSG2(((LPSTR)"\rDEBUG: lpIn=lpOut=0\r"));
		iD.d=1;
		rc=Control(lpdv, ENUMPAPERBINS, 0L, 0L);
		for(i=0;i<oD1.numOfBins && rc>0;i++){
			DBMSG2(((LPSTR)"DEBUG: oD2.bL[%d]=%d, oD2.pN[%d][]=%ls\r",
				i,oD2.binList[i],i,(LPSTR)&oD2.paperNames[i][0]));
		}
		DBMSG2(((LPSTR)"DEBUG: rc=%d\r",rc));
		if(rc<0){
			DBMSG2(((LPSTR)"DEBUG: ERROR!\r"));
		}else if(rc>0){
			DBMSG2(((LPSTR)"DEBUG: SUCCESS\r"));
		}else{
			DBMSG2(((LPSTR)"DEBUG: NOT IMPLEMENTED\r"));
		}

		/* enumerate available bins--should be an error */
		DBMSG2(((LPSTR)"\rDEBUG: lpOut=0\r"));
		iD.d=MAXELEMENTS;
		rc=Control(lpdv, ENUMPAPERBINS, (LPSTR)&iD, 0L);
		for(i=0;i<oD1.numOfBins && rc>0;i++){
			DBMSG2(((LPSTR)"DEBUG: oD2.bL[%d]=%d, oD2.pN[%d][]=%ls\r",
				i,oD2.binList[i],i,(LPSTR)&oD2.paperNames[i][0]));
		}
		DBMSG2(((LPSTR)"DEBUG: rc=%d\r",rc));
		if(rc<0){
			DBMSG2(((LPSTR)"DEBUG: ERROR!\r"));
		}else if(rc>0){
			DBMSG2(((LPSTR)"DEBUG: SUCCESS\r"));
		}else{
			DBMSG2(((LPSTR)"DEBUG: NOT IMPLEMENTED\r"));
		}

		/* enumerate available bins--should be an error */
		DBMSG2(((LPSTR)"\rDEBUG: #elements < 0\r"));
		iD.d=-1;
		rc=Control(lpdv, ENUMPAPERBINS, (LPSTR)&iD, (LPSTR)&oD2);
		for(i=0;i<oD1.numOfBins && rc>0;i++){
			DBMSG2(((LPSTR)"DEBUG: oD2.bL[%d]=%d, oD2.pN[%d][]=%ls\r",
				i,oD2.binList[i],i,(LPSTR)&oD2.paperNames[i][0]));
		}
		DBMSG2(((LPSTR)"DEBUG: rc=%d\r",rc));
		if(rc<0){
			DBMSG2(((LPSTR)"DEBUG: ERROR!\r"));
		}else if(rc>0){
			DBMSG2(((LPSTR)"DEBUG: SUCCESS\r"));
		}else{
			DBMSG2(((LPSTR)"DEBUG: NOT IMPLEMENTED\r"));
		}

	}
/* DEBUG DEBUG DEBUG DEBUG DEBUG DEBUG DEBUG DEBUG DEBUG DEBUG DEBUG */
#endif
	break;

	case GETPRINTINGOFFSET:
		/* Used to do this */
/*	    ((LPPOINT)lpbOut)->x = 0;
 *	    ((LPPOINT)lpbOut)->y = 0;
 */
	    ((LPPOINT)lpbOut)->x = lpdv->dh.paper.cxMargin;
    	((LPPOINT)lpbOut)->y = lpdv->dh.paper.cyMargin;
    break;

	case GETSCALINGFACTOR:
            ((LPPOINT)lpbOut)->x = ((LPPOINT)lpbOut)->y = 0;
    break;

	case SETCOPYCOUNT:
	    i = lpdv->dh.iCopies;

	    if (lpbIn!= (LPSTR) (long) NULL)
		lpdv->dh.iCopies = *((short far *)lpbIn);

	    if (lpbOut!= (LPSTR) (long) NULL)
		*((short far *)lpbOut) = i;

		if (lpdv->dh.fh>0)
			PrintChannel(lpdv, (LPSTR) "userdict begin /#copies %d def end\n",
				lpdv->dh.iCopies);
	    iResult = TRUE;
    break;

#if 0
	case SELECTPAPERSOURCE:
		DBMSG1(((LPSTR)"***SELECTPAPERSOURCE\r"));
	    if (!SetPaperSource(lpdv, *((short far *)lpbIn))) return(-1);

	    if (lpbOut!= (LPSTR) (long) NULL){
			((LPPM)lpbOut)->x = lpdv->dh.paper.cxPaper;
			((LPPM)lpbOut)->y = lpdv->dh.paper.cyPaper;

#if 0
/* big lie: */
			((LPPM)lpbOut)->rcPage.top = 0;
			((LPPM)lpbOut)->rcPage.left = 0;
			((LPPM)lpbOut)->rcPage.bottom = lpdv->dh.paper.cyPage;
#endif
			((LPPM)lpbOut)->rcPage.top = lpdv->dh.paper.cyMargin;
			((LPPM)lpbOut)->rcPage.left = lpdv->dh.paper.cxMargin;
			((LPPM)lpbOut)->rcPage.bottom = lpdv->dh.paper.cyMargin +
				lpdv->dh.paper.cyPage;

			((LPPM)lpbOut)->orientation = lpdv->dh.fLandscape ? 2 : 1;
		}
		DBMSG1(((LPSTR)"SELECTPAPERSOURCE***\r"));
    break;
#endif

	case PASSTHROUGH:
	    if (lpbIn!= (LPSTR) (long) NULL){
			cb = *((short far *)lpbIn)++;
			WriteChannel(lpdv, lpbIn, cb);

			if (lpdv->dh.fh>0) return(cb);
			else return(-1);
		}
	    return(-1);
    break;

	case STRETCHBLT:
	    StretchBlt(lpdv, lpbIn);
    break;

	case SETKERNTRACK:
	    i = lpdv->dh.iTrack;
	    lpbIn = (LPSTR) ((LPETD)lpbIn)->lpInData;
	    if (lpbIn) lpdv->dh.iTrack = *((short far *)lpbIn);
	    if (lpbOut) *((short far *)lpbOut) = i;
    break;

	case SETCHARSET:
		if ( *(WORD FAR *)lpbIn < 2) return TRUE;
	break;

	/* added by Aldus Corporation--19Jan87--sjp */
	case SETALLJUSTVALUES:{
		LPEXTTEXTDATA lpExtText=(LPEXTTEXTDATA)lpbIn;
		LPALLJUSTREC lpAllJust=(LPALLJUSTREC)lpExtText->lpInData;
		LPDRAWMODE lpDrawMode=lpExtText->lpDrawMode;
		LPFONTINFO lpFont=lpExtText->lpFont;

		DBMSG(((LPSTR)"SETALLJUSTVALUES\r"));
		DBMSG(((LPSTR)"TBreakExtra=%d\r", lpDrawMode->TBreakExtra));
		DBMSG(((LPSTR)"BreakExtra=%d\r", lpDrawMode->BreakExtra));
		DBMSG(((LPSTR)"BreakErr=%d\r", lpDrawMode->BreakErr));
		DBMSG(((LPSTR)"BreakRem=%d\r", lpDrawMode->BreakRem));
		DBMSG(((LPSTR)"BreakCount=%d\r", lpDrawMode->BreakCount));
		DBMSG(((LPSTR)"CharExtra=%d\r", lpDrawMode->CharExtra));

		lpDrawMode->TBreakExtra = 0;
		lpDrawMode->BreakExtra = 0;
		lpDrawMode->BreakErr = 1;
		lpDrawMode->BreakRem = 0;
		lpDrawMode->BreakCount = 0;
		lpDrawMode->CharExtra = 0;

		DBMSG(((LPSTR)"TBreakExtra=%d\r", lpDrawMode->TBreakExtra));
		DBMSG(((LPSTR)"BreakExtra=%d\r", lpDrawMode->BreakExtra));
		DBMSG(((LPSTR)"BreakErr=%d\r", lpDrawMode->BreakErr));
		DBMSG(((LPSTR)"BreakRem=%d\r", lpDrawMode->BreakRem));
		DBMSG(((LPSTR)"BreakCount=%d\r", lpDrawMode->BreakCount));
		DBMSG(((LPSTR)"CharExtra=%d\r", lpDrawMode->CharExtra));

		if (lpFont->dfCharSet == OEM_CHARSET){
			/*	Vector font: disable ALLJUSTVALUES and
			 *	return false.
			 */
			lpdv->dh.epJust = fromdrawmode;
			DBMSG(((LPSTR)" Control(): vector font disable and return\r"));
			return FALSE;
		}

		if (lpbIn){
			CalcBreaks (&lpdv->dh.epJustWB, lpAllJust->nBreakExtra,
				lpAllJust->nBreakCount);
			CalcBreaks (&lpdv->dh.epJustLTR, lpAllJust->nCharExtra,
				lpAllJust->nCharCount);
			if (lpdv->dh.epJustWB.extra || lpdv->dh.epJustWB.rem ||
				lpdv->dh.epJustLTR.extra || lpdv->dh.epJustLTR.rem
			){
				if (lpdv->dh.epJustLTR.rem){
					lpdv->dh.epJust = justifyletters;
				}else{
					lpdv->dh.epJust = justifywordbreaks;
				}
			}else{
				/*	Zero justification == shut off ALLJUSTVALUES.
				 */
				lpdv->dh.epJust = fromdrawmode;
				DBMSG(((LPSTR)"zero just.\r"));
			}
		}
	}
	break;

	case ENABLEPAIRKERNING:
	    i = lpdv->dh.fPairKern;
	    lpbIn = (LPSTR) ((LPETD)lpbIn)->lpInData;
	    if (lpbIn)
		lpdv->dh.fPairKern = *((short far *)lpbIn);
	    if (lpbOut)
		*((short far *)lpbOut) = i;
	break;

	case GETTECHNOLOGY:
        lstrcpy(lpbOut, (LPSTR) "PostScript");
    break;

    case SETLINECAP:
        i = lpdv->dh.iNewLineCap;
        if (lpbIn)
            lpdv->dh.iNewLineCap = *((short far *) lpbIn);
        if (lpbOut)
            *((short far *) lpbOut) = i;
    break;

    case SETLINEJOIN:
        i = lpdv->dh.iNewLineJoin;
        if (lpbIn)
            lpdv->dh.iNewLineJoin = *((short far *) lpbIn);
        if (lpbOut)
            *((short far *) lpbOut) = i;
    break;

    case SETMITERLIMIT:
        i = lpdv->dh.iNewMiterLimit;
        if (lpbIn)
            lpdv->dh.iNewMiterLimit = *((short far *) lpbIn);
        if (lpbOut)
            *((short far *) lpbOut) = i;
    break;

	case GETSETPRINTERORIENTATION:
	{
		/* lpInData is set up in the following way...
		 * struct { short orientation,spare1,spare2,spare3; } lpInData;
		 */
		DEVMODE devmode;
		short orientation;
		short rc=ESCAPE_PORTRAIT;

		DBMSG2(((LPSTR)" Control(): GETSETPRINTERORIENTATION rc=%d\r",rc));

		/* always return the present value of the orientation...
		 * unless there is an error
		 */
		if(lpdv->dh.fLandscape) rc=ESCAPE_LANDSCAPE;
		DBMSG2(((LPSTR)" Control(): rc=%d\r",rc));

		/* if NULL then return the current orientation
		 * (previously set)...if NOT NULL process...
		 */
		if(lpbIn){
			orientation = *((short far *) lpbIn);

			DBMSG2(((LPSTR)" Control(): CHANGE o=%d\r",orientation));

			/* check to make sure the orientation is withing the
			 * available bounds
			 */
			if(orientation<=ESCAPE_LANDSCAPE && orientation>=ESCAPE_PORTRAIT){

				/* make the environment...doesn't fail */
				MakeEnvironment(lpdv->dh.hmod,(LPSTR)lpdv->szDevice,
					(LPSTR)lpdv->szFile,(LPDEVMODE)&devmode);

				if(orientation==ESCAPE_LANDSCAPE) devmode.fLandscape=TRUE;
				else devmode.fLandscape=FALSE;

			    /* Update the environment with the new info and inform
				 * everyone.
				 */
				SaveEnvironment(lpdv->dh.hmod,(LPSTR)lpdv->szDevice,
					(LPSTR)lpdv->szFile,(LPDEVMODE)&devmode,
					0/*don't write*/,0/*don't send message*/);

			}else{
				rc=-1; /* invalid selection */
				DBMSG2(((LPSTR)" Control(): ERROR rc=%d\r",rc));
			}
		}
		DBMSG2(((LPSTR)"<Control(): GETSETPRINTERORIENTATION exit rc=%d\r",
			rc));
		return(rc);
	}
	break;

	case GETSETPAPERBINS:
	{
		/* lpInData is set up in the following way...
		 * struct { short theBin,spare1,spare2,spare3; } lpInData;
		 * ...the spare entries are not used at this time
		 */
		DEVMODE devmode;
		short theBin;
		short binList[NUMBINS];
		short numOfBins;
		short currentBin;
		short rc=1;
		short i;
		BOOL foundIt;
		PRINTER thePrinter;

		theBin = *((LPSHORT) lpbIn);

		DBMSG2(((LPSTR)" Control():>GETSETPAPERBINS tb=%d,rc=%d\r",
			theBin,rc));

		if( (!lpbIn && !lpbOut) || (lpbIn && (theBin < 0)) ){
			DBMSG2(((LPSTR)" Control(): ERROR\r"));
			rc=-1;
		}else{
			/* these two variables control the modes arguments for the
			 * SaveEnvironment() procedure...
			 * the first tells the proc to write the changes to win.ini
			 * the second tells it to send a WM_DEVMODECHANGE message
			 */
			short writeTheProfile=FALSE;
			short sendTheMessage=FALSE;

			DBMSG2(((LPSTR)" Control(): get the bins only\r"));

			/* make the environment...doesn't fail */
			MakeEnvironment(lpdv->dh.hmod,(LPSTR)lpdv->szDevice,
				(LPSTR)lpdv->szFile,(LPDEVMODE)&devmode);

			if(!GetPrinterCaps(lpdv->dh.hmod,lpdv->dh.iPrinter,
				(LPPRINTER)&thePrinter)
			){
				goto ERROR0;
			}

			/* generate the list of bins supported by this printer */
			MakeBinList((LPPRINTER)&thePrinter,(LPSHORT)&binList[0]);

			/* calculate the number of bins used by this printer */
			numOfBins=0;
			for(i=0;i<NUMFEEDS;i++){
				DBMSG2(((LPSTR)" Control(): BEFORE tP.feed[%d]=%d, nOB=%d",
					i,thePrinter.feed[i],numOfBins));
				if(thePrinter.feed[i]) numOfBins++;
				DBMSG2(((LPSTR)"  AFTER nOB=%d\r",numOfBins));
			}

			/* always return the current bin...
			 * for each bin in the list get the feed
			 * if the feeds match then save it so it can
			 * be returned later
			 */
			foundIt=FALSE;
			for(i=0;i<numOfBins;i++){
				DBMSG2((
					(LPSTR)" Control(): map[binList[%d]=%d]=%d,iFeed=%d\r",
					i,binList[i],gBinToFeedMap[binList[i]],devmode.iFeed));
				if(gBinToFeedMap[binList[i]] == (char)devmode.iFeed){
					currentBin=binList[i];
					foundIt=TRUE;
					DBMSG2(((LPSTR)" Control(): 1.FOUND IT!...in list\r"));
					break;
				}
			}
			if(!foundIt){
				/* processing error */
				rc=-1;
				DBMSG2(((LPSTR)" Control(): 1.???...NOT in list\r"));
			}

			/* N.B. even though there are only 3 feed sources
			 *      there can be more than 3 (i.e. 5) bin names
			 */

			if(lpbIn){
				/* check to see if the requested new bin is actually
				 * in the list of available bins for this printer
				 */
				foundIt=FALSE;
				for(i=0;i<numOfBins;i++){
					if(theBin==binList[i]){
						foundIt=TRUE;
						break;
					}
				}
				if(foundIt){
					if(lpbOut){
						/* select a new bin number */
						devmode.iFeed=gBinToFeedMap[theBin];

						/* Since devmode was changed we must send a
						 * WM_DEVMODECHANGE message.  At present it's decided
						 * NOT to update WIN.INI as changes in bin selection
						 * may negatively affect the behavior of other
						 * applications (from the user's point of view).
						 */
						writeTheProfile=FALSE;
						sendTheMessage=TRUE;
						DBMSG2(((LPSTR)" Control():2.FOUND IT!...in list\r"));
					}
				}else{
					/* parameter error */
					rc=-1;
					DBMSG2(((LPSTR)" Control(): 2.???...NOT in list\r"));
				}
			}
			if(lpbOut){
				((LPGSPBOD)lpbOut)->binNum=currentBin;
				DBMSG2(((LPSTR)" Control(): oD1.bN=%d\r",
					((LPGSPBOD)lpbOut)->binNum));

				((LPGSPBOD)lpbOut)->numOfBins=numOfBins;
				DBMSG2((
					(LPSTR)" Control():nOB=%d,oD1.nOB=%d, theBin=%d, rc=%d\r",
					numOfBins,((LPGSPBOD)lpbOut)->numOfBins,theBin,rc));

				/* for future use... */
				((LPGSPBOD)lpbOut)->spare1=0;
				((LPGSPBOD)lpbOut)->spare2=0;
				((LPGSPBOD)lpbOut)->spare3=0;
				((LPGSPBOD)lpbOut)->spare4=0;
			}

			DBMSG2(((LPSTR)" Control(): oD1.nOB=%d, tB=%d, rc=%d\r",
				((LPGSPBOD)lpbOut)->numOfBins,theBin,rc));
		    /* Update the environment with the new info and inform
			 * everyone.
			 */
			SaveEnvironment(lpdv->dh.hmod,(LPSTR)lpdv->szDevice,
				(LPSTR)lpdv->szFile,(LPDEVMODE)&devmode,
				writeTheProfile,sendTheMessage);

			/* !!! At present ignoring the possibility that a change
			 * !!! bin request occurs while the dialog is displayed.
			 */
		}
		DBMSG2(((LPSTR)"<Control():<GETSETPAPERBINS exit rc=%d\r",rc));
		return(rc);

ERROR0:
/*		This doesn't work...figure out a better way later.
 *		MessageBox(lpdv->dh.hmod,(LPSTR)"Can't read resource.",
 *			(LPSTR)"Fatal driver error!",MB_OK | MB_ICONEXCLAMATION);
 */		DBMSG2(((LPSTR)"<Control():<GETSETPAPERBINS exit rc=%d\r",rc));
		return(-1);
	}
	break;

	case ENUMPAPERBINS:
	{
		/* lpInData is set up in the following way...
		 * struct { short numOfElements,spare1,spare2,spare3; } lpInData;
		 * ...the spare entries are not used at this time
		 */
		DEVMODE devmode;
		short binList[NUMBINS];
		short numOfBins;
		short rc=1;
		short i;
		short numOfElements;
		LPSHORT lpBinList;
		LPSTR lpPaperNames;
		LPSTR lpPaperBins;
		PRINTER thePrinter;
		HANDLE hData;

		DBMSG2(((LPSTR)" Control():>ENUMPAPERBINS rc=%d\r",rc));

		/* make the environment...doesn't fail */
		MakeEnvironment(lpdv->dh.hmod,(LPSTR)lpdv->szDevice,
			(LPSTR)lpdv->szFile,(LPDEVMODE)&devmode);

		if(!GetPrinterCaps(lpdv->dh.hmod,lpdv->dh.iPrinter,
			(LPPRINTER)&thePrinter)
		){
			goto ERROR1;
		}

		/* generate the list of bins supported by this printer */
		MakeBinList((LPPRINTER)&thePrinter,(LPSHORT)&binList[0]);

		/* calculate the number of bins used by this printer */
		numOfBins=0;
		for(i=0;i<NUMFEEDS;i++){
			DBMSG2(((LPSTR)" Control(): BEFORE gPC.feed[%d]=%d, nOB=%d",
				i,thePrinter.feed[i],numOfBins));
			if(thePrinter.feed[i]) numOfBins++;
			DBMSG2(((LPSTR)"  AFTER nOB=%d\r",numOfBins));
		}

		/* get the number of elements in the ouput
		 * data structure
		 */
		numOfElements=*((LPSHORT)lpbIn);
		DBMSG2(((LPSTR)" Control(): ENUMPAPERBINS nE=%d\r",numOfElements));

		if(numOfElements<numOfBins || !lpbIn || (lpbIn && !lpbOut)){
			DBMSG2(((LPSTR)" Control(): ERROR: "));
#ifdef DEBUG_CONTROL
			if(numOfElements<numOfBins)
				DBMSG2(((LPSTR)"nOE=%d,nOB=%d\r",numOfElements,numOfBins));
			if(!lpbIn)
				DBMSG2(((LPSTR)"lpbIn=%d,!lpbIn=%ld\r",lpbIn,!lpbIn));
			if(lpbIn && !lpbOut)
				DBMSG2(((LPSTR)"lpbIn=%d,lpbOut=%ld,!lpbOut=%ld\r",
					lpbIn,lpbOut,!lpbOut));
#endif
			rc=-1;
		}else{
			HANDLE hData;
			short offset=0;/* for indexing into the array of bins names */

			/* get 1st array element */
			lpBinList=(LPSHORT)lpbOut;

			/* get 1st paper name position */
			lpPaperNames=(LPSTR)(lpBinList+numOfElements);

			if(!(lpPaperBins=GetResourceData(lpdv->dh.hmod,(LPHANDLE)&hData,
				(LPSTR)(long)PAPERBINS,(LPSTR)(long)MY_DATA))
			){
				goto ERROR1;
			}
			for(i=0;i<numOfBins;i++){

				/* add the bin number to the list */
				*(lpBinList+i)=binList[i];
				DBMSG2(((LPSTR)" Control(): bL[%d]=%d\r",
					i,*(lpBinList+i) ));

				/* add the bin string to the list */
				lstrcpy(lpPaperNames+offset,lpPaperBins+binList[i]);
				DBMSG2(((LPSTR)" Control(): name=%ls,pN[%d]=%ls\r",
					lpPaperBins+binList[i],i,lpPaperNames+offset));
				offset+=BINSTRLEN;
			}
			if(!UnGetResourceData(hData)) goto ERROR1;
		}
		DBMSG2(((LPSTR)" Control():<ENUMPAPERBINS exit rc=%d\r",rc));
		return(rc);

ERROR1:
/*		This doesn't work...figure out a better way later.
  *		MessageBox(lpdv->dh.hmod,(LPSTR)"Can't read resource.",
 *			(LPSTR)"Fatal driver error!",MB_OK | MB_ICONEXCLAMATION);
 */		DBMSG2(((LPSTR)" Control():<ENUMPAPERBINS exit rc=%d\r",rc));
		return(-1);

	}
	break;

	default:
	    return(FALSE);
	break;
	}
	DBMSG2(((LPSTR)"<Control(): Normal exit\r"));
    return(TRUE);
}

