/*/   DEVCAPS.C for TTY
 +-----------------------------------------+
 | Fralc Consultores (c), Mexico 1986-1989 |
 +-----------------------------------------+
/*/

// ------------------------------------------------------------------------
// Copyright (C) 1989-1990, Microsoft Corporation. All rights reserved.
// ------------------------------------------------------------------------
//	Microsoft History (latest first)
//	20 oct 89	peterbe		Checked in.
// ------------------------------------------------------------------------

#include "generic.h"

typedef WORD FAR * LPWORD;

extern HANDLE hInstancia;

/* checa que un devmode sea el apropiado por medio de los datos internos */
// checks whether a devmode structure is appropriate.
BOOL far pascal CheckDevMode(
    LPDM   lpDevmode
)
{
    if(lpDevmode->dm.dmSpecVersion != DEV_WINVERSION)
	return FALSE;
    if(lpDevmode->dm.dmDriverVersion != DEV_MIVERSION)
	return FALSE;
    if(lpDevmode->dm.dmSize != sizeof(DEVMODE))
	return FALSE;
    if(lpDevmode->dm.dmDriverExtra != (sizeof(EXDEVMODE) - sizeof(DEVMODE)))
	return FALSE;

    lpDevmode->paper = PaperID2Index(lpDevmode->dm.dmPaperSize);

    if(lpDevmode->dm.dmDefaultSource == DMBIN_MANUAL)
	lpDevmode->feed = CUT;
    else
	lpDevmode->feed = CONTINUOUS;
    return TRUE;
}


/*/

    DeviceCapabilities

    This function returns the capibilities of the driver to its caller.
    Esta funcion regresa las capacidades del driver a la rutina que la llama

/*/


DWORD FAR PASCAL DeviceCapabilities(
    LPSTR lpDevice,
    LPSTR lpPort,
    WORD nIndex,
    LPSTR lpOutput,
    LPDM lpDevmode)
{
    EXDEVMODE	 dmModo;
    LPDM    lpModo;
    DWORD    rc;
    LPPOINT    lpPoint = (LPPOINT)lpOutput;
    LPWORD    lpWord =  (LPWORD)lpOutput;
    int        i;

    /* revisar si el devmode esta bien */
    // see if the devmode struc. is ok.
    if(!lpDevmode)
    {
	if(!GetEnvironment(lpPort, (LPSTR) &dmModo, sizeof(EXDEVMODE)) ||
            lstrcmp((LPSTR) lpDevice, (LPSTR)dmModo.dm.dmDeviceName)  ||
	    !CheckDevMode((LPDM)&dmModo))
        {
            GetDevMode( lpDevice, &dmModo, NULL);
        }
        lpModo = &dmModo;
    }
    else
    {
        lpModo = lpDevmode;
    }
     /*  regresar capacidades requeridas por el indice */
    switch (nIndex) {
    case DC_FIELDS:
        rc = lpModo->dm.dmFields;
        break;

    case DC_PAPERS:        /* rc es el numero de papeles soportados */
    case DC_PAPERSIZE:    /* poner en lpOutput los resultados */
                // rc = number of paper (sizes) supported.
                // returns results in lpOutput.
	if (lpOutput)
	    {
	    if (nIndex == DC_PAPERS)
		{
		for (i = 0; i < NSMALLPAPERFORMATS; i++)
		    *lpWord++ = PaperFormat[i].code;
		}
	    else
		{
		for (i = 0; i < NSMALLPAPERFORMATS; i++, lpPoint++)
		    {
		    lpPoint->x = PaperFormat[i].XPhysMM * 10;
		    lpPoint->y = PaperFormat[i].YPhysMM * 10;
		    }
		}
	    }
        rc = NSMALLPAPERFORMATS;
        break;

    case DC_BINS:        /* rc es el numero de alimentaciones */
                /* poner en lpOutput los resultados */
                // rc = no. of feed trays.
                // returns results in lpOutput
        if(lpWord){
        *lpWord++ = DMBIN_TRACTOR;
        *lpWord++ = DMBIN_MANUAL;
        }
        rc = NBINS;
        break;

    case DC_SIZE:
        rc = lpModo->dm.dmSize;
        break;

    case DC_EXTRA:
        rc = lpModo->dm.dmDriverExtra;
        break;

    case DC_VERSION:
        rc = lpModo->dm.dmSpecVersion;
        break;

    case DC_DRIVER:
        rc = lpModo->dm.dmDriverVersion;
        break;

    case DC_ORIENTATION:
	return 0;

    default:
        rc = 0;
        break;

    }

    return(rc);
}
