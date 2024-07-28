/**********************************************************************
 * devcap.c -
 *
 * Copyright (C) 1989 Microsoft Corporation.  All rights reserved.
 *
 *********************************************************************/

/*
 *   2-14-89	jimmat	Created as part of the Driver Initialization
 *			changes.
 *
 *   3-13-89	chrisg	cloned. adding to the PS driver.
 *   3-28-91	msd	added support for duplex and renamed Reserved
 *          	   	to iDeviceRes.
 */

#include "pscript.h"
#include "driver.h"
#include "psdata.h"
#include "profile.h"
#include "defaults.h"
#include "getdata.h"
#include "utils.h"
#include "control2.h"
#include "debug.h"
#include "resource.h"


//  temporary defines until they migrate to drivinit.h

#define  CCHPAPERNAME  64
#define  CCHBINNAME    24
#define  DC_PAPERNAMES 16
#define  DC_TRUETYPE   15


/*  DeviceCapabilities -
 *
 *  This function returns the capabilities of the device driver to
 *  the caller.
 */

DWORD FAR PASCAL DeviceCapabilities(
	LPSTR lpDevice, 
	LPSTR lpPort, 
	WORD nIndex,
	LPSTR lpOutput, 
	LPPSDEVMODE lpDevmode)
{

	DWORD rc = -1;
	LPPOINT pp;
	int	i;
	short	FAR *wp;
    char  FAR  *lpPaperNames;
	PPSDEVMODE pCurEnv = NULL;
	LPPSDEVMODE lpdm;
	PPRINTER pPrinter;
	PPAPER pPaper;

	DBMSG(("DeviceCapabilities(%lp,%lp,%d,%lp,%lp)\n",
	    lpDevice, lpPort, nIndex, lpOutput, lpDevmode));

	/* If the caller didn't pass in a PSDEVMODE pointer, then get/build
	 * a current one
	 */

	if (!lpDevmode) {

      pCurEnv = AllocDevMode();
      if (!pCurEnv)
         return -1;

		MakeEnvironment(lpDevice, lpPort, pCurEnv, NULL);

		lpdm = pCurEnv;		/* use current/constructed PSDEVMODE */

	} else

		lpdm = lpDevmode;	/* use caller's PSDEVMODE struct */


	/* Return capability value(s) to caller based on requested index
	 */

	pPrinter = GetPrinter(lpdm->iPrinter);


	switch (nIndex) {

	case DC_COPIES:
		rc = 999;
		break;

	case DC_ORIENTATION:
		rc = (DWORD)lpdm->LandscapeOrient;
		break;

	case DC_FIELDS:
		rc = lpdm->dm.dmFields;
		break;

	case DC_TRUETYPE:
		rc = DCTT_DOWNLOAD | DCTT_SUBDEV ;
		break;

    case DC_PAPERNAMES:  /*  return array of papername strings  */
	case DC_PAPERS:   	/* return value is # supported paper */
	case DC_PAPERSIZE:	/* sizes, and (maybe) list of papers */
				/* or sizes in 10ths of a mm	     */


		if (nIndex == DC_PAPERNAMES) 
        {
            lpPaperNames = (char FAR * )lpOutput;
		  	pp = NULL;
			wp = NULL;
        }
		else if (nIndex == DC_PAPERS) {
			wp = (short FAR * )lpOutput;
		  	pp = NULL;
            lpPaperNames = NULL;
		} 
        else 
        {
            pp = (LPPOINT)lpOutput;
			wp = NULL;
            lpPaperNames = NULL;
        }

		pPaper = GetPaperMetrics(pPrinter, lpdm);

		if (!pPaper)
        {
            rc = -1;  // error
			break;
        }

		rc = (DWORD)GetNumPapers(pPrinter);

        if(!(lpPaperNames || pp || wp))
            break;

		for (i = 0; i < (int)rc; i++) 
        {
			int iPaper;

            iPaper = pPaper[i].iPaper;

                // = GetPaperEntry(pPrinter, lpdm, i);

            if(lpPaperNames)
            {
        		LoadString(ghInst, 
                    iPaper + DMPAPER_BASE, 
                    lpPaperNames + i * CCHPAPERNAME, CCHPAPERNAME);
            }
			else if (wp)		/* DC_PAPERS index */
				*wp++ = iPaper;
			if (pp) {	/* DC_PAPERSIZE index */
				pp->x = Scale(pPaper[i].cxPaper, 254, DEFAULTRES);
				pp->y = Scale(pPaper[i].cyPaper, 254, DEFAULTRES);
				pp++;
			}
		}

        if(pPaper)
    		LocalFree((HANDLE)pPaper);
		break;


    case DC_BINNAMES:   // return array of strings containing feed
                        //    names  or just number of strings 
	case DC_BINS:		// return value is # supported paper 
				// bins, and (maybe) the list of bins
                //  CHEAT! reusing lpPaperNames, the correct name 
                //  should be  lpBinNames.

		if (nIndex == DC_BINNAMES) 
        {
            lpPaperNames = (char FAR * )lpOutput;
			wp = NULL;
        }
		else 
        {
			wp = (short FAR * )lpOutput;
            lpPaperNames = NULL;
		} 

		/* following check _very_ similar to code in MergeEnvironment() */

		for (i = 0, rc = 0; i < NUMFEEDS; i++)
        {
			if (pPrinter->feed[i]) 
            {
				if (wp)
					*wp++ = i + DMBIN_FIRST;
                if(lpPaperNames)
        			LoadString(ghInst, i + DMBIN_FIRST + DMBIN_BASE, 
                        lpPaperNames + rc * CCHBINNAME, CCHBINNAME);

				rc++;
			}
        }

		break;

	case DC_DUPLEX:
		rc = IS_DUPLEX(pPrinter) ? 1 : 0;
		break;

	case DC_SIZE:

		rc = lpdm->dm.dmSize;
		break;


	case DC_EXTRA:

		rc = lpdm->dm.dmDriverExtra;
		break;


	case DC_VERSION:

		rc = lpdm->dm.dmSpecVersion;
		break;


	case DC_DRIVER:

		rc = lpdm->dm.dmDriverVersion;
		break;

        case DC_ENUMRESOLUTIONS:

                /* every printer has at least a default resolution */
                if (lpOutput) {
                    ((DWORD FAR *) lpOutput)[0] = 
                    ((DWORD FAR *) lpOutput)[1] = pPrinter->defRes;
                }

                /* see if others are listed in iDeviceRes array */
                for (rc = 1; rc <= NUMDEVICERES
                             && pPrinter->iDeviceRes[rc-1];
                     ++rc) {

                    if (lpOutput) {
                        ((DWORD FAR *) lpOutput)[rc*2] = 
                        ((DWORD FAR *) lpOutput)[rc*2+1] = pPrinter->iDeviceRes[rc-1];
                    }
                }
                break;

        case DC_FILEDEPENDENCIES:

                rc = (lpdm->iPrinter >= EXT_PRINTER_MIN) ? 1 : 0;

                if (rc && lpOutput)
                    GetExternPrinterFilename(lpdm->iPrinter - EXT_PRINTER_MIN + 1, lpOutput);

                break;

	default:

		rc = -1;
		break;

	}

	FreePrinter(pPrinter);

   if (pCurEnv)
      FreeDevMode(pCurEnv);

	DBMSG(("DeviceCapabilities() returning %ld\n", rc));

	return rc;
}
