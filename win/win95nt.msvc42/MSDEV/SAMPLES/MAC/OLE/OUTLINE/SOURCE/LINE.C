/*****************************************************************************\
*                                                                             *
*    Line.c                                                                   *
*                                                                             *
*    OLE Version 2.0 Sample Code                                              *
*                                                                             *
*    Copyright (c) 1992-1994, Microsoft Corp. All rights reserved.            *
*                                                                             *
\*****************************************************************************/

#if !defined(_MSC_VER) && !defined(THINK_C)
#include "OLine.h"
#endif

#include "types.h"

#if defined(USEHEADER)
#include "OleHdrs.h"
#endif
#include "OleDebug.h"
#include "Debug.h"
#include "Line.h"
#include "OleXcept.h"
#include "Util.h"
#include "Const.h"
#include "Doc.h"
#include "Table.h"
#include "PObj.h"
#include "ole2ui.h"
#include "Site.h"
#include <ToolUtils.h>
#include "LowMem.h"

OLEDBGDATA

static LineVtblPtr	gLineVtbl = nil;

//#pragma segment VtblInitSeg
LineVtblPtr LineGetVtbl()
{
	ASSERTCOND(gLineVtbl != nil);
	return gLineVtbl;
}

//#pragma segment VtblInitSeg
void LineInitVtbl()
{
	LineVtblPtr	vtbl;
	
	vtbl = gLineVtbl = (LineVtblPtr)NewPtrClear(sizeof(*vtbl));
	ASSERTCOND(vtbl != nil);
	FailNIL(vtbl);

	vtbl->m_DisposeProcPtr =		LineDispose;
	vtbl->m_FreeProcPtr =			LineFree;
	
	vtbl->m_CopyToDocProcPtr =		LineCopyToDoc;
	
	vtbl->m_DrawProcPtr =			LineDraw;
	vtbl->m_DrawSelectedProcPtr =	LineDrawSelected;
	vtbl->m_DrawFrameProcPtr =		LineDrawFrame;
	vtbl->m_FocusProcPtr = 			LineFocus;
	
	vtbl->m_UpdateViewProcPtr =		LineUpdateView;

	vtbl->m_GetTextProcPtr =		LineGetText;
	
	vtbl->m_LineIndentProcPtr =		LineIndent;
	
	vtbl->m_DoDoubleClickProcPtr =	LineDoDoubleClick;
	
	vtbl->m_SetExtentProcPtr =		LineSetExtent;

#if qOle
	vtbl->m_SaveToStorageProcPtr = 	LineSaveToStorage;
#endif

	ASSERTCOND(ValidVtbl(vtbl, sizeof(*vtbl)));
}

//#pragma segment VtblDisposeSeg
void LineDisposeVtbl()
{
	ASSERTCOND(gLineVtbl != nil);
	DisposePtr((Ptr)gLineVtbl);
}

//#pragma segment LineSeg
void LineInit(LinePtr pLine, struct LineListRec* pLineList)
{
	pLine->vtbl = LineGetVtbl();

	pLine->m_LineType =					kUnkownLineType;
	
	pLine->m_fNeedsUpdateView =			true;
	
	pLine->m_LineList =					pLineList;

	pLine->m_TabLevel =					0;
	pLine->m_TabWidthInPoints =			kTabWidth;

	pLine->m_HeightInPoints =			0;
	pLine->m_WidthInPoints =			0;
}

//#pragma segment LineSeg
void LineDispose(LinePtr pLine)
{
	DisposePtr((Ptr)pLine);
}

//#pragma segment LineSeg
void LineFree(LinePtr pLine)
{
	ASSERTCOND(pLine->vtbl->m_DisposeProcPtr != nil);
	(*pLine->vtbl->m_DisposeProcPtr)(pLine);
}

//#pragma segment LineSeg
void LineCopyToDoc(LinePtr pLine, struct DocumentRec* pDoc)
{
	// virtual function, do nothing
}

//#pragma segment LineSeg
void LineIndent(LinePtr pLine, Boolean indent)
{
	if (indent)
		pLine->m_TabLevel++;
	else
	{
		pLine->m_TabLevel--;
		if (pLine->m_TabLevel < 0)
			pLine->m_TabLevel = 0;
	}

	pLine->m_fNeedsUpdateView = true;
}

//#pragma segment LineSeg
short LineGetTabLevel(LinePtr pLine)
{
	return pLine->m_TabLevel;
}

//#pragma segment LineSeg
void LineSetTabLevel(LinePtr pLine, short tabLevel)
{
	if (tabLevel != pLine->m_TabLevel)
		pLine->m_fNeedsUpdateView = true;
		
	pLine->m_TabLevel = tabLevel;
}

//#pragma segment LineSeg
void LineUpdateView(LinePtr pLine)
{
	pLine->m_fNeedsUpdateView = false;
}

//#pragma segment LineSeg
void LineDrawNow(LinePtr pLine)
{
	LineListDrawLine(pLine->m_LineList, pLine);
}

//#pragma segment LineSeg
void LineInval(LinePtr pLine)
{
	LineListInvalLine(pLine->m_LineList, pLine);
}

//#pragma segment LineSeg
void LineValid(LinePtr pLine)
{
	LineListValidLine(pLine->m_LineList, pLine);
}

//#pragma segment LineSeg
void LineDraw(LinePtr pLine, Rect* r)
{
}

//#pragma segment LineSeg
void LineDrawSelected(LinePtr pLine, Rect* r)
{
	Rect	cellRect;
	
	ASSERTCOND(pLine->vtbl->m_DrawProcPtr != nil);
	(*pLine->vtbl->m_DrawProcPtr)(pLine, r);
	
	LineListGetLineRect(pLine->m_LineList, pLine, &cellRect);
	
	LMSetHiliteMode(LMGetHiliteMode() & ~(1<<hiliteBit));
	InvertRect(&cellRect);
}

//#pragma segment LineSeg
void LineDrawFrame(LinePtr pLine, Rect* r)
{
}

//#pragma segment LineSeg
void LineFocus(LinePtr pLine, Rect* r)
{
}

//#pragma segment LineSeg
char* LineGetText(LinePtr pLine)
{
	return nil;
}

//#pragma segment LineSeg
void LineGetBounds(LinePtr pLine, Rect* r)
{
	if (pLine->m_fNeedsUpdateView)
	{
		ASSERTCOND(pLine->vtbl->m_UpdateViewProcPtr != nil);
		(*pLine->vtbl->m_UpdateViewProcPtr)(pLine);
	}
	
	LineListGetLineRect(pLine->m_LineList, pLine, r);
	r->left += pLine->m_TabLevel * pLine->m_TabWidthInPoints;
	r->right = r->left + pLine->m_WidthInPoints;
	r->bottom = r->top + pLine->m_HeightInPoints;
}

//#pragma segment LineSeg
void LineSetBounds(LinePtr pLine, Rect* r)
{
	Rect	rCurrentBounds;
	SIZEL	sizel;
	
	LineGetBounds(pLine, &rCurrentBounds);
	
	// if they are equal, we are done
	if (EqualRect(r, &rCurrentBounds))
		return;

	sizel.cx = r->right - r->left;
	sizel.cy = r->bottom - r->top;
	
	LineSetExtent(pLine, &sizel);

	// make sure the top stays anchored
	r->top = rCurrentBounds.top;
	r->bottom = (short)(r->top + sizel.cy);

	LineListGetLineRect(pLine->m_LineList, pLine, &rCurrentBounds);

	// calcalate tab level
	pLine->m_TabLevel = (r->left - rCurrentBounds.left + (pLine->m_TabWidthInPoints>>1)) / pLine->m_TabWidthInPoints;
	if (pLine->m_TabLevel < 0)
		pLine->m_TabLevel = 0;

	// calculate rect edges based on tab level
	r->left  = rCurrentBounds.left + pLine->m_TabLevel * pLine->m_TabWidthInPoints;
	r->right = r->left + pLine->m_WidthInPoints;
}

//#pragma segment LineSeg
LineType LineGetType(LinePtr pLine)
{
	return pLine->m_LineType;
}

//#pragma segment LineSeg
void LineGetExtent(LinePtr pLine, LPSIZEL lpsizel)
{
	if (pLine->m_fNeedsUpdateView)
	{
		ASSERTCOND(pLine->vtbl->m_UpdateViewProcPtr != nil);
		(*pLine->vtbl->m_UpdateViewProcPtr)(pLine);
	}

	lpsizel->cx = pLine->m_WidthInPoints;
	lpsizel->cy = pLine->m_HeightInPoints;
}

//#pragma segment LineSeg
void LineSetExtent(LinePtr pLine, LPSIZEL lpsizel)
{
	pLine->m_WidthInPoints	= (short)lpsizel->cx;
	pLine->m_HeightInPoints	= (short)lpsizel->cy;
	
	pLine->m_fNeedsUpdateView = true;
}

//#pragma segment LineSeg
WindowPtr LineGetWindow(LinePtr pLine)
{
	return LineListGetWindow(pLine->m_LineList);
}

//#pragma segment LineSeg
void LineDoDoubleClick(LinePtr pLine)
{
}


// OLDNAME: LineList.c
extern	ApplicationPtr	gApplication;

static Boolean LineListClickLoop(TablePtr theTable);

//#pragma segment LineListSeg
void LineListInit(LineListPtr pLineList, WindowPtr theWindow, Rect* theRect, DocumentPtr pDoc)
{
	Rect		dataBounds;
	Cell		cellSize;
	
	// check the validity of parameter
	ASSERTCOND(pLineList && theWindow && theRect);
	
	SetRect(&dataBounds, 0, 0, 1, 0);
	SetPt(&cellSize, 0, 0);
		
	pLineList->m_Lines = TableNew(theRect, &dataBounds, cellSize,
				LineListDrawCell, theWindow, true, true, true, true);
	ASSERTCOND(pLineList->m_Lines != nil);
	FailNIL(pLineList->m_Lines);
	
	pLineList->m_Lines->m_RefCon = pLineList;
	
	pLineList->m_Lines->m_ClickLoopProcPtr = LineListClickLoop;
	
	TableSetDrawCellSelectedProc(pLineList->m_Lines, LineListDrawSelectedCell);
	
	pLineList->m_pDoc		  = pDoc;
	pLineList->m_fForceUpdate = false;
}

//#pragma segment LineListSeg
void LineListDispose(LineListPtr pLineList)
{
	short		i;
	LinePtr		pLine;
	Cell		theCell;

	// check the validity of parameter
	ASSERTCOND(pLineList);
	
	theCell.h = 0;

	for(i = pLineList->m_Lines->m_DataBounds.bottom - 1; i>=0; i--)
	{
		theCell.v = i;
		TableGetCell(pLineList->m_Lines, &pLine, theCell);
		ASSERTCOND(pLine != nil);

		if (pLine != nil)
		{
			ASSERTCOND(pLine->vtbl->m_FreeProcPtr != nil);
			(*pLine->vtbl->m_FreeProcPtr)(pLine);
		}
	}

	TableDispose(pLineList->m_Lines);
	
	DisposePtr((Ptr)pLineList);
}

//#pragma segment LineListSeg
void LineListGetView(LineListPtr pLineList, Rect *r)
{
	TableGetView(pLineList->m_Lines, r);
}

//#pragma segment LineListSeg
void LineListActivate(LineListPtr pLineList, Boolean becomingActive)
{
	// check the validity of parameter
	ASSERTCOND(pLineList);

	TableActivate(pLineList->m_Lines, becomingActive);
}

//#pragma segment LineListSeg
Boolean LineListIsSelection(LineListPtr pLineList)
{
	// check the validity of parameter
	ASSERTCOND(pLineList);

	return TableIsSelection(pLineList->m_Lines);
}

//#pragma segment LineListSeg
Boolean LineListIsEmpty(LineListPtr pLineList)
{
	// check the validity of parameter
	ASSERTCOND(pLineList);

	return TableIsEmpty(pLineList->m_Lines);
}

//#pragma segment LineListSeg
void LineListSelectRange(LineListPtr pLineList, LineRangePtr plrSel)
{
	Cell	theCell;
	short	start;
	short	end;

	// check the validity of parameter
	ASSERTCOND(pLineList);

	if (plrSel && plrSel->m_nStartLine >= 0)
		start = plrSel->m_nStartLine;
	else
		start = 0;
		
	if (plrSel && plrSel->m_nEndLine < LineListGetCount(pLineList))
		end = plrSel->m_nEndLine;
	else
		end = LineListGetCount(pLineList) - 1;
		
	theCell.h = 0;
	
	TableClearSelection(pLineList->m_Lines);
	for (theCell.v = start; theCell.v <= end; theCell.v++)
		TableSetSelect(pLineList->m_Lines, theCell, true);
}

//#pragma segment LineListSeg
void LineListIndentLines(LineListPtr pLineList, Boolean indent)
{
	Cell		theCell;
	LinePtr		pLine;

	// check the validity of parameter
	ASSERTCOND(pLineList);
	
	TableDoDraw(pLineList->m_Lines, false);

	SetPt(&theCell, 0, 0);

	while(TableGetSelect(pLineList->m_Lines, true, &theCell))
	{
		TableGetCell(pLineList->m_Lines, &pLine, theCell);
		
		ASSERTCOND(pLine->vtbl->m_LineIndentProcPtr != nil);
		(*pLine->vtbl->m_LineIndentProcPtr)(pLine, indent);
		
		TableInvalCell(pLineList->m_Lines, theCell);
	
#if qOleServerApp
		NameTableEditLineUpdate(OutlineDocGetNameTable((OutlineDocPtr)pLineList->m_pDoc), theCell.v);
#endif // qOleServerApp
	
		TableNextCell(pLineList->m_Lines, true, true, &theCell);
	}

	TableDoDraw(pLineList->m_Lines, true);
}

//#pragma segment LineListSeg
void LineListDoClear(LineListPtr pLineList)
{
	LineRangeRec	lrSel;

	// check the validity of parameter
	ASSERTCOND(pLineList);
	
	if (LineListGetSelection(pLineList, &lrSel) > 0)
		LineListDeleteRange(pLineList, &lrSel);
}

/* LineListAddLine
 *
 *	Purpose:
 *		To insert a line into the linelist
 *
 *	Parameters:
 *		pLine		pointer to the line to be inserted
 *
 *	Returns:
 *		TRUE		the line is inserted successfully
 *		FALSE		fail
 */
//#pragma segment LineListSeg
Boolean LineListAddLine(LineListPtr pLineList, LinePtr pLine)
{
	Cell			theCell;
	LineRangeRec	lrSel;

	// check the validity of parameter
	ASSERTCOND(pLineList && pLine);

	TableDoDraw(pLineList->m_Lines, false);

	theCell.h = 0;
	theCell.v = 32767;
	
	
	if (!TableAddRow(pLineList->m_Lines, 1, &theCell.v)) {
		TableDoDraw(pLineList->m_Lines, true);
		return false;
	}
	
	TableSetCell(pLineList->m_Lines, pLine, theCell);

	lrSel.m_nStartLine = theCell.v;
	lrSel.m_nEndLine = theCell.v;
	LineListSelectRange(pLineList, &lrSel);

	LineListUpdateView(pLineList);

	TableDoDraw(pLineList->m_Lines, true);
	
	LineListShowIndexedLine(pLineList, theCell.v);

	NameTableAddLineUpdate(OutlineDocGetNameTable((OutlineDocPtr)pLineList->m_pDoc), theCell.v);
	
	return true;
}

//#pragma segment LineListSeg
void LineListDeleteLine(LineListPtr pLineList, LinePtr pLine)
{
	Cell	theCell;
	
	TableDoDraw(pLineList->m_Lines, false);
	
	if (LineListFindLineCell(pLineList, pLine, &theCell))
	{
		ASSERTCOND(pLine->vtbl->m_FreeProcPtr != nil);
		(*pLine->vtbl->m_FreeProcPtr)(pLine);

		TableDeleteRow(pLineList->m_Lines, 1, theCell.v);
		NameTableDeleteLineUpdate(OutlineDocGetNameTable((OutlineDocPtr)pLineList->m_pDoc), theCell.v);
	}

	TableDoDraw(pLineList->m_Lines, true);
}

//#pragma segment LineListSeg
/* LineListDeleteRange
 *
 *	Purpose:
 *		To delete a range of lines into the linelist
 *
 *	Parameters:
 *		pLine		pointer to the line to be inserted
 *
 *	Returns:
 *		TRUE		the line is inserted successfully
 *		FALSE		fail
 */
void LineListDeleteRange(LineListPtr pLineList, LineRangePtr pLineRange)
{
	Cell			theCell;
	short			nStart;
	short			nEnd;
	LinePtr			pLine;
	NameTablePtr	pNameTable;
	
    nStart = pLineRange ? pLineRange->m_nStartLine : 0;
    nEnd = pLineRange ? pLineRange->m_nEndLine : LineListGetCount(pLineList) - 1;
	
	if ((nStart < 0) || (nEnd >= LineListGetCount(pLineList)) || (nStart > nEnd))
		return;

	TableDoDraw(pLineList->m_Lines, false);
	
	theCell.h = 0;
	theCell.v = nStart;
	pNameTable = OutlineDocGetNameTable((OutlineDocPtr)pLineList->m_pDoc);
	
	for (theCell.v = nStart; theCell.v <= nEnd; nEnd--) {
		TableGetCell(pLineList->m_Lines, &pLine, theCell);
		
		ASSERTCOND(pLine->vtbl->m_FreeProcPtr != nil);
		(*pLine->vtbl->m_FreeProcPtr)(pLine);
			
		TableDeleteRow(pLineList->m_Lines, 1, theCell.v);
		NameTableDeleteLineUpdate(pNameTable, theCell.v);
	}
	
	TableDoDraw(pLineList->m_Lines, true);
}

//#pragma segment LineListSeg
void LineListNewTextLine(LineListPtr pLineList, StringPtr s)
{
	TextLinePtr		pTextLine;
	LinePtr			pLine;

	// check the validity of parameter
	ASSERTCOND(pLineList && s);

	pTextLine = (TextLinePtr)NewPtrClear(sizeof(TextLineRec));
	ASSERTCOND(pTextLine != nil);
	FailNIL(pTextLine);

	pLine = (LinePtr)pTextLine;
	
	TextLineInit(pTextLine, pLineList);
	TextLineSetText(pTextLine, s);

	if (!LineListAddLine(pLineList, pLine)) {
		ASSERTCOND(pLine->vtbl->m_FreeProcPtr != nil);
		(*pLine->vtbl->m_FreeProcPtr)(pLine);
	}		
}

//#pragma segment LineListSeg
void LineListCopyToDoc(LineListPtr pLineList, DocumentPtr pDoc)
{
	Cell			theCell;
	LinePtr			pLine;
	int				Copied = 0;
	LineRangeRec	lrSel;
	LineListPtr		pDestLineList;
	
	// check the validity of parameter
	ASSERTCOND(pLineList);

	if (!LineListGetSelection(pLineList, &lrSel))		// no selection
		return;
	
	theCell.h = 0;
	for (theCell.v = lrSel.m_nStartLine; theCell.v <= lrSel.m_nEndLine; theCell.v++) {
		TableGetCell(pLineList->m_Lines, &pLine, theCell);
		
		ASSERTCOND(pLine->vtbl->m_CopyToDocProcPtr != nil);
		(*pLine->vtbl->m_CopyToDocProcPtr)(pLine, pDoc);
		Copied++;
	}

	pDestLineList = OutlineDocGetLineList((OutlineDocPtr)pDoc);
	LineListSetCopiedRange(pDestLineList, &lrSel);

#if qOleServerApp

	OleServerDocDoCopyRange(&((OleOutlineDocPtr)pDoc)->m_OleDoc);
	
#elif qOleContainerApp
	if ((Copied == 1) && (pLine->m_LineType == kContainerLineType))
	{
		LPOLEOBJECT			pSrcOleObj = nil;

		pSrcOleObj = ((OleContainerLinePtr)pLine)->site.m_pOleObj;
		if (pSrcOleObj)
		{
	        OleContainerLinePtr	pDestLine = nil;
	
	        pDestLine = (OleContainerLinePtr)LineListGetLine(pDestLineList, 0);
			ASSERTCOND(pDestLine != nil);

			OleContainerDocDoCopySingleEmbedding(
					&((OleOutlineDocPtr)pDoc)->m_OleDoc,
					pSrcOleObj,
					((OleContainerLinePtr)pLine)->m_DrawAspect,
					pDestLine,
					pLine);
		}
	}
#endif // qOleContainerApp
}

//#pragma segment LineListSeg
void LineListUpdate(LineListPtr pLineList, RgnHandle visRgn)
{
	GrafPtr		oldPort;
	Rect		r;
	
	GetPort(&oldPort);
	SetPort(pLineList->m_Lines->m_Port);

	r = pLineList->m_Lines->m_View;
	InsetRect(&r, -1, -1);
	FrameRect(&r);
	
	TableUpdate(pLineList->m_Lines, visRgn);
	
	SetPort(oldPort);
}

//#pragma segment LineListSeg
void LineListUpdateView(LineListPtr pLineList)
{
	Cell		theCell;
	LinePtr		pLine;
	short		maxWidth;
	short		totalLineWidth;
	GrafPtr		oldPort;
	
	// check the validity of parameter
	ASSERTCOND(pLineList);

	if (!pLineList->m_fForceUpdate && !LineListIsVisible(pLineList))
		return;

	pLineList->m_fForceUpdate = false;

	if (TableIsEmpty(pLineList->m_Lines))
		return;
		
	SetPt(&theCell, 0, 0);
	maxWidth = 0;
	
	GetPort(&oldPort);
	SetPort(LineListGetWindow(pLineList));
	
	do
	{
		TableGetCell(pLineList->m_Lines, &pLine, theCell);
		ASSERTCOND(pLine != nil);
		
		if (pLine->m_fNeedsUpdateView)
		{
			ASSERTCOND(pLine->vtbl->m_UpdateViewProcPtr != nil);
			(*pLine->vtbl->m_UpdateViewProcPtr)(pLine);
								
			if (pLine->m_HeightInPoints != TableGetRowHeight(pLineList->m_Lines, theCell.v))
				LineListSetRowHeight(pLineList, theCell, pLine->m_HeightInPoints);
				
			LineInval(pLine);
		}
		
		totalLineWidth = pLine->m_WidthInPoints + (pLine->m_TabLevel * pLine->m_TabWidthInPoints);
		
		// make sure we get the maximum line length
		if (totalLineWidth > maxWidth)
			maxWidth = totalLineWidth;
	
	} while (TableNextCell(pLineList->m_Lines, true, true, &theCell));
	
	// make sure that the max is greater than the view width
	if (maxWidth < TableGetViewWidth(pLineList->m_Lines))
		maxWidth = TableGetViewWidth(pLineList->m_Lines);
	
	if (maxWidth != TableGetColumnWidth(pLineList->m_Lines, 0))
	{
		ASSERTCOND(maxWidth > 0);
		TableSetColumnWidth(pLineList->m_Lines, 0, maxWidth);
	}
	
	SetPort(oldPort);
}

//#pragma segment LineListSeg
void LineListUpdateTableLineExtent(LineListPtr pLineList, LinePtr pLine)
{
	Cell	theCell;
	short	maxWidth;
	
	LineListFindLineCell(pLineList, pLine, &theCell);

	if (pLine->m_HeightInPoints != TableGetRowHeight(pLineList->m_Lines, theCell.v))
		LineListSetRowHeight(pLineList, theCell, pLine->m_HeightInPoints);

	maxWidth = pLine->m_WidthInPoints + (pLine->m_TabLevel * pLine->m_TabWidthInPoints);

	// make sure that the max is greater than the view width
	if (maxWidth < TableGetViewWidth(pLineList->m_Lines))
		maxWidth = TableGetViewWidth(pLineList->m_Lines);
	
	if (maxWidth != TableGetColumnWidth(pLineList->m_Lines, 0))
	{
		ASSERTCOND(maxWidth > 0);
		TableSetColumnWidth(pLineList->m_Lines, 0, maxWidth);
	}
}

//#pragma segment LineListSeg
Boolean LineListFindLineCell(LineListPtr pLineList, LinePtr pLine, Cell* theCell)
{
	Cell	testCell;
	LinePtr	pTestLine;
	
	// check the validity of parameter
	ASSERTCOND(pLineList && pLine && theCell);

	if (TableIsEmpty(pLineList->m_Lines))
		return false;
		
	SetPt(&testCell, 0, 0);
	
	do
	{
		TableGetCell(pLineList->m_Lines, &pTestLine, testCell);
		if (pLine == pTestLine)
		{
			*theCell = testCell;
			return true;
		}	
	} while(TableNextCell(pLineList->m_Lines, true, true, &testCell));
		
	return false;
}

//#pragma segment LineListSeg
void LineListDrawLine(LineListPtr pLineList, LinePtr pLine)
{
	Cell	theCell;
	
	// check the validity of parameter
	ASSERTCOND(pLineList && pLine);

	if (!LineListFindLineCell(pLineList, pLine, &theCell))
		return;
	
	TableDraw(pLineList->m_Lines, theCell, true);
}

//#pragma segment LineListSeg
void LineListInvalLine(LineListPtr pLineList, LinePtr pLine)
{
	Cell	theCell;
	
	// check the validity of parameter
	ASSERTCOND(pLineList && pLine);

	if (!LineListFindLineCell(pLineList, pLine, &theCell))
		return;
	
	TableInvalCell(pLineList->m_Lines, theCell);
}

//#pragma segment LineListSeg
void LineListValidLine(LineListPtr pLineList, LinePtr pLine)
{
	Cell	theCell;
	
	// check the validity of parameter
	ASSERTCOND(pLineList && pLine);

	if (!LineListFindLineCell(pLineList, pLine, &theCell))
		return;
		
	TableValidCell(pLineList->m_Lines, theCell);
}

//#pragma segment LineListSeg
void LineListGetLineRect(LineListPtr pLineList, LinePtr pLine, Rect* r)
{
	Cell	theCell;

	// check the validity of parameter
	ASSERTCOND(pLineList && pLine && r);

	SetRect(r, 0, 0, 0, 0);
	
	if (!LineListFindLineCell(pLineList, pLine, &theCell))
		return;
		
	TableGetCellRect(pLineList->m_Lines, theCell, r);
}

//#pragma segment LineListSeg
WindowPtr LineListGetWindow(LineListPtr pLineList)
{	
	// check the validity of parameter
	ASSERTCOND(pLineList);

	return pLineList->m_Lines->m_Port;
}

//#pragma segment LineListSeg
Boolean LineListClick(LineListPtr pLineList, Point pt, short modifiers)
{
	LineRangeRec	lrSel;
	LinePtr			pLine;
	CursorPos		cp;
	
	// check the validity of parameter
	ASSERTCOND(pLineList);

	cp = LineListGiveCursorFeedback(pLineList, pt);

	switch (cp) {
	
#if qOleDragDrop	
		// in the drag handle with one or more item selected
		case kCPTopDragHandle:
		case kCPBotDragHandle:
			OleOutlineDocStartDrag((OleOutlineDocPtr)pLineList->m_pDoc, pt);
			return false;		// not a double click
#endif // qOleDragDrop

#if qOleContainerApp

		// in the resize handle with one object selected
		case kCPTopLeftHandle:
		case kCPMidLeftHandle:
		case kCPBotLeftHandle:
		case kCPTopMidHandle:
		case kCPBotMidHandle:
		case kCPTopRightHandle:
		case kCPMidRightHandle:
		case kCPBotRightHandle:
			LineListGetSelection(pLineList, &lrSel);
			pLine = LineListGetLine(pLineList, lrSel.m_nStartLine);
			LineListDoResizeLine(pLineList, cp, (OleContainerLinePtr)pLine);
			return false;		// not a double click

#endif // qOleContainerApp

		default:	// kCPContent or kCPControlRegion
			return TableClick(pLineList->m_Lines, pt, modifiers);
	}

	return false;			
}

//#pragma segment LineListSeg
Boolean LineListClickLoop(TablePtr theTable)
{
	LineListPtr		pLineList;
	short			deltaH;
	short			deltaV;
	short			newHPos;
	short			newVPos;
	
	pLineList = theTable->m_RefCon;
	ASSERTCOND(pLineList != nil);
	
	newHPos = LineListGetHorizOffset(pLineList);
	newVPos = LineListGetVertOffset(pLineList);
	
	deltaH = pLineList->m_OldScrollHPos - newHPos;
	deltaV = pLineList->m_OldScrollVPos - newVPos;
	
	if (deltaH != 0 || deltaV != 0)
	{
		ASSERTCOND(pLineList->m_pDoc->vtbl->m_ScrollRectProcPtr != nil);
		(*pLineList->m_pDoc->vtbl->m_ScrollRectProcPtr)(pLineList->m_pDoc, deltaH, deltaV);
	}
	
	pLineList->m_OldScrollHPos = newHPos;
	pLineList->m_OldScrollVPos = newVPos;
	
	return true;
}

//#pragma segment LineListSeg
short LineListGetHorizOffset(LineListPtr pLineList)
{
	return GetCtlValue(pLineList->m_Lines->m_ScrollH);
}

//#pragma segment LineListSeg
short LineListGetVertOffset(LineListPtr pLineList)
{
	return GetCtlValue(pLineList->m_Lines->m_ScrollV);
}

void LineListSetRowHeight(LineListPtr pLineList, Point theCell, short newHeight)
{
	Rect		r;
	Rect		cellRect;
	GrafPtr		oldPort;
	
	OutlineDocGetRowHeadingRect((OutlineDocPtr)pLineList->m_pDoc, &r);
	InsetRect(&r, 1, 1);

	TableGetCellRect(pLineList->m_Lines, theCell, &cellRect);
	r.top = cellRect.top;

	GetPort(&oldPort);
	SetPort(LineListGetWindow(pLineList));
	
	InvalRect(&r);
	
	SetPort(oldPort);
	
	TableSetRowHeight(pLineList->m_Lines, theCell.v, newHeight);
}

//#pragma segment LineListSeg
void LineListDoDoubleClick(LineListPtr pLineList)
{
	GrafPtr		oldPort;
	Cell		theCell;
	LinePtr		pLine;
	
	// check the validity of parameter
	ASSERTCOND(pLineList);

	GetPort(&oldPort);
	SetPort(LineListGetWindow(pLineList));

	SetPt(&theCell, 0, 0);
	while(TableGetSelect(pLineList->m_Lines, true, &theCell))
	{
		TableGetCell(pLineList->m_Lines, &pLine, theCell);
		
		ASSERTCOND(pLine->vtbl->m_DoDoubleClickProcPtr != nil);
		(*pLine->vtbl->m_DoDoubleClickProcPtr)(pLine);
		
		LineInval(pLine);
		
		TableNextCell(pLineList->m_Lines, true, true, &theCell);
	}

	SetPort(oldPort);
}

//#pragma segment LineListSeg
void LineListTempSize(LineListPtr pLineList, short width, short height)
{
	// check the validity of parameter
	ASSERTCOND(pLineList);

	TableTempSize(pLineList->m_Lines, width, height);
}

//#pragma segment LineListSeg
void LineListSize(LineListPtr pLineList, short width, short height)
{
	// check the validity of parameter
	ASSERTCOND(pLineList);

	TableDoDraw(pLineList->m_Lines, false);

	TableSize(pLineList->m_Lines, width, height);

	LineListUpdateView(pLineList);

	TableDoDraw(pLineList->m_Lines, true);
}

//#pragma segment LineListSeg
void LineListDrawCell(TablePtr theTable, Rect* r, void* p)
{
	LinePtr		pLine;
	Rect		rLine;
	
	// check the validity of parameter
	ASSERTCOND(theTable && r && p);

	EraseRect(r);

	if (p == nil)
		return;

	pLine = (LinePtr)p;

	rLine = *r;
	
	rLine.left += (pLine->m_TabLevel * pLine->m_TabWidthInPoints);
	rLine.right = rLine.left + pLine->m_WidthInPoints;
	
	rLine.bottom = rLine.top + pLine->m_HeightInPoints;

	ASSERTCOND(pLine->vtbl->m_DrawProcPtr != nil);
	(*pLine->vtbl->m_DrawProcPtr)((LinePtr)p, &rLine);
}

//#pragma segment LineListSeg
void LineListDrawSelectedCell(TablePtr theTable, Boolean selected, Rect* r, void* p)
{
	LinePtr		pLine;
	Rect		rLine;
	
	// check the validity of parameter
	ASSERTCOND(theTable && r && p);

	EraseRect(r);

	if (p == nil)
		return;

	pLine = (LinePtr)p;

	rLine = *r;
	
	rLine.left += (pLine->m_TabLevel * pLine->m_TabWidthInPoints);
	rLine.right = rLine.left + pLine->m_WidthInPoints;
	
	rLine.bottom = rLine.top + pLine->m_HeightInPoints;

	if (selected)
	{
		ASSERTCOND(pLine->vtbl->m_DrawSelectedProcPtr != nil);
		(*pLine->vtbl->m_DrawSelectedProcPtr)((LinePtr)p, &rLine);
	}
	else
	{
		ASSERTCOND(pLine->vtbl->m_DrawProcPtr != nil);
		(*pLine->vtbl->m_DrawProcPtr)((LinePtr)p, &rLine);
	}
}

//#pragma segment LineListSeg
short LineListGetCount(LineListPtr pLineList)
{
	Rect	r;

	// check the validity of parameter
	ASSERTCOND(pLineList);

	r = pLineList->m_Lines->m_DataBounds;

	return r.bottom - r.top;
}

//#pragma segment LineListSeg
LinePtr LineListGetLine(LineListPtr pLineList, short index)
{
	LinePtr		pLine;
	Cell		theCell;

	// check the validity of parameter
	ASSERTCOND(pLineList);

	theCell.h = 0;
	theCell.v = index;

	TableGetCell(pLineList->m_Lines, &pLine, theCell);

	return pLine;
}

//#pragma segment LineListSeg
void LineListSetLine(LineListPtr pLineList, short index, LinePtr pLine)
{
	Cell	theCell;

	// check the validity of parameter
	ASSERTCOND(pLineList && pLine);

	theCell.h = 0;
	theCell.v = index;

	TableSetCell(pLineList->m_Lines, pLine, theCell);
}

//#pragma segment LineListSeg
void LineListGetExtentRect(LineListPtr pLineList, LineRangePtr pLineRange, Rect* r)
{
	Rect	rBounds;
	
	// check the validity of parameter
	ASSERTCOND(pLineList && r);

	rBounds.left = 0;
	rBounds.right = 1;
	rBounds.top = pLineRange ? pLineRange->m_nStartLine : 0;
	rBounds.bottom = pLineRange ? pLineRange->m_nEndLine + 1: LineListGetCount(pLineList);
	
	SetRect(r, 0, 0, 0, 0);

	if (!TableIsEmpty(pLineList->m_Lines))
	{
		GrafPtr		oldPort;
		Cell		theCell;
		short		maxWidth;
		LinePtr		pLine;
		short		height;
		short		totalLineWidth;
		
		GetPort(&oldPort);
		SetPort(LineListGetWindow(pLineList));

		theCell = topLeft(rBounds);
		maxWidth = 0;
		height = 0;
		
		do
		{
			TableGetCell(pLineList->m_Lines, &pLine, theCell);

			if (pLine->m_fNeedsUpdateView)
			{
				ASSERTCOND(pLine->vtbl->m_UpdateViewProcPtr != nil);
				(*pLine->vtbl->m_UpdateViewProcPtr)(pLine);
				
				if (pLine->m_HeightInPoints != TableGetRowHeight(pLineList->m_Lines, theCell.v))
					LineListSetRowHeight(pLineList, theCell, pLine->m_HeightInPoints);

				LineInval(pLine);
			}
			
			totalLineWidth = pLine->m_WidthInPoints + (pLine->m_TabLevel * pLine->m_TabWidthInPoints);
			
			// make sure we get the maximum line length
			if (totalLineWidth > maxWidth)
				maxWidth = totalLineWidth;

			height += pLine->m_HeightInPoints;

		} while (TableNextCell(pLineList->m_Lines, true, true, &theCell) && PtInRect(theCell, &rBounds));

		if (maxWidth != TableGetColumnWidth(pLineList->m_Lines, 0))
		{
			ASSERTCOND(maxWidth > 0);
			TableSetColumnWidth(pLineList->m_Lines, 0, maxWidth);
		}

		r->right = maxWidth;
		r->bottom = height;

		SetPort(oldPort);
	}
}

//#pragma segment LineListSeg
void LineListGetExtent(LineListPtr pLineList, LineRangePtr pLineRange, LPSIZEL lpsizel)
{
	Rect	rBounds;
	
	// check the validity of parameter
	ASSERTCOND(pLineList && lpsizel);

	rBounds.left = 0;
	rBounds.right = 1;
	rBounds.top = pLineRange ? pLineRange->m_nStartLine : 0;
	rBounds.bottom = pLineRange ? pLineRange->m_nEndLine + 1: LineListGetCount(pLineList);

	lpsizel->cx = 0;
	lpsizel->cy = 0;
	
	if (!TableIsEmpty(pLineList->m_Lines))
	{
		GrafPtr		oldPort;
		Cell		theCell;
		short		maxWidth;
		LinePtr		pLine;
		short		height;
		short		totalLineWidth;
		
		GetPort(&oldPort);
		SetPort(LineListGetWindow(pLineList));

		theCell = topLeft(rBounds);
		maxWidth = 0;
		height = 0;
		
		do
		{
			TableGetCell(pLineList->m_Lines, &pLine, theCell);

			if (pLine->m_fNeedsUpdateView)
			{
				ASSERTCOND(pLine->vtbl->m_UpdateViewProcPtr != nil);
				(*pLine->vtbl->m_UpdateViewProcPtr)(pLine);
				
				if (pLine->m_HeightInPoints != TableGetRowHeight(pLineList->m_Lines, theCell.v))
					LineListSetRowHeight(pLineList, theCell, pLine->m_HeightInPoints);

				LineInval(pLine);
			}
			
			totalLineWidth = pLine->m_WidthInPoints + (pLine->m_TabLevel * pLine->m_TabWidthInPoints);
			
			// make sure we get the maximum line length
			if (totalLineWidth > maxWidth)
				maxWidth = totalLineWidth;

			height += pLine->m_HeightInPoints;

		} while (TableNextCell(pLineList->m_Lines, true, true, &theCell) && PtInRect(theCell, &rBounds));

		if (maxWidth != TableGetColumnWidth(pLineList->m_Lines, 0))
		{
			ASSERTCOND(maxWidth > 0);
			TableSetColumnWidth(pLineList->m_Lines, 0, maxWidth);
		}

		lpsizel->cx = maxWidth;
		lpsizel->cy = height;

		SetPort(oldPort);
	}
}

//#pragma segment LineListSeg
PicHandle LineListGetPICT(LineListPtr pLineList, LineRangePtr pLineRange, Rect* r)
{
	Rect		rRealBounds;
	GrafPtr		oldPort;
	GrafPort	pictPort;
	Rect		rFrame;
	PicHandle	pictHandle;
	
	// check the validity of parameter
	ASSERTCOND(pLineList);

	rRealBounds.left = 0;
	rRealBounds.right = 1;
	rRealBounds.top = pLineRange ? pLineRange->m_nStartLine : 0;
	rRealBounds.bottom = pLineRange ? pLineRange->m_nEndLine + 1: LineListGetCount(pLineList);
	
	if (r != nil)
		rFrame = *r;
	else
		LineListGetExtentRect(pLineList, pLineRange, &rFrame);

	GetPort(&oldPort);
	OpenPort(&pictPort);
	SetPort(&pictPort);
	ClipRect(&rFrame);
	
	pictHandle = OpenPicture(&rFrame);
	TableDrawIntoPort(pLineList->m_Lines, &pictPort, &rRealBounds);
	ClosePicture();
	
	ClosePort(&pictPort);
	SetPort(oldPort);
	
	return pictHandle;
}

//#pragma segment LineListSeg
Handle LineListGetText(LineListPtr pLineList, LineRangePtr pLineRange)
{
	short		nStart;
	short		nEnd;
	Handle		dataHandle;
	char*		dataPtr;
	short		i, j;
	int			nTabLevel;
	Size		memSize;
	LinePtr		pLine;
	char*		lpszText;

	// check the validity of parameter
	ASSERTCOND(pLineList);

    nStart = pLineRange ? pLineRange->m_nStartLine : 0;
    nEnd = pLineRange ? pLineRange->m_nEndLine : LineListGetCount(pLineList) - 1;
	
	memSize = 1;	// null char at the end
	for (i = nStart; i <= nEnd; i++) {
		pLine = LineListGetLine(pLineList, i);
		
		memSize += LineGetTabLevel(pLine);

		ASSERTCOND(pLine->vtbl->m_GetTextProcPtr != nil);
		lpszText = (*pLine->vtbl->m_GetTextProcPtr)(pLine);
		ASSERTCOND(lpszText != nil);
		memSize += strlen(lpszText);
		
		memSize += 1; // add 1 for '\r' at the end of each line
	}
	
	dataHandle = NewHandle(memSize);
	if (!dataHandle)
		return nil;
		
	HLock(dataHandle);
	dataPtr = *dataHandle;

    for (i = nStart; i <= nEnd; i++) {
	    pLine = LineListGetLine(pLineList, i);

    	nTabLevel = LineGetTabLevel(pLine);
    	
    	for (j = 0; j < nTabLevel; j++)
    		*dataPtr++='\t';

		ASSERTCOND(pLine->vtbl->m_GetTextProcPtr != nil);
    	lpszText = (*pLine->vtbl->m_GetTextProcPtr)(pLine);
		
		while (*lpszText)
    		*dataPtr++ = *lpszText++;     // advance to end of string

#ifndef THINK_C
    	*dataPtr++ = '\n';
#else
    	*dataPtr++ = '\r';
#endif

	}
	*dataPtr = '\0';
	
	HUnlock(dataHandle);
	
	return dataHandle;
}

//#pragma segment LineListSeg
void LineListShowLine(LineListPtr pLineList, LinePtr pLine)
{
	Cell	theCell;
	
	// check the validity of parameter
	ASSERTCOND(pLineList && pLine);

	LineListFindLineCell(pLineList, pLine, &theCell);

	TableScrollToCell(pLineList->m_Lines, theCell);
}

//#pragma segment LineListSeg
void LineListShowIndexedLine(LineListPtr pLineList, int i)
{
	Cell	theCell;
	
	// check the validity of parameter
	ASSERTCOND(pLineList);

	if (i >= 0 && i < LineListGetCount(pLineList)) {
		theCell.h = 0;
		theCell.v = i;
		TableScrollToCell(pLineList->m_Lines, theCell);
	}
}

//#pragma segment LineListSeg
void LineListSetRedraw(LineListPtr pLineList, Boolean redraw)
{
	// check the validity of parameter
	ASSERTCOND(pLineList);

	TableDoDraw(pLineList->m_Lines, redraw);
}

//#pragma segment LineListSeg
short LineListGetSelection(LineListPtr pLineList, LineRangePtr plrSel)
{
	Cell	theCell;
	
	// check the validity of parameter
	ASSERTCOND(pLineList && plrSel);

	theCell.h = theCell.v = 0;
	
	if (!TableGetSelect(pLineList->m_Lines, true, &theCell))
		return 0;
	
	plrSel->m_nStartLine = theCell.v;
	
	do {
		plrSel->m_nEndLine = theCell.v;
		TableNextCell(pLineList->m_Lines, true, true, &theCell);
	} while(TableGetSelect(pLineList->m_Lines, true, &theCell));

	return plrSel->m_nEndLine - plrSel->m_nStartLine + 1;
}

//#pragma segment LineListSeg
LineRangePtr LineListGetCopiedRange(LineListPtr pLineList)
{
	// check the validity of parameter
	ASSERTCOND(pLineList);

	return &pLineList->m_lrCopied;
}

//#pragma segment LineListSeg
void LineListSetCopiedRange(LineListPtr pLineList, LineRangePtr pLineRange)
{
	// check the validity of parameter
	ASSERTCOND(pLineList && pLineRange);

	pLineList->m_lrCopied = *pLineRange;
}

//#pragma segment LineListSeg
Boolean LineListIsLineSelected(LineListPtr pLineList, LinePtr pLine)
{
	Cell			theCell;
	
	LineListFindLineCell(pLineList, pLine, &theCell);
	return TableGetSelect(pLineList->m_Lines, false, &theCell);
}

//#pragma segment LineListSeg
Boolean LineListLineIsVisible(LineListPtr pLineList, LinePtr pLine)
{
	Cell	theCell;
	
	if (!LineListFindLineCell(pLineList, pLine, &theCell))
		return false;
		
	return TableCellIsVisible(pLineList->m_Lines, theCell);
}

//#pragma segment LineListSeg
Boolean LineListIsVisible(LineListPtr pLineList)
{
	DocumentPtr		pDoc;
	
	pDoc = pLineList->m_pDoc;
	
	ASSERTCOND(pDoc->vtbl->m_IsVisibleProcPtr != nil);
	return (*pDoc->vtbl->m_IsVisibleProcPtr)(pDoc);
}

//#pragma segment LineListSeg
DocumentPtr	LineListGetDoc(LineListPtr pLineList)
{
	return pLineList->m_pDoc;
}

//#pragma segment LineListSeg
CursorPos LineListGiveCursorFeedback(LineListPtr pLineList, Point mPt)
{
	LineRangeRec		lrSel;
	LinePtr				pStartLine;
	LinePtr				pEndLine;
	short				handlePos;
	short				linesSelected;
	CursPtr				pNewCursor;
	Rect				rLine;

	if (!PtInRect(mPt, &pLineList->m_Lines->m_View)) {
		ASSERTCOND(gApplication->vtbl->m_SetIdleCursorProcPtr != nil);
		(*gApplication->vtbl->m_SetIdleCursorProcPtr)(gApplication, &qd.arrow);
		return kCPElseWhere;
	}
		
	linesSelected = LineListGetSelection(pLineList, &lrSel);

	/* cursor in the content with no selection */
	if (!linesSelected) {
		ASSERTCOND(gApplication->vtbl->m_SetIdleCursorProcPtr != nil);
		(*gApplication->vtbl->m_SetIdleCursorProcPtr)(gApplication, *GetCursor(plusCursor));
		return kCPContent;
	}

#if qOle

	pStartLine = LineListGetLine(pLineList, lrSel.m_nStartLine);
	pEndLine = LineListGetLine(pLineList, lrSel.m_nEndLine);

#if qOleContainerApp
	
	if (linesSelected == 1 && LineGetType(pStartLine) == kContainerLineType) {
		handlePos = OleContainerLineFindHandle((OleContainerLinePtr)pStartLine, &mPt);

		/* cursor on a handle with one object selected */
		if (handlePos) {
			CursorPos	cursorPos;

			switch (handlePos) {
				case 1:
					pNewCursor = *GetCursor(kCursorBackwardResize);
					cursorPos = kCPTopLeftHandle;
					break;
					
				case 2:
					pNewCursor = *GetCursor(kCursorHorizResize);
					cursorPos = kCPMidLeftHandle;
					break;

				case 3:
					pNewCursor = *GetCursor(kCursorForwardResize);
					cursorPos = kCPBotLeftHandle;
					break;

				case 4:
					pNewCursor = *GetCursor(kCursorVertResize);
					cursorPos = kCPTopMidHandle;
					break;
					
				case 5:
					pNewCursor = *GetCursor(kCursorVertResize);
					cursorPos = kCPBotMidHandle;
					break;
					
				case 6:
					pNewCursor = *GetCursor(kCursorForwardResize);
					cursorPos = kCPTopRightHandle;
					break;
					
				case 7:
					pNewCursor = *GetCursor(kCursorHorizResize);
					cursorPos = kCPMidRightHandle;
					break;
					
				case 8:
					pNewCursor = *GetCursor(kCursorBackwardResize);
					cursorPos = kCPBotRightHandle;
					break;
			}

			if (handlePos >= 1 && handlePos <= 8)
			{
				ASSERTCOND(gApplication->vtbl->m_SetIdleCursorProcPtr != nil);
				(*gApplication->vtbl->m_SetIdleCursorProcPtr)(gApplication, pNewCursor);

				return cursorPos;
			}
		}
	}
	
#endif // qOleContainerApp	

#if qOleDragDrop
		
	/* cursor on the upper drag handle */
	LineGetBounds(pStartLine, &rLine);
	if ((mPt.v >= rLine.top) && (mPt.v - rLine.top <= kHitTestDelta)) {
		ASSERTCOND(gApplication->vtbl->m_SetIdleCursorProcPtr != nil);
		(*gApplication->vtbl->m_SetIdleCursorProcPtr)(gApplication, &qd.arrow);
		return kCPTopDragHandle;
	}
	
	/* cursor on the lower drag handle */	
	LineGetBounds(pEndLine, &rLine);
	if ((rLine.bottom >= mPt.v) && (rLine.bottom - mPt.v <= kHitTestDelta)) {
		ASSERTCOND(gApplication->vtbl->m_SetIdleCursorProcPtr != nil);
		(*gApplication->vtbl->m_SetIdleCursorProcPtr)(gApplication, &qd.arrow);
		return kCPBotDragHandle;
	}
	
#endif // qOleDragDrop

#endif // qOle
		
	ASSERTCOND(gApplication->vtbl->m_SetIdleCursorProcPtr != nil);
	(*gApplication->vtbl->m_SetIdleCursorProcPtr)(gApplication, *GetCursor(plusCursor));

	return kCPContent;
}

#if qOleContainerApp

//#pragma segment LineListSeg
/* LineListDoResizeLine
 * --------------------
 *
 *	Purpose:
 *		Resize a containerline. It starts by tracking the mouse and get the new size
 *
 *  Parameter:
 *		pContainerLine - the container line to be resized
 *
 *	Return:
 */	
void LineListDoResizeLine(LineListPtr pLineList, CursorPos cp, OleContainerLinePtr pContainerLine)
{
	Point		mPt;
	Point		mOldPt;
	Rect		rLine;
	Rect		rSize;
	PenState	oldPS;
	LinePtr		pLine = (LinePtr)pContainerLine;
	SIZEL		sizel;
	
	if (cp != kCPTopLeftHandle &&
		cp != kCPMidLeftHandle &&
		cp != kCPBotLeftHandle &&
		cp != kCPTopMidHandle &&
		cp != kCPBotMidHandle &&
		cp != kCPTopRightHandle &&
		cp != kCPMidRightHandle &&
		cp != kCPBotRightHandle)
		return;
	
	LineGetBounds(pLine, &rLine);
	GetPenState(&oldPS);
	
	PenMode(patXor);
	PenPat((ConstPatternParam)&qd.gray);

	GetMouse(&mOldPt);
	rSize = rLine;
					
	switch (cp) {
		case kCPTopLeftHandle:
			topLeft(rSize) = mOldPt;
			break;
			
		case kCPMidLeftHandle:
			rSize.left = mOldPt.h;
			break;
			
		case kCPBotLeftHandle:
			rSize.left = mOldPt.h;
			rSize.bottom = mOldPt.v;
			break;
			
		case kCPTopMidHandle:
			rSize.top = mOldPt.v;
			break;
			
		case kCPBotMidHandle:
			rSize.bottom = mOldPt.v;
			break;

		case kCPTopRightHandle:
			rSize.right = mOldPt.h;
			rSize.top = mOldPt.v;
			break;
			
		case kCPMidRightHandle:
			rSize.right = mOldPt.h;
			break;
			
		case kCPBotRightHandle:
			botRight(rSize) = mOldPt;
			break;
			
	}
	
	FrameRect(&rSize);
	
	while (StillDown())
	{
		GetMouse(&mPt);			
		if (mOldPt.v != mPt.v || mOldPt.h != mPt.h) {
			FrameRect(&rSize);
			
			switch (cp) {
				case kCPTopLeftHandle:
					topLeft(rSize) = mPt;
					break;
					
				case kCPMidLeftHandle:
					rSize.left = mPt.h;
					break;
					
				case kCPBotLeftHandle:
					rSize.left = mPt.h;
					rSize.bottom = mPt.v;
					break;
					
				case kCPTopMidHandle:
					rSize.top = mPt.v;
					break;
					
				case kCPBotMidHandle:
					rSize.bottom = mPt.v;
					break;
		
				case kCPTopRightHandle:
					rSize.right = mPt.h;
					rSize.top = mPt.v;
					break;
					
				case kCPMidRightHandle:
					rSize.right = mPt.h;
					break;
					
				case kCPBotRightHandle:
					botRight(rSize) = mPt;
					break;
			}
							
			FrameRect(&rSize);
			mOldPt = mPt;
		}
	}
	
	FrameRect(&rSize);
	
	SetPenState(&oldPS);
	
	if (!EqualRect(&rSize, &rLine)) {
		sizel.cx = rSize.right - rSize.left;
		sizel.cy = rSize.bottom - rSize.top;
		
		ASSERTCOND(pLine->vtbl->m_SetExtentProcPtr != nil);
		(*pLine->vtbl->m_SetExtentProcPtr)(pLine, &sizel);
	}
		
}

#endif // qOleContainerApp

#if qOleDragDrop

//#pragma segment LineListSeg
void LineListScroll(LineListPtr pLineList, SCROLLDIR scrolldir)
{
	switch (scrolldir) {
		case SCROLLDIR_UP:
			TableScroll(pLineList->m_Lines, 0, -1);
			break;
		
		case SCROLLDIR_DOWN:
			TableScroll(pLineList->m_Lines, 0, 1);
			break;

		case SCROLLDIR_LEFT:
			TableScroll(pLineList->m_Lines, -1, 0);
			break;

		case SCROLLDIR_RIGHT:
			TableScroll(pLineList->m_Lines, 1, 0);
			break;
	}
}

#endif // qOleDragDrop

// OLDNAME: TextLine.c
static TextLineVtblPtr	gTextLineVtbl = nil;

//#pragma segment VtblInitSeg
TextLineVtblPtr TextLineGetVtbl()
{
	ASSERTCOND(gTextLineVtbl != nil);
	return gTextLineVtbl;
}

//#pragma segment VtblInitSeg
void TextLineInitVtbl()
{
	TextLineVtblPtr	vtbl;
	
	vtbl = gTextLineVtbl = (TextLineVtblPtr)NewPtrClear(sizeof(*vtbl));
	ASSERTCOND(vtbl != nil);
	FailNIL(vtbl);

	InheritFromVtbl(vtbl, LineGetVtbl());

	vtbl->m_CopyToDocProcPtr = 			TextLineCopyToDoc;

	vtbl->m_DrawProcPtr =				TextLineDraw;
	
	vtbl->m_UpdateViewProcPtr =			TextLineUpdateView;

	vtbl->m_GetTextProcPtr =			TextLineGetText;

#if qOle
		vtbl->m_SaveToStorageProcPtr = 	TextLineSaveToStorage;
#endif

	ASSERTCOND(ValidVtbl(vtbl, sizeof(*vtbl)));
}

//#pragma segment VtblDisposeSeg
void TextLineDisposeVtbl()
{
	ASSERTCOND(gTextLineVtbl != nil);
	DisposePtr((Ptr)gTextLineVtbl);
}

//#pragma segment TextLineSeg
void TextLineInit(TextLinePtr pTextLine, struct LineListRec* pLineList)
{
	LinePtr		pLine;

	pLine = (LinePtr)pTextLine;
	
	LineInit(&pTextLine->superClass, pLineList);

	pTextLine->vtbl = ((LinePtr)pTextLine)->vtbl;
	((LinePtr)pTextLine)->vtbl = TextLineGetVtbl();

	pLine->m_LineType = kTextLineType;

	{
		GrafPtr		p;

		GetPort(&p);
		pTextLine->m_TextFont = p->txFont;
		pTextLine->m_TextSize = p->txSize;
		pTextLine->m_TextFace = p->txFace;
	}

	pTextLine->m_TextLineLength = 0;
}

//#pragma segment TextLineSeg
void TextLineDraw(LinePtr pLine, Rect* r)
{
	TextLinePtr		pTextLine;

	pTextLine = (TextLinePtr)pLine;

	if (((TextLinePtr)pLine)->m_TextLineLength == 0)
		return;

	TextFont(pTextLine->m_TextFont);
	TextSize(pTextLine->m_TextSize);
	TextFace(pTextLine->m_TextFace);

	MoveTo(r->left, (short)(r->bottom - pTextLine->m_TextDescent));
	DrawText(pTextLine->m_Text, 0, pTextLine->m_TextLineLength);
}

//#pragma segment TextLineSeg
void TextLineUpdateView(LinePtr pLine)
{
	TextLinePtr		pTextLine;
	
	pTextLine = (TextLinePtr)pLine;
	
	TextFont(pTextLine->m_TextFont);
	TextSize(pTextLine->m_TextSize);
	TextFace(pTextLine->m_TextFace);

	{
		FontInfo	fi;

		GetFontInfo(&fi);
		pLine->m_HeightInPoints = fi.ascent + fi.descent + fi.leading;
		pTextLine->m_TextDescent = fi.descent + fi.leading;
	}
	
	pLine->m_WidthInPoints = TextWidth(pTextLine->m_Text, 0, pTextLine->m_TextLineLength);
	
	ASSERTCOND(pTextLine->vtbl->m_UpdateViewProcPtr != nil);
	(*pTextLine->vtbl->m_UpdateViewProcPtr)(pLine);
}

//#pragma segment TextLineSeg
void TextLineSetText(TextLinePtr pTextLine, StringPtr s)
{
	ASSERTCOND(s[0] <= kMaxTextLineLength);

	bcopy(&s[1], pTextLine->m_Text, s[0]);
	pTextLine->m_TextLineLength = s[0];
}

//#pragma segment TextLineSeg
char* TextLineGetText(LinePtr pLine)
{
	TextLinePtr		pTextLine;
	
	pTextLine = (TextLinePtr)pLine;
	
	return pTextLine->m_Text;
}

//#pragma segment TextLineSeg
void TextLineCopyToDoc(LinePtr pLine, DocumentPtr pDoc)
{
	TextLinePtr		pDestTextLine;
	LinePtr			pDestLine;
	LineListPtr		pLineList;
	Str255			s;
	char*			p;
	int				TabLevel;
	
	pDestTextLine = (TextLinePtr)NewPtrClear(sizeof(TextLineRec));
	ASSERTCOND(pDestTextLine != nil);
	FailNIL(pDestTextLine);

	pDestLine = (LinePtr)pDestTextLine;
	
	pLineList = ((OutlineDocPtr)pDoc)->m_LineList;
	TextLineInit(pDestTextLine, pLineList);
	p = TextLineGetText(pLine);
	strcpy((char*)s, p);
	c2pstr((char*)s);
	TextLineSetText(pDestTextLine, s);
	
	TabLevel = LineGetTabLevel(pLine);
	LineSetTabLevel(pDestLine, (short)(TabLevel));
	
	if (!LineListAddLine(pLineList, pDestLine)) {
		ASSERTCOND(pDestLine->vtbl->m_FreeProcPtr != nil);
		(*pDestLine->vtbl->m_FreeProcPtr)(pDestLine);
	}	
}

#if qOle
// OLDNAME: OleLine.c
//#pragma segment OleLineSeg
void LineSaveToStorage(LinePtr pLine, LPSTORAGE lpSrcStg, LPSTORAGE lpDestStg, LPSTREAM lpDestStm, Boolean fRemember)
{
	LineHeaderRec	lineHeader;
	HRESULT			hrErr;
	unsigned long	nWritten;

	bzero(&lineHeader, sizeof(LineHeaderRec));

	lineHeader.m_LineType =				pLine->m_LineType;
	lineHeader.m_nTabLevel =			pLine->m_TabLevel;
	lineHeader.m_nTabWidthInHimetric =	pLine->m_TabWidthInPoints * pLine->m_TabLevel * 2530 / 72;
	lineHeader.m_nWidthInHimetric = 	pLine->m_WidthInPoints * 2530 / 72;
	lineHeader.m_nHeightInHimetric =	pLine->m_HeightInPoints * 2530 / 72;

	lineHeader.m_nTabLevel = SwapWord(lineHeader.m_nTabLevel);
	lineHeader.m_nTabWidthInHimetric = SwapWord(lineHeader.m_nTabWidthInHimetric);
	lineHeader.m_nWidthInHimetric = SwapWord(lineHeader.m_nWidthInHimetric);
	lineHeader.m_nHeightInHimetric = SwapWord(lineHeader.m_nHeightInHimetric);
	lineHeader.m_reserved = SwapLong(lineHeader.m_reserved);

	// write line header
	hrErr = lpDestStm->lpVtbl->Write(
				lpDestStm,
				&lineHeader,
				sizeof(LineHeaderRec),
				&nWritten
			);
	ASSERTNOERROR(hrErr);
	FailOleErr(hrErr);

	lineHeader.m_nTabLevel = SwapWord(lineHeader.m_nTabLevel);
	lineHeader.m_nTabWidthInHimetric = SwapWord(lineHeader.m_nTabWidthInHimetric);
	lineHeader.m_nWidthInHimetric = SwapWord(lineHeader.m_nWidthInHimetric);
	lineHeader.m_nHeightInHimetric = SwapWord(lineHeader.m_nHeightInHimetric);
	lineHeader.m_reserved = SwapLong(lineHeader.m_reserved);

}

// OLDNAME: OleLineList.c

extern ApplicationPtr		gApplication;
extern char					gApplicationName[];

//#pragma segment OleLineListSeg
void LineListSaveSelectionToStorage(LineListPtr pLineList, LineRangePtr pSelection, LPSTORAGE lpSrcStg, LPSTORAGE lpDestStg, LPSTREAM lpLineListStm, Boolean fRemember)
{
	short		startLine;
	short		endLine;
	HRESULT		hrErr;

	if (pSelection == nil)
	{
		startLine = 0;
		endLine = LineListGetCount(pLineList) - 1;
	}
	else
	{
		startLine = pSelection->m_nStartLine;
		endLine = pSelection->m_nEndLine;
	}

	{
		LineListHeaderRec	lineListHeader;
		ULARGE_INTEGER		dlibSaveHeaderPos;
		LARGE_INTEGER		dlibZeroOffset;
		unsigned long		nWritten;
		short				i;
		LinePtr				pLine;

		bzero(&lineListHeader, sizeof(LineListHeaderRec));

		// save seek position for LineList header record
		LISet32(dlibZeroOffset, 0);
		hrErr = lpLineListStm->lpVtbl->Seek(
						lpLineListStm,
						dlibZeroOffset,
						STREAM_SEEK_CUR,
						&dlibSaveHeaderPos
				);
		ASSERTNOERROR(hrErr);
		FailOleErr(hrErr);

		// write LineList header;
		hrErr = lpLineListStm->lpVtbl->Write(
						lpLineListStm,
						&lineListHeader,
						sizeof(LineListHeaderRec),
						&nWritten
				);
		ASSERTNOERROR(hrErr);
		FailOleErr(hrErr);

		for(i = startLine; i<= endLine; i++)
		{
			pLine = LineListGetLine(pLineList, i);

			ASSERTCOND(pLine->vtbl->m_SaveToStorageProcPtr != nil);
			(*pLine->vtbl->m_SaveToStorageProcPtr)(pLine, lpSrcStg, lpDestStg, lpLineListStm, fRemember);

			lineListHeader.m_nNumLines++;
		}

		// restore seek position for LineList header
		hrErr = lpLineListStm->lpVtbl->Seek(
						lpLineListStm,
						*(LARGE_INTEGER *)&dlibSaveHeaderPos,
						STREAM_SEEK_SET,
						NULL
				);
		ASSERTNOERROR(hrErr);
		FailOleErr(hrErr);


		lineListHeader.m_nNumLines = SwapWord(lineListHeader.m_nNumLines);
		lineListHeader.m_reserved1 = SwapLong(lineListHeader.m_reserved1);
		lineListHeader.m_reserved2 = SwapLong(lineListHeader.m_reserved2);

		// write LineList header
		hrErr = lpLineListStm->lpVtbl->Write(
						lpLineListStm,
						&lineListHeader,
						sizeof(LineListHeaderRec),
						&nWritten
				);
		ASSERTNOERROR(hrErr);
		FailOleErr(hrErr);

		lineListHeader.m_nNumLines = SwapWord(lineListHeader.m_nNumLines);
		lineListHeader.m_reserved1 = SwapLong(lineListHeader.m_reserved1);
		lineListHeader.m_reserved2 = SwapLong(lineListHeader.m_reserved2);

		// reset seek position to end of stream
		hrErr = lpLineListStm->lpVtbl->Seek(
						lpLineListStm,
						dlibZeroOffset,
						STREAM_SEEK_END,
						NULL
				);
		ASSERTNOERROR(hrErr);
		FailOleErr(hrErr);
	}
}

//#pragma segment OleLineListSeg
void LineListLoadFromStorage(LineListPtr pLineList, struct OleOutlineDocRec* pOleOutlineDoc, LPSTORAGE lpSrcStg, LPSTREAM lpLineListStm)
{
	HRESULT		hrErr;
	LineListHeaderRec	lineListHeader;
	unsigned long		nRead;
	short				i;
	LinePtr				pLine;
	short				maxWidth;
	short				totalLineWidth;
	short				row;
	
	hrErr = lpLineListStm->lpVtbl->Read(
				lpLineListStm,
				&lineListHeader,
				sizeof(LineListHeaderRec),
				&nRead
			);
	ASSERTCOND(hrErr == noErr);
	FailOleErr(hrErr);

	lineListHeader.m_nNumLines = SwapWord(lineListHeader.m_nNumLines);
	lineListHeader.m_reserved1 = SwapLong(lineListHeader.m_reserved1);
	lineListHeader.m_reserved2 = SwapLong(lineListHeader.m_reserved2);

	maxWidth = 0;

	if (lineListHeader.m_nNumLines > 0) {
		row = 32767;
		if (!TableAddRow(pLineList->m_Lines, lineListHeader.m_nNumLines, &row)) {
			lineListHeader.m_nNumLines = 0;
		}
	}

	for(i = 0; i < lineListHeader.m_nNumLines; i++)
	{
		LineHeaderRec	lineHeader;
		HRESULT			hrErr;
	
		// read header
		hrErr = lpLineListStm->lpVtbl->Read(
					lpLineListStm,
					&lineHeader,
					sizeof(LineHeaderRec),
					&nRead
				);
		ASSERTNOERROR(hrErr);
		FailOleErr(hrErr);
	
		lineHeader.m_nTabLevel = SwapWord(lineHeader.m_nTabLevel);
		lineHeader.m_nTabWidthInHimetric = SwapWord(lineHeader.m_nTabWidthInHimetric);
		lineHeader.m_nWidthInHimetric = SwapWord(lineHeader.m_nWidthInHimetric);
		lineHeader.m_nHeightInHimetric = SwapWord(lineHeader.m_nHeightInHimetric);
		lineHeader.m_reserved = SwapLong(lineHeader.m_reserved);

		ASSERTCOND(lineHeader.m_LineType != kUnkownLineType);
	
		switch((short)lineHeader.m_LineType)
		{
			case kTextLineType:
				{
					TextLinePtr		pTextLine;
					
					pTextLine = (TextLinePtr)NewPtrClear(sizeof(TextLineRec));
					ASSERTCOND(pTextLine != nil);
					FailNIL(pTextLine);
					
					TextLineInit(pTextLine, pLineList);	
					
					TextLineLoadFromStorage(pTextLine, lpSrcStg, lpLineListStm);
					pLine = (LinePtr)pTextLine;
				}
				break;
			
#if qOleContainerApp
			case kContainerLineType:
				{
					OleContainerLinePtr		pOleContainerLine;
					
					pOleContainerLine = (OleContainerLinePtr)NewPtrClear(sizeof(OleContainerLineRec));
					ASSERTCOND(pOleContainerLine != nil);
					FailNIL(pOleContainerLine);
					
					OleContainerLineInit(pOleContainerLine, pLineList);
					OleContainerLineAddRef(pOleContainerLine);
					
					TRY
					{
						OleContainerLineLoadFromStorage(pOleContainerLine, lpSrcStg, lpLineListStm);
					}
					CATCH
					{
						OleContainerLineRelease(pOleContainerLine);
						pOleContainerLine = nil;
						
						NO_PROPAGATE;
					}
					ENDTRY
	
					pLine = (LinePtr)pOleContainerLine;
				}
				break;
#endif
			
			default:
				pLine = nil;
				break;
		}
	
		ASSERTCOND(pLine != nil);
		if (pLine)
		{
			pLine->m_LineType =				lineHeader.m_LineType;
			pLine->m_TabLevel =				lineHeader.m_nTabLevel;
			pLine->m_WidthInPoints =		lineHeader.m_nWidthInHimetric * 72 / 2530;
			pLine->m_HeightInPoints = 		lineHeader.m_nHeightInHimetric * 72 / 2530;
	
			totalLineWidth = pLine->m_WidthInPoints + (pLine->m_TabLevel * pLine->m_TabWidthInPoints);
	
			if (totalLineWidth > maxWidth)
				maxWidth = totalLineWidth;
			
			if (pLine->m_HeightInPoints != TableGetRowHeight(pLineList->m_Lines, i))
				TableSetRowHeight(pLineList->m_Lines, i, pLine->m_HeightInPoints);
			
			LineListSetLine(pLineList, i, pLine);

			pLine->m_fNeedsUpdateView =			true;		// let the line update it's size in case it has changed		
		}
	}
	
	// make sure that the max is greater than the view width
	if (maxWidth < TableGetViewWidth(pLineList->m_Lines))
		maxWidth = TableGetViewWidth(pLineList->m_Lines);
	
	if (maxWidth != TableGetColumnWidth(pLineList->m_Lines, 0))
	{
		ASSERTCOND(maxWidth > 0);
		TableSetColumnWidth(pLineList->m_Lines, 0, maxWidth);
	}
}

#if qOleContainerApp
//#pragma segment OleLineListSeg
void LineListNewContainerLine(LineListPtr pLineList, struct OleOutlineDocRec* pOleOutlineDoc, unsigned long oleCreateType, const CLSID* clsid, FSSpecPtr pFSSpec, PicHandle hIcon, LPSTORAGE pStorage, char* pContainerStorageName)
{
	volatile OleContainerLinePtr	pOleContainerLine;
	HRESULT							hrErr;
	
	pOleContainerLine = (OleContainerLinePtr)NewPtrClear(sizeof(OleContainerLineRec));
	ASSERTCOND(pOleContainerLine != nil);
	FailNIL(pOleContainerLine);
	
	OleContainerLineInit(pOleContainerLine, pLineList);
	OleContainerLineAddRef(pOleContainerLine);
	
	TRY
	{
		// now we need to create the containers stream
		strcpy(pOleContainerLine->m_sStorageName, pContainerStorageName);
		
		hrErr = pStorage->lpVtbl->CreateStorage(
					pStorage,
					pContainerStorageName,
					STGM_READWRITE | STGM_TRANSACTED | STGM_SHARE_EXCLUSIVE,
					0,
					0,
					&pOleContainerLine->m_pStorage
				);
		ASSERTNOERROR(hrErr);
		FailOleErr(hrErr);
		
		if (oleCreateType & IOF_SELECTCREATENEW) {
				hrErr = OleCreate(
							clsid,
							&IID_IOleObject,
							OLERENDER_DRAW,
							NULL,
							(LPOLECLIENTSITE)&pOleContainerLine->m_OleClientSite,
							pOleContainerLine->m_pStorage,
							(void*)&pOleContainerLine->site.m_pOleObj
						);
		}
		else if (oleCreateType & IOF_SELECTCREATEFROMFILE) {
			if (oleCreateType & IOF_CHECKLINK) {
				hrErr = OleCreateLinkToFSp(
							pFSSpec,
							&IID_IOleObject,
							OLERENDER_DRAW,
							NULL,
							(LPOLECLIENTSITE)&pOleContainerLine->m_OleClientSite,
							pOleContainerLine->m_pStorage,
							(void*)&pOleContainerLine->site.m_pOleObj
						);
				pOleContainerLine->m_fIsLink = TRUE;
			}
			else {
				hrErr = OleCreateFromFSp(
							&CLSID_NULL,
							pFSSpec,
							&IID_IOleObject,
							OLERENDER_DRAW,
							NULL,
							(LPOLECLIENTSITE)&pOleContainerLine->m_OleClientSite,
							pOleContainerLine->m_pStorage,
							(void*)&pOleContainerLine->site.m_pOleObj
						);
			}				
		}
		
		ASSERTNOERROR(hrErr);
		FailOleErr(hrErr);

		// setup advises for notification
		OleStdSetupAdvises(
				pOleContainerLine->site.m_pOleObj,
				pOleContainerLine->m_DrawAspect,
				gApplicationName,
				pOleContainerLine->m_sStorageName,
				(LPADVISESINK)&pOleContainerLine->m_AdviseSink2
			);
		
		if (oleCreateType & IOF_CHECKDISPLAYASICON) {
			/* user has requested to display icon aspect instead of content
			**    aspect.
			**    NOTE: we do not have to delete the previous aspect cache
			**    because one did not get set up.
			*/
			OleStdSwitchDisplayAspect(
					pOleContainerLine->site.m_pOleObj,
					&pOleContainerLine->m_DrawAspect,
					DVASPECT_ICON,
					hIcon,
					FALSE,	/* fDeleteOldAspect */
					TRUE,	/* fSetupViewAdvise */
					(LPADVISESINK)&pOleContainerLine->m_AdviseSink2,
					NULL /*fMustUpdate*/		// this can be ignored; update
												// for switch to icon not req'd
			);
		}
		
		if (!LineListAddLine(pLineList, (LinePtr)pOleContainerLine))
			FailNIL(nil);		// trigger exception

		TRY
		{
			if (oleCreateType & IOF_SELECTCREATENEW)
			{
				OleContainerLineDoVerb(pOleContainerLine, OLEIVERB_SHOW, TRUE, TRUE);
		
		        /* OLE2NOTE: we will immediately force a save of the object
		        **    to guarantee that a valid initial object is saved
		        **    with our document. if the object is a OLE 1.0 object,
		        **    then it may exit without update. by forcing this
		        **    initial save we consistently always have a valid
		        **    object even if it is a OLE 1.0 object that exited
		        **    without saving. if we did NOT do this save here, then
		        **    we would have to worry about deleting the object if
		        **    it was a OLE 1.0 object that closed without saving.
		        **    the OLE 2.0 User Model dictates that the object
		        **    should always be valid after CreateNew performed. the
		        **    user must explicitly delete it.
		        */
		        hrErr = OleContainerLineSaveOleObject(
		                		pOleContainerLine,
		                		pOleContainerLine->m_pStorage,
		                		TRUE,	/* fSameAsLoad */
		                		TRUE,	/* fRemember */
		                		TRUE);	/* fForceUpdate */
				ASSERTNOERROR(hrErr);
				FailOleErr(hrErr);
			}
		}
		CATCH
		{
			LineListDeleteLine(pLineList, (LinePtr)pOleContainerLine);
		}
		ENDTRY
	}
	CATCH
	{
		OleContainerLineRelease(pOleContainerLine);
		
		NO_PROPAGATE;
	}
	ENDTRY

}

//#pragma segment OleLineListSeg
void LineListNewContainerLineFromData(LineListPtr pLineList, OleOutlineDocPtr pOleOutlineDoc, LPDATAOBJECT pSrcDataObj, unsigned long oleCreateType, ResType cfFormat, Boolean fDisplayAsIcon, PicHandle hMetaPict, char* pContainerStorageName)
{
	OleContainerLinePtr		pOleContainerLine;
	LinePtr					pLine;
	
	pOleContainerLine = OleContainerLineCreateFromData(pOleOutlineDoc, pSrcDataObj, oleCreateType, cfFormat, fDisplayAsIcon, hMetaPict, pContainerStorageName);
	ASSERTCOND(pOleContainerLine != nil);
	FailNIL(pOleContainerLine);
	
	pLine = (LinePtr)pOleContainerLine;
	if (!LineListAddLine(pLineList, pLine)) {
		ASSERTCOND(pLine->vtbl->m_FreeProcPtr != nil);
		(*pLine->vtbl->m_FreeProcPtr)(pLine);
	}		
}

//#pragma segment OleLineListSeg
HRESULT LineListGetItemObject(LineListPtr pLineList, char* pszItem, unsigned long SpeedNeeded, REFIID riid, void* * ppvObject)
{
	OleContainerLinePtr 	pOleContainerLine;
	
	pOleContainerLine = LineListFindLineByStorageName(pLineList, pszItem);
	if (!pOleContainerLine)
		return ResultFromScode(MK_E_NOOBJECT);
		
	return OleContainerLineGetObject(pOleContainerLine, SpeedNeeded, riid, ppvObject);
}

//#pragma segment OleLineListSeg
HRESULT LineListIsItemRunning(LineListPtr pLineList, char* pszItem)
{
	OleContainerLinePtr 	pOleContainerLine;
	
	pOleContainerLine = LineListFindLineByStorageName(pLineList, pszItem);
	if (!pOleContainerLine)
		return ResultFromScode(MK_E_NOOBJECT);
		
	return OleContainerLineIsRunning(pOleContainerLine);
}

//#pragma segment OleLineListSeg
HRESULT LineListGetItemStorage(LineListPtr pLineList, char* pszItem, void** lplpvStorage)
{
	OleContainerLinePtr 	pOleContainerLine;
	
	pOleContainerLine = LineListFindLineByStorageName(pLineList, pszItem);
	if (!pOleContainerLine)
		return ResultFromScode(MK_E_NOOBJECT);
		
	*lplpvStorage = (void*)OleContainerLineGetStorage(pOleContainerLine);
	
	return NOERROR;
}

//#pragma segment OleLineListSeg
Boolean LineListDoClose(LineListPtr pLineList)
{
	if (!TableIsEmpty(pLineList->m_Lines))
	{
		LinePtr	pLine;
		Cell	theCell;
	
		SetPt(&theCell, 0, 0);

		do{
		
			TableGetCell(pLineList->m_Lines, &pLine, theCell);
			ASSERTCOND(pLine != nil);
			
			if (LineGetType(pLine) == kContainerLineType)
			{
				if (!OleContainerLineCloseOleObject((OleContainerLinePtr)pLine))
					return false;
			}
	
		} while (TableNextCell(pLineList->m_Lines, true, true, &theCell));
	}
	
	return true;
}

//#pragma segment OleLineListSeg
void LineListInformAllOleObjectsDocRenamed(LineListPtr pLineList, LPMONIKER pmkDoc)
{
	Cell					theCell;
	TablePtr 				theTable;
	LinePtr					pLine;

	if (! LineListIsEmpty(pLineList))
	{
		theCell.h = 0;
		theCell.v = 0;
		theTable = pLineList->m_Lines;
			
		do {
			TableGetCell(theTable, &pLine, theCell);
	
	        if (pLine && (LineGetType(pLine) == kContainerLineType)) {
				OleContainerLineInformOleObjectDocRenamed((OleContainerLinePtr)pLine, pmkDoc);
	        }
		} while (TableNextCell(theTable, true, true, &theCell));
	}
}

//#pragma segment OleLineListSeg
OleContainerLinePtr LineListFindLineByStorageName(LineListPtr pLineList, char* StorageName)
{
	Cell					theCell;
	TablePtr 				theTable;
	LinePtr					pLine;
	OleContainerLinePtr 	pOleContainerLine;
	const char*				TestStorageName;

	if (! LineListIsEmpty(pLineList))
	{
		theCell.h = 0;
		theCell.v = 0;
		theTable = pLineList->m_Lines;
			
		do {
			TableGetCell(theTable, &pLine, theCell);
	
	        if (pLine && (LineGetType(pLine) == kContainerLineType)) {
	
	            pOleContainerLine = (OleContainerLinePtr)pLine;
				TestStorageName = OleContainerLineGetStorageName(pOleContainerLine);
				
	            if (!strcmp(StorageName, TestStorageName))
	            	return pOleContainerLine;
	        }
		} while (TableNextCell(theTable, true, true, &theCell));
	}
	
	return nil;
}


//#pragma segment OleLineListSeg
unsigned long LineListGetNextLink(LineListPtr pLineList, unsigned long dwLink)
{
    unsigned long dwNextLink = 0;
    LinePtr pLine;
    static int nIndex = 0;
    int Count;
	
	if (!dwLink)
		nIndex = 0;

	Count = LineListGetCount(pLineList);
	
    for ( ; nIndex < Count; nIndex++) {
        pLine = LineListGetLine(pLineList, (short)nIndex);

        if (pLine
            && (LineGetType(pLine) == kContainerLineType)
            && OleContainerLineIsOleLink((OleContainerLinePtr)pLine)) {
			
			nIndex++;
            OleContainerLineLoadOleObject((OleContainerLinePtr)pLine);
			return (unsigned long)pLine;
		}
	}

    return 0;
}

//#pragma segment OleLineListSeg
void LineListUpdateEditMenu(LineListPtr pLineList, VerbMenuRec* pVerbMenuRec)
{
	LineRangeRec	LineRange;
	LPOLEOBJECT pOleObj = nil;
	MenuHandle 	hEditMenu;
	MenuHandle 	hVerbMenu;
	OleContainerLinePtr pContainerLine = nil;
	LinePtr		pLine = nil;
	short		menuID;
	short		menuItem;
	
	hEditMenu = GetMHandle(kEdit_MENU);
	hVerbMenu = GetMHandle(kObject_MENU);

	if (LineListGetSelection(pLineList, &LineRange) == 1) {		// one line selected
			
		pLine = LineListGetLine(pLineList, LineRange.m_nStartLine);
		ASSERTCOND(pLine != nil);
		
		if (LineGetType(pLine) == kContainerLineType) {
			pContainerLine = (OleContainerLinePtr)pLine;
			
			pOleObj = (LPOLEOBJECT)OleContainerLineGetOleObject(pContainerLine, &IID_IOleObject);
			ASSERTCOND(pOleObj != nil);
		}	
	}

	ASSERTCOND(gApplication->vtbl->m_CmdToMenuItemProcPtr != nil);
	(*gApplication->vtbl->m_CmdToMenuItemProcPtr)(gApplication, cmdObject, &menuID, &menuItem);
	
	OleUIAddVerbMenu(
			(LPOLEOBJECT)pOleObj,
			(pContainerLine ? pContainerLine->m_pszShortType : NULL),
			hEditMenu,
			kEdit_MENU,
			hVerbMenu,
			kObject_MENU,
			menuItem,		// id for Edit/Object
			true,           // Add Convert menu item
			pVerbMenuRec
	);

	if (pOleObj)
		OleStdRelease((LPUNKNOWN)pOleObj);

}

//#pragma segment OleLineListSeg
void LineListDoObjectVerb(LineListPtr pLineList, long verb)
{
	LineRangeRec	LineRange;
	OleContainerLinePtr pContainerLine = nil;
	LinePtr		pLine = nil;
	
	if (LineListGetSelection(pLineList, &LineRange) == 1) {		// one line selected
			
		pLine = LineListGetLine(pLineList, LineRange.m_nStartLine);
		ASSERTCOND(pLine != nil);
		
		if (LineGetType(pLine) == kContainerLineType) {
			pContainerLine = (OleContainerLinePtr)pLine;
			
			OleContainerLineDoVerb(pContainerLine, verb, true, true);
		}	
	}
}

#endif // qOleContainerApp

#if qOleContainerApp
// OLDNAME: OleContaineLineInterface.c
static IUnknownVtbl			gContainerLineUnknownVtbl;
static IOleClientSiteVtbl	gContainerLineOleClientSiteVtbl;
static IAdviseSink2Vtbl		gContainerLineAdviseSink2Vtbl;
static IContainerSiteVtbl	gContainerSiteVtbl;

extern ApplicationPtr		gApplication;

//#pragma segment OleContainerLineInterfaceInitSeg
void OleContainerLineInitInterfaces(void)
{
	// OleContainerLine::IUnknown method table
	{
		IUnknownVtbl*	p;
		
		p = &gContainerLineUnknownVtbl;
		
		p->QueryInterface =					IOleContainerLineUnknownQueryInterface;
		p->AddRef =							IOleContainerLineUnknownAddRef;
		p->Release =						IOleContainerLineUnknownRelease;

		ASSERTCOND(ValidVtbl(((char *)p) + sizeof(p->b), sizeof(*p) - sizeof(p->b)));
	}
	
	// OleContainerLine::IOleClientSite method table
	{
		IOleClientSiteVtbl*		p;
		
		p = &gContainerLineOleClientSiteVtbl;
		
		p->QueryInterface =					IOleContainerLineOleClientSiteQueryInterface;
		p->AddRef =							IOleContainerLineOleClientSiteAddRef;
		p->Release =						IOleContainerLineOleClientSiteRelease;
		p->SaveObject =						IOleContainerLineOleClientSiteSaveObject;
		p->GetMoniker =						IOleContainerLineOleClientSiteGetMoniker;
		p->GetContainer =					IOleContainerLineOleClientSiteGetContainer;
		p->ShowObject =						IOleContainerLineOleClientSiteShowObject;
		p->OnShowWindow =					IOleContainerLineOleClientSiteOnShowWindow;
		p->RequestNewObjectLayout =			IOleContainerLineOleClientSiteRequestNewObjectLayout;

		ASSERTCOND(ValidVtbl(((char *)p) + sizeof(p->b), sizeof(*p) - sizeof(p->b)));
	}
	
	// OleContainerLine::IAdviseSink2 method table
	{
		IAdviseSink2Vtbl*	p;
		
		p = &gContainerLineAdviseSink2Vtbl;
		
		p->QueryInterface =					IOleContainerLineAdviseSink2QueryInterface;
		p->AddRef =							IOleContainerLineAdviseSink2AddRef;
		p->Release =						IOleContainerLineAdviseSink2Release;
		p->OnDataChange =					IOleContainerLineAdviseSink2OnDataChange;
		p->OnViewChange =					IOleContainerLineAdviseSink2OnViewChange;
		p->OnRename =						IOleContainerLineAdviseSink2OnRename;
		p->OnSave =							IOleContainerLineAdviseSink2OnSave;
		p->OnClose =						IOleContainerLineAdviseSink2OnClose;
		p->OnLinkSrcChange =				IOleContainerLineAdviseSink2OnLinkSrcChange;

		ASSERTCOND(ValidVtbl(((char *)p) + sizeof(p->b), sizeof(*p) - sizeof(p->b)));
	}
	
	{
		IContainerSiteVtbl		*p;
		
		p = &gContainerSiteVtbl;
		
		p->m_GetWindowProcPtr =				IOleContainerLineSiteGetWindow;
		p->m_GetOleDocumentProcPtr =		IOleContainerLineSiteGetOleDocument;
		p->m_GetAppCreatorProcPtr =			IOleContainerLineSiteGetAppCreator;
		p->m_GetBoundsProcPtr =				IOleContainerLineSiteGetBounds;
		p->m_GetOleObjectProcPtr = 			IOleContainerLineSiteGetOleObject;
		p->m_GetDrawAspectProcPtr =			IOleContainerLineSiteGetDrawAspect;
		p->m_SetExtentProcPtr =				IOleContainerLineSiteSetExtent;
		
#if qOleInPlace
		p->m_GetClipRgnProcPtr =			IOleContainerLineSiteGetClipRgn;

		p->m_OnPosRectChangeProcPtr =		IOleContainerLineSiteOnPosRectChange;

		p->m_GetIPFrameProcPtr =			IOleContainerLineSiteGetIPFrame;
		p->m_GetIPUIWindowProcPtr =			IOleContainerLineSiteGetIPUIWindow;

		p->m_DrawNowProcPtr =				IOleContainerLineSiteDrawNow;
#endif

		ASSERTCOND(ValidVtbl(p, sizeof(*p)));
	}
}

//#pragma segment OleContainerLineInterfaceSeg
void OleContainerLineIUnknownInit(ContainerLineUnknownImplPtr pContainerLineUnknownImpl, struct OleContainerLineRec* pOleContainerLine)
{
	pContainerLineUnknownImpl->lpVtbl					= &gContainerLineUnknownVtbl;
	pContainerLineUnknownImpl->lpOleContainerLine		= pOleContainerLine;
	pContainerLineUnknownImpl->cRef						= 0;
}

//#pragma segment OleContainerLineInterfaceSeg
STDMETHODIMP IOleContainerLineUnknownQueryInterface(LPUNKNOWN lpThis, REFIID riid, void* * lplpvObj)
{
	OleDbgEnterInterface();
	
	return OleContainerLineQueryInterface(((ContainerLineUnknownImplPtr)lpThis)->lpOleContainerLine, riid, lplpvObj);
}

//#pragma segment OleContainerLineInterfaceSeg
STDMETHODIMP_(unsigned long) IOleContainerLineUnknownAddRef(LPUNKNOWN lpThis)
{
	OleDbgEnterInterface();
	
	return OleContainerLineAddRef(((ContainerLineUnknownImplPtr)lpThis)->lpOleContainerLine);
}

//#pragma segment OleContainerLineInterfaceSeg
STDMETHODIMP_(unsigned long) IOleContainerLineUnknownRelease(LPUNKNOWN lpThis)
{
	OleDbgEnterInterface();

	return OleContainerLineRelease(((ContainerLineUnknownImplPtr)lpThis)->lpOleContainerLine);
}

//#pragma segment OleContainerLineInterfaceSeg
void OleContainerLineIOleClientSiteInit(ContainerLineOleClientSiteImplPtr pContainerLineOleClientSiteImpl, struct OleContainerLineRec* pOleContainerLine)
{
	pContainerLineOleClientSiteImpl->lpVtbl				= &gContainerLineOleClientSiteVtbl;
	pContainerLineOleClientSiteImpl->lpOleContainerLine	= pOleContainerLine;
	pContainerLineOleClientSiteImpl->cRef				= 0;
}

//#pragma segment OleContainerLineInterfaceSeg
STDMETHODIMP IOleContainerLineOleClientSiteQueryInterface(LPOLECLIENTSITE lpThis, REFIID riid, void* * lplpvObj)
{
	OleDbgEnterInterface();

	return OleContainerLineQueryInterface(((ContainerLineOleClientSiteImplPtr)lpThis)->lpOleContainerLine, riid, lplpvObj);
}

//#pragma segment OleContainerLineInterfaceSeg
STDMETHODIMP_(unsigned long) IOleContainerLineOleClientSiteAddRef(LPOLECLIENTSITE lpThis)
{
	OleDbgEnterInterface();

	return OleContainerLineAddRef(((ContainerLineOleClientSiteImplPtr)lpThis)->lpOleContainerLine);
}

//#pragma segment OleContainerLineInterfaceSeg
STDMETHODIMP_(unsigned long) IOleContainerLineOleClientSiteRelease(LPOLECLIENTSITE lpThis)
{
	OleDbgEnterInterface();

	return OleContainerLineRelease(((ContainerLineOleClientSiteImplPtr)lpThis)->lpOleContainerLine);
}

//#pragma segment OleContainerLineInterfaceSeg
STDMETHODIMP IOleContainerLineOleClientSiteSaveObject(LPOLECLIENTSITE lpThis)
{
	OleContainerLinePtr		pOleContainerLine;
	
	pOleContainerLine = ((ContainerLineUnknownImplPtr)lpThis)->lpOleContainerLine;
	
	OleDbgEnterInterface();

	ASSERTCOND(pOleContainerLine->site.m_pOleObj != nil);
	
	return OleContainerLineSaveOleObject(
					pOleContainerLine,
					pOleContainerLine->m_pStorage,
					TRUE,
					FALSE,
					FALSE);
}

//#pragma segment OleContainerLineInterfaceSeg
STDMETHODIMP IOleContainerLineOleClientSiteGetMoniker(LPOLECLIENTSITE lpThis, unsigned long dwAssign, unsigned long dwWhichMoniker, LPMONIKER * ppmk)
{
	OleContainerLinePtr		pOleContainerLine;
	HRESULT					hrErr;
	
	OleDbgEnterInterface();

	pOleContainerLine = ((ContainerLineUnknownImplPtr)lpThis)->lpOleContainerLine;
	hrErr = NOERROR;

    // OLE2NOTE: we must make sure to set output pointer parameters to NULL
    *ppmk = NULL;

    switch (dwWhichMoniker) {

        case OLEWHICHMK_CONTAINER:
            /* OLE2NOTE: create a FileMoniker which identifies the
            **    entire container document.
            */
            *ppmk = OleOutlineDocGetFullMoniker(
                    	(OleOutlineDocPtr)((LinePtr)pOleContainerLine)->m_LineList->m_pDoc,
                    	dwAssign
            		);
            break;

        case OLEWHICHMK_OBJREL:

            /* OLE2NOTE: create an ItemMoniker which identifies the
            **    OLE object relative to the container document.
            */
            *ppmk = OleContainerLineGetRelMoniker(pOleContainerLine, dwAssign);
            break;

        case OLEWHICHMK_OBJFULL:
        {
        	LPMONIKER pmkDoc;
        	LPMONIKER pmkItem;
        	
            /* OLE2NOTE: create an absolute moniker which identifies the
            **    OLE object in the container document. this moniker is
            **    created as a composite of the absolute moniker for the
            **    entire document appended with an item moniker which
            **    identifies the OLE object relative to the document.
            */
			pmkDoc = OleOutlineDocGetFullMoniker(
                    	(OleOutlineDocPtr)((LinePtr)pOleContainerLine)->m_LineList->m_pDoc,
                    	dwAssign
            		 );
		    if (!pmkDoc)
		    	return NULL;
		    	
            pmkItem = OleContainerLineGetRelMoniker(pOleContainerLine, dwAssign);
    		if (pmkItem) {
        		CreateGenericComposite(pmkDoc, pmkItem, (LPMONIKER*)ppmk);
        		OleStdRelease((LPUNKNOWN)pmkItem);
    		}

		    if (pmkDoc)
        		OleStdRelease((LPUNKNOWN)pmkDoc);
            break;
        }
    }

    OLEDBG_END2

    if (*ppmk)
        return NOERROR;
    else
        return ResultFromScode(E_FAIL);
}

//#pragma segment OleContainerLineInterfaceSeg
STDMETHODIMP IOleContainerLineOleClientSiteGetContainer(LPOLECLIENTSITE lpThis, LPOLECONTAINER * ppContainer)
{	
	OleDbgEnterInterface();
	
	return OleOutlineDocQueryInterface(
				(OleOutlineDocPtr)((LinePtr)((ContainerLineOleClientSiteImplPtr)lpThis)->lpOleContainerLine)->m_LineList->m_pDoc,
				&IID_IOleContainer,
				(void*) ppContainer
			);
}

//#pragma segment OleContainerLineInterfaceSeg
STDMETHODIMP IOleContainerLineOleClientSiteShowObject(LPOLECLIENTSITE lpThis)
{
	OleContainerLinePtr		pOleContainerLine;
	DocumentPtr				pDoc;
	
	pOleContainerLine = ((ContainerLineAdviseSink2ImplPtr)lpThis)->lpOleContainerLine;
	pDoc = ((LinePtr)pOleContainerLine)->m_LineList->m_pDoc;
	
	OleDbgEnterInterface();

    /* if our doc window is not already visible, then make it visible.
    **    the OutlineDoc_ShowWindow function will cause the app window
    **    to show itself if it is NOT currently visible.
    */
    ASSERTCOND(pDoc->vtbl->m_ShowProcPtr != nil);
    (*pDoc->vtbl->m_ShowProcPtr)(pDoc);
	
    /* make sure that the OLE object is currently in view. if necessary
    **    scroll the document in order to bring it into view.
    */
	LineListShowLine(((LinePtr)pOleContainerLine)->m_LineList, (LinePtr)pOleContainerLine);

	return NOERROR;
}

//#pragma segment OleContainerLineInterfaceSeg
STDMETHODIMP IOleContainerLineOleClientSiteOnShowWindow(LPOLECLIENTSITE lpThis, unsigned long fShow)
{
	OleContainerLinePtr		pOleContainerLine;

	pOleContainerLine = ((ContainerLineAdviseSink2ImplPtr)lpThis)->lpOleContainerLine;

	OleDbgEnterInterface();

	/* OLE2NOTE: if fShow we need to hatch out the OLE object now; it has
	**    just been opened in a window elsewhere (open editing as
	**    opposed to in-place activation).
	**    force the line to re-draw with the hatch.
	*/
	
	/* OLE2NOTE: else if !fShow the object associated with this container site has
	**    just closed its server window. we should now remove the
	**    hatching that indicates that the object is open
	**    elsewhere. also our window should now come to the top.
	**    force the line to re-draw without the hatch.
	*/

	pOleContainerLine->m_fWindowOpen = fShow;
	
	// force redraw
	// LineInval((LinePtr)pOleContainerLine);
	LineDrawNow((LinePtr)pOleContainerLine);

	if (!fShow)
	{
		DocumentPtr		pDoc;
		
		pDoc = ((LinePtr)pOleContainerLine)->m_LineList->m_pDoc;
		ASSERTCOND(pDoc != nil);
		
		ASSERTCOND(pDoc->vtbl->m_BringToFrontProcPtr != nil);
		(*pDoc->vtbl->m_BringToFrontProcPtr)(pDoc);
	}

	return NOERROR;
}

//#pragma segment OleContainerLineInterfaceSeg
STDMETHODIMP IOleContainerLineOleClientSiteRequestNewObjectLayout(LPOLECLIENTSITE lpThis)
{
	OleDbgEnterInterface();

	/* OLE2NOTE: this method is NOT yet used. it is for future layout
	**    negotiation support.
	*/

	return ResultFromScode(E_NOTIMPL);
}

//#pragma segment OleContainerLineInterfaceSeg
void OleContainerLineIAdviseSink2Init(ContainerLineAdviseSink2ImplPtr pContainerLineAdviseSink2Impl, struct OleContainerLineRec* pOleContainerLine)
{
	pContainerLineAdviseSink2Impl->lpVtbl				= &gContainerLineAdviseSink2Vtbl;
	pContainerLineAdviseSink2Impl->lpOleContainerLine	= pOleContainerLine;
	pContainerLineAdviseSink2Impl->cRef					= 0;
}

//#pragma segment OleContainerLineInterfaceSeg
STDMETHODIMP IOleContainerLineAdviseSink2QueryInterface(LPADVISESINK2 lpThis, REFIID riid, void* * lplpvObj)
{
	OleDbgEnterInterface();

	return OleContainerLineQueryInterface(((ContainerLineAdviseSink2ImplPtr)lpThis)->lpOleContainerLine, riid, lplpvObj);
}

//#pragma segment OleContainerLineInterfaceSeg
STDMETHODIMP_(unsigned long) IOleContainerLineAdviseSink2AddRef(LPADVISESINK2 lpThis)
{
	OleDbgEnterInterface();

	return OleContainerLineAddRef(((ContainerLineAdviseSink2ImplPtr)lpThis)->lpOleContainerLine);
}

//#pragma segment OleContainerLineInterfaceSeg
STDMETHODIMP_(unsigned long) IOleContainerLineAdviseSink2Release(LPADVISESINK2 lpThis)
{
	OleDbgEnterInterface();

	return OleContainerLineRelease(((ContainerLineAdviseSink2ImplPtr)lpThis)->lpOleContainerLine);
}

//#pragma segment OleContainerLineInterfaceSeg
STDMETHODIMP_(void) IOleContainerLineAdviseSink2OnDataChange(LPADVISESINK2 lpThis, FORMATETC * pFormatetc, STGMEDIUM * pStgmed)
{
	OleDbgEnterInterface();

    // We are not interested in data changes (only view changes)
    //      (ie. nothing to do)
}

//#pragma segment OleContainerLineInterfaceSeg
STDMETHODIMP_(void) IOleContainerLineAdviseSink2OnViewChange(LPADVISESINK2 lpThis, unsigned long dwAspect, long lindex)
{
	OleContainerLinePtr		pOleContainerLine;
	LineListPtr				pLineList;
	DocumentPtr				pDoc;
	
	pOleContainerLine = ((ContainerLineAdviseSink2ImplPtr)lpThis)->lpOleContainerLine;
	pLineList = ((LinePtr)pOleContainerLine)->m_LineList;
	pDoc = LineListGetDoc(pLineList);
	
	OleDbgEnterInterface();
	
	// force a delayed update of the line
	((LinePtr)pOleContainerLine)->m_fNeedsUpdateView = true;
	
	// the extent may have changed
	pOleContainerLine->m_fDoGetExtent = true;
	
	// make sure we get do an idle soon
	gApplication->m_fNeedsIdle = true;
	
	// make the document dirty
	ASSERTCOND(pDoc->vtbl->m_SetDirtyProcPtr != nil);
	(*pDoc->vtbl->m_SetDirtyProcPtr)(pDoc, true);
}

//#pragma segment OleContainerLineInterfaceSeg
STDMETHODIMP_(void) IOleContainerLineAdviseSink2OnRename(LPADVISESINK2 lpThis, LPMONIKER pmk)
{
	OleDbgEnterInterface();

    /* OLE2NOTE: the Embedding Container has nothing to do here. this
    **    notification is important for linking situations. it tells
    **    the OleLink objects to update their moniker because the
    **    source object has been renamed (track the link source).
    */
}

//#pragma segment OleContainerLineInterfaceSeg
STDMETHODIMP_(void) IOleContainerLineAdviseSink2OnSave(LPADVISESINK2 lpThis)
{
	OleDbgEnterInterface();

    /* OLE2NOTE: the Embedding Container has nothing to do here. this
    **    notification is only useful to clients which have set up a
    **    data cache with the ADVFCACHE_ONSAVE flag.
    */
}

//#pragma segment OleContainerLineInterfaceSeg
STDMETHODIMP_(void) IOleContainerLineAdviseSink2OnClose(LPADVISESINK2 lpThis)
{
	OleDbgEnterInterface();

    /* OLE2NOTE: the Embedding Container has nothing to do here. this
    **    notification is important for the OLE's default object handler
    **    and the OleLink object. it tells them the remote object is
    **    shutting down.
    */
}

//#pragma segment OleContainerLineInterfaceSeg
STDMETHODIMP_(void) IOleContainerLineAdviseSink2OnLinkSrcChange(LPADVISESINK2 lpThis, LPMONIKER pmk)
{
	OleContainerLinePtr		pOleContainerLine;
	LineListPtr				pLineList;
	DocumentPtr				pDoc;
	
	pOleContainerLine = ((ContainerLineAdviseSink2ImplPtr)lpThis)->lpOleContainerLine;
	pLineList = ((LinePtr)pOleContainerLine)->m_LineList;
	pDoc = LineListGetDoc(pLineList);
	OleDbgEnterInterface();

    /* OLE2NOTE: the Embedding Container needs to set the dirty flag so that the new link
    **    information will be saved when the document closes
    */
	ASSERTCOND(pDoc->vtbl->m_SetDirtyProcPtr != nil);
	(*pDoc->vtbl->m_SetDirtyProcPtr)(pDoc, true);
}

//#pragma segment OleContainerLineInterfaceSeg
void OleContainerLineIContainerSiteInit(ContainerLineSiteImplPtr pContainerLineSiteImpl, struct OleContainerLineRec* pOleContainerLine)
{
	pContainerLineSiteImpl->lpVtbl =				&gContainerSiteVtbl;
	pContainerLineSiteImpl->lpOleContainerLine =	pOleContainerLine;		
}

//#pragma segment OleContainerLineInterfaceSeg
WindowPtr IOleContainerLineSiteGetWindow(ContainerSiteImplPtr lpThis)
{
	OleContainerLinePtr		pOleContainerLine;
	
	pOleContainerLine = ((ContainerLineSiteImplPtr)lpThis)->lpOleContainerLine;

	return LineGetWindow((LinePtr)pOleContainerLine);
}

//#pragma segment OleContainerLineInterfaceSeg
OleDocumentPtr IOleContainerLineSiteGetOleDocument(ContainerSiteImplPtr lpThis)
{
	OleContainerLinePtr		pOleContainerLine;
	
	pOleContainerLine = ((ContainerLineSiteImplPtr)lpThis)->lpOleContainerLine;

	return &((OleOutlineDocPtr)(((LinePtr)pOleContainerLine)->m_LineList->m_pDoc))->m_OleDoc;
}

//#pragma segment OleContainerLineInterfaceSeg
OSType IOleContainerLineSiteGetAppCreator(ContainerSiteImplPtr lpThis)
{
	return kOutlineInPlaceContainerType;
}

//#pragma segment OleContainerLineInterfaceSeg
void IOleContainerLineSiteGetBounds(ContainerSiteImplPtr lpThis, Rect* rBounds)
{
	OleContainerLinePtr		pOleContainerLine;
	
	pOleContainerLine = ((ContainerLineSiteImplPtr)lpThis)->lpOleContainerLine;
	
	LineGetBounds((LinePtr)pOleContainerLine, rBounds);
}

//#pragma segment OleContainerLineInterfaceSeg
LPUNKNOWN IOleContainerLineSiteGetOleObject(ContainerSiteImplPtr lpThis, REFIID riid)
{
	OleContainerLinePtr		pOleContainerLine;
	
	pOleContainerLine = ((ContainerLineSiteImplPtr)lpThis)->lpOleContainerLine;
	
	return OleContainerLineGetOleObject(pOleContainerLine, riid);
}

//#pragma segment OleContainerLineInterfaceSeg
unsigned long IOleContainerLineSiteGetDrawAspect(ContainerSiteImplPtr lpThis)
{
	OleContainerLinePtr		pOleContainerLine;
	
	pOleContainerLine = ((ContainerLineSiteImplPtr)lpThis)->lpOleContainerLine;
	
	return pOleContainerLine->m_DrawAspect;
}

//#pragma segment OleContainerLineInterfaceSeg
void IOleContainerLineSiteSetExtent(ContainerSiteImplPtr lpThis, LPSIZEL lpsizel)
{
	OleContainerLinePtr		pOleContainerLine;
	
	pOleContainerLine = ((ContainerLineSiteImplPtr)lpThis)->lpOleContainerLine;
	
	OleContainerLineSetExtent((LinePtr)pOleContainerLine, lpsizel);
}

#if qOleInPlace
//#pragma segment OleContainerLineInterfaceSeg
void IOleContainerLineSiteGetClipRgn(ContainerSiteImplPtr lpThis, RgnHandle clipRgn)
{
	LinePtr		pLine;
	Rect		r;
	
	ASSERTCOND(clipRgn != nil);
	
	pLine = (LinePtr)((ContainerLineSiteImplPtr)lpThis)->lpOleContainerLine;
	
	LineListGetView(pLine->m_LineList, &r);
	LocalToGlobalRect(&r);
	
	RectRgn(clipRgn, &r);
}

//#pragma segment OleContainerLineInterfaceSeg
void IOleContainerLineSiteOnPosRectChange(ContainerSiteImplPtr lpThis, Rect* rBounds)
{
	OleContainerLinePtr		pOleContainerLine;
	
	pOleContainerLine = ((ContainerLineSiteImplPtr)lpThis)->lpOleContainerLine;
	
	LineSetBounds((LinePtr)pOleContainerLine, rBounds);
	
	LineInval((LinePtr)pOleContainerLine);
}

//#pragma segment OleContainerLineInterfaceSeg
LPOLEINPLACEFRAME IOleContainerLineSiteGetIPFrame(ContainerSiteImplPtr lpThis)
{
	OleOutlineAppPtr		pOleOutlineApp;
	
	pOleOutlineApp = (OleOutlineAppPtr)gApplication;
	
	return OleInPlaceContainerAppGetIPFrame(&pOleOutlineApp->m_OleApp);
}

//#pragma segment OleContainerLineInterfaceSeg
LPOLEINPLACEUIWINDOW IOleContainerLineSiteGetIPUIWindow(ContainerSiteImplPtr lpThis)
{
	OleContainerLinePtr		pOleContainerLine;
	
	pOleContainerLine = ((ContainerLineSiteImplPtr)lpThis)->lpOleContainerLine;
	
	return OleInPlaceContainerDocGetIPUIWindow(&((OleOutlineDocPtr)((LinePtr)pOleContainerLine)->m_LineList->m_pDoc)->m_OleDoc);
}

//#pragma segment OleContainerLineInterfaceSeg
void IOleContainerLineSiteDrawNow(ContainerSiteImplPtr lpThis)
{
	OleContainerLinePtr		pOleContainerLine;
	
	pOleContainerLine = ((ContainerLineSiteImplPtr)lpThis)->lpOleContainerLine;
	
	LineDrawNow((LinePtr)pOleContainerLine);
}
#endif

// OLDNAME: OleContainerLine.c
extern CursHandle			gWatchCursor;
extern ApplicationPtr		gApplication;
extern char					gApplicationName[];

static OleContainerLineVtblPtr	gOleContainerLineVtbl = nil;

//#pragma segment VtblInitSeg
OleContainerLineVtblPtr OleContainerLineGetVtbl()
{
	ASSERTCOND(gOleContainerLineVtbl != nil);
	return gOleContainerLineVtbl;
}

//#pragma segment VtblInitSeg
void OleContainerLineInitVtbl()
{
	OleContainerLineVtblPtr		vtbl;
	
	vtbl = gOleContainerLineVtbl = (OleContainerLineVtblPtr)NewPtrClear(sizeof(*vtbl));
	ASSERTCOND(vtbl != nil);
	FailNIL(vtbl);

	InheritFromVtbl(vtbl, LineGetVtbl());

	vtbl->m_DisposeProcPtr =			OleContainerLineDispose;
	vtbl->m_FreeProcPtr =				OleContainerLineFree;
	
	vtbl->m_UpdateViewProcPtr =			OleContainerLineUpdateView;
	
	vtbl->m_DrawProcPtr =				OleContainerLineDraw;
	vtbl->m_DrawSelectedProcPtr =		OleContainerLineDrawSelected;
	
	vtbl->m_DoDoubleClickProcPtr =		OleContainerLineDoDoubleClick;
	
	vtbl->m_SaveToStorageProcPtr =		OleContainerLineSaveToStorage;
	
	vtbl->m_CopyToDocProcPtr =			OleContainerLineCopyToDoc;
	
	vtbl->m_GetTextProcPtr = 			OleContainerLineGetText;
	
	vtbl->m_SetExtentProcPtr =			OleContainerLineSetExtent;

	ASSERTCOND(ValidVtbl(vtbl, sizeof(*vtbl)));
}

//#pragma segment VtblDisposeSeg
void OleContainerLineDisposeVtbl()
{
	ASSERTCOND(gOleContainerLineVtbl != nil);
	DisposePtr((Ptr)gOleContainerLineVtbl);
}

//#pragma segment OleContainerLineSeg
void OleContainerLineInit(OleContainerLinePtr pOleContainerLine, LineListPtr pLineList)
{
	LineInit(&pOleContainerLine->superClass, pLineList);

	pOleContainerLine->vtbl = ((LinePtr)pOleContainerLine)->vtbl;
	((LinePtr)pOleContainerLine)->vtbl = OleContainerLineGetVtbl();
	
	OleContainerLineIUnknownInit(&pOleContainerLine->m_Unknown, pOleContainerLine);
	OleContainerLineIOleClientSiteInit(&pOleContainerLine->m_OleClientSite, pOleContainerLine);
	OleContainerLineIAdviseSink2Init(&pOleContainerLine->m_AdviseSink2, pOleContainerLine);
	OleContainerLineIContainerSiteInit(&pOleContainerLine->m_ContainerSite, pOleContainerLine);
	
	OleContainerSiteInit(&pOleContainerLine->site, (struct ContainerSiteImpl*)&pOleContainerLine->m_ContainerSite, (IUnknown*)&pOleContainerLine->m_Unknown);
	
	((LinePtr)pOleContainerLine)->m_LineType	= kContainerLineType;

	pOleContainerLine->m_cRef 					= 0;
	
	pOleContainerLine->m_fWindowOpen			= false;
	pOleContainerLine->m_fMonikerAssigned		= false;
	pOleContainerLine->m_fDoGetExtent 			= true;
	pOleContainerLine->m_fDoSetExtent			= false;
	pOleContainerLine->m_DrawAspect				= DVASPECT_CONTENT;
	pOleContainerLine->m_fIsLink				= false;
	pOleContainerLine->m_fLinkUnavailable		= false;
	pOleContainerLine->m_sStorageName[0]		= '\0';
	pOleContainerLine->m_pszShortType			= nil;

	pOleContainerLine->m_pStorage				= nil;
}

//#pragma segment OleContainerLineSeg
void OleContainerLineDispose(LinePtr pLine)
{
	OleContainerLinePtr		pOleContainerLine;
	
	pOleContainerLine = (OleContainerLinePtr)pLine;
	
	OleContainerSiteDispose(&pOleContainerLine->site);
	
	if (pOleContainerLine->m_pStorage)
	{
		OleStdRelease((LPUNKNOWN)pOleContainerLine->m_pStorage);
		pOleContainerLine->m_pStorage = nil;
	}
	
	if (pOleContainerLine->m_pszShortType)
	{
		OleStdFreeString(pOleContainerLine->m_pszShortType, NULL);
		pOleContainerLine->m_pszShortType = nil;
	}
	
	ASSERTCOND(pOleContainerLine->site.m_pOleObj == nil);
	ASSERTCOND(pOleContainerLine->site.m_pViewObj == nil);
	ASSERTCOND(pOleContainerLine->site.m_pPersistStorage == nil);
	
	ASSERTCOND(pOleContainerLine->vtbl->m_DisposeProcPtr != nil);
	(*pOleContainerLine->vtbl->m_DisposeProcPtr)((LinePtr)pOleContainerLine);
}

//#pragma segment OleContainerLineSeg
void OleContainerLineFree(LinePtr pLine)
{	
	OleContainerLinePtr		pOleContainerLine;
	
	pOleContainerLine = (OleContainerLinePtr)pLine;
	
    /* OLE2NOTE: in order to have a stable line object during the
    **    process of deleting, we intially AddRef the line ref cnt and
    **    later Release it. This initial AddRef is artificial; it is
    **    simply done to guarantee that our object does not destroy
    **    itself until the END of this routine.
    */
	OleContainerLineAddRef(pOleContainerLine);
	
    // Unload the loaded OLE object
    OleContainerLineUnloadOleObject(pOleContainerLine, OLECLOSE_NOSAVE);

    /* OLE2NOTE: we can NOT directly free the memory for the ContainerLine
    **    data structure until everyone holding on to a pointer to our
    **    ClientSite interface and IAdviseSink interface has released
    **    their pointers. There is one refcnt on the ContainerLine object
    **    which is held by the container itself. we will release this
	**    refcnt here.
    */
	OleContainerLineRelease(pOleContainerLine);

	// release external AddRefs
	CoDisconnectObject((LPUNKNOWN)&pOleContainerLine->m_Unknown, 0);
	
	OleContainerLineRelease(pOleContainerLine);
}

//#pragma segment OleContainerLineSeg
OleContainerLinePtr OleContainerLineCreateFromData(
		OleOutlineDocPtr 	pOleOutlineDoc,
		LPDATAOBJECT 		pSrcDataObj,
		unsigned long 		oleCreateType,
		ResType				cfFormat,
		Boolean 			fDisplayAsIcon,
		PicHandle 			hMetaPict,
		char* 				pContainerStorageName
)
{
	volatile OleContainerLinePtr	pOleContainerLine = nil;
	LineListPtr 					pLineList;
	LPSTORAGE 						pStorage;
	HRESULT							hrErr;
	FSSpec							fileSP;
	unsigned long					OleRenderOpt;
    FORMATETC       				renderFmtEtc;
    LPFORMATETC     				pRenderFmtEtc = NULL;
	
    if (oleCreateType == OLECREATEFROMDATA_STATIC && cfFormat != 0) {
        // a particular type of static object should be created

        OleRenderOpt = OLERENDER_FORMAT;
        pRenderFmtEtc = (LPFORMATETC)&renderFmtEtc;

        if (cfFormat == 'PICT')
            SETDEFAULTFORMATETC(renderFmtEtc, cfFormat, TYMED_MFPICT);
        else if (cfFormat == 'BMAP')
            SETDEFAULTFORMATETC(renderFmtEtc, cfFormat, TYMED_GDI);
        else
            SETDEFAULTFORMATETC(renderFmtEtc, cfFormat, TYMED_HGLOBAL);

    } else if (oleCreateType == OLECREATEFROMDATA_STATIC && fDisplayAsIcon) {
        // a link that currently displayed as an icon needs to be
        // converted to a STATIC picture object. this case is driven
        // from "BreakLink" in the "Links" dialog. because the current
        // data in the source object's cache is DVASPECT_ICON we need
        // to tell the OleCreateStaticFromData API to look for
        // DVASPECT_ICON data. the static object that results however,
        // is considered to be displayed in the DVASPECT_CONTENT view.

        OleRenderOpt = OLERENDER_DRAW;
        pRenderFmtEtc = (LPFORMATETC)&renderFmtEtc;
        SETFORMATETC(renderFmtEtc, 0, DVASPECT_ICON, NULL, TYMED_NULL, -1);

    } else if (fDisplayAsIcon && hMetaPict) {
        // a special icon should be used. first we create the object
        // OLERENDER_NONE and then we stuff the special icon into the cache.

        OleRenderOpt = OLERENDER_NONE;

    } else if (fDisplayAsIcon && hMetaPict == NULL) {
        // the object's default icon should be used

        OleRenderOpt = OLERENDER_DRAW;
        pRenderFmtEtc = (LPFORMATETC)&renderFmtEtc;
        SETFORMATETC(renderFmtEtc, 0, DVASPECT_ICON, NULL, TYMED_NULL, -1);

    } else {
        // create standard DVASPECT_CONTENT/OLERENDER_DRAW object
        OleRenderOpt = OLERENDER_DRAW;
    }
	
	pOleContainerLine = (OleContainerLinePtr)NewPtrClear(sizeof(OleContainerLineRec));
	ASSERTCOND(pOleContainerLine != nil);
	FailNIL(pOleContainerLine);

	pLineList = OutlineDocGetLineList((OutlineDocPtr)pOleOutlineDoc);
		
	OleContainerLineInit(pOleContainerLine, pLineList);
	
	pStorage = OleOutlineDocGetStorage(pOleOutlineDoc);
	ASSERTCOND(pStorage != nil);
	
	// need to addref so that we do not have the line going away
	OleContainerLineAddRef(pOleContainerLine);
	
	TRY
	{
		// now we need to create the containers stream
		strcpy(pOleContainerLine->m_sStorageName, pContainerStorageName);
		
		hrErr = pStorage->lpVtbl->CreateStorage(
					pStorage,
					pContainerStorageName,
					STGM_READWRITE | STGM_TRANSACTED | STGM_SHARE_EXCLUSIVE,
					0,
					0,
					&pOleContainerLine->m_pStorage
				);
		ASSERTNOERROR(hrErr);
		FailOleErr(hrErr);
		
		switch (oleCreateType)
		{
			case OLECREATEFROMDATA_LINK:
				hrErr = OleCreateLinkFromData(
							pSrcDataObj,
							&IID_IOleObject,
							OleRenderOpt,
							pRenderFmtEtc,
							(LPOLECLIENTSITE)&pOleContainerLine->m_OleClientSite,
							pOleContainerLine->m_pStorage,
							(void*)&pOleContainerLine->site.m_pOleObj
						);
				
				pOleContainerLine->m_fIsLink = TRUE;
				
				break;
				
			case OLECREATEFROMDATA_OBJECT:
				{
					LPUNKNOWN	lpUnk;
					
					hrErr = OleCreateFromData(
								pSrcDataObj,
								&IID_IOleObject,
								OleRenderOpt,
								pRenderFmtEtc,
								(LPOLECLIENTSITE)&pOleContainerLine->m_OleClientSite,
								pOleContainerLine->m_pStorage,
								(void*)&pOleContainerLine->site.m_pOleObj
							);
				
					lpUnk = OleContainerLineGetOleObject(pOleContainerLine, &IID_IOleLink);
					if (lpUnk)
					{
						OleStdRelease(lpUnk);
						pOleContainerLine->m_fIsLink = TRUE;
					}
					else
						pOleContainerLine->m_fIsLink = FALSE;
				}
				break;
				
			case OLECREATEFROMDATA_STATIC:
				hrErr = OleCreateStaticFromData(
							pSrcDataObj,
							&IID_IOleObject,
							OleRenderOpt,
							pRenderFmtEtc,
							(LPOLECLIENTSITE)&pOleContainerLine->m_OleClientSite,
							pOleContainerLine->m_pStorage,
							(void*)&pOleContainerLine->site.m_pOleObj
						);
				
				pOleContainerLine->m_fIsLink = FALSE;
				
				break;
		}
		
		ASSERTNOERROR(hrErr);
		FailOleErr(hrErr);

		DocGetFSSpec((DocumentPtr)pOleOutlineDoc, &fileSP);
		
		// setup advises for notification
		OleStdSetupAdvises(
				pOleContainerLine->site.m_pOleObj,
				pOleContainerLine->m_DrawAspect,
				gApplicationName,
				(char*)p2cstr(fileSP.name),
				(LPADVISESINK)&pOleContainerLine->m_AdviseSink2
			);
			
		if (fDisplayAsIcon) {
			/* user has requested to display icon aspect instead of content
			**    aspect.
			**    NOTE: we do not have to delete the previous aspect cache
			**    because one did not get set up.
			*/
			OleStdSwitchDisplayAspect(
					pOleContainerLine->site.m_pOleObj,
					&pOleContainerLine->m_DrawAspect,
					DVASPECT_ICON,
					hMetaPict,
					FALSE,	/* fDeleteOldAspect */
					TRUE,	/* fSetupViewAdvise */
					(LPADVISESINK)&pOleContainerLine->m_AdviseSink2,
					NULL /*fMustUpdate*/		// this can be ignored; update
												// for switch to icon not req'd
			);
		}
			
	}
	CATCH
	{
		OleContainerLineRelease(pOleContainerLine);
		return nil;
	}
	ENDTRY

	return pOleContainerLine;
}

//#pragma segment OleContainerLineSeg
void OleContainerLineSetExtent(LinePtr pLine, LPSIZEL pSizeL)
{
	HRESULT 		hrErr;
	unsigned long	dwStatus;
	Boolean			fMustClose = false;
	
	OleContainerLinePtr pOleContainerLine = (OleContainerLinePtr)pLine;
	
	if (!pOleContainerLine->site.m_pOleObj)
		OleContainerLineLoadOleObject(pOleContainerLine);

	/* OLE2NOTE: if the OLE object is already running then we can
	**    immediately call SetExtent. But, if the object is NOT
	**    currently running then we will check if the object
	**    indicates that it is normally recomposes itself on
	**    resizing. ie. that the object does not simply scale its
	**    display when it it resized. if so then we will force the
	**    object to run so that we can call IOleObject::SetExtent.
	**    SetExtent does not have any effect if the object is only
	**    loaded. if the object does NOT indicate that it
	**    recomposes on resize (OLEMISC_RECOMPOSEONRESIZE) then we
	**    will wait till the next time that the object is run to
	**    call SetExtent. we will store a flag in the ContainerLine
	**    to indicate that a SetExtent is necessary. It is
	**    necessary to persist this flag.
	*/
	if (!OleIsRunning(pOleContainerLine->site.m_pOleObj)) {
		
		hrErr = pOleContainerLine->site.m_pOleObj->lpVtbl->GetMiscStatus(
				pOleContainerLine->site.m_pOleObj,
				pOleContainerLine->m_DrawAspect,
				&dwStatus);

		if (hrErr == NOERROR && (dwStatus & OLEMISC_RECOMPOSEONRESIZE)) {
			// force the object to run
			OleContainerLineRunOleObject(pOleContainerLine);
			fMustClose = true;
		}
	}
		
	hrErr = pOleContainerLine->site.m_pOleObj->lpVtbl->SetExtent(
					pOleContainerLine->site.m_pOleObj,
					pOleContainerLine->m_DrawAspect,
					pSizeL);
			
	if (GetScode(hrErr) == OLE_E_NOTRUNNING)
		pOleContainerLine->m_fDoSetExtent = true;
	
	if (fMustClose)
		OleContainerLineCloseOleObject(pOleContainerLine);
	
	ASSERTCOND(pOleContainerLine->vtbl->m_SetExtentProcPtr != nil);
	(*pOleContainerLine->vtbl->m_SetExtentProcPtr)(pLine, pSizeL);
}

//#pragma segment OleContainerLineSeg
void OleContainerLineUpdateExtent(OleContainerLinePtr pOleContainerLine)
{
	HRESULT		hrErr;
	SIZEL		sizel;
	LinePtr		pLine;
	LineListPtr	pLineList;
	DocumentPtr	pDoc;
	
	pLine = (LinePtr)pOleContainerLine;

	// the object is not loaded
	if (pOleContainerLine->site.m_pOleObj == nil)
	{
		pLine->m_HeightInPoints = kDefaultObjectHeight;
		pLine->m_WidthInPoints = kDefaultObjectWidth;
		
		return;
	}

	pOleContainerLine->m_fDoGetExtent = false;
	
	hrErr = pOleContainerLine->site.m_pOleObj->lpVtbl->GetExtent(
					pOleContainerLine->site.m_pOleObj,
					pOleContainerLine->m_DrawAspect,
					&sizel
			);
	ASSERTCOND(hrErr == NOERROR || hrErr == ResultFromScode(OLE_E_BLANK));
	
	if (hrErr == NOERROR)
	{
		// if the size has changed, need to mark
		if (sizel.cx != pLine->m_WidthInPoints || sizel.cy != pLine->m_HeightInPoints)
		{
			pLineList = pLine->m_LineList;
			pDoc = LineListGetDoc(pLineList);
			
			ASSERTCOND(pDoc->vtbl->m_SetDirtyProcPtr != nil);
			(*pDoc->vtbl->m_SetDirtyProcPtr)(pLine->m_LineList->m_pDoc, true);
		}
	}

	// if there is nothing in the object make sure that we have a default size
	if (sizel.cx <= 0)
		sizel.cx = kDefaultObjectWidth;
		
	if (sizel.cy <= 0)
		sizel.cy = kDefaultObjectHeight;

	pLine->m_HeightInPoints = sizel.cy;
	pLine->m_WidthInPoints = sizel.cx;
}

//#pragma segment OleContainerLineSeg
void OleContainerLineUpdateView(LinePtr pLine)
{
	OleContainerLinePtr		pOleContainerLine;
	
	pOleContainerLine = (OleContainerLinePtr)pLine;

	{
		HRESULT					hrErr;
	
		if (!pOleContainerLine->site.m_pOleObj)
			OleContainerLineLoadOleObject(pOleContainerLine);
			
		if (!pOleContainerLine->site.m_pViewObj)
		{
			hrErr = pOleContainerLine->site.m_pOleObj->lpVtbl->QueryInterface(
							pOleContainerLine->site.m_pOleObj,
							&IID_IViewObject,
							(void*)&pOleContainerLine->site.m_pViewObj
					);
			ASSERTNOERROR(hrErr);
			ASSERTCOND(pOleContainerLine->site.m_pViewObj != nil);
		}
	}

	if (pOleContainerLine->m_fDoGetExtent)
		OleContainerLineUpdateExtent(pOleContainerLine);

	ASSERTCOND(pOleContainerLine->vtbl->m_UpdateViewProcPtr != nil);
	(*pOleContainerLine->vtbl->m_UpdateViewProcPtr)(pLine);
}

//#pragma segment OleContainerLineSeg
void OleContainerLineDraw(LinePtr pLine, Rect* r)
{
	OleContainerLinePtr		pOleContainerLine;
	GrafPtr					currentPort;
	RECTL					lr;

	if (pLine->m_fNeedsUpdateView)
		return;

    GetPort(&currentPort);
	pOleContainerLine = (OleContainerLinePtr)pLine;

#if qOleInPlace

	if (OleContainerSiteIsUIVisible(&pOleContainerLine->site))
		return;
	
	if (pOleContainerLine->site.m_pViewObj == nil)
	{
		pLine->m_fNeedsUpdateView = true;
		return;
	}

#endif

#if qOleInPlace
	if (!OleContainerSiteIsUIVisible(&pOleContainerLine->site))
#endif
	{
		lr.left = r->left;
		lr.top = r->top;
		lr.right = r->right;
		lr.bottom = r->bottom;

		ASSERTCOND(pOleContainerLine->site.m_pViewObj != nil);
		pOleContainerLine->site.m_pViewObj->lpVtbl->Draw(
						pOleContainerLine->site.m_pViewObj,
						pOleContainerLine->m_DrawAspect,
						-1,
						NULL,
						NULL,
						NULL,
						currentPort,
						&lr,
						NULL,
						NULL,
						0);

		if (pOleContainerLine->m_fWindowOpen) {
	        /* OLE2NOTE: if the object servers window is Open (ie. not active
	        **    in-place) then we must shade the object in our document to
	        **    indicate to the user that the object is open elsewhere.
	        */
	        OleUIDrawShading(r, currentPort, OLEUI_SHADE_FULLRECT, 0);
		}

		OleUIShowObject(r, currentPort, pOleContainerLine->m_fIsLink);
	}
}

//#pragma segment OleContainerLineSeg
void OleContainerLineDrawSelected(LinePtr pLine, Rect* r)
{
	OleContainerLinePtr		pOleContainerLine;
	GrafPtr					currentPort;
	
	if (pLine->m_fNeedsUpdateView)
		return;
	
	pOleContainerLine = (OleContainerLinePtr)pLine;
	
#if qOleInPlace
	if (OleContainerSiteIsUIVisible(&pOleContainerLine->site))
		return;
#endif

	ASSERTCOND(pOleContainerLine->vtbl->m_DrawSelectedProcPtr != nil);
	(*pOleContainerLine->vtbl->m_DrawSelectedProcPtr)(pLine, r);
	
	GetPort(&currentPort);	
	OleUIDrawHandles(r, currentPort, OLEUI_HANDLES_INSIDE | OLEUI_HANDLES_USEINVERSE, kHandleSize);

	OleUIShowObject(r, currentPort, pOleContainerLine->m_fIsLink);
}

//#pragma segment OleContainerLineSeg
void OleContainerLineDoDoubleClick(LinePtr pLine)
{
	OleContainerLinePtr		pOleContainerLine;
	
	pOleContainerLine = (OleContainerLinePtr)pLine;
	
#if qOleInPlace
	// if we caught this double-click while in the background it's
	// best to skip it. we might be in the middle of shutting down
	// another inplace session and we should wait until that's done.
	if (gApplication->m_InBackground)
		return;
#endif

	OleContainerLineDoVerb(pOleContainerLine, OLEIVERB_PRIMARY, true, true);
}

//#pragma segment OleContainerLineSeg
HRESULT OleContainerLineQueryInterface(OleContainerLinePtr pOleContainerLine, REFIID riid, void* * lplpvObj)
{
	if (IsEqualIID(riid, &IID_IUnknown))
	{
		*lplpvObj = (void *) &pOleContainerLine->m_Unknown;
		OleContainerLineAddRef(pOleContainerLine);
		return NOERROR;
	}
	else if (IsEqualIID(riid, &IID_IOleClientSite))
	{
		*lplpvObj = (void *) &pOleContainerLine->m_OleClientSite;
		OleContainerLineAddRef(pOleContainerLine);
		return NOERROR;
	}
	else if (IsEqualIID(riid, &IID_IAdviseSink))
	{
		*lplpvObj = (void *) &pOleContainerLine->m_AdviseSink2;
		OleContainerLineAddRef(pOleContainerLine);
		return NOERROR;
	}
	else if (IsEqualIID(riid, &IID_IAdviseSink2))
	{
		*lplpvObj = (void *) &pOleContainerLine->m_AdviseSink2;
		OleContainerLineAddRef(pOleContainerLine);
		return NOERROR;
	}

	return OleContainerSiteLocalQueryInterface(&pOleContainerLine->site, riid, lplpvObj);
}

//#pragma segment OleContainerLineSeg
unsigned long OleContainerLineAddRef(OleContainerLinePtr pOleContainerLine)
{
	++pOleContainerLine->m_cRef;

	return pOleContainerLine->m_cRef;
}

//#pragma segment OleContainerLineSeg
unsigned long OleContainerLineRelease(OleContainerLinePtr pOleContainerLine)
{
	unsigned long		cRef;

	ASSERTCOND(pOleContainerLine->m_cRef > 0);
	
	cRef = --pOleContainerLine->m_cRef;
	
	if (cRef == 0)
	{
		LinePtr		pLine;
		
		pLine = (LinePtr)pOleContainerLine;
		
		ASSERTCOND(pLine->vtbl->m_DisposeProcPtr != nil);
		(*pLine->vtbl->m_DisposeProcPtr)(pLine);
	}
	
	return cRef;
}

//#pragma segment OleContainerLineSeg
void OleContainerLineLoadOleObject(OleContainerLinePtr pOleContainerLine)
{
	LPSTORAGE			pDocStorage;
	HRESULT				hrErr;
	LPOLECLIENTSITE		pOleClientSite;
	
	if (pOleContainerLine->site.m_pOleObj)
		return;
	
	pDocStorage = OleOutlineDocGetStorage((OleOutlineDocPtr)((LinePtr)pOleContainerLine)->m_LineList->m_pDoc);
	ASSERTCOND(pDocStorage != nil);
	
	// if object storage is not already open, then open it	
	if (!pOleContainerLine->m_pStorage)
	{
		hrErr = pDocStorage->lpVtbl->OpenStorage(
				pDocStorage,
				pOleContainerLine->m_sStorageName,
				NULL,
				STGM_READWRITE | STGM_TRANSACTED | STGM_SHARE_EXCLUSIVE,
				NULL,
				0,
				&pOleContainerLine->m_pStorage
			);
		ASSERTNOERROR(hrErr);
		FailOleErr(hrErr);
	}

    /* OLE2NOTE: if the OLE object being loaded is in a data transfer
    **    document, then we should NOT pass a IOleClientSite* pointer
    **    to the OleLoad call. This particularly critical if the OLE
    **    object is an OleLink object. If a non-NULL client site is
    **    passed to the OleLoad function, then the link will bind to
    **    the source if its is running. in the situation that we are
    **    loading the object as part of a data transfer document we do
    **    not want this connection to be established. even worse, if
    **    the link source is currently blocked or busy, then this could
    **    hang the system. it is simplest to never pass a
    **    IOleClientSite* when loading an object in a data transfer
    **    document.
    */
	pOleClientSite = ((LinePtr)pOleContainerLine)->m_LineList->m_pDoc->m_fDataTransferDoc
							? NULL : (LPOLECLIENTSITE)&pOleContainerLine->m_OleClientSite;
							
	hrErr = OleLoad(
				pOleContainerLine->m_pStorage,
				&IID_IOleObject,
				pOleClientSite,
				(void * *)&pOleContainerLine->site.m_pOleObj
			);
	ASSERTNOERROR(hrErr);
	FailOleErr(hrErr);

    // Cache a pointer to the IViewObject* interface.
    //      we need this everytime we draw the object.
	pOleContainerLine->site.m_pViewObj =
			(LPVIEWOBJECT)OleStdQueryInterface((LPUNKNOWN)pOleContainerLine->site.m_pOleObj, &IID_IViewObject);
	ASSERTCOND(pOleContainerLine->site.m_pViewObj != nil);
	if (pOleContainerLine->site.m_pViewObj == nil)
		FailOleErr(ResultFromScode(E_FAIL));

    // Cache a pointer to the IPersistStorage* interface.
    //      we need this everytime we save the object.
	pOleContainerLine->site.m_pPersistStorage =
			(LPPERSISTSTORAGE)OleStdQueryInterface((LPUNKNOWN)pOleContainerLine->site.m_pOleObj, &IID_IPersistStorage);
	ASSERTCOND(pOleContainerLine->site.m_pPersistStorage != nil);
	if (pOleContainerLine->site.m_pPersistStorage == nil)
		FailOleErr(ResultFromScode(E_FAIL));
		
    /* OLE2NOTE: similarly, if the OLE object being loaded is in a data
    **    transfer document, then we do NOT need to setup any advises,
    **    call SetHostNames, SetMoniker, etc.
    */
	if (pOleClientSite)
	{
		LineListPtr		pLineList = ((LinePtr)pOleContainerLine)->m_LineList;
		DocumentPtr		pDoc = LineListGetDoc(pLineList);
		FSSpec			fileSP;
		
		DocGetFSSpec(pDoc, &fileSP);

        /* Setup the Advises (OLE notifications) that we are interested
        ** in receiving.
        */
		OleStdSetupAdvises(
				pOleContainerLine->site.m_pOleObj,
				pOleContainerLine->m_DrawAspect,
				gApplicationName,
				(char*)p2cstr(fileSP.name),
				(LPADVISESINK)&pOleContainerLine->m_AdviseSink2
			);

        /* OLE2NOTE: if the OLE object has a moniker assigned, we need to
        **    inform the object by calling IOleObject::SetMoniker. this
        **    will force the OLE object to register in the
        **    RunningObjectTable when it enters the running state.
        */
        if (pOleContainerLine->m_fMonikerAssigned)
        {
        	LPMONIKER	pmkObj;
        	
        	pmkObj = OleContainerLineGetRelMoniker(pOleContainerLine, OLEGETMONIKER_ONLYIFTHERE);
        	
        	if (pmkObj)
        	{
        		pOleContainerLine->site.m_pOleObj->lpVtbl->SetMoniker(
        						pOleContainerLine->site.m_pOleObj,
        						OLEWHICHMK_OBJREL,
        						pmkObj
        				);
        		OleStdRelease((LPUNKNOWN)pmkObj);
        	}

		}
		
        /* get the Short form of the user type name of the object. this
        **    is used all the time when we have to build the object
        **    verb menu. we will cache this information to make it
        **    quicker to build the verb menu.
        */
		pOleContainerLine->site.m_pOleObj->lpVtbl->GetUserType(
					pOleContainerLine->site.m_pOleObj,
					USERCLASSTYPE_SHORT,
					&pOleContainerLine->m_pszShortType
			);

         /* OLE2NOTE: if the object we just loaded is a link then it
        **    might have immediately bound to its link source if it was
        **    already running. when this happens the link will get an
        **    immediate update of data if the link is automatic. in
        **    this situation, it is possible that the extents of the
        **    linked object have changed.
        */
		if (pOleContainerLine->m_fIsLink)
		{
			if (OleIsRunning(pOleContainerLine->site.m_pOleObj))
			{
				LinePtr		pLine;
				
				pLine = (LinePtr)pOleContainerLine;
				
				OleContainerLineUpdateExtent(pOleContainerLine);
			
				LineListUpdateTableLineExtent(pLine->m_LineList, pLine);
			}
		}
	}
}

/* ContainerLine_CloseOleObject
** ----------------------------
**    Close the OLE object associated with the ContainerLine.
**
**    Closing the object forces the object to transition from the
**    running state to the loaded state. if the object was not running,
**    then there is no effect. it is necessary to close the OLE object
**    before releasing the pointers to the OLE object.
**
**    Returns TRUE if successfully closed,
**            FALSE if closing was aborted.
*/
//#pragma segment OleContainerLineSeg
Boolean OleContainerLineCloseOleObject(OleContainerLinePtr pOleContainerLine)
{
	HRESULT		hrErr;

	if (!pOleContainerLine->site.m_pOleObj)
		return false;

	hrErr = pOleContainerLine->site.m_pOleObj->lpVtbl->Close(
								pOleContainerLine->site.m_pOleObj,
								OLECLOSE_SAVEIFDIRTY /* OLECLOSE_NOSAVE */);
	ASSERTNOERROR(hrErr);
		
#if qOleInPlace
	if (pOleContainerLine->site.inplace.m_fServerRunning)
	{		
		 /* OLE2NOTE: unlock the lock held on the in-place object.
		 **    it is VERY important that an in-place container
		 **    that also support linking to embeddings properly manage
		 **    the running of its in-place objects. in an outside-in
		 **    style in-place container, when the user clicks
		 **    outside of the in-place active object, the object gets
		 **    UIDeactivated and the object hides its window. in order
		 **    to make the object fast to reactivate, the container
		 **    deliberately does not call IOleObject::Close. the object
		 **    stays running in the invisible unlocked state. the idea
		 **    here is if the user simply clicks outside of the object
		 **    and then wants to double click again to re-activate the
		 **    object, we do not want this to be slow. if we want to
		 **    keep the object running, however, we MUST Lock it
		 **    running. otherwise the object will be in an unstable
		 **    state where if a linking client does a "silent-update"
		 **    (eg. UpdateNow from the Links dialog), then the in-place
		 **    server will shut down even before the object has a chance
		 **    to be saved back in its container. this saving normally
		 **    occurs when the in-place container closes the object. also
		 **    keeping the object in the unstable, hidden, running,
		 **    not-locked state can cause problems in some scenarios.
		 **    ICntrOtl keeps only one object running. if the user
		 **    initiates a DoVerb on another object, then that last
		 **    running in-place active object is closed. a more
		 **    sophistocated in-place container may keep more objects running.
		 **    (see OleInPlaceContainerSiteOnInPlaceActivate)
		 */

		ASSERTCOND(pOleContainerLine->site.m_pOleObj != nil);
		hrErr = OleLockRunning((LPUNKNOWN)pOleContainerLine->site.m_pOleObj, false, true);
		ASSERTNOERROR(hrErr);
	}
#endif

	if (hrErr != NOERROR)
	{
		SCODE		scode;
		
		scode = GetScode(hrErr);
		if (scode == RPC_E_CALL_REJECTED || scode == OLE_E_PROMPTSAVECANCELLED)
			return false;
	}
	
	return true;
}

/* ContainerLine_UnloadOleObject
** -----------------------------
**    Close the OLE object associated with the ContainerLine and
**    release all pointer held to the object.
**
**    Closing the object forces the object to transition from the
**    running state to the loaded state. if the object was not running,
**    then there is no effect. it is necessary to close the OLE object
**    before releasing the pointers to the OLE object. releasing all
**    pointers to the object allows the object to transition from
**    loaded to unloaded (or passive).
*/
//#pragma segment OleContainerLineSeg
void OleContainerLineUnloadOleObject(OleContainerLinePtr pOleContainerLine, unsigned long dwSaveOption)
{
	HRESULT		hrErr;
	
	if (pOleContainerLine->site.m_pOleObj)
	{
		hrErr = pOleContainerLine->site.m_pOleObj->lpVtbl->Close(
									pOleContainerLine->site.m_pOleObj,
									dwSaveOption);
		ASSERTNOERROR(hrErr);
		
        /* OLE2NOTE: we will take our IOleClientSite* pointer away from
        **    the object before we release all pointers to the object.
        **    in the scenario where the object is implemented as an
        **    in-proc server (DLL object), then, if there are link
        **    connections to the DLL object, it is possible that the
        **    object will not be destroyed when we release our pointers
        **    to the object. the existance of the remote link
        **    connections will hold the object alive. later when these
        **    strong connections are released, then the object may
        **    attempt to call IOleClientSite::Save if we had not taken
        **    away the client site pointer.
        */
        OLEDBG_BEGIN2("IOleObject::SetClientSite(NULL) called\r\n")
        pOleContainerLine->site.m_pOleObj->lpVtbl->SetClientSite(
                pOleContainerLine->site.m_pOleObj, NULL);
        OLEDBG_END2
		
		OleStdRelease((LPUNKNOWN)pOleContainerLine->site.m_pOleObj);
		pOleContainerLine->site.m_pOleObj = nil;
		
		if (pOleContainerLine->site.m_pViewObj)
		{
			OleStdRelease((LPUNKNOWN)pOleContainerLine->site.m_pViewObj);
			pOleContainerLine->site.m_pViewObj = nil;
		}
		
		if (pOleContainerLine->site.m_pPersistStorage)
		{
			OleStdRelease((LPUNKNOWN)pOleContainerLine->site.m_pPersistStorage);
			pOleContainerLine->site.m_pPersistStorage = nil;
		}
	}

    if (pOleContainerLine->m_pszShortType) {
        OleStdFreeString(pOleContainerLine->m_pszShortType, NULL);
        pOleContainerLine->m_pszShortType = nil;
    }
}

//#pragma segment OleContainerLineSeg
void OleContainerLineDoVerb(OleContainerLinePtr pOleContainerLine, long verb, Boolean fMessage, Boolean fAction)
{
	Rect	posRect;
	HRESULT	hrErr;
	
	// if not loaded, load it
	if (!pOleContainerLine->site.m_pOleObj)
		OleContainerLineLoadOleObject(pOleContainerLine);
		
	LineGetBounds((LinePtr)pOleContainerLine, &posRect);
	
	OleContainerLineRunOleObject(pOleContainerLine);
	
	hrErr = pOleContainerLine->site.m_pOleObj->lpVtbl->DoVerb(
				pOleContainerLine->site.m_pOleObj,
				verb,
				NULL,
				(LPOLECLIENTSITE)&pOleContainerLine->m_OleClientSite,
				-1,
				LineGetWindow((LinePtr)pOleContainerLine),
				&posRect);
				
	ASSERTNOERROR(hrErr);
	FailOleErr(hrErr);
}

/* OleContainerLineRunOleObject
** --------------------------
**    Load and run the object. Upon running and if size of object has changed,
**	  use SetExtent to change to new size.
*/
//#pragma segment OleContainerLineSeg
void OleContainerLineRunOleObject(OleContainerLinePtr pOleContainerLine)
{
	LPUNKNOWN	pUnknown;
	HRESULT		hrErr;
	SIZEL		sizel;
	LinePtr		pLine = (LinePtr)pOleContainerLine;
	
	if (pOleContainerLine->site.m_pOleObj && OleIsRunning(pOleContainerLine->site.m_pOleObj))
		return;
		
	// this may take a moment, so put up a stopwatch
	SetCursor(*gWatchCursor);
	
	pUnknown = OleContainerLineGetOleObject(pOleContainerLine, &IID_IUnknown);
	ASSERTCOND(pUnknown != nil);
	
	hrErr = OleRun(pUnknown);
	
	if (GetScode(hrErr) == MK_E_MUSTBOTHERUSER) {
		LPOLELINK	pOleLink = nil;
		
		hrErr = pOleContainerLine->site.m_pOleObj->lpVtbl->QueryInterface(
					pOleContainerLine->site.m_pOleObj,
					&IID_IOleLink,
					(void**)&pOleLink);
					
		if (pOleLink) {
			IBindCtx	*pbc = nil;
			BIND_OPTS	bindopts;
			
            CreateBindCtx(0, (LPBC *)&pbc);

            bindopts.cbStruct = sizeof(BIND_OPTS);
            pbc->lpVtbl->GetBindOptions(pbc, &bindopts);
            bindopts.grfFlags |= BIND_MAYBOTHERUSER;
            pbc->lpVtbl->SetBindOptions(pbc, &bindopts);

            hrErr = pOleLink->lpVtbl->BindToSource(pOleLink, OLELINKBIND_EVENIFCLASSDIFF, pbc);

			pOleLink->lpVtbl->Release(pOleLink);
			pbc->lpVtbl->Release(pbc);

		}
	}						
	
	ASSERTNOERROR(hrErr);
	
	OleStdRelease(pUnknown);
	
	// clear watch cursor
	SetCursor(&qd.arrow);

	FailOleErr(hrErr);
	
    if (pOleContainerLine->m_fDoSetExtent) {
        /* OLE2NOTE: the OLE object was resized when it was not running
        **    and the object did not have the OLEMISC_RECOMPOSEONRESIZE
        **    bit set. if it had, the object would have been run
        **    immediately when it was resized. this flag indicates that
        **    the object does something other than simple scaling when
        **    it is resized. because the object is being run now, we
        **    will call IOleObject::SetExtent.
        */
        pOleContainerLine->m_fDoSetExtent = false;
		sizel.cx = pLine->m_WidthInPoints;
		sizel.cy = pLine->m_HeightInPoints;

		pOleContainerLine->site.m_pOleObj->lpVtbl->SetExtent(
				pOleContainerLine->site.m_pOleObj,
				pOleContainerLine->m_DrawAspect,
				&sizel);
    }
}

//#pragma segment OleContainerLineSeg
HRESULT OleContainerLineSaveOleObject(OleContainerLinePtr pOleContainerLine, LPSTORAGE pStorage, Boolean fSameAsLoad, Boolean fRemember, Boolean fForceUpdate)
{
	HRESULT		hrErr;
	
	hrErr = NOERROR;

	TRY
	{
		if (!pOleContainerLine->site.m_pPersistStorage)
		{
			pOleContainerLine->site.m_pPersistStorage
					= (LPPERSISTSTORAGE)OleStdQueryInterface((LPUNKNOWN)pOleContainerLine->site.m_pOleObj, &IID_IPersistStorage);
			ASSERTCOND(pOleContainerLine->site.m_pPersistStorage != nil);
			if (pOleContainerLine->site.m_pPersistStorage == nil)
				FailOleErr(ResultFromScode(E_FAIL));
		}
	
		/* OLE2NOTE: if the object is an embedded object and it is
		**    currently running, then the user can be changing
		**    the object and we can not be certain of the frequency of data
		**    updates. thus we need to force an update to guarantee that we
		**    have the latest data. this is particularly important for OLE
		**    1.0 embeddings.
		*/
		if (fForceUpdate && !pOleContainerLine->m_fIsLink
				&& OleIsRunning(pOleContainerLine->site.m_pOleObj))
		{
			hrErr = pOleContainerLine->site.m_pOleObj->lpVtbl->Update(pOleContainerLine->site.m_pOleObj);
	
			if (GetScode(hrErr) == MK_E_MUSTBOTHERUSER) {
				LPOLELINK	pOleLink = nil;
				
				hrErr = pOleContainerLine->site.m_pOleObj->lpVtbl->QueryInterface(
							pOleContainerLine->site.m_pOleObj,
							&IID_IOleLink,
							(void**)&pOleLink);
							
				if (pOleLink) {
					IBindCtx	*pbc = nil;
					BIND_OPTS	bindopts;
					
					CreateBindCtx(0, (LPBC *)&pbc);
					
					bindopts.cbStruct = sizeof(BIND_OPTS);
					pbc->lpVtbl->GetBindOptions(pbc, &bindopts);
					bindopts.grfFlags |= BIND_MAYBOTHERUSER;
					pbc->lpVtbl->SetBindOptions(pbc, &bindopts);
					
					hrErr = pOleLink->lpVtbl->Update(pOleLink, pbc);
					
					pOleLink->lpVtbl->Release(pOleLink);
					pbc->lpVtbl->Release(pbc);
				}
			}						
	
			ASSERTNOERROR(hrErr);
		}
		
		hrErr = OleSave(pOleContainerLine->site.m_pPersistStorage, pStorage, fSameAsLoad);
		ASSERTNOERROR(hrErr);
		FailOleErr(hrErr);
	
		/* OLE2NOTE: a root level container should immediately
		**    call IPersistStorage::SaveCompleted after calling OleSave. a
		**    nested level container should not call SaveCompleted now, but
		**    must wait until SaveCompleted is call on it by its container.
		**    since our container is not a container/server, then we always
		**    call SaveComplete here.
		**
		**    if this is a SaveAs operation, then we need to pass the lpStg
		**    back in SaveCompleted to inform the object of its new storage
		**    that it may hold on to.
		**    if this is a Save or a SaveCopyAs operation, then we simply
		**    pass NULL in SaveCompleted; the object can continue to hold
		**    its current storage.
		*/
		hrErr = pOleContainerLine->site.m_pPersistStorage->lpVtbl->SaveCompleted(
						pOleContainerLine->site.m_pPersistStorage,
						(fRemember && !fSameAsLoad) ? pStorage : NULL);
		ASSERTNOERROR(hrErr);
		FailOleErr(hrErr);
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

/* ContainerLineGetOleObject
** --------------------------
**    return pointer to desired interface of embedded/linked object.
**
**    NOTE: this function causes an AddRef to the object. when the caller is
**          finished with the object, it must call Release.
**          this function does not AddRef the ContainerLine object.
*/
//#pragma segment OleContainerLineSeg
LPUNKNOWN OleContainerLineGetOleObject(OleContainerLinePtr pOleContainerLine, REFIID riid)
{
	if (!pOleContainerLine->site.m_pOleObj)
		OleContainerLineLoadOleObject(pOleContainerLine);
		
	if (pOleContainerLine->site.m_pOleObj)
		return OleStdQueryInterface((LPUNKNOWN)pOleContainerLine->site.m_pOleObj, riid);
		
	return NULL;
}

/* ContainerLineGetRelMoniker
** --------------------------
**    Retrieve the relative item moniker which identifies the OLE object
**    relative to the container document.
**
**    Returns NULL if a moniker can NOT be created.
*/
//#pragma segment OleContainerLineSeg
LPMONIKER OleContainerLineGetRelMoniker(OleContainerLinePtr pOleContainerLine, OLEGETMONIKER dwAssign)
{
	LPMONIKER lpmk = NULL;

	/* OLE2NOTE: we should only give out a moniker for the OLE object
	**    if the object is allowed to be linked to from the inside. if
	**    so we are allowed to give out a moniker which binds to the
	**    running OLE object). if the object is an OLE 2.0 embedded
	**    object then it is allowed to be linked to from the inside. if
	**    the object is either an OleLink or an OLE 1.0 embedding
	**    then it can not be linked to from the inside.
	**    if we were a container/server app then we could offer linking
	**    to the outside of the object (ie. a pseudo object within our
	**    document). we are a container only app that does not support
	**    linking to ranges of its data.
	*/

	switch (dwAssign)
	{
		case OLEGETMONIKER_FORCEASSIGN:
                /* Force the assignment of the name. This is called when a
                **    Paste Link actually occurs. From now on we want
                **    to inform the OLE object that its moniker is
                **    assigned and is thus necessary to register itself
                **    in the RunningObjectTable.
                */
				CreateItemMoniker(OLESTDDELIM, pOleContainerLine->m_sStorageName, &lpmk);
				
				pOleContainerLine->m_fMonikerAssigned = TRUE;
				
                /* OLE2NOTE: if the OLE object is already loaded we
                **    need to inform it that it now has a moniker
                **    assigned by calling IOleObject::SetMoniker. this
                **    will force the OLE object to register in the
                **    RunningObjectTable when it enters the running
                **    state. if the object is not currently loaded,
                **    SetMoniker will be called automatically later when
                **    the object is loaded by the function
                **    ContainerLine_LoadOleObject.
                */
				if (pOleContainerLine->site.m_pOleObj)
				{
					pOleContainerLine->site.m_pOleObj->lpVtbl->SetMoniker(
								pOleContainerLine->site.m_pOleObj,
								OLEWHICHMK_OBJREL,
								lpmk
						);
				}
				
				break;
			
		case OLEGETMONIKER_ONLYIFTHERE:
                /* If the OLE object currently has a moniker assigned,
                **    then return it.
                */
				if (pOleContainerLine->m_fMonikerAssigned)
					CreateItemMoniker(OLESTDDELIM, pOleContainerLine->m_sStorageName, &lpmk);
				
				break;
			
		case OLEGETMONIKER_TEMPFORUSER:
                /* Return the moniker that would be used for the OLE
                **    object but do NOT force moniker assignment at
                **    this point. Since our strategy is to use the
                **    storage name of the object as its item name, we
                **    can simply create the corresponding ItemMoniker
                **    (indepenedent of whether the moniker is currently
                **    assigned or not).
                */
				CreateItemMoniker(OLESTDDELIM, pOleContainerLine->m_sStorageName, &lpmk);
				
				break;
			
		case OLEGETMONIKER_UNASSIGN:
				pOleContainerLine->m_fMonikerAssigned = FALSE;
				
				break;
	}
	
	return lpmk;
}

//#pragma segment OleContainerLineSeg
LPSTORAGE OleContainerLineGetStorage(OleContainerLinePtr pOleContainerLine)
{
	return pOleContainerLine->m_pStorage;
}

//#pragma segment OleContainerLineSeg
const char* OleContainerLineGetStorageName(OleContainerLinePtr pOleContainerLine)
{
	return pOleContainerLine->m_sStorageName;
}

//#pragma segment OleContainerLineSeg
HRESULT OleContainerLineGetObject(OleContainerLinePtr pOleContainerLine, unsigned long SpeedNeeded, REFIID riid, void* * ppvObject)
{
	HRESULT hrErr;
	unsigned long Status;

	// check if object is loaded.
	if (! pOleContainerLine->site.m_pOleObj) {

		// if BINDSPEED_IMMEDIATE is requested, object must
		// ALREADY be loadded.
		if (SpeedNeeded == BINDSPEED_IMMEDIATE)
			return ResultFromScode(MK_E_EXCEEDEDDEADLINE);

		OleContainerLineLoadOleObject(pOleContainerLine);
		if (! pOleContainerLine->site.m_pOleObj)
			return ResultFromScode(E_OUTOFMEMORY);
	}

	/* OLE2NOTE: check if the object is allowed to be linked
	**    to from the inside (ie. we are allowed to
	**    give out a moniker which binds to the running
	**    OLE object). if the object is an OLE
	**    2.0 embedded object then it is allowed to be
	**    linked to from the inside. if the object is
	**    either an OleLink or an OLE 1.0 embedding
	**    then it can not be linked to from the inside.
	**    if we were a container/server app then we
	**    could offer linking to the outside of the
	**    object (ie. a pseudo object within our
	**    document). we are a container only app that
	**    does not support linking to ranges of its data.
	*/
	pOleContainerLine->site.m_pOleObj->lpVtbl->GetMiscStatus(
			pOleContainerLine->site.m_pOleObj,
			DVASPECT_CONTENT, /* aspect is not important */
			&Status
	);
	if (Status & OLEMISC_CANTLINKINSIDE)
		return ResultFromScode(MK_E_NOOBJECT);

	// check if object is running.
	if (! OleIsRunning(pOleContainerLine->site.m_pOleObj)) {

		// if BINDSPEED_MODERATE is requested, object must
		// ALREADY be running.
		if (SpeedNeeded == BINDSPEED_MODERATE)
			return ResultFromScode(MK_E_EXCEEDEDDEADLINE);

			/* OLE2NOTE: we have found a match for the item name.
			**    now we must return a pointer to the desired
			**    interface on the RUNNING object. we must
			**    carefully load the object and initially ask for
			**    an interface that we are sure the loaded form of
			**    the object supports. if we immediately ask the
			**    loaded object for the desired interface, the
			**    QueryInterface call might fail if it is an
			**    interface that is supported only when the object
			**    is running. thus we force the object to load and
			**    return its IUnknown*. then we force the object to
			**    run, and then finally, we can ask for the
			**    actually requested interface.
			*/			
			TRY
			{
				OleContainerLineRunOleObject(pOleContainerLine);
			}
			CATCH
			{
				/* OLE2NOTE: this demonstrates an example use of
				**    PropagateResult. this allows us to return an
				**    SCODE of our choosing, but still passing
				**    along the error context of the previous
				**    error. in the future this will be used to
				**    stack up error contexts.
				*/
				return ResultFromScode(E_FAIL);
			}
			ENDTRY
	}

	// Retrieve the requested interface
	*ppvObject = OleStdQueryInterface((LPUNKNOWN)pOleContainerLine->site.m_pOleObj, riid);
	if (*ppvObject)
		return NOERROR;
	else
		return ResultFromScode(E_NOINTERFACE);
}

//#pragma segment OleContainerLineSeg
HRESULT OleContainerLineIsRunning(OleContainerLinePtr pOleContainerLine)
{
	unsigned long Status;
	
	/* OLE2NOTE: we must check if the OLE object is running.
	**    we will load the object if not already loaded.
	*/
	if (! pOleContainerLine->site.m_pOleObj) {
		OleContainerLineLoadOleObject(pOleContainerLine);
		if (! pOleContainerLine->site.m_pOleObj)
 			return ResultFromScode(E_OUTOFMEMORY);
	}
			
	/* OLE2NOTE: check if the object is allowed to be linked
	**    to from the inside (ie. we are allowed to
	**    give out a moniker which binds to the running
	**    OLE object). if the object is an OLE
	**    2.0 embedded object then it is allowed to be
	**    linked to from the inside. if the object is
	**    either an OleLink or an OLE 1.0 embedding
	**    then it can not be linked to from the inside.
	**    if we were a container/server app then we
	**    could offer linking to the outside of the
	**    object (ie. a pseudo object within our
	**    document). we are a container only app that
	**    does not support linking to ranges of its data.
	*/
	pOleContainerLine->site.m_pOleObj->lpVtbl->GetMiscStatus(
			pOleContainerLine->site.m_pOleObj,
			DVASPECT_CONTENT, /* aspect is not important */
			&Status
	);
	if (Status & OLEMISC_CANTLINKINSIDE)
		return ResultFromScode(MK_E_NOOBJECT);

	if (OleIsRunning(pOleContainerLine->site.m_pOleObj))
		return NOERROR;
	else
		return ResultFromScode(S_FALSE);
}

//#pragma segment OleContainerLineSeg
void OleContainerLineInformOleObjectDocRenamed(OleContainerLinePtr pOleContainerLine, LPMONIKER pmkDoc)
{
	/* OLE2NOTE: if the OLE object is already loaded AND the
	**    object already has a moniker assigned, then we need
	**    to inform it that the moniker of the ContainerDoc has
	**    changed. of course, this means the full moniker of
	**    the object has changed. to do this we call
	**    IOleObject::SetMoniker. this will force the OLE
	**    object to re-register in the RunningObjectTable if it
	**    is currently in the running state. it is not in the
	**    running state, the object handler can make not that
	**    the object has a new moniker. if the object is not
	**    currently loaded, SetMoniker will be called
	**    automatically later when the object is loaded by the
	**    function ContainerLine_LoadOleObject.
	**    also if the object is a linked object, we always want
	**    to call SetMoniker on the link so that in case the
	**    link source is contained within our same container,
	**    the link source will be tracked. the link rebuilds
	**    its absolute moniker if it has a relative moniker.
	*/
	if (pOleContainerLine->site.m_pOleObj) {
	
		LineListPtr		pLineList = ((LinePtr)pOleContainerLine)->m_LineList;
		DocumentPtr		pDoc = LineListGetDoc(pLineList);
		FSSpec			fileSP;
		
		DocGetFSSpec(pDoc, &fileSP);
	
		if (pOleContainerLine->m_fMonikerAssigned ||
				pOleContainerLine->m_fIsLink) {
			pOleContainerLine->site.m_pOleObj->lpVtbl->SetMoniker(
                            pOleContainerLine->site.m_pOleObj,
                            OLEWHICHMK_CONTAINER,
                            pmkDoc
			);
		}

		/* OLE2NOTE: we must call IOleObject::SetHostNames so
		**    any open objects can update their window titles.
		*/
		pOleContainerLine->site.m_pOleObj->lpVtbl->SetHostNames(
                        pOleContainerLine->site.m_pOleObj,
                        gApplicationName,
                        (char*)p2cstr(fileSP.name));
	}	
}

//#pragma segment OleContainerLineSeg
void OleContainerLineSaveToStorage(LinePtr pLine, LPSTORAGE lpSrcStg, LPSTORAGE lpDestStg, LPSTREAM lpDestStm, Boolean fRemember)
{
	OleContainerLinePtr		pOleContainerLine;
	HRESULT					hrErr;
	LARGE_INTEGER			dlibSavePos;
	unsigned long			nWritten;
	
	pOleContainerLine = (OleContainerLinePtr)pLine;
	
	{
		LARGE_INTEGER		dlibZeroOffset;
	
		LISet32(dlibZeroOffset, 0);

		// save seek position before line record is written in case of error
		hrErr = lpDestStm->lpVtbl->Seek(
					lpDestStm,
					dlibZeroOffset,
					STREAM_SEEK_CUR,
					(ULARGE_INTEGER*)&dlibSavePos
				);
		ASSERTNOERROR(hrErr);
		FailOleErr(hrErr);
	}
	
	TRY
	{
		ASSERTCOND(pOleContainerLine->vtbl->m_SaveToStorageProcPtr != nil);
		(*pOleContainerLine->vtbl->m_SaveToStorageProcPtr)(pLine, lpSrcStg, lpDestStg, lpDestStm, fRemember);

		{
			ContainerLineRec	objLineRecord;
			
			strcpy(objLineRecord.m_szStgName, pOleContainerLine->m_sStorageName);
			objLineRecord.m_fMonikerAssigned = 	pOleContainerLine->m_fMonikerAssigned;
			objLineRecord.m_dwDrawAspect = 		pOleContainerLine->m_DrawAspect;
			LineGetExtent(pLine, &objLineRecord.m_sizeInHimetric);
			objLineRecord.m_dwLinkType = 			pOleContainerLine->m_fIsLink;
			objLineRecord.m_fDoSetExtent =			pOleContainerLine->m_fDoSetExtent;
			
			objLineRecord.m_fMonikerAssigned = SwapWord(objLineRecord.m_fMonikerAssigned);
			objLineRecord.m_dwDrawAspect = SwapLong(objLineRecord.m_dwDrawAspect);
			objLineRecord.m_sizeInHimetric.cx = SwapLong(objLineRecord.m_sizeInHimetric.cx);
			objLineRecord.m_sizeInHimetric.cy = SwapLong(objLineRecord.m_sizeInHimetric.cy);
			objLineRecord.m_dwLinkType = SwapLong(objLineRecord.m_dwLinkType);
			objLineRecord.m_fDoSetExtent = SwapWord(objLineRecord.m_fDoSetExtent);
			
			// write the line
			hrErr = lpDestStm->lpVtbl->Write(
						lpDestStm,
						&objLineRecord,
						sizeof(ContainerLineRec),
						&nWritten
					);
			ASSERTNOERROR(hrErr);
			FailOleErr(hrErr);

			objLineRecord.m_fMonikerAssigned = SwapWord(objLineRecord.m_fMonikerAssigned);
			objLineRecord.m_dwDrawAspect = SwapLong(objLineRecord.m_dwDrawAspect);
			objLineRecord.m_sizeInHimetric.cx = SwapLong(objLineRecord.m_sizeInHimetric.cx);
			objLineRecord.m_sizeInHimetric.cy = SwapLong(objLineRecord.m_sizeInHimetric.cy);
			objLineRecord.m_dwLinkType = SwapLong(objLineRecord.m_dwLinkType);
			objLineRecord.m_fDoSetExtent = SwapWord(objLineRecord.m_fDoSetExtent);

		}
		
		if (!pOleContainerLine->site.m_pOleObj)
		{
			/*****************************************************************
			 ** CASE 1: object is NOT loaded.
			 *****************************************************************/
			
			if (lpSrcStg == lpDestStg)
			{
				/*************************************************************
				 ** CASE 1A: we are saving to the current storage. because
				 **    the object is not loaded, it is up-to-date
				 **    (ie. nothing to do).
				 *************************************************************/
			}
			else
			{
				LPSTORAGE	pObjDestStg;
				
				/*************************************************************
				 ** CASE 1B: we are saving to a new storage. because
				 **    the object is not loaded, we can simply copy the
				 **    object's current storage to the new storage.
				 *************************************************************/
				
				// if the current object storage is not already open, then open it
				if (!pOleContainerLine->m_pStorage)
				{
					hrErr = lpSrcStg->lpVtbl->OpenStorage(
									lpSrcStg,
									pOleContainerLine->m_sStorageName,
									NULL,
									STGM_READWRITE | STGM_TRANSACTED | STGM_SHARE_EXCLUSIVE,
									NULL,
									0,
									&pOleContainerLine->m_pStorage
								);
					ASSERTCOND(hrErr != nil);
					FailOleErr(hrErr);
				}
				
				// create a child storage inside the destination storage
				hrErr = lpDestStg->lpVtbl->CreateStorage(
								lpDestStg,
								pOleContainerLine->m_sStorageName,
								STGM_READWRITE | STGM_TRANSACTED | STGM_SHARE_EXCLUSIVE,
								0,
								0,
								&pObjDestStg
							);
				ASSERTNOERROR(hrErr);
				FailOleErr(hrErr);
				
				hrErr = pOleContainerLine->m_pStorage->lpVtbl->CopyTo(
								pOleContainerLine->m_pStorage,
								0,
								NULL,
								NULL,
								pObjDestStg
							);
				ASSERTNOERROR(hrErr);
				FailOleErr(hrErr);
				
				hrErr = pObjDestStg->lpVtbl->Commit(pObjDestStg, 0);
				ASSERTNOERROR(hrErr);
				FailOleErr(hrErr);
				
				 /* if we are supposed to remember this storage as the new
				 **    storage for the object, then release the old one and
				 **    save the new one. else, throw away the new one.
				 */
				
				 if (fRemember)
				 {
					OleStdVerifyRelease((LPUNKNOWN)pOleContainerLine->m_pStorage);
					pOleContainerLine->m_pStorage = pObjDestStg;
				 }
				 else
				 	OleStdVerifyRelease((LPUNKNOWN)pObjDestStg);
			}
		}
		else
		{
			/*****************************************************************
			 ** CASE 2: object IS loaded.
			 *****************************************************************/
		
			if (lpSrcStg == lpDestStg)
			{
				LPPERSISTSTORAGE	pPersistStg;
				
				/*************************************************************
				 ** CASE 2A: we are saving to the current storage. if the object
				 **    is not dirty, then the current storage is up-to-date
				 **    (ie. nothing to do).
				 *************************************************************/
			
				if (!pOleContainerLine->site.m_pPersistStorage)
				{
					pOleContainerLine->site.m_pPersistStorage = (LPPERSISTSTORAGE)OleStdQueryInterface(
												(LPUNKNOWN)pOleContainerLine->site.m_pOleObj,
												&IID_IPersistStorage
											);
				}
				
				pPersistStg = pOleContainerLine->site.m_pPersistStorage;
				ASSERTCOND(pPersistStg != NULL);
				
				hrErr = pPersistStg->lpVtbl->IsDirty(pPersistStg);
				if (hrErr == NOERROR)
				{
					// ole object IS dirty
					hrErr = OleContainerLineSaveOleObject(
									pOleContainerLine,
									pOleContainerLine->m_pStorage,
									(Boolean)(lpSrcStg == lpDestStg),
									fRemember,
									TRUE);
					ASSERTNOERROR(hrErr);
					FailOleErr(hrErr);
				}
			}
			else
			{
				LPSTORAGE	pObjDestStg;

				/*************************************************************
				 ** CASE 2B: we are saving to a new storage. we must
				 **    tell the object to save into the new storage.
				 *************************************************************/
				
				// create a child storage inside the destination storage
				hrErr = lpDestStg->lpVtbl->CreateStorage(
								lpDestStg,
								pOleContainerLine->m_sStorageName,
								STGM_READWRITE | STGM_TRANSACTED | STGM_SHARE_EXCLUSIVE,
								0,
								0,
								&pObjDestStg
							);
				ASSERTNOERROR(hrErr);
				FailOleErr(hrErr);
				
				hrErr = OleContainerLineSaveOleObject(
								pOleContainerLine,
								pObjDestStg,
								(Boolean)(lpSrcStg == lpDestStg),
								fRemember,
								TRUE);
				ASSERTNOERROR(hrErr);
				FailOleErr(hrErr);

				 /* if we are supposed to remember this storage as the new
				 **    storage for the object, then release the old one and
				 **    save the new one. else, throw away the new one.
				 */
				
				 if (fRemember)
				 {
					OleStdVerifyRelease((LPUNKNOWN)pOleContainerLine->m_pStorage);
					pOleContainerLine->m_pStorage = pObjDestStg;
				 }
				 else
				 	OleStdVerifyRelease((LPUNKNOWN)pObjDestStg);
			}
		}
	}
	CATCH
	{
		hrErr = lpDestStm->lpVtbl->Seek(
					lpDestStm,
					dlibSavePos,
					STREAM_SEEK_SET,
					NULL
				);
		ASSERTNOERROR(hrErr);
	}
	ENDTRY				
}

//#pragma segment OleContainerLineSeg
void OleContainerLineLoadFromStorage(OleContainerLinePtr pOleContainerLine, LPSTORAGE lpSrcStg, LPSTREAM lpSrcStm)
{
	HRESULT				hrErr;
	unsigned long		nRead;
	ContainerLineRec	objLineRecord;
	
	/* OLE2NOTE: In order to have a stable ContainerLine object we must
	**    AddRef the object's refcnt. this will be released at the end of function
	*/
	OleContainerLineAddRef(pOleContainerLine);
	
	// read the line
	hrErr = lpSrcStm->lpVtbl->Read(
				lpSrcStm,
				&objLineRecord,
				sizeof(ContainerLineRec),
				&nRead
			);
	ASSERTNOERROR(hrErr);
	FailOleErr(hrErr);

	objLineRecord.m_fMonikerAssigned = SwapWord(objLineRecord.m_fMonikerAssigned);
	objLineRecord.m_dwDrawAspect = SwapLong(objLineRecord.m_dwDrawAspect);
	objLineRecord.m_sizeInHimetric.cx = SwapLong(objLineRecord.m_sizeInHimetric.cx);
	objLineRecord.m_sizeInHimetric.cy = SwapLong(objLineRecord.m_sizeInHimetric.cy);
	objLineRecord.m_dwLinkType = SwapLong(objLineRecord.m_dwLinkType);
	objLineRecord.m_fDoSetExtent = SwapWord(objLineRecord.m_fDoSetExtent);
	
	strcpy(pOleContainerLine->m_sStorageName, objLineRecord.m_szStgName);
	pOleContainerLine->m_fMonikerAssigned =		objLineRecord.m_fMonikerAssigned;
	pOleContainerLine->m_DrawAspect =			objLineRecord.m_dwDrawAspect;
	pOleContainerLine->m_fIsLink =				objLineRecord.m_dwLinkType;
	pOleContainerLine->m_fDoSetExtent =			objLineRecord.m_fDoSetExtent;
	
	OleContainerLineRelease(pOleContainerLine);		// release the addref above
}

//#pragma segment OleContainerLineSeg
void OleContainerLineCopyToDoc(LinePtr pLine, DocumentPtr pDoc)
{
	OleContainerLinePtr				pSrcLine;
	volatile OleContainerLinePtr	pDestLine;
	OleOutlineDocPtr				pSrcDoc;
	OleOutlineDocPtr				pDestDoc;
	LineListPtr						pSrcLineList;
	LineListPtr						pDestLineList;
	HRESULT							hrErr;
    LPSTORAGE   					pDestDocStg = NULL;
    LPSTORAGE   					pDestObjStg = NULL;
	
	
	pDestDoc = (OleOutlineDocPtr)pDoc;
	pDestLineList = ((OutlineDocPtr)pDestDoc)->m_LineList;
	pSrcLine = (OleContainerLinePtr)pLine;
	pDestDocStg = OleOutlineDocGetStorage(pDestDoc);
	
	pDestLine = (OleContainerLinePtr)NewPtrClear(sizeof(OleContainerLineRec));
	ASSERTCOND(pDestLine != nil);
	FailNIL(pDestLine);
	
	OleContainerLineInit(pDestLine, pDestLineList);

    /* OLE2NOTE: In order to have a stable ContainerLine object we must
	**    AddRef the object's refcnt. this will be later released when
	**    the ContainerLine is deleted.
    */
	OleContainerLineAddRef(pDestLine);

    // Copy data of the source ContainerLine
    pDestLine->m_fMonikerAssigned = pSrcLine->m_fMonikerAssigned;
    pDestLine->m_DrawAspect = pSrcLine->m_DrawAspect;
    pDestLine->m_fIsLink = pSrcLine->m_fIsLink;

    ((LinePtr)pDestLine)->m_fNeedsUpdateView = true;			// force view update
	((LinePtr)pDestLine)->m_TabLevel = pLine->m_TabLevel;
	((LinePtr)pDestLine)->m_TabWidthInPoints = pLine->m_TabWidthInPoints;
	((LinePtr)pDestLine)->m_HeightInPoints = pLine->m_HeightInPoints;
	((LinePtr)pDestLine)->m_WidthInPoints = pLine->m_WidthInPoints;


	TRY
	{	
	    /* We must create a new sub-storage for the embedded object within
	    **    the destination document's storage. We will first attempt to
	    **    use the same storage name as the source line. if this name is
	    **    in use, then we will allocate a new name. in this way we try
	    **    to keep the name associated with the OLE object unchanged
	    **    through a Cut/Paste operation.
	    */
		hrErr = pDestDocStg->lpVtbl->CreateStorage(
					pDestDocStg,
					pSrcLine->m_sStorageName,
					STGM_READWRITE | STGM_TRANSACTED | STGM_SHARE_EXCLUSIVE,
					0,
					0,
					&pDestObjStg
				);
	 	
	    if (pDestObjStg) {
	        strcpy(pDestLine->m_sStorageName, pSrcLine->m_sStorageName);
	    } else {
	        /* the original name was in use, make up a new name. */
	        OleOutlineContainerDocNewStorageName(pDestDoc, pDestLine->m_sStorageName);

        	hrErr = pDestDocStg->lpVtbl->CreateStorage(
						pDestDocStg,
						pDestLine->m_sStorageName,
						STGM_READWRITE | STGM_TRANSACTED | STGM_SHARE_EXCLUSIVE,
						0,
						0,
						&pDestObjStg
			);
	    }

		ASSERTNOERROR(hrErr);
		FailOleErr(hrErr);
	    		
	    // Copy over storage of the embedded object itself
	
	    if (! pSrcLine->site.m_pOleObj) {
	
	        /*****************************************************************
	        ** CASE 1: object is NOT loaded.
	        **    because the object is not loaded, we can simply copy the
	        **    object's current storage to the new storage.
	        *****************************************************************/
	
	        /* if current object storage is not already open, then open it */
	        if (! pSrcLine->m_pStorage) {
	            LPSTORAGE pSrcDocStg;
	
	            pSrcLineList = ((LinePtr)pSrcLine)->m_LineList;
	            pSrcDoc = (OleOutlineDocPtr)LineListGetDoc(pSrcLineList);
	
	            pSrcDocStg = OleOutlineDocGetStorage(pSrcDoc);
				ASSERTCOND(pSrcDocStg != nil);
	
	            // open object storage.sure.
	
		        hrErr = pSrcDocStg->lpVtbl->OpenStorage(
		                pSrcDocStg,
		                pSrcLine->m_sStorageName,
		                NULL,
		                STGM_READWRITE | STGM_TRANSACTED | STGM_SHARE_EXCLUSIVE,
		                NULL,
		                0,
		                &pSrcLine->m_pStorage
		        );

	            ASSERTNOERROR(hrErr);
	            FailOleErr(hrErr);
	        }
	
	        hrErr = pSrcLine->m_pStorage->lpVtbl->CopyTo(
	                pSrcLine->m_pStorage,
	                0,
	                NULL,
	                NULL,
	                pDestObjStg
	        );
	
	        ASSERTNOERROR(hrErr);
	        FailOleErr(hrErr);
	
		    // make the changes permanent
		    hrErr = pDestObjStg->lpVtbl->Commit(pDestObjStg, STGC_DEFAULT);
		
		    if (GetScode(hrErr) == STGC_OVERWRITE) {
		        // try to commit changes in less robust manner.
		        hrErr = pDestObjStg->lpVtbl->Commit(pDestObjStg, STGC_OVERWRITE);
		    }
		
		    ASSERTNOERROR(hrErr);
		    FailOleErr(hrErr);
	
	    } else {
	
	        /*****************************************************************
	        ** CASE 2: object IS loaded.
	        **    we must tell the object to save into the new storage.
	        *****************************************************************/
	
	        hrErr = OleContainerLineSaveOleObject(
							pSrcLine,
							pDestObjStg,
							false,	/* fSameAsLoad */
							false,	/* fRemember */
	                		true);  /* fForceUpdate */
			ASSERTNOERROR(hrErr);
			FailOleErr(hrErr);
	    }
	
    	OleStdVerifyRelease((LPUNKNOWN)pDestObjStg);

    	LineListAddLine(pDestLineList, (LinePtr)pDestLine);
	}
	CATCH
	{
		OleContainerLineRelease(pDestLine);
	}
	ENDTRY
	
}

//#pragma segment OleContainerLineSeg
char* OleContainerLineGetText(LinePtr pLine)
{
	static char Embed[]= "Embedded Object";		// REVIEW: hack
	static char Link[] = "Link Object";
	
	if (((OleContainerLinePtr)pLine)->m_fIsLink)
		return Link;
	else
		return Embed;
}

//#pragma segment OleContainerLineSeg
/* OleContainerLineIsOleLink
** -------------------------
**
**    return TRUE if the ContainerLine has an OleLink.
**           FALSE if the ContainerLine has an embedding
*/
Boolean OleContainerLineIsOleLink(OleContainerLinePtr pContainerLine)
{
	ASSERTCOND(pContainerLine);

	// REVIEW: use m_dwLinkType in Windows
	return pContainerLine->m_fIsLink;
}

//#pragma segment OleContainerLineSeg
void OleContainerLineSetLinkUnavailable(OleContainerLinePtr pContainerLine, Boolean fUnavail)
{
	pContainerLine->m_fLinkUnavailable = fUnavail;
}

//#pragma segment OleContainerLineSeg
Boolean OleContainerLineIsLinkUnavailable(OleContainerLinePtr pContainerLine)
{
	return pContainerLine->m_fLinkUnavailable;
}

//#pragma segment OleContainerLineSeg
void OleContainerLineDoConvert(OleContainerLinePtr pContainerLine, Boolean fServerNotRegistered)
{
	OLEUICONVERT	ct;
	HRESULT 		hrErr;
	Boolean			fMustActivate 		= false;
	Boolean			fHaveCLSID			= false;
	Boolean			fHaveFmtUserType	= false;
	Boolean			fObjConverted		= false;
	Boolean			fRunServer			= false;
	Boolean			fMustUpdate			= false;
	Boolean			fDisplayChanged		= false;
    char         	szUserType[128];
	unsigned int	uRet;
	char			szOle1ProgID[OLEUI_OLE1PROGIDMAX];
	
	
    /* OLE2NOTE: if we came to the Convert dialog because the user
    **    activated a non-registered object, then we should activate
    **    the object after the user has converted it or setup an
    **    ActivateAs server.
    */
    fMustActivate = fServerNotRegistered;

    OleStdMemSet(&ct, 0, sizeof(OLEUICONVERT));		// the standard memset in the library doesn't work
	
	ct.cbStruct = sizeof(OLEUICONVERT);
    ct.dvAspect = pContainerLine->m_DrawAspect;
	ct.fIsLinkedObject = OleContainerLineIsOleLink(pContainerLine);
	ct.lpfnHook 		 = OleOutlineDocUIDialogHook;
	

    if (! ct.fIsLinkedObject) {
        /* OLE2NOTE: the object is an embedded object. we should first
        **    attempt to read the actual object CLSID, file data
        **    format, and full user type name that is written inside of
        **    the object's storage as this should be the most
        **    definitive information. if this fails we will ask the
        **    object what its class is and attempt to get the rest of
        **    the information out of the REGDB.
        */
        hrErr = ReadClassStg(pContainerLine->m_pStorage, &ct.clsid);
        if (hrErr == NOERROR)
            fHaveCLSID = true;
        else {
            ASSERTCONDSZ(0, "ReadClassStg returned error");
        }

        hrErr = ReadFmtUserTypeStg(pContainerLine->m_pStorage, &ct.fmt.dwOle2Format, &ct.lpszUserType);
        if (hrErr == NOERROR)
            fHaveFmtUserType = true;
        else {
            ASSERTCONDSZ(0, "ReadFmtUserTypeStg returned error");
        }

        if (ct.fmt.dwOle2Format == 'OLE1') {
        	ct.fIsOle1Object = true;
        	ReadOle1FmtProgIDStgMac(pContainerLine->m_pStorage, &ct.fmt.lpszOle1Format);
        }
    }

    if (! fHaveCLSID) {
        hrErr = pContainerLine->site.m_pOleObj->lpVtbl->GetUserClassID(
                pContainerLine->site.m_pOleObj,
                &ct.clsid
        );
        if (hrErr != NOERROR)
            ct.clsid = CLSID_NULL;
    }
    if (! fHaveFmtUserType) {
        ct.fmt.dwOle2Format = 0;
        if (OleStdGetUserTypeOfClass(&ct.clsid, szUserType, sizeof(szUserType), 0)) {
            ct.lpszUserType = OleStdCopyString(szUserType, NULL);
        } else {
            ct.lpszUserType = NULL;
        }
    }

    if (pContainerLine->m_DrawAspect == DVASPECT_ICON) {
    	LPDATAOBJECT	pDataObj = nil;
    	STGMEDIUM		medium;

	    pDataObj = (LPDATAOBJECT)OleStdQueryInterface(
	    							(LPUNKNOWN)pContainerLine->site.m_pOleObj,
	    							&IID_IDataObject);
		ASSERTCOND(pDataObj != NULL);
		
		if (pDataObj) {
	        ct.hMetaPict = (PicHandle)OleStdGetData(
			                pDataObj,
			                'PICT',
			                NULL,
			                DVASPECT_ICON,
			                (LPSTGMEDIUM)&medium
	        );
	        OleStdRelease((LPUNKNOWN)pDataObj);
		}
    }

	{
		OleOutlineDocPtr	pOleOutlineDoc;
		
		pOleOutlineDoc = (OleOutlineDocPtr)((LinePtr)pContainerLine)->m_LineList->m_pDoc;
		
		OleDocEnableDialog(&pOleOutlineDoc->m_OleDoc);

    	uRet = OleUIConvert(&ct);
	
		OleDocDisableDialog(&pOleOutlineDoc->m_OleDoc, true);	// REVIEW: do we always resume inplace here?
	}
	
    // this may take a while, put up hourglass cursor
    SetCursor(*gWatchCursor);

    if (uRet == OLEUI_OK) {

        /*****************************************************************
        **  OLE2NOTE: the convert dialog actually allows the user to
        **    change two orthogonal properties of the object: the
        **    object's type/server and the object's display aspect.
        **    first we will execute the ConvertTo/ActivateAs action and
        **    then we will deal with any display aspect change. we want
        **    to be careful to only call IOleObject::Update once
        **    because this is an expensive operation; it results in
        **    launching the object's server.
        *****************************************************************/

        if (ct.dwFlags & CF_SELECTCONVERTTO &&
                ! IsEqualCLSID(&ct.clsid, &ct.clsidNew)) {

            /* user selected CONVERT.
            **
            ** OLE2NOTE: to achieve the "Convert To" at this point we
            **    need to take the following steps:
            **    1. unload the object.
            **    2. write the NEW CLSID and NEW user type name
            **       string into the storage of the object,
            **       BUT write the OLD format tag.
            **    3. force an update to force the actual conversion of
            **       the data bits.
            */
//            lpErrMsg = ErrMsgConvertObj; // setup correct msg in case of error

            OleContainerLineUnloadOleObject(pContainerLine, OLECLOSE_SAVEIFDIRTY);

            hrErr = OleStdDoConvert(
                    pContainerLine->m_pStorage, (REFCLSID)&ct.clsidNew);
            if (hrErr != NOERROR)
                goto error;

            // Reload the object
            OleContainerLineLoadOleObject(pContainerLine);

            /* we need to call IOleObject::Update to complete the
            **    conversion. set flag to force this Update at end of
            **    function.
            */
            fObjConverted = true;

        } else if (ct.dwFlags & CF_SELECTACTIVATEAS) {
            /* user selected ACTIVATE AS.
            **
            ** OLE2NOTE: to achieve the "Activate As" at this point we
            **    need to take the following steps:
            **    1. unload ALL objects of the OLD class that app knows about
            **    2. add the TreatAs tag in the registration database
            **    by calling CoTreatAsClass().
            **    3. lazily it can reload the objects; when the objects
            **    are reloaded the TreatAs will take effect.
            */
//            lpErrMsg = ErrMsgActivateAsObj; // setup msg in case of error

            OleOutlineContainerDocUnloadAllOleObjectsOfClass(
                    (OleOutlineDocPtr)((LinePtr)pContainerLine)->m_LineList->m_pDoc,
                    (REFCLSID)&ct.clsid,
                    OLECLOSE_SAVEIFDIRTY);

			hrErr = OleStdDoTreatAsClass(ct.lpszUserType, (REFCLSID)&ct.clsid,
					(REFCLSID)&ct.clsidNew);

            // Reload the object
            OleContainerLineLoadOleObject(pContainerLine);

            fMustActivate = TRUE;   // we should activate this object
        }

        /*****************************************************************
        **  OLE2NOTE: now we will try to change the display if
        **    necessary.
        *****************************************************************/

        if (pContainerLine->site.m_pOleObj && ct.dvAspect != pContainerLine->m_DrawAspect) {
            /* user has selected to change display aspect between icon
            **    aspect and content aspect.
            **
            ** OLE2NOTE: if we got here because the server was not
            **    registered, then we will NOT delete the object's
            **    original display aspect. because we do not have the
            **    original server, we can NEVER get it back. this is a
            **    safety precaution.
            */

            hrErr = OleStdSwitchDisplayAspect(
	                    pContainerLine->site.m_pOleObj,
	                    &pContainerLine->m_DrawAspect,
	                    ct.dvAspect,
	                    ct.hMetaPict,
	                    !fServerNotRegistered,   /* fDeleteOldAspect */
	                    TRUE,                    /* fSetupViewAdvise */
	                    (LPADVISESINK)&pContainerLine->m_AdviseSink2,
	                    &fMustUpdate);

            if (hrErr == NOERROR)
                fDisplayChanged = TRUE;

#if LATER && qOleInplace
                ContainerDoc_UpdateInPlaceObjectRects(
                        lpContainerLine->m_lpDoc, nIndex);
#endif

        } else if (ct.dvAspect == DVASPECT_ICON && ct.fObjectsIconChanged) {
            hrErr = OleStdSetIconInCache(
		                pContainerLine->site.m_pOleObj,
	                    ct.hMetaPict
            );

            if (hrErr == NOERROR)
                fDisplayChanged = TRUE;
        }

		/* we deliberately run the object so that the update won't shut
		** the server down.
		*/
		if (fMustActivate) {
			OleContainerLineRunOleObject(pContainerLine);
			fRunServer = TRUE;
		}
		
        if (fObjConverted || fMustUpdate) {
            hrErr = pContainerLine->site.m_pOleObj->lpVtbl->Update(
                    pContainerLine->site.m_pOleObj
            );

			if (GetScode(hrErr) == MK_E_MUSTBOTHERUSER) {
				LPOLELINK	pOleLink = nil;
				
				hrErr = pContainerLine->site.m_pOleObj->lpVtbl->QueryInterface(
							pContainerLine->site.m_pOleObj,
							&IID_IOleLink,
							(void**)&pOleLink);
							
				if (pOleLink) {
					IBindCtx	*pbc = nil;
					BIND_OPTS	bindopts;
					
		            CreateBindCtx(0, (LPBC *)&pbc);
		
		            bindopts.cbStruct = sizeof(BIND_OPTS);
		            pbc->lpVtbl->GetBindOptions(pbc, &bindopts);
		            bindopts.grfFlags |= BIND_MAYBOTHERUSER;
		            pbc->lpVtbl->SetBindOptions(pbc, &bindopts);
		
		            hrErr = pOleLink->lpVtbl->Update(pOleLink, pbc);
		
					pOleLink->lpVtbl->Release(pOleLink);
					pbc->lpVtbl->Release(pbc);
				}
			}						

            if (hrErr != NOERROR) {
                ASSERTCONDSZ(0, "IOleObject::Update returned");
                goto error;
            }
        }

        if (fDisplayChanged) {
            /* the Object's display was changed, force a repaint of
            **    the line. note the extents of the object may have
            **    changed. Force a delayed update of the line
            */
            ((LinePtr)pContainerLine)->m_fNeedsUpdateView = true;
			pContainerLine->m_fDoGetExtent = true;
        }

        if (fDisplayChanged || fObjConverted) {
            /* mark ContainerDoc as now dirty. if display changed, then
            **    the extents of the object may have changed.
            */
            DocumentPtr	pDoc;

            pDoc = ((LinePtr)pContainerLine)->m_LineList->m_pDoc;

            ASSERTCOND(pDoc->vtbl->m_SetDirtyProcPtr != nil);
            (*pDoc->vtbl->m_SetDirtyProcPtr)(pDoc, true);
        }
		
		if (fMustActivate)
			OleContainerLineDoVerb(pContainerLine, OLEIVERB_SHOW, false, false);
    }

    if (ct.hMetaPict)
        OleUIPictIconFree(ct.hMetaPict);    // clean up metafile

	if (ct.fIsOle1Object && ct.fmt.lpszOle1Format)
		OleStdFree(ct.fmt.lpszOle1Format);
	
	// clear watch cursor
	SetCursor(&qd.arrow);

    return;

error:
	if (fRunServer)
		OleContainerLineCloseOleObject(pContainerLine);

    if (ct.lpszUserType)
        OleStdFreeString(ct.lpszUserType, NULL);

    if (ct.hMetaPict)
        OleUIPictIconFree(ct.hMetaPict);    // clean up metafile

	if (ct.fIsOle1Object && ct.fmt.lpszOle1Format)
		OleStdFree(ct.fmt.lpszOle1Format);
	
	// clear watch cursor
    SetCursor(&qd.arrow);

#if LATER
    if (lpErrMsg)
        OutlineApp_ErrorMessage(g_lpApp, lpErrMsg);
#endif // LATER
}


//#pragma segment OleContainerLineSeg
short OleContainerLineFindHandle(OleContainerLinePtr pContainerLine, Point* pLocalPt)
{
	Rect	rLine;
	Rect	rHandle;
	LinePtr	pLine = (LinePtr)pContainerLine;
	
	LineGetBounds(pLine, &rLine);
	
	/* Loop through all handle position and see on which handle the point is located */
	
	// top left
	rHandle.left = rLine.left;
	rHandle.top = rLine.top;
	rHandle.right = rHandle.left + kHandleSize;
	rHandle.bottom = rHandle.top + kHandleSize;
	if (PtInRect(*pLocalPt, &rHandle))
		return 1;
		
	// middle left
	rHandle.top = rLine.top+(rLine.bottom-rLine.top-kHandleSize)/2;
	rHandle.right = rHandle.left + kHandleSize;
	rHandle.bottom = rHandle.top + kHandleSize;
	if (PtInRect(*pLocalPt, &rHandle))
		return 2;
		
	// bottom left
	rHandle.top = rLine.bottom-kHandleSize;
	rHandle.right = rHandle.left + kHandleSize;
	rHandle.bottom = rHandle.top + kHandleSize;
	if (PtInRect(*pLocalPt, &rHandle))
		return 3;
		
	// top middle
	rHandle.left = rLine.left+(rLine.right-rLine.left-kHandleSize)/2;
	rHandle.top = rLine.top;
	rHandle.right = rHandle.left + kHandleSize;
	rHandle.bottom = rHandle.top + kHandleSize;
	if (PtInRect(*pLocalPt, &rHandle))
		return 4;
		
	// bottom middle
	rHandle.top = rLine.bottom-kHandleSize;
	rHandle.right = rHandle.left + kHandleSize;
	rHandle.bottom = rHandle.top + kHandleSize;
	if (PtInRect(*pLocalPt, &rHandle))
		return 5;
		
	// top right	
	rHandle.left = rLine.right-kHandleSize;
	rHandle.top = rLine.top;
	rHandle.right = rHandle.left + kHandleSize;
	rHandle.bottom = rHandle.top + kHandleSize;
	if (PtInRect(*pLocalPt, &rHandle))
		return 6;
		
	// middle right
	rHandle.top = rLine.top+(rLine.bottom-rLine.top-kHandleSize)/2;
	rHandle.right = rHandle.left + kHandleSize;
	rHandle.bottom = rHandle.top + kHandleSize;
	if (PtInRect(*pLocalPt, &rHandle))
		return 7;
		
	// bottom right
	rHandle.top = rLine.bottom-kHandleSize;
	rHandle.right = rHandle.left + kHandleSize;
	rHandle.bottom = rHandle.top + kHandleSize;
	if (PtInRect(*pLocalPt, &rHandle))
		return 8;
		
	return kCPElseWhere;	// not on any handle
}

#endif  // qOleContainerApp

// OLDNAME: OleTextLine.c

//#pragma segment OleTextLineSeg
void TextLineSaveToStorage(LinePtr pLine, LPSTORAGE lpSrcStg, LPSTORAGE lpDestStg, LPSTREAM lpDestStm, Boolean fRemember)
{
	TextLinePtr		pTextLine;
	HRESULT			hrErr;
	unsigned long	nWritten;

	pTextLine = (TextLinePtr)pLine;

	ASSERTCOND(pTextLine->vtbl->m_SaveToStorageProcPtr != nil);
	(*pTextLine->vtbl->m_SaveToStorageProcPtr)(pLine, lpSrcStg, lpDestStg, lpDestStm, fRemember);

	pTextLine->m_TextLineLength = SwapWord(pTextLine->m_TextLineLength);

	hrErr = lpDestStm->lpVtbl->Write(
				lpDestStm,
				&pTextLine->m_TextLineLength,
				sizeof(pTextLine->m_TextLineLength),
				&nWritten
			);
	ASSERTNOERROR(hrErr);
	FailOleErr(hrErr);

	pTextLine->m_TextLineLength = SwapWord(pTextLine->m_TextLineLength);

	hrErr = lpDestStm->lpVtbl->Write(
				lpDestStm,
				pTextLine->m_Text,
				pTextLine->m_TextLineLength,
				&nWritten
			);
	ASSERTNOERROR(hrErr);
	FailOleErr(hrErr);
}

//#pragma segment OleTextLineSeg
void TextLineLoadFromStorage(TextLinePtr pTextLine, LPSTORAGE lpSrcStg, LPSTREAM lpSrcStm)
{
	HRESULT			hrErr;
	unsigned long	nRead;

	hrErr = lpSrcStm->lpVtbl->Read(
				lpSrcStm,
				&pTextLine->m_TextLineLength,
				sizeof(pTextLine->m_TextLineLength),
				&nRead
			);
	ASSERTNOERROR(hrErr);
	FailOleErr(hrErr);
	
	pTextLine->m_TextLineLength = SwapWord(pTextLine->m_TextLineLength);

	ASSERTCOND(pTextLine->m_TextLineLength <= kMaxTextLineLength);

	hrErr = lpSrcStm->lpVtbl->Read(
				lpSrcStm,
				pTextLine->m_Text,
				pTextLine->m_TextLineLength,
				&nRead
			);
	ASSERTNOERROR(hrErr);
	FailOleErr(hrErr);
}


#endif // qOle
