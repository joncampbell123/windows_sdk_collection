/* Driver for the HP7475A plotter. */

#include    "plotters.h"
#include    "hp7475A.h"


LPSTR	far PASCAL lstrcpy( LPSTR, LPSTR );
extern	GDIINFO hp747xAGDIInfo;


MODEBLOCK   hp7475AModeBlock;
BOOL	    hp7475ADevModeBusy = FALSE;


BOOL FAR PASCAL HP7475ADialogFn(HWND, unsigned, WORD, DWORD);


extern	DWORD	hp747xAPenColors[];



INTEGER FAR PASCAL
hp7475ASetupEnable(lpPDevice, lpData)
LPPDEVICE	    lpPDevice;
LPSTR		    lpData;
{

    /* Set up the device dependent parts of GDIINFO. */

    /* Number of brushes the device has. */
    hp747xAGDIInfo.dpNumBrushes = NUMPENCOLORS;
    /* Number of pens the device has. */
    hp747xAGDIInfo.dpNumPens = NUMPENSTYLES*NUMPENCOLORS;
    /* Number of colors in color table. */
    hp747xAGDIInfo.dpNumColors = NUMPENCOLORS;
    /* Length of segment for line styles. */
    hp747xAGDIInfo.dpStyleLen = (500/RESOLUTION)*10/8;

    /* Logical pixels/inch in X. */
    hp747xAGDIInfo.dpLogPixelsX = (STEPSPERMM*254)/(10*RESOLUTION);
    /* Logical pixels/inch in Y. */
    hp747xAGDIInfo.dpLogPixelsY = (STEPSPERMM*254)/(10*RESOLUTION);


    /* Use lpData if it's there. */
    if(lpData){
	switch (((LPMODEBLOCK)lpData)->PaperType){

	    case APAPER:
		hp747xAGDIInfo.dpHorzSize = XAPHYSICALUNITS/STEPSPERMM;
		hp747xAGDIInfo.dpVertSize = YAPHYSICALUNITS/STEPSPERMM;
		hp747xAGDIInfo.dpHorzRes  = XAPHYSICALUNITS/RESOLUTION;
		hp747xAGDIInfo.dpVertRes  = YAPHYSICALUNITS/RESOLUTION;

		if(lpPDevice){
		    lpPDevice->Header.PageSize.xcoord = (11*STEPSPERMM*254)/(10*RESOLUTION);
		    lpPDevice->Header.PageSize.ycoord = (85*STEPSPERMM*254)/(10*RESOLUTION*10);

		    lpPDevice->Header.LandPageOffset.xcoord = 40;
		    lpPDevice->Header.LandPageOffset.ycoord = 56;
		    lpPDevice->Header.PortPageOffset.xcoord = 56;
		    lpPDevice->Header.PortPageOffset.ycoord = 0; /* ???????????@@@@@????????????? */
		}
		break;

	    case BPAPER:
		hp747xAGDIInfo.dpHorzSize = XBPHYSICALUNITS/STEPSPERMM;
		hp747xAGDIInfo.dpVertSize = YBPHYSICALUNITS/STEPSPERMM;
		hp747xAGDIInfo.dpHorzRes  = XBPHYSICALUNITS/RESOLUTION;
		hp747xAGDIInfo.dpVertRes  = YBPHYSICALUNITS/RESOLUTION;

		if(lpPDevice){
		    lpPDevice->Header.PageSize.xcoord = (17*STEPSPERMM*254)/(10*RESOLUTION);
		    lpPDevice->Header.PageSize.ycoord = (11*STEPSPERMM*254)/(10*RESOLUTION);

		    lpPDevice->Header.LandPageOffset.xcoord = 56;
		    lpPDevice->Header.LandPageOffset.ycoord = 45;
		    lpPDevice->Header.PortPageOffset.xcoord = 45;
		    lpPDevice->Header.PortPageOffset.ycoord = 0; /* ???????????@@@@@????????????? */
		}
		break;

	    case A3PAPER:
		hp747xAGDIInfo.dpHorzSize = XA3PHYSICALUNITS/STEPSPERMM;
		hp747xAGDIInfo.dpVertSize = YA3PHYSICALUNITS/STEPSPERMM;
		hp747xAGDIInfo.dpHorzRes  = XA3PHYSICALUNITS/RESOLUTION;
		hp747xAGDIInfo.dpVertRes  = YA3PHYSICALUNITS/RESOLUTION;

		if(lpPDevice){
		    lpPDevice->Header.PageSize.xcoord = (420*STEPSPERMM)/RESOLUTION;
		    lpPDevice->Header.PageSize.ycoord = (297*STEPSPERMM)/RESOLUTION;

		    lpPDevice->Header.LandPageOffset.xcoord = 56;
		    lpPDevice->Header.LandPageOffset.ycoord = 45;
		    lpPDevice->Header.PortPageOffset.xcoord = 45;
		    lpPDevice->Header.PortPageOffset.ycoord = 0; /* ???????????@@@@@????????????? */
		}
		break;

	    case A4PAPER:
		hp747xAGDIInfo.dpHorzSize = XA4PHYSICALUNITS/STEPSPERMM;
		hp747xAGDIInfo.dpVertSize = YA4PHYSICALUNITS/STEPSPERMM;
		hp747xAGDIInfo.dpHorzRes  = XA4PHYSICALUNITS/RESOLUTION;
		hp747xAGDIInfo.dpVertRes  = YA4PHYSICALUNITS/RESOLUTION;

		if(lpPDevice){
		    lpPDevice->Header.PageSize.xcoord = (297*STEPSPERMM)/RESOLUTION;
		    lpPDevice->Header.PageSize.ycoord = (210*STEPSPERMM)/RESOLUTION;

		    lpPDevice->Header.LandPageOffset.xcoord = 40;
		    lpPDevice->Header.LandPageOffset.ycoord = 56;
		    lpPDevice->Header.PortPageOffset.xcoord = 56;
		    lpPDevice->Header.PortPageOffset.ycoord = 0; /* ???????????@@@@@????????????? */
		}
		break;
	}

	if(lpPDevice){
	    lpPDevice->Colors[0] = 0x00FFFFFF;
	    lpPDevice->Colors[1] = (((LPMODEBLOCK)lpData)->PenColors[0]);
	    lpPDevice->Colors[2] = (((LPMODEBLOCK)lpData)->PenColors[1]);
	    lpPDevice->Colors[3] = (((LPMODEBLOCK)lpData)->PenColors[2]);
	    lpPDevice->Colors[4] = (((LPMODEBLOCK)lpData)->PenColors[3]);
	    lpPDevice->Colors[5] = (((LPMODEBLOCK)lpData)->PenColors[4]);
	    lpPDevice->Colors[6] = (((LPMODEBLOCK)lpData)->PenColors[5]);
	}
    }
    else {
	hp747xAGDIInfo.dpHorzSize = XAPHYSICALUNITS/STEPSPERMM;
	hp747xAGDIInfo.dpVertSize = YAPHYSICALUNITS/STEPSPERMM;
	hp747xAGDIInfo.dpHorzRes  = XAPHYSICALUNITS/RESOLUTION;
	hp747xAGDIInfo.dpVertRes  = YAPHYSICALUNITS/RESOLUTION;

	if(lpPDevice){
	    lpPDevice->Colors[0] = 0x00FFFFFF;
	    lpPDevice->Colors[1] = 0x000000FF;
	    lpPDevice->Colors[2] = 0x0000FF00;
	    lpPDevice->Colors[3] = 0x00FF0000;
	    lpPDevice->Colors[4] = 0x00FF00FF;
	    lpPDevice->Colors[5] = 0x00FFFF00;
	    lpPDevice->Colors[6] = 0x00000000;

	    lpPDevice->Header.PageSize.xcoord = (11*STEPSPERMM*254)/(10*RESOLUTION);
	    lpPDevice->Header.PageSize.ycoord = (85*STEPSPERMM*254)/(10*RESOLUTION*10);

	    lpPDevice->Header.LandPageOffset.xcoord = 40;
	    lpPDevice->Header.LandPageOffset.ycoord = 56;
	    lpPDevice->Header.PortPageOffset.xcoord = 56;
	    lpPDevice->Header.PortPageOffset.ycoord = 0; /* ???????????@@@@@????????????? */
	}
    }

    if(lpPDevice){
	lpPDevice->Header.OutFileOffset = (WORD)(lpPDevice->OutputFile) - (WORD)lpPDevice;
    }

    return(sizeof(PDEVICE));
}



INTEGER FAR PASCAL
hp7475AInitDevice(lpPDevice, InitAll)
LPPDEVICE	    lpPDevice;
BOOL		    InitAll;
{
    INTEGER	Args[4];

    lpPDevice->HeightNumerator = 48;
    lpPDevice->HeightDenominator = 61;
    lpPDevice->WidthNumerator = 2;
    lpPDevice->WidthDenominator = 3;

    if(!InitAll) return;

    SendString((LPPDEVICEHEADER)lpPDevice, (LPSTR)"\03;IN;IN;IM0;", 12);

    if(lpPDevice->Header.PenSpeed == SLOWSPEED){
	SendString((LPPDEVICEHEADER)lpPDevice, (LPSTR)"VS10;", 5);
    }
    else {
	SendString((LPPDEVICEHEADER)lpPDevice, (LPSTR)"VS;", 3);
    }

    if(lpPDevice->Header.Orientation == LANDSCAPE){
	SendString((LPPDEVICEHEADER)lpPDevice, (LPSTR)"DI;", 3);
    }
    else {
	SendString((LPPDEVICEHEADER)lpPDevice, (LPSTR)"DI0,-1;", 7);
    }

    /* Set the paper size. */
    switch (lpPDevice->Header.HorzRes){

	case XAPHYSICALUNITS/RESOLUTION:
	case XA4PHYSICALUNITS/RESOLUTION:
	    SendString((LPPDEVICEHEADER)lpPDevice, (LPSTR)"PS4;", 4);
	    break;

	case XBPHYSICALUNITS/RESOLUTION:
	case XA3PHYSICALUNITS/RESOLUTION:
	    SendString((LPPDEVICEHEADER)lpPDevice, (LPSTR)"PS3;", 4);
	    break;
    }

    Args[0] = 0;
    Args[1] = 0;
    Args[2] = (lpPDevice->Header.HorzRes) * RESOLUTION;
    Args[3] = (lpPDevice->Header.VertRes) * RESOLUTION;

    HPSendCommand((LPPDEVICEHEADER)lpPDevice, (LPSTR)"IW", (LPINT)Args, 4, 0, 0);

    HPSendCommand((LPPDEVICEHEADER)lpPDevice, (LPSTR)"IP", (LPINT)Args, 4, 0, 0);

    Args[1] = (lpPDevice->Header.HorzRes);
    Args[2] = 0;
    Args[3] = (lpPDevice->Header.VertRes);

    HPSendCommand((LPPDEVICEHEADER)lpPDevice, (LPSTR)"SC", (LPINT)Args, 4, 0, 0);

    SendString((LPPDEVICEHEADER)lpPDevice, (LPSTR)"PU;SP0;PA0,0;", 13);
}



INTEGER FAR PASCAL
hp7475ADeviceMode(hWnd, hInstance, lpDeviceType, lpOutputFile)
HANDLE		    hWnd;
HANDLE		    hInstance;
LPSTR		    lpDeviceType;
LPSTR		    lpOutputFile;
{

    /* make sure we don't do two devmodes for this driver at once */
    if(hp7475ADevModeBusy)
	return(FALSE);
    else
	hp7475ADevModeBusy = TRUE;

    /* default */
    if(!GetEnvironment(lpOutputFile, (LPSTR)&hp7475AModeBlock, sizeof(hp7475AModeBlock))
	|| lstrcmp((LPSTR)lpDeviceType, (LPSTR)&hp7475AModeBlock)){
#ifdef SPOOLING
	hp7475AModeBlock.Spooling = TRUE;
#endif
	hp7475AModeBlock.PenSpeed = FASTSPEED;
	hp7475AModeBlock.Orientation = LANDSCAPE;
	hp7475AModeBlock.PaperType = APAPER;

	hp7475AModeBlock.PenColors[0] = hp747xAPenColors[hp7475AModeBlock.PenIndexes[0] = REDPEN - BLACKPEN];

	hp7475AModeBlock.PenColors[1] = hp747xAPenColors[hp7475AModeBlock.PenIndexes[1] = GREENPEN - BLACKPEN];

	hp7475AModeBlock.PenColors[2] = hp747xAPenColors[hp7475AModeBlock.PenIndexes[2] = BLUEPEN - BLACKPEN];

	hp7475AModeBlock.PenColors[3] = hp747xAPenColors[hp7475AModeBlock.PenIndexes[3] = VIOLETPEN - BLACKPEN];

	hp7475AModeBlock.PenColors[4] = hp747xAPenColors[hp7475AModeBlock.PenIndexes[4] = TURQUOISEPEN - BLACKPEN];

	hp7475AModeBlock.PenColors[5] = hp747xAPenColors[hp7475AModeBlock.PenIndexes[5] = BLACKPEN - BLACKPEN];
    }

    if(DialogBox(hInstance, (LPSTR)"HP7475AdtMODE", hWnd,(FARPROC)HP7475ADialogFn) == IDCANCEL){
	hp7475ADevModeBusy = FALSE;
	return(FALSE);
    }

    lstrcpy((LPSTR)&hp7475AModeBlock, (LPSTR)"HP 7475A");
    SetEnvironment(lpOutputFile, (LPSTR)&hp7475AModeBlock, sizeof(hp7475AModeBlock));

    SendMessage((HWND)(-1), WM_DEVMODECHANGE, 0, (DWORD)(LPSTR)lpDeviceType);

    hp7475ADevModeBusy = FALSE;
    return(TRUE);
}



/* A menu item has been selected, or a control is notifying
** its parent.	wParam is the menu item value (for menus),
** or control ID (for controls).  For controls, the low word
** of lParam has the window handle of the control, and the hi
** word has the notification code.  For menus, lParam contains
** 0L. */

BOOL FAR PASCAL HP7475ADialogFn(hDB, message, wParam, lParam)
HWND		    hDB;
unsigned	    message;
WORD		    wParam;
DWORD		    lParam;
{
    char	Buffer[16];
    INTEGER	id;

    switch(message){

	case WM_INITDIALOG:
	    hp7475AModeBlock.CurPenID = PEN1BUTTON;
	    hp7475AModeBlock.CurColorID =
				hp7475AModeBlock.PenIndexes[0] + BLACKPEN;
	    CheckDlgButton(hDB, hp7475AModeBlock.PaperType, TRUE);
	    CheckDlgButton(hDB, hp7475AModeBlock.PenSpeed, TRUE);
	    CheckDlgButton(hDB, hp7475AModeBlock.Orientation, TRUE);
	    CheckDlgButton(hDB, hp7475AModeBlock.CurPenID, TRUE);
	    CheckDlgButton(hDB, hp7475AModeBlock.CurColorID, TRUE);

	    for(id = PEN1COLOR; id <= PEN6COLOR; id++){
		GetDlgItemText(hDB, hp7475AModeBlock.PenIndexes[id-PEN1COLOR] + BLACKPEN, (LPSTR)Buffer, 16);
		SetDlgItemText(hDB, id, (LPSTR)Buffer);
	    }

#ifdef SPOOLING
/* ---------------------------------------------------------------------------
	    CheckDlgButton(hDB, (hp7475AModeBlock.Spooling ? SETSPOOLING : RESETSPOOLING), TRUE);

--------------------------------------------------------------------------- */
#endif

	    SetFocus(GetDlgItem(hDB, PEN1BUTTON));
	    return(FALSE);
	    break;

	case WM_COMMAND:
	    switch (wParam){

#ifdef SPOOLING
/* ---------------------------------------------------------------------------

		case SETSPOOLING:
		case RESETSPOOLING:
		    CheckDlgButton(hDB, (hp7475AModeBlock.Spooling ? SETSPOOLING :RESETSPOOLING),FALSE);

		    CheckDlgButton(hDB, wParam, TRUE);

		    hp7475AModeBlock.Spooling = (wParam == SETSPOOLING);

		    break;
--------------------------------------------------------------------------- */
#endif

		case SLOWSPEED:
		case FASTSPEED:
		    CheckDlgButton(hDB,hp7475AModeBlock.PenSpeed,FALSE);

		    CheckDlgButton(hDB,hp7475AModeBlock.PenSpeed = wParam, TRUE);

		    break;

		case LANDSCAPE:
		case PORTRAIT:
		    CheckDlgButton(hDB, hp7475AModeBlock.Orientation, FALSE);

		    CheckDlgButton(hDB, hp7475AModeBlock.Orientation = wParam, TRUE);

		    break;

		case APAPER:
		case BPAPER:
		case A3PAPER:
		case A4PAPER:
		    CheckDlgButton(hDB, hp7475AModeBlock.PaperType, FALSE);

		    CheckDlgButton(hDB, hp7475AModeBlock.PaperType = wParam, TRUE);

		    break;

		case PEN1BUTTON:
		case PEN2BUTTON:
		case PEN3BUTTON:
		case PEN4BUTTON:
		case PEN5BUTTON:
		case PEN6BUTTON:

		    CheckDlgButton(hDB, hp7475AModeBlock.CurPenID, FALSE);

		    CheckDlgButton(hDB, hp7475AModeBlock.CurPenID = wParam, TRUE);

		    CheckDlgButton(hDB, hp7475AModeBlock.CurColorID, FALSE);

		    hp7475AModeBlock.CurColorID = hp7475AModeBlock.PenIndexes[hp7475AModeBlock.CurPenID-PEN1BUTTON] + BLACKPEN;

		    CheckDlgButton(hDB, hp7475AModeBlock.CurColorID, TRUE);

		    break;

		case BLACKPEN:
		case REDPEN:
		case GREENPEN:
		case BLUEPEN:
		case YELLOWPEN:
		case VIOLETPEN:
		case TURQUOISEPEN:
		case ORANGEPEN:
		case BROWNPEN:

		    hp7475AModeBlock.PenIndexes[hp7475AModeBlock.CurPenID-PEN1BUTTON] = wParam - BLACKPEN;

		    hp7475AModeBlock.PenColors[hp7475AModeBlock.CurPenID-PEN1BUTTON] = hp747xAPenColors[wParam - BLACKPEN];

		    CheckDlgButton(hDB, hp7475AModeBlock.CurColorID, FALSE);

		    CheckDlgButton(hDB, hp7475AModeBlock.CurColorID = wParam, TRUE);

		    GetDlgItemText(hDB, hp7475AModeBlock.CurColorID, (LPSTR)Buffer, 16);

		    SetDlgItemText(hDB, hp7475AModeBlock.CurPenID - PEN1BUTTON + PEN1COLOR, (LPSTR)Buffer);

		    break;

		case IDOK:
		    EndDialog(hDB, IDOK);
		    break;

		case IDCANCEL:
		    EndDialog(hDB, IDCANCEL);
		    break;

		default:
		    return(FALSE);
		    break;
	    }
	return(TRUE);
	break;
    }

    return(FALSE);
}
