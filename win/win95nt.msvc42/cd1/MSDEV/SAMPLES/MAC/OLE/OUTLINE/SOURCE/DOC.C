/*****************************************************************************\
*                                                                             *
*    Document.c                                                               *
*                                                                             *
*    OLE Version 2.0 Sample Code                                              *
*                                                                             *
*    Copyright (c) 1992-1994, Microsoft Corp. All rights reserved.            *
*                                                                             *
\*****************************************************************************/

#if !defined(_MSC_VER) && !defined(THINK_C)
#include "OLine.h"
#endif

#include "Types.h"

#if defined(USEHEADER)
#include "OleHdrs.h"
#endif
#include "Debug.h"
#include "OleDebug.h"
#include "Doc.h"
#include "Const.h"
#include "Gopher.h"
#if qFrameTools
	#include "Layers.h"
	#include "Toolbar.h"
#endif
#include "Util.h"
#include "OleXcept.h"
#include "Line.h"
#include "App.h"
#include "PObj.h"
#include "DXferDoc.h"
#include "Fonts.h"
#include "Script.h"
#include <ToolUtils.h>
#include <TextUtils.h>

#if qOle
    OLEDBGDATA
#endif

#ifndef _MSC_VER
static pascal Boolean SaveDialogFilter(DialogPtr theDialog, EventRecord *theEvent, short *itemHit);
#else
static Boolean __pascal SaveDialogFilter(DialogPtr theDialog, EventRecord *theEvent, short *itemHit);
#endif

#ifdef __powerc
static ModalFilterUPP gSaveDialogFilter = NULL;
#endif


static RGBColor	gWhite  = { 65535, 65535, 65535 },
				gLtGray = { 49344, 49344, 49344 },
				gDkGray = { 32896, 32896, 32896 },
				gBlack  = {     0,     0,     0 };

// OLDNAME: Document.c

extern ApplicationPtr	gApplication;
static DocumentVtblPtr		gDocVtbl = nil;

//#pragma segment VtblInitSeg
DocumentVtblPtr DocGetVtbl()
{
	ASSERTCOND(gDocVtbl != nil);
	return gDocVtbl;
}

//#pragma segment VtblInitSeg
void DocInitVtbl()
{
	DocumentVtblPtr	vtbl;

	vtbl = gDocVtbl = (DocumentVtblPtr)NewPtrClear(sizeof(*vtbl));
	ASSERTCOND(vtbl != nil);
	FailNIL(vtbl);
		
	InheritFromVtbl(vtbl, WinGetVtbl());

	vtbl->m_DisposeProcPtr =			DocDispose;

	vtbl->m_DoActivateProcPtr =			DocDoActivate;
	vtbl->m_HideProcPtr =				DocHide;

	vtbl->m_DoKeyDownProcPtr =			DocDoKeyDown;
	vtbl->m_ScrollRectProcPtr =			DocScrollRect;

	vtbl->m_SetFSSpecProcPtr =			DocSetFSSpec;
	vtbl->m_IsEqualFSSpecProcPtr =		DocIsEqualFSSpec;

	vtbl->m_NeedsSaveProcPtr =			DocNeedsSave;
	vtbl->m_DoCloseProcPtr = 			DocDoClose;
	vtbl->m_IsClosingProcPtr =			DocIsClosing;
	vtbl->m_OpenFileProcPtr = 			DocOpenFile;
	vtbl->m_CloseFileProcPtr =			DocCloseFile;
	vtbl->m_DoSaveProcPtr =				DocDoSave;
	vtbl->m_DoSaveAsProcPtr =			DocDoSaveAs;
	vtbl->m_DoRevertProcPtr =			DocDoRevert;
	vtbl->m_DoNewProcPtr =				DocDoNew;
	vtbl->m_DoOpenProcPtr =				DocDoOpen;
	vtbl->m_WriteToFileProcPtr =		DocWriteToFile;
	vtbl->m_ReadFromFileProcPtr =		DocReadFromFile;
	vtbl->m_SetDefaultCursorProcPtr =	DocSetDefaultCursor;
	vtbl->m_PresentSaveDialogProcPtr =	DocPresentSaveDialog;

	vtbl->m_CanCutProcPtr =				DocCanCut;
	vtbl->m_CanCopyProcPtr =			DocCanCopy;
	vtbl->m_CanPasteProcPtr =			DocCanPaste;
	vtbl->m_CanClearProcPtr =			DocCanClear;
	vtbl->m_CanSaveProcPtr =			DocCanSave;
	vtbl->m_CanSaveAsProcPtr =			DocCanSaveAs;
	vtbl->m_CanRevertProcPtr =			DocCanRevert;

	vtbl->m_DoCutProcPtr =				DocDoCut;
	vtbl->m_DoCopyProcPtr =				DocDoCopy;
	vtbl->m_DoPasteProcPtr =			DocDoPaste;
	vtbl->m_DoClearProcPtr =			DocDoClear;

	vtbl->m_IsDirtyProcPtr =			DocIsDirty;
	vtbl->m_SetDirtyProcPtr =			DocSetDirty;

	vtbl->m_SetBorderSpaceProcPtr =		DocSetBorderSpace;
	vtbl->m_EnableFrameToolsProcPtr =	DocEnableFrameTools;

	ASSERTCOND(ValidVtbl(vtbl, sizeof(*vtbl)));
}

//#pragma segment VtblDisposeSeg
void DocDisposeVtbl()
{
	ASSERTCOND(gDocVtbl != nil);
	DisposePtr((Ptr)gDocVtbl);
}

//#pragma segment DocumentSeg
void DocInit(DocumentPtr pDoc, short resID, OSType fileType, Boolean fDataTransferDoc)
{
	WinInit((WinPtr)pDoc, resID, false);

	pDoc->baseVtbl = (WinVtblPtr)pDoc->vtbl;
	pDoc->vtbl = DocGetVtbl();

	pDoc->m_GopherUpdateMenus = GopherNewUpdateMenus((UpdateMenusProcPtr)DocUpdateMenus, pDoc);
	FailNIL(pDoc->m_GopherUpdateMenus);

	pDoc->m_GopherDoCmd = GopherNewDoCmd((DoCmdProcPtr)DocDoCmd, pDoc);
	FailNIL(pDoc->m_GopherDoCmd);

	// the data transfer document is never made visible,
	// nor does it need a name.
	if (!fDataTransferDoc)
	{
		ApplicationPtr	pApp = gApplication;
		
		ASSERTCOND(pApp->vtbl->m_GetNextNewFileNameProcPtr != nil);
		(*pApp->vtbl->m_GetNextNewFileNameProcPtr)(pApp, &pDoc->m_File);
	}

	pDoc->m_FileRefNum = -1;
	pDoc->m_DocType = kUnknownDocumentType;
	pDoc->m_FileCreator = gApplication->m_Creator;
	pDoc->m_fDataTransferDoc = fDataTransferDoc;
	pDoc->m_ReadOnly = false;
	pDoc->m_FileType = fileType;
	pDoc->m_Active = false;

	pDoc->m_Dirty = false;
	
	ASSERTCOND(pDoc->vtbl->m_SetWindowTitleProcPtr != nil);
	(*pDoc->vtbl->m_SetWindowTitleProcPtr)(pDoc, pDoc->m_File.name);
}

//#pragma segment DocumentSeg
void DocDispose(DocumentPtr pDoc)
{
	ASSERTCOND(pDoc->vtbl->m_CloseFileProcPtr != nil);
	(*pDoc->vtbl->m_CloseFileProcPtr)(pDoc);

	GopherDispose(pDoc->m_GopherUpdateMenus);
	GopherDispose(pDoc->m_GopherDoCmd);
	
	ASSERTCOND(pDoc->baseVtbl->m_DisposeProcPtr != nil);
	(*pDoc->baseVtbl->m_DisposeProcPtr)((WinPtr)pDoc);

	// make sure the reference to the front document is updated
	ASSERTCOND(gApplication->vtbl->m_UpdateCurrentDocProcPtr != nil);
	(*gApplication->vtbl->m_UpdateCurrentDocProcPtr)(gApplication);
}

//#pragma segment DocumentSeg
void DocDoActivate(DocumentPtr pDoc, Boolean becomingActive)
{
	// test to make sure we have not already seen activate event
	ASSERTCOND(pDoc->m_Active != becomingActive);
	
	pDoc->m_Active = becomingActive;

	if (becomingActive)
	{
		GopherAdd(pDoc->m_GopherUpdateMenus);
		GopherAdd(pDoc->m_GopherDoCmd);
	}
	else
	{
		GopherRemove(pDoc->m_GopherUpdateMenus);
		GopherRemove(pDoc->m_GopherDoCmd);
	}
}

//#pragma segment DocumentSeg
void DocHide(DocumentPtr pDoc)
{
	ASSERTCOND(pDoc->baseVtbl->m_HideProcPtr != nil);
	(*pDoc->baseVtbl->m_HideProcPtr)((WinPtr)pDoc);
}

//#pragma segment DocumentSeg
void DocDoKeyDown(DocumentPtr pDoc, EventRecord* theEvent)
{
}

//#pragma segment DocumentSeg
void DocScrollRect(DocumentPtr pDoc, short distHoriz, short distVert)
{
}

//#pragma segment DocumentSeg
void DocGetFSSpec(DocumentPtr pDoc, FSSpecPtr pFile)
{
	*pFile = pDoc->m_File;
}

/* DocSetFSSpec
 * ----------
 *
 *	Purpose:
 *      Set the FSSpec of document.
 *
 */
//#pragma segment DocumentSeg
void DocSetFSSpec(DocumentPtr pDoc, FSSpecPtr pFile)
{
	/* A new valid file record is being associated with the document */
	
	pDoc->m_File = *pFile;
	pDoc->m_DocType = kFromFileDocumentType;
}

/* DocIsEqualFSSpec
 * ----------------
 *
 *	Purpose:
 *      Compare the FSSpec of document to see if equal
 *
 */
//#pragma segment DocumentSeg
Boolean DocIsEqualFSSpec(DocumentPtr pDoc, FSSpecPtr pFile)
{
	return FSSpecCompare(&pDoc->m_File, pFile);
}

//#pragma segment DocumentSeg
YNCResult DocNeedsSave(DocumentPtr pDoc, Boolean askUserToSave, YNCResult defaultAnswer, Boolean quitting)
{
	ASSERTCOND(pDoc->vtbl->m_IsVisibleProcPtr != nil);
	ASSERTCOND(pDoc->vtbl->m_IsDirtyProcPtr != nil);

	if ((*pDoc->vtbl->m_IsDirtyProcPtr)(pDoc))
	{
		if ((*pDoc->vtbl->m_IsVisibleProcPtr)(pDoc) && askUserToSave)
		{
			ASSERTCOND(pDoc->vtbl->m_PresentSaveDialogProcPtr != nil);
			return (*pDoc->vtbl->m_PresentSaveDialogProcPtr)(pDoc, quitting);
		}
		else
			return defaultAnswer;
	}
	
	return noResult;
}

//#pragma segment DocumentSeg
YNCResult DocDoClose(DocumentPtr pDoc, Boolean askUserToSave, YNCResult defaultAnswer, Boolean quitting)
{
	YNCResult	res;
#if qFrameTools
	WindowPtr	pSavedLayer;

	pSavedLayer = SwapLayer(gApplication->m_pDocLayer);
#endif

	ASSERTCOND(pDoc->m_fIsClosing == false);
	
	// While closing set the flag
	pDoc->m_fIsClosing = true;
	
	ASSERTCOND(pDoc->vtbl->m_NeedsSaveProcPtr != nil);
	res = (*pDoc->vtbl->m_NeedsSaveProcPtr)(pDoc, askUserToSave, defaultAnswer, quitting);

	ASSERTCOND(pDoc->vtbl->m_IsDirtyProcPtr != nil);
	if ((*pDoc->vtbl->m_IsDirtyProcPtr)(pDoc))
	{
		if (res == yesResult)
		{
			ASSERTCOND(pDoc->vtbl->m_DoSaveProcPtr != nil);
			(*pDoc->vtbl->m_DoSaveProcPtr)(pDoc);
	
			if ((*pDoc->vtbl->m_IsDirtyProcPtr)(pDoc))
				res = cancelResult;
		}
	}
	else if (res == noResult)
		res = yesResult;
	
	if (res != cancelResult)
	{
		ASSERTCOND(pDoc->vtbl->m_HideProcPtr != nil);
		(*pDoc->vtbl->m_HideProcPtr)(pDoc);

		// if we are active, make sure we deactivate
		if (pDoc->m_Active)
		{
			ASSERTCOND(pDoc->vtbl->m_DoActivateProcPtr != nil);
			(*pDoc->vtbl->m_DoActivateProcPtr)(pDoc, false);
		}
	}
	
	// Either we closed, or are about to be disposed. Reset the flag
	pDoc->m_fIsClosing = false;

#if qFrameTools
	SetLayer(pSavedLayer);
#endif

	return res;
}

//#pragma segment DocumentSeg
Boolean DocIsClosing(DocumentPtr pDoc)
{
	return pDoc->m_fIsClosing;
}

//#pragma segment DocumentSeg
void DocDoNew(DocumentPtr pDoc)
{
	ASSERTCOND(pDoc->m_DocType == kUnknownDocumentType);
	
	pDoc->m_DocType = kNewDocumentType;
}

//#pragma segment DocumentSeg
void DocDoOpen(DocumentPtr pDoc, FSSpecPtr theFile, Boolean readOnly)
{
	pDoc->m_DocType = kFromFileDocumentType;
	pDoc->m_ReadOnly = readOnly;

	ASSERTCOND(pDoc->vtbl->m_SetFSSpecProcPtr != nil);
	(*pDoc->vtbl->m_SetFSSpecProcPtr)(pDoc, theFile);

	ASSERTCOND(pDoc->vtbl->m_SetDirtyProcPtr != nil);
	(*pDoc->vtbl->m_SetDirtyProcPtr)(pDoc, false);

	ASSERTCOND(pDoc->vtbl->m_OpenFileProcPtr != nil);
	(*pDoc->vtbl->m_OpenFileProcPtr)(pDoc, readOnly, false);

	ASSERTCOND(pDoc->vtbl->m_ReadFromFileProcPtr != nil);
	(*pDoc->vtbl->m_ReadFromFileProcPtr)(pDoc);

	ASSERTCOND(pDoc->vtbl->m_SetWindowTitleProcPtr != nil);
	(*pDoc->vtbl->m_SetWindowTitleProcPtr)(pDoc, pDoc->m_File.name);

	SetPort(pDoc->m_WindowPtr);
}

//#pragma segment DocumentSeg
void DocDoSave(DocumentPtr pDoc)
{
	ASSERTCOND(pDoc->vtbl->m_IsDirtyProcPtr != nil);
	if (!(*pDoc->vtbl->m_IsDirtyProcPtr)(pDoc))
		return;

	if (pDoc->m_DocType == kNewDocumentType)
	{
		ASSERTCOND(pDoc->vtbl->m_DoSaveAsProcPtr != nil);
		(*pDoc->vtbl->m_DoSaveAsProcPtr)(pDoc);
		return;
	}

	ASSERTCOND(pDoc->vtbl->m_WriteToFileProcPtr != nil);
	(*pDoc->vtbl->m_WriteToFileProcPtr)(pDoc);

	ASSERTCOND(pDoc->vtbl->m_SetDirtyProcPtr != nil);
	(*pDoc->vtbl->m_SetDirtyProcPtr)(pDoc, false);
}

//#pragma segment DocumentSeg
void DocDoSaveAs(DocumentPtr pDoc)
{
	StandardFileReply	reply;

	StandardPutFile("\p", pDoc->m_File.name, &reply);

	if (reply.sfGood == false)
		return;

	// make sure it is closed
	ASSERTCOND(pDoc->vtbl->m_CloseFileProcPtr != nil);
	(*pDoc->vtbl->m_CloseFileProcPtr)(pDoc);

	ASSERTCOND(pDoc->vtbl->m_SetFSSpecProcPtr != nil);
	(*pDoc->vtbl->m_SetFSSpecProcPtr)(pDoc, &reply.sfFile);

	ASSERTCOND(pDoc->vtbl->m_OpenFileProcPtr != nil);
	(*pDoc->vtbl->m_OpenFileProcPtr)(pDoc, false, true);

	ASSERTCOND(pDoc->vtbl->m_SetWindowTitleProcPtr != nil);
	(*pDoc->vtbl->m_SetWindowTitleProcPtr)(pDoc, pDoc->m_File.name);

	ASSERTCOND(pDoc->vtbl->m_WriteToFileProcPtr != nil);
	(*pDoc->vtbl->m_WriteToFileProcPtr)(pDoc);

	pDoc->m_DocType = kFromFileDocumentType;

	ASSERTCOND(pDoc->vtbl->m_SetDirtyProcPtr != nil);
	(*pDoc->vtbl->m_SetDirtyProcPtr)(pDoc, false);
}

//#pragma segment DocumentSeg
void DocDoRevert(DocumentPtr pDoc)
{
}


//#pragma segment DocumentSeg
YNCResult DocPresentSaveDialog(DocumentPtr pDoc, Boolean quitting)
{
	Str255		buzzword;

	// make sure the document is in selected, and the app is the front process
	ASSERTCOND(pDoc->vtbl->m_BringToFrontProcPtr != nil);
	(*pDoc->vtbl->m_BringToFrontProcPtr)(pDoc);

	SetCursor(&qd.arrow);

	GetIndString(buzzword, (short)kAppBuzzword_STRs, (short)(quitting ? kQuiting_AppBuzzword : kClosing_AppBuzzword));

	ParamText(pDoc->m_File.name, buzzword, "\p", "\p");

	{
		DialogPtr	theDialog;
		short		item;

		theDialog = GetNewDialog(kSave_DLOG, nil, (WindowPtr)-1);
		FailNIL(theDialog);

#ifndef __powerc
		ModalDialog(SaveDialogFilter, &item);
#else
		if (gSaveDialogFilter == NULL)
			gSaveDialogFilter =  NewModalFilterProc(SaveDialogFilter);

		ModalDialog(gSaveDialogFilter, &item);
#endif

		DisposeDialog(theDialog);

		switch(item)
		{
			case 1:
			default:
				return yesResult;
			case 2:
				return cancelResult;
			case 3:
				return noResult;
		}
	}
}

//#pragma segment DocumentSeg
#ifndef _MSC_VER
static pascal Boolean
#else
static Boolean __pascal
#endif
SaveDialogFilter(DialogPtr theDialog, EventRecord *theEvent, short *itemHit)
{
	char			theChar;
	ApplicationPtr	pApp = gApplication;
	GrafPtr			oldPort;
	DocumentPtr		pDoc;
	
	// case on the event
	switch (theEvent->what) {
		case updateEvt:
			{
				WindowPtr		whichWindow;
	
				whichWindow = (WindowPtr)theEvent->message;
				if (whichWindow == theDialog)
				{
					PenState	penState;
					short		type;
					Handle		h;
					Rect		r;
					
					GetPort(&oldPort);
					SetPort(theDialog);
					
					GetPenState(&penState);
				  	PenSize(3,3);
					
					GetDItem(theDialog, 1, &type, &h, &r);
					InsetRect(&r, -4, -4);
				  	FrameRoundRect(&r, 16, 16);

					SetPenState(&penState);
					
					SetPort(oldPort);
				}
				else
				{
					// check if window belongs to a document
					ASSERTCOND(pApp->vtbl->m_FindDocProcPtr != nil);
					pDoc = (*pApp->vtbl->m_FindDocProcPtr)(pApp, whichWindow);
				
					GetPort(&oldPort);
					SetPort(whichWindow);
					
					BeginUpdate(whichWindow);
				
					if (pDoc != nil)
					{
						ASSERTCOND(pDoc->vtbl->m_DoUpdateProcPtr != nil);
						(*pDoc->vtbl->m_DoUpdateProcPtr)(pDoc);
					}
				
					EndUpdate(whichWindow);
					
					SetPort(oldPort);
				}
			}
			break;	
				
		case autoKey:
		case keyDown:
			// if key was pressed, handle return key
			theChar = (theEvent->message) & charCodeMask;
			
			switch (theChar) {
				case 'y':
				case 'Y':
				case 13:			// <Return> key
				case 3:				// <Enter> key
					FlashItem(theDialog, 1, kDefFlashItemTime);		// Save
					*itemHit = 1;
					return true;
					
				case 'c':
				case 'C':
				case '.':	
				case 27:			// <Esc> key
					FlashItem(theDialog, 2, kDefFlashItemTime);		// Cancel
					*itemHit = 2;		
					return true;
					
				case 'd':
				case 'D':
				case 'n':
				case 'N':
					FlashItem(theDialog, 3, kDefFlashItemTime);		// don't save
					*itemHit = 3;
					return true;
					
			}

			break;				
	}
	
	return false;

}

//#pragma segment DocumentSeg
void DocOpenFile(DocumentPtr pDoc, Boolean readOnly, Boolean newFile)
{

	if (newFile)
	{
		FSpDelete(&pDoc->m_File);
		FailOSErr(FSpCreate(&pDoc->m_File, gApplication->m_Creator, pDoc->m_FileType, smSystemScript));
	}

	FailOSErr(FSpOpenDF(&pDoc->m_File, (char)(readOnly ? fsRdPerm : fsWrPerm), &pDoc->m_FileRefNum));
}

//#pragma segment DocumentSeg
void DocCloseFile(DocumentPtr pDoc)
{
	if (pDoc->m_FileRefNum != -1)
	{
		FailOSErr(FSClose(pDoc->m_FileRefNum));
		pDoc->m_FileRefNum = -1;
	}
}

//#pragma segment DocumentSeg
void DocWriteToFile(DocumentPtr pDoc)
{
}

//#pragma segment DocumentSeg
void DocReadFromFile(DocumentPtr pDoc)
{
}

//#pragma segment DocumentSeg
void DocSetDefaultCursor(DocumentPtr pDoc)
{
	ASSERTCOND(gApplication->vtbl->m_SetIdleCursorProcPtr != nil);
	(*gApplication->vtbl->m_SetIdleCursorProcPtr)(gApplication, &qd.arrow);
}

//#pragma segment DocumentSeg
void DocUpdateMenus(DocumentPtr pDoc)
{
	ASSERTCOND(pDoc->vtbl->m_CanSaveProcPtr != nil);
	if ((*pDoc->vtbl->m_CanSaveProcPtr)(pDoc))		EnableCmd(cmdSave);
	
	ASSERTCOND(pDoc->vtbl->m_CanSaveAsProcPtr != nil);
	if ((*pDoc->vtbl->m_CanSaveAsProcPtr)(pDoc))	EnableCmd(cmdSaveAs);
	
	ASSERTCOND(pDoc->vtbl->m_CanRevertProcPtr != nil);
	if ((*pDoc->vtbl->m_CanRevertProcPtr)(pDoc))	EnableCmd(cmdRevert);

	ASSERTCOND(pDoc->vtbl->m_CanCutProcPtr != nil);
	if ((*pDoc->vtbl->m_CanCutProcPtr)(pDoc))		EnableCmd(cmdCut);

	ASSERTCOND(pDoc->vtbl->m_CanCopyProcPtr != nil);
	if ((*pDoc->vtbl->m_CanCopyProcPtr)(pDoc))		EnableCmd(cmdCopy);

	ASSERTCOND(pDoc->vtbl->m_CanPasteProcPtr != nil);
	if ((*pDoc->vtbl->m_CanPasteProcPtr)(pDoc))		EnableCmd(cmdPaste);

	ASSERTCOND(pDoc->vtbl->m_CanClearProcPtr != nil);
	if ((*pDoc->vtbl->m_CanClearProcPtr)(pDoc))		EnableCmd(cmdClear);

	EnableCmd(cmdClose);

	InheritUpdateMenus(pDoc->m_GopherUpdateMenus);
}

//#pragma segment DocumentSeg
void DocDoCmd(DocumentPtr pDoc, long cmd)
{
	DocumentVtblPtr	vtbl;

	vtbl = pDoc->vtbl;

	switch(cmd)
	{
		case cmdClose:
			ASSERTCOND(vtbl->m_DoCloseProcPtr != nil);
			if ((*vtbl->m_DoCloseProcPtr)(pDoc, true, yesResult, false) != cancelResult)
			{
				ASSERTCOND(vtbl->m_FreeProcPtr != nil);
				(*vtbl->m_FreeProcPtr)(pDoc);
			}
			break;

		case cmdSave:
			ASSERTCOND(vtbl->m_DoSaveProcPtr != nil);
			(*vtbl->m_DoSaveProcPtr)(pDoc);
			break;

		case cmdSaveAs:
			ASSERTCOND(vtbl->m_DoSaveAsProcPtr != nil);
			(*vtbl->m_DoSaveAsProcPtr)(pDoc);
			break;

		case cmdRevert:
			ASSERTCOND(vtbl->m_DoRevertProcPtr != nil);
			(*vtbl->m_DoRevertProcPtr)(pDoc);
			break;
			
		case cmdCut:
			ASSERTCOND(vtbl->m_DoCutProcPtr != nil);
			(*vtbl->m_DoCutProcPtr)(pDoc);
			break;
			
		case cmdCopy:
			ASSERTCOND(vtbl->m_DoCopyProcPtr != nil);
			(*vtbl->m_DoCopyProcPtr)(pDoc);
			break;
			
		case cmdPaste:
			ASSERTCOND(vtbl->m_DoPasteProcPtr != nil);
			(*vtbl->m_DoPasteProcPtr)(pDoc);
			break;
			
		case cmdClear:
			ASSERTCOND(vtbl->m_DoClearProcPtr != nil);
			(*vtbl->m_DoClearProcPtr)(pDoc);
			break;
			

		default:
			InheritDoCmd(pDoc->m_GopherDoCmd, cmd);
			break;
	}
}

//#pragma segment DocumentSeg
Boolean DocIsDirty(DocumentPtr pDoc)
{
	return pDoc->m_Dirty;
}

//#pragma segment DocumentSeg
void DocSetDirty(DocumentPtr pDoc, Boolean fDirty)
{
	pDoc->m_Dirty = fDirty;
}

//#pragma segment DocumentSeg
Boolean DocCanCopy(DocumentPtr pDoc)
{
	return false;
}

//#pragma segment DocumentSeg
Boolean DocCanCut(DocumentPtr pDoc)
{
	return false;
}

//#pragma segment DocumentSeg
Boolean DocCanPaste(DocumentPtr pDoc)
{
	return false;
}

//#pragma segment DocumentSeg
Boolean DocCanClear(DocumentPtr pDoc)
{
	return false;
}

//#pragma segment DocumentSeg
Boolean DocCanSave(DocumentPtr pDoc)
{
	ASSERTCOND(pDoc->vtbl->m_IsDirtyProcPtr != nil);
	return (*pDoc->vtbl->m_IsDirtyProcPtr)(pDoc);
}

//#pragma segment DocumentSeg
Boolean DocCanSaveAs(DocumentPtr pDoc)
{	
	// enable save as for all documents except embeddings
	return pDoc->m_DocType != kEmbeddedDocumentType;
}

//#pragma segment DocumentSeg
Boolean DocCanRevert(DocumentPtr pDoc)
{
	return false;
}

//#pragma segment DocumentSeg
void DocDoCopy(DocumentPtr pDoc)
{
}

//#pragma segment DocumentSeg
void DocDoCut(DocumentPtr pDoc)
{
	ASSERTCOND(pDoc->vtbl->m_DoCopyProcPtr != nil);
	(pDoc->vtbl->m_DoCopyProcPtr)(pDoc);
	
	ASSERTCOND(pDoc->vtbl->m_DoClearProcPtr != nil);
	(pDoc->vtbl->m_DoClearProcPtr)(pDoc);	
}

//#pragma segment DocumentSeg
void DocDoPaste(DocumentPtr pDoc)
{
}

//#pragma segment DocumentSeg
void DocDoClear(DocumentPtr pDoc)
{
}

//#pragma segment DocumentSeg
void DocSetBorderSpace(DocumentPtr pDoc, Rect* prectBorder)
{
	if (prectBorder != nil)
		pDoc->m_BorderWidths = *prectBorder;

#if qFrameTools
	ASSERTCOND(pDoc->vtbl->m_EnableFrameToolsProcPtr != nil);
	(*pDoc->vtbl->m_EnableFrameToolsProcPtr)(pDoc, (Boolean)(prectBorder == nil), false);
#endif
}

//#pragma segment DocumentSeg
void DocEnableFrameTools(DocumentPtr pDoc, Boolean fEnable, Boolean fSetBorder)
{
#if qFrameTools
	Rect	rBorder;

	rBorder = pDoc->m_BorderWidths;
	pDoc->m_fShowFrameTools = fEnable;

	if (fEnable)
	{
		ASSERTCOND(gApplication->vtbl->m_FrameSpaceNeededProcPtr != nil);
		(*gApplication->vtbl->m_FrameSpaceNeededProcPtr)(gApplication, &rBorder);

		pDoc->m_BorderWidths = rBorder;
	}
	else
	{
		ASSERTCOND(gApplication->vtbl->m_HideFrameToolsProcPtr != nil);
		(*gApplication->vtbl->m_HideFrameToolsProcPtr)(gApplication);
	}

	if (fSetBorder)
	{
		ASSERTCOND(gApplication->vtbl->m_SetBorderSpaceProcPtr != nil);
		(*gApplication->vtbl->m_SetBorderSpaceProcPtr)(gApplication, &rBorder, false);
	}

	if (fEnable)
	{
		ASSERTCOND(gApplication->vtbl->m_ShowFrameToolsProcPtr != nil);
		(*gApplication->vtbl->m_ShowFrameToolsProcPtr)(gApplication);
	}
#endif
}

// OLDNAME: OutlineDocument.c
extern ApplicationPtr	gApplication;
static OutlineDocumentVtblPtr		gOutlineDocVtbl = nil;

//#pragma segment VtblInitSeg
OutlineDocumentVtblPtr OutlineDocGetVtbl()
{
	ASSERTCOND(gOutlineDocVtbl != nil);
	return gOutlineDocVtbl;
}

//#pragma segment VtblInitSeg
void OutlineDocInitVtbl()
{
	OutlineDocumentVtblPtr	vtbl;
	
	vtbl = gOutlineDocVtbl = (OutlineDocumentVtblPtr)NewPtrClear(sizeof(*vtbl));
	ASSERTCOND(vtbl != nil);
	FailNIL(vtbl);

	InheritFromVtbl(vtbl, DocGetVtbl());

	vtbl->m_DisposeProcPtr =		OutlineDocDispose;
	
	vtbl->m_DoActivateProcPtr =		OutlineDocDoActivate;
	vtbl->m_DoGrowProcPtr =			OutlineDocDoGrow;
	vtbl->m_DoZoomProcPtr =			OutlineDocDoZoom;
	vtbl->m_DoUpdateProcPtr =		OutlineDocDoUpdate;
	vtbl->m_DoContentProcPtr =		OutlineDocDoContent;
	vtbl->m_ScrollRectProcPtr =		OutlineDocScrollRect;

	vtbl->m_DoOpenProcPtr =			OutlineDocDoOpen;

	vtbl->m_CanCutProcPtr =			OutlineDocCanCut;
	vtbl->m_CanCopyProcPtr =		OutlineDocCanCopy;
	vtbl->m_CanPasteProcPtr =		OutlineDocCanPaste;
	vtbl->m_CanClearProcPtr =		OutlineDocCanClear;
	
	vtbl->m_DoCopyProcPtr =			OutlineDocDoCopy;
	vtbl->m_DoPasteProcPtr =		OutlineDocDoPaste;
	vtbl->m_DoClearProcPtr =		OutlineDocDoClear;
	
	vtbl->m_ShowProcPtr =			OutlineDocShow;

	vtbl->m_DoIdleProcPtr =			OutlineDocDoIdle;

	ASSERTCOND(ValidVtbl(vtbl, sizeof(*vtbl)));
}

//#pragma segment VtblDisposeSeg
void OutlineDocDisposeVtbl()
{
	ASSERTCOND(gOutlineDocVtbl != nil);
	DisposePtr((Ptr)gOutlineDocVtbl);
}

//#pragma segment OutlineDocumentSeg
void OutlineDocInit(OutlineDocPtr pOutlineDoc, short resID, OSType fileType, Boolean fDataTransferDoc)
{
	DocInit(&pOutlineDoc->superClass, resID, fileType, fDataTransferDoc);

	pOutlineDoc->vtbl = ((DocumentPtr)pOutlineDoc)->vtbl;
	((DocumentPtr)pOutlineDoc)->vtbl = OutlineDocGetVtbl();

	pOutlineDoc->m_GopherUpdateMenus = GopherNewUpdateMenus((UpdateMenusProcPtr)OutlineDocUpdateMenus, pOutlineDoc);
	FailNIL(pOutlineDoc->m_GopherUpdateMenus);

	pOutlineDoc->m_GopherDoCmd = GopherNewDoCmd((DoCmdProcPtr)OutlineDocDoCmd, pOutlineDoc);
	FailNIL(pOutlineDoc->m_GopherDoCmd);

	TextFont(geneva);
	TextSize(10);

	pOutlineDoc->m_fShowRowColumnHeadings = true;
	pOutlineDoc->m_fDrawColor = (GetWindowMinDepth(((DocumentPtr)pOutlineDoc)->m_WindowPtr) >= 4);

	{
		Rect		rBounds;
		
		pOutlineDoc->m_LineList = (LineListPtr)NewPtrClear(sizeof(LineListRec));
		ASSERTCOND(pOutlineDoc->m_LineList != nil);
		FailNIL(pOutlineDoc->m_LineList);

		OutlineDocGetLineListBounds(pOutlineDoc, &rBounds);
		LineListInit(pOutlineDoc->m_LineList, ((DocumentPtr)pOutlineDoc)->m_WindowPtr, &rBounds, (DocumentPtr)pOutlineDoc);

		pOutlineDoc->m_NameTable = (NameTablePtr)NewPtrClear(sizeof(NameTableRec));
		ASSERTCOND(pOutlineDoc->m_NameTable != nil);
		FailNIL(pOutlineDoc->m_NameTable);

		NameTableInit(pOutlineDoc->m_NameTable, (DocumentPtr)pOutlineDoc);
	}
	
	pOutlineDoc->m_nNextRangeNo = 0;
}

//#pragma segment OutlineDocumentSeg
void OutlineDocDispose(DocumentPtr pDoc)
{
	OutlineDocPtr	pOutlineDoc;

	pOutlineDoc = (OutlineDocPtr)pDoc;

	if (pOutlineDoc->m_LineList != nil)
		LineListDispose(pOutlineDoc->m_LineList);

	if (pOutlineDoc->m_NameTable != nil)
		NameTableDispose(pOutlineDoc->m_NameTable);

	GopherDispose(pOutlineDoc->m_GopherUpdateMenus);
	GopherDispose(pOutlineDoc->m_GopherDoCmd);

	ASSERTCOND(pOutlineDoc->vtbl->m_DisposeProcPtr != nil);
	(*pOutlineDoc->vtbl->m_DisposeProcPtr)(pDoc);

	// reset the idle cursor
	ASSERTCOND(gApplication->vtbl->m_SetIdleCursorProcPtr != nil);
	(*gApplication->vtbl->m_SetIdleCursorProcPtr)(gApplication, &qd.arrow);
}

//#pragma segment OutlineDocumentSeg
void OutlineDocDoActivate(DocumentPtr pDoc, Boolean becomingActive)
{
	OutlineDocPtr	pOutlineDoc = (OutlineDocPtr)pDoc;

	ASSERTCOND(pOutlineDoc->vtbl->m_DoActivateProcPtr != nil);
	(*pOutlineDoc->vtbl->m_DoActivateProcPtr)(pDoc, becomingActive);

	LineListActivate(pOutlineDoc->m_LineList, becomingActive);

	if (becomingActive)
	{
		GopherAdd(pOutlineDoc->m_GopherUpdateMenus);
		GopherAdd(pOutlineDoc->m_GopherDoCmd);
	}
	else
	{
		GopherRemove(pOutlineDoc->m_GopherUpdateMenus);
		GopherRemove(pOutlineDoc->m_GopherDoCmd);
	}

	OutlineDocDrawGrowIcon(pOutlineDoc);
}

//#pragma segment OutlineDocumentSeg
void OutlineDocDoCmd(OutlineDocPtr pOutlineDoc, long cmd)
{
	switch(cmd)
	{
		case cmdNewLine:
			OutlineDocDoNewLine(pOutlineDoc);
			break;

		case cmdIndentLine:
		case cmdUnIndentLine:
			OutlineDocIndentLine(pOutlineDoc, (Boolean)(cmd == cmdIndentLine));
			break;
			
		case cmdDoubleClick:
			LineListDoDoubleClick(pOutlineDoc->m_LineList);
			break;
		
		case cmdSelectAll:
			LineListSelectRange(pOutlineDoc->m_LineList, nil);
			break;
			
		case cmdShowName:
			NameTableDoShowName(pOutlineDoc->m_NameTable);
			break;

		default:
			InheritDoCmd(pOutlineDoc->m_GopherDoCmd, cmd);
			break;
	}
}

//#pragma segment OutlineDocumentSeg
void OutlineDocDoNewLine(OutlineDocPtr pOutlineDoc)
{
	{
		static short	gLineIndex = 1;
		Str255			str;
		Str32			s;

		pStrCopy(str, "\pTest line ");
		NumToString(gLineIndex, s);
		pStrCat(str, s);
		LineListNewTextLine(pOutlineDoc->m_LineList, str);

		gLineIndex++;
	}

	// set document dirty
	ASSERTCOND(((DocumentPtr)pOutlineDoc)->vtbl->m_SetDirtyProcPtr != nil);
	(*((DocumentPtr)pOutlineDoc)->vtbl->m_SetDirtyProcPtr)((DocumentPtr)pOutlineDoc, true);
	
#if qOle
	OleDocSetSizeChanged(&((OleOutlineDocPtr)pOutlineDoc)->m_OleDoc, true);
#endif
}

//#pragma segment OutlineDocumentSeg
void OutlineDocIndentLine(OutlineDocPtr pOutlineDoc, Boolean indent)
{
	LineListIndentLines(pOutlineDoc->m_LineList, indent);

	// set document dirty
	ASSERTCOND(((DocumentPtr)pOutlineDoc)->vtbl->m_SetDirtyProcPtr != nil);
	(*((DocumentPtr)pOutlineDoc)->vtbl->m_SetDirtyProcPtr)((DocumentPtr)pOutlineDoc, true);

#if qOle
	OleDocSetSizeChanged(&((OleOutlineDocPtr)pOutlineDoc)->m_OleDoc, true);
#endif
}

//#pragma segment OutlineDocumentSeg
void OutlineDocUpdateMenus(OutlineDocPtr pOutlineDoc)
{
	EnableCmd(cmdNewLine);

	if (!LineListIsEmpty(pOutlineDoc->m_LineList))
	{
		EnableCmd(cmdSelectAll);
		EnableCmd(cmdShowName);
		
		if (LineListIsSelection(pOutlineDoc->m_LineList))
		{
			EnableCmd(cmdIndentLine);
			EnableCmd(cmdUnIndentLine);
		}
	}		

	InheritUpdateMenus(pOutlineDoc->m_GopherUpdateMenus);
}

//#pragma segment OutlineDocumentSeg
void OutlineDocResize(OutlineDocPtr pOutlineDoc, Rect* rOldBounds, short wWidth, short wHeight)
{
	GrafPtr		oldPort;
	Rect		r;

	GetPort(&oldPort);
	SetPort(((DocumentPtr)pOutlineDoc)->m_WindowPtr);

	// inval old grow box
	r = *rOldBounds;
	r.left = r.right - 15;
	r.top = r.bottom - 15;
	InvalRect(&r);

	LineListSize(pOutlineDoc->m_LineList, wWidth, wHeight);

	// inval new grow box
	OutlineDocGetLineListBounds(pOutlineDoc, &r);
	r.left = r.right - 15;
	r.top = r.bottom - 15;
	InvalRect(&r);

	SetPort(oldPort);
}

//#pragma segment OutlineDocumentSeg
void OutlineDocGetLineListBounds(OutlineDocPtr pOutlineDoc, Rect* rBounds)
{
	*rBounds = ((DocumentPtr)pOutlineDoc)->m_WindowPtr->portRect;
	
	if (pOutlineDoc->m_fShowRowColumnHeadings)
	{
		rBounds->top += kColumnHeadingsHeight;
		rBounds->left += kRowHeadingsWidth;
	}
}

//#pragma segment OutlineDocumentSeg
void OutlineDocSetRowColumnHeadings(OutlineDocPtr pOutlineDoc, Boolean fRowColumnHeadings)
{
	pOutlineDoc->m_fShowRowColumnHeadings = fRowColumnHeadings;
}

//#pragma segment OutlineDocumentSeg
void OutlineDocDoUpdate(DocumentPtr pDoc)
{
	OutlineDocPtr	pOutlineDoc;

	pOutlineDoc = (OutlineDocPtr)pDoc;

	OutlineDocDrawGrowIcon(pOutlineDoc);

	LineListUpdate(pOutlineDoc->m_LineList, pDoc->m_WindowPtr->visRgn);
	
	if (pOutlineDoc->m_fShowRowColumnHeadings)
		OutlineDocUpdateRowColumnHeadings(pOutlineDoc);
}

//#pragma segment OutlineDocumentSeg
void OutlineDocDrawGrowIcon(OutlineDocPtr pOutlineDoc)
{
	Rect		r;
	RgnHandle	oldClipRgn;
	RgnHandle	newClipRgn;
	GrafPtr		oldPort;
	WindowPtr	whichWindow;
	
	whichWindow = ((DocumentPtr)pOutlineDoc)->m_WindowPtr;
	
	GetPort(&oldPort);
	SetPort(whichWindow);
	
	r = whichWindow->portRect;
	
	oldClipRgn = NewRgn();
	ASSERTCOND(oldClipRgn != nil);
	
	newClipRgn = NewRgn();
	ASSERTCOND(newClipRgn != nil);
	
	r.left = r.right - 15;
	r.top = r.bottom - 15;
	RectRgn(newClipRgn, &r);
	
	GetClip(oldClipRgn);
	SetClip(newClipRgn);
	
	DrawGrowIcon(whichWindow);
	
	SetClip(oldClipRgn);
	
	DisposeRgn(oldClipRgn);
	DisposeRgn(newClipRgn);
	
	SetPort(oldPort);
}

//#pragma segment OutlineDocumentSeg
void OutlineDocGetColumnHeadingRect(OutlineDocPtr pOutlineDoc, Rect* r)
{
	*r = ((DocumentPtr)pOutlineDoc)->m_WindowPtr->portRect;
	r->top -= 1;
	r->bottom = r->top + kColumnHeadingsHeight + 1;
	r->left += kRowHeadingsWidth - 1;
	r->right += 1;
}

//#pragma segment OutlineDocumentSeg
void OutlineDocGetRowHeadingRect(OutlineDocPtr pOutlineDoc, Rect* r)
{
	*r = ((DocumentPtr)pOutlineDoc)->m_WindowPtr->portRect;
	r->left -= 1;
	r->top += kColumnHeadingsHeight - 1;
	r->right = r->left + kRowHeadingsWidth + 1;
	r->bottom += 1;
}

//#pragma segment OutlineDocumentSeg
void OutlineDocUpdateRowColumnHeadings(OutlineDocPtr pOutlineDoc)
{
	WindowPtr	whichWindow;
	Boolean		fDrawColor;
	GrafPtr		oldPort;
	Rect		rClip;
	Rect		r;
	Rect		rSect;
	short		oldFontNum;
	short		oldFontSize;
	RgnHandle	oldClipRgn;
	RgnHandle	newClipRgn;
	
	whichWindow = ((DocumentPtr)pOutlineDoc)->m_WindowPtr;

	// If the drawing mode has changed we need to force
	// a complete redraw so there's no point in continuing.
	fDrawColor = (GetWindowMinDepth(whichWindow) >= 4);
	if (pOutlineDoc->m_fDrawColor != fDrawColor)
	{
		pOutlineDoc->m_fDrawColor = fDrawColor;
		InvalRect(&whichWindow->portRect);
		return;
	}
	
	oldFontNum = whichWindow->txFont;
	TextFont(systemFont);
	
	oldFontSize = whichWindow->txSize;
	TextSize(0);
	
	oldClipRgn = NewRgn();
	ASSERTCOND(oldClipRgn != nil);
	
	newClipRgn = NewRgn();
	ASSERTCOND(newClipRgn != nil);
	
	GetPort(&oldPort);
	SetPort(whichWindow);
	
	GetClip(oldClipRgn);

	r.top = r.left = 0;
	r.right = kRowHeadingsWidth;
	r.bottom = kColumnHeadingsHeight;

	if (fDrawColor)
	{
		RGBForeColor(&gLtGray);
		PaintRect(&r);
	
		RGBForeColor(&gWhite);
	
		MoveTo((short)r.left, (short)(r.bottom-2));
		LineTo(r.left, r.top);
		LineTo((short)(r.right-2), r.top);
	
		MoveTo((short)(r.right-3),(short)(r.top+1));
		LineTo((short)(r.left+1),(short)(r.top+1));
		LineTo((short)(r.left+1),(short)(r.bottom-3));
	
		RGBForeColor(&gDkGray);
	
		MoveTo((short)r.left, (short)(r.bottom-2));
		LineTo((short)(r.right-2), (short)(r.bottom-2));
		LineTo((short)(r.right-2), r.top);
	
		MoveTo((short)(r.right-3), (short)(r.top+1));
		LineTo((short)(r.right-3), (short)(r.bottom-3));
		LineTo((short)(r.left+1), (short)(r.bottom-3));
	
		RGBForeColor(&gBlack);
	}
	else
		EraseRect(&r);

	{
		char	c;
		
		OutlineDocGetColumnHeadingRect(pOutlineDoc, &rClip);

		if (SectRect(&rClip, &(**oldClipRgn).rgnBBox, &rSect))
		{
			FrameRect(&rClip);
			r = rClip;
			InsetRect(&rClip, 1, 1);
	
			if (fDrawColor)
			{
				RGBForeColor(&gDkGray);
				PaintRect(&rClip);
				RGBForeColor(&gBlack);
			}
			else
				EraseRect(&rClip);
			
			RectRgn(newClipRgn, &rClip);
			SectRgn(newClipRgn, oldClipRgn, newClipRgn);
			SetClip(newClipRgn);
			
			r.right = r.left + kTabWidth + 1;
			OffsetRect(&r, (short)-LineListGetHorizOffset(pOutlineDoc->m_LineList), 0);
			
			for (c = 'A'; r.left < rClip.right; c++)
			{
				if (SectRect(&r, &(**newClipRgn).rgnBBox, &rSect))
				{
					Rect	rText;

					if (fDrawColor)
					{
						RGBForeColor(&gLtGray);
						PaintRect(&r);
			
						RGBForeColor(&gWhite);
						MoveTo((short)(r.left+1), (short)(r.bottom-1));
						LineTo((short)(r.left+1), (short)(r.top+1));
						LineTo((short)(r.right-1), (short)(r.top+1));
			
						RGBForeColor(&gBlack);
						FrameRect(&r);
					}
					else
					{
						EraseRect(&r);
						FrameRect(&r);
					}
		
					rText = r;
					rText.top++;

					DrawTextCentered(&c, 1, &rText);
				}
				
				OffsetRect(&r, kTabWidth, 0);
			}
			
			SetClip(oldClipRgn);
		}
	}
	
	{
		short		index;
		Str32		str;

		OutlineDocGetRowHeadingRect(pOutlineDoc, &rClip);

		if (SectRect(&rClip, &(**oldClipRgn).rgnBBox, &rSect))
		{
			FrameRect(&rClip);
			r = rClip;
			InsetRect(&rClip, 1, 1);
			
			if (fDrawColor)
			{
				RGBForeColor(&gDkGray);
				PaintRect(&rClip);
				RGBForeColor(&gBlack);
			}
			else
				EraseRect(&rClip);
	
			RectRgn(newClipRgn, &rClip);
			SectRgn(newClipRgn, oldClipRgn, newClipRgn);
			SetClip(newClipRgn);
			
			OffsetRect(&r, 0, (short)-LineListGetVertOffset(pOutlineDoc->m_LineList));
			
			for (index = 0; r.top < rClip.bottom; index++)
			{
				LinePtr		pLine;
				Rect		rText;
				
				pLine = LineListGetLine(pOutlineDoc->m_LineList, index);
		
				if (pLine == nil)
					break;
				
				r.bottom = r.top + pLine->m_HeightInPoints + 1;
				
				if (SectRect(&r, &(**newClipRgn).rgnBBox, &rSect))
				{
					if (fDrawColor)
					{
						RGBForeColor(&gLtGray);
						PaintRect(&r);
			
						RGBForeColor(&gWhite);
						MoveTo((short)(r.left+1), (short)(r.bottom-1));
						LineTo((short)(r.left+1), (short)(r.top+1));
						LineTo((short)(r.right-1), (short)(r.top+1));
			
						RGBForeColor(&gBlack);
						FrameRect(&r);
					}
					else
					{
						EraseRect(&r);
						FrameRect(&r);
					}
					
					rText = r;
					rText.top++;
		
					NumToString(index+1, str);
					DrawTextCentered((Ptr)&str[1], str[0], &rText);
				}
	
				OffsetRect(&r, 0, pLine->m_HeightInPoints);
			}
	
			SetClip(oldClipRgn);
		}
	}
	
	SetPort(oldPort);
	
	DisposeRgn(oldClipRgn);
	
	TextFont(oldFontNum);
	TextSize(oldFontSize);
}

//#pragma segment OutlineDocumentSeg
void OutlineDocScrollRect(DocumentPtr pDoc, short distHoriz, short distVert)
{
	OutlineDocPtr	pOutlineDoc;
	WindowPtr		whichWindow;
	GrafPtr			oldPort;
	RgnHandle		clipRgn;
	RgnHandle		tempRgn;
	Rect			r;
			
	whichWindow = pDoc->m_WindowPtr;
	
	clipRgn = NewRgn();
	ASSERTCOND(clipRgn != nil);
	
	tempRgn = NewRgn();
	ASSERTCOND(tempRgn != nil);
	
	GetPort(&oldPort);
	SetPort(whichWindow);

	pOutlineDoc = (OutlineDocPtr)pDoc;

	if (distHoriz != 0)
	{
		OutlineDocGetColumnHeadingRect(pOutlineDoc, &r);
		InsetRect(&r, 1, 1);
		
		ScrollRect(&r, distHoriz, 0, clipRgn);
	}
	
	if (distVert != 0)
	{
		OutlineDocGetRowHeadingRect(pOutlineDoc, &r);
		InsetRect(&r, 1, 1);

		ScrollRect(&r, 0, distVert, tempRgn);
		UnionRgn(clipRgn, tempRgn, clipRgn);
	}
	
	
	if (!EmptyRgn(clipRgn))
	{
		GetClip(tempRgn);
		SetClip(clipRgn);

		OutlineDocUpdateRowColumnHeadings(pOutlineDoc);

		SetClip(tempRgn);
	}
	
	SetPort(oldPort);
	
	DisposeRgn(clipRgn);
	DisposeRgn(tempRgn);

	// Make sure the updateRgn is still accurate
	if (!EmptyRgn(((WindowPeek)whichWindow)->updateRgn))
		OffsetRgn(((WindowPeek)whichWindow)->updateRgn, distHoriz, distVert);
}

//#pragma segment OutlineDocumentSeg
void OutlineDocDoOpen(DocumentPtr pDoc, FSSpecPtr theFile, Boolean readOnly)
{
	OutlineDocPtr	pOutlineDoc;
	
	pOutlineDoc = (OutlineDocPtr)pDoc;

	ASSERTCOND(pOutlineDoc->vtbl->m_DoOpenProcPtr != nil);
	(*pOutlineDoc->vtbl->m_DoOpenProcPtr)(pDoc, theFile, readOnly);

	pOutlineDoc->m_LineList->m_fForceUpdate = true;
	LineListUpdateView(pOutlineDoc->m_LineList);

	ASSERTCOND(pDoc->vtbl->m_SetDirtyProcPtr != nil);
	(*pDoc->vtbl->m_SetDirtyProcPtr)(pDoc, false);
}

//#pragma segment OutlineDocumentSeg
void OutlineDocDoContent(DocumentPtr pDoc, EventRecord* theEvent)
{
	OutlineDocPtr	pOutlineDoc;
	Point			pt;

	pOutlineDoc = (OutlineDocPtr)pDoc;

	pt = theEvent->where;
	GlobalToLocal(&pt);

	if (LineListClick(pOutlineDoc->m_LineList, pt, theEvent->modifiers))
		DoCmd(cmdDoubleClick);
}

//#pragma segment OutlineDocumentSeg
long OutlineDocDoGrow(DocumentPtr pDoc, EventRecord* theEvent)
{
	OutlineDocPtr		pOutlineDoc;
	long				newSize;
	Rect				r;

	pOutlineDoc = (OutlineDocPtr)pDoc;

	OutlineDocGetLineListBounds(pOutlineDoc, &r);

	ASSERTCOND(pOutlineDoc->vtbl->m_DoGrowProcPtr != nil);
	newSize = (*pOutlineDoc->vtbl->m_DoGrowProcPtr)(pDoc, theEvent);

	if (newSize != 0)
		OutlineDocResize(pOutlineDoc, &r, (short)(LoWord(newSize) - kRowHeadingsWidth), (short)(HiWord(newSize) - kColumnHeadingsHeight));

	return newSize;
}

//#pragma segment OutlineDocumentSeg
void OutlineDocDoZoom(DocumentPtr pDoc, short partCode)
{
	OutlineDocPtr	pOutlineDoc;
	Rect			r,
					rPort;
	short			width;
	short			height;

	pOutlineDoc = (OutlineDocPtr)pDoc;

	OutlineDocGetLineListBounds(pOutlineDoc, &r);

	ASSERTCOND(pOutlineDoc->vtbl->m_DoZoomProcPtr != nil);
	(*pOutlineDoc->vtbl->m_DoZoomProcPtr)(pDoc, partCode);

	rPort = pDoc->m_WindowPtr->portRect;
	OutlineDocResize(pOutlineDoc, &r, (short)(rPort.right - rPort.left - kRowHeadingsWidth), (short)(rPort.bottom - rPort.top - kColumnHeadingsHeight));
}

/* OutlineDocCanCopy
 * -----------------
 *
 *		Enable or Disable Edit/Copy menuitem depending on the state of OutlineDoc
 */
//#pragma segment OutlineDocumentSeg
Boolean OutlineDocCanCopy(DocumentPtr pDoc)
{
	return LineListIsSelection(((OutlineDocPtr)pDoc)->m_LineList);
}

//#pragma segment OutlineDocumentSeg
Boolean OutlineDocCanCut(DocumentPtr pDoc)
{
	return LineListIsSelection(((OutlineDocPtr)pDoc)->m_LineList);
}

//#pragma segment OutlineDocumentSeg
Boolean OutlineDocCanPaste(DocumentPtr pDoc)
{
	return true;		// this needs to be fixed
}

//#pragma segment OutlineDocumentSeg
Boolean OutlineDocCanClear(DocumentPtr pDoc)
{
	return LineListIsSelection(((OutlineDocPtr)pDoc)->m_LineList);
}

//#pragma segment OutlineDocumentSeg
/* OutlineDocDoCopy
 * ----------------
 *  Copy selection to clipboard.
 *  Post to the clipboard the formats that the app can render.
 *  the actual data is not rendered at this time. using the
 *  delayed rendering technique. The app will receive AppleEvent 'RNCF'
 *  when the actual data is requested.
 *
 */
void OutlineDocDoCopy(DocumentPtr pDoc)
{
}

/* OutlineDocDoPaste
 * -----------------
 *
 * Paste lines from clipboard
 */
void OutlineDocDoPaste(DocumentPtr pDoc)
{
}

/* OutlineDocPasteTextData
 * -----------------------
 *
 *      Build Line Objects from the strings (separated by '\n') in hText
 * and put them into the document
 */
int OutlineDocPasteTextData(OutlineDocPtr pOutlineDoc, Handle hText)
{
    LineListPtr 	pLIneList = pOutlineDoc->m_LineList;
    char*       	pszText;
    char*       	pszEnd;
    TextLinePtr 	pTextLine;
    LinePtr			pLine;
    int         	nLineCount;
    int         	i;
    unsigned int    nTab;
	Str255			s;

	HLock(hText);
    pszText = *hText;
    if(!pszText)
        return 0;

    pszEnd = pszText + GetHandleSize(hText);
    nLineCount=0;

    while(*pszText && (pszText<pszEnd)) {

        // count the tab level
        nTab = 0;
        while((*pszText == '\t') && (pszText<pszEnd)) {
            nTab++;
            pszText++;
        }

        // collect the text string character by character
        for(i=1; (i<=255) && (pszText<pszEnd); i++) {
#ifndef THINK_C
            if ((! *pszText) || (*pszText == '\n'))
#else
            if ((! *pszText) || (*pszText == '\r'))
#endif
                break;
            s[i] = *pszText++;
        }
        pszText++;
		s[0] = i-1;

		pTextLine = (TextLinePtr)NewPtrClear(sizeof(TextLineRec));
		ASSERTCOND(pTextLine != nil);
		FailNIL(pTextLine);
	
		pLine = (LinePtr)pTextLine;
		
		TextLineInit(pTextLine, pOutlineDoc->m_LineList);
		TextLineSetText(pTextLine, s);
		
		LineSetTabLevel(pLine, (short)nTab);

        if (LineListAddLine(pOutlineDoc->m_LineList, pLine)) {
			nLineCount++;
		}
		else {
			ASSERTCOND(pLine->vtbl->m_FreeProcPtr != nil);
			(*pLine->vtbl->m_FreeProcPtr)(pLine);
		}
    }

    HUnlock(hText);

    return nLineCount;
}

//#pragma segment OutlineDocumentSeg
void OutlineDocDoClear(DocumentPtr pDoc)
{
	GrafPtr	oldPort;
	Rect	r;
	
	LineListDoClear(((OutlineDocPtr)pDoc)->m_LineList);

	OutlineDocGetRowHeadingRect((OutlineDocPtr)pDoc, &r);

	GetPort(&oldPort);
	SetPort(pDoc->m_WindowPtr);
	
	InsetRect(&r, 1, 1);
	EraseRect(&r);
	InvalRect(&r);
	
	SetPort(oldPort);

	// set document dirty
	ASSERTCOND(pDoc->vtbl->m_SetDirtyProcPtr != nil);
	(*pDoc->vtbl->m_SetDirtyProcPtr)(pDoc, true);

#if qOle
	OleDocSetSizeChanged(&((OleOutlineDocPtr)pDoc)->m_OleDoc, true);
#endif
}

//#pragma segment OutlineDocumentSeg
void OutlineDocShow(DocumentPtr pDoc)
{
#if !qOle
	if (!pDoc->m_fPositioned)
	{
		ASSERTCOND(gApplication->vtbl->m_PositionNewDocProcPtr != nil);
		(*gApplication->vtbl->m_PositionNewDocProcPtr)(gApplication, pDoc);

		pDoc->m_fPositioned = true;

#if qFrameTools
		ASSERTCOND(pDoc->vtbl->m_EnableFrameToolsProcPtr != nil);
		(*pDoc->vtbl->m_EnableFrameToolsProcPtr)(pDoc, true, true);
#endif
	}
#endif

	ASSERTCOND(pDoc->baseVtbl->m_ShowProcPtr != nil);
	(*pDoc->baseVtbl->m_ShowProcPtr)((WinPtr)pDoc);
}

//#pragma segment OutlineDocumentSeg
LineListPtr OutlineDocGetLineList(OutlineDocPtr pOutlineDoc)
{
	return pOutlineDoc->m_LineList;
}

//#pragma segment OutlineDocumentSeg
NameTablePtr OutlineDocGetNameTable(OutlineDocPtr pOutlineDoc)
{
	return pOutlineDoc->m_NameTable;
}

//#pragma segment OutlineDocumentSeg
void OutlineDocDoIdle(DocumentPtr pDoc)
{
	OutlineDocPtr	pOutlineDoc = (OutlineDocPtr)pDoc;
	
	LineListUpdateView(pOutlineDoc->m_LineList);
	
	/* do mouse tracking and cursor feedback for the active document */
	if (pDoc->m_Active)
	{
		Point		mPt;
		Rect		rBounds;

		GetMouse(&mPt);

		// check if cursor is on LineList
		OutlineDocGetLineListBounds(pOutlineDoc, &rBounds);
		if (PtInRect(mPt, &rBounds) && PtInRgn(mPt, pDoc->m_WindowPtr->visRgn))
			LineListGiveCursorFeedback(pOutlineDoc->m_LineList, mPt);
		else
		{
			ASSERTCOND(pDoc->vtbl->m_SetDefaultCursorProcPtr != nil);
			(*pDoc->vtbl->m_SetDefaultCursorProcPtr)(pDoc);
			return;
		}
	}
}

#if qOle

// OLDNAME: OleDocumentInterface.c
static IPersistFileVtbl			gOleDocPersistFileVtbl;
static IOleItemContainerVtbl	gOleItemContainerVtbl;
static IDataObjectVtbl			gDataObjectVtbl;

#if qOleDragDrop
static IDropTargetVtbl			gDropTargetVtbl;
#endif

//#pragma segment OleDocumentInterfaceInitSeg
void OleDocInitInterfaces(void)
{
	// OleDoc::IPersistFile method table
	{
		IPersistFileVtbl*	p;

		p = &gOleDocPersistFileVtbl;

		p->QueryInterface		= OleDocIntPersistFileQueryInterface;
		p->AddRef				= OleDocIntPersistFileAddRef;
		p->Release				= OleDocIntPersistFileRelease;
		p->GetClassID			= OleDocIntPersistFileGetClassID;
		p->IsDirty				= OleDocIntPersistFileIsDirty;
		p->Load					= OleDocIntPersistFileLoad;
		p->Save					= OleDocIntPersistFileSave;
		p->SaveCompleted		= OleDocIntPersistFileSaveCompleted;
		p->GetCurFile			= OleDocIntPersistFileGetCurFile;
		p->LoadFSP				= OleDocIntPersistFileLoadFsp;
		p->SaveFSP				= OleDocIntPersistFileSaveFsp;
		p->SaveCompletedFSP		= OleDocIntPersistFileSaveCompletedFSP;
		p->GetCurFSP			= OleDocIntPersistFileGetCurFsp;

		ASSERTCOND(ValidVtbl(((char *)p) + sizeof(p->b), sizeof(*p) - sizeof(p->b)));
	}

	// OleDoc::IOleItemContainer method table
	{
		IOleItemContainerVtbl*	p;

		p = &gOleItemContainerVtbl;

		p->QueryInterface		= OleDocIntItemContainerQueryInterface;
		p->AddRef				= OleDocIntItemContainerAddRef;
		p->Release				= OleDocIntItemContainerRelease;
		p->ParseDisplayName		= OleDocIntItemContainerParseDisplayName;
		p->EnumObjects			= OleDocIntItemContainerEnumObjects;
		p->LockContainer		= OleDocIntItemContainerLockContainer;
		p->GetObject			= OleDocIntItemContainerGetObject;
		p->GetObjectStorage		= OleDocIntItemContainerGetObjectStorage;
		p->IsRunning			= OleDocIntItemContainerIsRunning;

		ASSERTCOND(ValidVtbl(((char *)p) + sizeof(p->b), sizeof(*p) - sizeof(p->b)));
	}

	// OleDoc::IDataObject method table
	{
		IDataObjectVtbl*	p;

		p = &gDataObjectVtbl;

		p->QueryInterface			= OleDocIntDataObjectQueryInterface;
		p->AddRef					= OleDocIntDataObjectAddRef;
		p->Release					= OleDocIntDataObjectRelease;
		p->GetData					= OleDocIntDataObjectGetData;
		p->GetDataHere				= OleDocIntDataObjectGetDataHere;
		p->QueryGetData				= OleDocIntDataObjectQueryGetData;
		p->GetCanonicalFormatEtc	= OleDocIntDataObjectGetCanonicalFormatEtc;
		p->SetData					= OleDocIntDataObjectSetData;
		p->EnumFormatEtc			= OleDocIntDataObjectEnumFormatEtc;
		p->DAdvise					= OleDocIntDataObjectAdvise;
		p->DUnadvise				= OleDocIntDataObjectUnadvise;
		p->EnumDAdvise				= OleDocIntDataObjectEnumAdvise;

		ASSERTCOND(ValidVtbl(((char *)p) + sizeof(p->b), sizeof(*p) - sizeof(p->b)));
	}

#if qOleDragDrop	
	// OleDoc::IDropTarget method table
	{
		IDropTargetVtbl*	p;
		
		p = &gDropTargetVtbl;
		
		p->QueryInterface			= OleDocIntDropTargetQueryInterface;
		p->AddRef					= OleDocIntDropTargetAddRef;
		p->Release					= OleDocIntDropTargetRelease;
		p->DragEnter				= OleDocIntDropTargetDragEnter;
		p->DragOver					= OleDocIntDropTargetDragOver;
		p->DragLeave				= OleDocIntDropTargetDragLeave;
		p->Drop						= OleDocIntDropTargetDrop;

		ASSERTCOND(ValidVtbl(((char *)p) + sizeof(p->b), sizeof(*p) - sizeof(p->b)));
	}
#endif // qOleDragDrop

}

//#pragma segment OleDocumentInterfaceSeg
void DocIPersistFileInit(DocPersistFileImplPtr pDocPersistFileImpl, struct OleDocumentRec* pOleDoc)
{
	pDocPersistFileImpl->lpVtbl			= &gOleDocPersistFileVtbl;
	pDocPersistFileImpl->lpOleDoc		= pOleDoc;
	pDocPersistFileImpl->cRef			= 0;
}

//#pragma segment OleDocumentInterfaceSeg
STDMETHODIMP OleDocIntPersistFileQueryInterface(LPPERSISTFILE lpThis, REFIID riid, void* * lplpvObj)
{
	OleDocumentPtr		pOleDoc;

	OleDbgEnterInterface();

	pOleDoc = ((DocPersistFileImplPtr)lpThis)->lpOleDoc;

	return OleDocQueryInterface(pOleDoc, riid, lplpvObj);
}

//#pragma segment OleDocumentInterfaceSeg
STDMETHODIMP_(unsigned long) OleDocIntPersistFileAddRef(LPPERSISTFILE lpThis)
{
	OleDocumentPtr		pOleDoc;

	OleDbgEnterInterface();

	pOleDoc = ((DocPersistFileImplPtr)lpThis)->lpOleDoc;

	return OleDocAddRef(pOleDoc);
}

//#pragma segment OleDocumentInterfaceSeg
STDMETHODIMP_(unsigned long) OleDocIntPersistFileRelease(LPPERSISTFILE lpThis)
{
	OleDocumentPtr		pOleDoc;

	OleDbgEnterInterface();

	pOleDoc = ((DocPersistFileImplPtr)lpThis)->lpOleDoc;

	return OleDocRelease(pOleDoc);
}

//#pragma segment OleDocumentInterfaceSeg
STDMETHODIMP OleDocIntPersistFileGetClassID(LPPERSISTFILE lpThis, CLSID * lpclsid)
{
	OleDocumentPtr		pOleDoc;

	OleDbgEnterInterface();

	pOleDoc = ((DocPersistFileImplPtr)lpThis)->lpOleDoc;

	ASSERTCOND(pOleDoc->m_pIDoc->lpVtbl->m_GetClassIDProcPtr != nil);
	return (*pOleDoc->m_pIDoc->lpVtbl->m_GetClassIDProcPtr)(pOleDoc->m_pIDoc, lpclsid, nil);
}

//#pragma segment OleDocumentInterfaceSeg
STDMETHODIMP OleDocIntPersistFileIsDirty(LPPERSISTFILE lpThis)
{
	HRESULT			hrErr;
	OleDocumentPtr	pOleDoc;
	
	OleDbgEnterInterface();

	pOleDoc = ((DocPersistFileImplPtr)lpThis)->lpOleDoc;

	ASSERTCOND(pOleDoc->m_pIDoc->lpVtbl->m_IsDirtyProcPtr != nil);
	if ((*pOleDoc->m_pIDoc->lpVtbl->m_IsDirtyProcPtr)(pOleDoc->m_pIDoc))
		hrErr = NOERROR;
	else
		hrErr = ResultFromScode(S_FALSE);

	return hrErr;
}

//#pragma segment OleDocumentInterfaceSeg
STDMETHODIMP OleDocIntPersistFileLoad(LPPERSISTFILE lpThis, const char* lpszFileName, unsigned long grfMode)
{
    FSSpec fs;

	OleDbgEnterInterface();

	FSMakeFSSpec(0, 0, (StringPtr)c2pstr((char*)lpszFileName), &fs);
	p2cstr((StringPtr)lpszFileName);		// and convert it back
	
	return OleDocIntPersistFileLoadFsp(lpThis, &fs, grfMode);
}

//#pragma segment OleDocumentInterfaceSeg
STDMETHODIMP OleDocIntPersistFileSave(LPPERSISTFILE lpThis, const char* lpszFileName, unsigned long fRemember)
{
	OleDbgEnterInterface();

	return ResultFromScode(E_FAIL);
}

//#pragma segment OleDocumentInterfaceSeg
STDMETHODIMP OleDocIntPersistFileSaveCompleted(LPPERSISTFILE lpThis, const char* lpszFileName)
{
	OleDbgEnterInterface();

	return ResultFromScode(E_FAIL);
}

//#pragma segment OleDocumentInterfaceSeg
STDMETHODIMP OleDocIntPersistFileGetCurFile(LPPERSISTFILE lpThis, char* * lplpszFileName)
{
	OleDbgEnterInterface();

	return ResultFromScode(E_FAIL);
}

//#pragma segment OleDocumentInterfaceSeg
STDMETHODIMP OleDocIntPersistFileLoadFsp(LPPERSISTFILE lpThis, const FSSpec *pSpec, unsigned long grfMode)
{
	OleDocumentPtr pOleDoc;

	OleDbgEnterInterface();

	pOleDoc = ((DocPersistFileImplPtr)lpThis)->lpOleDoc;

	return OleDocLoadFsp(pOleDoc, (FSSpecPtr)pSpec, grfMode);
}

//#pragma segment OleDocumentInterfaceSeg
STDMETHODIMP OleDocIntPersistFileSaveFsp(LPPERSISTFILE lpThis, const FSSpec *pSpec, unsigned long fRemember)
{
	OleDbgEnterInterface();

	return ResultFromScode(E_FAIL);
}

//#pragma segment OleDocumentInterfaceSeg
STDMETHODIMP OleDocIntPersistFileSaveCompletedFSP(LPPERSISTFILE lpThis, const FSSpec *pSpec)
{
	OleDbgEnterInterface();

	return ResultFromScode(E_FAIL);
}

//#pragma segment OleDocumentInterfaceSeg
STDMETHODIMP OleDocIntPersistFileGetCurFsp(LPPERSISTFILE lpThis, FSSpec *pSpec)
{
	OleDbgEnterInterface();

	return ResultFromScode(E_FAIL);
}

//#pragma segment OleDocumentInterfaceSeg
void DocIOleItemContainerInit(DocOleItemContainerImplPtr pDocOleItemContainerImpl, struct OleDocumentRec* pOleDoc)
{
	pDocOleItemContainerImpl->lpVtbl		= &gOleItemContainerVtbl;
	pDocOleItemContainerImpl->lpOleDoc		= pOleDoc;
	pDocOleItemContainerImpl->cRef			= 0;
}

//#pragma segment OleDocumentInterfaceSeg
STDMETHODIMP OleDocIntItemContainerQueryInterface(LPOLEITEMCONTAINER lpThis, REFIID riid, void* * lplpvObj)
{
	OleDocumentPtr		pOleDoc;

	OleDbgEnterInterface();

	pOleDoc = ((DocOleItemContainerImplPtr)lpThis)->lpOleDoc;

	return OleDocQueryInterface(pOleDoc, riid, lplpvObj);
}

//#pragma segment OleDocumentInterfaceSeg
STDMETHODIMP_(unsigned long) OleDocIntItemContainerAddRef(LPOLEITEMCONTAINER lpThis)
{
	OleDocumentPtr		pOleDoc;

	OleDbgEnterInterface();

	pOleDoc = ((DocOleItemContainerImplPtr)lpThis)->lpOleDoc;

	return OleDocAddRef(pOleDoc);
}

//#pragma segment OleDocumentInterfaceSeg
STDMETHODIMP_(unsigned long) OleDocIntItemContainerRelease(LPOLEITEMCONTAINER lpThis)
{
	OleDocumentPtr		pOleDoc;

	OleDbgEnterInterface();

	pOleDoc = ((DocOleItemContainerImplPtr)lpThis)->lpOleDoc;

	return OleDocRelease(pOleDoc);
}

//#pragma segment OleDocumentInterfaceSeg
STDMETHODIMP OleDocIntItemContainerParseDisplayName(LPOLEITEMCONTAINER lpThis, LPBC lpbc, char* lpszDisplayName, unsigned long * lpchEaten, LPMONIKER * lplpmkOut)
{
	OleDocumentPtr		pOleDoc;
    char 				szItemName[kMaxNameLen];
    LPUNKNOWN			lpUnk;
	HRESULT				hrErr;

	OleDbgEnterInterface();

	pOleDoc = ((DocOleItemContainerImplPtr)lpThis)->lpOleDoc;

    /* OLE2NOTE: we must make sure to set all out ptr parameters to NULL. */
    *lplpmkOut = NULL;

    *lpchEaten = OleStdGetItemToken(
            lpszDisplayName,
            szItemName,
            sizeof(szItemName)
    );

    /* OLE2NOTE: get a pointer to a running instance of the object. we
    **    should force the object to go running if necessary (even if
    **    this means launching its server EXE). this is the meaining of
    **    BINDSPEED_INDEFINITE. Parsing a Moniker is known to be an
    **    "EXPENSIVE" operation.
    */
    hrErr = OleDocIntItemContainerGetObject(
            lpThis,
            szItemName,
            BINDSPEED_INDEFINITE,
            lpbc,
            &IID_IUnknown,
            (void**)&lpUnk
    );

    if (hrErr == NOERROR) {
        OleStdRelease(lpUnk);   // item name FOUND; don't need obj ptr.
        CreateItemMoniker(OLESTDDELIM, szItemName, lplpmkOut);
    } else
        *lpchEaten = 0;     // item name is NOT valid

    return hrErr;
}

//#pragma segment OleDocumentInterfaceSeg
STDMETHODIMP OleDocIntItemContainerEnumObjects(LPOLEITEMCONTAINER lpThis, unsigned long grfFlags, LPENUMUNKNOWN * lplpenumUnknown)
{
	OleDbgEnterInterface();

    /* OLE2NOTE: we must make sure to set all out ptr parameters to NULL. */
    *lplpenumUnknown = NULL;

    /* OLE2NOTE: this method should be implemented to allow programatic
    **    clients the ability to what elements the container holds.
    **    this method is NOT called in the standard linking scenarios.
    **
    **    grfFlags can be one of the following:
    **        OLECONTF_EMBEDDINGS   -- enumerate embedded objects
    **        OLECONTF_LINKS        -- enumerate linked objects
    **        OLECONTF_OTHERS       -- enumerate non-OLE compound doc objs
    **        OLECONTF_ONLYUSER     -- enumerate only objs named by user
    **        OLECONTF_ONLYIFRUNNING-- enumerate only objs in running state
    */

    return ResultFromScode(E_NOTIMPL);
}

//#pragma segment OleDocumentInterfaceSeg
STDMETHODIMP OleDocIntItemContainerLockContainer(LPOLEITEMCONTAINER lpThis, unsigned long fLock)
{
	OleDocumentPtr		pOleDoc;
	HRESULT				hrErr;

	OleDbgEnterInterface();

	pOleDoc = ((DocOleItemContainerImplPtr)lpThis)->lpOleDoc;

    /* OLE2NOTE: in order to hold the document alive we call
    **    CoLockObjectExternal to add a strong reference to our Doc
    **    object. this will keep the Doc alive when all other external
    **    references release us. whenever an embedded object goes
    **    running a LockContainer(TRUE) is called. when the embedded
    **    object shuts down (ie. transitions from running to loaded)
    **    LockContainer(FALSE) is called. if the user issues File.Close
    **    the document will shut down in any case ignoring any
    **    outstanding LockContainer locks because CoDisconnectObject is
    **    called in OleDoc_Close. this will forceably break any
    **    existing strong reference counts including counts that we add
    **    ourselves by calling CoLockObjectExternal and guarantee that
    **    the Doc object gets its final release (ie. cRefs goes to 0).
    */
    hrErr = OleDocLock(pOleDoc, (Boolean)fLock, true /* fLastUnlockReleases */);

    return hrErr;
}

//#pragma segment OleDocumentInterfaceSeg
STDMETHODIMP OleDocIntItemContainerGetObject(LPOLEITEMCONTAINER lpThis, char* pszItem, unsigned long SpeedNeeded, LPBINDCTX pbc, REFIID riid, void* * ppvObject)
{
	OleDocumentPtr		pOleDoc;

	OleDbgEnterInterface();

	pOleDoc = ((DocOleItemContainerImplPtr)lpThis)->lpOleDoc;

    /* OLE2NOTE: we must make sure to set all out ptr parameters to NULL. */
    *ppvObject = NULL;

	ASSERTCOND(pOleDoc->m_pIDoc->lpVtbl->m_GetItemObjectProcPtr != nil);
	return (*pOleDoc->m_pIDoc->lpVtbl->m_GetItemObjectProcPtr)(pOleDoc->m_pIDoc, pszItem, SpeedNeeded, pbc, riid, ppvObject);
}

//#pragma segment OleDocumentInterfaceSeg
STDMETHODIMP OleDocIntItemContainerGetObjectStorage(LPOLEITEMCONTAINER lpThis, char* pszItem, LPBINDCTX lpbc, REFIID riid, void* * lplpvStorage)
{
	OleDocumentPtr		pOleDoc;
	LPSTORAGE			lpStorage;
	HRESULT				hrErr;

	OleDbgEnterInterface();

	pOleDoc = ((DocOleItemContainerImplPtr)lpThis)->lpOleDoc;

    /* OLE2NOTE: we must make sure to set all out ptr parameters to NULL. */
    *lplpvStorage = NULL;

#if qOleServerApp

    /* OLE2NOTE: in the SERVER ONLY version, item names identify pseudo
    **    objects. pseudo objects, do NOT have identifiable storage.
    */
    return ResultFromScode(E_FAIL);

#elif qOleContainerApp

    // We can only return an IStorage* type pointer
    if (! IsEqualIID(riid, &IID_IStorage))
        return ResultFromScode(E_FAIL);

	ASSERTCOND(pOleDoc->m_pIDoc->lpVtbl->m_GetItemStorageProcPtr != nil);
	hrErr = (*pOleDoc->m_pIDoc->lpVtbl->m_GetItemStorageProcPtr)(pOleDoc->m_pIDoc, pszItem, (void**)&lpStorage);

	if (hrErr == NOERROR)
		hrErr = lpStorage->lpVtbl->QueryInterface(lpStorage, riid, lplpvStorage);

	return hrErr;

#endif
}

//#pragma segment OleDocumentInterfaceSeg
STDMETHODIMP OleDocIntItemContainerIsRunning(LPOLEITEMCONTAINER lpThis, char* pszItem)
{
	OleDocumentPtr		pOleDoc;

	OleDbgEnterInterface();

	pOleDoc = ((DocOleItemContainerImplPtr)lpThis)->lpOleDoc;

	ASSERTCOND(pOleDoc->m_pIDoc->lpVtbl->m_IsItemRunningProcPtr != nil);
	return (*pOleDoc->m_pIDoc->lpVtbl->m_IsItemRunningProcPtr)(pOleDoc->m_pIDoc, pszItem);
}

//#pragma segment OleDocumentInterfaceSeg
void DocIDataObjectInit(DocDataObjectImplPtr pDocDataObjectImpl, struct OleDocumentRec* pOleDoc)
{
	pDocDataObjectImpl->lpVtbl		= &gDataObjectVtbl;
	pDocDataObjectImpl->lpOleDoc	= pOleDoc;
	pDocDataObjectImpl->cRef		= 0;
}

//#pragma segment OleDocumentInterfaceSeg
STDMETHODIMP OleDocIntDataObjectQueryInterface(LPDATAOBJECT lpThis, REFIID riid, void* * lplpvObj)
{
	OleDocumentPtr		pOleDoc;

	OleDbgEnterInterface();

	pOleDoc = ((DocDataObjectImplPtr)lpThis)->lpOleDoc;

	return OleDocQueryInterface(pOleDoc, riid, lplpvObj);
}

//#pragma segment OleDocumentInterfaceSeg
STDMETHODIMP_(unsigned long) OleDocIntDataObjectAddRef(LPDATAOBJECT lpThis)
{
	OleDocumentPtr		pOleDoc;

	OleDbgEnterInterface();

	pOleDoc = ((DocDataObjectImplPtr)lpThis)->lpOleDoc;

	return OleDocAddRef(pOleDoc);
}

//#pragma segment OleDocumentInterfaceSeg
STDMETHODIMP_(unsigned long) OleDocIntDataObjectRelease (LPDATAOBJECT lpThis)
{
	OleDocumentPtr		pOleDoc;

	OleDbgEnterInterface();

	pOleDoc = ((DocDataObjectImplPtr)lpThis)->lpOleDoc;

	return OleDocRelease(pOleDoc);
}

//#pragma segment OleDocumentInterfaceSeg
STDMETHODIMP OleDocIntDataObjectGetData(LPDATAOBJECT lpThis, LPFORMATETC lpFormatetc, LPSTGMEDIUM lpMedium)
{
	OleDocumentPtr		pOleDoc;
	
	OleDbgEnterInterface();

	pOleDoc = ((DocDataObjectImplPtr)lpThis)->lpOleDoc;

	return OleDocDataObjectGetData(pOleDoc, lpFormatetc, lpMedium);
}

//#pragma segment OleDocumentInterfaceSeg
STDMETHODIMP OleDocIntDataObjectGetDataHere(LPDATAOBJECT lpThis, LPFORMATETC lpFormatetc, LPSTGMEDIUM lpMedium)
{
	OleDbgEnterInterface();

	return ResultFromScode(E_FAIL);
}

//#pragma segment OleDocumentInterfaceSeg
STDMETHODIMP OleDocIntDataObjectQueryGetData(LPDATAOBJECT lpThis, LPFORMATETC lpFormatetc)
{
	OleDocumentPtr	pOleDoc;
	HRESULT			hrErr;
	
	OleDbgEnterInterface();
	
	pOleDoc = ((DocDataObjectImplPtr)lpThis)->lpOleDoc;

	hrErr = OleDocDataObjectQueryGetData(pOleDoc, lpFormatetc);
	
	return hrErr;
}

//#pragma segment OleDocumentInterfaceSeg
STDMETHODIMP OleDocIntDataObjectGetCanonicalFormatEtc(LPDATAOBJECT lpThis, LPFORMATETC lpformatetc, LPFORMATETC lpformatetcOut)
{
	OleDbgEnterInterface();

	return ResultFromScode(E_FAIL);
}

//#pragma segment OleDocumentInterfaceSeg
STDMETHODIMP OleDocIntDataObjectSetData(LPDATAOBJECT lpThis, LPFORMATETC lpFormatetc, LPSTGMEDIUM lpMedium, unsigned long fRelease)
{
	OleDbgEnterInterface();

	return ResultFromScode(E_FAIL);
}

//#pragma segment OleDocumentInterfaceSeg
STDMETHODIMP OleDocIntDataObjectEnumFormatEtc(LPDATAOBJECT lpThis, unsigned long dwDirection, LPENUMFORMATETC * lplpenumFormatEtc)
{
	OleDbgEnterInterface();

	*lplpenumFormatEtc = NULL;

	return ResultFromScode(OLE_S_USEREG);
}

//#pragma segment OleDocumentInterfaceSeg
STDMETHODIMP OleDocIntDataObjectAdvise(LPDATAOBJECT lpThis, FORMATETC * lpFormatetc, unsigned long advf, LPADVISESINK lpAdvSink, unsigned long * lpdwConnection)
{
	OleDocumentPtr		pOleDoc;
	HRESULT				hrErr;
	
	OleDbgEnterInterface();
	
	pOleDoc = ((DocDataObjectImplPtr)lpThis)->lpOleDoc;
	
	*lpdwConnection = 0;
	
	hrErr = OleDocDataObjectAdvise(pOleDoc, lpFormatetc, advf, lpAdvSink, lpdwConnection);

	return hrErr;
}

//#pragma segment OleDocumentInterfaceSeg
STDMETHODIMP OleDocIntDataObjectUnadvise(LPDATAOBJECT lpThis, unsigned long dwConnection)
{
	OleDocumentPtr		pOleDoc;
	HRESULT				hrErr;
	
	OleDbgEnterInterface();
	
	pOleDoc = ((DocDataObjectImplPtr)lpThis)->lpOleDoc;

	hrErr = OleDocDataObjectUnadvise(pOleDoc, dwConnection);

	return hrErr;
}

//#pragma segment OleDocumentInterfaceSeg
STDMETHODIMP OleDocIntDataObjectEnumAdvise(LPDATAOBJECT lpThis, LPENUMSTATDATA * lplpenumAdvise)
{
	OleDbgEnterInterface();

	return ResultFromScode(E_FAIL);
}


#if qOleDragDrop

//#pragma segment OleDocumentInterfaceSeg
void DocIDropTargetInit(DocDropTargetImplPtr pDocDropTargetImpl, struct OleDocumentRec* pOleDoc)
{
	pDocDropTargetImpl->lpVtbl			= &gDropTargetVtbl;
	pDocDropTargetImpl->lpOleDoc		= pOleDoc;
	pDocDropTargetImpl->cRef			= 0;
}

STDMETHODIMP OleDocIntDropTargetQueryInterface(LPDROPTARGET lpThis, REFIID riid, void* * lplpvObj)
{
	OleDocumentPtr		pOleDoc;

	OleDbgEnterInterface();

	pOleDoc = ((DocDropTargetImplPtr)lpThis)->lpOleDoc;

	return OleDocQueryInterface(pOleDoc, riid, lplpvObj);
}

//#pragma segment OleDocumentInterfaceSeg
STDMETHODIMP_(unsigned long) OleDocIntDropTargetAddRef(LPDROPTARGET lpThis)
{
	OleDocumentPtr		pOleDoc;

	OleDbgEnterInterface();

	pOleDoc = ((DocDropTargetImplPtr)lpThis)->lpOleDoc;

	return OleDocAddRef(pOleDoc);
}

//#pragma segment OleDocumentInterfaceSeg
STDMETHODIMP_(unsigned long) OleDocIntDropTargetRelease (LPDROPTARGET lpThis)
{
	OleDocumentPtr		pOleDoc;

	OleDbgEnterInterface();

	pOleDoc = ((DocDropTargetImplPtr)lpThis)->lpOleDoc;

	return OleDocRelease(pOleDoc);
}

//#pragma segment OleDocumentInterfaceSeg
STDMETHODIMP    OleDocIntDropTargetDragEnter(
    LPDROPTARGET            lpThis,
    LPDATAOBJECT            lpDataObj,
    unsigned long           grfKeyState,
    POINTL                  pointl,
    unsigned long*          lpdwEffect
)
{
	OleDocumentPtr pOleDoc = ((DocDropTargetImplPtr)lpThis)->lpOleDoc;
	
	// REVIEW: should it be done in OleDoc or OleOutlineDoc?
    Boolean       fDragScroll = false;

	OleDbgEnterInterface();

    pOleDoc->m_TimeEnterScrollArea	= 0;
    pOleDoc->m_LastScrollDir		= SCROLLDIR_NULL;

    /* Determine if the drag source data object offers a data format
    **    that we can copy and/or link to.
    */
    OleDocQueryPasteFromData(
            pOleDoc,
            lpDataObj,
            &pOleDoc->m_fCanDropCopy,
            &pOleDoc->m_fCanDropLink
    );

    fDragScroll = OleDocDoDragScroll ( pOleDoc, pointl );

	OleDocQueryDrop(pOleDoc,grfKeyState,pointl,fDragScroll,lpdwEffect);

    return NOERROR;
}

//#pragma segment OleDocumentInterfaceSeg
STDMETHODIMP OleDocIntDropTargetDragOver(
    LPDROPTARGET            lpThis,
    unsigned long           grfKeyState,
    POINTL                  pointl,
    unsigned long*          lpdwEffect
)
{
	OleDocumentPtr pOleDoc = ((DocDropTargetImplPtr)lpThis)->lpOleDoc;
    Boolean		fDragScroll = false;

	OleDbgEnterInterface();

    fDragScroll = OleDocDoDragScroll ( pOleDoc, pointl );

	OleDocQueryDrop(pOleDoc, grfKeyState, pointl, fDragScroll, lpdwEffect);

    return NOERROR;
}


//#pragma segment OleDocumentInterfaceSeg
STDMETHODIMP    OleDocIntDropTargetDragLeave(LPDROPTARGET lpThis)
{
	OleDbgEnterInterface();

    return NOERROR;
}

//#pragma segment OleDocumentInterfaceSeg
STDMETHODIMP    OleDocIntDropTargetDrop(
    LPDROPTARGET            lpThis,
    LPDATAOBJECT            lpDataObj,
    unsigned long           grfKeyState,
    POINTL                  pointl,
    unsigned long*          lpdwEffect
)
{
	OleDocumentPtr pOleDoc = ((DocDropTargetImplPtr)lpThis)->lpOleDoc;

	OleDbgEnterInterface();

//    LPLINELIST lpLL     = (LPLINELIST)&((LPOUTLINEDOC)lpOleDoc)->m_LineList;

    OLEDEBUG_BEGIN2("OleDoc_DropTarget_Drop\r\n")

//    pOleDoc->m_fLocalDrop = TRUE;

//    LineList_RestoreDragFeedback( lpLL );
//    SetFocus( LineList_GetWindow( lpLL) );

    if (OleDocQueryDrop(pOleDoc, grfKeyState, pointl, FALSE, lpdwEffect))
    {
        Boolean fLink     = (*lpdwEffect == DROPEFFECT_LINK);
		
		// make sure the document is brought to the front
		ASSERTCOND(pOleDoc->m_pIDoc->lpVtbl->m_BringToFrontProcPtr != nil);
		(*pOleDoc->m_pIDoc->lpVtbl->m_BringToFrontProcPtr)(pOleDoc->m_pIDoc);

		OleDocPasteFromData(pOleDoc, lpDataObj, fLink);

#ifdef LATER
#if defined( INPLACE_CNTR )
        {
            LPCONTAINERDOC lpContainerDoc = (LPCONTAINERDOC)lpOleDoc;

            /* OLE2NOTE: if there is currently a UIActive OLE object,
            **    then we must tell it to UIDeactivate after
            **    the drop has completed.
            */
            if (lpContainerDoc->m_lpLastUIActiveLine) {
                ContainerLine_UIDeactivate(
                        lpContainerDoc->m_lpLastUIActiveLine);
            }
        }
#endif

#if defined( INPLACE_SVR )
        {
            /* OLE2NOTE: if the drop was into a in-place visible
            **    (in-place active but NOT UIActive object), then we
            **    want to UIActivate the object after the drop is
            **    complete.
            */
            ServerDoc_UIActivate((LPSERVERDOC) lpOleDoc);
        }
#endif
#endif // LATER
	
		// set document dirty
		ASSERTCOND(pOleDoc->m_pIDoc->lpVtbl->m_SetDirtyProcPtr != nil);
		(*pOleDoc->m_pIDoc->lpVtbl->m_SetDirtyProcPtr)(pOleDoc->m_pIDoc, true);
		
		OleDocSetSizeChanged(pOleDoc, true);
    }

    OLEDEBUG_END2

    return NOERROR;
}

#endif // qOleDragDrop

// OLDNAME: OleDocument.c
extern OleApplicationPtr	gOleApp;
extern ApplicationPtr		gApplication;

//#pragma segment OleDocumentSeg
void OleDocInit(OleDocumentPtr pOleDoc, DocImplPtr pIDoc, IUnknown* pIUnknown)
{
	pOleDoc->m_pIDoc = 			pIDoc;
	pOleDoc->m_pIUnknown = 		pIUnknown;

	pOleDoc->m_lpStorage = nil;
	
	pOleDoc->m_dwRegROT				= 0;
	pOleDoc->m_lpFileMoniker		= 0;
	pOleDoc->m_pSrcDocOfCopy		= nil;
	
	DocIPersistFileInit(&pOleDoc->m_PersistFile, pOleDoc);
	DocIOleItemContainerInit(&pOleDoc->m_OleItemContainer, pOleDoc);
	
	ASSERTCOND(pOleDoc->m_pIDoc->lpVtbl->m_IsDataTransferDocProcPtr != nil);
	if (!(*pOleDoc->m_pIDoc->lpVtbl->m_IsDataTransferDocProcPtr)(pOleDoc->m_pIDoc))
	{
#if qOleServerApp	
		DocIDataObjectInit(&pOleDoc->m_DataObject, pOleDoc);
#endif // qOleServerApp

		ASSERTCOND(pOleDoc->m_pIDoc->lpVtbl->m_DocLockAppProcPtr != nil);
		(*pOleDoc->m_pIDoc->lpVtbl->m_DocLockAppProcPtr)(pOleDoc->m_pIDoc);

#if qOleDragDrop
		DocIDropTargetInit(&pOleDoc->m_DropTarget, pOleDoc);
		pOleDoc->m_fCanDropCopy = false;
		pOleDoc->m_fCanDropLink  = false;
		pOleDoc->m_fRegDragDrop = false;
		pOleDoc->m_TimeEnterScrollArea = 0L;
		pOleDoc->m_LastScrollDir = SCROLLDIR_NULL;
		pOleDoc->m_NextScrollTime = 0L;
#endif // qOleDragDrop
	}
	else
	{
		DataXferDocIDataObjectInit(&pOleDoc->m_DataObject, pOleDoc);

#if qOleDragDrop
		DataXferDocIDropSourceInit(&pOleDoc->m_DropSource, pOleDoc);
#endif // qOleDragDrop
	}
	
#if qOleServerApp
	OleServerDocInit(pOleDoc);
#elif qOleContainerApp
	OleContainerDocInit(pOleDoc);	
#endif
}

//#pragma segment OleDocumentSeg
void OleDocDispose(OleDocumentPtr pOleDoc)
{
	OleDocumentPtr	pClipboardDoc;

	OleStdRevokeAsRunning(&pOleDoc->m_dwRegROT);

	if (pOleDoc->m_lpFileMoniker)
	{
		OleStdRelease((LPUNKNOWN)pOleDoc->m_lpFileMoniker);
		pOleDoc->m_lpFileMoniker = NULL;
    }

	ASSERTCOND(pOleDoc->m_pIDoc->lpVtbl->m_IsDataTransferDocProcPtr != nil);
	if (!(*pOleDoc->m_pIDoc->lpVtbl->m_IsDataTransferDocProcPtr)(pOleDoc->m_pIDoc))
	{
		ASSERTCOND(pOleDoc->m_pIDoc->lpVtbl->m_DocUnlockAppProcPtr != nil);
		(*pOleDoc->m_pIDoc->lpVtbl->m_DocUnlockAppProcPtr)(pOleDoc->m_pIDoc);
	}
	else if (pOleDoc == gOleApp->m_pClipboardDoc)
		gOleApp->m_pClipboardDoc = nil;
		
	// if the document is the source of the clipboard doc, we need to disable the
	// link format in the clipboard doc
	pClipboardDoc = gOleApp->m_pClipboardDoc;
	if (pClipboardDoc && pClipboardDoc->m_pSrcDocOfCopy == pOleDoc)
		OleDocDisableLinkSource(pOleDoc);

#if qOleServerApp
	OleServerDocDispose(pOleDoc);
#endif
}

//#pragma segment OleDocumentSeg
void OleDocOpenFile(OleDocumentPtr pOleDoc, Boolean readOnly, Boolean newFile)
{
	HRESULT			hrErr;
	FSSpec			fsSpec;
		
	// need to register it

	ASSERTCOND(pOleDoc->m_pIDoc->lpVtbl->m_GetFSSpecProcPtr != nil);
	(*pOleDoc->m_pIDoc->lpVtbl->m_GetFSSpecProcPtr)(pOleDoc->m_pIDoc, &fsSpec);

	if (newFile)
	{
		ASSERTCOND(pOleDoc->m_pIDoc->lpVtbl->m_GetCreatorProcPtr != nil);
		ASSERTCOND(pOleDoc->m_pIDoc->lpVtbl->m_GetTypeProcPtr != nil);

		hrErr = StgCreateDocfileFSp(
					&fsSpec,
					(*pOleDoc->m_pIDoc->lpVtbl->m_GetCreatorProcPtr)(pOleDoc->m_pIDoc),
					(*pOleDoc->m_pIDoc->lpVtbl->m_GetTypeProcPtr)(pOleDoc->m_pIDoc),
					smSystemScript,
					(readOnly ? STGM_READ : STGM_READWRITE) | STGM_TRANSACTED | STGM_SHARE_EXCLUSIVE | STGM_CREATE,
					0,
					&pOleDoc->m_lpStorage);
		ASSERTNOERROR(hrErr);
		FailOleErr(hrErr);
	}
	else
	{
		hrErr = StgOpenStorageFSp(
					&fsSpec,
					NULL,
					(readOnly ? STGM_READ : STGM_READWRITE) | STGM_TRANSACTED | STGM_SHARE_EXCLUSIVE,
					NULL,
					0,
					&pOleDoc->m_lpStorage);
		ASSERTNOERROR(hrErr);
		FailOleErr(hrErr);
	}
}

//#pragma segment OleDocumentSeg
void OleDocCloseFile(OleDocumentPtr pOleDoc)
{
	// need to unregister

	if (pOleDoc->m_lpStorage != nil)
	{
		OleStdRelease((LPUNKNOWN)pOleDoc->m_lpStorage);
		pOleDoc->m_lpStorage = nil;
	}
}

//#pragma segment DocumentSeg
void OleDocWriteToFile(OleDocumentPtr pOleDoc)
{
	HRESULT				hrErr;
	
	hrErr = WriteClassStg(pOleDoc->m_lpStorage, &CLSID_Application);
	ASSERTNOERROR(hrErr);
	
	ASSERTCOND(pOleDoc->m_pIDoc->lpVtbl->m_SaveToStgProcPtr != nil);
	hrErr = (*pOleDoc->m_pIDoc->lpVtbl->m_SaveToStgProcPtr)(pOleDoc->m_pIDoc, 0, pOleDoc->m_lpStorage, true);
	ASSERTNOERROR(hrErr);
		
	hrErr = pOleDoc->m_lpStorage->lpVtbl->Commit(pOleDoc->m_lpStorage, 0);
	ASSERTNOERROR(hrErr);
	
	// try commit changes in less robust manner
	if (GetScode(hrErr) == STGC_OVERWRITE)
	{
		hrErr = pOleDoc->m_lpStorage->lpVtbl->Commit(pOleDoc->m_lpStorage, STGC_OVERWRITE);
		ASSERTNOERROR(hrErr);
	}
	
	FailOleErr(hrErr);
}

//#pragma segment OleDocumentSeg
void OleDocShow(OleDocumentPtr pOleDoc)
{
	if (pOleDoc->m_fIsVisible)
		return;
		
	pOleDoc->m_fIsVisible = true;
	
	OleDocLock(pOleDoc, TRUE /* fLock */, false);
	
#if qOleDragDrop
	if (!pOleDoc->m_fRegDragDrop)
	{
		WindowPtr pWin;
		
		ASSERTCOND(pOleDoc->m_pIDoc->lpVtbl->m_GetWindowProcPtr != nil);
		pWin = (*pOleDoc->m_pIDoc->lpVtbl->m_GetWindowProcPtr)(pOleDoc->m_pIDoc);
		RegisterDragDrop(pWin, (LPDROPTARGET)&pOleDoc->m_DropTarget);
		
		pOleDoc->m_fRegDragDrop = true;
	}
#endif
	
#if qOleServerApp
	OleServerDocShow(pOleDoc);
#endif
}

//#pragma segment OleDocumentSeg
void OleDocHide(OleDocumentPtr pOleDoc)
{
	if (!pOleDoc->m_fIsVisible)
		return;
		
	pOleDoc->m_fIsVisible = false;
		
#if qOleDragDrop
	if (pOleDoc->m_fRegDragDrop)
	{
		WindowPtr pWin;
		
		ASSERTCOND(pOleDoc->m_pIDoc->lpVtbl->m_GetWindowProcPtr != nil);
		pWin = (*pOleDoc->m_pIDoc->lpVtbl->m_GetWindowProcPtr)(pOleDoc->m_pIDoc);
		RevokeDragDrop(pWin);
		
		pOleDoc->m_fRegDragDrop = false;
	}
#endif
	
	{
		Boolean		fLastUnlockReleases = false;
		
#if qOleServerApp && qOleInPlace
		if (!OleInPlaceServerDocIsInPlace(pOleDoc))
#endif
		{
			ASSERTCOND(pOleDoc->m_pIDoc->lpVtbl->m_IsClosingProcPtr != nil);
			fLastUnlockReleases = (*pOleDoc->m_pIDoc->lpVtbl->m_IsClosingProcPtr)(pOleDoc->m_pIDoc);
		}
	
		OleDocLock(pOleDoc, FALSE /* fLock */, fLastUnlockReleases);
	}

#if qOleServerApp
	OleServerDocHide(pOleDoc);
#endif
}

//#pragma segment OleDocumentSeg
void OleDocDoActivate(OleDocumentPtr pOleDoc, Boolean becomingActive)
{
#if qOleServerApp
	OleServerDocDoActivate(pOleDoc, becomingActive);
#elif qOleContainerApp
	OleContainerDocDoActivate(pOleDoc, becomingActive);
#endif
}

//#pragma segment OleDocumentSeg
void OleDocUpdateWindowPosition(OleDocumentPtr pOleDoc)
{
#if qOleInPlace
#if qOleServerApp
	OleInPlaceServerDocUpdateWindowPosition(pOleDoc);
#elif qOleContainerApp
	OleInPlaceContainerDocUpdateWindowPosition(pOleDoc);
#endif
#endif
}

//#pragma segment OleDocumentSeg
void OleDocDoSave(OleDocumentPtr pOleDoc)
{
#if qOleServerApp
	OleServerDocDoSave(pOleDoc);
#endif
}

//#pragma segment OleDocumentSeg
void OleDocDoSaveAs(OleDocumentPtr pOleDoc)
{
#if qOleServerApp
	OleServerDocSendAdvise(pOleDoc, OLE_ONSAVE, nil, 0);
#endif
}

//#pragma segment OleDocumentSeg
void OleDocDoClose(OleDocumentPtr pOleDoc, Boolean askUserToSave, YNCResult defaultAnswer, Boolean quitting)
{
	OleDocAddRef(pOleDoc);			// artificial addref
	
#if qOleServerApp
	OleServerDocDoClose(pOleDoc, askUserToSave, defaultAnswer, quitting);
#endif

	CoDisconnectObject((LPUNKNOWN)pOleDoc->m_pIUnknown, 0);
	
	OleDocRelease(pOleDoc);			// release artificial addref
}

//#pragma segment OleDocumentSeg
Boolean OleDocSetDefaultCursor(OleDocumentPtr pOleDoc)
{
#if qOleServerApp && qOleInPlace
	return OleInPlaceServerDocSetDefaultCursor(pOleDoc);
#endif

	return false;
}

//#pragma segment OleDocumentSeg
void OleDocDoIdle(OleDocumentPtr pOleDoc)
{
#if qOleServerApp
	OleServerDocDoIdle(pOleDoc);
#endif
}

//#pragma segment OleDocumentSeg
void OleDocDoCopy(OleDocumentPtr pOleDoc)
{
    /* OLE2NOTE: initially the Doc object is created with a 0 ref
    **    count. in order to have a stable Doc object during the
    **    process of initializing the Doc instance and transfering it
    **    to the clipboard, we intially AddRef the Doc ref cnt and later
    **    Release it. This initial AddRef is artificial; it is simply
    **    done to guarantee that a harmless QueryInterface followed by
    **    a Release does not inadvertantly force our object to destroy
    **    itself prematurely.
    */
	OleDocAddRef(pOleDoc);

    /* OLE2NOTE: the OLE 2.0 style to put data onto the clipboard is to
    **    give the clipboard a pointer to an IDataObject interface that
    **    is able to statisfy IDataObject::GetData calls to render
    **    data. in our case we give the pointer to the ClipboardDoc
    **    which holds a cloned copy of the current user's selection.
    */
    OleSetClipboard((LPDATAOBJECT)&pOleDoc->m_DataObject);

    OleDocRelease(pOleDoc);   // rel artificial AddRef above

    gOleApp->m_pClipboardDoc = pOleDoc;
}

#if qOleDragDrop

/* OleDocDoDragScroll
 * ------------------
 * Check to see if Drag scroll operation should be initiated. A Drag scroll
 * operation should be initiated when the mouse has remained in the active
 * scroll area (11 pixels frame around border of window) for a specified
 * amount of time (50ms).
 */

Boolean OleDocDoDragScroll(OleDocumentPtr pOleDoc, POINTL pointl)
{
	Point 		mPt;
	Rect		rClient;
	SCROLLDIR	scrolldir = SCROLLDIR_NULL;
	long		Time = TickCount() * 1000 / 60;		// convert to millisecond
	GrafPtr		savePort;
	GrafPtr		docPort;
	
    mPt.h = (short)pointl.x;
    mPt.v = (short)pointl.y;

	ASSERTCOND(pOleDoc->m_pIDoc->lpVtbl->m_GetClientRectProcPtr != nil);
	(*pOleDoc->m_pIDoc->lpVtbl->m_GetClientRectProcPtr)(pOleDoc->m_pIDoc, &rClient);
	
	ASSERTCOND(pOleDoc->m_pIDoc->lpVtbl->m_GetWindowProcPtr != nil);
	docPort = (GrafPtr)(*pOleDoc->m_pIDoc->lpVtbl->m_GetWindowProcPtr)(pOleDoc->m_pIDoc);
	ASSERTCOND(docPort != nil);

	GetPort(&savePort);
	SetPort(docPort);	
	GlobalToLocal(&mPt);
	SetPort(savePort);
	
	
    if ((rClient.top <= mPt.v) && (mPt.v<= rClient.top + kScrollInset))
        scrolldir = SCROLLDIR_UP;
    else if ((rClient.bottom - kScrollInset <= mPt.v) && (mPt.v <= rClient.bottom))
        scrolldir = SCROLLDIR_DOWN;

    if (pOleDoc->m_TimeEnterScrollArea) {

        /* cursor was already in Scroll Area */

        if (! scrolldir) {
            /* cusor moved OUT of scroll area.
            **      clear "EnterScrollArea" time.
            */
            pOleDoc->m_TimeEnterScrollArea = 0L;
            pOleDoc->m_NextScrollTime = 0L;
            pOleDoc->m_LastScrollDir = SCROLLDIR_NULL;
        } else if (scrolldir != pOleDoc->m_LastScrollDir) {
            /* cusor moved into a different direction scroll area.
            **      reset "EnterScrollArea" time to start a new 50ms delay.
            */
            pOleDoc->m_TimeEnterScrollArea = Time;
            pOleDoc->m_NextScrollTime = Time + kScrollDelay;
            pOleDoc->m_LastScrollDir = scrolldir;
        } else if (Time  && Time >= pOleDoc->m_NextScrollTime) {
			ASSERTCOND(pOleDoc->m_pIDoc->lpVtbl->m_ScrollProcPtr != nil);
            (*pOleDoc->m_pIDoc->lpVtbl->m_ScrollProcPtr)(pOleDoc->m_pIDoc, scrolldir);  // Scroll doc NOW
            pOleDoc->m_NextScrollTime = Time + kScrollInterval;
        }
    } else {
        if (scrolldir) {
            /* cusor moved INTO a scroll area.
            **      reset "EnterScrollArea" time to start a new 50ms delay.
            */
            pOleDoc->m_TimeEnterScrollArea = Time;
            pOleDoc->m_NextScrollTime = Time + kScrollDelay;
            pOleDoc->m_LastScrollDir = scrolldir;
        }
    }

    return (scrolldir ? true : false);
}

//#pragma segment OleDocumentSeg
/* OleDocStartDrag
 * ---------------
 *
 *	Purpose:
 *		Do a mouse loop and see if Dragging should be started. The criteria is to hold the
 *		button down and exceed a distance or time threshold
 *
 *	Parameter:
 *		mPt		position of the mouse when the function is called
 *
 *	Returns:
 *		true	if dragging can be started
 *		false 	otherwise
 */
Boolean OleDocStartDrag(
    OleDocumentPtr  pOleDoc,
	Point			mPt)
{
	long		Time = TickCount() * 1000 / 60;		// convert to millisecond
	Point		mCurrentPt;
	long		currentTime;
	
	while (StillDown())
	{
		GetMouse(&mCurrentPt);
		
		// distance threshold exceeded
		if ((ABS(mCurrentPt.h - mPt.h) > kDragMinDist) || (ABS(mCurrentPt.v - mPt.v) > kDragMinDist))
			return true;
		
		currentTime = TickCount() * 1000 / 60;
		
		// time threshold exceeded
		if (currentTime - Time > kDragDelay)
			return true;
	}
	
	return false;	// mouse release without dragging
}

//#pragma segment OleDocumentSeg
/* OleDocQueryDrop
** ---------------
**    Check if the desired drop operation (identified by the given key
**    state) is possible at the current mouse position (pointl).
*/
Boolean OleDocQueryDrop (
    OleDocumentPtr  pOleDoc,
    unsigned long   grfKeyState,
    POINTL          pointl,
    Boolean         fDragScroll,
    unsigned long*  pEffect
)
{
	Rect			rClient;
	Point			mPt;
	GrafPtr			savePort;
	GrafPtr			docPort;
	OleDocumentPtr	pDragSourceDoc = gOleApp->m_pDragSourceDoc;
	
    /* if we have already determined that the source does NOT have any
    **    acceptable data for us, the return NO-DROP
    */
    if (! pOleDoc->m_fCanDropCopy && ! pOleDoc->m_fCanDropLink)
        goto dropeffect_none;

	ASSERTCOND(pOleDoc->m_pIDoc->lpVtbl->m_GetClientRectProcPtr != nil);
	(*pOleDoc->m_pIDoc->lpVtbl->m_GetClientRectProcPtr)(pOleDoc->m_pIDoc, &rClient);
	
	mPt.h = pointl.x;
	mPt.v = pointl.y;
	
	ASSERTCOND(pOleDoc->m_pIDoc->lpVtbl->m_GetWindowProcPtr != nil);
	docPort = (GrafPtr)(*pOleDoc->m_pIDoc->lpVtbl->m_GetWindowProcPtr)(pOleDoc->m_pIDoc);
	ASSERTCOND(docPort != nil);

	GetPort(&savePort);
	SetPort(docPort);	
	GlobalToLocal(&mPt);
	SetPort(savePort);
	
	/* the mouse cursor is not on the client area, no drop allowed */
	if (!PtInRect(mPt, &rClient))
		goto dropeffect_none;
	
    /* OLE2NOTE: determine what type of drop should be performed given
    **    the current modifier key state. we rely on the standard
    **    interpretation of the modifier keys:
    **			no modifier  --	DROPEFFECT_MOVE for same doc,
    **							DROPEFFECT_COPY for diff doc
    **			Option       --	DROPEFFECT_COPY
    **			Shift		 -- DROPEFFECT_MOVE
    **			Option+Shift --	DROPEFFECT_LINK
    */
	if (grfKeyState & optionKey) {
		if (grfKeyState & shiftKey)
			*pEffect = DROPEFFECT_LINK;
		else
			*pEffect = DROPEFFECT_COPY;
	}
	else {
		if (pDragSourceDoc && (pOleDoc == pDragSourceDoc->m_pSrcDocOfCopy))
			*pEffect = DROPEFFECT_MOVE;
		else
			*pEffect = DROPEFFECT_COPY;
	}	

    if ( (*pEffect == DROPEFFECT_COPY || *pEffect == DROPEFFECT_MOVE)
            && ! pOleDoc->m_fCanDropCopy )
        goto dropeffect_none;

    if ( *pEffect == DROPEFFECT_LINK && ! pOleDoc->m_fCanDropLink )
        goto dropeffect_none;

    if (fDragScroll)
	    *pEffect |= DROPEFFECT_SCROLL;

    return TRUE;

dropeffect_none:

    *pEffect = DROPEFFECT_NONE;
    return FALSE;
}

//#pragma segment OleDocumentSeg
unsigned long OleDocDoDragDrop(OleDocumentPtr pOleDoc)
{
	unsigned long 		Effect = 0;
	OleApplicationPtr	pOleApp = gOleApp;
	
	/* OLE2NOTE: We need to store the drag source document here so as to
	**			 determine whether we are dropping to the source document
	**			 while we are doing drag drop
	*/
	pOleApp->m_pDragSourceDoc = pOleDoc;
	
    /* OLE2NOTE: initially the Doc object is created with a 0 ref
    **    count. in order to have a stable Doc object during the
    **    process of initializing the Doc instance and performing the
    **    drag operation, we intially AddRef the Doc ref cnt and later
    **    Release it. This initial AddRef is artificial; it is simply
    **    done to guarantee that a harmless QueryInterface followed by
    **    a Release does not inadvertantly force our object to destroy
    **    itself prematurely.
    */
    OleDocAddRef(pOleDoc);

    DoDragDrop((LPDATAOBJECT)&pOleDoc->m_DataObject,
               (LPDROPSOURCE)&pOleDoc->m_DropSource,
               DROPEFFECT_MOVE | DROPEFFECT_COPY | DROPEFFECT_LINK,
               (unsigned long*) &Effect
    );

    OleDocRelease(pOleDoc);  // rel artificial AddRef above

    // OLE2NOTE: We will clear the member variable here after finishing
	pOleApp->m_pDragSourceDoc = nil;
	
    return Effect;
}

#endif // qOleDragDrop

//#pragma segment OleDocumentSeg
void OleDocSetDirty(OleDocumentPtr pOleDoc, Boolean fDirty)
{
#if qOleServerApp
	OleServerDocSetDirty(pOleDoc, fDirty);
#endif
}

//#pragma segment OleDocumentSeg
void OleDocSetSizeChanged(OleDocumentPtr pOleDoc, Boolean fSizeChanged)
{
#if qOleServerApp

#if 0
#if qOleInPlace
	if (OleInPlaceServerDocIsUIVisible(pOleDoc))
		return;
#endif
#endif

	OleServerDocSetSizeChanged(pOleDoc, fSizeChanged);

#endif
}

//#pragma segment OleDocumentSeg
unsigned long OleDocAddRef(OleDocumentPtr pOleDoc)
{
	return (*pOleDoc->m_pIUnknown->lpVtbl->AddRef)(pOleDoc->m_pIUnknown);
}

//#pragma segment OleDocumentSeg
unsigned long OleDocRelease(OleDocumentPtr pOleDoc)
{
	return (*pOleDoc->m_pIUnknown->lpVtbl->Release)(pOleDoc->m_pIUnknown);
}

//#pragma segment OleDocumentSeg
HRESULT OleDocLock(OleDocumentPtr pOleDoc, Boolean fLock, Boolean fLastUnlockReleases)
{
    HRESULT hrErr;

    OleDocAddRef(pOleDoc);       // artificial AddRef to make object stable

    hrErr = CoLockObjectExternal(pOleDoc->m_pIUnknown, fLock, fLastUnlockReleases);
	ASSERTNOERROR(hrErr);
	
    OleDocRelease(pOleDoc);       // release artificial AddRef above

    return hrErr;
}

//#pragma segment OleDocumentSeg
HRESULT OleDocQueryInterface(OleDocumentPtr pOleDoc, REFIID riid, void* * lplpvObj)
{
	return (*pOleDoc->m_pIUnknown->lpVtbl->QueryInterface)(pOleDoc->m_pIUnknown, riid, lplpvObj);
}

//#pragma segment OleDocumentSeg
HRESULT OleDocLocalQueryInterface(OleDocumentPtr pOleDoc, REFIID riid, void* * lplpvObj)
{	
	ASSERTCOND(pOleDoc->m_pIDoc->lpVtbl->m_IsDataTransferDocProcPtr != nil);

	if ((*pOleDoc->m_pIDoc->lpVtbl->m_IsDataTransferDocProcPtr)(pOleDoc->m_pIDoc))
	{
		if (IsEqualIID(riid, &IID_IDataObject))
		{
			*lplpvObj = &pOleDoc->m_DataObject;
			OleDocAddRef(pOleDoc);
			return NOERROR;
		}
#if qOleDragDrop
		else if (IsEqualIID(riid, &IID_IDropSource))
		{
			*lplpvObj = &pOleDoc->m_DropSource;
			OleDocAddRef(pOleDoc);
			return NOERROR;
		}
#endif // qOleDragDrop
	}
	
	if (!(*pOleDoc->m_pIDoc->lpVtbl->m_IsDataTransferDocProcPtr)(pOleDoc->m_pIDoc))
	{
		if (IsEqualIID(riid, &IID_IPersist) || IsEqualIID(riid, &IID_IPersistFile))
		{
			*lplpvObj = &pOleDoc->m_PersistFile;
			OleDocAddRef(pOleDoc);
			return NOERROR;
		}
		else if (IsEqualIID(riid, &IID_IOleItemContainer)
					|| IsEqualIID(riid, &IID_IOleContainer)
					|| IsEqualIID(riid, &IID_IParseDisplayName))
		{
			*lplpvObj = &pOleDoc->m_OleItemContainer;
			OleDocAddRef(pOleDoc);
			return NOERROR;
		}
#if qOleDragDrop
		else if (IsEqualIID(riid, &IID_IDropTarget))
		{
			*lplpvObj = &pOleDoc->m_DropTarget;
			OleDocAddRef(pOleDoc);
			return NOERROR;
		}
#endif // qOleDragDrop
	}
	
#if qOleServerApp
		return OleServerDocLocalQueryInterface(pOleDoc, riid, lplpvObj);
#elif qOleContainerApp
		return OleContainerDocLocalQueryInterface(pOleDoc, riid, lplpvObj);
#endif
	
	*lplpvObj = NULL;
	return ResultFromScode(E_NOINTERFACE);
}

//#pragma segment OleDocumentSeg
HRESULT OleDocDataObjectGetData(OleDocumentPtr pOleDoc, LPFORMATETC lpFormatetc, LPSTGMEDIUM lpMedium)
{
#if qOleServerApp
	return OleServerDocDataObjectGetData(pOleDoc, lpFormatetc, lpMedium);
#else
	return NOERROR;
#endif
}

//#pragma segment OleDocumentSeg
HRESULT OleDocDataObjectQueryGetData(OleDocumentPtr pOleDoc, LPFORMATETC lpFormatetc)
{
#if qOleServerApp
	return OleServerDocDataObjectQueryGetData(pOleDoc, lpFormatetc);
#else
	return NOERROR;
#endif
}

//#pragma segment OleDocumentSeg
HRESULT OleDocDataObjectAdvise(OleDocumentPtr pOleDoc, FORMATETC * lpFormatetc, unsigned long advf, LPADVISESINK lpAdvSink, unsigned long * lpdwConnection)
{
#if qOleServerApp
	return OleServerDocDataObjectAdvise(pOleDoc, lpFormatetc, advf, lpAdvSink, lpdwConnection);
#else
	return NOERROR;
#endif
}

//#pragma segment OleDocumentSeg
HRESULT OleDocDataObjectUnadvise(OleDocumentPtr pOleDoc, unsigned long dwConnection)
{
#if qOleServerApp
	return OleServerDocDataObjectUnadvise(pOleDoc, dwConnection);
#else
	return NOERROR;
#endif
}

//#pragma segment OleDocumentSeg
void OleDocRenamedUpdate(OleDocumentPtr pOleDoc, LPMONIKER lpmkDoc)
{
#if qOleServerApp
	OleServerDocRenamedUpdate(pOleDoc, lpmkDoc);
#elif qOleContainerApp
	OleContainerDocRenamedUpdate(pOleDoc, lpmkDoc);
#endif

	OleDocAddRef(pOleDoc);			// temporary addref

	OleStdRegisterAsRunning((LPUNKNOWN)pOleDoc->m_pIUnknown, lpmkDoc, &pOleDoc->m_dwRegROT);
	
	OleDocRelease(pOleDoc);
}

//#pragma segment OleDocumentSeg
LPSTORAGE OleDocGetStorage(OleDocumentPtr pOleDoc)
{
	return pOleDoc->m_lpStorage;
}

/* OleDocGetFullMoniker
** --------------------
**    Return the full, absolute moniker of the document.
**
**    NOTE: the caller must release the pointer returned when done.
*/
//#pragma segment OleDocumentSeg
LPMONIKER OleDocGetFullMoniker(OleDocumentPtr pOleDoc, unsigned long dwAssign)
{
    LPMONIKER lpMoniker = NULL;

    OLEDBG_BEGIN3("OleDoc_GetFullMoniker\r\n")

    if (pOleDoc->m_pSrcDocOfCopy) {
        /* CASE I: this document was created for a copy or drag/drop
        **    operation. generate the moniker which identifies the
        **    source document of the original copy.
        */
        if (! pOleDoc->m_fLinkSourceAvail)
            goto done;        // we already know a moniker is not available

        lpMoniker = OleDocGetFullMoniker(
                pOleDoc->m_pSrcDocOfCopy,
                dwAssign
        );
    }
    else if (pOleDoc->m_lpFileMoniker) {

        /* CASE II: this document is a top-level user document (either
        **    file-based or untitled). return the FileMoniker stored
        **    with the document; it uniquely identifies the document.
        */

        // we must AddRef the moniker to pass out a ptr
        pOleDoc->m_lpFileMoniker->lpVtbl->AddRef(pOleDoc->m_lpFileMoniker);

        lpMoniker = pOleDoc->m_lpFileMoniker;
    }

#if qOleServerApp

    else if (pOleDoc->server.m_lpOleClientSite) {

        /* CASE III: this document is an embedded object, ask our
        **    container for our moniker.
        */
        OLEDBG_BEGIN2("IOleClientSite::GetMoniker called\r\n");
        pOleDoc->server.m_lpOleClientSite->lpVtbl->GetMoniker(
                pOleDoc->server.m_lpOleClientSite,
                dwAssign,
                OLEWHICHMK_OBJFULL,
                &lpMoniker
        );
        OLEDBG_END2
    }

#endif

    else {
        lpMoniker = NULL;
    }

done:
    OLEDBG_END3
    return lpMoniker;
}

/* OleDocSetFileMoniker
** --------------------
**	Purpose:
**    Set the file moniker of the document
**
**	Parameters:
**		pSpec - FSSpec for moniker
*/
//#pragma segment OleDocumentSeg
void OleDocSetFileMonikerFSSpec(OleDocumentPtr pOleDoc, FSSpecPtr pSpec)
{	
	if (pOleDoc->m_lpFileMoniker)
	{
		OleStdRelease((LPUNKNOWN)pOleDoc->m_lpFileMoniker);
		pOleDoc->m_lpFileMoniker = nil;
    }

    CreateFileMonikerFSp(pSpec, &pOleDoc->m_lpFileMoniker);
    OleDocRenamedUpdate(pOleDoc, pOleDoc->m_lpFileMoniker);
}

//#pragma segment OleDocumentSeg
HRESULT OleDocLoadFsp(OleDocumentPtr pOleDoc, FSSpecPtr pSpec, unsigned long grfMode)
{
    SCODE	sc;

    /* OLE2NOTE: grfMode passed from the caller indicates if the caller
    **    needs Read or ReadWrite permissions. if appropriate the
    **    callee should open the file with the requested permissions.
    **    the caller will normally not impose sharing permissions.
    **
    **    the sample code currently always opens its file ReadWrite.
    */

	ASSERTCOND(pOleDoc->m_pIDoc->lpVtbl->m_SetFSSpecProcPtr != nil);
	(*pOleDoc->m_pIDoc->lpVtbl->m_SetFSSpecProcPtr)(pOleDoc->m_pIDoc, (FSSpecPtr)pSpec);

	ASSERTCOND(pOleDoc->m_pIDoc->lpVtbl->m_SetWindowTitleProcPtr != nil);
	(*pOleDoc->m_pIDoc->lpVtbl->m_SetWindowTitleProcPtr)(pOleDoc->m_pIDoc, (StringPtr)pSpec->name);

	TRY
	{
		OleDocOpenFile(
				pOleDoc,
				false,	/* readOnly */
				false	/* newFile */
		);

		ASSERTCOND(pOleDoc->m_pIDoc->lpVtbl->m_LoadFromStgProcPtr != nil);
		(*pOleDoc->m_pIDoc->lpVtbl->m_LoadFromStgProcPtr)(pOleDoc->m_pIDoc, pOleDoc->m_lpStorage);
	
		OleDocSetFileMonikerFSSpec(pOleDoc, (FSSpecPtr)pSpec);

        sc = S_OK;
	}
	CATCH
	{
        sc = S_FALSE;

    	NO_PROPAGATE;
	}
	ENDTRY
	
	return ResultFromScode(sc);
}

/* OleDocDoPaste
** -------------
**    Paste default format data from the clipboard.
**    In this function we choose the highest fidelity format that the
**    source clipboard IDataObject* offers that we understand.
**
**    OLE2NOTE: clipboard handling in an OLE 2.0 application is
**    different than normal Windows clipboard handling. Data from the
**    clipboard is retieved by getting the IDataObject* pointer
**    returned by calling OleGetClipboard.
*/
//#pragma segment OleDocumentSeg
void OleDocDoPaste(OleDocumentPtr pOleDoc)
{
    volatile LPDATAOBJECT pClipboardDataObj = NULL;
    Boolean fLink = false;
    HRESULT hrErr;

    hrErr = OleGetClipboard((LPDATAOBJECT *)&pClipboardDataObj);
    if (hrErr != NOERROR)
        return;     // Clipboard seems to be empty or can't be accessed

	TRY
	{
		hrErr = OleDocPasteFromData(
	            	pOleDoc,
	            	pClipboardDataObj,
	            	fLink);
		ASSERTNOERROR(hrErr);
		FailOleErr(hrErr);
	
	    if (pClipboardDataObj)
	        OleStdRelease((LPUNKNOWN)pClipboardDataObj);
	}
	CATCH
	{
	    if (pClipboardDataObj)
	        OleStdRelease((LPUNKNOWN)pClipboardDataObj);
	}
	ENDTRY

	// set document dirty
	ASSERTCOND(pOleDoc->m_pIDoc->lpVtbl->m_SetDirtyProcPtr != nil);
	(*pOleDoc->m_pIDoc->lpVtbl->m_SetDirtyProcPtr)(pOleDoc->m_pIDoc, true);
	
	OleDocSetSizeChanged(pOleDoc, true);
}


/* OleDocPasteFromData
** -------------------
**
**    Paste data from an IDataObject*. The IDataObject* may come from
**    the clipboard (GetClipboard) or from a drag/drop operation.
**    In this function we choose the best format that we prefer.
**
**    Returns TRUE if data was successfully pasted.
**            FALSE if data could not be pasted.
*/

//#pragma segment OleDocumentSeg
HRESULT OleDocPasteFromData(
        OleDocumentPtr    pOleDoc,
        LPDATAOBJECT      pSrcDataObj,
        Boolean           fLink
)
{
	OleApplicationPtr	pOleApp = gOleApp;
    ResType     		cfFormat;
    Boolean     		fDisplayAsIcon = FALSE;
    Handle      		hMem = NULL;
    PicHandle   		hMetaPict = NULL;
    STGMEDIUM			medium;
	HRESULT				hrErr;

	hrErr = NOERROR;

	TRY
	{
		if (fLink)
		{
		
#if qOleServerApp
			return NOERROR;       // server version of app does NOT support links
#elif qOleContainerApp
			// container version of app only supports OLE object type links
			cfFormat = pOleApp->m_cfLinkSource;
#endif
	
		}
		else
		{
			int nFmtEtc;
	
			nFmtEtc = OleStdGetPriorityClipboardFormat(
				pSrcDataObj,
				pOleApp->m_arrPasteEntries,
				pOleApp->m_nPasteEntries
			);
	
			if (nFmtEtc < 0)
				return ResultFromScode(E_FAIL);   // there is no format we like
	
			cfFormat = pOleApp->m_arrPasteEntries[nFmtEtc].fmtetc.cfFormat;
		}
	
		/* OLE2NOTE: we need to check what dwDrawAspect is being
		**    transfered. if the data is an object that is displayed as an
		**    icon in the source, then we want to keep it as an icon. the
		**    aspect the object is displayed in at the source is transfered
		**    via the CF_OBJECTDESCRIPTOR format for a Paste operation.
		*/
		hMem = OleStdGetData(pSrcDataObj, pOleApp->m_cfObjectDescriptor,
							 NULL, DVASPECT_CONTENT, (LPSTGMEDIUM)&medium);
		if (hMem)
		{
			LPOBJECTDESCRIPTOR lpOD;
			
			HLock(hMem);
			lpOD = (LPOBJECTDESCRIPTOR)*hMem;
			fDisplayAsIcon = (lpOD->dwDrawAspect == DVASPECT_ICON ? TRUE : FALSE);
			HUnlock(hMem);
			ReleaseStgMedium((LPSTGMEDIUM)&medium);     // equiv to GlobalFree
	
			if (fDisplayAsIcon) {
				hMetaPict = (PicHandle) OleStdGetData(
												pSrcDataObj,
												'PICT',
												NULL,
												DVASPECT_ICON,
												(LPSTGMEDIUM)&medium);
				if (hMetaPict == NULL)
					fDisplayAsIcon = FALSE; // give up; failed to get icon MFP
			}
		}
	
		OleDocPasteFormatFromData(
				pOleDoc,
				cfFormat,
				pSrcDataObj,
				fLink,
				fDisplayAsIcon,
				hMetaPict
		);
	
		if (hMetaPict)
			ReleaseStgMedium((LPSTGMEDIUM)&medium);  // properly free METAFILEPICT
	}
	CATCH
	{
		if (IsOleFailure())
			hrErr = GetOleFailure();
		else
			hrErr = ResultFromScode(E_FAIL);

	    NO_PROPAGATE;
	}
	ENDTRY

	return hrErr;
}


/* OleDocPasteFormatFromData
** -------------------------
**
**    Paste a particular data format from a IDataObject*. The
**    IDataObject* may come from the clipboard (GetClipboard) or from a
**    drag/drop operation.
**
**    Returns TRUE if data was successfully pasted.
**            FALSE if data could not be pasted.
*/

//#pragma segment OleDocumentSeg
void OleDocPasteFormatFromData(
        OleDocumentPtr      pOleDoc,
        ResType          	cfFormat,
        LPDATAOBJECT        pSrcDataObj,
        unsigned long       fLink,
        unsigned long       fDisplayAsIcon,
        PicHandle           hMetaPict
)
{
#if qOleServerApp

    /* call server specific version of the function. */
    OleServerDocPasteFormatFromData(
            pOleDoc,
            cfFormat,
            pSrcDataObj,
            (Boolean)fLink
    );

#elif qOleContainerApp

    /* call container specific version of the function. */
    OleContainerDocPasteFormatFromData(
            pOleDoc,
            cfFormat,
            pSrcDataObj,
            (Boolean)fLink,
            (Boolean)fDisplayAsIcon,
            hMetaPict);

#endif
}


/* OleDocQueryPasteFromData
** ------------------------
**
**    Check if the IDataObject* offers data in a format that we can
**    paste or paste link. The IDataObject* may come from the clipboard
**    (GetClipboard) or from a drag/drop operation.
**
*/
//#pragma segment OleDocumentSeg
void OleDocQueryPasteFromData(
    	OleDocumentPtr 		pOleDoc,		// not used
        LPDATAOBJECT        pSrcDataObj,
        Boolean*			pCanCopy,
        Boolean*            pCanLink
)
{
    OleApplicationPtr pOleApp = gOleApp;

    if (pCanLink)
    {
#if qOleServerApp
		/* we do not support links */
		*pCanLink = false;
#elif qOleContainerApp
        /* check if we can paste a Link to the data */
        *pCanLink = (OleQueryLinkFromData(pSrcDataObj) == NOERROR);
#endif
	}
	
	if (pCanCopy)
    {
        int nFmtEtc;

        nFmtEtc = OleStdGetPriorityClipboardFormat(
                pSrcDataObj,
                pOleApp->m_arrPasteEntries,
                pOleApp->m_nPasteEntries
        );

        *pCanCopy = (nFmtEtc >= 0);
    }
}

/* OleDocGetPasteStatus
 * --------------------
 *
 *	Purpose:
 *		Get the current state of the clipboard and determine if we can paste
 *		and/or paste link
 *
 *	Parameters:
 *		pCanCopy - pointer to a boolean for paste
 *		pCanLink - pointer to a boolean for paste link
 *
 */
//#pragma segment OleDocumentSeg
void OleDocGetPasteStatus(
   		OleDocumentPtr 		pOleDoc,		// not used
    	Boolean*			pCanCopy,
    	Boolean*            pCanLink
)
{
	LPDATAOBJECT    pDataObj = nil;
    HRESULT			hrErr;

#if qOleMessageFilter
	Boolean			fPrevNREnable,
					fPrevBusyEnable;

	if (gOleApp->m_pIMessageFilter) {
		fPrevNREnable = OleStdMsgFilter_EnableNotRespondingDialog(gOleApp->m_pIMessageFilter, FALSE);
		fPrevBusyEnable = OleStdMsgFilter_EnableBusyDialog(gOleApp->m_pIMessageFilter, FALSE);
	}
#endif

    hrErr = OleGetClipboard((LPDATAOBJECT *)&pDataObj);
    if (hrErr != NOERROR) {		// Clipboard seems to be empty or can't be accessed
    	if (pCanCopy)
    		*pCanCopy = false;
    	if (pCanLink)
    		*pCanLink = false;
        goto done;
    }

	OleDocQueryPasteFromData(pOleDoc, pDataObj, pCanCopy, pCanLink);
	
    if (pDataObj)
        OleStdRelease((LPUNKNOWN)pDataObj);

done:
#if qOleMessageFilter
	if (gOleApp->m_pIMessageFilter) {
		OleStdMsgFilter_EnableNotRespondingDialog(gOleApp->m_pIMessageFilter, fPrevNREnable);
		OleStdMsgFilter_EnableBusyDialog(gOleApp->m_pIMessageFilter, fPrevBusyEnable);
	}
#endif
}

//#pragma segment OleDocumentSeg
void OleDocCopyToDoc(OleDocumentPtr pSrcOleDoc, OleDocumentPtr pDestOleDoc)
{
	pDestOleDoc->m_pSrcDocOfCopy = pSrcOleDoc;
}

//#pragma segment OleDocumentSeg
void OleDocDisableLinkSource(OleDocumentPtr pSrcOleDoc)
{
	OleDocumentPtr pClipboardDoc = gOleApp->m_pClipboardDoc;
	
	if (pClipboardDoc &&
		pClipboardDoc->m_fLinkSourceAvail &&
		pClipboardDoc->m_pSrcDocOfCopy == pSrcOleDoc)
	{
		pClipboardDoc->m_fLinkSourceAvail = false;
		
#if qOleContainerApp		
		pClipboardDoc->container.m_pCopiedEmbeddingSource = nil;
#endif
		
	    /* OLE2NOTE: since we are changing the list of formats on
		**    the clipboard (ie. removing CF_LINKSOURCE), we must
		**    do copy again.
		*/
		OleDocDoCopy(pClipboardDoc);
	}
}

static RgnHandle	rgnBlast = nil;

#if qOleContainerApp && qOleInPlace
//#pragma segment OleDocumentSeg
void OleDocMoveWindow(OleDocumentPtr pOleDoc, short h, short v)
{
	WindowPtr	pWin;
	GrafPtr		oldPort;
	Point		ptCorner;

	if (rgnBlast == nil)
		return;

	if (OleInPlaceContainerDocIsUIVisible(pOleDoc))
	{
		ASSERTCOND(pOleDoc->m_pIDoc->lpVtbl->m_GetWindowProcPtr != nil);
		pWin = (*pOleDoc->m_pIDoc->lpVtbl->m_GetWindowProcPtr)(pOleDoc->m_pIDoc);

		GetPort(&oldPort);
		SetPort(pWin);

		ptCorner.h = ptCorner.v = 0;
		LocalToGlobal(&ptCorner);

		OffsetRgn(rgnBlast,(short)(h - ptCorner.h), (short)(v - ptCorner.v));

		SetPort(oldPort);
	}
}
#endif

//#pragma segment OleDocumentSeg
void OleDocEnableDialog(OleDocumentPtr pOleDoc)
{
#if qOleInPlace
#if qOleServerApp
	if (OleInPlaceServerDocIsInPlace(pOleDoc))
		OleMaskMouse(false);
#elif qOleContainerApp
	if (OleInPlaceContainerDocIsUIVisible(pOleDoc))
	{
		HRESULT		hrErr;

		GetFrontProcess(&pOleDoc->container.inplace.m_ServerPSN);

		hrErr = pOleDoc->container.inplace.m_pActiveObject->lpVtbl->EnableModeless(pOleDoc->container.inplace.m_pActiveObject, false);
		ASSERTNOERROR(hrErr);

		rgnBlast = NewRgn();
		if (rgnBlast != nil)
		{
			OleStdGetInplaceClipRgn(rgnBlast);
			OleStdBlastWindows(rgnBlast);
		}

		ASSERTCOND(gApplication->vtbl->m_ShowProcPtr != nil);
		(*gApplication->vtbl->m_ShowProcPtr)(gApplication);
	}
#endif
#endif
}

//#pragma segment OleDocumentSeg
void OleDocDisableDialog(OleDocumentPtr pOleDoc, Boolean resumeInPlace)
{
#if qOleInPlace
#if qOleServerApp
	if (OleInPlaceServerDocIsInPlace(pOleDoc))
		OleMaskMouse(true);
#elif qOleContainerApp
	if (OleInPlaceContainerDocIsUIVisible(pOleDoc))
	{
		HRESULT		hrErr;

		ASSERTCOND(pOleDoc->container.inplace.m_pActiveObject != nil);
		hrErr = pOleDoc->container.inplace.m_pActiveObject->lpVtbl->EnableModeless(pOleDoc->container.inplace.m_pActiveObject, true);
		ASSERTNOERROR(hrErr);

		if (resumeInPlace)
		{
			WindowPtr	pWin;

			OleStdUnblastWindows(nil);

			// bring the server to the foreground
			SetFrontProcess(&pOleDoc->container.inplace.m_ServerPSN);

			// cycle until we've been switched out and eat the suspend event
			ASSERTCOND(gApplication->vtbl->m_WaitContextSwitchProcPtr != nil);
			(*gApplication->vtbl->m_WaitContextSwitchProcPtr)(gApplication, false, true);
		
			ASSERTCOND(pOleDoc->m_pIDoc->lpVtbl->m_GetWindowProcPtr != nil);
			pWin = (*pOleDoc->m_pIDoc->lpVtbl->m_GetWindowProcPtr)(pOleDoc->m_pIDoc);

			HiliteWindow(pWin, true);
		}
		else
		{
			OleStdUnblastWindows(rgnBlast);

			// if another document is now in the foreground, cause this inplace
			// document to go doc window deactive
			hrErr = pOleDoc->container.inplace.m_pActiveObject->lpVtbl->OnDocWindowActivate(pOleDoc->container.inplace.m_pActiveObject, false);
			ASSERTNOERROR(hrErr);
		}
	}

	if (rgnBlast)
	{		
		DisposeRgn(rgnBlast);
		rgnBlast = nil;
	}
#endif
#endif
}
// OLDNAME: OleOutlineDocInterface.c
extern ApplicationPtr			gApplication;

static IUnknownVtbl				gOleOutlineDocUnknownVtbl;
static IDocVtbl					gOleOutlineDocDocVtbl;

//#pragma segment OleOutlineDocumentInterfaceSeg
void OleOutlineDocInitInterfaces(void)
{
	// OleOutlineDoc::IUnknown method table
	{
		IUnknownVtbl*	p;

		p = &gOleOutlineDocUnknownVtbl;

		p->QueryInterface = 						IOleOutlineDocUnknownQueryInterface;
		p->AddRef = 								IOleOutlineDocUnknownAddRef;
		p->Release = 								IOleOutlineDocUnknownRelease;

		ASSERTCOND(ValidVtbl(((char *)p) + sizeof(p->b), sizeof(*p) - sizeof(p->b)));
	}

	{
		IDocVtbl*		p;
	
		p = &gOleOutlineDocDocVtbl;

		p->m_GetClassIDProcPtr =					IOleOutlineDocGetClassID;

		p->m_GetWindowProcPtr =						IOleOutlineDocGetWindow;
		p->m_GetBoundsProcPtr =						IOleOutlineDocGetBounds;
		p->m_BringToFrontProcPtr =					IOleOutlineDocBringToFront;

		p->m_GetExtentProcPtr =						IOleOutlineDocGetExtent;
		p->m_GetMetafilePictDataProcPtr =			IOleOutlineDocGetMetafilePictData;
		p->m_GetTextDataProcPtr =					IOleOutlineDocGetTextData;
	
		p->m_IsDataTransferDocProcPtr =				IOleOutlineDocIsDataTransferDoc;
		p->m_IsDirtyProcPtr =						IOleOutlineDocIsDirty;
		p->m_IsVisibleProcPtr =						IOleOutlineDocIsVisibile;
		p->m_SetDirtyProcPtr =						IOleOutlineDocSetDirty;
		
		p->m_SetDocTypeProcPtr =					IOleOutlineDocSetDocType;
		p->m_GetDocTypeProcPtr =					IOleOutlineDocGetDocType;
	
		p->m_DocLockAppProcPtr =					IOleOutlineDocDocLockApp;
		p->m_DocUnlockAppProcPtr =					IOleOutlineDocUnlockApp;
	
		p->m_GetFSSpecProcPtr =						IOleOutlineDocGetFSSpec;
	
		p->m_GetCreatorProcPtr =					IOleOutlineDocGetCreator;
		p->m_GetTypeProcPtr =						IOleOutlineDocGetType;
	
		p->m_ShowProcPtr =							IOleOutlineDocShow;
		p->m_HideProcPtr =							IOleOutlineDocHide;
		p->m_SetWindowTitleProcPtr =				IOleOutlineDocSetWindowTitle;
		p->m_SetFSSpecProcPtr =						IOleOutlineDocSetFSSpec;

		p->m_ResizeProcPtr =						IOleOutlineDocResize;
		
		p->m_DoCloseProcPtr =						IOleOutlineDocDoClose;
		p->m_IsClosingProcPtr =						IOleOutlineDocIsClosing;
		
		p->m_LoadFromStgProcPtr =					IOleOutlineDocLoadFromStg;
		p->m_SaveToStgProcPtr =						IOleOutlineDocSaveToStg;
		
		p->m_PasteTextDataProcPtr = 				IOleOutlineDocPasteTextData;
		
		p->m_IsItemRunningProcPtr =					IOleOutlineDocIsItemRunning;
		p->m_GetItemObjectProcPtr =					IOleOutlineDocGetItemObject;
		
#if qOleDragDrop
		p->m_GetClientRectProcPtr =					IOleOutlineDocGetClientRect;
		p->m_ScrollProcPtr =						IOleOutlineDocScroll;
#endif // qOleDragDrop

#if qOleContainerApp
		p->m_GetItemStorageProcPtr = 				IOleOutlineDocGetItemStorage;
		p->m_GetObjectInterfaceProcPtr = 			IOleOutlineDocGetObjectInterface;
		p->m_PasteOutlineDataProcPtr =				IOleOutlineDocPasteOutlineData;
		p->m_PasteOleObjectProcPtr =				IOleOutlineDocPasteOleObject;
		p->m_InformAllOleObjectsDocRenamedProcPtr =	IOleOutlineDocInformAllOleObjectsDocRenamed;

#if qOleInPlace
		p->m_UpdateWindowPositionProcPtr =			IOleOutlineDocUpdateWindowPosition;
		p->m_DoActivateProcPtr =					IOleOutlineDocDoActivate;
		p->m_DoIPDeactivateProcPtr =				IOleOutlineDocDoIPDeactivate;
		p->m_DoUIDeactivateProcPtr =				IOleOutlineDocDoUIDeactivate;
#if qFrameTools
		p->m_FrameToolsEnabledProcPtr =				IOleOutlineDocFrameToolsEnabled;
#endif
#endif
		
#endif // qOleContainerApp

#if qOleServerApp
		p->m_GetItemNameProcPtr =					IOleOutlineDocGetItemName;		
		p->m_SelectItemProcPtr =					IOleOutlineDocSelectItem;		
		p->m_RemovePseudoObjFromItemProcPtr =		IOleOutlineDocRemovePseudoObjFromItem;		
		p->m_GetCopiedRangeProcPtr = 				IOleOutlineDocGetCopiedRange;
		p->m_GetRangeFullMonikerProcPtr = 			IOleOutlineDocGetRangeFullMoniker;
		p->m_GetRangeRelMonikerProcPtr = 			IOleOutlineDocGetRangeRelMoniker;		
		p->m_InformAllPseudoObjectsDocRenamedProcPtr = 	IOleOutlineDocInformAllPseudoObjectsDocRenamed;
		p->m_InformAllPseudoObjectsDocSavedProcPtr = 	IOleOutlineDocInformAllPseudoObjectsDocSaved;
		p->m_CloseAllPseudoObjsProcPtr = 			IOleOutlineDocCloseAllPseudoObjs;
		p->m_PositionNewDocProcPtr =				IOleOutlineDocPositionNewDoc;

#if qOleInPlace
		p->m_PreInPlaceInsertMenusProcPtr =			IOleOutlineDocPreInPlaceInsertMenus;
		p->m_GetInPlaceBeforeMenuIDsProcPtr =		IOleOutlineDocGetInPlaceBeforeMenuIDs;
#if qFrameTools
		p->m_EnableFrameToolsProcPtr =				IOleOutlineDocEnableFrameTools;
#endif
#endif

#if qOleTreatAs
		p->m_DoTreatAsProcPtr = 					IOleOutlineDocDoTreatAs;
#endif // qOleTreatAs

#endif // qOleServerApp

#if qOleInPlace
		p->m_IsIPActiveProcPtr =					IOleOutlineDocIsIPActive;
		p->m_IsUIActiveProcPtr =					IOleOutlineDocIsUIActive;
		p->m_IsUIVisibleProcPtr =					IOleOutlineDocIsUIVisible;
#endif

		ASSERTCOND(ValidVtbl(p, sizeof(*p)));
	}
}

//#pragma segment OleOutlineDocumentInterfaceSeg
void OleOutlineDocIUnknownInit(OutlineDocUnknownImplPtr pOutlineDocUnknownImpl, struct OleOutlineDocRec* pOleOutlineDoc)
{
	pOutlineDocUnknownImpl->lpVtbl				= &gOleOutlineDocUnknownVtbl;
	pOutlineDocUnknownImpl->lpOleOutlineDoc		= pOleOutlineDoc;
	pOutlineDocUnknownImpl->cRef				= 0;
}

//#pragma segment OleOutlineDocumentInterfaceSeg
STDMETHODIMP IOleOutlineDocUnknownQueryInterface(LPUNKNOWN lpThis, REFIID riid, void* * lplpvObj)
{
	HRESULT					hrErr;
	OleOutlineDocPtr		pOleOutlineDoc;
	
	OleDbgEnterInterface();

	pOleOutlineDoc = ((OutlineDocUnknownImplPtr)lpThis)->lpOleOutlineDoc;

	hrErr = OleOutlineDocQueryInterface(pOleOutlineDoc, riid, lplpvObj);

	return hrErr;
}


//#pragma segment OleOutlineDocumentInterfaceSeg
STDMETHODIMP_(unsigned long) IOleOutlineDocUnknownAddRef(LPUNKNOWN lpThis)
{
	OleOutlineDocPtr			pOleOutlineDoc;
	unsigned long						cRef;
	
	OleDbgEnterInterface();

	pOleOutlineDoc = ((OutlineDocUnknownImplPtr)lpThis)->lpOleOutlineDoc;

	cRef = OleOutlineDocAddRef(pOleOutlineDoc);

	return cRef;
}


//#pragma segment OleOutlineDocumentInterfaceSeg
STDMETHODIMP_(unsigned long) IOleOutlineDocUnknownRelease (LPUNKNOWN lpThis)
{
	OleOutlineDocPtr			pOleOutlineDoc;
	unsigned long						cRef;
	
	OleDbgEnterInterface();

	pOleOutlineDoc = ((OutlineDocUnknownImplPtr)lpThis)->lpOleOutlineDoc;

	cRef = OleOutlineDocRelease(pOleOutlineDoc);

	return cRef;
}

//#pragma segment OleOutlineDocumentInterfaceSeg
void OleOutlineDocIDocInit(OutlineDocDocImplPtr pOutlineDocDocImpl, OleOutlineDocPtr pOleOutlineDoc)
{
	pOutlineDocDocImpl->lpVtbl = 			&gOleOutlineDocDocVtbl;
	pOutlineDocDocImpl->lpOleOutlineDoc = 	pOleOutlineDoc;
}

//#pragma segment OleOutlineDocumentInterfaceSeg
HRESULT IOleOutlineDocGetClassID(DocImplPtr lpThis, CLSID* lpclsidReal, CLSID* lpclsidTreatAs)
{
	OleOutlineDocPtr		pOleOutlineDoc;
	
	pOleOutlineDoc = ((OutlineDocDocImplPtr)lpThis)->lpOleOutlineDoc;

#if qOleServerApp && qOleTreatAs
	if (lpclsidTreatAs) {
		if (!IsEqualCLSID(&pOleOutlineDoc->server.m_clsidTreatAs, &CLSID_NULL))
			*lpclsidTreatAs = pOleOutlineDoc->server.m_clsidTreatAs;
		else
			*lpclsidTreatAs = CLSID_Application;
	}
#endif // qOleTreatAs

	if (lpclsidReal)
		*lpclsidReal = CLSID_Application;

	return NOERROR;
}

//#pragma segment OleOutlineDocumentInterfaceSeg
WindowPtr IOleOutlineDocGetWindow(DocImplPtr lpThis)
{
	DocumentPtr		pDoc;
	
	pDoc = (DocumentPtr)((OutlineDocDocImplPtr)lpThis)->lpOleOutlineDoc;
	
	ASSERTCOND(pDoc->vtbl->m_GetWindowProcPtr != nil);
	return (*pDoc->vtbl->m_GetWindowProcPtr)(pDoc);
}

//#pragma segment OleOutlineDocumentInterfaceSeg
void IOleOutlineDocGetBounds(DocImplPtr lpThis, Rect* rBounds)
{
	OutlineDocPtr	pOutlineDoc;
	
	pOutlineDoc = (OutlineDocPtr)((OutlineDocDocImplPtr)lpThis)->lpOleOutlineDoc;
	
	LineListGetExtentRect(pOutlineDoc->m_LineList, nil, rBounds);
}

//#pragma segment OleOutlineDocumentInterfaceSeg
void IOleOutlineDocBringToFront(DocImplPtr lpThis)
{
	DocumentPtr		pDoc;
	
	pDoc = (DocumentPtr)((OutlineDocDocImplPtr)lpThis)->lpOleOutlineDoc;
	
	ASSERTCOND(pDoc->vtbl->m_BringToFrontProcPtr != nil);
	(*pDoc->vtbl->m_BringToFrontProcPtr)(pDoc);
}

//#pragma segment OleOutlineDocumentInterfaceSeg
void IOleOutlineDocGetExtent(DocImplPtr lpThis, void* pItem, LPSIZEL lpsizel)
{
	DocumentPtr		pDoc;
	NamePtr			pName = (NamePtr)pItem;
	LineRangeRec	LineRange;
	
#if qOleInPlace
	{
		ASSERTCOND(lpThis->lpVtbl->m_IsIPActiveProcPtr != nil);
		if ((*lpThis->lpVtbl->m_IsIPActiveProcPtr)(lpThis))
		{
			WindowPtr	pWindow;

			ASSERTCOND(lpThis->lpVtbl->m_GetWindowProcPtr != nil);
			pWindow = (*lpThis->lpVtbl->m_GetWindowProcPtr)(lpThis);

			lpsizel->cx = pWindow->portRect.right - (kRowHeadingsWidth + 15);
			lpsizel->cy = pWindow->portRect.bottom - (kColumnHeadingsHeight + 15);

			return;
		}
	}
#endif

	pDoc = (DocumentPtr)((OutlineDocDocImplPtr)lpThis)->lpOleOutlineDoc;
	if (pName) {
		NameGetSel(pName, &LineRange);	
		LineListGetExtent(((OutlineDocPtr)pDoc)->m_LineList, &LineRange, lpsizel);
	}
	else
		LineListGetExtent(((OutlineDocPtr)pDoc)->m_LineList, nil, lpsizel);

	// If we don't have any lines the extent is {0, 0} so instead
	// we return a default extent.
	if (lpsizel->cx == 0 || lpsizel->cy == 0)
	{
		lpsizel->cx = kDefaultObjectWidth;
		lpsizel->cy = kDefaultObjectHeight;
	}
}

//#pragma segment OleOutlineDocumentInterfaceSeg
Handle IOleOutlineDocGetMetafilePictData(DocImplPtr lpThis, void* pItem)
{
	DocumentPtr		pDoc;
	Rect			rBounds;
	Rect*			pRect = nil;
	NamePtr			pName = (NamePtr)pItem;
	LineRangeRec	LineRange;
	
	pDoc = (DocumentPtr)((OutlineDocDocImplPtr)lpThis)->lpOleOutlineDoc;

#if qOleInPlace
	{
		ASSERTCOND(lpThis->lpVtbl->m_IsIPActiveProcPtr != nil);
		if ((*lpThis->lpVtbl->m_IsIPActiveProcPtr)(lpThis))
		{
			WindowPtr	pWindow;

			ASSERTCOND(lpThis->lpVtbl->m_GetWindowProcPtr != nil);
			pWindow = (*lpThis->lpVtbl->m_GetWindowProcPtr)(lpThis);

			rBounds = pWindow->portRect;
			rBounds.right -= kRowHeadingsWidth + 15;
			rBounds.bottom -= kColumnHeadingsHeight + 15;

			pRect = &rBounds;
		}
	}
#endif

	if (pName) {	
		NameGetSel(pName, &LineRange);
		return (Handle)LineListGetPICT(((OutlineDocPtr)pDoc)->m_LineList, &LineRange, pRect);
	}
	else
		return (Handle)LineListGetPICT(((OutlineDocPtr)pDoc)->m_LineList, nil, pRect);
	
}

//#pragma segment OleOutlineDocumentInterfaceSeg
Handle IOleOutlineDocGetTextData(DocImplPtr lpThis, void* pItem)
{
	DocumentPtr		pDoc;
	NamePtr			pName = (NamePtr)pItem;
	LineRangeRec	LineRange;
	
	pDoc = (DocumentPtr)((OutlineDocDocImplPtr)lpThis)->lpOleOutlineDoc;
	if (pName) {
		NameGetSel(pName, &LineRange);
		return (Handle)LineListGetText(((OutlineDocPtr)pDoc)->m_LineList, &LineRange);
	}
	else
		return (Handle)LineListGetText(((OutlineDocPtr)pDoc)->m_LineList, nil);
}

//#pragma segment OleOutlineDocumentInterfaceSeg
Boolean IOleOutlineDocIsDataTransferDoc(DocImplPtr lpThis)
{
	DocumentPtr		pDoc;
	
	pDoc = (DocumentPtr)((OutlineDocDocImplPtr)lpThis)->lpOleOutlineDoc;
	
	return ((DocumentPtr)pDoc)->m_fDataTransferDoc;
}

//#pragma segment OleOutlineDocumentInterfaceSeg
Boolean IOleOutlineDocIsDirty(DocImplPtr lpThis)
{
	DocumentPtr		pDoc;
	
	pDoc = (DocumentPtr)((OutlineDocDocImplPtr)lpThis)->lpOleOutlineDoc;
	
	ASSERTCOND(pDoc->vtbl->m_IsDirtyProcPtr != nil);
	return (*pDoc->vtbl->m_IsDirtyProcPtr)(pDoc);
}

//#pragma segment OleOutlineDocumentInterfaceSeg
void IOleOutlineDocSetDirty(DocImplPtr lpThis, Boolean fDirty)
{
	DocumentPtr		pDoc;
	
	pDoc = (DocumentPtr)((OutlineDocDocImplPtr)lpThis)->lpOleOutlineDoc;
	
	ASSERTCOND(pDoc->vtbl->m_SetDirtyProcPtr != nil);
	(*pDoc->vtbl->m_SetDirtyProcPtr)(pDoc, fDirty);
}

//#pragma segment OleOutlineDocumentInterfaceSeg
Boolean IOleOutlineDocIsVisibile(DocImplPtr lpThis)
{
	DocumentPtr		pDoc;
	
	pDoc = (DocumentPtr)((OutlineDocDocImplPtr)lpThis)->lpOleOutlineDoc;
	
	ASSERTCOND(pDoc->vtbl->m_IsVisibleProcPtr != nil);
	return (*pDoc->vtbl->m_IsVisibleProcPtr)(pDoc);
}

//#pragma segment OleOutlineDocumentInterfaceSeg
void IOleOutlineDocDocLockApp(DocImplPtr lpThis)
{
	OleOutlineAppDocLock((OleOutlineAppPtr)gApplication);
}

//#pragma segment OleOutlineDocumentInterfaceSeg
void IOleOutlineDocUnlockApp(DocImplPtr lpThis)
{
	OleOutlineAppDocUnlock((OleOutlineAppPtr)gApplication);
}

//#pragma segment OleOutlineDocumentInterfaceSeg
void IOleOutlineDocGetFSSpec(DocImplPtr lpThis, FSSpecPtr pFSSpec)
{
	DocumentPtr		pDoc;
	
	pDoc = (DocumentPtr)((OutlineDocDocImplPtr)lpThis)->lpOleOutlineDoc;
	
	*pFSSpec = pDoc->m_File;
}

//#pragma segment OleOutlineDocumentInterfaceSeg
OSType IOleOutlineDocGetCreator(DocImplPtr lpThis)
{
	DocumentPtr		pDoc;
	
	pDoc = (DocumentPtr)((OutlineDocDocImplPtr)lpThis)->lpOleOutlineDoc;
	
	return pDoc->m_FileCreator;
}

//#pragma segment OleOutlineDocumentInterfaceSeg
OSType IOleOutlineDocGetType(DocImplPtr lpThis)
{
	DocumentPtr		pDoc;
	
	pDoc = (DocumentPtr)((OutlineDocDocImplPtr)lpThis)->lpOleOutlineDoc;
	
	return pDoc->m_FileType;
}

//#pragma segment OleOutlineDocumentInterfaceSeg
void IOleOutlineDocShow(DocImplPtr lpThis)
{
	DocumentPtr		pDoc;
	
	pDoc = (DocumentPtr)((OutlineDocDocImplPtr)lpThis)->lpOleOutlineDoc;
	
	ASSERTCOND(pDoc->vtbl->m_ShowProcPtr != nil);
	(*pDoc->vtbl->m_ShowProcPtr)(pDoc);
}

//#pragma segment OleOutlineDocumentInterfaceSeg
void IOleOutlineDocHide(DocImplPtr lpThis)
{
	DocumentPtr		pDoc;
	
	pDoc = (DocumentPtr)((OutlineDocDocImplPtr)lpThis)->lpOleOutlineDoc;
	
	ASSERTCOND(pDoc->vtbl->m_HideProcPtr != nil);
	(*pDoc->vtbl->m_HideProcPtr)(pDoc);
}

//#pragma segment OleOutlineDocumentInterfaceSeg
void IOleOutlineDocSetDocType(DocImplPtr lpThis, OleDocumentType docType)
{
	DocumentPtr		pDoc;
	
	pDoc = (DocumentPtr)((OutlineDocDocImplPtr)lpThis)->lpOleOutlineDoc;
	
	ASSERTCOND(pDoc->m_DocType == kUnknownDocumentType);
	
	pDoc->m_DocType = docType;
}

//#pragma segment OleOutlineDocumentInterfaceSeg
OleDocumentType IOleOutlineDocGetDocType(DocImplPtr lpThis)
{
	DocumentPtr		pDoc;
	
	pDoc = (DocumentPtr)((OutlineDocDocImplPtr)lpThis)->lpOleOutlineDoc;
	
	return pDoc->m_DocType;
}

//#pragma segment OleOutlineDocumentInterfaceSeg
void IOleOutlineDocSetWindowTitle(DocImplPtr lpThis, StringPtr pTitle)
{
	DocumentPtr		pDoc;
	
	pDoc = (DocumentPtr)((OutlineDocDocImplPtr)lpThis)->lpOleOutlineDoc;
	
	ASSERTCOND(pDoc->vtbl->m_SetWindowTitleProcPtr != nil);
	(*pDoc->vtbl->m_SetWindowTitleProcPtr)(pDoc, pTitle);
}

//#pragma segment OleOutlineDocumentInterfaceSeg
void IOleOutlineDocSetFSSpec(DocImplPtr lpThis, FSSpecPtr pSpec)
{
	DocumentPtr		pDoc;

	pDoc = (DocumentPtr)((OutlineDocDocImplPtr)lpThis)->lpOleOutlineDoc;
	
	ASSERTCOND(pDoc->vtbl->m_SetFSSpecProcPtr != nil);
	(*pDoc->vtbl->m_SetFSSpecProcPtr)(pDoc, pSpec);

	OleDocSetFileMonikerFSSpec(&((OleOutlineDocPtr)pDoc)->m_OleDoc, pSpec);
}

//#pragma segment OleOutlineDocumentInterfaceSeg
void IOleOutlineDocResize(DocImplPtr lpThis, Rect* rOldBounds, short wWidth, short wHeight)
{
	OutlineDocPtr	pOutlineDoc;
	
	pOutlineDoc = (OutlineDocPtr)((OutlineDocDocImplPtr)lpThis)->lpOleOutlineDoc;
	
	OutlineDocResize(pOutlineDoc, rOldBounds, wWidth, wHeight);
}

//#pragma segment OleOutlineDocumentInterfaceSeg
YNCResult IOleOutlineDocDoClose(DocImplPtr lpThis, Boolean askUserToSave, YNCResult defaultAnswer, Boolean quitting)
{
	DocumentPtr		pDoc;
	
	pDoc = (DocumentPtr)((OutlineDocDocImplPtr)lpThis)->lpOleOutlineDoc;
	
	ASSERTCOND(pDoc->vtbl->m_DoCloseProcPtr != nil);
	return (*pDoc->vtbl->m_DoCloseProcPtr)(pDoc, askUserToSave, defaultAnswer, quitting);
}

//#pragma segment OleOutlineDocumentInterfaceSeg
Boolean IOleOutlineDocIsClosing(DocImplPtr lpThis)
{
	DocumentPtr		pDoc;
	
	pDoc = (DocumentPtr)((OutlineDocDocImplPtr)lpThis)->lpOleOutlineDoc;

	ASSERTCOND(pDoc->vtbl->m_IsClosingProcPtr != nil);
	return (*pDoc->vtbl->m_IsClosingProcPtr)(pDoc);
}

//#pragma segment OleOutlineDocumentInterfaceSeg
HRESULT IOleOutlineDocLoadFromStg(DocImplPtr lpThis, LPSTORAGE lpStg)
{
	DocumentPtr		pDoc;
	
	pDoc = (DocumentPtr)((OutlineDocDocImplPtr)lpThis)->lpOleOutlineDoc;
	
	return OleOutlineDocLoadFromStorage((OleOutlineDocPtr)pDoc, lpStg);
}

//#pragma segment OleOutlineDocumentInterfaceSeg
HRESULT IOleOutlineDocSaveToStg(DocImplPtr lpThis, unsigned int uFormat, LPSTORAGE lpStg, Boolean fRemember)
{
	DocumentPtr		pDoc;
	
	pDoc = (DocumentPtr)((OutlineDocDocImplPtr)lpThis)->lpOleOutlineDoc;
	if (!uFormat)
		uFormat = ((OleOutlineDocPtr)pDoc)->m_cfSaveFormat;

	return OleOutlineDocSaveSelectionToStorage((OleOutlineDocPtr)pDoc, nil, uFormat, lpStg, fRemember);
}

//#pragma segment OleOutlineDocumentInterfaceSeg
void IOleOutlineDocPasteTextData(DocImplPtr lpThis, Handle hData)
{	
	DocumentPtr		pDoc;
	
	pDoc = (DocumentPtr)((OutlineDocDocImplPtr)lpThis)->lpOleOutlineDoc;
	
	OutlineDocPasteTextData((OutlineDocPtr)pDoc, hData);
}

//#pragma segment OleOutlineDocumentInterfaceSeg
HRESULT IOleOutlineDocGetItemObject(DocImplPtr lpThis, char* pszItem, unsigned long SpeedNeeded, LPBINDCTX pbc, REFIID riid, void* * ppvObject)
{
	OleOutlineDocPtr pOleOutlineDoc;
	
	pOleOutlineDoc = ((OutlineDocDocImplPtr)lpThis)->lpOleOutlineDoc;
		
#if qOleServerApp
	return OleOutlineServerDocGetItemObject(pOleOutlineDoc, pszItem, SpeedNeeded, pbc, riid, ppvObject);
#elif qOleContainerApp	
	return OleOutlineContainerDocGetItemObject(pOleOutlineDoc, pszItem, SpeedNeeded, pbc, riid, ppvObject);
#endif
}

//#pragma segment OleOutlineDocumentInterfaceSeg
HRESULT IOleOutlineDocIsItemRunning(DocImplPtr lpThis, char* pszItem)
{
	OleOutlineDocPtr pOleOutlineDoc;
	
	pOleOutlineDoc = ((OutlineDocDocImplPtr)lpThis)->lpOleOutlineDoc;
	
#if qOleServerApp
	return OleOutlineServerDocIsItemRunning(pOleOutlineDoc, pszItem);
#elif qOleContainerApp	
	return OleOutlineContainerDocIsItemRunning(pOleOutlineDoc, pszItem);
#endif
}

#if qOleDragDrop

void IOleOutlineDocGetClientRect(struct DocImpl* lpThis, Rect* rClient)
{
	OleOutlineDocPtr pOleOutlineDoc;
	
	pOleOutlineDoc = ((OutlineDocDocImplPtr)lpThis)->lpOleOutlineDoc;

	LineListGetView(((OutlineDocPtr)pOleOutlineDoc)->m_LineList, rClient);		
}

void IOleOutlineDocScroll(struct DocImpl* lpThis, SCROLLDIR scrolldir)
{
	OleOutlineDocPtr pOleOutlineDoc;
	
	pOleOutlineDoc = ((OutlineDocDocImplPtr)lpThis)->lpOleOutlineDoc;

	LineListScroll(((OutlineDocPtr)pOleOutlineDoc)->m_LineList, scrolldir);		
}

#endif // qOleDragDrop

#if qOleContainerApp

//#pragma segment OleOutlineDocumentInterfaceSeg
HRESULT IOleOutlineDocGetItemStorage(DocImplPtr lpThis, char* pszItem, void** lplpvStorage)
{
	OleOutlineDocPtr pOleOutlineDoc;
	
	pOleOutlineDoc = ((OutlineDocDocImplPtr)lpThis)->lpOleOutlineDoc;
	
	return OleOutlineContainerDocGetItemStorage(pOleOutlineDoc, pszItem, lplpvStorage);
}

//#pragma segment OleOutlineDocumentInterfaceSeg
void IOleOutlineDocPasteOutlineData(DocImplPtr lpThis, LPSTORAGE pStorage)
{
	OleOutlineDocPtr pOleOutlineDoc;
	
	pOleOutlineDoc = ((OutlineDocDocImplPtr)lpThis)->lpOleOutlineDoc;
	
	OleOutlineContainerDocPasteOutlineData(pOleOutlineDoc, pStorage);
}

//#pragma segment OleOutlineDocumentInterfaceSeg
void IOleOutlineDocPasteOleObject(DocImplPtr lpThis, LPDATAOBJECT pSrcDataObj, unsigned long dwCreateType, ResType cfFormat, Boolean fDisplayAsIcon, PicHandle hMetaPict)
{
	OleOutlineDocPtr pOleOutlineDoc;
	
	pOleOutlineDoc = ((OutlineDocDocImplPtr)lpThis)->lpOleOutlineDoc;
	
	OleOutlineContainerDocPasteOleObject(pOleOutlineDoc, pSrcDataObj, dwCreateType, cfFormat, fDisplayAsIcon, hMetaPict);
}

/* IOleOutlineDocGetObjectInterface
** --------------------------------
**    Get the desired OLE interface from the item
**
**    Returns nil if there is no interface available in the item.
*/
//#pragma segment OleOutlineDocumentInterfaceSeg
LPUNKNOWN IOleOutlineDocGetObjectInterface(DocImplPtr lpThis, void* pItem, REFIID riid)
{
	LinePtr pLine = (LinePtr)pItem;
	
	ASSERTCOND(pLine->m_LineType == kContainerLineType);
		
    return OleContainerLineGetOleObject((OleContainerLinePtr)pLine, riid);
}

//#pragma segment OleOutlineDocumentInterfaceSeg
void IOleOutlineDocInformAllOleObjectsDocRenamed(DocImplPtr lpThis, LPMONIKER pmkDoc)
{
	OleOutlineDocPtr pOleOutlineDoc;
	
	pOleOutlineDoc = ((OutlineDocDocImplPtr)lpThis)->lpOleOutlineDoc;
	
	OleOutlineContainerDocInformAllOleObjectsDocRenamed(pOleOutlineDoc, pmkDoc);
}

#if qOleInPlace

//#pragma segment OleOutlineDocumentInterfaceSeg
void IOleOutlineDocUpdateWindowPosition(DocImplPtr lpThis)
{
	OleOutlineDocPtr pOleOutlineDoc;
	
	pOleOutlineDoc = ((OutlineDocDocImplPtr)lpThis)->lpOleOutlineDoc;

	OleOutlineContainerDocUpdateWindowPosition(pOleOutlineDoc);
}

//#pragma segment OleOutlineDocumentInterfaceSeg
void IOleOutlineDocDoActivate(DocImplPtr lpThis, Boolean becomingActive)
{
	DocumentPtr		pDocument;
	
	pDocument = (DocumentPtr)((OutlineDocDocImplPtr)lpThis)->lpOleOutlineDoc;
	
	if (pDocument->m_Active != becomingActive)
	{
		ASSERTCOND(pDocument->vtbl->m_DoActivateProcPtr != nil);
		(*pDocument->vtbl->m_DoActivateProcPtr)(pDocument, becomingActive);
	}
}

//#pragma segment OleOutlineDocumentInterfaceSeg
void IOleOutlineDocDoIPDeactivate(DocImplPtr lpThis)
{
	OleOutlineDocPtr pOleOutlineDoc;
	
	pOleOutlineDoc = ((OutlineDocDocImplPtr)lpThis)->lpOleOutlineDoc;

	OleOutlineContainerDocDoIPDeactivate(pOleOutlineDoc);
}

//#pragma segment OleOutlineDocumentInterfaceSeg
void IOleOutlineDocDoUIDeactivate(DocImplPtr lpThis)
{
	OleOutlineDocPtr pOleOutlineDoc;
	
	pOleOutlineDoc = ((OutlineDocDocImplPtr)lpThis)->lpOleOutlineDoc;

	OleOutlineContainerDocDoUIDeactivate(pOleOutlineDoc);
}

#if qFrameTools
//#pragma segment OleOutlineDocumentInterfaceSeg
Boolean IOleOutlineDocFrameToolsEnabled(DocImplPtr lpThis)
{
	OleOutlineDocPtr pOleOutlineDoc;
	
	pOleOutlineDoc = ((OutlineDocDocImplPtr)lpThis)->lpOleOutlineDoc;

	return OleOutlineContainerDocFrameToolsEnabled(pOleOutlineDoc);
}
#endif

#endif

#endif // qOleContainerApp

#if qOleServerApp
//#pragma segment OleOutlineDocumentInterfaceSeg
char* IOleOutlineDocGetItemName(DocImplPtr lpThis, void* pItem)
{
	NamePtr	pName = (NamePtr)pItem;
	
	return NameGetText(pName);
}

//#pragma segment OleOutlineDocumentInterfaceSeg
void IOleOutlineDocSelectItem(DocImplPtr lpThis, void* pItem)
{
	OleOutlineDocPtr 	pOleOutlineDoc;
	OutlineDocPtr	pOutlineDoc;
	
	pOleOutlineDoc = ((OutlineDocDocImplPtr)lpThis)->lpOleOutlineDoc;
	pOutlineDoc = (OutlineDocPtr)pOleOutlineDoc;
	
	NameTableGotoName(pOutlineDoc->m_NameTable, (NamePtr)pItem);
}

//#pragma segment OleOutlineDocumentInterfaceSeg
void IOleOutlineDocRemovePseudoObjFromItem(DocImplPtr lpThis, void* pItem)
{
	NamePtr	pName;
	
	pName = (NamePtr)pItem;
	pName->m_pPseudoObj = nil;
}

//#pragma segment OleOutlineDocumentInterfaceSeg
void* IOleOutlineDocGetCopiedRange(DocImplPtr lpThis)
{
	OleOutlineDocPtr 	pOleOutlineDoc;
	OutlineDocPtr		pOutlineDoc;
	LineListPtr			pLineList;	
	
	pOleOutlineDoc = ((OutlineDocDocImplPtr)lpThis)->lpOleOutlineDoc;
	pOutlineDoc = (OutlineDocPtr)pOleOutlineDoc;
	pLineList = pOutlineDoc->m_LineList;
	
	return LineListGetCopiedRange(pLineList);
}
	
//#pragma segment OleOutlineDocumentInterfaceSeg
LPMONIKER IOleOutlineDocGetRangeFullMoniker(DocImplPtr lpThis, void* pRange, unsigned long dwAssign)
{
	OleOutlineDocPtr 	pOleOutlineDoc;
	
	pOleOutlineDoc = ((OutlineDocDocImplPtr)lpThis)->lpOleOutlineDoc;

	return OleOutlineServerDocGetRangeFullMoniker(pOleOutlineDoc, pRange, dwAssign);
}

//#pragma segment OleOutlineDocumentInterfaceSeg
LPMONIKER IOleOutlineDocGetRangeRelMoniker(DocImplPtr lpThis, void* pRange, unsigned long dwAssign)
{
	OleOutlineDocPtr 	pOleOutlineDoc;
	
	pOleOutlineDoc = ((OutlineDocDocImplPtr)lpThis)->lpOleOutlineDoc;

	return OleOutlineServerDocGetRangeRelMoniker(pOleOutlineDoc, pRange, dwAssign);
}

//#pragma segment OleOutlineDocumentInterfaceSeg
void IOleOutlineDocInformAllPseudoObjectsDocRenamed(DocImplPtr lpThis, LPMONIKER pmkDoc)
{
	OleOutlineDocPtr 	pOleOutlineDoc;
	OutlineDocPtr		pOutlineDoc;
	
	pOleOutlineDoc = ((OutlineDocDocImplPtr)lpThis)->lpOleOutlineDoc;
	pOutlineDoc = (OutlineDocPtr)pOleOutlineDoc;

	NameTableInformAllPseudoObjectsDocRenamed(pOutlineDoc->m_NameTable, pmkDoc);
}	

//#pragma segment OleOutlineDocumentInterfaceSeg
void IOleOutlineDocInformAllPseudoObjectsDocSaved(DocImplPtr lpThis, LPMONIKER pmkDoc)
{
	OleOutlineDocPtr 	pOleOutlineDoc;
	OutlineDocPtr		pOutlineDoc;
	
	pOleOutlineDoc = ((OutlineDocDocImplPtr)lpThis)->lpOleOutlineDoc;
	pOutlineDoc = (OutlineDocPtr)pOleOutlineDoc;

	NameTableInformAllPseudoObjectsDocSaved(pOutlineDoc->m_NameTable, pmkDoc);
}	

//#pragma segment OleOutlineDocumentInterfaceSeg
void IOleOutlineDocCloseAllPseudoObjs(DocImplPtr lpThis)
{
	OutlineDocPtr		pOutlineDoc;
	
	pOutlineDoc = (OutlineDocPtr)((OutlineDocDocImplPtr)lpThis)->lpOleOutlineDoc;

	NameTableCloseAllPseudoObjs(pOutlineDoc->m_NameTable);
}

//#pragma segment OleOutlineDocumentInterfaceSeg
void IOleOutlineDocPositionNewDoc(DocImplPtr lpThis)
{
	DocumentPtr		pDoc;
	
	pDoc = (DocumentPtr)((OutlineDocDocImplPtr)lpThis)->lpOleOutlineDoc;

	ASSERTCOND(gApplication->vtbl->m_PositionNewDocProcPtr != nil);
	(*gApplication->vtbl->m_PositionNewDocProcPtr)(gApplication, pDoc);
}

#if qOleInPlace

//#pragma segment OleOutlineDocumentInterfaceSeg
void IOleOutlineDocPreInPlaceInsertMenus(DocImplPtr lpThis)
{
	DeleteMenu(kFile_MENU);
	DeleteMenu(kWindows_MENU);
}

//#pragma segment OleOutlineDocumentInterfaceSeg
void IOleOutlineDocGetInPlaceBeforeMenuIDs(DocImplPtr lpThis, short* beforeID1, short* beforeID3, short* beforeID5)
{
	*beforeID1 = kEdit_MENU;
	*beforeID3 = 0;
	*beforeID5 = 0;
}

#if qFrameTools
//#pragma segment OleOutlineDocumentInterfaceSeg
void IOleOutlineDocEnableFrameTools(DocImplPtr lpThis, Boolean fEnable, Boolean fSetBorder)
{
	DocumentPtr		pDocument;

	pDocument = (DocumentPtr)((OutlineDocDocImplPtr)lpThis)->lpOleOutlineDoc;

	ASSERTCOND(pDocument->vtbl->m_EnableFrameToolsProcPtr != nil);
	(*pDocument->vtbl->m_EnableFrameToolsProcPtr)(pDocument, fEnable, fSetBorder);
}
#endif

#endif // qOleInPlace

#if qOleTreatAs

//#pragma segment OleOutlineDocumentInterfaceSeg
void IOleOutlineDocDoTreatAs(DocImplPtr lpThis, LPSTORAGE pStg)
{
	OleOutlineDocPtr 	pOleOutlineDoc;
	
	pOleOutlineDoc = ((OutlineDocDocImplPtr)lpThis)->lpOleOutlineDoc;

	OleOutlineDocDoTreatAs(pOleOutlineDoc, pStg);
}

#endif // qOleTreatAs

#endif // qOleServerApp

#if qOleInPlace

//#pragma segment OleOutlineDocumentInterfaceSeg
Boolean IOleOutlineDocIsIPActive(DocImplPtr lpThis)
{
#if qOleServerApp
	OleDocumentPtr	pOleDoc;

	pOleDoc = &(((OutlineDocDocImplPtr)lpThis)->lpOleOutlineDoc)->m_OleDoc;

	return OleInPlaceServerDocIsInPlace(pOleDoc);

#elif qOleContainerApp

	OleOutlineDocPtr pOleOutlineDoc;
	
	pOleOutlineDoc = ((OutlineDocDocImplPtr)lpThis)->lpOleOutlineDoc;

	return OleOutlineContainerDocIsIPActive(pOleOutlineDoc);
#endif
}

//#pragma segment OleOutlineDocumentInterfaceSeg
Boolean IOleOutlineDocIsUIActive(DocImplPtr lpThis)
{
#if qOleServerApp
	OleDocumentPtr	pOleDoc;

	pOleDoc = &(((OutlineDocDocImplPtr)lpThis)->lpOleOutlineDoc)->m_OleDoc;

	return OleInPlaceServerDocIsUIActive(pOleDoc);

#elif qOleContainerApp

	OleOutlineDocPtr pOleOutlineDoc;
	
	pOleOutlineDoc = ((OutlineDocDocImplPtr)lpThis)->lpOleOutlineDoc;

	return OleOutlineContainerDocIsUIActive(pOleOutlineDoc);
#endif
}

//#pragma segment OleOutlineDocumentInterfaceSeg
Boolean IOleOutlineDocIsUIVisible(DocImplPtr lpThis)
{
#if qOleServerApp
	OleDocumentPtr	pOleDoc;

	pOleDoc = &(((OutlineDocDocImplPtr)lpThis)->lpOleOutlineDoc)->m_OleDoc;

	return OleInPlaceServerDocIsUIVisible(pOleDoc);

#elif qOleContainerApp

	OleOutlineDocPtr pOleOutlineDoc;
	
	pOleOutlineDoc = ((OutlineDocDocImplPtr)lpThis)->lpOleOutlineDoc;

	return OleOutlineContainerDocIsUIVisible(pOleOutlineDoc);
#endif
}

#endif

//  OLDNAME: OleOutlineDocument.c
extern ApplicationPtr			gApplication;

static OleOutlineDocVtblPtr		gOleOutlineDocVtbl = nil;

//#pragma segment VtblInitSeg
OleOutlineDocVtblPtr OleOutlineDocGetVtbl(void)
{
	ASSERTCOND(gOleOutlineDocVtbl != nil);
	return gOleOutlineDocVtbl;
}

//#pragma segment VtblInitSeg
void OleOutlineDocInitVtbl(void)
{
	OleOutlineDocVtblPtr	vtbl;
	
	vtbl = gOleOutlineDocVtbl = (OleOutlineDocVtblPtr)NewPtrClear(sizeof(*vtbl));
	ASSERTCOND(vtbl != nil);
	FailNIL(vtbl);

	InheritFromVtbl(vtbl, OutlineDocGetVtbl());

	vtbl->m_DisposeProcPtr =			OleOutlineDocDispose;
	vtbl->m_FreeProcPtr =				OleOutlineDocFree;

	vtbl->m_DoNewProcPtr =				OleOutlineDocDoNew;
	vtbl->m_OpenFileProcPtr =			OleOutlineDocOpenFile;
	vtbl->m_CloseFileProcPtr =			OleOutlineDocCloseFile;
	vtbl->m_WriteToFileProcPtr =		OleOutlineDocWriteToFile;
	vtbl->m_ReadFromFileProcPtr =		OleOutlineDocReadFromFile;
	
	vtbl->m_NeedsSaveProcPtr =			OleOutlineDocNeedsSave;
	vtbl->m_DoCloseProcPtr =			OleOutlineDocDoClose;
	vtbl->m_DoSaveProcPtr =				OleOutlineDocDoSave;
	vtbl->m_DoSaveAsProcPtr = 			OleOutlineDocDoSaveAs;
	vtbl->m_DoOpenProcPtr =				OleOutlineDocDoOpen;
	vtbl->m_PresentSaveDialogProcPtr =	OleOutlineDocPresentSaveDialog;

	vtbl->m_CanPasteProcPtr = 			OleOutlineDocCanPaste;
	vtbl->m_DoCopyProcPtr =				OleOutlineDocDoCopy;
	vtbl->m_DoClearProcPtr = 			OleOutlineDocDoClear;
	vtbl->m_DoPasteProcPtr =			OleOutlineDocDoPaste;

	vtbl->m_SetFSSpecProcPtr =			OleOutlineDocSetFSSpec;
	vtbl->m_ShowProcPtr =				OleOutlineDocShow;
	vtbl->m_HideProcPtr =				OleOutlineDocHide;

	vtbl->m_DoKeyDownProcPtr =			OleOutlineDocDoKeyDown;
	
	vtbl->m_SetDirtyProcPtr =			OleOutlineDocSetDirty;

#if qOleServerApp && qOleInPlace
	vtbl->m_SetDefaultCursorProcPtr =	OleOutlineDocSetDefaultCursor;
#endif

	vtbl->m_DoIdleProcPtr =				OleOutlineDocDoIdle;
	vtbl->m_DoActivateProcPtr =			OleOutlineDocDoActivate;
	vtbl->m_DoContentProcPtr =			OleOutlineDocDoContent;

#if qOleInPlace
	vtbl->m_DoDragProcPtr =				OleOutlineDocDoDrag;
	vtbl->m_DoGrowWindowProcPtr =		OleOutlineDocDoGrowWindow;
	vtbl->m_DoZoomWindowProcPtr =		OleOutlineDocDoZoomWindow;
	vtbl->m_MoveWindowProcPtr =			OleOutlineDocMoveWindow;
	vtbl->m_ScrollRectProcPtr =			OleOutlineDocScrollRect;
#endif

	ASSERTCOND(ValidVtbl(vtbl, sizeof(*vtbl)));
}

//#pragma segment VtblDisposeSeg
void OleOutlineDocDisposeVtbl(void)
{
	ASSERTCOND(gOleOutlineDocVtbl != nil);
	DisposePtr((Ptr)gOleOutlineDocVtbl);
}

//#pragma segment OleOutlineDocumentSeg
void OleOutlineDocInit(OleOutlineDocPtr pOleOutlineDoc, short resID, OSType fileType, Boolean fDataTransferDoc)
{
	OutlineDocInit(&pOleOutlineDoc->superClass, resID, fileType, fDataTransferDoc);

	OleOutlineDocIUnknownInit(&pOleOutlineDoc->m_IUnknown, pOleOutlineDoc);
	OleOutlineDocIDocInit(&pOleOutlineDoc->m_IDoc, pOleOutlineDoc);

	OleDocInit(&pOleOutlineDoc->m_OleDoc, (DocImplPtr)&pOleOutlineDoc->m_IDoc, (IUnknown*)&pOleOutlineDoc->m_IUnknown);

#if qOleContainerApp
	OleOutlineContainerDocInit(pOleOutlineDoc);
#elif qOleServerApp
	OleOutlineServerDocInit(pOleOutlineDoc);
#endif
	
	pOleOutlineDoc->vtbl = ((DocumentPtr)pOleOutlineDoc)->vtbl;
	((DocumentPtr)pOleOutlineDoc)->vtbl = OleOutlineDocGetVtbl();

	pOleOutlineDoc->m_GopherUpdateMenus = GopherNewUpdateMenus((UpdateMenusProcPtr)OleOutlineDocUpdateMenus, pOleOutlineDoc);
	FailNIL(pOleOutlineDoc->m_GopherUpdateMenus);

	pOleOutlineDoc->m_GopherDoCmd = GopherNewDoCmd((DoCmdProcPtr)OleOutlineDocDoCmd, pOleOutlineDoc);
	FailNIL(pOleOutlineDoc->m_GopherDoCmd);

	pOleOutlineDoc->m_cfSaveFormat = fileType;
	
	pOleOutlineDoc->m_cRef			= 0;
	
	pOleOutlineDoc->m_fLinkSourceEnabled = false;
}

//#pragma segment OleOutlineDocumentSeg
void OleOutlineDocDispose(DocumentPtr pDoc)
{
	OleOutlineDocPtr	pOleOutlineDoc;
	
	pOleOutlineDoc = (OleOutlineDocPtr)pDoc;
	
#if qOleContainerApp
	OleOutlineContainerDocDispose(pOleOutlineDoc);
#endif
	OleDocDispose(&pOleOutlineDoc->m_OleDoc);
	
	GopherDispose(pOleOutlineDoc->m_GopherUpdateMenus);
	GopherDispose(pOleOutlineDoc->m_GopherDoCmd);

	ASSERTCOND(pOleOutlineDoc->vtbl->m_DisposeProcPtr != nil);
	(*pOleOutlineDoc->vtbl->m_DisposeProcPtr)(pDoc);
}

//#pragma segment OleOutlineDocumentSeg
void OleOutlineDocFree(DocumentPtr pDoc)
{
	// calling Free has no effect on a component object. Instead
	// we wait until the reference count drops to zero, at which
	// point we are disposed automatically.
}

/* OleOutlineDocDoNew
 * ------------------
 *
 *  Initialize the document to be a new (Untitled) document.
 *  This function sets the docInitType to DOCTYPE_NEW.
 *
 *  OLE2NOTE: if this is a visible user document then generate a unique
 *  untitled name that we can use to register in the RunningObjectTable.
 *  We need a unique name so that clients can link to data in this document
 *  even when the document is in the un-saved (untitled) state. it would be
 *  ambiguous to register two documents titled "Untitled 1" in the ROT. we
 *  thus generate the lowest numbered document that is not already
 *  registered in the ROT.
 */
//#pragma segment OleOutlineDocumentSeg
void OleOutlineDocDoNew(DocumentPtr pDoc)
{
	OleDocumentPtr pOleDoc;

	ASSERTCOND(pDoc);
	ASSERTCOND(pDoc->m_DocType == kUnknownDocumentType);
	
	pOleDoc = &((OleOutlineDocPtr)pDoc)->m_OleDoc;
	
#if qOleContainerApp
	OleOutlineContainerDocDoNew((OleOutlineDocPtr)pDoc);
#endif

	pDoc->m_DocType = kNewDocumentType;

    if (! pDoc->m_fDataTransferDoc)
    {
        pOleDoc->m_fLinkSourceAvail = true;

        CreateFileMonikerFSp(
        		&pDoc->m_File,
                &pOleDoc->m_lpFileMoniker
        );

		OleDocAddRef(pOleDoc);			// temporary addref

        OLEDBG_BEGIN3("OleStdRegisterAsRunning called\r\n")
        OleStdRegisterAsRunning(
                (LPUNKNOWN)&pOleDoc->m_PersistFile,
                (LPMONIKER)pOleDoc->m_lpFileMoniker,
                &pOleDoc->m_dwRegROT
        );
        OLEDBG_END3

        OleDocRelease(pOleDoc);
    }
}

//#pragma segment OleOutlineDocumentSeg
void OleOutlineDocOpenFile(DocumentPtr pDoc, Boolean readOnly, Boolean newFile)
{
	OleOutlineDocPtr	pOleOutlineDoc;
	
	pOleOutlineDoc = (OleOutlineDocPtr)pDoc;

	OleDocOpenFile(&pOleOutlineDoc->m_OleDoc, readOnly, newFile);
}

//#pragma segment OleOutlineDocumentSeg
void OleOutlineDocCloseFile(DocumentPtr pDoc)
{
	OleOutlineDocPtr	pOleOutlineDoc;
	
	pOleOutlineDoc = (OleOutlineDocPtr)pDoc;

	OleDocCloseFile(&pOleOutlineDoc->m_OleDoc);
}

//#pragma segment OleOutlineDocumentSeg
void OleOutlineDocWriteToFile(DocumentPtr pDoc)
{
	OleOutlineDocPtr	pOleOutlineDoc;
	
	pOleOutlineDoc = (OleOutlineDocPtr)pDoc;

	OleDocWriteToFile(&pOleOutlineDoc->m_OleDoc);
}

//#pragma segment OleOutlineDocumentSeg
void OleOutlineDocReadFromFile(DocumentPtr pDoc)
{
	OleOutlineDocPtr	pOleOutlineDoc;
	LPSTORAGE			srcStg;
	HRESULT				hrErr;
	
	pOleOutlineDoc = (OleOutlineDocPtr)pDoc;

	srcStg = OleDocGetStorage(&pOleOutlineDoc->m_OleDoc);
	ASSERTCOND(srcStg != nil);
	
	hrErr = OleOutlineDocLoadFromStorage(pOleOutlineDoc, srcStg);
	ASSERTNOERROR(hrErr);
	FailOleErr(hrErr);
	
#if qOleContainerApp
	OleOutlineContainerDocUpdateLinksDlg(pOleOutlineDoc);
#endif	
}

//#pragma segment OleOutlineDocumentSeg
void OleOutlineDocShow(DocumentPtr pDoc)
{
	OleOutlineDocPtr	pOleOutlineDoc;
	
	pOleOutlineDoc = (OleOutlineDocPtr)pDoc;

#if qOleServerApp && qOleInPlace
	if (!OleInPlaceServerDocIsInPlace(&pOleOutlineDoc->m_OleDoc))
#endif
	{
		if (!pDoc->m_fPositioned)
		{
			ASSERTCOND(gApplication->vtbl->m_PositionNewDocProcPtr != nil);
			(*gApplication->vtbl->m_PositionNewDocProcPtr)(gApplication, pDoc);

			pDoc->m_fPositioned = true;

#if qFrameTools
			ASSERTCOND(pDoc->vtbl->m_EnableFrameToolsProcPtr != nil);
			(*pDoc->vtbl->m_EnableFrameToolsProcPtr)(pDoc, true, true);
#endif
		}
	}

	// make sure we set things up before actually showing the window
	OleDocShow(&pOleOutlineDoc->m_OleDoc);

	ASSERTCOND(pOleOutlineDoc->vtbl->m_ShowProcPtr != nil);
	(*pOleOutlineDoc->vtbl->m_ShowProcPtr)(pDoc);
}

//#pragma segment OleOutlineDocumentSeg
void OleOutlineDocHide(DocumentPtr pDoc)
{
	OleOutlineDocPtr	pOleOutlineDoc;
	
	pOleOutlineDoc = (OleOutlineDocPtr)pDoc;

#if qOleContainerApp && qOleInPlace
	if (!pDoc->m_fIsClosing || !OleOutlineContainerDocIsIPActive(pOleOutlineDoc))
#endif
	{
		ASSERTCOND(pOleOutlineDoc->vtbl->m_HideProcPtr != nil);
		(*pOleOutlineDoc->vtbl->m_HideProcPtr)(pDoc);
	}

	OleDocHide(&pOleOutlineDoc->m_OleDoc);
}

//#pragma segment OleOutlineDocumentSeg
void OleOutlineDocDoKeyDown(DocumentPtr pDoc, EventRecord* theEvent)
{
#if qOleServerApp && qOleInPlace
	OleOutlineDocPtr	pOleOutlineDoc;
	
	pOleOutlineDoc = (OleOutlineDocPtr)pDoc;

	OleInPlaceServerDocDoKeyDown(&pOleOutlineDoc->m_OleDoc, theEvent);
#endif
}

//#pragma segment OleOutlineDocumentSeg
YNCResult OleOutlineDocNeedsSave(DocumentPtr pDoc, Boolean askUserToSave, YNCResult defaultAnswer, Boolean quitting)
{
	OleOutlineDocPtr	pOleOutlineDoc;
	
	pOleOutlineDoc = (OleOutlineDocPtr)pDoc;
	
	if (pDoc->m_DocType == kEmbeddedDocumentType)
		askUserToSave = false;

	ASSERTCOND(pOleOutlineDoc->vtbl->m_NeedsSaveProcPtr != nil);
	return (*pOleOutlineDoc->vtbl->m_NeedsSaveProcPtr)(pDoc, askUserToSave, defaultAnswer, quitting);
}

//#pragma segment OleOutlineDocumentSeg
void OleOutlineDocDoSaveAs(DocumentPtr pDoc)
{
	volatile OleOutlineDocPtr	pOleOutlineDoc;
	
	pOleOutlineDoc = (OleOutlineDocPtr)pDoc;

	OleDocEnableDialog(&pOleOutlineDoc->m_OleDoc);

	TRY
	{
		ASSERTCOND(pOleOutlineDoc->vtbl->m_DoSaveAsProcPtr != nil);
		(*pOleOutlineDoc->vtbl->m_DoSaveAsProcPtr)(pDoc);

		OleDocSetFileMonikerFSSpec(&pOleOutlineDoc->m_OleDoc, &pDoc->m_File);

		OleDocDisableDialog(&pOleOutlineDoc->m_OleDoc, true);
	}
	CATCH
	{
		OleDocDisableDialog(&pOleOutlineDoc->m_OleDoc, true);
	}
	ENDTRY

	OleDocDoSaveAs(&pOleOutlineDoc->m_OleDoc);
}


//#pragma segment OleOutlineDocumentSeg
YNCResult OleOutlineDocDoClose(DocumentPtr pDoc, Boolean askUserToSave, YNCResult defaultAnswer, Boolean quitting)
{
	OleOutlineDocPtr	pOleOutlineDoc;
	YNCResult			res;

	pOleOutlineDoc = (OleOutlineDocPtr)pDoc;

	OleOutlineDocAddRef(pOleOutlineDoc);

	ASSERTCOND(pOleOutlineDoc->vtbl->m_DoCloseProcPtr != nil);
	res = (*pOleOutlineDoc->vtbl->m_DoCloseProcPtr)(pDoc, askUserToSave, defaultAnswer, quitting);

#if qOleContainerApp
	if (res != cancelResult)
		res = OleOutlineContainerDocDoClose(pOleOutlineDoc, askUserToSave, defaultAnswer, quitting);
#endif

	if (res != cancelResult)
		OleDocDoClose(&pOleOutlineDoc->m_OleDoc, askUserToSave, defaultAnswer, quitting);

	OleOutlineDocRelease(pOleOutlineDoc);

	return res;
}

//#pragma segment OleOutlineDocumentSeg
void OleOutlineDocDoSave(DocumentPtr pDoc)
{
	OleOutlineDocPtr	pOleOutlineDoc;
	
	pOleOutlineDoc = (OleOutlineDocPtr)pDoc;

	if (pDoc->m_DocType == kEmbeddedDocumentType)
	{
		OleDocDoSave(&pOleOutlineDoc->m_OleDoc);

		ASSERTCOND(pDoc->vtbl->m_SetDirtyProcPtr != nil);
		(*pDoc->vtbl->m_SetDirtyProcPtr)(pDoc, false);
	}
	else
	{
		ASSERTCOND(pOleOutlineDoc->vtbl->m_DoSaveProcPtr != nil);
		(*pOleOutlineDoc->vtbl->m_DoSaveProcPtr)(pDoc);
	}
}

//#pragma segment OleOutlineDocumentSeg
void OleOutlineDocDoOpen(DocumentPtr pDoc, FSSpecPtr theFile, Boolean readOnly)
{
	volatile OleOutlineDocPtr	pOleOutlineDoc;
	
	pOleOutlineDoc = (OleOutlineDocPtr)pDoc;

	TRY
	{
		OleOutlineDocAddRef(pOleOutlineDoc);

		ASSERTCOND(pOleOutlineDoc->vtbl->m_DoOpenProcPtr != nil);
		(*pOleOutlineDoc->vtbl->m_DoOpenProcPtr)(pDoc, theFile, readOnly);

		OleDocSetFileMonikerFSSpec(&pOleOutlineDoc->m_OleDoc, theFile);

		OleOutlineDocRelease(pOleOutlineDoc);
	}
	CATCH
	{
		OleOutlineDocRelease(pOleOutlineDoc);
	}
	ENDTRY
}

//#pragma segment OleOutlineDocumentSeg
YNCResult OleOutlineDocPresentSaveDialog(DocumentPtr pDoc, Boolean quitting)
{
	volatile OleOutlineDocPtr	pOleOutlineDoc;
	YNCResult					res;
	
	pOleOutlineDoc = (OleOutlineDocPtr)pDoc;

	OleDocEnableDialog(&pOleOutlineDoc->m_OleDoc);
	
	TRY
	{
		ASSERTCOND(pOleOutlineDoc->vtbl->m_PresentSaveDialogProcPtr != nil);
		res = (*pOleOutlineDoc->vtbl->m_PresentSaveDialogProcPtr)(pDoc, quitting);

		OleDocDisableDialog(&pOleOutlineDoc->m_OleDoc, true);
	}
	CATCH
	{
		OleDocDisableDialog(&pOleOutlineDoc->m_OleDoc, true);
	}
	ENDTRY
		
	return res;
}

//#pragma segment OleOutlineDocumentSeg
void OleOutlineDocSetDirty(DocumentPtr pDoc, Boolean fDirty)
{
	OleOutlineDocPtr	pOleOutlineDoc;
	
	pOleOutlineDoc = (OleOutlineDocPtr)pDoc;

	OleDocSetDirty(&pOleOutlineDoc->m_OleDoc, fDirty);
	
	ASSERTCOND(pOleOutlineDoc->vtbl->m_SetDirtyProcPtr != nil);
	(*pOleOutlineDoc->vtbl->m_SetDirtyProcPtr)(pDoc, fDirty);
}

#if qOleServerApp && qOleInPlace
//#pragma segment OleOutlineDocumentSeg
void OleOutlineDocSetDefaultCursor(DocumentPtr pDoc)
{
	OleOutlineDocPtr	pOleOutlineDoc;
	
	pOleOutlineDoc = (OleOutlineDocPtr)pDoc;

	if (OleDocSetDefaultCursor(&pOleOutlineDoc->m_OleDoc))
		return;

	ASSERTCOND(pOleOutlineDoc->vtbl->m_SetDefaultCursorProcPtr != nil);
	(*pOleOutlineDoc->vtbl->m_SetDefaultCursorProcPtr)(pDoc);
}
#endif

//#pragma segment OleOutlineDocumentSeg
void OleOutlineDocDoCmd(OleOutlineDocPtr pOleOutlineDoc, long cmd)
{
#if qOleContainerApp
	short 		menuID;
	short 		menuItem;
	Boolean 	fConvert = false;
	long		verb;

	ASSERTCOND(gApplication->vtbl->m_CmdToMenuItemProcPtr != nil);
	if (!(*gApplication->vtbl->m_CmdToMenuItemProcPtr)(gApplication, cmd, &menuID, &menuItem)) {
		menuID = (-cmd) >> 16;
		menuItem = (-cmd) & 0x0000ffff;
	}

	if (OleUIFindVerbConvert(
			&pOleOutlineDoc->container.m_VerbMenuRec,
			menuID,
			menuItem,
			&fConvert,
			&verb))
	{
		if (fConvert)
			OleOutlineContainerDocConvert(pOleOutlineDoc, false);
		else
			LineListDoObjectVerb(OutlineDocGetLineList((OutlineDocPtr)pOleOutlineDoc), verb);
	}
	else
#endif			
			
	switch(cmd)
	{
#if qOleContainerApp
		case cmdInsertObject:	
			OleOutlineContainerDocInsertDlg(pOleOutlineDoc);
			break;
		case cmdPasteLink:		
			OleContainerDocPasteLink(&pOleOutlineDoc->m_OleDoc);		
			break;
		case cmdPasteSpecial:	
			OleOutlineContainerDocPasteSpecialDlg(pOleOutlineDoc);		
			break;
		case cmdLinks:			
			OleOutlineContainerDocEditLinksDlg(pOleOutlineDoc);			
			break;
#elif qOleServerApp
		case cmdOpenVerb:
			OleServerDocOleObjectDoVerb(&pOleOutlineDoc->m_OleDoc, OLEIVERB_OPEN, NULL, NULL, -1, NULL, NULL);
			break;
#endif	
		default:
			InheritDoCmd(pOleOutlineDoc->m_GopherDoCmd, cmd);
			break;
	}
}

//#pragma segment OleOutlineDocumentSeg
void OleOutlineDocUpdateMenus(OleOutlineDocPtr pOleOutlineDoc)
{
#if qOleContainerApp
	LineListPtr 	pLineList;
	
	pLineList = OutlineDocGetLineList((OutlineDocPtr)pOleOutlineDoc);
	LineListUpdateEditMenu(pLineList, &pOleOutlineDoc->container.m_VerbMenuRec);
	
	EnableCmd(cmdInsertObject);
		
	{
		Boolean		canPaste;
		Boolean		canPasteLink;
		
		OleDocGetPasteStatus(&pOleOutlineDoc->m_OleDoc, &canPaste, &canPasteLink);
		
		if (canPasteLink)
			EnableCmd(cmdPasteLink);

		if (canPaste || canPasteLink)
			EnableCmd(cmdPasteSpecial);
	}

	EnableCmd(cmdLinks);

#elif qOleServerApp && qOleInPlace
	OleInPlaceServerDocUpdateMenus(&pOleOutlineDoc->m_OleDoc);
#endif
	
	InheritUpdateMenus(pOleOutlineDoc->m_GopherUpdateMenus);
}

/* OleOutlineDocSetFSSpec
 * --------------------
 *
 *	Purpose:
 *      Set the file record of document.
 *
 *  OLE2NOTE: If the ServerDoc has a valid filename then the object is
 *  registered in the running object table (ROT). if the name of the doc
 *  changes (eg. via SaveAs) then the previous registration must be revoked
 *  and the document re-registered under the new name.
 *
 *	Returns:
 *		True if file record is changed
 *		False if not
 */
//#pragma segment OleOutlineDocumentSeg
void OleOutlineDocSetFSSpec(DocumentPtr pDoc, FSSpecPtr pFile)
{
	OleOutlineDocPtr pOleOutlineDoc;
	
	ASSERTCOND(pDoc != nil && pFile != nil);
	
	pOleOutlineDoc = (OleOutlineDocPtr)pDoc;
	
	// make sure the FSSpecs are not the same
	ASSERTCOND(pDoc->vtbl->m_IsEqualFSSpecProcPtr != nil);
	if (!(*pDoc->vtbl->m_IsEqualFSSpecProcPtr)(pDoc, pFile))
	{
		ASSERTCOND(pOleOutlineDoc->vtbl->m_SetFSSpecProcPtr != nil);
		(*pOleOutlineDoc->vtbl->m_SetFSSpecProcPtr)(pDoc, pFile);

        /* OLE2NOTE: both containers and servers must properly
        **    register in the RunningObjectTable. if the document
        **    is performing a SaveAs operation, then it must
        **    re-register in the ROT with the new moniker. in
        **    addition any embedded object, pseudo objects, and/or
        **    linking clients must be informed that the document's
        **    moniker has changed.
        */

#if 0
        OleOutlineDocAddRef(pOleOutlineDoc);
		OleDocSetFileMonikerFSSpec(&pOleOutlineDoc->m_OleDoc, pFile);
		OleOutlineDocRelease(pOleOutlineDoc);
#endif
    }
}

//#pragma segment OleOutlineDocumentSeg
HRESULT OleOutlineDocQueryInterface(OleOutlineDocPtr pOleOutlineDoc, REFIID riid, void* * lplpvObj)
{
	if (IsEqualIID(riid, &IID_IUnknown))
	{
		*lplpvObj = &pOleOutlineDoc->m_IUnknown;
		OleOutlineDocAddRef(pOleOutlineDoc);
		return NOERROR;
	}
	
#if qOleContainerApp
    else if (IsEqualIID(riid, &IID_IOleUILinkContainer)) {

        *lplpvObj = &pOleOutlineDoc->container.m_IOleUILinkContainer;
		OleOutlineDocAddRef(pOleOutlineDoc);
		return NOERROR;
    }
#endif

	else
		return OleDocLocalQueryInterface(&pOleOutlineDoc->m_OleDoc, riid, lplpvObj);
}

//#pragma segment OleOutlineDocumentSeg
unsigned long OleOutlineDocAddRef(OleOutlineDocPtr pOleOutlineDoc)
{
	++pOleOutlineDoc->m_cRef;

	return pOleOutlineDoc->m_cRef;
}

//#pragma segment OleOutlineDocumentSeg
unsigned long OleOutlineDocRelease(OleOutlineDocPtr pOleOutlineDoc)
{
	DocumentPtr	pDoc;
	unsigned long		cRef;
	
	pDoc = (DocumentPtr)pOleOutlineDoc;
	
	ASSERTCOND(pOleOutlineDoc->m_cRef > 0);
	
	cRef = --pOleOutlineDoc->m_cRef;
	
	// if ref count is zero make sure that we free the memory associate with
	// the document
	if (cRef == 0)
	{
		DocumentPtr		pDoc;
		
		pDoc = (DocumentPtr)pOleOutlineDoc;
		
		ASSERTCOND(pDoc->vtbl->m_DisposeProcPtr != nil);
		(*pDoc->vtbl->m_DisposeProcPtr)(pDoc);
	}
	
	return cRef;
}

//#pragma segment OleOutlineDocumentSeg
HRESULT OleOutlineDocSaveSelectionToStorage(OleOutlineDocPtr pOleOutlineDoc, struct LineRangeRec* pSelection, unsigned int uFormat, LPSTORAGE lpDestStg, Boolean fRemember)
{
	OutlineDocPtr		pOutlineDoc;
	HRESULT				hrErr;
	volatile LPSTREAM	lpLineListStm = nil;
    char* 				lpszUserType;

	hrErr = NOERROR;

	TRY
	{
		pOutlineDoc = (OutlineDocPtr)pOleOutlineDoc;
	
		/*  OLE2NOTE: we must be sure to write the information required for
		**    OLE into our docfile. this includes user type
		**    name, data format, etc. Even for top "file-level" objects
		**    this information should be written to the file. Both
		**    containters and servers should write this information.
		*/
	
	#if qOleServerApp && qOleTreatAs
	
		/* OLE2NOTE: if the Server is emulating another class (ie.
		**    "TreatAs" aka. ActivateAs), it must write the same user type
		**    name and format that was was originally written into the
		**    storage rather than its own user type name.
		**
		**    SVROUTL and ISVROTL can emulate each other. they have the
		**    simplification that they both read/write the identical
		**    format. thus for these apps no actual conversion of the
		**    native bits is actually required.
		*/
		if (! IsEqualCLSID(&pOleOutlineDoc->server.m_clsidTreatAs, &CLSID_NULL))
			lpszUserType = pOleOutlineDoc->server.m_pszTreatAsType;
		else
	#endif  // qOleTreatAs
	
	#if qOleServerApp
		lpszUserType = kOutlineServerFullUserTypeName;
	#elif qOleContainerApp
		lpszUserType = kOutlineContainerFullUserTypeName;
	#endif
	
		hrErr = WriteFmtUserTypeStg(lpDestStg, uFormat, lpszUserType);
		ASSERTNOERROR(hrErr);
		FailOleErr(hrErr);
	
		TRY
		{
			hrErr = lpDestStg->lpVtbl->CreateStream(
						lpDestStg,
						kLineListStreamName,
						STGM_WRITE | STGM_SHARE_EXCLUSIVE | STGM_CREATE,
						0,
						0,
						(LPSTREAM *)&lpLineListStm);
			ASSERTNOERROR(hrErr);
			FailOleErr(hrErr);
	
			{
				OutlineDocumentHeaderRec		docHeader;
				unsigned long					nWritten;
	
				bzero(&docHeader, sizeof(OutlineDocumentHeaderRec));
	
				OleOutlineAppGetAppVersionNo((OleOutlineAppPtr)gApplication, docHeader.m_narrAppVersionNo);
				docHeader.m_fShowHeading = pOutlineDoc->m_fShowRowColumnHeadings;
			
	#if qOleServerApp
				{
					// Store ServerDoc specific data
					docHeader.m_nNextRangeNo = pOutlineDoc->m_nNextRangeNo;
					strcpy(docHeader.m_szFormatName, OUTLINEDOCFORMAT);
				}
	#endif
	
	#if qOleContainerApp
				{
					// Store ContainerDoc specific data
					docHeader.m_nNextObjNo = pOleOutlineDoc->container.m_nNextObjNo;
					strcpy(docHeader.m_szFormatName, CONTAINERDOCFORMAT);
				}
	#endif
	
				docHeader.m_narrAppVersionNo[0] = SwapWord(docHeader.m_narrAppVersionNo[0]);
				docHeader.m_narrAppVersionNo[1] = SwapWord(docHeader.m_narrAppVersionNo[1]);
				docHeader.m_fShowHeading = SwapWord(docHeader.m_fShowHeading);
				docHeader.m_nNextRangeNo = SwapLong(docHeader.m_nNextRangeNo);
				docHeader.m_nNextObjNo = SwapLong(docHeader.m_nNextObjNo);
				docHeader.m_reserved3 = SwapLong(docHeader.m_reserved3);
				docHeader.m_reserved4 = SwapLong(docHeader.m_reserved4);
	
				hrErr = lpLineListStm->lpVtbl->Write(
						lpLineListStm,
						&docHeader,
						sizeof(OutlineDocumentHeaderRec),
						&nWritten
					);
				ASSERTNOERROR(hrErr);
				FailOleErr(hrErr);
	
				docHeader.m_narrAppVersionNo[0] = SwapWord(docHeader.m_narrAppVersionNo[0]);
				docHeader.m_narrAppVersionNo[1] = SwapWord(docHeader.m_narrAppVersionNo[1]);
				docHeader.m_fShowHeading = SwapWord(docHeader.m_fShowHeading);
				docHeader.m_nNextRangeNo = SwapLong(docHeader.m_nNextRangeNo);
				docHeader.m_nNextObjNo = SwapLong(docHeader.m_nNextObjNo);
				docHeader.m_reserved3 = SwapLong(docHeader.m_reserved3);
				docHeader.m_reserved4 = SwapLong(docHeader.m_reserved4);
	
			}
	
			LineListSaveSelectionToStorage(
					pOutlineDoc->m_LineList,
					pSelection,
					OleOutlineDocGetStorage(pOleOutlineDoc),
					lpDestStg,
					lpLineListStm,
					fRemember
				);
	
			OleStdRelease((LPUNKNOWN)lpLineListStm);
	
			NameTableSaveSelectionToStorage(pOutlineDoc->m_NameTable, pSelection, lpDestStg);
			
			pOleOutlineDoc->m_cfSaveFormat = uFormat;  // remember format used to save
	
	#if qOleContainerApp	
			if (fRemember) {
				if (pOleOutlineDoc->container.m_pStorage)
					OleStdRelease((LPUNKNOWN)pOleOutlineDoc->container.m_pStorage);
				pOleOutlineDoc->container.m_pStorage = lpDestStg;
				pOleOutlineDoc->container.m_pStorage->lpVtbl->AddRef(pOleOutlineDoc->container.m_pStorage);
			}
				
	#endif
		
		}
		CATCH
		{
			if (lpLineListStm != nil)
				OleStdRelease((LPUNKNOWN)lpLineListStm);
		}
		ENDTRY
	}
	CATCH
	{
		if (IsOleFailure())
			hrErr = GetOleFailure();
		else
			hrErr = ResultFromScode(E_FAIL);

		NO_PROPAGATE;
	}
	ENDTRY

	return hrErr;
}

//#pragma segment OleOutlineDocumentSeg
HRESULT OleOutlineDocLoadFromStorage(OleOutlineDocPtr pOleOutlineDoc, LPSTORAGE lpSrcStg)
{
	OutlineDocPtr		pOutlineDoc;
	HRESULT				hrErr;
	volatile LPSTREAM	lpLineListStm;
	
	pOutlineDoc = (OutlineDocPtr)pOleOutlineDoc;
	hrErr = NOERROR;

	TRY
	{
		hrErr = lpSrcStg->lpVtbl->OpenStream(
					lpSrcStg,
					kLineListStreamName,
					NULL,
					STGM_READ | STGM_SHARE_EXCLUSIVE,
					0,
					(LPSTREAM *)&lpLineListStm);
		ASSERTNOERROR(hrErr);
		FailOleErr(hrErr);

		{
			OutlineDocumentHeaderRec		docHeader;
			unsigned long					nRead;

			bzero(&docHeader, sizeof(OutlineDocumentHeaderRec));

			hrErr = lpLineListStm->lpVtbl->Read(
					lpLineListStm,
					&docHeader,
					sizeof(OutlineDocumentHeaderRec),
					&nRead
				);
			ASSERTNOERROR(hrErr);
			FailOleErr(hrErr);
			
			docHeader.m_narrAppVersionNo[0] = SwapWord(docHeader.m_narrAppVersionNo[0]);
			docHeader.m_narrAppVersionNo[1] = SwapWord(docHeader.m_narrAppVersionNo[1]);
			docHeader.m_fShowHeading = SwapWord(docHeader.m_fShowHeading);
			docHeader.m_nNextRangeNo = SwapLong(docHeader.m_nNextRangeNo);
			docHeader.m_nNextObjNo = SwapLong(docHeader.m_nNextObjNo);
			docHeader.m_reserved3 = SwapLong(docHeader.m_reserved3);
			docHeader.m_reserved4 = SwapLong(docHeader.m_reserved4);

		    OleOutlineAppVersionNoCheck(
		            (OleOutlineAppPtr)gApplication,
		            docHeader.m_szFormatName,
		            docHeader.m_narrAppVersionNo
		    );
		    pOutlineDoc->m_fShowRowColumnHeadings = docHeader.m_fShowHeading;
		
#if qOleServerApp
		    {
		        // Load ServerDoc specific data
		        pOutlineDoc->m_nNextRangeNo = docHeader.m_nNextRangeNo;
		
#if qOleTreatAs
				{
			        CLSID       clsid;
			        ResType     cfFmt;
			        char*       lpszType;
				
			        /* OLE2NOTE: if the Server is capable of supporting "TreatAs"
			        **    (aka. ActivateAs), it must read the class that is written
			        **    into the storage. if this class is NOT the app's own
			        **    class ID, then this is a TreatAs operation. the server
			        **    then must faithfully pretend to be the class that is
			        **    written into the storage. it must also faithfully write
			        **    the data back to the storage in the SAME format as is
			        **    written in the storage.
			        **
			        **    ServerOutline and ServerOutline (IP) can emulate each other. they have the
			        **    simplification that they both read/write the identical
			        **    format. thus for these apps no actual conversion of the
			        **    native bits is actually required.
			        */
			        pOleOutlineDoc->server.m_clsidTreatAs = CLSID_NULL;
			        if (OleStdGetTreatAsFmtUserType(&CLSID_Application, lpSrcStg, &clsid,
			                            &cfFmt, &lpszType)) {
			                            		
			            if (cfFmt == kOutlineDocumentFileType) {
			                // We should perform TreatAs operation
			                if (pOleOutlineDoc->server.m_pszTreatAsType)
			                    OleStdFreeString(pOleOutlineDoc->server.m_pszTreatAsType, NULL);
			
			                pOleOutlineDoc->server.m_clsidTreatAs = clsid;
			                pOleOutlineDoc->m_cfSaveFormat = cfFmt;
			                pOleOutlineDoc->server.m_pszTreatAsType = lpszType;
			
			            } else {
			                // ERROR: we ONLY support TreatAs for CF_OUTLINE format
			                OleStdFreeString(lpszType, NULL);
			            }
			        }
		        }
#endif  // qOleTreatAs
		    }
#endif  // qOleServerApp

#if qOleContainerApp
		    {
		        // Load ContainerDoc specific data
		        pOleOutlineDoc->container.m_nNextObjNo = docHeader.m_nNextObjNo;
		    }
#endif  // qOleContainerApp
		
		}

		LineListSetRedraw(pOutlineDoc->m_LineList, false);

		LineListLoadFromStorage(pOutlineDoc->m_LineList, pOleOutlineDoc, lpSrcStg, lpLineListStm);
		OleStdRelease((LPUNKNOWN)lpLineListStm);
		lpLineListStm = nil;

#if qOleContainerApp
		{
			// free previous storage
			if (pOleOutlineDoc->container.m_pStorage)
				OleStdVerifyRelease((LPUNKNOWN)pOleOutlineDoc->container.m_pStorage);
			
			// make sure we addref it for later freeing
			lpSrcStg->lpVtbl->AddRef(lpSrcStg);
			pOleOutlineDoc->container.m_pStorage = lpSrcStg;
		}
#endif

		LineListSetRedraw(pOutlineDoc->m_LineList, true);

		NameTableLoadFromStorage(pOutlineDoc->m_NameTable, lpSrcStg);
	}
	CATCH
	{
		if (lpLineListStm != nil)
			OleStdRelease((LPUNKNOWN)lpLineListStm);

		if (IsOleFailure())
			hrErr = GetOleFailure();
		else
			hrErr = ResultFromScode(E_FAIL);

		NO_PROPAGATE;
	}
	ENDTRY

	return hrErr;
}


//#pragma segment OleOutlineDocumentSeg
LPMONIKER OleOutlineDocGetFullMoniker(OleOutlineDocPtr pOleOutlineDoc, unsigned long dwAssign)
{
	return OleDocGetFullMoniker(&pOleOutlineDoc->m_OleDoc, dwAssign);
}


//#pragma segment OleOutlineDocumentSeg
Boolean OleOutlineDocCanPaste(DocumentPtr pDoc)
{
	Boolean 			CanPasteCopy;
	OleOutlineDocPtr 	pOleOutlineDoc = (OleOutlineDocPtr)pDoc;
	OleOutlineAppPtr	pOleOutlineApp = (OleOutlineAppPtr)gApplication;
	
	OleDocGetPasteStatus(&pOleOutlineDoc->m_OleDoc, &CanPasteCopy, nil);
	
	return CanPasteCopy;		// this needs to be fixed
}

//#pragma segment OleOutlineDocumentSeg
/* OleOutlineDocDoCopy
 * -------------------
 *  Copy selection to clipboard.
 *  Post to the clipboard the formats that the app can render.
 *  the actual data is not rendered at this time. using the
 *  delayed rendering technique, Apple Events will be sent
 *  when the actual data is requested.
 *
 */
void OleOutlineDocDoCopy(DocumentPtr pDoc)
{
	OutlineDocPtr		pOutlineDoc;
    OleOutlineDocPtr 	pClipboardDoc;

	pOutlineDoc = (OutlineDocPtr)pDoc;

    /* squirrel away a copy of the current selection to the ClipboardDoc */
    pClipboardDoc = (OleOutlineDocPtr)OleOutlineAppCreateDataXferDoc((OleOutlineAppPtr)gApplication);
    FailNIL(pClipboardDoc);

	ASSERTCOND(((DocumentPtr)pClipboardDoc)->vtbl->m_DoNewProcPtr != nil);
	(*((DocumentPtr)pClipboardDoc)->vtbl->m_DoNewProcPtr)((DocumentPtr)pClipboardDoc);

	OleOutlineDocCopyToDoc(pDoc, (DocumentPtr)pClipboardDoc);

	OleDocDoCopy(&pClipboardDoc->m_OleDoc);
	
	((OleOutlineDocPtr)pDoc)->m_fLinkSourceEnabled = true;
}

//#pragma segment OleOutlineDocumentSeg
/* OleOutlineDocDoClear
 * --------------------
 *  Clear selection in the document
 *
 */
void OleOutlineDocDoClear(DocumentPtr pDoc)
{
	OleOutlineDocPtr pOleOutlineDoc = (OleOutlineDocPtr)pDoc;

	ASSERTCOND(pOleOutlineDoc->vtbl->m_DoClearProcPtr != nil);
	(*pOleOutlineDoc->vtbl->m_DoClearProcPtr)(pDoc);
		
	/* OLE2NOTE: if the document that is the source of data on the
	**    clipborad has just had lines deleted, then the copied data
	**    is no longer considered a valid potential link source.
	**    disable the offering of CF_LINKSOURCE from the clipboard
	**    document. this avoids problems that arise when the
	**    editing operation changes or deletes the original data
	**    copied. we will not go to the trouble of determining if
	**    the deleted line actually is part of the link source.
	*/
	if (pOleOutlineDoc->m_fLinkSourceEnabled) {
		OleDocDisableLinkSource(&pOleOutlineDoc->m_OleDoc);
		pOleOutlineDoc->m_fLinkSourceEnabled = false;
	}
}

//#pragma segment OleOutlineDocumentSeg
/* OleOutlineDocCopyToDoc
** ----------------------
**
**	  Copy the selected data to another Document
*/
void OleOutlineDocCopyToDoc(DocumentPtr pSrcDoc, DocumentPtr pDestDoc)
{	
	OleDocCopyToDoc(&((OleOutlineDocPtr)pSrcDoc)->m_OleDoc, &((OleOutlineDocPtr)pDestDoc)->m_OleDoc);
	LineListCopyToDoc(((OutlineDocPtr)pSrcDoc)->m_LineList, pDestDoc);
}	

//#pragma segment OleOutlineDocumentSeg
/* OleOutlineDocDoPaste
** --------------------
**    Paste default format data from the clipboard.
**    In this function we choose the highest fidelity format that the
**    source clipboard IDataObject* offers that we understand.
**
**    OLE2NOTE: clipboard handling in an OLE 2.0 application is
**    different than normal Windows clipboard handling. Data from the
**    clipboard is retieved by getting the IDataObject* pointer
**    returned by calling OleGetClipboard.
*/
void OleOutlineDocDoPaste(DocumentPtr pDoc)
{
	OleDocumentPtr pOleDoc = &((OleOutlineDocPtr)pDoc)->m_OleDoc;
	
	OleDocDoPaste(pOleDoc);
}

//#pragma segment OleOutlineDocumentSeg
unsigned int OleOutlineDocUIDialogHook(DialogPtr pDialog, EventRecord *pEvent, short *itemHit, long lCustData)
{
	ApplicationPtr	pApp = gApplication;
	DocumentPtr		pDoc;
	GrafPtr			oldPort;
	WindowPtr		whichWindow;

	switch (pEvent->what)
	{
		case updateEvt:
			whichWindow = (WindowPtr)pEvent->message;
			if (whichWindow != pDialog)
			{
				// check if window belongs to a document
				ASSERTCOND(pApp->vtbl->m_FindDocProcPtr != nil);
				pDoc = (*pApp->vtbl->m_FindDocProcPtr)(pApp, whichWindow);
			
				GetPort(&oldPort);
				SetPort(whichWindow);
				BeginUpdate(whichWindow);
			
				if (pDoc != nil)
				{
					ASSERTCOND(pDoc->vtbl->m_DoUpdateProcPtr != nil);
					(*pDoc->vtbl->m_DoUpdateProcPtr)(pDoc);
				}
			
				EndUpdate(whichWindow);
				SetPort(oldPort);
				
				*itemHit = 0;
				return true;
			}
			break;
				
		case nullEvent:		// allow the documents to update their views
			if (pApp->m_fNeedsIdle)
			{
				ASSERTCOND(pApp->vtbl->m_DoIdleProcPtr != nil);
				(*pApp->vtbl->m_DoIdleProcPtr)(pApp);

				pApp->m_fNeedsIdle = false;
			}
			break;
	}
	
	return false;
}

#if qOleDragDrop

//#pragma segment OleOutlineDocumentSeg
/* OleOutlineDocStartDrag
 * ----------------------
 *
 *	Purpose:
 *		Test the dragging criteria to see if it should be started. DoDragDrop if so.
 *
 *	Parameter:
 *		mPt		position of the mouse when the function is called
 *
 *	Returns:
 *		true	if dragging can be started
 *		false 	otherwise
 */
void OleOutlineDocStartDrag(OleOutlineDocPtr pOleOutlineDoc, Point mPt)
{
	if (OleDocStartDrag(&pOleOutlineDoc->m_OleDoc, mPt))
		OleOutlineDocDoDragDrop(pOleOutlineDoc);
}


//#pragma segment OleOutlineDocumentSeg
/* OleOutlineDocDoDragDrop
 * -----------------------
 *  Actually perform a drag/drop operation with the current selection in
 *      the source document (lpSrcOleDoc).
 *
 *  returns the result effect of the drag/drop operation:
 *      DROPEFFECT_NONE,
 *      DROPEFFECT_COPY,
 *      DROPEFFECT_MOVE, or
 *      DROPEFFECT_LINK
 */
unsigned long OleOutlineDocDoDragDrop (OleOutlineDocPtr pSrcOleOutlineDoc)
{
    OutlineDocPtr 		pSrcOutlineDoc = (OutlineDocPtr)pSrcOleOutlineDoc;
    OleOutlineDocPtr 	pDragDoc;
    unsigned long 		dwEffect;
    LineRangeRec		lrSel;
    LinePtr				pStartLine;
    LinePtr				pEndLine;
    LineListPtr			pSrcLineList;

    OLEDBG_BEGIN3("OleDoc_DoDragDrop\r\n")

    /* squirrel away a copy of the current selection to the DragDoc */
    pDragDoc = (OleOutlineDocPtr)OleOutlineAppCreateDataXferDoc((OleOutlineAppPtr)gApplication);
    if (!pDragDoc)
    	return DROPEFFECT_NONE;

	ASSERTCOND(((DocumentPtr)pDragDoc)->vtbl->m_DoNewProcPtr != nil);
	(*((DocumentPtr)pDragDoc)->vtbl->m_DoNewProcPtr)((DocumentPtr)pDragDoc);

	OleOutlineDocCopyToDoc((DocumentPtr)pSrcOleOutlineDoc, (DocumentPtr)pDragDoc);
	
	pSrcLineList = ((OutlineDocPtr)pSrcOleOutlineDoc)->m_LineList;
	LineListGetSelection(pSrcLineList, &lrSel);
	pStartLine = LineListGetLine(pSrcLineList, lrSel.m_nStartLine);
	pEndLine = LineListGetLine(pSrcLineList, lrSel.m_nEndLine);
	
    dwEffect = OleDocDoDragDrop(&pDragDoc->m_OleDoc);

    /* if after the Drag/Drop modal (mouse capture) loop is finished
    **    and a drag MOVE operation was performed, then we must delete
    **    the lines that were dragged.
    */
    if (dwEffect & DROPEFFECT_MOVE)
    {
    	Cell		theCell;
    	
    	if (!LineListFindLineCell(pSrcLineList, pStartLine, &theCell))
    		return dwEffect;
    	lrSel.m_nStartLine = theCell.v;
    	
    	if (!LineListFindLineCell(pSrcLineList, pEndLine, &theCell))
    		return dwEffect;
    	lrSel.m_nEndLine = theCell.v;
    	
    	LineListDeleteRange(pSrcLineList, &lrSel);
	}
	
    OLEDBG_END3
    return dwEffect;
}
#endif  // qOleDragDrop

//#pragma segment OleOutlineDocumentSeg
void OleOutlineDocDoIdle(DocumentPtr pDoc)
{
	OleOutlineDocPtr	pOleOutlineDoc;
	
	pOleOutlineDoc = (OleOutlineDocPtr)pDoc;

	OleDocDoIdle(&pOleOutlineDoc->m_OleDoc);

	ASSERTCOND(pOleOutlineDoc->vtbl->m_DoIdleProcPtr != nil);
	(*pOleOutlineDoc->vtbl->m_DoIdleProcPtr)(pDoc);
}

//#pragma segment OleOutlineDocumentSeg
void OleOutlineDocDoActivate(DocumentPtr pDoc, Boolean becomingActive)
{
	OleOutlineDocPtr	pOleOutlineDoc;

	pOleOutlineDoc = (OleOutlineDocPtr)pDoc;

#if qOleContainerApp && qOleInPlace
	if (!OleInPlaceContainerDocNeedsActivateEvent(&pOleOutlineDoc->m_OleDoc))
		return;
#endif

	ASSERTCOND(pOleOutlineDoc->vtbl->m_DoActivateProcPtr != nil);
	(*pOleOutlineDoc->vtbl->m_DoActivateProcPtr)(pDoc, becomingActive);

	if (becomingActive)
	{
		ApplicationPtr		pApp;

		pApp = gApplication;

#if qOleServerApp && qOleInPlace
		if (!OleInPlaceServerDocIsInPlace(&pOleOutlineDoc->m_OleDoc))
#endif
		{
			ASSERTCOND(pApp->vtbl->m_SetBorderSpaceProcPtr != nil);
			(*pApp->vtbl->m_SetBorderSpaceProcPtr)(pApp, &pDoc->m_BorderWidths, false);
		}

#if qFrameTools
		if (pDoc->m_fShowFrameTools)
		{
			ASSERTCOND(pApp->vtbl->m_ShowFrameToolsProcPtr != nil);
			(*pApp->vtbl->m_ShowFrameToolsProcPtr)(pApp);
		}
#endif

		GopherAdd(pOleOutlineDoc->m_GopherUpdateMenus);
		GopherAdd(pOleOutlineDoc->m_GopherDoCmd);
	}
	else
	{
		GopherRemove(pOleOutlineDoc->m_GopherUpdateMenus);
		GopherRemove(pOleOutlineDoc->m_GopherDoCmd);
	}
	
	OleDocDoActivate(&pOleOutlineDoc->m_OleDoc, becomingActive);
}

//#pragma segment OleOutlineDocumentSeg
void OleOutlineDocDoContent(DocumentPtr pDoc, EventRecord* theEvent)
{
	OleOutlineDocPtr pOleOutlineDoc = (OleOutlineDocPtr)pDoc;

#if qOleContainerApp && qOleInPlace
	{
		Point	pt;
		Rect	rList;

		pt = theEvent->where;
		GlobalToLocal(&pt);

		LineListGetView(((OutlineDocPtr)pDoc)->m_LineList, &rList);

		if (PtInRect(pt, &rList) &&
			OleInPlaceContainerDocIsUIActive(&((OleOutlineDocPtr)pDoc)->m_OleDoc))
		{
			OleInPlaceContainerDocUIDeactivate(&((OleOutlineDocPtr)pDoc)->m_OleDoc);
			return;
		}
	}
#endif

	ASSERTCOND(pOleOutlineDoc->vtbl->m_DoContentProcPtr != nil);
	(*pOleOutlineDoc->vtbl->m_DoContentProcPtr)(pDoc, theEvent);
}

#if qOleServerApp && qOleTreatAs

//#pragma segment OleOutlineDocumentSeg
void OleOutlineDocDoTreatAs(OleOutlineDocPtr pOleOutlineDoc, LPSTORAGE lpSrcStg)
{
	CLSID	clsid;
	ResType	cfFmt;
	char*	lpszType;
	
	/* OLE2NOTE: if the Server is capable of supporting "TreatAs"
	**    (aka. ActivateAs), it must read the class that is written
	**    into the storage. if this class is NOT the app's own
	**    class ID, then this is a TreatAs operation. the server
	**    then must faithfully pretend to be the class that is
	**    written into the storage. it must also faithfully write
	**    the data back to the storage in the SAME format as is
	**    written in the storage.
	**
	**    ServerOutline and ServerOutline (IP) can emulate each other. they have the
	**    simplification that they both read/write the identical
	**    format. thus for these apps no actual conversion of the
	**    native bits is actually required.
	*/
	pOleOutlineDoc->server.m_clsidTreatAs = CLSID_NULL;
	if (OleStdGetTreatAsFmtUserType(&CLSID_Application, lpSrcStg, &clsid,
				&cfFmt, &lpszType)) {
                            		
		if (cfFmt == kOutlineDocumentFileType) {
			// We should perform TreatAs operation
			if (pOleOutlineDoc->server.m_pszTreatAsType)
				OleStdFreeString(pOleOutlineDoc->server.m_pszTreatAsType, NULL);

			pOleOutlineDoc->server.m_clsidTreatAs = clsid;
			pOleOutlineDoc->m_cfSaveFormat = cfFmt;
			pOleOutlineDoc->server.m_pszTreatAsType = lpszType;

		} else {
			// ERROR: we ONLY support TreatAs for 'OUTL' format
			OleStdFreeString(lpszType, NULL);
		}
	}
}

#endif // qOleTreatAs

#if qOleInPlace
//#pragma segment OleOutlineDocumentSeg
void OleOutlineDocDoDrag(DocumentPtr pDoc, EventRecord* theEvent, Boolean front)
{
	OleOutlineDocPtr	pOleOutlineDoc;
	Boolean				fUIVisible;
	Rect				rDrag;
	WindowPtr			pWindow;

	if (!StillDown())
		return;

	pOleOutlineDoc = (OleOutlineDocPtr)pDoc;

#if qOleInPlace

	ASSERTCOND(*pOleOutlineDoc->vtbl->m_GetDragBoundsProcPtr != nil);
	(*pOleOutlineDoc->vtbl->m_GetDragBoundsProcPtr)(pDoc, &rDrag);

#if qOleContainerApp

	ASSERTCOND(*pOleOutlineDoc->vtbl->m_DoDragProcPtr != nil);
	(*pOleOutlineDoc->vtbl->m_DoDragProcPtr)(pDoc, theEvent, false);

	if (!OleInPlaceContainerDocIsUIVisible(&pOleOutlineDoc->m_OleDoc))
	{
		ASSERTCOND(*pOleOutlineDoc->vtbl->m_BringToFrontProcPtr != nil);
		(*pOleOutlineDoc->vtbl->m_BringToFrontProcPtr)(pDoc);
	}

#elif qOleServerApp

	if (OleInPlaceServerDocIsUIVisible(&pOleOutlineDoc->m_OleDoc))
	{
		WindowPtr	pParentWin;

		if (OleWhichGrowHandle(pDoc->m_WindowPtr, theEvent->where) > 0)
		{
			ASSERTCOND(*pDoc->vtbl->m_DoGrowProcPtr != nil);
			(*pDoc->vtbl->m_DoGrowProcPtr)(pDoc, theEvent);
		}
		else
		{
			pParentWin = pOleOutlineDoc->m_OleDoc.server.inplace.m_pWindowParent;
			rDrag = (**((WindowPeek)pParentWin)->contRgn).rgnBBox;
			OleDragObjectWindow(pDoc->m_WindowPtr, theEvent->where, &rDrag, &rDrag,
								pOleOutlineDoc->m_OleDoc.server.inplace.m_FrameInfo.dragConstraint, NULL);
		}

		OleDocUpdateWindowPosition(&pOleOutlineDoc->m_OleDoc);
	}
	else
	{
		OleOutlineDocPtr	pCurDoc;

		ASSERTCOND(gApplication->vtbl->m_FindWinProcPtr != nil);
		pCurDoc = (OleOutlineDocPtr)(*gApplication->vtbl->m_FindWinProcPtr)(gApplication, FrontWindow());

		// if we're not the current doc and the current doc is uivisible, it's possible that
		// our structure region has been munged so that we appear to be behind the inplace
		// window. if so we don't want to initiate a drag or the munged scrucRgn will show.
		if ((pDoc != (DocumentPtr)pCurDoc) && OleInPlaceServerDocIsUIVisible(&pCurDoc->m_OleDoc))
			SelectWindow(pDoc->m_WindowPtr);
		else
		{
			ASSERTCOND(*pOleOutlineDoc->vtbl->m_DoDragProcPtr != nil);
			(*pOleOutlineDoc->vtbl->m_DoDragProcPtr)(pDoc, theEvent, false);
		}
	}

#endif	// qOleServerApp

	return;

#endif	// qOleInPlace

	ASSERTCOND(pOleOutlineDoc->vtbl->m_DoDragProcPtr != nil);
	(*pOleOutlineDoc->vtbl->m_DoDragProcPtr)(pDoc, theEvent, front);
}

//#pragma segment OleOutlineDocumentSeg
long OleOutlineDocDoGrowWindow(DocumentPtr pDoc, EventRecord* theEvent)
{
	OleOutlineDocPtr	pOleOutlineDoc;
	long				newSize;
	Rect				newRect;
	
	pOleOutlineDoc = (OleOutlineDocPtr)pDoc;
	
#if qOleInPlace

#if qOleContainerApp
	newSize = OleGrowParentWindow(pDoc->m_WindowPtr, theEvent->where, &pDoc->m_GrowSizeRect);
	if (newSize != 0)
		OleSizeParentWindow(pDoc->m_WindowPtr, LoWord(newSize), HiWord(newSize), true);
#elif qOleServerApp
	newSize = OleGrowObjectWindow(pDoc->m_WindowPtr, theEvent->where, &pDoc->m_GrowSizeRect, &newRect);
	if (newSize != 0)
		OleSizeObjectWindow(pDoc->m_WindowPtr, &newRect, true);
#endif

	if (newSize != 0)
	{
		// calling LineListTempSize stores the new size without moving the scrollbars. this
		// way the the server ends up being clipped appropriately but we don't move the
		// scrollbars until after calling SetObjectRects, at which point the scrollbars will
		// be visible in their new location.
		LineListTempSize(
				OutlineDocGetLineList((OutlineDocPtr)pOleOutlineDoc),
				(short)(LoWord(newSize) - kRowHeadingsWidth),
				(short)(HiWord(newSize) - kColumnHeadingsHeight));

		OleDocUpdateWindowPosition(&pOleOutlineDoc->m_OleDoc);
	}

#else	// !qOleInPlace

	ASSERTCOND(pOleOutlineDoc->vtbl->m_DoGrowWindowProcPtr != nil);
	newSize = (*pOleOutlineDoc->vtbl->m_DoGrowWindowProcPtr)(pDoc, theEvent);
	
#endif	// qOleInPlace

	return newSize;
}

//#pragma segment OleOutlineDocumentSeg
void OleOutlineDocDoZoomWindow(DocumentPtr pDoc, short partCode)
{
	OleOutlineDocPtr	pOleOutlineDoc;
	GrafPtr				oldPort;

	pOleOutlineDoc = (OleOutlineDocPtr)pDoc;

#if qOleContainerApp && qOleInPlace

	GetPort(&oldPort);
	SetPort(pDoc->m_WindowPtr);

	EraseRect(&pDoc->m_WindowPtr->portRect);
#if qFrameTools
	if (partCode == inZoomOut)
	{
		Rect	rBounds;
		Rect	rIndent;

		rBounds = qd.screenBits.bounds;
		rBounds.top += GetMBarHeight() + 18;

		ASSERTCOND(gApplication->vtbl->m_GetBorderSpaceProcPtr != nil);
		(gApplication->vtbl->m_GetBorderSpaceProcPtr)(gApplication, &rIndent);

		rBounds.left   += rIndent.left   + 2;
		rBounds.top    += rIndent.top    + 2;
		rBounds.right  -= rIndent.right  + 1;
		rBounds.bottom -= rIndent.bottom + 1;

		(*(WStateDataHandle)(((WindowPeek)pDoc->m_WindowPtr)->dataHandle))->stdState = rBounds;
	}
#endif

	OleZoomParentWindow(pDoc->m_WindowPtr, partCode, false);

	SetPort(oldPort);

	// calling LineListTempSize stores the new size without moving the scrollbars. this
	// way the the server ends up being clipped appropriately but we don't move the
	// scrollbars until after calling SetObjectRects, at which point the scrollbars will
	// be visible in their new location.
	LineListTempSize(
			OutlineDocGetLineList((OutlineDocPtr)pOleOutlineDoc),
			(short)(pDoc->m_WindowPtr->portRect.right - kRowHeadingsWidth),
			(short)(pDoc->m_WindowPtr->portRect.bottom - kColumnHeadingsHeight));

	OleDocUpdateWindowPosition(&pOleOutlineDoc->m_OleDoc);

#else

	ASSERTCOND(pOleOutlineDoc->vtbl->m_DoZoomWindowProcPtr != nil);
	(*pOleOutlineDoc->vtbl->m_DoZoomWindowProcPtr)(pDoc, partCode);

#endif
}

//#pragma segment OleOutlineDocumentSeg
void OleOutlineDocMoveWindow(DocumentPtr pDoc, short h, short v, Boolean front)
{
	OleOutlineDocPtr	pOleOutlineDoc;

	pOleOutlineDoc = (OleOutlineDocPtr)pDoc;

#if qOleInPlace

#if qOleContainerApp
	OleDocMoveWindow(&pOleOutlineDoc->m_OleDoc, h, v);
#endif

	OleMoveWindow(pDoc->m_WindowPtr, h, v, front);

	OleDocUpdateWindowPosition(&pOleOutlineDoc->m_OleDoc);
	return;

#endif	// qOleInPlace

	ASSERTCOND(pOleOutlineDoc->vtbl->m_MoveWindowProcPtr != nil);
	(*pOleOutlineDoc->vtbl->m_MoveWindowProcPtr)(pDoc, h, v, front);
}

//#pragma segment OleOutlineDocumentSeg
void OleOutlineDocScrollRect(DocumentPtr pDoc, short distHoriz, short distVert)
{
	OleOutlineDocPtr	pOleOutlineDoc;

	pOleOutlineDoc = (OleOutlineDocPtr)pDoc;

#if qOleContainerApp && qOleInPlace
	if (distHoriz != 0 || distVert != 0)
		OleOutlineContainerDocScrollRect(pOleOutlineDoc, distHoriz, distVert);
#endif

	ASSERTCOND(pOleOutlineDoc->vtbl->m_ScrollRectProcPtr != nil);
	(*pOleOutlineDoc->vtbl->m_ScrollRectProcPtr)(pDoc, distHoriz, distVert);
}
#endif


#endif // qOle
