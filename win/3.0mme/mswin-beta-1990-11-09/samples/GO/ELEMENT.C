/*
 * element.c	
 */

#include <windows.h>
#include <wincom.h>
#include <mediaman.h>
#include "medgo.h"
#include "go.h"

/* prototype form the c 6.0 STDLIB.H include file */
extern void _splitpath(char *, char *, char *, char *, char *);

#define _MAX_PATH      144      /* max. length of full pathname */
#define _MAX_FNAME   9      /* max. length of file name component */
#define _MAX_EXT     5      /* max. length of extension component */

/* ResourceFixTitle(hwnd, medid)
 * 
 * This procedure fixes the window title of <hwnd> and the instance title
 * of the application to use the name of resource <medid>.
 * 
 * If <medid> == 0L, then the title is changed to indicate that no
 * resource is currently being edited, i.e. just the name of the
 * application on the window, and the title "(no-resource)" as the
 * workbench instance title.
 * 
 * If <medid> != 0L, the window title is changed to be the name of the app
 * followed by a dash, followed by the name of the resource.  The
 * workbench instance title is changed to the name of the resource.  The
 * name of the resource is determined as follows:  If the resource is
 * dynamic, the title defaults to "(untitled)".  Otherwise, the filename
 * and extension portions of the resource path is used.
 */
VOID ResourceFixTitle(HWND hwnd, MEDID medid)
{
	WORD		w;
	char		achTitle[_MAX_PATH];
	char		achName[_MAX_FNAME + _MAX_EXT];
	char		achExt[_MAX_EXT];  
	LPSTR		lpFilename;

	/* If there is no resource, then just set the title to be
	 * the name of the application, and set the instance title
	 * to be "(none)".  This is sort of a hack, and this condition
	 * may not be allowed on certain tools.
	 */
	if (medid == 0L)
	{
		LoadString(hInstApp, IDS_APPTITLE,
			achTitle, sizeof(achTitle));
		SetWindowText(hwnd, achTitle);
		LoadString(hInstApp, IDS_NORESOURCE,
			achTitle, sizeof(achTitle));
	}
	else
	{
		/* There is a resource, so determine if it is untitled or not.
		 * If untitled, use the string "(untitled)" for the instance
		 * title.  Otherwise, use the fname.ext portion of the
		 * resource filepath as the instance title.  The window
		 * title is the name of the app, followed by a dash, followed
		 * by the instance title that was constructed above.
		 */

		/* load the "(untitled)" string as a default */
		LoadString(hInstApp, IDS_UNTITLED, achName, sizeof(achName));
		lpFilename = achName;

		/* if there's a resource, then get its name */
		if (medid != 0L)
		{
			w = medGetFileName(medid, achTitle, sizeof(achTitle));
			if (w == MEDNAME_FILE)
			{
				/* if resource is not untitled, grab
				 * filename.ext
				 */
				_splitpath(achTitle, NULL, NULL,
					  achName, achExt);
				lstrcat(achName, achExt);
			}
		}

		/* Load the header for use in setting the window caption.
		 * This has the application name followed by a dash.
		 * Concatenate the instance title onto this.
		 */
		LoadString(hInstApp, IDS_APPTITLEHEADER,
			achTitle, sizeof(achTitle));
		lstrcat(achTitle, lpFilename);

		SetWindowText(hwnd, achTitle);
	}
}


/* fOk = ResourceAskToSave(hwnd)
 * 
 * This procedure does the standard "The current resource has changed.
 * Do you wish to save it?" message and saves the resource if the user
 * answers yes.
 * 
 * The return code is as follows:  If TRUE is returned, then the user
 * has approved abandoning the current resource (either he answered yes
 * to save and the resource was saved, or he answered no don't save).
 * If FALSE is returned, then abandoning the resource is not approved
 * (either the user pressed cancel or the resource could not be saved
 * for some reason).
 */
BOOL ResourceAskToSave(HWND hwnd)
{
	PWinInfo	pWinInfo;
	WORD		w;
	char		achName[_MAX_FNAME + _MAX_EXT];
	char		achExt[_MAX_EXT];
	char		achMedName[_MAX_PATH];

	pWinInfo = GetWinInfo(hwnd);

	if (pWinInfo->medid && medIsDirty(pWinInfo->medid))
	{
		/* get the name of the resource */
		if (MEDNAME_FILE == medGetFileName(pWinInfo->medid,
			achMedName, sizeof(achMedName)))
		{
			/* resource has a title -- use it */
			_splitpath(achMedName, NULL, NULL, achName, achExt);
			lstrcat(achName, achExt);
			w = ErrorResBox(hwnd, hInstApp,
				MB_YESNOCANCEL | MB_ICONQUESTION,
				IDS_APPTITLE, IDS_RESOURCECHANGE,
				(LPSTR) achName);
		}
		else
		{
			w = ErrorResBox(hwnd, hInstApp,
				MB_YESNOCANCEL | MB_ICONQUESTION,
				IDS_APPTITLE, IDS_UNTITLEDCHANGE);
		}

		if (w == IDYES)
		{
			if (!ResourceSaveFile(hwnd, FALSE))
				w = IDCANCEL;	// check save failure!
		}

		/* If the user answers "no" to the save message,
		 * release the resource.  This will avoid the
		 * problem of modify a resource, fileOpen.  Answer
		 * NO to FileSave? message, then select same
		 * filename to open.  Due to way mediaman
		 * operates, file will not be reloaded, but you'll
		 * get the original (pre-open) modified original.
		 */
		if (w == IDNO)
		{
			/* Release the resource of the window here, so that
			 * MediaMan will flush the resource.
			 */
			InvalidateRect(hwnd, NULL, TRUE);
			ResourceRelease(hwnd, FALSE);
		}
	}
	else
	{
		/* Otherwise, there is no current resource, so by default
		 * the resource change is approved.
		 */
		w = IDYES;
	}

	return (w != IDCANCEL);
}


/* fSuccess = ResourceRelease(hwnd, fImmediate)
 * 
 * Causes any current resource on window <hwnd> to be released, and the
 * window switched into the no-resource state.  The resource is released
 * using medRelease(), and all instance information in the window
 * instance structure is deallocated or fixed up as required.
 * 
 * If the resource is being released and another resource is not going
 * to be immediately placed onto the window, then <fImmediate> should be
 * FALSE so as to cause the title to be updated to reflect the new
 * no-resource status.  If another resource is going to immediately be
 * placed onto the window, fImmediate should be TRUE to prevent title
 * update flicker.
 */
BOOL ResourceRelease(HWND hwnd, BOOL fImmediate)
{
	MEDUSER		meduser;
	MEDID		medidT;
	PWinInfo	pWinInfo;

	pWinInfo = GetWinInfo(hwnd);

	/* Get current resource, return if there isn't one */
	if (pWinInfo->medid == 0L)
	return FALSE;

	/* Do any cleanup of data in the window instance structure that
	 * depends on the current resource here.  Potential stuff:
	 * sizes, copies of information, extra windows (zoom, palEdit), etc.
	 */

	/* unregister myself if this is the case */
	medUnregisterUsage(pWinInfo->medid, pWinInfo->meduser);

	/* get rid of the old resource */
	medRelease(pWinInfo->medid, 0L);

	/* clear up instance data */
	pWinInfo->medid = 0L;
	pWinInfo->fFileOp = FALSE;

	/* If I'm not going to immediately place a resource back onto
	 * this window, then switch myself into the no-resource state
	 * by updating my title and clearing the window. (may not be
	 * necessary for all systems).
	 */
	if (!fImmediate)
	{
		ResourceFixTitle(hwnd, 0L);
		InvalidateRect(hwnd, NULL, TRUE);
	}

	return TRUE;
}


/* fSuccess = ResourceSet(hwnd, medidNew)
 * 
 * Causes the given window to begin using resource <medidNew> from now
 * on.  If a resource is currenly in use by <hwnd>, it is released using
 * ResourceRelease().  Then the new resource is put into the window
 * instance data structure and the appropriate information fixed up.
 */
BOOL ResourceSet(HWND hwnd, MEDID medidNew)
{
	PWinInfo	pWinInfo;

	/* release any resource currently in use, with <fImmediate> on
	 * to avoid title flicker
	 */
	ResourceRelease(hwnd, TRUE);

	pWinInfo = GetWinInfo(hwnd);

	/* reset any window instance that depends on the resource here */
	pWinInfo->medid = medidNew;

	/* hook this window onto the resource change notification
	 * system so I will get updates on the resource status
	 */
	medRegisterUsage(medidNew, pWinInfo->meduser);

	/* fixup my title to be the title of the new resource */
	ResourceFixTitle(hwnd, medidNew);
	InvalidateRect(hwnd, 0L, TRUE);

	return TRUE;
}


/* fSuccess = ResourceSaveFile(hwnd, fSaveAs)
 * 
 * This routine handles all the stuff needed to save a resource.  If
 * this is to be an explicit SaveAs, the <fSaveAs> should be TRUE,
 * otherwise a save operation with no opportunity to change the resource
 * name will be done, unless the workbench/media element manager decides that
 * a saveAs is required (due to insufficient information, or a secondary
 * handler that is incapable of saving, etc).
 * 
 * The internal instance structure is fixed up as needed.  Note that
 * calling this function may cause the resource ID (and the resource
 * instance data as well) to change.
 */
BOOL ResourceSaveFile(HWND hwnd, BOOL fSaveAs)
{
	FARPROC		fpfn;
	PWinInfo	pWinInfo;
	WORD		f;
	int		i;
	MEDID		medidNew;
	MedReturn	medRtn;
	char		achExt[20];
	char		achTitle[100];
	char		achName[100];
	
	pWinInfo = GetWinInfo(hwnd);
	pWinInfo->fFileOp = TRUE;

	medUnregisterUsage(pWinInfo->medid, pWinInfo->meduser);

	if( fSaveAs || (MEDNAME_DYNAMIC == 
		    medGetFileName( pWinInfo->medid, NULL, 0 )) ) {
	    
	    LoadString(hInstApp,IDS_SAVETITLE,achTitle,sizeof(achTitle));
	    LoadString(hInstApp,IDS_GOEXT,achExt,sizeof(achExt));

	    i = OpenFileDialog( hwnd, (LPSTR)achTitle, (LPSTR)achExt,
			DLGOPEN_SAVE, (LPSTR)achName, sizeof(achName) );
	    f = MEDF_ERROR;
	    if( i != DLG_CANCEL ) {
		fpfn = MakeProcInstance(PhysTypeDlgProc, hInstApp);
		i = DialogBox(hInstApp, "PHYSTYPE", hwnd, fpfn);
		FreeProcInstance(fpfn);

		if( i ) {
		    medSetPhysicalType(pWinInfo->medid, medtypePhys);
		    medRtn.medid = 0L;
		    f = medSaveAs( pWinInfo->medid, (FPMedReturn)&medRtn,
			(LPSTR)achName, 0L, TRUE, NULL, 0L );
		    medidNew = medRtn.medid;
		}
	    }
	} else {
	    medidNew = pWinInfo->medid;
	    LoadString(hInstApp, IDS_SAVETITLE, achTitle, sizeof(achTitle));
    	    f = medSave( pWinInfo->medid, 0L, TRUE, NULL, 0L );
	}

	pWinInfo->fFileOp = FALSE;

	/* If the save(as) failed, the old resource is still there,
	 * so continue to use it.  Re-register my meduser onto the resource.
	 */
	if (f != MEDF_OK) {
		medRegisterUsage(pWinInfo->medid, pWinInfo->meduser);
		return FALSE;
	}

	medRegisterUsage(medidNew, pWinInfo->meduser);

	/* If the resource ID (not the resource) has changed (as will happen
	 * when a SaveAs is done), swap in the the new resource ID and
	 * update the name displays.
	 */
	if (medidNew != pWinInfo->medid) {
		pWinInfo->medid = medidNew;
		ResourceFixTitle(hwnd, medidNew);
	}
	return TRUE;
}
