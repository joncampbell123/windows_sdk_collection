/*********************************************************************
 * PROFILE.C
 *
 * 14Aug87	sjp		Moved MapProfile(), GetPaperType() and ReadProfile()
 *					from segment RESET.
 *********************************************************************
 */

#include "pscript.h"
#include "printers.h"


char szKey[40];     /* The profile key */
char szPaper[] = "paperX";
BOOL gl_dloaded = FALSE;	/* header (supposedly) downloaded */
BOOL gl_tm = FALSE;			/* "tile mode" (big printable area) */
BOOL gl_lie = FALSE;		/* "little lie" (printable area = paper size) */

BOOL FAR PASCAL isUSA(void);
void FAR PASCAL SetKey(LPSTR);
void FAR PASCAL ReadProfile(HANDLE, DEVMODE FAR *, LPSTR);
int FAR PASCAL GetPaperType(HANDLE, int);
int FAR PASCAL MapProfile(HANDLE, LPSTR, int, int, int);


/**************************************************************
* Name: SetKey()
*
* Action: This function forms a key for indexing into the
*	  win.ini file to retrieve information about a
*	  PostScript printer on a specific port.
*
*	  The key has the following format: [PostScript,COM1]
*
* Output: The key is left in the global variable called "szKey".
*
**************************************************************
*/
void FAR PASCAL SetKey(lszFile)
	LPSTR lszFile;
{
    static char szDevice[] = "PostScript,";

    LPSTR lpbDst;
    LPSTR lpbSrc;
    int cb;

    lpbSrc = szDevice;
    lpbDst = szKey;

    while (*lpbDst++ = *lpbSrc++)
	;
    if (lszFile){
		--lpbDst;
		cb = sizeof(szKey) - sizeof(szDevice);
		while (*lszFile && cb>0){
		    if (*lszFile==':' && *(lszFile+1)==0)
				break;
		    *lpbDst++ = *lszFile++;
		    --cb;
		}
		*lpbDst++ = 0;
	}
}


/******************************************************************
* Name: MapProfile()
*
* Action: Map a profile string from win.ini to an index from 0 .. N
*
*	  The win.ini string is compared with a series of strings
*	  from the driver's resource file till a matching string
*	  is found.
*
********************************************************************
*/
int FAR PASCAL MapProfile(hinst, lszKey, idsFirst, idsLast, idsDefault)
HANDLE hinst;	    /* The instance handle */
LPSTR lszKey;	    /* The profile key */
int idsFirst;	    /* The first resource string id */
int idsLast;	    /* The last resource string  id */
int idsDefault;     /* The default resource string */
{
    static char szRes[32];	/* The resource string */
    static char szProf[32];	/* The profile string */
    int ids;			/* The string resource id */


    /* Default to the Laser-Writer Plus */
    LoadString(hinst, idsDefault, (LPSTR) szRes, sizeof(szRes));

    GetProfileString((LPSTR) szKey, lszKey, (LPSTR)szRes, (LPSTR)szProf,
	    sizeof(szProf));
    for (ids=idsFirst; ids<=idsLast; ++ids){
		LoadString(hinst, ids, (LPSTR) szRes, sizeof(szRes));

		if (!lstrcmp((LPSTR)szRes, (LPSTR)szProf))
		    return(ids - idsFirst);
	}
    return(idsDefault - idsFirst);
}


/***********************************************************************
* Name: GetPaper()
*
* Action: Get the paper type from win.ini
*
************************************************************************
*/
int FAR PASCAL GetPaperType(hinst, iFeed)
HANDLE hinst;
int iFeed;      /* The tray from which the paper is being fed */
{
    int idsDefault;
    int i;

    szPaper[sizeof(szPaper) - 2] = (char)(iFeed + (int)'0');

    if (isUSA())
        idsDefault = IDS_PAPER0;    /* US LETTER */
    else
        idsDefault = IDS_PAPER5;    /* DIN A4 */


    i = MapProfile(hinst, (LPSTR)szPaper, IDS_PAPERMIN, IDS_PAPERMAX,
			idsDefault) + LETTER;
    return(i);
}


/********************************************************************
* Name: ReadProfile()
*
* Action: Read the device mode parameters from win.ini.
*
**********************************************************************
*/
void FAR PASCAL ReadProfile(hinst, lpdm, lszFile)
HANDLE hinst;	    /* The instance handle */
DEVMODE FAR *lpdm;  /* Far ptr to the device mode info */
LPSTR lszFile;
{
    int i;
	char dlstr[30];
	char yesstr[15];
	char nostr[15];
	char dlbuf[40];
	char tmstr[30];
	char tmbuf[40];
	char liestr[30];
	char liebuf[40];

    SetKey(lszFile);

    lpdm->iPrinter = MapProfile(hinst, (LPSTR)"device", IDS_PRMIN, IDS_PRMAX,
		IDS_PR1);

	/* There should be some error checking on the return value for
	 * iFeed since there could be a win.ini error.
	 */
    lpdm->iFeed = GetProfileInt((LPSTR) szKey, (LPSTR)"papersource",
		UPPERTRAY - IFEEDMIN);

    lpdm->iPaper = GetPaperType(hinst, lpdm->iFeed);

    lpdm->iRes = GetProfileInt((LPSTR) szKey,(LPSTR)"resolution",DEFAULTRESOLUTION);
	if(lpdm->iRes<0 || lpdm->iRes>IRESMAX){
                lpdm->iRes=DEFAULTRESOLUTION;
	}
    lpdm->fLandscape =
		GetProfileInt((LPSTR) szKey,(LPSTR)"orientation",DEFAULTORIENTATION);

    lpdm->szDevice[0] = 0;
    lpdm->iCopies = 1;

    for (i=0; i<IPAPERMAX-IPAPERMIN; ++i)
        lpdm->rgiPaper[i] = GetPaperType(hinst, i);

	/* get the downloaded flag
	 */
	LoadString (hinst, IDS_DLOADED, (LPSTR) dlstr,  sizeof(dlstr));
	LoadString (hinst, IDS_YES,     (LPSTR) yesstr, sizeof(yesstr));
	GetProfileString ((LPSTR) szKey, (LPSTR) dlstr, (LPSTR) "no",
	 (LPSTR)dlbuf, sizeof(dlbuf));

	/* if Header Downloaded=yes, set the global
	 */
	if (lstrcmp((LPSTR)dlbuf, (LPSTR)yesstr))
		gl_dloaded = FALSE;
	else
		gl_dloaded = TRUE;

	/* get the big printable area flag
	 */
	LoadString (hinst, IDS_TM, (LPSTR) tmstr,  sizeof(tmstr));
	GetProfileString ((LPSTR) szKey, (LPSTR) tmstr, (LPSTR) "no",
	 (LPSTR)tmbuf, sizeof(tmbuf));
	/* if Tile Mode=yes, set the global
	 */
	if (lstrcmp((LPSTR)tmbuf, (LPSTR)yesstr))
		gl_tm = FALSE;
	else
		gl_tm = TRUE;

	/* get the little lie (printable area = page size) flag
	 * big lie has precedence over this
	 */
	LoadString (hinst, IDS_LIE, (LPSTR) liestr,  sizeof(liestr));
	LoadString (hinst, IDS_NO, (LPSTR) nostr, sizeof(nostr));
	GetProfileString ((LPSTR) szKey, (LPSTR) liestr, (LPSTR) "yes",
	 (LPSTR)liebuf, sizeof(liebuf));
	/* if Margins=no, set the global
	 */
	if (lstrcmp((LPSTR)liebuf, (LPSTR)nostr))
		gl_lie = FALSE;
	else
		gl_lie = TRUE;
}



