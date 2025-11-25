/*****************************************************************************\
*                                                                             *
*    Name.c                                                                   *
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
#include "PObj.h"
#include "Line.h"
#include "Util.h"
#include "OleXcept.h"
#include "Const.h"
#include "App.h"
#include "Doc.h"
#if qFrameTools
	#include "Layers.h"
#endif
#include "ole2ui.h"
#include "OleDebug.h"
#include "stdio.h"
#include <TextUtils.h>

OLEDBGDATA

#ifdef __powerc
static ModalFilterUPP gNameTableDialogFilter = NULL;
#endif


// OLDNAME: Name.c

/* NameCreate
 * ----------
 *
 *		Create a name with the specified parameters
 */
NamePtr NameCreate(char* szText, short iFrom, short iTo)
{
	NamePtr pName;
	
	pName = (NamePtr)NewPtrClear(sizeof(NameRec));
	if (pName) {
		strcpy(pName->m_szText, szText);
		pName->m_nStartLine = iFrom;
		pName->m_nEndLine = iTo;
		
#if qOleServerApp
		pName->m_pPseudoObj = nil;
#endif
	}
	
	return pName;
}

/* NameDispose
 * -----------
 *
 *		Dispose a name and free up allocated memory
 */
void NameDispose(NamePtr pName)
{
	DisposePtr((Ptr)pName);
}

/* NameGetText
 * -----------
 *
 *		Get the string of a name
 */
char* NameGetText(NamePtr pName)
{
	return pName->m_szText;
}

/* NameSetText
 * -----------
 *
 *      Change the string of a name.
 */
void NameSetText(NamePtr pName, char* pszText)
{
	ASSERTCOND(strlen(pszText) <= kMaxNameLen);
	
    strcpy(pName->m_szText, pszText);
}


/* NameSetSel
 * ----------
 *
 *      Change the line range of a  name.
 */
void NameSetSel(NamePtr pName, LineRangePtr pLineRange, Boolean fRangeModified)
{
#if qOleServerApp

    Boolean fPseudoObjChanged = fRangeModified;

    if (pName->m_nStartLine != pLineRange->m_nStartLine) {
        pName->m_nStartLine = pLineRange->m_nStartLine;
        fPseudoObjChanged = TRUE;
    }

    if (pName->m_nEndLine != pLineRange->m_nEndLine) {
        pName->m_nEndLine = pLineRange->m_nEndLine;
        fPseudoObjChanged = TRUE;
    }

    /* OLE2NOTE: if the range of an active pseudo object has
    **    changed, then inform any linking clients that the object
    **    has changed.
    */
    if (pName->m_pPseudoObj && fPseudoObjChanged) {
        PseudoObjSendAdvise(
                pName->m_pPseudoObj,
				OLE_ONDATACHANGE,
				NULL,	/* lpmkDoc -- not relevant here */
				0		/* advf -- no flags necessary */
		);
    }

#else

    pName->m_nStartLine = pLineRange->m_nStartLine;
    pName->m_nEndLine = pLineRange->m_nEndLine;

#endif
}


/* NameGetSel
 * ----------
 *
 *      Retrieve the line range of a name.
 */
void NameGetSel(NamePtr pName, LineRangePtr pLineRange)
{
    pLineRange->m_nStartLine = pName->m_nStartLine;
    pLineRange->m_nEndLine = pName->m_nEndLine;
}


/* NameSaveToStg
 * -------------
 *
 *      Save a name into a storage
 */
HRESULT NameSaveToStg(NamePtr pName, LineRangePtr pLineRange, LPSTREAM pNTStm, Boolean* pfNameSaved)
{
    HRESULT hrErr = NOERROR;
    unsigned long nWritten;

    *pfNameSaved = FALSE;

    /* if no range given or if the name is completely within the range,
    **      write it out.
    */
    if (!pLineRange ||
        ((pLineRange->m_nStartLine <= pName->m_nStartLine) &&
        (pLineRange->m_nEndLine >= pName->m_nEndLine))) {

	    pName->m_nStartLine = SwapWord(pName->m_nStartLine);
	    pName->m_nEndLine = SwapWord(pName->m_nEndLine);

	    hrErr = pNTStm->lpVtbl->Write(
	            pNTStm,
	            pName->m_szText,
	            sizeof(pName->m_szText),
	            &nWritten
	    );
	    hrErr = pNTStm->lpVtbl->Write(
	            pNTStm,
	            &pName->m_nStartLine,
	            sizeof(pName->m_nStartLine),
	            &nWritten
	    );
	    hrErr = pNTStm->lpVtbl->Write(
	            pNTStm,
	            &pName->m_nEndLine,
	            sizeof(pName->m_nEndLine),
	            &nWritten
	    );

	    pName->m_nStartLine = SwapWord(pName->m_nStartLine);
	    pName->m_nEndLine = SwapWord(pName->m_nEndLine);

        *pfNameSaved = TRUE;
    }
    return hrErr;
}


/* NameLoadFromStg
 * ---------------
 *
 *      Load names from an open stream of a storage. if the name already
 * exits in the NameTable, it is NOT modified.
 *
 *      Returns TRUE is all ok, else FALSE.
 */
HRESULT NameLoadFromStg(NamePtr pName, LPSTREAM pNTStm)
{
    HRESULT hrErr = NOERROR;
    unsigned long nRead;

    hrErr = pNTStm->lpVtbl->Read(
            pNTStm,
            pName->m_szText,
            sizeof(pName->m_szText),
            &nRead
    );
    hrErr = pNTStm->lpVtbl->Read(
            pNTStm,
            &pName->m_nStartLine,
            sizeof(pName->m_nStartLine),
            &nRead
    );
    hrErr = pNTStm->lpVtbl->Read(
            pNTStm,
            &pName->m_nEndLine,
            sizeof(pName->m_nEndLine),
            &nRead
    );

    pName->m_nStartLine = SwapWord(pName->m_nStartLine);
    pName->m_nEndLine = SwapWord(pName->m_nEndLine);

#if qOleServerApp
	pName->m_pPseudoObj = nil;
#endif

    return hrErr;
}


// OLDNAME: NameTable.c

#define ENTERKEY			 3
#define RETURNKEY			13
#define ESCAPEKEY			27

extern ApplicationPtr	gApplication;

//#pragma segment NameTableSeg
void NameTableInit(NameTablePtr pNameTable, DocumentPtr pDoc)
{
	short cx;
	short cy;
	short 	iType;
	Handle 	iHandle;
	Rect 	iRect;
	Rect	rView;
	Rect	rcDataBounds;
	Point	ptSize;
#if qFrameTools
	WindowPtr	pSavedLayer;

	pSavedLayer = SwapLayer(gApplication->m_pRootLayer);
#endif

	pNameTable->m_Count = 0;
	pNameTable->m_pDoc = pDoc;
	
	pNameTable->m_NameDialogPtr = GetNewDialog(kName_DLOG, nil, (WindowPtr)-1);
	ASSERTCOND(pNameTable->m_NameDialogPtr != nil);
	FailNIL(pNameTable->m_NameDialogPtr);
	cx = (qd.screenBits.bounds.right  - pNameTable->m_NameDialogPtr->portRect.right)/2;
	cy = (qd.screenBits.bounds.bottom - pNameTable->m_NameDialogPtr->portRect.bottom)/3;
	MoveWindow(pNameTable->m_NameDialogPtr, cx, cy, false);
	SetWRefCon(pNameTable->m_NameDialogPtr, (long)pNameTable);
	
	GetDItem(pNameTable->m_NameDialogPtr, ID_UITEM_NAMELIST, &iType, &iHandle, &iRect);
	rView = iRect;
	InsetRect(&rView, 1, 1);
	rView.right -= 15;							//Add space for vertical scroll bar
	SetRect(&rcDataBounds, 0, 0, 2, 0);			//Two column listbox
	SetPt(&ptSize, (short)(rView.right - rView.left), 0);	// First column fills width of list (second isn't visible)
	pNameTable->m_hListbox = LNew(
							&rView,
							&rcDataBounds, 	
							ptSize,
							0,
							(WindowPtr)pNameTable->m_NameDialogPtr,
							true, 	// drawIt
							false,	// hasGrow
							false, 	// scrollHoriz
							true);	// scrollVert
	ASSERTCOND(pNameTable->m_hListbox != nil);
	FailNIL(pNameTable->m_hListbox);	
	(**pNameTable->m_hListbox).selFlags = lOnlyOne | lNoNilHilite;

#if qFrameTools
	SetLayer(pSavedLayer);
#endif
}

//#pragma segment NameTableSeg
void NameTableDispose(NameTablePtr pNameTable)
{
#if qFrameTools
	WindowPtr	pSavedLayer;

	pSavedLayer = SwapLayer(gApplication->m_pRootLayer);
#endif

	LDispose(pNameTable->m_hListbox);

	DisposDialog(pNameTable->m_NameDialogPtr);
	
	DisposePtr((Ptr)pNameTable);

#if qFrameTools
	SetLayer(pSavedLayer);
#endif
}

//#pragma segment NameTableSeg
void NameTableDoShowName(NameTablePtr pNameTable)
{
	short 	itemHit;
	Boolean done;
	Cell	cell;
	NamePtr	pName;
	short	len;
	short 	iType;
	Handle 	iHandle;
	Rect 	iRect;
	Str255	sText;
	GrafPtr	savePort;
	LineRangeRec	LineRange;
	LineListPtr	pLineList;
#if qFrameTools
	WindowPtr	pSavedLayer;

	pSavedLayer = SwapLayer(gApplication->m_pRootLayer);
#endif
	
	GetPort(&savePort);

	SetPort(pNameTable->m_NameDialogPtr);
	cell.h = cell.v = 0;
	if (LGetSelect(true, &cell, pNameTable->m_hListbox))
		LSetSelect(false, cell, pNameTable->m_hListbox);

	// clear the edit control
	GetDItem(pNameTable->m_NameDialogPtr, ID_TE_NAME, &iType, &iHandle, &iRect);
	SetIText(iHandle, "\p");

	pLineList = OutlineDocGetLineList((OutlineDocPtr)pNameTable->m_pDoc);
	if (LineListGetSelection(pLineList, &LineRange)) {
		GetDItem(pNameTable->m_NameDialogPtr, ID_TE_FROM, &iType, &iHandle, &iRect);
		NumToString(LineRange.m_nStartLine + 1, sText);
		SetIText(iHandle, sText);

		GetDItem(pNameTable->m_NameDialogPtr, ID_TE_TO, &iType, &iHandle, &iRect);
		NumToString(LineRange.m_nEndLine + 1, sText);
		SetIText(iHandle, sText);
	}	
	
#if qOle
	OleDocEnableDialog(&((OleOutlineDocPtr)pNameTable->m_pDoc)->m_OleDoc);
#endif
	
	SelectWindow(pNameTable->m_NameDialogPtr);
	ShowWindow(pNameTable->m_NameDialogPtr);

	done = false;
	
	do {
#ifndef __powerc
		ModalDialog(NameTableDialogFilter, &itemHit);
#else
		if (gNameTableDialogFilter == NULL)
			gNameTableDialogFilter =  NewModalFilterProc(NameTableDialogFilter);

		ModalDialog(gNameTableDialogFilter, &itemHit);
#endif
		
		switch (itemHit) {
			case ID_BTN_CLOSE:
				done = true;
				break;
				
			case ID_BTN_DEFINE:
			{
				long	iFrom;
				long	iTo;

				GetDItem(pNameTable->m_NameDialogPtr, ID_TE_FROM, &iType, &iHandle, &iRect);
				GetIText(iHandle, sText);
				if (!*sText) {
					SysBeep(50);
					break;
				}
				StringToNum(sText, &iFrom);
				iFrom --;

				GetDItem(pNameTable->m_NameDialogPtr, ID_TE_TO, &iType, &iHandle, &iRect);
				GetIText(iHandle, sText);
				if (!*sText) {
					SysBeep(50);
					break;
				}
				StringToNum(sText, &iTo);
				iTo--;
				
				// REVIEW: need more error checking
				if (iFrom > iTo) {
					SysBeep(50);
					break;
				}
				
				GetDItem(pNameTable->m_NameDialogPtr, ID_TE_NAME, &iType, &iHandle, &iRect);
				GetIText(iHandle, sText);
				
				if (!*sText || (*sText > kMaxNameLen)) {
					SysBeep(50);
					break;
				}
				p2cstr(sText);
				pName = NameTableFindName(pNameTable, (char*)sText);
				if (pName) {
					NameGetSel(pName, &LineRange);
					if (LineRange.m_nStartLine != iFrom ||
						LineRange.m_nEndLine != iTo)
					{
						LineRange.m_nStartLine = (short)iFrom;
						LineRange.m_nEndLine = (short)iTo;
						NameSetSel(pName, &LineRange, true);
					}
				}
				else {
					pName = (NamePtr)NewPtrClear(sizeof(NameRec));
					ASSERTCOND(pName != nil);
					
					pName = NameCreate((char*)sText, (short)iFrom, (short)iTo);
					ASSERTCOND(pName != nil);
					
					NameTableAddName(pNameTable, pName);
				}
				break;
			}
			
			case ID_BTN_DELETE:
			{
				cell.h = cell.v = 0;
				if (!LGetSelect(true, &cell, pNameTable->m_hListbox))
					break;
				NameTableDeleteName(pNameTable, cell.v);			
				break;
			}	
			
			case ID_BTN_GOTO:
			{
				cell.h = cell.v = 0;
				if (!LGetSelect(true, &cell, pNameTable->m_hListbox))
					break;
				pName = NameTableGetName(pNameTable, cell.v);
				ASSERTCOND(pName != nil);
				NameGetSel(pName, &LineRange);
				LineListSelectRange(pLineList, &LineRange);
				LineListShowIndexedLine(pLineList, LineRange.m_nStartLine);
				
				done = true;
				break;
			}
			
			case ID_UITEM_NAMELIST:
			{
				LineRangeRec LineRange;
				
				cell.h = cell.v = 0;
				if (!LGetSelect(true, &cell, pNameTable->m_hListbox))
					break;
					
				len = sizeof(NamePtr);
				cell.h++;
				LGetCell(&pName, &len, cell, pNameTable->m_hListbox);
				ASSERTCOND(len == sizeof (NamePtr));
				
				NameGetSel(pName, &LineRange);

				GetDItem(pNameTable->m_NameDialogPtr, ID_TE_FROM, &iType, &iHandle, &iRect);
				NumToString(LineRange.m_nStartLine + 1, sText);
				SetIText(iHandle, sText);

				GetDItem(pNameTable->m_NameDialogPtr, ID_TE_TO, &iType, &iHandle, &iRect);
				NumToString(LineRange.m_nEndLine + 1, sText);
				SetIText(iHandle, sText);
				
				GetDItem(pNameTable->m_NameDialogPtr, ID_TE_NAME, &iType, &iHandle, &iRect);
				*sText = sprintf((char*)&sText[1], "%s", NameGetText(pName));
				SetIText(iHandle, sText);
				SelIText(pNameTable->m_NameDialogPtr, ID_TE_NAME, 0, 32767);
				
				break;
			}				
		}
	} while (!done);
	
	HideWindow(pNameTable->m_NameDialogPtr);
	SetPort(savePort);

#if qOle
	OleDocDisableDialog(&((OleOutlineDocPtr)pNameTable->m_pDoc)->m_OleDoc, true);
#endif

#if qFrameTools
	SetLayer(pSavedLayer);
#endif
}

//#pragma segment NameTableSeg
/* NameTableAddName
 * ----------------
 *
 *      Add a name to the table. Assume that the name doesn't exist in the table.
 */
void NameTableAddName(NameTablePtr pNameTable, NamePtr pName)
{
	Cell 	cell;
	
	cell.h = 0;
	cell.v = 0;
	LAddRow(1, cell.v, pNameTable->m_hListbox);
	LSetCell((Ptr)NameGetText(pName), (short)strlen(NameGetText(pName)), cell,
			pNameTable->m_hListbox);
	cell.h++;
	LSetCell((Ptr)&pName, sizeof(NamePtr), cell, pNameTable->m_hListbox);

    pNameTable->m_Count++;

    ASSERTCOND(pNameTable->m_pDoc->vtbl->m_SetDirtyProcPtr != nil);
    (*pNameTable->m_pDoc->vtbl->m_SetDirtyProcPtr)(pNameTable->m_pDoc, true);
}


//#pragma segment NameTableSeg
/* NameTableDeleteName
 * -------------------
 *
 *      Delete a name from table
 */
void NameTableDeleteName(NameTablePtr pNameTable, int nIndex)
{
	Cell	cell;
	NamePtr	pName;
	short	len;
	
	ASSERTCOND(nIndex < pNameTable->m_Count);
	cell.h = 1;
	cell.v = nIndex;
	len = sizeof(NamePtr);
	LGetCell(&pName, &len, cell, pNameTable->m_hListbox);
	ASSERTCOND(len == sizeof (NamePtr));
	
#if qOleServerApp
    /* OLE2NOTE: if there is a pseudo object attached to this name, it
    **    must first be closed before deleting the Name. this will
    **    cause OnClose notification to be sent to all linking clients.
    */
    NameClosePseudoObj(pName);
#endif
	
	NameDispose(pName);
	LDelRow(1, (short)nIndex, pNameTable->m_hListbox);
	
    pNameTable->m_Count--;

    ASSERTCOND(pNameTable->m_pDoc->vtbl->m_SetDirtyProcPtr != nil);
    (*pNameTable->m_pDoc->vtbl->m_SetDirtyProcPtr)(pNameTable->m_pDoc, true);

}

//#pragma segment NameTableSeg
/* NameTableGotoName
 * -----------------
 *
 *		Make a selection on the document specified by the name
 */
void NameTableGotoName(NameTablePtr pNameTable, NamePtr pName)
{
	LineRangeRec	LineRange;
	LineListPtr		pLineList;
	
	pLineList = OutlineDocGetLineList((OutlineDocPtr)pNameTable->m_pDoc);
	NameGetSel(pName, &LineRange);
	HideWindow(pNameTable->m_NameDialogPtr);
	LineListSelectRange(pLineList, &LineRange);
	LineListShowIndexedLine(pLineList, LineRange.m_nStartLine);
}


//#pragma segment NameTableSeg
/* NameTableDialogFilter
 * ---------------------
 *
 *      Keep track of click in the listbox and key pressed
 */
#ifndef _MSC_VER
pascal Boolean
#else
Boolean __pascal
#endif
NameTableDialogFilter(DialogPtr theDialog, EventRecord *theEvent, short *itemHit)
{
	short			type;
	Handle			iHandle;
	Rect			iRect;
	GrafPtr			savePort;
	Point			p;
	char			theChar;
	short			in;
	WindowPtr		whichWindow;
	NameTablePtr	pNameTable = (NameTablePtr)GetWRefCon(theDialog);
	ApplicationPtr	pApp = gApplication;

	GetPort(&savePort);
	SetPort(theDialog);

	// case on the event
	switch (theEvent->what) {
		case updateEvt:
			whichWindow = (WindowPtr)theEvent->message;
			if (whichWindow == theDialog)
			{
				PenState	penState;
				
				// draw a frame for the Listbox	
				GetDItem(pNameTable->m_NameDialogPtr, ID_UITEM_NAMELIST, &type, &iHandle, &iRect);
				FrameRect(&iRect);
				LUpdate((*pNameTable->m_hListbox)->port->visRgn, pNameTable->m_hListbox);				

				GetPenState(&penState);
			  	PenSize(3,3);
				
				GetDItem(theDialog, 1, &type, &iHandle, &iRect);
				InsetRect(&iRect, -4, -4);
			  	FrameRoundRect(&iRect, 16, 16);

				SetPenState(&penState);
			}
			else {
				DocumentPtr		pDoc;
				
				// check if window belongs to a document
				ASSERTCOND(pApp->vtbl->m_FindDocProcPtr != nil);
				pDoc = (*pApp->vtbl->m_FindDocProcPtr)(pApp, whichWindow);
			
				BeginUpdate(whichWindow);
			
				if (pDoc != nil)
				{
					ASSERTCOND(pDoc->vtbl->m_DoUpdateProcPtr != nil);
					(*pDoc->vtbl->m_DoUpdateProcPtr)(pDoc);
				}
			
				EndUpdate(whichWindow);
			}

			break;	

		case autoKey:
		case keyDown:
			// if key was pressed, handle return key
			theChar = (theEvent->message) & charCodeMask;
			switch(theChar)
			{
				case RETURNKEY:			// <Return> key
				case ENTERKEY:			// <Enter> key
				case ESCAPEKEY:			// <Esc> key
					*itemHit = ID_BTN_CLOSE;
					FlashItem(theDialog, *itemHit, kDefFlashItemTime);
					SetPort(savePort);
					return true;
			}
			break;

		case mouseDown:
			in = FindWindow(theEvent->where, &whichWindow);
			if ((whichWindow == (WindowPtr)theDialog) && (in == inDrag))
			{
				DragWindow(theDialog, theEvent->where, &qd.screenBits.bounds);
				SetPort(savePort);
				return true;
			}

			// Get where mouse click occured in global coordinates.
			p = theEvent->where;

			// Convert the coordinates to local to the dialog window
			GlobalToLocal(&p);

			// Since I am only concerned with mouse clicks in the user item,
			// get information for that item.
			switch (FindDItem(theDialog, p) + 1) {
				case ID_UITEM_NAMELIST:
					*itemHit = ID_UITEM_NAMELIST;
					LClick(p, theEvent->modifiers, pNameTable->m_hListbox);
					SetPort(savePort);
					return true;
			}
			
			break;
	}
	
	SetPort(savePort);

	return false;
}

//#pragma segment NameTableSeg
/* NameTableGetNameIndex
 * ---------------------
 *
 *      Return the index of the Name given a pointer to the Name.
 *      Return -1 if the Name is not found.
 */
short NameTableGetNameIndex(NameTablePtr pNameTable, NamePtr pName)
{
	Cell	cell;
	NamePtr	pTestName;
	short	dataLen;
	
	cell.h = 1;
	for (cell.v = 0; cell.v < pNameTable->m_Count; cell.v++) {
		dataLen = sizeof(NamePtr);
		LGetCell(&pTestName, &dataLen, cell, pNameTable->m_hListbox);
		ASSERTCOND(dataLen == sizeof(NamePtr));
		if (pTestName == pName)
			break;
	}
	
	return (cell.v < pNameTable->m_Count ? cell.v : -1);
}


//#pragma segment NameTableSeg
/* NameTableGetName
 * ----------------
 *
 *      Retrieve the pointer to the Name given its index in the NameTable
 */
NamePtr NameTableGetName(NameTablePtr pNameTable, short nIndex)
{
    NamePtr	pName;
    Cell	cell;
    short 	dataLen;

    if (!pNameTable->m_Count ||   nIndex >= pNameTable->m_Count || nIndex < 0) {
        return nil;
    }

	cell.h = 1;
	cell.v = nIndex;
	dataLen = sizeof(NamePtr);
	LGetCell(&pName, &dataLen, cell, pNameTable->m_hListbox);
	
    return (dataLen == sizeof(NamePtr) ? pName : nil);
}


//#pragma segment NameTableSeg
/* NameTableFindName
 * -----------------
 *
 *      Find a name in the name table given a string.
 */
NamePtr NameTableFindName(NameTablePtr pNameTable, char* pszName)
{
    NamePtr	pTestName;
    Cell	cell;
    short	dataLen;
	
	cell.h = 1;
	for (cell.v = 0; cell.v < pNameTable->m_Count; cell.v++) {
		dataLen = sizeof(NamePtr);
		LGetCell(&pTestName, &dataLen, cell, pNameTable->m_hListbox);
		ASSERTCOND(dataLen == sizeof(NamePtr));
		if (!strcmp(NameGetText(pTestName), pszName))
			break;
	}

	return (cell.v < pNameTable->m_Count ? pTestName : nil);
}


//#pragma segment NameTableSeg
/* NameTableFindNamedRange
 * -----------------------
 *
 *      Find a name in the name table which matches a given line range.
 */
NamePtr NameTableFindNamedRange(NameTablePtr pNameTable, LineRangePtr pLineRange)
{
    NamePtr	pTestName;
    Cell	cell;
    short	dataLen;
    LineRangeRec	lrTest;
	
	cell.h = 1;
	for (cell.v = 0; cell.v < pNameTable->m_Count; cell.v++) {
		dataLen = sizeof(NamePtr);
		LGetCell(&pTestName, &dataLen, cell, pNameTable->m_hListbox);
		ASSERTCOND(dataLen == sizeof(NamePtr));
		NameGetSel(pTestName, &lrTest);
		if ((lrTest.m_nStartLine == pLineRange->m_nStartLine) &&
			(lrTest.m_nEndLine == pLineRange->m_nEndLine))
			break;
	}

	return (cell.v < pNameTable->m_Count ? pTestName : nil);
}


//#pragma segment NameTableSeg
/* NameTableGetCount
 * -----------------
 *
 * Return number of names in nametable
 */
short NameTableGetCount(NameTablePtr pNameTable)
{
    return pNameTable ? pNameTable->m_Count : 0;
}


//#pragma segment NameTableSeg
/* NameTableClearAll
 * -----------------
 *
 *      Remove all names from table
 */
void NameTableClearAll(NameTablePtr pNameTable)
{
    int 		i;
	int 		nCount = pNameTable->m_Count;

    for (i = 0; i < nCount; i++)
		NameTableDeleteName(pNameTable, 0);
}


//#pragma segment NameTableSeg
/* NameTableAddLineUpdate
 * ----------------------
 *
 *      Update table when a new line is added at nAddIndex
 * The line used to be at nAddIndex is pushed down
 */
void NameTableAddLineUpdate(NameTablePtr pNameTable, short nAddIndex)
{
    NamePtr			pName;
    LineRangeRec 	lrSel;
    Boolean 		fRangeModified;
    int				i;

    for(i = 0; i < pNameTable->m_Count; i++) {
        pName = NameTableGetName(pNameTable, (short)i);
        ASSERTCOND(pName != nil);
        NameGetSel(pName, &lrSel);
		fRangeModified = false;
        if(lrSel.m_nStartLine > nAddIndex) {
            lrSel.m_nStartLine++;
            fRangeModified = !fRangeModified;
        }
        if(lrSel.m_nEndLine > nAddIndex) {
            lrSel.m_nEndLine++;
            fRangeModified = !fRangeModified;
        }
        NameSetSel(pName, &lrSel, fRangeModified);
    }
}


//#pragma segment NameTableSeg
/* NameTableDeleteLineUpdate
 * -------------------------
 *
 *      Update the table when a line at nDeleteIndex is removed
 */
void NameTableDeleteLineUpdate(NameTablePtr pNameTable, short nDeleteIndex)
{
    NamePtr			pName;
    LineRangeRec 	lrSel;
    int 			i;
    Boolean 		fRangeModified;

    for(i = 0; i < pNameTable->m_Count; i++) {
        pName = NameTableGetName(pNameTable, (short)i);
        ASSERTCOND(pName != nil);
        NameGetSel(pName, &lrSel);

        if(lrSel.m_nStartLine > nDeleteIndex) {
            lrSel.m_nStartLine--;
            fRangeModified = !fRangeModified;
        }
        if(lrSel.m_nEndLine >= nDeleteIndex) {
            lrSel.m_nEndLine--;
            fRangeModified = !fRangeModified;
        }

        // delete the name if its entire range is deleted
        if(lrSel.m_nStartLine > lrSel.m_nEndLine) {
            NameTableDeleteName(pNameTable, i);
            i--;  // re-examine this name
        } else {
            NameSetSel(pName, &lrSel, fRangeModified);
        }
    }
}

#if qOle

//#pragma segment NameTableSeg
/* NameTableSaveSelectonToStorage
 * ------------------------------
 *
 *      Save only the names that refer to lines completely contained in the
 * specified selection range.
 */
void NameTableSaveSelectionToStorage(NameTablePtr pNameTable, LineRangePtr pSelection, LPSTORAGE lpDestStg)
{
	HRESULT				hrErr;
	volatile LPSTREAM	pNameTableStm = nil;
    unsigned long 		nWritten;
    NamePtr 			pName;
    short				nNameCount = 0;
    Boolean 			fNameSaved;
    int					i;
    LARGE_INTEGER		dlibZeroOffset;

    LISet32( dlibZeroOffset, 0 );

	TRY
	{
		hrErr = lpDestStg->lpVtbl->CreateStream(
					lpDestStg,
					kNameTableStreamName,
					STGM_WRITE | STGM_SHARE_EXCLUSIVE | STGM_CREATE,
					0,
					0,
					(LPSTREAM *)&pNameTableStm
				);
		ASSERTCOND(hrErr == nil);
		FailOleErr(hrErr);

	    /* initially write 0 for count of names. the correct count will be
	    **    written at the end when we know how many names qualified to
	    **    be written (within the selection).
	    */
	    hrErr = pNameTableStm->lpVtbl->Write(
	            pNameTableStm,
	            &nNameCount,
	            sizeof(nNameCount),
	            &nWritten
	    );
	   	ASSERTNOERROR(hrErr);
		FailOleErr(hrErr);
		
	    for(i = 0; i < pNameTable->m_Count; i++) {
	        pName = NameTableGetName(pNameTable, (short)i);
	        ASSERTCOND(pName != nil);
	        hrErr = NameSaveToStg(
	                pName,
	                pSelection,
	                pNameTableStm,
	                &fNameSaved
	        );
		   	ASSERTNOERROR(hrErr);
			FailOleErr(hrErr);
	        if (fNameSaved)
	        	nNameCount++;
	    }
	
	    /* write the final count of names written. */
	    hrErr = pNameTableStm->lpVtbl->Seek(
	            pNameTableStm,
	            dlibZeroOffset,
	            STREAM_SEEK_SET,
	            NULL
	    );
	   	ASSERTNOERROR(hrErr);
		FailOleErr(hrErr);
	
		nNameCount = SwapWord(nNameCount);

	    hrErr = pNameTableStm->lpVtbl->Write(
	            pNameTableStm,
	            &nNameCount,
	            sizeof(nNameCount),
	            &nWritten
	    );
	   	ASSERTNOERROR(hrErr);
		FailOleErr(hrErr);
	
		nNameCount = SwapWord(nNameCount);
	
	    OleStdRelease((LPUNKNOWN)pNameTableStm);
    }
	CATCH
	{
	    if (pNameTableStm)
	        OleStdRelease((LPUNKNOWN)pNameTableStm);
    	
	}
	ENDTRY;
}


//#pragma segment NameTableSeg
/* NameTableLoadFromStorage
 * ------------------------
 *
 *      Load Name Table from file
 *
 *      Return TRUE if ok, FALSE if error
 */
void NameTableLoadFromStorage(NameTablePtr pNameTable, LPSTORAGE pSrcStg)
{
    HRESULT 			hrErr;
    volatile IStream*	pNameTableStm;
    unsigned long 		nRead;
    short 				nCount;
    NamePtr 			pName;
    int 				i;

	TRY
	{
	    hrErr = pSrcStg->lpVtbl->OpenStream(
	            pSrcStg,
	            "NameTable",
	            NULL,
	            STGM_READ | STGM_SHARE_EXCLUSIVE,
	            0,
	            (LPSTREAM *)&pNameTableStm);
	   	ASSERTNOERROR(hrErr);
		FailOleErr(hrErr);
	
	    hrErr = pNameTableStm->lpVtbl->Read((LPSTREAM)pNameTableStm, &nCount, sizeof(nCount), &nRead);
	   	ASSERTNOERROR(hrErr);
		FailOleErr(hrErr);

		nCount = SwapWord(nCount);

	    for (i = 0; i < nCount; i++) {
	        pName = NameCreate("", 0, 0);	// empty name
	        ASSERTCOND(pName != nil);
	        FailNIL(pName);
	        hrErr = NameLoadFromStg(pName, (LPSTREAM)pNameTableStm);
		   	ASSERTNOERROR(hrErr);
			FailOleErr(hrErr);
	        NameTableAddName(pNameTable, pName);
	    }
	
	    OleStdRelease((LPUNKNOWN)pNameTableStm);
	    pNameTableStm = nil;
    }
	CATCH
	{
	    if (pNameTableStm)
	        OleStdRelease((LPUNKNOWN)pNameTableStm);
	
    }
    ENDTRY
}

#endif // qOle

#if qOleServerApp

// OLDNAME: OleName.c
/* NameSendPendingAdvises
 * ----------------------
 *
 *      Send any pending change notifications for the associated
 *  pseudo objects for this name (if one exists).
 *  while ReDraw is diabled on the ServerDoc, then change advise
 *  notifications are not sent to pseudo object clients.
 */
void NameSendPendingAdvises(NamePtr pName)
{
    if (pName->m_pPseudoObj && pName->m_pPseudoObj->m_fDataChanged)
        PseudoObjSendAdvise(
                pName->m_pPseudoObj,
				OLE_ONDATACHANGE,
				NULL,	/* lpmkDoc -- not relevant here */
				0	/* advf -- no flags necessary */
		);
}


/* NameGetPseudoObj
** ----------------
**
**    Return a pointer to a pseudo object associated to a ServerName.
**    if the pseudo object already exists, then return the
**    existing object, otherwise allocate a new pseudo object.
**
**    NOTE: the PseudoObj is returned with a 0 refcnt if first created,
**    else the existing refcnt is unchanged.
*/
PseudoObjPtr NameGetPseudoObj(
        NamePtr            pName,
        OleDocumentPtr     pOleDoc
)
{
    // Check if a PseudoObj already exists
    if (pName->m_pPseudoObj)
        return pName->m_pPseudoObj;

    // A PseudoObj does NOT already exist, allocate a new one.
    pName->m_pPseudoObj = (PseudoObjPtr) NewPtrClear(sizeof(PseudoObjRec));
    ASSERTCOND(pName->m_pPseudoObj != nil);
	if (!pName->m_pPseudoObj)
		return nil;
		
    PseudoObjInit(pName->m_pPseudoObj, pName, pOleDoc);
    return pName->m_pPseudoObj;
}


/* NameClosePseudoObj
 * ------------------
 *
 *      if there is an associated pseudo objects for this name (if one
 *  exists), then close it. this results in sending OnClose
 *  notification to the pseudo object's linking clients.
 */
void NameClosePseudoObj(NamePtr pName)
{
    if (! pName->m_pPseudoObj)
        return;     // no associated pseudo object

    PseudoObjClose(pName->m_pPseudoObj);
}

// OLDNAME: OleNameTable.c

/* NameTableEditLineUpdate
 * -----------------------
 *
 *      Update the table when a line at nEditIndex is edited.
 */
void NameTableEditLineUpdate(
        NameTablePtr	pNameTable,
        int             nEditIndex
)
{
    NamePtr			pName;
    LineRangeRec 	lrSel;
    PseudoObjPtr	pPseudoObj;
    int 			i;

    for(i = 0; i < pNameTable->m_Count; i++) {
        pName = NameTableGetName(pNameTable, (short)i);
        ASSERTCOND(pName != nil);

        pPseudoObj = pName->m_pPseudoObj;

        /* if there is a pseudo object associated with this name, then
        **    check if the line that was modified is included within
        **    the named range.
        */
        if (pPseudoObj) {
            NameGetSel(pName, &lrSel);

            if(((int)lrSel.m_nStartLine <= nEditIndex) &&
                ((int)lrSel.m_nEndLine >= nEditIndex)) {

                // inform linking clients data has changed
                PseudoObjSendAdvise(
						pPseudoObj,
						OLE_ONDATACHANGE,
						NULL,	/* lpmkDoc -- not relevant here */
						0	/* advf -- no flags necessary */
				);
            }

        }
    }
}


/* NameTableInformAllPseudoObjectsDocRenamed
 * -----------------------------------------
 *
 *      Inform all pseudo object clients that the name of the pseudo
 *      object has changed.
 */
void NameTableInformAllPseudoObjectsDocRenamed(
        NameTablePtr	pNameTable,
        LPMONIKER       pmkDoc
)
{
    NamePtr			pName;
    PseudoObjPtr 	pPseudoObj;
    LPMONIKER 		pmkObj;
    int 			i;

    for(i = 0; i < pNameTable->m_Count; i++) {
        pName = NameTableGetName(pNameTable, (short)i);
        ASSERTCOND(pName != nil);

        pPseudoObj = pName->m_pPseudoObj;

        /* if there is a pseudo object associated with this name, then
        **    send OnRename advise to its linking clients.
        */
		if (pPseudoObj && ((pmkObj = PseudoObjGetFullMoniker(pPseudoObj, pmkDoc)) != NULL)) {
            // inform the clients that the name has changed
            PseudoObjSendAdvise (
					pPseudoObj,
					OLE_ONRENAME,
					pmkObj,
					0		/* advf -- not relevant here */
			);
        }
    }
    OLEDBG_END2
}


/* NameTableInformAllPseudoObjectsDocSaved
 * ---------------------------------------
 *
 *      Inform all pseudo object clients that the name of the pseudo
 *      object has changed.
 */
void NameTableInformAllPseudoObjectsDocSaved(
        NameTablePtr		pNameTable,
        LPMONIKER           pmkDoc
)
{
	NamePtr			pName;
	PseudoObjPtr	pPseudoObj;
    LPMONIKER		pmkObj;
    int 			i;

    for(i = 0; i < pNameTable->m_Count; i++) {
        pName = NameTableGetName(pNameTable, (short)i);
        ASSERTCOND(pName != nil);

        pPseudoObj = pName->m_pPseudoObj;

        /* if there is a pseudo object associated with this name, then
        **    send OnSave advise to its linking clients.
        */
        if (pPseudoObj && ((pmkObj=PseudoObjGetFullMoniker(pPseudoObj,pmkDoc))!=NULL)) {

            // inform the clients that the name has been saved
            PseudoObjSendAdvise (
					pPseudoObj,
					OLE_ONSAVE,
					NULL,	/* lpmkDoc -- not relevant here */
					0	/* advf -- not relevant here */
			);
        }
    }
    OLEDBG_END2
}


/* NameTableSendPendingAdvises
 * ---------------------------
 *
 *      Send any pending change notifications for pseudo objects.
 *  while ReDraw is diabled on the ServerDoc, then change advise
 *  notifications are not sent to pseudo object clients.
 */
void NameTableSendPendingAdvises(NameTablePtr pNameTable)
{
    NamePtr 	pName;
    int i;

    for(i = 0; i < pNameTable->m_Count; i++) {
        pName = NameTableGetName(pNameTable, (short)i);
        ASSERTCOND(pName != nil);
        NameSendPendingAdvises(pName);
    }
}


/* NameTableGetPseudoObj
** ---------------------
**
**    Return a pointer to a pseudo object identified by an item string
**    (lpszItem). if the pseudo object already exists, then return the
**    existing object, otherwise allocate a new pseudo object.
*/
PseudoObjPtr NameTableGetPseudoObj(
        NameTablePtr			pNameTable,
        char*                   pszItem,
        OleDocumentPtr			pOleDoc
)
{
	NamePtr		pName;
	
    pName = NameTableFindName(pNameTable, pszItem);

    if (pName)
        return NameGetPseudoObj(pName, pOleDoc);
    else
        return nil;
}


/* NameTableCloseAllPseudoObjs
 * ---------------------------
 *
 *  Force all pseudo objects to close. this results in sending OnClose
 *  notification to each pseudo object's linking clients.
 */
void NameTableCloseAllPseudoObjs(NameTablePtr pNameTable)
{
	NamePtr		pName;
	int i;

    for(i = 0; i < pNameTable->m_Count; i++) {
        pName = NameTableGetName(pNameTable, (short)i);
        ASSERTCOND(pName != nil);
        NameClosePseudoObj(pName);
    }
}


// OLDNAME: OlePseudoObjInterface.c
static IUnknownVtbl			gPseudoObjIUnknownVtbl;
static IOleObjectVtbl		gPseudoObjIOleObjectVtbl;
static IDataObjectVtbl		gPseudoObjIDataObjectVtbl;

#ifndef _MSC_VER
#define STGMEDIUM_HGLOBAL	u.hGlobal
#else
#define STGMEDIUM_HGLOBAL	hGlobal
#endif

//#pragma segment OlePseudoObjInterfaceInitSeg
void OlePseudoObjInitInterfaces(void)
{
	// PseudoObj::IUnknown method table
	{
		IUnknownVtbl*		p;

		p = &gPseudoObjIUnknownVtbl;

		p->QueryInterface		= IPseudoObjUnknownQueryInterface;
		p->AddRef				= IPseudoObjUnknownAddRef;
		p->Release				= IPseudoObjUnknownRelease;

		ASSERTCOND(ValidVtbl(((char *)p) + sizeof(p->b), sizeof(*p) - sizeof(p->b)));
	}

	// PseudoObj::IOleObject method table
	{
		IOleObjectVtbl*		p;

		p = &gPseudoObjIOleObjectVtbl;

		p->QueryInterface		= IPseudoObjOleObjectQueryInterface;
		p->AddRef				= IPseudoObjOleObjectAddRef;
		p->Release				= IPseudoObjOleObjectRelease;
		p->SetClientSite		= IPseudoObjOleObjectSetClientSite;
		p->GetClientSite		= IPseudoObjOleObjectGetClientSite;
		p->SetHostNames			= IPseudoObjOleObjectSetHostNames;
		p->Close				= IPseudoObjOleObjectClose;
		p->SetMoniker			= IPseudoObjOleObjectSetMoniker;
		p->GetMoniker			= IPseudoObjOleObjectGetMoniker;
		p->InitFromData			= IPseudoObjOleObjectInitFromData;
		p->GetClipboardData		= IPseudoObjOleObjectGetClipboardData;
		p->DoVerb				= IPseudoObjOleObjectDoVerb;
		p->EnumVerbs			= IPseudoObjOleObjectEnumVerbs;
		p->Update				= IPseudoObjOleObjectUpdate;
		p->IsUpToDate			= IPseudoObjOleObjectIsUpToDate;
		p->GetUserClassID		= IPseudoObjOleObjectGetUserClassID;
		p->GetUserType			= IPseudoObjOleObjectGetUserType;
		p->SetExtent			= IPseudoObjOleObjectSetExtent;
		p->GetExtent			= IPseudoObjOleObjectGetExtent;
		p->Advise				= IPseudoObjOleObjectAdvise;
		p->Unadvise				= IPseudoObjOleObjectUnadvise;
		p->EnumAdvise			= IPseudoObjOleObjectEnumAdvise;
		p->GetMiscStatus		= IPseudoObjOleObjectGetMiscStatus;
		p->SetColorScheme		= IPseudoObjOleObjectSetColorScheme;

		ASSERTCOND(ValidVtbl(((char *)p) + sizeof(p->b), sizeof(*p) - sizeof(p->b)));
	}

	// PseudoObj::IDataObject method table
	{
		IDataObjectVtbl*	p;

		p = &gPseudoObjIDataObjectVtbl;

		p->QueryInterface			= IPseudoObjDataObjectQueryInterface;
		p->AddRef					= IPseudoObjDataObjectAddRef;
		p->Release					= IPseudoObjDataObjectRelease;
		p->GetData					= IPseudoObjDataObjectGetData;
		p->GetDataHere				= IPseudoObjDataObjectGetDataHere;
		p->QueryGetData				= IPseudoObjDataObjectQueryGetData;
		p->GetCanonicalFormatEtc	= IPseudoObjDataObjectGetCanonicalFormatEtc;
		p->SetData					= IPseudoObjDataObjectSetData;
		p->EnumFormatEtc			= IPseudoObjDataObjectEnumFormatEtc;
		p->DAdvise					= IPseudoObjDataObjectAdvise;
		p->DUnadvise				= IPseudoObjDataObjectUnadvise;
		p->EnumDAdvise				= IPseudoObjDataObjectEnumAdvise;

		ASSERTCOND(ValidVtbl(((char *)p) + sizeof(p->b), sizeof(*p) - sizeof(p->b)));
	}
}

//#pragma segment OlePseudoObjInterfaceSeg
void PseudoObjIUnknownInit(PseudoObjUnknownImplPtr pPseudoObjUnknownImpl, PseudoObjPtr pPseudoObj)
{
	pPseudoObjUnknownImpl->lpVtbl		= &gPseudoObjIUnknownVtbl;
	pPseudoObjUnknownImpl->lpPseudoObj	= pPseudoObj;
	pPseudoObjUnknownImpl->cRef			= 0;
}

//#pragma segment OlePseudoObjInterfaceSeg
STDMETHODIMP IPseudoObjUnknownQueryInterface(LPUNKNOWN lpThis, REFIID riid, void* * lplpvObj)
{
	PseudoObjPtr		pPseudoObj;

	OleDbgEnterInterface();

	pPseudoObj = ((PseudoObjUnknownImplPtr)lpThis)->lpPseudoObj;

	return PseudoObjQueryInterface(pPseudoObj, riid, lplpvObj);
}

//#pragma segment OlePseudoObjInterfaceSeg
STDMETHODIMP_(unsigned long) IPseudoObjUnknownAddRef(LPUNKNOWN lpThis)
{
	PseudoObjPtr		pPseudoObj;

	OleDbgEnterInterface();

	pPseudoObj = ((PseudoObjUnknownImplPtr)lpThis)->lpPseudoObj;

	return PseudoObjAddRef(pPseudoObj);
}

//#pragma segment OlePseudoObjInterfaceSeg
STDMETHODIMP_(unsigned long) IPseudoObjUnknownRelease(LPUNKNOWN lpThis)
{
	PseudoObjPtr		pPseudoObj;

	OleDbgEnterInterface();

	pPseudoObj = ((PseudoObjUnknownImplPtr)lpThis)->lpPseudoObj;

	return PseudoObjRelease(pPseudoObj);
}

//#pragma segment OlePseudoObjInterfaceSeg
void PseudoObjIOleObjectInit(PseudoObjOleObjectImplPtr pPseudoObjOleObjectImpl, PseudoObjPtr pPseudoObj)
{
	pPseudoObjOleObjectImpl->lpVtbl			= &gPseudoObjIOleObjectVtbl;
	pPseudoObjOleObjectImpl->lpPseudoObj	= pPseudoObj;
	pPseudoObjOleObjectImpl->cRef			= 0;
}

//#pragma segment OlePseudoObjInterfaceSeg
STDMETHODIMP IPseudoObjOleObjectQueryInterface(LPOLEOBJECT lpThis, REFIID riid, void* * lplpvObj)
{
	PseudoObjPtr		pPseudoObj;

	OleDbgEnterInterface();

	pPseudoObj = ((PseudoObjOleObjectImplPtr)lpThis)->lpPseudoObj;

	return PseudoObjQueryInterface(pPseudoObj, riid, lplpvObj);
}

//#pragma segment OlePseudoObjInterfaceSeg
STDMETHODIMP_(unsigned long) IPseudoObjOleObjectAddRef(LPOLEOBJECT lpThis)
{
	PseudoObjPtr		pPseudoObj;

	OleDbgEnterInterface();

	pPseudoObj = ((PseudoObjOleObjectImplPtr)lpThis)->lpPseudoObj;

	return PseudoObjAddRef(pPseudoObj);
}

//#pragma segment OlePseudoObjInterfaceSeg
STDMETHODIMP_(unsigned long) IPseudoObjOleObjectRelease(LPOLEOBJECT lpThis)
{
	PseudoObjPtr		pPseudoObj;

	OleDbgEnterInterface();

	pPseudoObj = ((PseudoObjOleObjectImplPtr)lpThis)->lpPseudoObj;

	return PseudoObjRelease(pPseudoObj);
}

//#pragma segment OlePseudoObjInterfaceSeg
STDMETHODIMP IPseudoObjOleObjectSetClientSite(LPOLEOBJECT lpThis, LPOLECLIENTSITE lpclientSite)
{
	OleDbgEnterInterface();

    // OLE2NOTE: a pseudo object does NOT support SetClientSite
	return ResultFromScode(E_FAIL);
}

//#pragma segment OlePseudoObjInterfaceSeg
STDMETHODIMP IPseudoObjOleObjectGetClientSite(LPOLEOBJECT lpThis, LPOLECLIENTSITE * lplpClientSite)
{
	OleDbgEnterInterface();

    *lplpClientSite = NULL;

    // OLE2NOTE: a pseudo object does NOT support GetClientSite
	return ResultFromScode(E_FAIL);
}

//#pragma segment OlePseudoObjInterfaceSeg
STDMETHODIMP IPseudoObjOleObjectSetHostNames(LPOLEOBJECT lpThis, const char* szContainerApp, const char* szContainerObj)
{
	OleDbgEnterInterface();

    // OLE2NOTE: a pseudo object does NOT support SetHostNames
    return ResultFromScode(E_FAIL);
}

//#pragma segment OlePseudoObjInterfaceSeg
STDMETHODIMP IPseudoObjOleObjectClose(LPOLEOBJECT lpThis, unsigned long dwSaveOption)
{
    PseudoObjPtr pPseudoObj = ((PseudoObjOleObjectImplPtr)lpThis)->lpPseudoObj;
    Boolean fStatus;

	OleDbgEnterInterface();

    /* OLE2NOTE: a pseudo object's implementation of IOleObject::Close
    **    should ignore the dwSaveOption parameter. it is NOT
    **    applicable to pseudo objects.
    */
    fStatus = PseudoObjClose(pPseudoObj);
    ASSERTCOND(fStatus == true);

    return fStatus ? NOERROR : ResultFromScode(E_FAIL);
}

//#pragma segment OlePseudoObjInterfaceSeg
STDMETHODIMP IPseudoObjOleObjectSetMoniker(LPOLEOBJECT lpThis, unsigned long dwWhichMoniker, LPMONIKER lpmk)
{
	OleDbgEnterInterface();

    // OLE2NOTE: a pseudo object does NOT support SetMoniker
    return ResultFromScode(E_FAIL);
}

//#pragma segment OlePseudoObjInterfaceSeg
STDMETHODIMP IPseudoObjOleObjectGetMoniker(LPOLEOBJECT lpThis, unsigned long dwAssign, unsigned long dwWhichMoniker, LPMONIKER * ppmk)
{
    PseudoObjPtr 	pPseudoObj = ((PseudoObjOleObjectImplPtr)lpThis)->lpPseudoObj;
    OleDocumentPtr 	pOleDoc = pPseudoObj->m_lpOleDoc;
    LPMONIKER 		pmkDoc;

	OleDbgEnterInterface();

    pmkDoc = OleDocGetFullMoniker(pOleDoc, OLEGETMONIKER_ONLYIFTHERE);
    *ppmk = PseudoObjGetFullMoniker(pPseudoObj, pmkDoc);

	return *ppmk ? NOERROR : ResultFromScode(E_FAIL);
}

//#pragma segment OlePseudoObjInterfaceSeg
STDMETHODIMP IPseudoObjOleObjectInitFromData(LPOLEOBJECT lpThis, LPDATAOBJECT lpDataObject, unsigned long fCreation, unsigned long reserved)
{
	OleDbgEnterInterface();

    // REVIEW: NOT YET IMPLEMENTED
    return ResultFromScode(E_NOTIMPL);
}

//#pragma segment OlePseudoObjInterfaceSeg
STDMETHODIMP IPseudoObjOleObjectGetClipboardData(LPOLEOBJECT lpThis, unsigned long reserved, LPDATAOBJECT * lplpDataObject)
{
	OleDbgEnterInterface();

    // REVIEW: NOT YET IMPLEMENTED
    return ResultFromScode(E_NOTIMPL);
}

//#pragma segment OlePseudoObjInterfaceSeg
STDMETHODIMP IPseudoObjOleObjectDoVerb(
		LPOLEOBJECT 	lpThis,
		long 			lVerb,
		EventRecord* 	lpmsg,
		LPOLECLIENTSITE lpActiveSite,
		long 			lindex,
		WindowPtr 		hwndParent,
		LPCRECT 		lprcPosRect
)
{
    PseudoObjPtr pPseudoObj = ((PseudoObjOleObjectImplPtr)lpThis)->lpPseudoObj;
    OleDocumentPtr pOleDoc = pPseudoObj->m_lpOleDoc;
    OleServerDocPtr pServerDoc = &pOleDoc->server;
	HRESULT hrErr;

	OleDbgEnterInterface();

	/* OLE2NOTE: we must first ask our Document to perform the same
	**    verb. then if the verb is NOT OLEIVERB_HIDE we should also
	**    select the range of our pseudo object.
	**    however, we must give our document its own embedding site as
	**    its active site.
	*/
	hrErr = OleServerDocOleObjectDoVerb(
			pOleDoc,
			lVerb,
			lpmsg,
			pServerDoc->m_lpOleClientSite,
			lindex,
			NULL,	/* we have no hwndParent to give */
			NULL	/* we have no lprcPosRect to give */
	);
	if (hrErr != NOERROR ) {
		return hrErr;
	}

	if (lVerb != OLEIVERB_HIDE) {
		ASSERTCOND(pOleDoc->m_pIDoc->lpVtbl->m_SelectItemProcPtr != nil);
		(pOleDoc->m_pIDoc->lpVtbl->m_SelectItemProcPtr)(pOleDoc->m_pIDoc, pPseudoObj->m_lpItem);
	}

	return NOERROR;
}

//#pragma segment OlePseudoObjInterfaceSeg
STDMETHODIMP IPseudoObjOleObjectEnumVerbs(LPOLEOBJECT lpThis, LPENUMOLEVERB * lplpenumOleVerb)
{
	OleDbgEnterInterface();

    /* OLE2NOTE: we must make sure to set all out parameters to NULL. */
    *lplpenumOleVerb = NULL;

    /* A pseudo object may NOT return OLE_S_USEREG; they must call the
    **    OleReg* API or provide their own implementation. Because this
    **    pseudo object does NOT implement IPersist, simply a low-level
    **    remoting handler (ProxyManager) object as opposed to a
    **    DefHandler object is used as the handler for the pseudo
    **    object in a clients process space. The ProxyManager does NOT
    **    handle the OLE_S_USEREG return values.
    */
    return OleRegEnumVerbs((REFCLSID)&CLSID_Application, lplpenumOleVerb);
}

//#pragma segment OlePseudoObjInterfaceSeg
STDMETHODIMP IPseudoObjOleObjectUpdate(LPOLEOBJECT lpThis)
{
	OleDbgEnterInterface();

    /* OLE2NOTE: a server-only app is always "up-to-date".
    **    a container-app which contains links where the link source
    **    has changed since the last update of the link would be
    **    considered "out-of-date". the "Update" method instructs the
    **    object to get an update from any out-of-date links.
    */

    return NOERROR;
}

//#pragma segment OlePseudoObjInterfaceSeg
STDMETHODIMP IPseudoObjOleObjectIsUpToDate(LPOLEOBJECT lpThis)
{
	OleDbgEnterInterface();

    /* OLE2NOTE: a server-only app is always "up-to-date".
    **    a container-app which contains links where the link source
    **    has changed since the last update of the link would be
    **    considered "out-of-date".
    */
    return NOERROR;
}

//#pragma segment OlePseudoObjInterfaceSeg
STDMETHODIMP IPseudoObjOleObjectGetUserClassID(LPOLEOBJECT lpThis, LPCLSID lpclsid)
{
    PseudoObjPtr pPseudoObj = ((PseudoObjOleObjectImplPtr)lpThis)->lpPseudoObj;
    OleDocumentPtr pOleDoc = pPseudoObj->m_lpOleDoc;

	OleDbgEnterInterface();

	/* OLE2NOTE: we must be carefull to return the correct CLSID here.
	**    if we are currently preforming a "TreatAs (aka. ActivateAs)"
	**    operation then we need to return the class of the object
	**    written in the storage of the object. otherwise we would
	**    return our own class id.
	*/
	ASSERTCOND(pOleDoc->m_pIDoc->lpVtbl->m_GetClassIDProcPtr != nil);
	return (*pOleDoc->m_pIDoc->lpVtbl->m_GetClassIDProcPtr)(pOleDoc->m_pIDoc, lpclsid, nil);
}

//#pragma segment OlePseudoObjInterfaceSeg
STDMETHODIMP IPseudoObjOleObjectGetUserType(LPOLEOBJECT lpThis, unsigned long dwFormOfType, char* * lpszUserType)
{
    PseudoObjPtr pPseudoObj = ((PseudoObjOleObjectImplPtr)lpThis)->lpPseudoObj;
    OleDocumentPtr pOleDoc = pPseudoObj->m_lpOleDoc;
	OleServerDocPtr pServerDoc = &pOleDoc->server;
	CLSID			clsidCurrent;
	
	OleDbgEnterInterface();

    /* OLE2NOTE: we must make sure to set all out parameters to NULL. */
    *lpszUserType = NULL;

	/* OLE2NOTE: we must be carefull to return the correct user type here.
	**    if we are currently preforming a "TreatAs (aka. ActivateAs)"
	**    operation then we need to return the user type name that
    **    corresponds to the class of the object we are currently
    **    emmulating. otherwise we should return our normal user type
    **    name corresponding to our own class. This routine determines
    **    the current clsid in effect.
    **
    **    A pseudo object may NOT return OLE_S_USEREG; they must call the
    **    OleReg* API or provide their own implementation. Because this
    **    pseudo object does NOT implement IPersist, simply a low-level
    **    remoting handler (ProxyManager) object as opposed to a
    **    DefHandler object is used as the handler for the pseudo
    **    object in a clients process space. The ProxyManager does NOT
    **    handle the OLE_S_USEREG return values.
    */
	ASSERTCOND(pOleDoc->m_pIDoc->lpVtbl->m_GetClassIDProcPtr != nil);
	(*pOleDoc->m_pIDoc->lpVtbl->m_GetClassIDProcPtr)(pOleDoc->m_pIDoc, &clsidCurrent, nil);

    return OleRegGetUserType((REFCLSID)&clsidCurrent, dwFormOfType, lpszUserType);
}

//#pragma segment OlePseudoObjInterfaceSeg
STDMETHODIMP IPseudoObjOleObjectSetExtent(LPOLEOBJECT lpThis, unsigned long dwDrawAspect, LPSIZEL lplgrc)
{
	OleDbgEnterInterface();

    // OLE2NOTE: a pseudo object does NOT support SetExtent
    return ResultFromScode(E_FAIL);
}

//#pragma segment OlePseudoObjInterfaceSeg
STDMETHODIMP IPseudoObjOleObjectGetExtent(LPOLEOBJECT lpThis, unsigned long dwDrawAspect, LPSIZEL lplgrc)
{
	OleDbgEnterInterface();

    return ResultFromScode(E_NOTIMPL);
}

//#pragma segment OlePseudoObjInterfaceSeg
STDMETHODIMP IPseudoObjOleObjectAdvise(LPOLEOBJECT lpThis, LPADVISESINK lpAdvSink, unsigned long* lpdwConnection)
{
    PseudoObjPtr pPseudoObj = ((PseudoObjOleObjectImplPtr)lpThis)->lpPseudoObj;
    HRESULT hrErr;

	OleDbgEnterInterface();

    if (pPseudoObj->m_lpOleAdviseHolder == NULL &&
        CreateOleAdviseHolder(&pPseudoObj->m_lpOleAdviseHolder) != NOERROR) {
        return ResultFromScode(E_OUTOFMEMORY);
    }

    hrErr = pPseudoObj->m_lpOleAdviseHolder->lpVtbl->Advise(
            pPseudoObj->m_lpOleAdviseHolder,
            lpAdvSink,
            lpdwConnection
    );

    return hrErr;
}

//#pragma segment OlePseudoObjInterfaceSeg
STDMETHODIMP IPseudoObjOleObjectUnadvise(LPOLEOBJECT lpThis, unsigned long dwConnection)
{
    PseudoObjPtr pPseudoObj = ((PseudoObjOleObjectImplPtr)lpThis)->lpPseudoObj;
    HRESULT hrErr;

	OleDbgEnterInterface();

    if (!pPseudoObj->m_lpOleAdviseHolder) {
        return ResultFromScode(E_FAIL);
    }

    hrErr = pPseudoObj->m_lpOleAdviseHolder->lpVtbl->Unadvise(
            pPseudoObj->m_lpOleAdviseHolder,
            dwConnection
    );

    return hrErr;
}

//#pragma segment OlePseudoObjInterfaceSeg
STDMETHODIMP IPseudoObjOleObjectEnumAdvise(LPOLEOBJECT lpThis, LPENUMSTATDATA * lplpenumAdvise)
{
    PseudoObjPtr pPseudoObj = ((PseudoObjOleObjectImplPtr)lpThis)->lpPseudoObj;
    HRESULT hrErr;

	OleDbgEnterInterface();

    /* OLE2NOTE: we must make sure to set all out parameters to NULL. */
    *lplpenumAdvise = NULL;

    if (!pPseudoObj->m_lpOleAdviseHolder) {
        return ResultFromScode(E_FAIL);
    }

    hrErr = pPseudoObj->m_lpOleAdviseHolder->lpVtbl->EnumAdvise(
            pPseudoObj->m_lpOleAdviseHolder,
            lplpenumAdvise
    );

    return hrErr;
}

//#pragma segment OlePseudoObjInterfaceSeg
STDMETHODIMP IPseudoObjOleObjectGetMiscStatus(LPOLEOBJECT lpThis, unsigned long dwAspect, unsigned long * lpdwStatus)
{
    PseudoObjPtr pPseudoObj = ((PseudoObjOleObjectImplPtr)lpThis)->lpPseudoObj;
    OleDocumentPtr  pOleDoc = pPseudoObj->m_lpOleDoc;
    OleDocumentType	docType;

	OleDbgEnterInterface();

    /* Get our default MiscStatus for the given Aspect. this
    **    information is registered in the RegDB. We query the RegDB
    **    here to guarantee that the value returned from this method
    **    agrees with the values in RegDB. in this way we only have to
    **    maintain the info in one place (in the RegDB). Alternatively
    **    we could have the values hard coded here.
    **
    ** OLE2NOTE: A pseudo object may NOT return OLE_S_USEREG; they must
    **    call the
    **    OleReg* API or provide their own implementation. Because this
    **    pseudo object does NOT implement IPersist, simply a low-level
    **    remoting handler (ProxyManager) object as opposed to a
    **    DefHandler object is used as the handler for the pseudo
    **    object in a clients process space. The ProxyManager does NOT
    **    handle the OLE_S_USEREG return values.
    */
    OleRegGetMiscStatus((REFCLSID)&CLSID_Application, dwAspect, lpdwStatus);

    /* OLE2NOTE: check if the pseudo object is compatible to be
    **    linked by an OLE 1.0 container. it is compatible if
    **    either the pseudo object is an untitled document or a
    **    file-based document. if the pseudo object is part of
    **    an embedded object, then it is NOT compatible to be
    **    linked by an OLE 1.0 container. if it is compatible then
    **    we should include OLEMISC_CANLINKBYOLE1 as part of the
    **    dwStatus flags.
    */
	ASSERTCOND(pOleDoc->m_pIDoc->lpVtbl->m_GetDocTypeProcPtr != nil);
	docType = (pOleDoc->m_pIDoc->lpVtbl->m_GetDocTypeProcPtr)(pOleDoc->m_pIDoc);
	
	if (docType == kNewDocumentType || docType == kFromFileDocumentType)
		*lpdwStatus |= OLEMISC_CANLINKBYOLE1;

	return NOERROR;
}

//#pragma segment OlePseudoObjInterfaceSeg
STDMETHODIMP IPseudoObjOleObjectSetColorScheme(LPOLEOBJECT lpThis, LPOLECOLORSCHEME lpLogpal)
{
	OleDbgEnterInterface();

    // REVIEW: NOT YET IMPLEMENTED
    return ResultFromScode(E_NOTIMPL);
}

#if 0
//#pragma segment OlePseudoObjInterfaceSeg
STDMETHODIMP IPseudoObjOleObjectLockObject(LPOLEOBJECT lpThis, unsigned long fLock)
{
	OleDbgEnterInterface();

return ResultFromScode(E_FAIL);
}
#endif

//#pragma segment OlePseudoObjInterfaceSeg
void PseudoObjIDataObjectInit(PseudoObjDataObjectImplPtr pPseudoObjDataObjectImpl, PseudoObjPtr pPseudoObj)
{
	pPseudoObjDataObjectImpl->lpVtbl		= &gPseudoObjIDataObjectVtbl;
	pPseudoObjDataObjectImpl->lpPseudoObj	= pPseudoObj;
	pPseudoObjDataObjectImpl->cRef			= 0;
}

//#pragma segment OlePseudoObjInterfaceSeg
STDMETHODIMP IPseudoObjDataObjectQueryInterface(LPDATAOBJECT lpThis, REFIID riid, void* * lplpvObj)
{
	PseudoObjPtr		pPseudoObj;

	OleDbgEnterInterface();

	pPseudoObj = ((PseudoObjDataObjectImplPtr)lpThis)->lpPseudoObj;

	return PseudoObjQueryInterface(pPseudoObj, riid, lplpvObj);
}

//#pragma segment OlePseudoObjInterfaceSeg
STDMETHODIMP_(unsigned long) IPseudoObjDataObjectAddRef(LPDATAOBJECT lpThis)
{
	PseudoObjPtr		pPseudoObj;

	OleDbgEnterInterface();

	pPseudoObj = ((PseudoObjDataObjectImplPtr)lpThis)->lpPseudoObj;

	return PseudoObjAddRef(pPseudoObj);
}

//#pragma segment OlePseudoObjInterfaceSeg
STDMETHODIMP_(unsigned long) IPseudoObjDataObjectRelease (LPDATAOBJECT lpThis)
{
	PseudoObjPtr		pPseudoObj;

	OleDbgEnterInterface();

	pPseudoObj = ((PseudoObjDataObjectImplPtr)lpThis)->lpPseudoObj;

	return PseudoObjRelease(pPseudoObj);
}

//#pragma segment OlePseudoObjInterfaceSeg
STDMETHODIMP IPseudoObjDataObjectGetData(LPDATAOBJECT lpThis, LPFORMATETC lpFormatetc, LPSTGMEDIUM lpMedium)
{
    PseudoObjPtr pPseudoObj = ((PseudoObjDataObjectImplPtr)lpThis)->lpPseudoObj;
    OleDocumentPtr pOleDoc = pPseudoObj->m_lpOleDoc;
    void* pItem = pPseudoObj->m_lpItem;
    SCODE sc = S_OK;

	OleDbgEnterInterface();

    /* OLE2NOTE: we must make sure to set all out parameters to NULL. */
    lpMedium->tymed = TYMED_NULL;
    lpMedium->pUnkForRelease = NULL;    // we transfer ownership to caller
    lpMedium->STGMEDIUM_HGLOBAL = NULL;

	if(lpFormatetc->cfFormat == 'PICT' &&
		(lpFormatetc->dwAspect & DVASPECT_CONTENT) ) {
        // Verify caller asked for correct medium
        if (!(lpFormatetc->tymed & TYMED_MFPICT)) {
            return ResultFromScode(DATA_E_FORMATETC);
        }

		ASSERTCOND(pOleDoc->m_pIDoc->lpVtbl->m_GetMetafilePictDataProcPtr != nil);
		lpMedium->STGMEDIUM_HGLOBAL = (*pOleDoc->m_pIDoc->lpVtbl->m_GetMetafilePictDataProcPtr)(pOleDoc->m_pIDoc, pItem);
		ASSERTCOND(lpMedium->STGMEDIUM_HGLOBAL != nil);

        if (! lpMedium->STGMEDIUM_HGLOBAL) {
            return ResultFromScode(E_OUTOFMEMORY);
        }
        lpMedium->tymed = TYMED_MFPICT;

    }

#if LATER
    else if (lpFormatetc->cfFormat == 'PICT' &&
		(lpFormatetc->dwAspect & DVASPECT_ICON) ) {
		
		CLSID clsid;
		
        // Verify caller asked for correct medium
        if (!(lpFormatetc->tymed & TYMED_MFPICT)) {
            return ResultFromScode(DATA_E_FORMATETC);
        }

		/* OLE2NOTE: we should return the default icon for our class.
		**    we must be careful to use the correct CLSID here.
		**    if we are currently preforming a "TreatAs (aka. ActivateAs)"
		**    operation then we need to use the class of the object
		**    written in the storage of the object. otherwise we would
		**    use our own class id.
		*/
		ASSERTCOND(pOleDoc->m_pIDoc->lpVtbl->m_GetClassIDProcPtr != nil);
		hrErr = (*pOleDoc->m_pIDoc->lpVtbl->m_GetClassIDProcPtr)(pOleDoc->m_pIDoc, &clsid, nil);
		
		if (hrErr != NOERROR) {
			return ResultFromScode(DATA_E_FORMATETC);
		}			

        lpMedium->STGMEDIUM_HGLOBAL = GetIconOfClass(
                g_lpApp->m_hInst, (REFCLSID)&clsid, NULL, FALSE);
        if (! lpMedium->STGMEDIUM_HGLOBAL) {
            sc = E_OUTOFMEMORY;
            goto error;
        }

        lpMedium->tymed = TYMED_MFPICT;
		return NOERROR;

    }
#endif // LATER

    else if (lpFormatetc->cfFormat == 'TEXT') {
        // Verify caller asked for correct medium
        if (!(lpFormatetc->tymed & TYMED_HGLOBAL)) {
            return ResultFromScode(DATA_E_FORMATETC);
        }

		ASSERTCOND(pOleDoc->m_pIDoc->lpVtbl->m_GetTextDataProcPtr != nil);
		lpMedium->STGMEDIUM_HGLOBAL = (*pOleDoc->m_pIDoc->lpVtbl->m_GetTextDataProcPtr)(pOleDoc->m_pIDoc, pItem);
		ASSERTCOND(lpMedium->STGMEDIUM_HGLOBAL != nil);
		
		if (!lpMedium->STGMEDIUM_HGLOBAL)
			return ResultFromScode(E_OUTOFMEMORY);

        lpMedium->tymed = TYMED_HGLOBAL;

    } else {
        return ResultFromScode(DATA_E_FORMATETC);
    }

    return NOERROR;

}

//#pragma segment OlePseudoObjInterfaceSeg
STDMETHODIMP IPseudoObjDataObjectGetDataHere(LPDATAOBJECT lpThis, LPFORMATETC lpFormatetc, LPSTGMEDIUM lpMedium)
{
	OleDbgEnterInterface();

    /* Caller is requesting data to be returned in Caller allocated
    **    medium, but we do NOT support this. we only support
    **    global memory blocks that WE allocate for the caller.
    */
    return ResultFromScode(DATA_E_FORMATETC);
}

//#pragma segment OlePseudoObjInterfaceSeg
STDMETHODIMP IPseudoObjDataObjectQueryGetData(LPDATAOBJECT lpThis, LPFORMATETC lpFormatetc)
{
    PseudoObjPtr pPseudoObj = ((PseudoObjDataObjectImplPtr)lpThis)->lpPseudoObj;
    OleDocumentPtr pOleDoc = pPseudoObj->m_lpOleDoc;

	OleDbgEnterInterface();

    /* Caller is querying if we support certain format but does not
    **    want any data actually returned.
    */
    if (lpFormatetc->cfFormat == 'PICT' &&
		(lpFormatetc->dwAspect & (DVASPECT_CONTENT | DVASPECT_ICON)) ) {
        return OleStdQueryFormatMedium(lpFormatetc, TYMED_MFPICT);

    } else if (lpFormatetc->cfFormat == 'TEXT') {
        return OleStdQueryFormatMedium(lpFormatetc, TYMED_HGLOBAL);
    }

    return ResultFromScode(DATA_E_FORMATETC);
}

//#pragma segment OlePseudoObjInterfaceSeg
STDMETHODIMP IPseudoObjDataObjectGetCanonicalFormatEtc(LPDATAOBJECT lpThis, LPFORMATETC lpformatetc, LPFORMATETC lpformatetcOut)
{
	OleDbgEnterInterface();

    // REVIEW: NOT-YET-IMPLEMENTED
    return ResultFromScode(E_NOTIMPL);
}

//#pragma segment OlePseudoObjInterfaceSeg
STDMETHODIMP IPseudoObjDataObjectSetData(LPDATAOBJECT lpThis, LPFORMATETC lpFormatetc, LPSTGMEDIUM lpMedium, unsigned long fRelease)
{
	OleDbgEnterInterface();

    // REVIEW: NOT-YET-IMPLEMENTED
    return ResultFromScode(E_NOTIMPL);
}

//#pragma segment OlePseudoObjInterfaceSeg
STDMETHODIMP IPseudoObjDataObjectEnumFormatEtc(LPDATAOBJECT lpThis, unsigned long dwDirection, LPENUMFORMATETC * lplpenumFormatEtc)
{
    SCODE sc;

	OleDbgEnterInterface();

    /* OLE2NOTE: a pseudo object only needs to enumerate the static list
    **    of formats that are registered for our app in the
    **    registration database. it is NOT
    **    required that a pseudo object (ie. non-DataTransferDoc)
    **    enumerate the OLE formats: CF_LINKSOURCE, CF_EMBEDSOURCE, or
    **    CF_EMBEDDEDOBJECT. we do NOT use pseudo objects for data
    **    transfers.
    **
    **    A pseudo object may NOT return OLE_S_USEREG; they must call the
    **    OleReg* API or provide their own implementation. Because this
    **    pseudo object does NOT implement IPersist, simply a low-level
    **    remoting handler (ProxyManager) object as opposed to a
    **    DefHandler object is used as the handler for the pseudo
    **    object in a clients process space. The ProxyManager does NOT
    **    handle the OLE_S_USEREG return values.
    */
    if (dwDirection == DATADIR_GET)
        return OleRegEnumFormatEtc(
        		(REFCLSID)&CLSID_Application,
        		dwDirection,
        		lplpenumFormatEtc);
    else if (dwDirection == DATADIR_SET)
        sc = E_NOTIMPL;
    else
        sc = E_INVALIDARG;

    return ResultFromScode(sc);
}

//#pragma segment OlePseudoObjInterfaceSeg
STDMETHODIMP IPseudoObjDataObjectAdvise(LPDATAOBJECT lpThis, FORMATETC * lpFormatetc, unsigned long advf, LPADVISESINK lpAdvSink, unsigned long * lpdwConnection)
{
    PseudoObjPtr 	pPseudoObj = ((PseudoObjDataObjectImplPtr)lpThis)->lpPseudoObj;
    HRESULT 		hrErr;

	OleDbgEnterInterface();

    /* OLE2NOTE: we must make sure to set all out parameters to NULL. */
    *lpdwConnection = 0;

    if (pPseudoObj->m_lpDataAdviseHolder == NULL &&
        CreateDataAdviseHolder(&pPseudoObj->m_lpDataAdviseHolder) != NOERROR) {
        return ResultFromScode(E_OUTOFMEMORY);
    }

    hrErr = pPseudoObj->m_lpDataAdviseHolder->lpVtbl->Advise(
            pPseudoObj->m_lpDataAdviseHolder,
            (LPDATAOBJECT)&pPseudoObj->m_DataObject,
            lpFormatetc,
            advf,
            lpAdvSink,
            lpdwConnection
    );

    return hrErr;
}

//#pragma segment OlePseudoObjInterfaceSeg
STDMETHODIMP IPseudoObjDataObjectUnadvise(LPDATAOBJECT lpThis, unsigned long dwConnection)
{
    PseudoObjPtr pPseudoObj = ((PseudoObjDataObjectImplPtr)lpThis)->lpPseudoObj;
    HRESULT hrErr;

	OleDbgEnterInterface();

    // no one registered
    if (pPseudoObj->m_lpDataAdviseHolder == NULL) {
        return ResultFromScode(E_FAIL);
    }

    hrErr = pPseudoObj->m_lpDataAdviseHolder->lpVtbl->Unadvise(
            pPseudoObj->m_lpDataAdviseHolder,
            dwConnection
    );

    return hrErr;
}

//#pragma segment OlePseudoObjInterfaceSeg
STDMETHODIMP IPseudoObjDataObjectEnumAdvise(LPDATAOBJECT lpThis, LPENUMSTATDATA * lplpenumAdvise)
{
    PseudoObjPtr pPseudoObj = ((PseudoObjDataObjectImplPtr)lpThis)->lpPseudoObj;
    HRESULT hrErr;

	OleDbgEnterInterface();

    /* OLE2NOTE: we must make sure to set all out parameters to NULL. */
    *lplpenumAdvise = NULL;

    if (!pPseudoObj->m_lpDataAdviseHolder) {
        return ResultFromScode(E_FAIL);
    }

    hrErr = pPseudoObj->m_lpDataAdviseHolder->lpVtbl->EnumAdvise(
            pPseudoObj->m_lpDataAdviseHolder,
            lplpenumAdvise
    );

    return hrErr;
}

// OLDNAME: OlePseudoObj.c

extern OleApplicationPtr		gOleApp;

//#pragma segment OlePseudoObjSeg
void PseudoObjInit(PseudoObjPtr pPseudoObj, void* pItem, OleDocumentPtr pOleDoc)
{
	pPseudoObj->m_cRef					= 0;
	pPseudoObj->m_fObjIsClosing			= false;
	pPseudoObj->m_lpItem				= pItem;
	pPseudoObj->m_lpOleDoc				= pOleDoc;
	pPseudoObj->m_lpOleAdviseHolder		= nil;
	pPseudoObj->m_lpDataAdviseHolder	= nil;

	PseudoObjIUnknownInit(&pPseudoObj->m_Unknown, pPseudoObj);
	PseudoObjIOleObjectInit(&pPseudoObj->m_OleObject, pPseudoObj);
	PseudoObjIDataObjectInit(&pPseudoObj->m_DataObject, pPseudoObj);

	/* OLE2NOTE: Increment the refcnt of the Doc on behalf of the
	**	PseudoObj. the Document should not shut down unless all
	**	pseudo objects are closed. when a pseudo object is destroyed,
	**	it call ServerDoc_PseudoObjRelease to release this hold on
	**	the document.
	*/
	OleServerDocPseudoObjLockDoc(pOleDoc);
}

//#pragma segment OlePseudoObjSeg
void PseudoObjDispose(PseudoObjPtr pPseudoObj)
{
    OleDocumentPtr pOleDoc = pPseudoObj->m_lpOleDoc;

	ASSERTCOND(pPseudoObj->m_cRef == 0);

    /* OLE2NOTE: in order to have a stable App, Doc, AND pseudo object
    **    during the process of closing, we intially AddRef the App,
    **    Doc ref counts and later Release them. These
    **    initial AddRefs are artificial; they are simply done to
    **    guarantee that these objects do not get destroyed until the
    **    end of this routine.
    */
    OleAppAddRef(gOleApp);
    OleDocAddRef(pOleDoc);

    /******************************************************************
    ** OLE2NOTE: we no longer need the advise and enum holder objects,
    **    so release them.
    ******************************************************************/

    if (pPseudoObj->m_lpDataAdviseHolder) {
        /* release DataAdviseHolder; we SHOULD be the only one using it. */
        OleStdVerifyRelease(
                (LPUNKNOWN)pPseudoObj->m_lpDataAdviseHolder
            );
        pPseudoObj->m_lpDataAdviseHolder = NULL;
    }

    if (pPseudoObj->m_lpOleAdviseHolder) {
        /* release OleAdviseHolder; we SHOULD be the only one using it. */
        OleStdVerifyRelease(
                (LPUNKNOWN)pPseudoObj->m_lpOleAdviseHolder
            );
        pPseudoObj->m_lpOleAdviseHolder = NULL;
    }

	/* forget the pointer to destroyed PseudoObj in NameTable */
	if (pPseudoObj->m_lpItem) {
		ASSERTCOND(pOleDoc->m_pIDoc->lpVtbl->m_RemovePseudoObjFromItemProcPtr != nil);
		(pOleDoc->m_pIDoc->lpVtbl->m_RemovePseudoObjFromItemProcPtr)(pOleDoc->m_pIDoc, pPseudoObj->m_lpItem);
	}
	
    /* OLE2NOTE: release the lock on the Doc held on behalf of the
    **    PseudoObj. the Document should not shut down unless all
    **    pseudo objects are closed. when a pseudo object is first
    **    created, it calls ServerDoc_PseudoObjLockDoc to guarantee
    **    that the document stays alive (called from PseudoObj_Init).
    */
    OleServerDocPseudoObjUnlockDoc(pOleDoc);

    DisposePtr((Ptr)pPseudoObj);        // Free the memory for the structure itself

    OleDocRelease(pOleDoc);       // release artificial AddRef above
    OleAppRelease(gOleApp);       // release artificial AddRef above
}

//#pragma segment OlePseudoObjSeg
unsigned long PseudoObjAddRef(PseudoObjPtr pPseudoObj)
{
	pPseudoObj->m_cRef++;

	return pPseudoObj->m_cRef;
}

//#pragma segment OlePseudoObjSeg
unsigned long PseudoObjRelease(PseudoObjPtr pPseudoObj)
{
	unsigned long	cRef;

	ASSERTCOND(pPseudoObj->m_cRef > 0);

	cRef = --pPseudoObj->m_cRef;

	if (cRef == 0)
		PseudoObjDispose(pPseudoObj);

	return cRef;
}

//#pragma segment OlePseudoObjSeg
HRESULT PseudoObjQueryInterface(PseudoObjPtr pPseudoObj, REFIID riid, void* * lplpvObj)
{
	SCODE		scode;
	
	
	if (IsEqualIID(riid, &IID_IUnknown))
	{
		*lplpvObj = &pPseudoObj->m_Unknown;
		PseudoObjAddRef(pPseudoObj);
		scode = S_OK;
	}
	else if (IsEqualIID(riid, &IID_IOleObject))
	{
		*lplpvObj = &pPseudoObj->m_OleObject;
		PseudoObjAddRef(pPseudoObj);
		scode = S_OK;
	}
	else if (IsEqualIID(riid, &IID_IDataObject))
	{
		*lplpvObj = &pPseudoObj->m_DataObject;
		PseudoObjAddRef(pPseudoObj);
		scode = S_OK;
	}
	else
	{
		*lplpvObj = NULL;
		scode = E_NOINTERFACE;
	}
	
	return ResultFromScode(scode);
}

/* PseudoObjClose
 * --------------
 *
 *  Close the pseudo object. Force all external connections to close
 *      down. This causes link clients to release this PseudoObj. when
 *      the refcount actually reaches 0, then the PseudoObj will be
 *      destroyed.
 *
 *  Returns:
 *      FALSE -- user canceled the closing of the doc.
 *      TRUE -- the doc was successfully closed
 */
//#pragma segment OlePseudoObjSeg
Boolean PseudoObjClose(PseudoObjPtr pPseudoObj)
{
	OleDocumentPtr		pOleDoc;
	
	if (pPseudoObj->m_fObjIsClosing)
		return TRUE;		// closing already in progress
			
	pPseudoObj->m_fObjIsClosing = true;
	pOleDoc = (OleDocumentPtr)pPseudoObj->m_lpOleDoc;
	
	OleAppAddRef(gOleApp);
	OleDocAddRef(pOleDoc);
	PseudoObjAddRef(pPseudoObj);
	
	if (pPseudoObj->m_lpDataAdviseHolder)
	{
		PseudoObjSendAdvise(pPseudoObj, OLE_ONDATACHANGE, NULL, ADVF_DATAONSTOP);
		
		OleStdVerifyRelease((LPUNKNOWN)pPseudoObj->m_lpDataAdviseHolder);
		pPseudoObj->m_lpDataAdviseHolder = NULL;
	}
	
	if (pPseudoObj->m_lpOleAdviseHolder)
	{
		PseudoObjSendAdvise(pPseudoObj, OLE_ONCLOSE, NULL, 0);
		
		OleStdVerifyRelease((LPUNKNOWN)pPseudoObj->m_lpOleAdviseHolder);
		pPseudoObj->m_lpOleAdviseHolder = NULL;
	}
	
	CoDisconnectObject((LPUNKNOWN)&pPseudoObj->m_Unknown, 0);
	
	PseudoObjRelease(pPseudoObj);
	OleDocRelease(pOleDoc);
	OleAppRelease(gOleApp);
	
	return true;
}


/* PseudoObjGetExtent
 * ------------------
 *
 *      Get the extent (width, height) of the entire document.
 */
void PseudoObjGetExtent(PseudoObjPtr pPseudoObj, LPSIZEL lpsizel)
{
#if 0
    OleDocumentPtr pOleDoc = (LPOLEDOC)pPseudoObj->m_lpOleDoc;
    LINERANGE lrSel;

    PseudoObj_GetSel(pPseudoObj, (LPLINERANGE)&lrSel);

    LineList_CalcSelExtentInHimetric(lpLL, (LPLINERANGE)&lrSel, lpsizel);
#else

Debugger();

#endif
}


//#pragma segment OlePseudoObjSeg
/* PseudoObjSendAdvise
 * -------------------
 *
 * This function sends an advise notification on behalf of a specific
 *  doc object to all its clients.
 */
void PseudoObjSendAdvise(PseudoObjPtr pPseudoObj, OLE_NOTIFICATION wAdvise, LPMONIKER pmkObj, ADVF dwAdvf)
{
    switch (wAdvise) {

        case OLE_ONDATACHANGE:

            // inform clients that the data of the object has changed
			if (true) {
//            if (pPseudoObj->m_lpOleDoc->m_fEnableDraw) {
                /* drawing is currently enabled. inform clients that
                **    the data of the object has changed
                */

                pPseudoObj->m_fDataChanged = false;
                if (pPseudoObj->m_lpDataAdviseHolder) {

                    pPseudoObj->m_lpDataAdviseHolder->lpVtbl->SendOnDataChange(
                            pPseudoObj->m_lpDataAdviseHolder,
                            (LPDATAOBJECT)&pPseudoObj->m_DataObject,
                            0,
                            dwAdvf
                    );
                }

            } else {
                /* drawing is currently disabled. do not send
                **    notifications until drawing is re-enabled.
                */
                pPseudoObj->m_fDataChanged = true;
            }
            break;

        case OLE_ONCLOSE:

            // inform clients that the object is shutting down

            if (pPseudoObj->m_lpOleAdviseHolder) {

                pPseudoObj->m_lpOleAdviseHolder->lpVtbl->SendOnClose(
                        pPseudoObj->m_lpOleAdviseHolder
                );
            }
            break;

        case OLE_ONSAVE:

            // inform clients that the object has been saved

            if (pPseudoObj->m_lpOleAdviseHolder) {

                pPseudoObj->m_lpOleAdviseHolder->lpVtbl->SendOnSave(
                        pPseudoObj->m_lpOleAdviseHolder
                );
            }
            break;

        case OLE_ONRENAME:

            // inform clients that the object's name has changed
            if (pmkObj && pPseudoObj->m_lpOleAdviseHolder) {

                if (pPseudoObj->m_lpOleAdviseHolder)
                    pPseudoObj->m_lpOleAdviseHolder->lpVtbl->SendOnRename(
                            pPseudoObj->m_lpOleAdviseHolder,
                            pmkObj
                    );
            }
            break;
    }
}


/* PseudoObjGetFullMoniker
 * -----------------------
 *
 * Returns the Full, absolute Moniker which identifies this pseudo object.
 */
LPMONIKER PseudoObjGetFullMoniker(PseudoObjPtr pPseudoObj, LPMONIKER pmkDoc)
{
    char* pItemName;
    OleDocumentPtr pOleDoc;
    LPMONIKER pmkItem = nil;
    LPMONIKER pmkPseudoObj = nil;

	pOleDoc = pPseudoObj->m_lpOleDoc;
	
    if (pmkDoc) {
    	ASSERTCOND(pOleDoc->m_pIDoc->lpVtbl->m_GetItemNameProcPtr != nil);
    	pItemName = (pOleDoc->m_pIDoc->lpVtbl->m_GetItemNameProcPtr)(pOleDoc->m_pIDoc, pPseudoObj->m_lpItem);
        CreateItemMoniker(OLESTDDELIM, pItemName, &pmkItem);

        /* OLE2NOTE: create an absolute moniker which identifies the
        **    pseudo object. this moniker is created as a composite of
        **    the absolute moniker for the entire document appended
        **    with an item moniker which identifies the selection of
        **    the pseudo object relative to the document.
        */
        CreateGenericComposite(pmkDoc, pmkItem, &pmkPseudoObj);

        if (pmkItem)
            OleStdRelease((LPUNKNOWN)pmkItem);

        return pmkPseudoObj;
    } else {
        return nil;
    }
}

#endif // qOleServerApp
