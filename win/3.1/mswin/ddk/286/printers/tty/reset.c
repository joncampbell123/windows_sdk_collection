/*/   RESET.C for TTY
 +-----------------------------------------+
 | Fralc Consultores (c), Mexico 1986-1989 |
 +-----------------------------------------+
/*/

// ------------------------------------------------------------------------
// Copyright (C) 1989-1990, Microsoft Corporation. All rights reserved.
// ------------------------------------------------------------------------
//	Microsoft History (latest first)
//	13 Sep 91	LinS		Rewrite
//	02 feb 90	craigc		Changed dpTechnology to DT_CHARSTREAM
//					(should have NO effect, though)
//	27 dec 89	peterbe		Call InitQueue() in Enable() instead
//					of calling CreatePQ() directly.  In
//					Disable(), call DeleteQueue() instead
//					of DeletePQ().
//	13 dec 89	peterbe		Cleaned up buffer allocation code.
//					Added more debug statements.
//	07 dec 89	peterbe		Disable() calls TextDump() now.
//	06 dec 89	peterbe		Add debug code and #ifdef's.
//	30 oct 89	peterbe		Added changes from 10-14-89 update
//					by Fralc.
//	20 oct 89	peterbe		Checked in.
// ------------------------------------------------------------------------

#include "generic.h"

#ifdef DEBUG
#include "debug.h"
#define DBGenable(msg) DBMSG(msg)
#define DBGalloc(msg) /* DBMSG(msg)*/
#else
#define DBGenable(msg) /* zip */
#define DBGalloc(msg) /* zip */
#endif

short FAR PASCAL Enable(
			 LPDV	 lpdv	 ,
			 short	     style	 ,
			 LPSTR	     lpdvType,
			 LPSTR	     lpOutputFile,
			 LPDM	     lpdmIn	)
{
    EXDEVMODE DevMode;

    if(!hInst)
	hInst = GetModuleHandle("TTY");
            //  the ModuleName is defined in tty.def  as the LIBRARY parm
    DBGenable(("Enable()\n"));

    /* asegurase de que tenemos los correctos datos de devmode */
    if(!lpdmIn ||
	lstrcmp((LPSTR) lpdvType, (LPSTR)lpdmIn->dm.dmDeviceName) ||
	!CheckDevMode(lpdmIn)) /* mal lpDevmodeIn */
	    GetDevMode((LPSTR)lpdvType, (LPDM)&DevMode, NULL);
    else
	Copy((LPSTR)&DevMode, (LPSTR)lpdmIn, sizeof(EXDEVMODE));

    /* Ahora DevMode contiene la estructura DevMode; no usar "lpDevmodeIn" */

    /* Si style es 1 llenar la estructura GDIINFO3 */


    if(style & InquireInfo)
	{
	Copy((LPSTR)lpdv, (LPSTR)&gBaseInfo, sizeof(GDIINFO));

	((GDIINFO far *)lpdv)->dpHorzSize = PaperFormat[DevMode.paper].XPhysMM;
	((GDIINFO far *)lpdv)->dpHorzRes  = PaperFormat[DevMode.paper].XPhys;
	((GDIINFO far *)lpdv)->dpVertSize = PaperFormat[DevMode.paper].YPhysMM;
	((GDIINFO far *)lpdv)->dpVertRes  = PaperFormat[DevMode.paper].YPhys;
#if 0
	if(DevMode.paper == LEGAL)
	    {
	    ((GDIINFO far *)lpdv)->dpHorzSize = MM_HSIZE_LEG;
	    ((GDIINFO far *)lpdv)->dpHorzRes  = PG_ACROSS_LEG;
	    ((GDIINFO far *)lpdv)->dpVertSize = MM_VSIZE_LEG;
	    ((GDIINFO far *)lpdv)->dpVertRes  = PG_DOWN_LEG;
	    }
	else if(DevMode.paper == A4)
	    {
	    ((GDIINFO far *)lpdv)->dpHorzSize = MM_HSIZE_A4;
	    ((GDIINFO far *)lpdv)->dpHorzRes  = PG_ACROSS_A4;
	    ((GDIINFO far *)lpdv)->dpVertSize = MM_VSIZE_A4;
	    ((GDIINFO far *)lpdv)->dpVertRes  = PG_DOWN_A4;
	    }
	else if(DevMode.paper == B5)
	    {
	    ((GDIINFO far *)lpdv)->dpHorzSize = MM_HSIZE_B5;
	    ((GDIINFO far *)lpdv)->dpHorzRes  = PG_ACROSS_B5;
	    ((GDIINFO far *)lpdv)->dpVertSize = MM_VSIZE_B5;
	    ((GDIINFO far *)lpdv)->dpVertRes  = PG_DOWN_B5;
	    }
	else
	    {
	    ((GDIINFO far *)lpdv)->dpHorzSize = MM_HSIZE_LET;
	    ((GDIINFO far *)lpdv)->dpHorzRes  = PG_ACROSS_LET;
	    ((GDIINFO far *)lpdv)->dpVertSize = MM_VSIZE_LET;
	    ((GDIINFO far *)lpdv)->dpVertRes  = PG_DOWN_LET;
	    }
#endif

	if(DevMode.use_wide_carriage)
	    {
	    ((GDIINFO far *)lpdv)->dpHorzSize = MM_HSIZE_15;
	    ((GDIINFO far *)lpdv)->dpHorzRes  = PG_ACROSS_15;
	    }

	((GDIINFO far *)lpdv)->dpDEVICEsize = sizeof(DEVICE);
	return sizeof(GDIINFO);
	}

    SetByteValue((LPSTR)lpdv, 0x0, sizeof(DEVICE));

    lpdv->iType = DEV_DEVICE;

    if (LoadThePrinter(&lpdv->escapecode))
	{
#if 0
	char Mesg[80];
	char Capt[30];

	LoadString(hInst, CAPTID, Capt, 29);
	LoadString(hInst, DFEID, Mesg, 79);
	MessageBox(NULL, Mesg, Capt, 0);
#endif
	return FALSE;
	}

    /* posiblemente quitar una estructura port_device
        pues hay que inicializar anchos de hoja */

    lpdv->epPageWidth = PaperFormat[DevMode.paper].XPhys;
    lpdv->epPageHeight = PaperFormat[DevMode.paper].YPhys;
    lpdv->epPageWidthInch = lpdv->epPageWidth / HDPI;

#if 0
    if(DevMode.paper == LEGAL)
	{
	lpdv->epPageWidth = PG_ACROSS_LEG;
	lpdv->epPageHeight = PG_DOWN_LEG;
	}
    else if(DevMode.paper == A4)
	{
	lpdv->epPageWidth = PG_ACROSS_A4;
	lpdv->epPageHeight = PG_DOWN_A4;
	}
    else if(DevMode.paper == B5)
	{
	lpdv->epPageWidth = PG_ACROSS_B5;
	lpdv->epPageHeight = PG_DOWN_B5;
	}
    else
	{
	lpdv->epPageWidth = PG_ACROSS_LET;
	lpdv->epPageHeight = PG_DOWN_LET;
	}
#endif

    lpdv->epBmpHdr.bmWidth = 1;
    lpdv->epBmpHdr.bmHeight = 1;
    lpdv->epBmpHdr.bmWidthBytes = 2;

    if(DevMode.use_wide_carriage)
	{
	lpdv->epPageWidth = PG_ACROSS_15;
	lpdv->epBmpHdr.bmWidth = PG_ACROSS_15;
	lpdv->epBmpHdr.bmWidthBytes = PG_ACROSS_15/8;
	}

    /* Esta bandera sirve para indicar a myWrite si es necesario desplegar
       El dia'logo para insertar nueva hoja de papel, siempre y cuando
       epCutSheet este en CUT -- this flag serves to indicate to myWrite
       whether it's necessary to display the dialog for inserting a new
       sheet of paper, whenever epCutSheet is in CUT */

    lpdv->bFirstCharacter = TRUE;

    lpdv->epBmpHdr.bmWidthPlanes = lpdv->epBmpHdr.bmHeight *
				       lpdv->epBmpHdr.bmWidthBytes;

    SetDeviceMode(lpdv, (LPDM)&DevMode);

    lpdv->epXcurwidth = CHARWIDTH;

/* si style es 0, inicializar el modulo de soporte y  el periferico
   para ser usado por las rutinas de "GDI" (es necesario un heap) */

    if(!(style & InfoContext))
	{ /* Inicializacion por un DC */
	lstrcpy(lpdv->epPort, lpOutputFile);

	if(heapinit(lpdv))
	    {
	    if(InitQueue(lpdv)) 	// changed 27 dec 89
		return TRUE;
	    else
		{
		DBGalloc(("Enable(): freeing epHeap\n"));
		GlobalFree(lpdv->epHeap);
		}
	    }
    /* esto ha fallado por causa de que halla fallado "heapinit" o no se pueda
       poner "priority queue" */

	return FALSE;
	}
    else
	lpdv->bInfo = TRUE;

    return TRUE;
}	// Enable()


FAR PASCAL Disable(LPDV lpdv)
{
    DBGenable(("Disable()\n"));

    // If a priority queue was created, discard any buffers and
    // delete the queue.

    if (lpdv->epYPQ)
	DeleteQueue(lpdv);

    if (lpdv->epHeap)
	GlobalFree(lpdv->epHeap);

    return TRUE;
}	// Disable()

void NEAR PASCAL SetDeviceMode(lpdv, lpdmIn)
LPDV lpdv;
LPDM lpdmIn;
{
    /* alta calidad hace baja velocidad y baja caliada pude dar
       alta velocidad */
    lpdv->pf = &PaperFormat[lpdmIn->paper - STANDARDPF +
		lpdmIn->use_wide_carriage * NSMALLPAPERFORMATS];
    lpdv->bPageBreak = lpdmIn->page_break;
    lpdv->bCutSheet  = lpdmIn->feed == CUT;
}	// SetDeviceMode()


short NEAR PASCAL heapinit(lpdv)
LPDV lpdv;
{
	if (!(lpdv->epHeap = GlobalAlloc(GMEM_MOVEABLE, (long) INIT_BUF)))
		return FALSE;
	lpdv->epHPsize = INIT_BUF;
	lpdv->epHPptr = 0;
	return TRUE;
}

// InitQueue(), DeleteQueue()

HANDLE NEAR PASCAL InitQueue(LPDV lpdv)
{
    if (lpdv->epYPQ == NULL)
	lpdv->epYPQ = CreatePQ(INITQSIZE);

    return lpdv->epYPQ;
}	// end InitQueue)

void NEAR PASCAL DeleteQueue(LPDV lpdv)
{
    if (lpdv->epYPQ != NULL)
	DeletePQ(lpdv->epYPQ);	    // delete the queue structure.
}	// end DeleteQueue()

