/*
 * PASTESPL.H
 *
 * Internal definitions, structures, and function prototypes for the
 * OLE 2.0 UI Paste Special dialog.
 *
 * Copyright (c)1992-1994 Microsoft Corporation, All Right Reserved
 */


#ifndef _PASTESPL_H
#define _PASTESPL_H


#include <compobj.h>
#include "ole2ui.h"


#define DIDPasteSpecial		  10001		//What should these IDs be for OLE Standard dlgs?


// Length of buffers to hold the strings 'Unknown Type', Unknown Source'
//   and 'the application which created it'
#define PS_UNKNOWNSTRLEN               100


//Internally used structure
typedef struct tagPASTESPECIAL
{
	//Keep this item first as the Standard* functions depend on it here.
	LPOLEUIPASTESPECIAL     lpOPS;       //Original structure passed.

	/*
	 * What we store extra in this structure besides the original caller's
	 * pointer are those fields that we need to modify during the life of
	 * the dialog but that we don't want to change in the original structure
	 * until the user presses OK.
	 */
	unsigned long	    dwFlags;
//	int				    nPasteListCurSel;
//	int				    nPasteLinkListCurSel;
	int				    nSelectedIndex;  // Index in arrPasteEntries[] corresponding to user selection
	Boolean             fItemSelected;
//	Boolean				fOleCompatibleData;
	Boolean				fLink;

    Handle              hBuff;                // Scratch Buffer for building up strings
    char                szUnknownType[PS_UNKNOWNSTRLEN];    // Buffer for 'Unknown Type' string
    char                szUnknownSource[PS_UNKNOWNSTRLEN];  // Buffer for 'Unknown Source' string
    char                szAppName[OLEUI_CCHKEYMAX]; // Application name of Source. Used in the result text
                                                    //   when Paste is selected. Obtained using clsidOD.

    // Information obtained from OBJECTDESCRIPTOR. This information is accessed when the Paste
    //    radio button is selected.
    CLSID               clsidOD;              // ClassID of source
    char               *szFullUserTypeNameOD; // Full User Type Name
    char               *szSourceOfDataOD;     // Source of Data
    Boolean             fSrcAspectIconOD;     // Does Source specify DVASPECT_ICON?
    Boolean             fSrcOnlyIconicOD;     // Does Source specify OLEMISC_ONLYICONIC?
    PicHandle           hIconOD;       	      // Metafile containing icon aspect
	char                szIconLabelOD[OLEUI_CCHLABELMAX];	// icon label
    Handle              hObjDesc;             // Handle to OBJECTDESCRIPTOR structure from which the
                                              //   above information is obtained

    // Information obtained from LINKSRCDESCRIPTOR. This infomation is accessed when the PasteLink
    //   radio button is selected.
    CLSID               clsidLSD;             // ClassID of source
    char				*szFullUserTypeNameLSD;// Full User Type Name
    char				*szSourceOfDataLSD;    // Source of Data
    Boolean             fSrcAspectIconLSD;    // Does Source specify DVASPECT_ICON?
    Boolean             fSrcOnlyIconicLSD;    // Does Source specify OLEMISC_ONLYICONIC?
    PicHandle           hIconLSD;         	  // Metafile containing icon aspect
	char                szIconLabelLSD[OLEUI_CCHLABELMAX];	// icon label
    Handle              hLinkSrcDesc;         // Handle to LINKSRCDESCRIPTOR structure from which the
                                              //   above information is obtained

	char                szResult[256];
	int                 nResultIcon;
	ListHandle			hListPaste,
						hListPasteLink;
	short				iActiveItem;
} PASTESPECIAL, *PPASTESPECIAL;



// Data corresponding to each list item. A pointer to this structure is attached to each
//   Paste\PasteLink list box item in an invisible column
typedef struct tagPASTELISTITEMDATA
{
   int                   nPasteEntriesIndex;   // Index of arrPasteEntries[] corresponding to list item
   Boolean               fCntrEnableIcon;      // Does calling application (called container here)
                                               //    specify OLEUIPASTE_ENABLEICON for this item?
} PASTELISTITEMDATA, *PPASTELISTITEMDATA;


//Internal function prototypes
//PASTESPL.C
unsigned int	UPasteSpecialInit(DialogPtr, LPOLEUIPASTESPECIAL);
Handle			AllocateScratchMem(PPASTESPECIAL);
unsigned int	PSUInitUserItems(DialogPtr);

Boolean			FAddPasteListItem(ListHandle, Boolean, Boolean, int, PPASTESPECIAL, char *, char *);
Boolean			FFillPasteList(PPASTESPECIAL);
Boolean			FFillPasteLinkList(PPASTESPECIAL);
Boolean			FHasPercentS(char *, PPASTESPECIAL );

#ifndef _MSC_VER
pascal Boolean	PasteSpecialDialogProc(DialogPtr, EventRecord *, short *);
pascal void		PSUserItemProc(DialogPtr, short);
#else
Boolean	__pascal PasteSpecialDialogProc(DialogPtr, EventRecord *, short *);
void __pascal 	 PSUserItemProc(DialogPtr, short);
#endif

void			PSUpdateOneControl(DialogPtr, short, Boolean);
void			PSUpdateAllControls(DialogPtr, PPASTESPECIAL);
void			PSUpdateResults(DialogPtr, PPASTESPECIAL);

void			PSShowListItem(ListHandle, Boolean);
void 			EnableDisplayAsIcon(DialogPtr pDialog, PPASTESPECIAL pPS);
void			TogglePasteType(DialogPtr, PPASTESPECIAL, unsigned long);
void 			PSUpdateDisplayAsIcon(DialogPtr pDialog, PPASTESPECIAL pPS);

void			PasteSpecialCleanup(DialogPtr);
void 			PSDrawIconLabel(DialogPtr pDialog, PPASTESPECIAL pPS);
void 			PSDrawIcon(DialogPtr pDialog, PPASTESPECIAL pPS, Boolean FocusRectOnly);
void 			PSDrawList(DialogPtr pDialog, PPASTESPECIAL pPS, Boolean FocusRectOnly);
void 			PSTabFocus(DialogPtr pDialog, PPASTESPECIAL pPS);
void 			PSSetActiveItem(DialogPtr pDialog, PPASTESPECIAL pPS, short item);
void 			PSToggleIconLabel(DialogPtr pDialog, PPASTESPECIAL pPS, short item);


#endif //!_PASTESPL_H
