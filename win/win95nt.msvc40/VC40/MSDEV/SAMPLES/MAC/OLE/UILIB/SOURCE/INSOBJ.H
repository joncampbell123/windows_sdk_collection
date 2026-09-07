/*
 * INSERTOB.H
 *
 * Internal definitions, structures, and function prototypes for the
 * OLE 2.0 UI Insert Object dialog.
 *
 * Copyright (c)1992-1994 Microsoft Corporation, All Right Reserved
 */


#ifndef _INSOBJ_H_
#define _INSOBJ_H_


#include <compobj.h>
#include "ole2ui.h"


#define DIDInsertObject		  10000		//What should these IDs be for OLE Standard dlgs?


//Internally used structure
typedef struct tagINSERTOBJECT
{
	//Keep this item first as the Standard functions depend on it here.
	LPOLEUIINSERTOBJECT     lpOIO;       //Original structure passed.

	/*
	 * What we store extra in this structure besides the original caller's
	 * pointer are those fields that we need to modify during the life of
	 * the dialog but that we don't want to change in the original structure
	 * until the user presses OK.
	 */
    unsigned int        dwFlags;
    CLSID               clsid;
	Boolean				fFirstCall,
						fFailure,
						fDoToggle;
	StandardFileReply	sfReply;
	Boolean             fObjSelected;
	Boolean				fCheckFileSelection;
	Boolean             fFileSelected;
	Boolean				fFolderSelected;
    Boolean             fAsIconNew;
    Boolean             fAsIconFile;
    Boolean             fDefaultIcon;
    Boolean				fJumpToList;
	int                 nSelectedIndex;
	char                szClass[256];		//REVIEW: How long should this be?
	char                szResult[256];
	int                 nResultIcon;
	ListHandle          hListNew;
	Boolean				fHitOK;
	PicHandle			hIcon;
	Boolean				fLabelEditActive;
	short				iActiveItem;
	FSSpec				fspSave;
	Boolean				fSavedFSP;
	Boolean 			fLabelEditInFileView;
	char                szIconLabel[OLEUI_CCHLABELMAX];		//REVIEW: How long should this be?
	FSSpec				fspLast;
	Boolean				fHaveLastFSp;
	int					nOldIndex;
	Boolean				fCheckClassSelection;
	Boolean				fForceIconUpdate;

} INSERTOBJECT, *PINSERTOBJECT, *LPINSERTOBJECT;



//Internal function prototypes
//INSOBJ.C

unsigned long	UInsertObjectInit(DialogPtr, PINSERTOBJECT);
unsigned long	IOUInitUserItems(DialogPtr, PINSERTOBJECT);
unsigned int	UFillClassList(ListHandle, unsigned int, LPCLSID, Boolean);

#ifndef _MSC_VER
pascal short	InsertObjectDialogProc(short, DialogPtr, PINSERTOBJECT);
pascal Boolean	InsertObjectModalProc(DialogPtr, EventRecord *, short *, PINSERTOBJECT);
pascal Boolean 	IOFileFilterProc(ParmBlkPtr PB, PINSERTOBJECT pIO);
pascal void		IOUserItemProc(DialogPtr, short);
#else
short	__pascal InsertObjectDialogProc(short, DialogPtr, PINSERTOBJECT);
Boolean __pascal InsertObjectModalProc(DialogPtr, EventRecord *, short *, PINSERTOBJECT);
Boolean __pascal IOFileFilterProc(ParmBlkPtr PB, PINSERTOBJECT pIO);
void __pascal	 IOUserItemProc(DialogPtr, short);
#endif

void			IODrawIconLabel(DialogPtr pDialog, PINSERTOBJECT pIO);
void 			IODrawIcon(DialogPtr pDialog, PINSERTOBJECT pIO, Boolean FocusRectOnly);
void 			IODrawClassList(DialogPtr pDialog, PINSERTOBJECT pIO, Boolean FocusRectOnly);

void			IOUpdateOneControl(DialogPtr, short, Boolean);
void			IOUpdateAllControls(DialogPtr, PINSERTOBJECT);
void			IOUpdateResults(DialogPtr, Boolean);

void			IOToggleObjectSource(DialogPtr, PINSERTOBJECT, unsigned int);
void 			IOUpdateDisplayAsIcon(DialogPtr pDialog, PINSERTOBJECT pIO);

#ifndef _MSC_VER
pascal void 	IOActivateProc(DialogPtr, short, Boolean, PINSERTOBJECT);
#else
void __pascal	IOActivateProc(DialogPtr, short, Boolean, PINSERTOBJECT);
#endif
void 			IOActivateItem(short iitem, PINSERTOBJECT pIO);

void 			IOFileListLosingFocus(DialogPtr pDialog, PINSERTOBJECT pIO);

void 			IOToggleIconLabel(DialogPtr pDialog, PINSERTOBJECT pIO, short item);
void 			IOUpdateIconArea(DialogPtr pDialog, PINSERTOBJECT pIO, Boolean fErase);

Boolean 		IOCheckClassSelection(PINSERTOBJECT pIO);
Boolean			IOCheckFileSelection(PINSERTOBJECT pIO);
void 			IOCheckResults(PINSERTOBJECT pIO);

#endif //!_INSOBJ_H_
