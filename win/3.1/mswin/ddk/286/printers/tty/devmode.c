/*/   DEVMODE.C for TTY
 +-----------------------------------------+
 | Fralc Consultores (c), Mexico 1986-1989 |
 +-----------------------------------------+
/*/

// ------------------------------------------------------------------------
// Copyright (C) 1989-1990, Microsoft Corporation. All rights reserved.
// ------------------------------------------------------------------------
//	Microsoft History (latest first)
//	29 nov 89	peterbe		removed def of MB_ICONQUESTION
//	20 oct 89	peterbe		Checked in.
// ------------------------------------------------------------------------

#include <ctype.h>

#include "generic.h"

#define WHITE_BRUSH	0
HANDLE API GetStockObject(int);

#define MB_DEFBUTTON2	0x0100

HANDLE	hInst = NULL;
int editing;
LPDM lpDevmode;

extern OFSTRUCT os;

BOOL FAR PASCAL	    DialogFn(HWND, unsigned, WORD, LONG);
void FAR PASCAL     GetDevMode(LPSTR, LPDM, LPSTR);
LONG FAR PASCAL	    PseudoEdit( HWND, unsigned, WORD, DWORD);

short GetProfileString2(
    LPSTR   lpSeccion,
    LPSTR   lpLlave,
    LPSTR   lpDefault,
    LPSTR   lpSalida,
    WORD    wNumero,
    LPSTR   lpProfile
    )
{
    if(lpProfile)
	return GetPrivateProfileString(lpSeccion, lpLlave, lpDefault,
		lpSalida, wNumero, lpProfile);
    else
	return GetProfileString(lpSeccion, lpLlave, lpDefault,
		lpSalida, wNumero);

}	// GetProfileString2()

/* mezcla dos devmodes de acuerdo al campo fields */
void near pascal MergeDevMode(
    LPDM lpDest,
    LPDM lpSrc)   /* lpSrc debe ser valido de todas todas ... */
{
    long Fields = lpSrc->dm.dmFields;

    if (Fields & DM_COPIES)
	lpDest->dm.dmCopies = 1;

    if (Fields & DM_PRINTQUALITY)
	lpDest->dm.dmPrintQuality = 0;

    if (Fields & DM_DUPLEX)
	lpDest->dm.dmDuplex = 0;

    if (Fields & DM_PAPERSIZE){
	short i;

	lpDest->dm.dmPaperSize = lpSrc->dm.dmPaperSize;
	lpDest->paper = PaperID2Index(lpDest->dm.dmPaperSize);
    }

    if (Fields & DM_DEFAULTSOURCE){
	lpDest->dm.dmDefaultSource = lpSrc->dm.dmDefaultSource;
	lpDest->feed = lpSrc->feed;
    }

    if (Fields & DM_WIDE)
	lpDest->use_wide_carriage = lpSrc->use_wide_carriage;

    if (Fields & DM_PAGEBREAK)
	lpDest->page_break = lpSrc->page_break;
}

short WriteProfileString2(
    LPSTR   lpSeccion,
    LPSTR   lpLlave,
    LPSTR   lpValor,
    LPSTR   lpProfile
){
    if(lpProfile)
	return WritePrivateProfileString(lpSeccion, lpLlave, lpValor,
		lpProfile);
    else
	return WriteProfileString(lpSeccion, lpLlave, lpValor);
}

void NEAR PASCAL WriteDevMode(
    LPSTR	lpDeviceName,
    LPDM  lpDevmode,
    LPSTR	lpProfile)
{
    char    lpLlave[15];
    char    lpValor[15];

    /* escribir en WIN.INI */

    LoadString(hInst, INI_PAPER, lpLlave, 15);
    LoadString(hInst, PAP_LETTER+lpDevmode->paper, lpValor, 15);
    WriteProfileString2(lpDeviceName, lpLlave, lpValor, lpProfile);

    LoadString(hInst, INI_WIDE, lpLlave, 15);
    LoadString(hInst, STR_NO+lpDevmode->use_wide_carriage, lpValor, 15);
    WriteProfileString2(lpDeviceName, lpLlave, lpValor, lpProfile);

    LoadString(hInst, INI_CUT, lpLlave, 15);
    LoadString(hInst, STR_NO+(lpDevmode->feed == CUT), lpValor, 15);
    WriteProfileString2(lpDeviceName, lpLlave, lpValor, lpProfile);

    LoadString(hInst, INI_BREAK, lpLlave, 15);
    LoadString(hInst, STR_NO+lpDevmode->page_break, lpValor, 15);
    WriteProfileString2(lpDeviceName, lpLlave, lpValor, lpProfile);
}

short FAR PASCAL ExtDeviceMode(
    HANDLE	hWnd,		// Handle de la ventana padre (of parent wind.)
    HANDLE	hInst,		// Identifica el modulo del driver (module ID)
    LPDM  lpDevModeOutput,// El driver escribe la inf. de ini.
				//   en esta estructura -- the driver writes
				// init. informatio in this structure.
    LPSTR	lpDeviceName,	// apunta a un string con el nombre del driver
				// pointer to a string with the device name.
    LPSTR	lpPort,		// apunta a un str. que contiene el nombre del
                                //   puerto conectado. ej "LPT1:" -- pointer to
				// string containing the current port, e.g.
				// "LPT1:".
    LPDM  lpDevModeInput, // Apunta a la estruct. que se recibe para la
                                //  inicializacion  -- ptr. to the struc. which
				// receives the init.
    LPSTR	lpProfile,	// Apunta a un str. que tiene el nombre del
                                // archivo de inicializacion.si NULL el default
                                // es WIN.INI -- ptr to str. containing name
				// of initialization file: if NULL, defaults
				// to WIN.INI.
    WORD	wMode )		// Tipo de operacion a realizar. Si esta es 0
                                // retorna el taman~o de Extended DEVMODE.
				// Type of operation to realize. If 0,
				// returns the size for Extended DEVMODE.
{
    short	iExclusivo;
    short	iRc;
    static BOOL bDevModeBusy = FALSE;
    HANDLE	hDevmode;
    char	szRealName[40];

    if(!wMode)
	return sizeof(EXDEVMODE);

    // Asegurarse que no hemos hecho 2 devmodes para este driver en las
    // porciones no reentrantes.
    // Make sure that we haven't made 2 devmode [calls] to the this driver
    // in the non-reentrant portions.


    iExclusivo = wMode & (DM_UPDATE | DM_PROMPT);

    if (bDevModeBusy) {
	if(iExclusivo){
	    return SP_ERROR;
	}
    } else
	bDevModeBusy = iExclusivo;

	hInst = hInst;

    iRc = SP_OUTOFMEMORY; /* asumimos que no hay memoria */

    if( hDevmode = GlobalAlloc(GMEM_MOVEABLE|GMEM_ZEROINIT,
		(DWORD)sizeof(EXDEVMODE)) ){
	if(!(lpDevmode = (LPDM)GlobalLock(hDevmode)) )
	    goto ExtExitFree;
    } else
	goto ExtExit;

    /* tratar de tomar devmode del ambiente */
    // try to get devmode from the environment

    /* si existe un archivo de inicializacion diferente de "WIN.INI" o
       si no se puede tomar (encontrar) devmode del ambiente o
       si el nombre del ambiente es igual al dado en el parametro
       Tomar devmode de WIN.INI */

    // if an init. file different from WIN.INI exists, or if it's not
    // possible to get devmode from the environment, or if the
    // name of the environment is that same as that given in the parameter,
    // get devmode from WIN.INI


    if(lpProfile ||
      !GetEnvironment(lpPort, (LPSTR) lpDevmode, sizeof(EXDEVMODE) ) ||
      lstrcmp((LPSTR)lpDeviceName, (LPSTR)lpDevmode->dm.dmDeviceName) )
	GetDevMode((LPSTR)lpDeviceName, lpDevmode , lpProfile );

    if(wMode & DM_MODIFY && lpDevModeInput )
	MergeDevMode(lpDevmode, lpDevModeInput);

    LoadString(hInst, IDS_MSG_CAPTION, szRealName, 40);
    if(wMode & DM_PROMPT &&
		!lstrcmp((LPSTR)szRealName,
			 (LPSTR)lpDevmode->dm.dmDeviceName)){

	iRc = DialogBox(hInst, MAKEINTRESOURCE(100), hWnd, (FARPROC)DialogFn);
    } else
	iRc = IDOK;  /* No llamar a Dialog pero hay que retornar IDOK */

    /* Si el que llama quiere una copia de el ambiente resultante, darsela. */

    if( (wMode & DM_COPY) && lpDevModeOutput )
	Copy( (LPSTR)lpDevModeOutput, (LPSTR)lpDevmode, sizeof(EXDEVMODE) );

    if( (wMode & DM_UPDATE) && iRc == IDOK ){
	SetEnvironment(lpPort, (LPSTR) lpDevmode, sizeof(EXDEVMODE));
	WriteDevMode(lpDeviceName, lpDevmode, lpProfile);
	SendMessage(0xFFFF, WM_DEVMODECHANGE, 0, (LONG)(LPSTR)lpDeviceName);
    }

    GlobalUnlock(hDevmode); /* liberalo */

ExtExitFree:
    GlobalFree(hDevmode);

ExtExit:
    if(iExclusivo)
	bDevModeBusy = FALSE;

    return iRc;
}

/* DeviceMode cambio para que ahora llame a ExtDevMode */

short FAR PASCAL DeviceMode(
    HANDLE hWnd,	    /* Handle de la ventana padre */
				// handle of parent window
    HANDLE hInst,	    /* El module handle del driver DLL */
				// driver DLL mod. handle
    LPSTR lpDeviceName,	    /* apunta a un string con el nombre del driver */
				// pter. to driver name string
    LPSTR lpPort )	    /* apunta al .puerto de salida */
				// ptr to output port name string
{
    return ExtDeviceMode(hWnd, hInst, NULL, lpDeviceName, lpPort, NULL, NULL,
			DM_PROMPT | DM_UPDATE ) == IDOK;
}


BOOL FAR PASCAL PseudoInit(
    HANDLE hInstance)
{
    WNDCLASS   clase;

    hInst	   = hInstance;
    clase.style		= CS_HREDRAW | CS_VREDRAW;
    clase.lpfnWndProc	= (long (far *)())PseudoEdit;
    clase.cbClsExtra	= 0;
    clase.cbWndExtra	= 8;
    clase.hInstance	= hInstance;
    clase.hIcon		= NULL;
    clase.hCursor	= LoadCursor( NULL, IDC_IBEAM);
    clase.hbrBackground = (HBRUSH)GetStockObject( WHITE_BRUSH );
    clase.lpszMenuName	= NULL;
    clase.lpszClassName = "PEdit";

    return RegisterClass( (LPWNDCLASS)&clase );
}

void FAR PASCAL GetDevMode(
    LPSTR	    lpDeviceType,
    LPDM      lpDevMode,
    LPSTR	    lpProfile
)
{
    char    lpLlave[15];
    char    lpDefault[15];
    char    lpValor[15];
    char    lpTalves[15];
    int	    i;

    /* cargar de WIN.INI */

    BuildDevMode(lpDevMode, lpDeviceType);

    LoadString(hInst, INI_PAPER, (LPSTR)lpLlave, 15);
    LoadString(hInst, PAP_DEF, (LPSTR)lpDefault, 15);
    GetProfileString2(lpDeviceType, lpLlave, lpDefault, lpValor, 15, lpProfile);
    for(i = PAP_FIRST; i<= PAP_LAST; i++)
	{
	LoadString(hInst, i, (LPSTR)lpTalves, 15);
	if(!lstrcmpi(lpValor, lpTalves))
	    {
	    lpDevMode->paper = i-PAP_FIRST+LETTER;
	    lpDevMode->dm.dmPaperSize =
		PaperFormat[lpDevMode->paper-LETTER].code;
	    break;
	    }
	}
    if( i > PAP_LAST )
	{
	lpDevMode->paper = LETTER;
	lpDevMode->dm.dmPaperSize = DMPAPER_LETTER;
	}

    LoadString(hInst, INI_WIDE, (LPSTR)lpLlave, 15);
    LoadString(hInst, STR_NO, (LPSTR)lpDefault, 15);
    LoadString(hInst, STR_YES, (LPSTR)lpTalves, 15);
    GetProfileString2(lpDeviceType, lpLlave, lpDefault, lpValor, 15, lpProfile);
    lpDevMode->use_wide_carriage = !lstrcmpi(lpValor, lpTalves);

    LoadString(hInst, INI_CUT, (LPSTR)lpLlave, 15);
    LoadString(hInst, STR_NO, (LPSTR)lpDefault, 15);
    LoadString(hInst, STR_YES, (LPSTR)lpTalves, 15);
    GetProfileString2(lpDeviceType, lpLlave, lpDefault, lpValor, 15, lpProfile);
    if(!lstrcmpi(lpValor, lpTalves))
	{
	lpDevMode->feed = CUT;
	lpDevMode->dm.dmDefaultSource = DMBIN_MANUAL;
	}
    else
	{
	lpDevMode->feed = CONTINUOUS;
	lpDevMode->dm.dmDefaultSource = DMBIN_TRACTOR;
	}

    LoadString(hInst, INI_BREAK, (LPSTR)lpLlave, 15);
    LoadString(hInst, STR_YES, (LPSTR)lpDefault, 15);
    LoadString(hInst, STR_NO, (LPSTR)lpTalves, 15);
    GetProfileString2(lpDeviceType, lpLlave, lpDefault, lpValor, 15, lpProfile);
    lpDevMode->page_break = lstrcmpi(lpValor, lpTalves);

    lstrcpy((LPSTR)(lpDevMode->dm.dmDeviceName), (LPSTR)lpDeviceType);

}	// GetDevMode()

/* construye un devmode con los defaults */
// constructs a devmode structure with default values.
void NEAR PASCAL BuildDevMode(
    LPDM  lpDevmode,
    LPSTR	lpDevice
)
{
    lstrcpy(lpDevmode->dm.dmDeviceName, lpDevice);
    lpDevmode->dm.dmSpecVersion	 = DEV_WINVERSION;
    lpDevmode->dm.dmDriverVersion= DEV_MIVERSION;
    lpDevmode->dm.dmSize	 = sizeof(DEVMODE);
    lpDevmode->dm.dmDriverExtra  = sizeof(EXDEVMODE) - sizeof(DEVMODE);
    lpDevmode->dm.dmFields	 = DM_PAPERSIZE | DM_DEFAULTSOURCE |
				   DM_WIDE | DM_PAGEBREAK;
    lpDevmode->dm.dmOrientation  = DMORIENT_PORTRAIT;
    lpDevmode->dm.dmPaperSize	 = DMPAPER_LETTER;
    lpDevmode->dm.dmDefaultSource= DMBIN_TRACTOR;
    lpDevmode->feed		 = CONTINUOUS;
    lpDevmode->paper		 = LETTER;
    lpDevmode->use_wide_carriage = FALSE;
    lpDevmode->page_break	 = TRUE;
}

#if 0
short near lstricmp(LPSTR a, LPSTR b)
{
    int ac, bc;

    for (; toupper(*a) == toupper(*b); a++,b++)
	;
    return *a - *b;
}
#endif

short	FAR PASCAL PaperID2Index(id)
short id;
{
    short i;

    // Need to map from ID to index
    for (i = 0; i < MAXPAPER; i++)
	if (PaperFormat[i].code == id)
	    return i;
}

