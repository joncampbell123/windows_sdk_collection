/*
 * CONVERT.H
 *
 * Internal definitions, structures, and function prototypes for the
 * OLE 2.0 UI Convert dialog.
 *
 * Copyright (c)1992-1994 Microsoft Corporation, All Right Reserved
 */


#ifndef _CONVERT_H
#define _CONVERT_H


#include <compobj.h>
#include "ole2ui.h"


#define DIDConvert			  10004		//What should these IDs be for OLE Standard dlgs?


//Internally used structure
typedef struct tagCONVERT
{
	//Keep this item first as the Standard* functions depend on it here.
	LPOLEUICONVERT	     lpOCV;       //Original structure passed.

	/*
	 * What we store extra in this structure besides the original caller's
	 * pointer are those fields that we need to modify during the life of
	 * the dialog but that we don't want to change in the original structure
	 * until the user presses OK.
	 */
	unsigned long	    dwFlags;
//	int				    nConvertCurSel;
//	int				    nActivateCurSel;
	short			    nSelectedIndex;
	Boolean             fItemSelected;

    CLSID               clsid;    // Class ID sent in to dialog: IN only
    unsigned long       dvAspect;
    Boolean             fDefaultIcon;

    char               *lpszCurrentObject;
    char               *lpszConvertDefault;
    char               *lpszActivateDefault;

	unsigned char       szResult[256];
	ListHandle			hListConvert,
						hListActivate;
						
	short				iActiveItem;		// item in focus
    PicHandle           hIcon;       	      			// Metafile containing icon aspect
	char                szIconLabel[OLEUI_CCHLABELMAX];	// icon label
	OleIconSource		IconSource;						// icon source
	
} CONVERT, *PCONVERT;



//Internal function prototypes
//CONVERT.C
unsigned int	UConvertInit(DialogPtr, LPOLEUICONVERT);
unsigned int	CVUInitUserItems(DialogPtr);
Boolean			IsValidClassID(CLSID cID);

void			FillClassList(PCONVERT);
void			LAddClassToList(ListHandle hList, char *szClassName, char *szClassID);
Boolean			FormatIncludedDW(char *szStringToSearch, unsigned long dwFormat);
Boolean			FormatIncludedSZ(char *szStringToSearch, char *szFormatSearch);

#ifndef _MSC_VER
pascal Boolean	ConvertDialogProc(DialogPtr, EventRecord *, short *);
pascal void		CVUserItemProc(DialogPtr, short);
#else
Boolean __pascal ConvertDialogProc(DialogPtr, EventRecord *, short *);
void __pascal 	 CVUserItemProc(DialogPtr, short);
#endif

void			CVUpdateOneControl(DialogPtr, short, Boolean);
void			CVUpdateAllControls(DialogPtr, PCONVERT);
void			CVUpdateResults(DialogPtr, PCONVERT);

void			CVShowListItem(ListHandle, Boolean);
void			ToggleConvertType(DialogPtr, PCONVERT, unsigned long);
void 			CVUpdateDisplayAsIcon(DialogPtr pDialog, PCONVERT pCV);

void			ConvertCleanup(DialogPtr);

void 			CVDrawIconLabel(DialogPtr pDialog, PCONVERT pCV);
void 			CVDrawIcon(DialogPtr pDialog, PCONVERT pCV, Boolean FocusRectOnly);
void 			CVDrawList(DialogPtr pDialog, PCONVERT pCV, Boolean FocusRectOnly);
void 			CVUpdateClassIcon(DialogPtr pDialog, PCONVERT pCV, Boolean fUpdateIcon, Boolean fUpdateLabel);
void 			CVTabFocus(DialogPtr pDialog, PCONVERT pCV);
void 			CVSetActiveItem(DialogPtr pDialog, PCONVERT pCV, short item);
void 			CVToggleIconLabel(DialogPtr pDialog, PCONVERT pCV, short item);


#endif //!_CONVERT_H
