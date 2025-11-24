//*************************************************************
//  File name: devmode.c
//
//  Description:  Contains DeviceMode and ExtDeviceMode functions
//
//*************************************************************
#define PRINTDRIVER
#define NOPQ
#define NOINSTALLFILES
#include <print.h>
#include <windowsx.h>
#include <gdidefs.inc>
#include <string.h>                // yuck - for bug fix

#include "device.h"
#include "driver.h"
#include "color.h"
#include "control.h"
#include "dialog.h"
#include "glib.h"
#include "initlib.h"
#include "object.h"
#include "output.h"
#include "print.h"
#include "profile.h"
#include "text.h"
#include "utils.h"
#include "devmode.h"
#include "data.h"
#include <memory.h>
#include <winerror.h>

// Static tables used for Property sheets
LPCSTR  Templates[DI_MAX] = {MAKEINTRESOURCE(DLG_PAPERS),
			     MAKEINTRESOURCE(DLG_DEVICE_OPTIONS)};

DLGPROC Procs[DI_MAX] = {(DLGPROC)PaperDlgProc,
			 (DLGPROC)DeviceOptionsDlgProc};

//--------------------------------------------------------------------
// Function: SetDlgFlags(lpdi)
//
// Action: Set the flags for the dialogs, based on the current device
//         and whether or not this is an AdvancedSetUpDialog call.
//--------------------------------------------------------------------
VOID NEAR PASCAL SetDlgFlags(LPDI lpdi)
{
   lpdi->wFlags[DI_DEVICE] = 1;
   lpdi->wFlags[DI_PAPER]  = lpdi->bExtDevMode;
}

//------------------------------------------------------------------------//
// Function: ExtDeviceMode(HWND, HANDLE, LPDM, LPSTR, LPSTR,
//                            LPDM, LPSTR, WORD)
//
// Action:
//          This function first retrieves the current device mode from either
//          the given port or the given profile (default: WIN.INI).
//          Then, it may modify the device mode if so requested
//          (DM_MODIFY). It may also display a dialog box to for
//          configuring the printer if so requested (DM_PROMPT).
//          Finally, if the DM_UPDATE bit of 'wMode' is on, it saves
//          the changes by writing them into both the port and
//          the given profile and broadcasts this change to the related
//          windows. It can also write into the given template if so
//          requested (DM_COPY).
//
// Return Value:
//          This functions returns -1 if it fails in any way. Examples:
//          (1) the given model is not supported by the specified driver;
//          (2) it is asked to display a device mode dialog box when
//              there is alreay one in the system at this time;
//          (3) it cannot load the mini driver resources successfully.
//
//          If wMode == 0, it returns the size
//          of EXTDEVMODE structure. If the dialog box is actually
//          displayed, it returns either IDOK or IDCANCEL depending
//          on what the user chooses. If there is no dialog box and
//          the function is successful, return IDOK.
//
// Side Effect:
//        * the environment associated with the given port might be
//          modified, if it is not for the calling device.
//        * The given private profile or "WIN.INI" might be changed if
//          there is a DM_UPDATE request.
//------------------------------------------------------------------------//

int FAR PASCAL ExtDeviceMode(hWnd, hInst, lpdmOut, lpDevName,
			    lpPort, lpdmIn, lpProfile, wMode)
HWND           hWnd;           // parent for DM_PROMPT dialog box
HINSTANCE      hInst;          // Instance handle of the driver
LPENVIRONMENT  lpdmOut;        // output DEVMODE for DM_COPY
LPSTR          lpDevName;      // device name
LPSTR          lpPort;         // port name
LPENVIRONMENT  lpdmIn;         // input DEVMODE for DM_MODIFY
LPSTR          lpProfile;      // alternate .INI file
WORD           wMode;          // operation(s) to carry out
{
    int  nReturn=-1;

    if(!wMode)
	return sizeof(ENVIRONMENT);

    nReturn=EnterDevMode(hWnd,hInst,lpdmOut,lpDevName,lpPort,
			  lpdmIn,lpProfile,wMode,TRUE,NULL,NULL);
    return nReturn;
}


//-------------------------------------------------------------------
// Function: ExtDeviceModePropSheet
//
// Action: Build a list of dialog pages and return them to the
//         caller (print manager) . Cleanup happens in IEndDevMode.
//-------------------------------------------------------------------
int WINAPI ExtDeviceModePropSheet(HWND                 hWnd,
				  HINSTANCE            hInst,
				  LPSTR                lpDevName,
				  LPSTR                lpPort,
				  DWORD                dwReserved,
				  LPFNADDPROPSHEETPAGE lpfnAdd,
				  LPARAM               lParam)
{
    return EnterDevMode(hWnd,hInst,NULL,lpDevName,lpPort,
			NULL,NULL,DM_PROMPT | DM_UPDATE,
			TRUE,lpfnAdd,lParam);
}

//----------------------*AdvancedSetUpDialog*----------------------------
// Action: display driver-specific settings and allow the user to modify
//          them.
//-------------------------------------------------------------------------

LONG WINAPI AdvancedSetUpDialog(hWnd, hInst, lpdmIn, lpdmOut)
HWND            hWnd;
HANDLE          hInst;          // handle of the driver module
LPENVIRONMENT   lpdmIn;         // initial device settings
LPENVIRONMENT   lpdmOut;        // final device settings
{
    if(!lpdmIn || !lpdmOut || lpdmIn->dm.dmSpecVersion < 0x0300)
	return -1L;

    return (LONG)EnterDevMode(hWnd,hInst,lpdmOut,
			       lpdmIn->dm.dmDeviceName,NULL,
			       lpdmIn,NULL,DM_COPY | DM_PROMPT,
			       FALSE,NULL,NULL);
}


//-----------------------------------------------------------------------
// Function: MyStrncpy(lpTo,lpFrom,wMax)
//
// Action: Copy up to wMax bytes from lpFrom to lpTo. Make sure that
//         the final contents of lpTo are always NULL-terminated.
//
// Assume: wMax > 0, lpTo is valid
//
// Return: Pointer to lpTo, or NULL (if lpFrom is NULL)
//-----------------------------------------------------------------------
LPSTR NEAR PASCAL MyStrncpy(LPSTR lpTo,
			    LPSTR lpFrom,
			    WORD  wMax)
{
    LPSTR lpReturn;

    if(lpFrom)
    {
	WORD wLength=0;

	lpReturn=lpTo;

	while(wLength<wMax && (*lpTo++ = *lpFrom++))
	    wLength++;

	lpTo[wMax-1]='\0';
    }
    else
	lpReturn=NULL;

    return lpReturn;
}



//-----------------------------------------------------------------------
// Function: EnterDevMode(hWnd,hInst,lpdmOut,lpDevName,lpPort,
//                         lpdmIn,lpProfile,wMode,bExtDevMode,
//                         lpfnAdd,lParam)
//
// Action: This is the engine that handles all UI for HPPLOT. It is
//         called, directly or indirectly, from DeviceMode,
//         ExtDeviceMode, ExtDeviceModePropSheet, and
//         AdvancedSetUpDialog. Since ExtDeviceModePropSheet
//         returns control to the caller (print manager), all of the
//         Cleanup/Profile write code is encapsulated into IEndDevMode.
//
// Return: -1 on failure
//-----------------------------------------------------------------------
int NEAR PASCAL EnterDevMode(HWND                  hWnd,
			     HINSTANCE            hInst,
			     LPENVIRONMENT        lpdmOut,
			     LPSTR                lpDevName,
			     LPSTR                lpPort,
			     LPENVIRONMENT        lpdmIn,
			     LPSTR                lpProfile,
			     WORD                 wMode,
			     BOOL                 bExtDevMode,
			     LPFNADDPROPSHEETPAGE lpfnAdd,
			     LPARAM               lParam)
{
    LPDI       lpdi;        // Far pointer to dialog information

    // Before we can do anything else, we need some memory to hold all of
    // the data. If we can't allocate it, fail immediately.
    lpdi = (LPDI)GlobalAllocPtr(GHND,sizeof(DIALOGINFO));

    if(!lpdi)
    {
	return -1;
    }
    // Initialize as much of lpdi as possible now
    MyStrncpy(lpdi->szPort,lpPort,sizeof(lpdi->szPort));
    lpdi->lpProfile=MyStrncpy(lpdi->szProfile,lpProfile,sizeof(lpdi->szProfile));
    lpdi->wMode=wMode;
    lpdi->bExtDevMode=bExtDevMode;
    lpdi->lpDMTarget=lpdmOut;
    MyStrncpy(lpdi->szDevName,lpDevName,sizeof(lpdi->szDevName));

    lpdi->hInst=hInst;

    if(bExtDevMode)
    {
	//----------------------------------------------
	// Block calls that would create race conditions
	//----------------------------------------------
	
#ifdef MICHELLE        

	if(wMode & DM_UPDATE)
	{
	    ATOM aFind;
	    ATOM aAdd;
	    char szBlockPair[64];

	    // Use the atom table to keep track of blocks for us. If the
	    // current modelname,port combination exists in the local
	    // table, then we're blocked--exit. If it doesn't exist, add
	    // it.

	    MakeAppName(szBlockPair,lpdi->szDevName,lpdi->szPort, sizeof(szBlockPair));

	    // When available, use semaphores to handle preemption

#ifdef SEMAPHORE
	    SetSemaphore();
#endif
	    if(!(aFind=FindAtom(szBlockPair)))
		aAdd=AddAtom(szBlockPair);

#ifdef SEMAPHORE
	    ClearSemaphore();
#endif

	    if(aFind)
	    {
		// If the user expected a dialog, tell him why he's not
		// getting one...
		if(wMode & DM_PROMPT)
		{
		  MessageBox(hWnd, GetString(DIALOG_BUSY), lpDevName,
			     MB_ICONSTOP | MB_OK);
		}
		goto IEDM_exit;
	    }
	    else
		lpdi->aBlock=aAdd;
	}
#endif

	
	
	if (!GetDeviceMode(&lpdi->dmOrig,lpdi->szPort,lpdi->szDevName,lpdi->lpProfile))
	    goto IEDM_exit;

	_fmemcpy((LPSTR)&lpdi->dmMerge,(LPSTR)&lpdi->dmOrig,sizeof(ENVIRONMENT));

	if ((wMode & DM_MODIFY) && lpdmIn)
	    ModifyDeviceMode(&lpdi->dmMerge,lpdmIn);
    }
    else
    {
	// Simply use lpdmIn unmodified--no need to worry about dmOld
	//
	// Validation needed here?
	//
	_fmemcpy((LPSTR)&lpdi->dmMerge,(LPSTR)lpdmIn,sizeof(ENVIRONMENT));
    }

    // We're now initialized, so set the flag. Since dmMerge contains the
    // final data if no prompt is requested, set lpDM to point to dmMerge,
    // and we'll change it if a prompt is needed.

    lpdi->lpDM=&lpdi->dmMerge;
    lpdi->bInitialized=TRUE;

    if (wMode & DM_PROMPT)
    {
	PROPSHEETPAGE   psp[DI_MAX];
	WORD            wUsed;
	WORD            wLoop;

	// Zero-init psp, in case the structure size changes...
	_fmemset((LPSTR)&psp,0,DI_MAX*sizeof(PROPSHEETPAGE));

	// Do all dialog operations on dmScratch, then point to it if
	// the user clicks OK. (dmTemp is already initialized to the
	// initial dialog settings)

	lpdi->lpDM=&lpdi->dmScratch;
	_fmemcpy((LPSTR)lpdi->lpDM,(LPSTR)&lpdi->dmMerge,sizeof(ENVIRONMENT));
	SetDlgFlags(lpdi);

	for(wUsed=0,wLoop=DI_PAPER;wLoop<DI_MAX;wLoop++)
	{
	    if(lpdi->wFlags[wLoop])
	    {
		psp[wUsed].dwSize=sizeof(PROPSHEETPAGE);
		psp[wUsed].hInstance=lpdi->hInst;
		psp[wUsed].pszTemplate=Templates[wLoop];
		psp[wUsed].pfnDlgProc=Procs[wLoop];
		psp[wUsed].lParam=(LPARAM)lpdi;
		wUsed++;
	    }
	}

	// No need to do any more if we don't have any pages
	if(wUsed)
	{
	    PROPSHEETPROC PSProc;
	    CREATEPROC    CreateProc;
	    FARPROC       InitProc;

	    // Time to load the DLLs that we need for the dialogs. Since
	    // they're not referenced anywhere else in the driver, don't
	    // just implicitly link to them. Handle all of the DLL type
	    // stuff here, so simplify error cases.

	    if( ((lpdi->hInstCommon=LoadLibrary("COMMCTRL.DLL")) < HINSTANCE_ERROR) ||
		(!(PSProc=(PROPSHEETPROC)GetProcAddress(lpdi->hInstCommon,
		    SHELL_PROPSHEET))) ||
		(!(CreateProc=(CREATEPROC)GetProcAddress(lpdi->hInstCommon,
		    SHELL_CREATEPAGE))) ||
		(!(InitProc=GetProcAddress(lpdi->hInstCommon,
		    COMMCTRL_INIT))))
	    {
		lpdi->bInitialized=FALSE;
		goto IEDM_exit;
	    }

	    // Since we use COMMCTRL.DLL, we need to call the init
	    // function, in case nobody else has.
	    InitProc();

	    // Time to handle things differently--if this is a call through
	    // ExtDeviceModePropSheets(), pass the pages back through the
	    // callback and return (The shell handles calling our cleanup
	    // function. Otherwise, call PropertySheet() and then call the
	    // cleanup function ourselves.

	    if(lpfnAdd)
	    {
		// Call from ExtDeviceModePropSheet...

		// Set the release function to the first page
		psp[0].pfnCallback=PropPageCallback;
		psp[0].dwFlags |= PSP_USECALLBACK;

		for(wLoop=0;wLoop<wUsed;wLoop++)
		{
		    HPROPSHEETPAGE hPage;

		    if(hPage=CreateProc(psp+wLoop))
			lpfnAdd(hPage,lParam);

		    else
		    {
			// Badness--we're not fully initialized!
			lpdi->bInitialized=FALSE;
			goto IEDM_exit;
		    }
		}

		//  At this point, we're done, so just return success

		return IDOK;
	    }
	    else
	    {
		// Call from ExtDeviceMode or AdvancedSetUpDialog...

		PROPSHEETHEADER psh;
		char            szTitle[MAX_STRING_LENGTH];

		// Zero-init the propsheetheader, build a caption, change
		// the interesting fields, and call PropertySheet.

		_fmemset((LPSTR)&psh,0,sizeof(PROPSHEETHEADER));

		if(bExtDevMode)
		{
		    char szFormat[MAX_STRING_LENGTH];

		    // Build title, including port ("HP LaserJet on LPT1:")

		    LoadString(lpdi->hInst,PROP_CAPTION,szFormat,sizeof(szFormat));
		    // wsprintf(szTitle,szFormat,(LPSTR)lpdi->szDevName,lpdi->szPort);
		    // this is completly ridiculous.  Clears up bug # 16251
		    // wsprintf(szTitle,szFormat,(LPSTR)lpdi->szDevName);
		    _fstrcpy(szFormat, lpdi->szDevName);
		    _fstrcat(szFormat, " on ");
		    _fstrcat(szFormat, lpdi->szPort);
		    _fstrcpy(szTitle, szFormat);
		}
		else
		    // Just use name from devmode ("HP LaserJet")
		    lstrcpy(szTitle,lpdi->szDevName);

		psh.pszCaption=szTitle;
		psh.dwSize=sizeof(PROPSHEETHEADER);
		psh.hwndParent=hWnd;
		psh.ppsp=(LPCPROPSHEETPAGE)psp;
		psh.nPages=wUsed;
		psh.dwFlags=PSH_PROPTITLE|PSH_PROPSHEETPAGE;

		PSProc(&psh);
	    }
	}
    }

IEDM_exit:

    // IEndDevMode will check the bInitialized field of lpdi. If bInitialized
    // is FALSE, we will return -1. If it is TRUE, then we will return either
    // IDOK or IDCANCEL, depending on the flags and what button the user
    // pushed (if we displayed a prompt)

    return IEndDevMode(lpdi,lpProfile);
}

//-------------------------------------------------------------------
// Function: PropPageCallback(hWnd,uMsg,lpPSP)
//
// Action: Callback for the dialog handler. Called when the property
//         pages go away. The assumption is made that the print
//         manager will be polite enough to call us back when it's
//         done.
//-------------------------------------------------------------------
UINT CALLBACK PropPageCallback(HWND            hWnd,
                               UINT            uMsg,
                               LPPROPSHEETPAGE lpPSP)
{
    if(PSPCB_RELEASE==uMsg)
        IEndDevMode((LPDI)(lpPSP->lParam),NULL);

    return 1;
}

//-------------------------------------------------------------------
// Function: IEndDevMode(lpdi,lpProfile)
//
// Action: Handle all cleanup/termination necessary for the dialogs.
//         This can be called either directly or via PropPageCallback,
//         the cleanup function for the dialogs. The end result is the
//         same.
//
// Return: -1       if not initialized (failure)
//         IDCANCEL if DM_PROMPT & cancel was pressed
//         IDOK     otherwise
//-------------------------------------------------------------------
int NEAR PASCAL IEndDevMode(LPDI  lpdi,
                            LPSTR lpProfile)
{
    int nResult=IDOK;   // Let's be optimistic

    if(!lpdi)
    {
	return -1;
    }
    if(lpdi->bInitialized)
    {
	// Don't do this stuff if we couldn't initialize!

	if(lpdi->wMode & DM_PROMPT && !lpdi->bOK)
	{
	    // User canceled changes--revert to dmMerge instead of dmScratch
	    lpdi->lpDM=&lpdi->dmMerge;
	    nResult=IDCANCEL;
	}

	if(lpdi->bExtDevMode && lpdi->wMode & DM_UPDATE)
	{
	DrvSetPrinterData(lpdi->lpDM->dm.dmDeviceName,
            lpProfile?lpProfile:(LPSTR)INT_PD_DEFAULT_DEVMODE,
            REG_BINARY,(LPBYTE)lpdi->lpDM,sizeof(ENVIRONMENT));

	    SendMessage(HWND_BROADCAST,WM_DEVMODECHANGE,NULL,
		       (LPARAM)(LPSTR)lpdi->lpDM->dm.dmDeviceName);
	}

	if ((lpdi->wMode & DM_COPY) && lpdi->lpDMTarget)

	    _fmemcpy((LPSTR)lpdi->lpDMTarget,(LPSTR)lpdi->lpDM,sizeof(ENVIRONMENT));

    }
    else
	nResult=-1;

    // Free everything up

    if(lpdi->hInstShell >= HINSTANCE_ERROR)
	FreeLibrary(lpdi->hInstShell);

    if(lpdi->hInstCommon >= HINSTANCE_ERROR)
	FreeLibrary(lpdi->hInstCommon);

    // Remove any block, if needed
#ifdef SEMAPHORE
    // SetSemaphore();
#endif

    // if(lpdi->aBlock)
       // DeleteAtom(lpdi->aBlock);

#ifdef SEMAPHORE
    // ClearSemaphore();
#endif

    // Finally, free up the memory allocated for structure
    GlobalFreePtr(lpdi);

    return nResult;
}

//*************************************************************
//
//  DeviceMode
//
//  Purpose: Calls ExtDevice mode to bring up DEVMODE dialog
//           box.
//
//
//  Parameters:
//      hWnd
//      hInstance
//      lpDeviceType
//      lpOutputFile
//
//
//  Return: (int FAR PASCAL)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//
//*************************************************************

int FAR PASCAL DeviceMode (hWnd,hInstance,lpDeviceType,lpOutputFile)
HANDLE hWnd,
       hInstance;
LPSTR lpDeviceType,
      lpOutputFile;
{
    return (ExtDeviceMode(hWnd, hInstance, NULL, lpDeviceType, lpOutputFile, NULL, NULL,
			   DM_PROMPT | DM_UPDATE) == IDOK);

} //*** DeviceMode


//--------------------------------------------------------------------------//
// Function: GetDeviceMode(LPDI, LPSTR, LPSTR, LPSTR)
//
// Action:
//          This function retrieves the device initialization information
//          (current device mode) based on the printer minidriver data.
//          It also upgrades from WIN3.1 settings by reading the approriate
//          section from WIN.INI if this has never been done.
//
// Returns: TRUE iff lpdm filled in correctly.
//
// Side Effect:
//          none.
//--------------------------------------------------------------------------//
int NEAR PASCAL GetDeviceMode(lpdm, lpPort, lpFriendly, lpProfile)
LPENVIRONMENT    lpdm;       // pointer to the DIALOGINFO struct
LPSTR            lpPort;     // long pointer to the port name
LPSTR            lpFriendly; // long pointer to the friendly name
LPSTR            lpProfile;  // long pointer to the profile name

{
    char            szDevName[CCHDEVICENAME];
    LPENVIRONMENT   lpdmTemp = NULL;
    int             n;
    LPSTR           lpDevmodeKey=lpProfile?lpProfile:(LPSTR)INT_PD_DEFAULT_DEVMODE;
    DWORD           dwReturn;
    DWORD           dwData;
    DWORD           dwType;
    
    if(DrvGetPrinterData(lpFriendly,(LPSTR)INT_PD_PRINTER_MODEL,&dwType,(LPSTR)szDevName,
        sizeof(szDevName),(LPDWORD)&dwData) != ERROR_SUCCESS)
    {
	    // failed getting the actual device name. Try the friendly name.
	    lstrcpyn((LPSTR)szDevName, lpFriendly, CCHDEVICENAME);
    }

    dwReturn=DrvGetPrinterData(lpFriendly,lpDevmodeKey,&dwType,
        (LPBYTE)lpdm,sizeof(ENVIRONMENT),(LPDWORD)&dwData);
	
// dwType is returning REG_SZ - don't know why, but don't check it...
    if((ERROR_SUCCESS==dwReturn)
//        && (REG_BINARY == dwType)
        && (sizeof(ENVIRONMENT) == dwData)
		&& (lpdm->dm.dmSpecVersion == 0x400)
		&& (lpdm->dm.dmDriverVersion == 100)
		&& (lpdm->dm.dmSize == sizeof(DEVMODE))
		&& (lpdm->dm.dmDriverExtra == (sizeof(ENVIRONMENT) - sizeof(DEVMODE)))
		&& (!lstrcmpi(lpdm->ModelName,(LPSTR)szDevName)) )
    {
	    // got our devmode
	    return TRUE;
    }

    // We didn't get our devmode, so we either got no devmode or someone
    // else's. If we got someone else's we need to get our default, then
    // merge in whatever we got because we want to retain settings when:
    // 1) the user changes the printer driver in the dialog;
    // 2) the user installs a new printer which inadverdently changes the
    //    the driver for a previously installed printer.
    
    if(dwData)  // got some other devmode.  Hold it in
    		    // lpdmTemp, and merge with our default
    {
	    if(lpdmTemp=(LPENVIRONMENT)GlobalAllocPtr(GHND,sizeof(ENVIRONMENT)))
	    {
            _fmemcpy((LPBYTE)lpdmTemp,(LPBYTE)lpdm,
                min(LOWORD(dwReturn),sizeof(ENVIRONMENT)));
	    }
    }

    // compose the profile section name: [device, port]
    // If such a section exists and has never been upgraded before,
    // take these settings into consideration when building the default
    // devmode.

    /* hmm - not working right now - worry about this later
    if (get_profile((LPSTR)szDevName, lpdmTemp, lpPort, NULL))
    {
	    // ok, we've just read in our 3.1 settings.  we
	    // can now just remove the section
	    char  szAppName[MAX_STRING_LENGTH];

	    MakeAppName(szAppName, (LPSTR)szDevName, lpPort, sizeof(szAppName));
	    WriteProfileString(szAppName, NULL, NULL);
    } 
    */


    // initialize to zero for the optimization in ModifyDeviceMode.
    _fmemset((LPSTR)lpdm, 0, sizeof(ENVIRONMENT));

    // set up .dmFields and standard DEVMODE fields.

    // need to keep real name in ModelName field, because friendly
    // name is now in dmDeviceName.
    lstrcpyn((LPSTR)(lpdm->ModelName), (LPSTR)szDevName, CCHDEVICENAME);
    lstrcpyn((LPSTR)(lpdm->dm.dmDeviceName), lpFriendly, CCHDEVICENAME);
    
    // now need to get the correct plotter number...
    
    for (n=0; n<NUM_PLOTTERS; n++)
    {
        if (!lstrcmpi((LPSTR)GetString(COLORPRO+n), lpdm->ModelName))
		{
		    lpdm->Plotter=n;
		    break;
		}
    }

    // call get_defaults (true) to set the paper size correctly.
    // Fixes divide by 0 error in print test page.
    // this really need to be cleaned up....

    get_defaults(lpdm, TRUE);
    
    lpdm->dm.dmSpecVersion = 0x400;
    lpdm->dm.dmDriverVersion = 100;
    lpdm->dm.dmSize = sizeof(DEVMODE);
    lpdm->dm.dmDriverExtra = sizeof(ENVIRONMENT) - lpdm->dm.dmSize;

    lpdm->dm.dmFields=0;
    lpdm->dm.dmFields |= DM_ORIENTATION;

    lpdm->dm.dmOrientation = (lpdm->Orientation == 0) ?
			DMORIENT_PORTRAIT : DMORIENT_LANDSCAPE;

    lpdm->dm.dmFields |= DM_PAPERSIZE;
    
    lpdm->dm.dmFields |= DM_COLOR;
    lpdm->dm.dmColor = DMCOLOR_COLOR;

    lpdm->dm.dmFields |= DM_COPIES;
    lpdm->dm.dmCopies = 1;

    lpdm->dm.dmFields |= DM_DEFAULTSOURCE;
    lpdm->dm.dmDefaultSource = MANUAL + lpdm->PaperFeed;

    lpdm->dm.dmFields |= DM_PRINTQUALITY;
    lpdm->dm.dmPrintQuality = (lpdm->Draft ==1) ? DMRES_DRAFT : DMRES_HIGH;

    if(lpdmTemp)
	    ModifyDeviceMode(lpdm,lpdmTemp);
    else
        get_defaults(lpdm, FALSE);

    DrvSetPrinterData(lpFriendly,lpDevmodeKey,REG_BINARY,(LPBYTE)lpdm,
        sizeof(ENVIRONMENT));

    if(lpdmTemp)
	    GlobalFreePtr(lpdmTemp);

    return TRUE;
}

//*************************************************************
//
//  ModifyDeviceMode
//
//  Purpose: To merge an input DEVMODE with the output DEVMODE
//              
//
//
//  Parameters:
//      lpdmCur
//      lpdmIn
//
//
//  Return: (int NEAR PASCAL)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//
//*************************************************************

int NEAR PASCAL ModifyDeviceMode(lpdmCur, lpdmIn)
LPENVIRONMENT   lpdmCur;
LPENVIRONMENT   lpdmIn;
{
   DWORD dwCommonFields;

   // extract the common fields and check for each potential field.
   // Note that the app doesn't understand DRIVEREXTRA part, but it
   // could get new values via calling AdvancedSetUpDialog().

   dwCommonFields = lpdmCur->dm.dmFields & lpdmIn->dm.dmFields;

   if (dwCommonFields & DM_ORIENTATION)
	  if (lpdmIn->dm.dmOrientation == DMORIENT_PORTRAIT ||
	     lpdmIn->dm.dmOrientation == DMORIENT_LANDSCAPE)
     {
	     lpdmCur->dm.dmOrientation = lpdmIn->dm.dmOrientation;
	lpdmCur->Orientation = lpdmIn->dm.dmOrientation - 1;
     }

   if (dwCommonFields & DM_PAPERSIZE)
	  if ((lpdmIn->dm.dmPaperSize >= SIZE_A) && (lpdmIn->dm.dmPaperSize <= ROLL_36))
	  {
	   lpdmCur->dm.dmPaperSize = lpdmIn->dm.dmPaperSize;
	   lpdmCur->Size =lpdmCur->dm.dmPaperSize - SIZE_A;
	  }

   // change later ???
   if (dwCommonFields & DM_COPIES)
      if (lpdmIn->dm.dmCopies == 1)
	      lpdmCur->dm.dmCopies = lpdmIn->dm.dmCopies;
	      

   if (dwCommonFields & DM_DEFAULTSOURCE)
	  if ((lpdmIn->dm.dmDefaultSource >= MANUAL) && (lpdmIn->dm.dmDefaultSource <= PRELOADED))
	  {
	     lpdmCur->dm.dmDefaultSource = lpdmIn->dm.dmDefaultSource;
	lpdmCur->PaperFeed = lpdmCur->dm.dmDefaultSource - MANUAL;
	  }

   if (dwCommonFields & DM_PRINTQUALITY)
	  if ((lpdmIn->dm.dmPrintQuality == DMRES_HIGH) ||
	 (lpdmIn->dm.dmPrintQuality == DMRES_DRAFT))
	  {
	   lpdmCur->dm.dmPrintQuality = lpdmIn->dm.dmPrintQuality;
	   lpdmCur->Draft = lpdmIn->Draft;
	  }

   // mquinton - added device dependent stuff 4/17/94 My Birthday

   if ((lpdmIn->dm.dmSpecVersion == 0x400)
	&& (lpdmIn->dm.dmDriverVersion == 100)
	&& (lpdmIn->dm.dmSize == sizeof(DEVMODE))
	&& (lpdmIn->dm.dmDriverExtra == (sizeof(ENVIRONMENT) - sizeof(DEVMODE))))

   {
   // hopefully one of our devmodes.  the app doesn't know about our
   // extra fields, but could have gotten and updated environment

	int i;
	
	lpdmCur->Plotter                   = lpdmIn->Plotter;
	lpdmCur->CurrentCarousel           = lpdmIn->CurrentCarousel;
	for (i=0; i<MAX_CAROUSELS; i++)
		lpdmCur->Carousel[i]       = lpdmIn->Carousel[i];
	for (i=0; i<6; i++)
		lpdmCur->ActiveCarousel[i] = lpdmIn->ActiveCarousel[i];
	lpdmCur->Draft                     = lpdmIn->Draft;
   }
   else
   
   // lpdmIn isn't one of our devicemodes.  Set device dependent info
   // to default.
        get_defaults(lpdmCur, FALSE);
  
   return 1;

} //*** ModifyDeviceMode

/*** EOF: devmode.c ***/
