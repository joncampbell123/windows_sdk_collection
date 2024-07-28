/**[f******************************************************************
 * atstuff.c - 
 *
 * Copyright (C) 1988 Aldus Corporation.  All rights reserved.
 * Copyright (C) 1989 Microsoft Corporation.
 * Company confidential.
 *
 **f]*****************************************************************/

#include "pscript.h"
#include <winexp.h>
#include "pserrors.h"
#include "debug.h"
#include "atprocs.h"
#include "atstuff.h"
#include "psdata.h"
#include "resource.h"
#include "utils.h"

/************************** global data ************************************/

BOOL	(FAR PASCAL *lpfnATSelect)(HWND);
void	(FAR PASCAL *lpfnATMessage)(short,WORD,LPSTR);
BOOL	(FAR PASCAL *lpfnATOpenStatusWnd)(HWND);
void	(FAR PASCAL *lpfnATCloseStatusWnd)(void);
short	(FAR PASCAL *lpfnATWrite)(LPSTR, short);
short	(FAR PASCAL *lpfnATClose)(void);
short	(FAR PASCAL *lpfnATSendEOF)(void);
BOOL	(FAR PASCAL *lpfnATChooser)(HWND);
HANDLE	(FAR PASCAL *lpfnATOpen)(LPSTR, LPSTR, HDC);

HWND ghATModule = NULL;		/* module handle for AppleTalk DLL */


/**************************** local functins ***************************/

void	PASCAL FixFile(LPSTR,LPSTR,short);
BOOL	PASCAL GetATModName(HANDLE,LPSTR,short);



/**************************** local data ********************************/

BOOL gfAT = FALSE;


#define SIZE 40


/* ATQuery()
 *
 * check to see if lpFile is "AppleTalk".  Sets global AT flag
 * to enable AppleTalk module loading and output.
 */

BOOL FAR PASCAL ATQuery(LPSTR lpFile)
{
	char idsAppleTalk[SIZE];

	LoadString(ghInst,IDS_APPLETALK,idsAppleTalk,sizeof(idsAppleTalk));

	if (lstrcmpi(idsAppleTalk, lpFile))
		gfAT = FALSE;		/* not apple talk */
	else
		gfAT = TRUE;		/* on apple talk */

	return(gfAT);
}

#if 0

/* returns TRUE if on an AppleTalk port */

BOOL FAR PASCAL ATState()	/* this has been replaced with a macro */
{
	return gfAT;
}

#endif

/* allows AppleTalk to be turned off or on */

void FAR PASCAL ATChangeState(fValue)
	BOOL fValue;
{
	gfAT=fValue;
}


/* disables appletalk if lpFileName is something other than AppleTalk
 *
 * returns TRUE if appletalk was bypassed (and should be turned back on
 * when done) */

BOOL FAR PASCAL ATBypass(lpFileName)
	LPSTR lpFileName;
{
	BOOL	fBypass = FALSE;
	char	tempString[SIZE];

	DBMSG((">ATBypass(): %d\n",ATState()));

	if (ATState()) {
		LoadString(ghInst, IDS_APPLETALK, tempString, sizeof(tempString));

		DBMSG((">ATBypass(): %ls,%ls\n",lpFileName,(LPSTR)tempString));

		if (lstrcmpi(tempString, lpFileName)) {
			ATChangeState(FALSE);
			fBypass = TRUE;
		}
	}
	DBMSG((">ATBypass(): bypass=%d\n",fBypass));
	return fBypass;
}


void PASCAL FixFile(lpS,lpExt,lenS)
	LPSTR lpS;
	LPSTR lpExt;
	short lenS;
{
	LPSTR lpTemp=lpS;
	BOOL fDot=FALSE;

	/* if the path does not have an extension, add the extension requested */
	while(*lpTemp){
		if(*lpTemp=='.'){
			fDot=TRUE;
			break;
		}
                lpTmp = AnsiNext(lpTmp);

	}
	DBMSG(((LPSTR)" FixFile(): fDot=%d,path=%ls,ext=%ls\n", fDot,lpS,lpExt));
	if(!fDot) lstrncat(lpS,lpExt,lenS);
}


BOOL PASCAL GetATModName(hInst,lpPathString,sizeofPathString)
	HANDLE hInst;
	LPSTR lpPathString;
	short sizeofPathString;
{
	char tempString[13];
        LPSTR lpLastSep, lpStr;


!!!  Warning !!!  this code won't work because we must
obtain and pass the name of the printer model to SetKey.
SetKey now requires both the model name and port.

	GetProfileStringFromResource(IDS_APPLETALK,1/*use key*/,
		IDS_ATMODULEFILE,IDS_DEFAULT_ATFILE,lpPathString,
		sizeofPathString);
	DBMSG((">GetATModName(): path=%ls\n",lpPathString));

	/* If there is no path available from the WIN.INI use the path
	 * of the PSCRIPT module.
	 */
	if(lpPathString[0]==':'){
		DBMSG(((LPSTR)" GetATModName(): DEFAULT AT path\n"));
		/* if the PSCRIPT module file name cannot be gotten...ERROR */
		if(!GetModuleFileName(hInst,lpPathString,sizeofPathString)){
			goto ERR;
		}
		DBMSG(((LPSTR)" GetATModName(): DEFAULT AT path=%ls\n",lpPathString));

		LoadString(hInst,IDS_DEFAULT_ATMODNAME,(LPSTR)tempString,
			sizeof(tempString));
		DBMSG(((LPSTR)" GetATModName(): mod=%ls\n",(LPSTR)tempString));

		/* find the end of the "path" portion of the file string */
                lpLastSep = NULL;
                for (lpStr = lpPathString; *lpStr; lpStr = AnsiNext(lpStr)) {
                    if (*lpStr == '\\' || *lpStr == ':')
                        lpLastSep = lpStr;
                }

                lpStr = lpLastSep ? AnsiNext(lpLastSep) : lpPathString;
                *lpStr = '\0';

		if((lstrlen(lpPathString)+lstrlen((LPSTR)tempString))
			> sizeofPathString
		){
			goto ERR;
		};
		lstrcpy(lpStr,(LPSTR)tempString);
		DBMSG(((LPSTR)" GetATModName(): path=%ls\n",lpPathString));
	}else{
		DBMSG(((LPSTR)" GetATModName(): WIN.INI AT path=%ls\n",lpPathString));
		FixFile(lpPathString,(LPSTR)".dll",sizeofPathString);
		DBMSG(((LPSTR)" GetATModName(): path=%ls\n",lpPathString));
	}
	return TRUE;

ERR:
	return FALSE;
}


BOOL FAR PASCAL LoadAT()
{
	char buf[100];

	DBMSG((">LoadAT()\n"));

	if (ghATModule >= 32)
		return TRUE;	/* already loaded */

	if (!GetATModName(ghInst, buf, sizeof(buf))){
		return FALSE;
	}

	if ((ghATModule = LoadLibrary(buf)) < 32) {
		ghATModule=0;
		return FALSE;
	}

	if (!(lpfnATSelect=GetProcAddress(ghATModule,"ATSelect"))		  || 
	    !(lpfnATMessage=GetProcAddress(ghATModule,"ATMessage"))		  || 
	    !(lpfnATOpenStatusWnd=GetProcAddress(ghATModule,"ATOpenStatusWnd"))   || 
	    !(lpfnATCloseStatusWnd=GetProcAddress(ghATModule,"ATCloseStatusWnd")) || 
	    !(lpfnATOpen=GetProcAddress(ghATModule,"ATOpen"))		        || 
	    !(lpfnATWrite=GetProcAddress(ghATModule,"ATWrite"))		        || 
	    !(lpfnATClose=GetProcAddress(ghATModule,"ATClose"))		        || 
	    !(lpfnATSendEOF=GetProcAddress(ghATModule,"ATSendEOF"))		||
	    !(lpfnATChooser=GetProcAddress(ghATModule,"ATChooser"))) {

		DBMSG(("GetProcAddresses failed\n"));
		FreeLibrary(ghATModule);
		return FALSE;
	}

	DBMSG(("<LoadAT()\n"));
	return TRUE;
}


void FAR PASCAL UnloadAT()
{
	if (ghATModule) {
		FreeLibrary(ghATModule);
		ghATModule=0;
	}
}



#define GW_OWNER	    4
HWND FAR PASCAL GetWindow(HWND, WORD);


/* check if on AppleTalk and if so
 * put up dialogs
 *
 * returns:
 *	0	not on apple talk
 *	1	user pressed cancel (abort printing)
 *	-1	failed to load apple talk module
 */

short FAR PASCAL TryAT()
{
	HWND hWnd;
	
	DBMSG((">TryAT()\n"));

	if (ATState()) {	/* don't try unless we are on AppleTalk */

		/* find the top level window */

		hWnd = GetFocus();

		/* bring up the current selections and display the status box */
		if (!LoadAT()) {
			DBMSG(("module load failed\n"));
			return(-1);	/* failed to load */
		}

		if (!ATSelect(hWnd)) {
			DBMSG(("user abort\n"));
			return(1);	/* user abort */
		}

		if (!ATOpenStatusWnd(hWnd)) {
			DBMSG(("open status failed\n"));
			return(-1);	/* couldn't load status wnd */
		}
	}
	DBMSG(("<TryAT()\n"));
	return 0;			/* everything ok */
}


void FAR PASCAL KillAT()
{
	if(ATState()){
		if (ghATModule)
			ATCloseStatusWnd();
		UnloadAT();
	}
}
