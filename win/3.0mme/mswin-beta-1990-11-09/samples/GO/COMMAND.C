/* 
 * command.c	
 */

#include <windows.h>
#include <wincom.h>
#include <mediaman.h>
#include "medgo.h"
#include "go.h"


/* InitMenuPopup(hwnd, wParam, lParam)
 * 
 * Handles WM_INITMENUPOPUP messages and initializes the appropriate
 * popup menu.  I use INITMENUPOPUP instead of INITMENU since in a large
 * application, initializing a menu make take significant time, which I
 * don't want to have to wait for (times the number of menus present).
 * 
 */
LONG InitMenuPopup(HWND hwnd, WORD wParam, LONG lParam)
{
	PWinInfo	pWinInfo;
	WORD		wMenu;
	int		i;

	/*  Make sure this isn't the system menu  */
	if (HIWORD(lParam))
		return DefWindowProc(hwnd, WM_INITMENUPOPUP, wParam, lParam);

	wMenu = LOWORD(lParam);
	pWinInfo = GetWinInfo(hwnd);

	switch (wMenu)
	{
	case 0:

		/* this is the "File" menu */

		/* Normally, the Open and new items are always enabled.
		 * the only exception is when a file operation is in progress,
		 * in which case even those must be disabled.
		 */

		if (pWinInfo->fFileOp == TRUE)
			i = MF_GRAYED;
		else
			i = MF_ENABLED;
		EnableMenuItem(wParam, IDM_FILEOPEN, i);
		EnableMenuItem(wParam, IDM_FILENEW, i);

		/* see if resource to save or close */
		if (pWinInfo->medid != 0L && pWinInfo->fFileOp == FALSE)
			i = MF_ENABLED;
		else
			i = MF_GRAYED;
		EnableMenuItem(wParam, IDM_FILESAVE, i);
		EnableMenuItem(wParam, IDM_FILESAVEAS, i);
		EnableMenuItem(wParam, IDM_FILECLOSE, i);

		break;

	default:

		break;
	}

	return 0L;
}


/* AppCommand()
 *
 * Application command message processor.
 */
LONG AppCommand(HWND hwnd, unsigned msg, WORD wParam, LONG lParam)
{
	FARPROC		fpfn;
	BOOL		f;
	int		i;
	PWinInfo	pWinInfo;
	MedReturn	medRet;
	char		achExt[20];
	char		achTitle[100];
	char		achName[100];
	MEDID		medidNew;

	pWinInfo = GetWinInfo(hwnd);

	switch (wParam)
	{

	case IDM_FILEEXIT:

		/* Tell myself to exit.  The normal WM_CLOSE processing
		 * mechanism will ask to save the file before closing,
		 * if needed.
		 */
		PostMessage(hwnd, WM_CLOSE, 0, 0L);
		break;

	case IDM_FILEABOUT:

		/* display the "About" box */
		DoAbout(hwnd, hInstApp);
		break;

	case IDM_FILENEW:

		/* ask user to save current GO resource */
		if (!ResourceAskToSave(hwnd))
			break;

		/* make a new resource of type "GO", with create parameter
		 * zero.  This call does an implicit medAccess().
		 */
		f = medCreate(&medRet, medtypeGO, 0L); 

		/* tell myself to use the new resource */
		if (f)
			PostMessage(hwnd, WM_SETMEDID, 0, medRet.medid);

		break;

	case IDM_FILEOPEN:
		
		/* open a new GO file */

		pWinInfo->fFileOp = TRUE;
		LoadString(hInstApp,IDS_OPENTITLE,achTitle,sizeof(achTitle));
		LoadString(hInstApp,IDS_GOEXT,achExt,sizeof(achExt));

		i = OpenFileDialog( hwnd, (LPSTR)achTitle, (LPSTR)achExt,
			DLGOPEN_OPEN, (LPSTR)achName, sizeof(achName) );
		if( i == DLG_OKFILE ) {
		    fpfn = MakeProcInstance(PhysTypeDlgProc, hInstApp);
		    i = DialogBox(hInstApp, "PHYSTYPE", hwnd, fpfn);
		    FreeProcInstance(fpfn);

		    if( i ) {
			medidNew = medLocate( (LPSTR)achName, medtypeGO, 
				MEDF_LOCATE, NULL );
			if( medidNew ) {
			    medSetPhysicalType(medidNew, medtypePhys);
			    medRet.medid =  medidNew;
			    medRet.dwReturn = 0L;
			    f = medAccess( medidNew, 0L, (FPMedReturn)&medRet,
				TRUE, NULL, 0L );
			    if( f == MEDF_OK ) {
				/* tell myself to use the new resource ID */
				PostMessage(hwnd, WM_SETMEDID, 0, medidNew);
			    } else {
				ErrorResBox( hwndApp, hInstApp,
					MB_OK | MB_ICONEXCLAMATION,
					IDS_APPTITLE, IDS_ERRORLOAD );
			    }
			}
		    }
		}
		pWinInfo->fFileOp = FALSE;
		break;

	case IDM_FILECLOSE:
		/* don't allow closing during a file operation */
		if (pWinInfo->fFileOp == TRUE)
		{
			MessageBeep(0);
			return 0L;
		}

		/* Release the current resource.  Use FALSE for <fImmediate>,
		 * since I do want to update the title bar to indicate
		 * no-resource.
		 */
		if (ResourceRelease(hwnd, FALSE))
			InvalidateRect(hwnd, 0L, TRUE);

		break;

	case IDM_FILESAVE:
		/* save Go file */
		ResourceSaveFile(hwnd, FALSE);
		break;

	case IDM_FILESAVEAS:
		/* save Go file under a new name and/or format */
		ResourceSaveFile(hwnd, TRUE);
		break;
	}

	return 0L;
}



