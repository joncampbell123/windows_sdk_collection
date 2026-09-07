/*****************************************************************************\
*                                                                             *
*    Table.c                                                                  *
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
#include "Table.h"
#include "Util.h"
#include "LowMem.h"
#include <ToolUtils.h>

static void TableRecalculate(TablePtr theTable);
static void TableUpdateScrollBars(TablePtr theTable);
static long TableCalcCellOffset(TablePtr theTable, short h, short v);
#ifndef _MSC_VER
static pascal void TableTrackScrollV(ControlHandle theControl, short part);
static pascal void TableTrackScrollH(ControlHandle theControl, short part);
#else
static void __pascal TableTrackScrollV(ControlHandle theControl, short part);
static void __pascal TableTrackScrollH(ControlHandle theControl, short part);
#endif
static Boolean TableGetCellAt(TablePtr theTable, Point localPt, Cell* theCell);

#ifdef __powerc

RoutineDescriptor gRDTableTrackScrollH =
	BUILD_ROUTINE_DESCRIPTOR(uppControlActionProcInfo,TableTrackScrollH);
ControlActionUPP gTableTrackScrollH = &gRDTableTrackScrollH;

RoutineDescriptor gRDTableTrackScrollV =
	BUILD_ROUTINE_DESCRIPTOR(uppControlActionProcInfo,TableTrackScrollV);
ControlActionUPP gTableTrackScrollV = &gRDTableTrackScrollV;
#endif


//#pragma segment TableSeg
TablePtr TableNew(Rect* rView, Rect* rDataBounds, Point cellSize,
			TableDrawCellProcPtr drawProcPtr,
			WindowPtr theWindow, Boolean drawIt, Boolean hasGrow, Boolean scrollHoriz, Boolean scrollVert)
{
	TablePtr	theTable;

	theTable = (TablePtr)NewPtrClear(sizeof(TableRec));
	ASSERTCOND(theTable != nil);
	if (theTable == nil)
		goto errorExit;

	theTable->m_Port = theWindow;
	theTable->m_HasGrow = hasGrow;

	theTable->m_View = *rView;
	if (scrollHoriz)
		theTable->m_View.right -= 15;
	if (scrollVert)
		theTable->m_View.bottom -= 15;

	if (!scrollHoriz)
		theTable->m_ScrollH = nil;
	else
	{
		Rect	r;

		r = *rView;
		r.top = r.bottom - 15;
		r.bottom += 1;
		r.left -= 1;

		if (hasGrow)
			r.right -= 14;

		theTable->m_ScrollH = NewControl(theWindow, &r, "\p", true, 0, 0, 0, scrollBarProc, (long)theTable);
		ASSERTCOND(theTable->m_ScrollH != nil);
	}

	if (!scrollVert)
		theTable->m_ScrollV = nil;
	else
	{
		Rect	r;

		r = *rView;
		r.left = r.right - 15;
		r.right += 1;
		r.top -= 1;

		if (hasGrow)
			r.bottom -= 14;

		theTable->m_ScrollV = NewControl(theWindow, &r, "\p", true, 0, 0, 0, scrollBarProc, (long)theTable);
		ASSERTCOND(theTable->m_ScrollV != nil);
	}

#ifdef __NEVER__
	theTable->m_CellSize = cellSize;
	if (theTable->m_CellSize.h == 0)
		theTable->m_CellSize.h = theTable->m_View.right - theTable->m_View.left;
	if (theTable->m_CellSize.v == 0)
	{
		FontInfo	fi;

		GetFontInfo(&fi);

		theTable->m_CellSize.v = fi.ascent + fi.descent + fi.leading;
	}
#endif

	ASSERTCOND(drawProcPtr != nil);
	theTable->m_DrawCellProcPtr = drawProcPtr;

	theTable->m_DrawCellFramedProcPtr = nil;
	theTable->m_DrawCellSelectedProcPtr = nil;

	theTable->m_DataBounds = *rDataBounds;

	theTable->m_Cells = NewHandleClear(sizeof(Ptr)
			* (rDataBounds->right - rDataBounds->left)
			* (rDataBounds->bottom - rDataBounds->top));
	ASSERTCOND(theTable->m_Cells != nil);
	if (theTable->m_Cells == nil)
		goto errorExit;

	{
		short	i;
		short*	p;

		for(i = rDataBounds->right - rDataBounds->left, p = theTable->m_ColumnWidth; i>0; i--)
			*p++ = theTable->m_CellSize.h;

		for(i = rDataBounds->bottom - rDataBounds->top, p = theTable->m_RowHeight; i>0; i--)
			*p++ = theTable->m_CellSize.v;
	}

	theTable->m_NeedsUpdateRgn = NewRgn();
	ASSERTCOND(theTable->m_NeedsUpdateRgn != nil);
	if (theTable->m_NeedsUpdateRgn == nil)
		goto errorExit;

	theTable->m_SelectionRgn = NewRgn();
	ASSERTCOND(theTable->m_SelectionRgn != nil);
	
	SetPt(&theTable->m_SelectionAnchor, 0, 0);
	
	theTable->m_LastClickTime = 0;

	theTable->m_NeedsRecalculation = true;
	theTable->m_NeedsScrollBarUpdate = true;
	theTable->m_SelectionFlags = 0;
	theTable->m_Active = true;
	theTable->m_DrawIt = drawIt;
	theTable->m_ListFlags = lDoVAutoscroll | lDoHAutoscroll;
	
	TableUpdateScrollBars(theTable);

	return theTable;

errorExit:

	if (theTable)
	{
		if (theTable->m_NeedsUpdateRgn != nil)		DisposeRgn(theTable->m_NeedsUpdateRgn);
		if (theTable->m_Cells != nil)				DisposeHandle(theTable->m_Cells);

		if (theTable->m_ScrollH != nil)				DisposeControl(theTable->m_ScrollH);
		if (theTable->m_ScrollV != nil)				DisposeControl(theTable->m_ScrollV);

		DisposePtr((Ptr)theTable);
	}

	return nil;
}

void TableDispose(TablePtr theTable)
{
	ASSERTCOND(theTable != nil);

	ASSERTCOND(theTable->m_NeedsUpdateRgn != nil);
	DisposeRgn(theTable->m_NeedsUpdateRgn);

	ASSERTCOND(theTable->m_Cells != nil);
	DisposeHandle(theTable->m_Cells);

#ifdef __OBSOLETE__
	ASSERTCOND(theTable->m_ScrollH != nil);
	DisposeControl(theTable->m_ScrollH);

	ASSERTCOND(theTable->m_ScrollV != nil);
	DisposeControl(theTable->m_ScrollV);
#endif
	
	ASSERTCOND(theTable->m_SelectionRgn != nil);
	DisposeRgn(theTable->m_SelectionRgn);
	
	DisposePtr((Ptr)theTable);
}

void TableSetDrawCellSelectedProc(TablePtr theTable, TableDrawCellSelectedProcPtr proc)
{
	theTable->m_DrawCellSelectedProcPtr = proc;
}

void TableSetDrawCellFramedProc(TablePtr theTable, TableDrawCellFramedProcPtr proc)
{
	theTable->m_DrawCellFramedProcPtr = proc;
}

void TableSetRefCon(TablePtr theTable, void* refCon)
{
	theTable->m_RefCon = refCon;
}

void* TableGetRefCon(TablePtr theTable)
{
	return theTable->m_RefCon;
}

void TableTempSize(TablePtr theTable, short tableWidth, short tableHeight)
{
	theTable->m_TempView = theTable->m_View;

	theTable->m_TempView.right = theTable->m_TempView.left + tableWidth;
	theTable->m_TempView.bottom = theTable->m_TempView.top + tableHeight;

	if (theTable->m_ScrollV)
		theTable->m_TempView.bottom -= 15;

	if (theTable->m_ScrollH)
		theTable->m_TempView.right -= 15;

	theTable->m_HaveTempView = true;
}

void TableSize(TablePtr theTable, short tableWidth, short tableHeight)
{
	GrafPtr		oldPort;
	Rect		rView;
	
	GetPort(&oldPort);
	SetPort(theTable->m_Port);
	
	if (theTable->m_HasGrow)
	{
		Rect	r;
		
		r = theTable->m_View;
		r.left = r.right - 15;
		r.top = r.bottom - 15;
		
		InvalRect(&r);
	}

	theTable->m_View.right = theTable->m_View.left + tableWidth;
	theTable->m_View.bottom = theTable->m_View.top + tableHeight;
	rView = theTable->m_View;
	
	if (theTable->m_ScrollV)
	{
		Rect	r;

		theTable->m_View.bottom -= 15;

		r = rView;
		r.left = r.right - 15;
		r.right += 1;
		r.top -= 1;

		if (theTable->m_HasGrow)
			r.bottom -= 14;

		HideControl(theTable->m_ScrollV);
		MoveControl(theTable->m_ScrollV, r.left, r.top);
		SizeControl(theTable->m_ScrollV, (short)(r.right - r.left), (short)(r.bottom - r.top));
		ShowControl(theTable->m_ScrollV);
	}

	if (theTable->m_ScrollH)
	{
		Rect	r;

		theTable->m_View.right -= 15;

		r = rView;
		r.top = r.bottom - 15;
		r.bottom += 1;
		r.left -= 1;

		if (theTable->m_HasGrow)
			r.right -= 14;

		HideControl(theTable->m_ScrollH);
		MoveControl(theTable->m_ScrollH, r.left, r.top);
		SizeControl(theTable->m_ScrollH, (short)(r.right - r.left), (short)(r.bottom - r.top));
		ShowControl(theTable->m_ScrollH);
	}
	
	if (theTable->m_HasGrow)
	{
		Rect	r;
		
		r = rView;
		r.left = r.right - 15;
		r.top = r.bottom - 15;
		
		InvalRect(&r);
	}

	SetPort(oldPort);

	theTable->m_NeedsRecalculation = true;
	theTable->m_NeedsScrollBarUpdate = true;

	theTable->m_HaveTempView = false;
}

void TableDoDraw(TablePtr theTable, Boolean drawIt)
{
	theTable->m_DrawIt = drawIt;

	if (drawIt)
	{
		GrafPtr		oldPort;

		GetPort(&oldPort);
		SetPort(theTable->m_Port);

		if (theTable->m_NeedsScrollBarUpdate)
		{
			short	hOffset,
					vOffset;

			hOffset = GetCtlValue(theTable->m_ScrollH);
			vOffset = GetCtlValue(theTable->m_ScrollV);

			TableUpdateScrollBars(theTable);

			if (GetCtlValue(theTable->m_ScrollH) != hOffset ||
				GetCtlValue(theTable->m_ScrollV) != vOffset)
			{
				if (theTable->m_ClickLoopProcPtr != nil)
					(*theTable->m_ClickLoopProcPtr)(theTable);

				theTable->m_NeedsRecalculation = true;
				TableRecalculate(theTable);
				InvalRect(&theTable->m_Port->portRect);
			}

		}

#ifdef __NEVER__
		EraseRgn(theTable->m_NeedsUpdateRgn);
		TableUpdate(theTable, theTable->m_NeedsUpdateRgn);
		SetEmptyRgn(theTable->m_NeedsUpdateRgn);
#else
		EraseRect(&theTable->m_View);
		TableUpdate(theTable, theTable->m_Port->visRgn);
		TableUpdateScrollBars(theTable);
	
		ValidRect(&theTable->m_View);
#endif

		SetPort(oldPort);
	}
}

void TableDrawIntoPort(TablePtr theTable, GrafPtr thePort, Rect* rBounds)
{
	GrafPtr		oldPort;
	RgnHandle	oldClipRgn;
	RgnHandle	tempRgn;
	
	GetPort(&oldPort);
	SetPort(thePort);
	
	oldClipRgn = NewRgn();
	ASSERTCOND(oldClipRgn != nil);
	GetClip(oldClipRgn);
	
	tempRgn = NewRgn();
	ASSERTCOND(tempRgn != nil);
	
	TableRecalculate(theTable);
	
	{
		short	i;
		short	j;
		short*	pRowStart;
		short*	pColumnStart;
		short*	pRowHeight;
		short*	pColWidth;
		short	indexLeft;
		short	indexTop;		
		Rect	cellRect;

		indexLeft = rBounds->left - theTable->m_DataBounds.left;
		ASSERTCOND(indexLeft >= 0);
		pColumnStart = &theTable->m_ColumnStart[indexLeft];
		pColWidth = &theTable->m_ColumnWidth[indexLeft];

		indexTop = rBounds->top - theTable->m_DataBounds.top;
		ASSERTCOND(indexTop >= 0);
		pRowStart = &theTable->m_RowStart[indexTop];
		pRowHeight = &theTable->m_RowHeight[indexTop];


		for(i = rBounds->left; i < rBounds->right; i++, pColumnStart++, pColWidth++)
		{
			cellRect.left = *pColumnStart;
			cellRect.right = cellRect.left + *pColWidth;

			for(j = rBounds->top; j < rBounds->bottom; j++, pRowStart++, pRowHeight++)
			{
				Rect	r;

				cellRect.top = *pRowStart - theTable->m_RowStart[indexTop];
				cellRect.bottom = cellRect.top + *pRowHeight;

				r = cellRect;

				if (RectInRgn(&r, oldClipRgn))
				{
					RectRgn(tempRgn, &r);
					SectRgn(tempRgn, oldClipRgn, tempRgn);
					SetClip(tempRgn);

					(*theTable->m_DrawCellProcPtr)(theTable, &cellRect,
								*((Ptr *)((char *)*theTable->m_Cells + TableCalcCellOffset(theTable, i, j))));
				}
			}
		}
	}

	SetClip(oldClipRgn);
	DisposeRgn(oldClipRgn);
	DisposeRgn(tempRgn);

	SetPort(oldPort);
}

void TableDraw(TablePtr theTable, Cell theCell, Boolean fClip)
{
	Rect		cellRect;
	void*		dataPtr;
	GrafPtr		oldPort;
	RgnHandle	oldClipRgn;
	RgnHandle	newClipRgn;
	RgnHandle	tempRgn;

	if (!theTable->m_DrawIt)
		return;
	
	// make sure everything is recaculated
	TableRecalculate(theTable);

	if (!PtInRect(theCell, &theTable->m_Visible))
		return;
	
	GetPort(&oldPort);
	SetPort(theTable->m_Port);
	
	if (fClip)
	{
		oldClipRgn = NewRgn();
		ASSERTCOND(oldClipRgn != nil);
		GetClip(oldClipRgn);

		tempRgn = NewRgn();
		ASSERTCOND(tempRgn != nil);
		RectRgn(tempRgn, &theTable->m_View);

		newClipRgn = NewRgn();
		ASSERTCOND(newClipRgn != nil);
		SectRgn(oldClipRgn, tempRgn, newClipRgn);

		SetClip(newClipRgn);
	}

	TableGetCellRect(theTable, theCell, &cellRect);
	dataPtr = *((Ptr *)((char *)*theTable->m_Cells + TableCalcCellOffset(theTable, theCell.h, theCell.v)));
	
	if (PtInRgn(theCell, theTable->m_SelectionRgn))
	{
		if (theTable->m_DrawCellSelectedProcPtr == nil)
		{
			(*theTable->m_DrawCellProcPtr)(theTable, &cellRect, dataPtr);

			LMSetHiliteMode(LMGetHiliteMode() & ~(1<<hiliteBit));
			InvertRect(&cellRect);
		}
		else
			(*theTable->m_DrawCellSelectedProcPtr)(theTable, true, &cellRect, dataPtr);
	}
	else
		(*theTable->m_DrawCellProcPtr)(theTable, &cellRect, dataPtr);
		
	if (fClip)
	{
		SetClip(oldClipRgn);
		DisposeRgn(oldClipRgn);
		DisposeRgn(newClipRgn);
		DisposeRgn(tempRgn);
	}

	SetPort(oldPort);
}

void TableUpdate(TablePtr theTable, RgnHandle theRgn)
{
	GrafPtr		oldPort;
	RgnHandle	oldClipRgn;
	RgnHandle	newClipRgn;
	RgnHandle	tempRgn;

	if (!theTable->m_DrawIt)
		return;

	GetPort(&oldPort);
	SetPort(theTable->m_Port);

	TableRecalculate(theTable);

	if (theTable->m_ScrollH != nil || theTable->m_ScrollV != nil)
		UpdtControl(theTable->m_Port, theRgn);

	oldClipRgn = NewRgn();
	ASSERTCOND(oldClipRgn != nil);
	GetClip(oldClipRgn);

	newClipRgn = NewRgn();
	ASSERTCOND(newClipRgn != nil);
	SectRgn(oldClipRgn, theRgn, newClipRgn);

	tempRgn = NewRgn();
	ASSERTCOND(tempRgn != nil);
	RectRgn(tempRgn, &theTable->m_View);
	SectRgn(newClipRgn, tempRgn, newClipRgn);

	SetClip(newClipRgn);

	EraseRect(&theTable->m_View);

	{
		short	i;
		short	j;
		short*	pRowStart;
		short*	pColumnStart;
		short*	pRowHeight;
		short*	pColWidth;
		short	index;
		Rect	cellRect;

		index = theTable->m_Visible.left - theTable->m_DataBounds.left;
		ASSERTCOND(index >= 0);
		pColumnStart = &theTable->m_ColumnStart[index];
		pColWidth = &theTable->m_ColumnWidth[index];

		for(i = theTable->m_Visible.left; i < theTable->m_Visible.right; i++, pColumnStart++, pColWidth++)
		{
			index = theTable->m_Visible.top - theTable->m_DataBounds.top;
			ASSERTCOND(index >= 0);
			pRowStart = &theTable->m_RowStart[index];
			pRowHeight = &theTable->m_RowHeight[index];

			cellRect.left = *pColumnStart + theTable->m_View.left;
			cellRect.right = cellRect.left + *pColWidth;

			for(j = theTable->m_Visible.top; j < theTable->m_Visible.bottom; j++, pRowStart++, pRowHeight++)
			{
				Rect	r;

				cellRect.top = *pRowStart + theTable->m_View.top;
				cellRect.bottom = cellRect.top + *pRowHeight;

				r = cellRect;

				if (SectRect(&r, &theTable->m_View, &r))
				{
					if (RectInRgn(&r, newClipRgn))
					{
						Cell	theCell;
						
						RectRgn(tempRgn, &r);
						SectRgn(tempRgn, newClipRgn, tempRgn);
						SetClip(tempRgn);
						
						theCell.h = i;
						theCell.v = j;
						
						TableDraw(theTable, theCell, false);
					}
				}
			}
		}
	}

	SetClip(oldClipRgn);
	DisposeRgn(oldClipRgn);
	DisposeRgn(newClipRgn);
	DisposeRgn(tempRgn);

	SetPort(oldPort);
}

void TableActivate(TablePtr theTable, Boolean activateIt)
{
	if (theTable->m_ScrollH != nil)
		HiliteControl(theTable->m_ScrollH, (short)(activateIt ? 0 : 255));

	if (theTable->m_ScrollV != nil)
		HiliteControl(theTable->m_ScrollV, (short)(activateIt ? 0 : 255));
}

Boolean TableClick(TablePtr theTable, Point localPt, short modifiers)
{
	ASSERTCOND(theTable->m_Active == true);
	ASSERTCOND(theTable->m_DrawIt == true);

	if (theTable->m_ScrollH != nil)
	{
		short	part;
		short	ctlValue;

		ctlValue = GetCtlValue(theTable->m_ScrollH);

		if ((part = TestControl(theTable->m_ScrollH, localPt)) != 0)
		{
#ifdef __powerc
			TrackControl(theTable->m_ScrollH, localPt, part == inThumb ? nil : gTableTrackScrollH);
#else
			TrackControl(theTable->m_ScrollH, localPt, part == inThumb ? nil : (ControlActionUPP)TableTrackScrollH);
#endif

			if (part == inThumb)
			{
				short	delta;

				delta = ctlValue - GetCtlValue(theTable->m_ScrollH);

				if (delta != 0)
				{
					RgnHandle	updateRgn;

					updateRgn = NewRgn();
					ASSERTCOND(updateRgn != nil);

					ScrollRect(&theTable->m_View, delta, 0, updateRgn);
					theTable->m_NeedsRecalculation = true;
					TableUpdate(theTable, updateRgn);

					DisposeRgn(updateRgn);
				}

				if (theTable->m_ClickLoopProcPtr != nil)
					(*theTable->m_ClickLoopProcPtr)(theTable);
			}

			return false;
		}
	}

	if (theTable->m_ScrollV != nil)
	{
		short	part;
		short	ctlValue;

		ctlValue = GetCtlValue(theTable->m_ScrollV);

		if ((part = TestControl(theTable->m_ScrollV, localPt)) != 0)
		{
#ifndef __powerc
			TrackControl(theTable->m_ScrollV, localPt, part == inThumb ? nil : (ControlActionUPP)TableTrackScrollV);
#else
			TrackControl(theTable->m_ScrollV, localPt, part == inThumb ? nil : gTableTrackScrollV);
#endif

			if (part == inThumb)
			{
				short	delta;

				delta = ctlValue - GetCtlValue(theTable->m_ScrollV);

				if (delta != 0)
				{
					RgnHandle	updateRgn;

					updateRgn = NewRgn();
					ASSERTCOND(updateRgn != nil);

					ScrollRect(&theTable->m_View, 0, delta, updateRgn);
					theTable->m_NeedsRecalculation = true;
					TableUpdate(theTable, updateRgn);

					DisposeRgn(updateRgn);
				}

				if (theTable->m_ClickLoopProcPtr != nil)
					(*theTable->m_ClickLoopProcPtr)(theTable);
			}

			return false;
		}
	}

	if (PtInRect(localPt, &theTable->m_View))
	{
		Cell	theCell;

		if (TableGetCellAt(theTable, localPt, &theCell))
		{
			Boolean		ret = false;
			
			// possible double click? Was it quick enough, and the same cell?
			if (TickCount() - theTable->m_LastClickTime <= GetDblTime() &&
				EqualPt(theTable->m_LastClickCell, theCell))
			{
				Rect	r;
				
				// now make sure the click was close to the last one
				r.left = theTable->m_LastClickLoc.h - 4;
				r.right = theTable->m_LastClickLoc.h + 4;
				r.top = theTable->m_LastClickLoc.v - 4;
				r.bottom = theTable->m_LastClickLoc.v + 4;
				
				if (PtInRect(localPt, &r))
					ret = true;
			}

			theTable->m_LastClickCell = theCell;
			theTable->m_LastClickLoc = localPt;

			theTable->m_LastClickTime = TickCount();
			
			if (ret)
				return true;
		}
	}

	{
		RgnHandle	oldClipRgn;
		RgnHandle	newClipRgn;
		RgnHandle	tempRgn;
		RgnHandle	diffRgn;
		Rect		cellRect;
		Cell		theCell;
		RgnHandle	newSelectionRgn;
		
		oldClipRgn = NewRgn();
		ASSERTCOND(oldClipRgn != nil);
		GetClip(oldClipRgn);

		tempRgn = NewRgn();
		ASSERTCOND(tempRgn != nil);
		RectRgn(tempRgn, &theTable->m_View);
		
		diffRgn = NewRgn();
		ASSERTCOND(diffRgn != nil);
		
		newSelectionRgn = NewRgn();
		ASSERTCOND(newSelectionRgn != nil);

		newClipRgn = NewRgn();
		ASSERTCOND(newClipRgn != nil);
		SectRgn(oldClipRgn, tempRgn, newClipRgn);
		SetEmptyRgn(tempRgn);
		
		if (!IsShiftKeyDown() || EmptyRgn(theTable->m_SelectionRgn))
		{
			if (!TableGetCellAt(theTable, localPt, &theCell) || !EqualPt(theCell, theTable->m_SelectionAnchor))
				TableClearSelection(theTable);
			TableGetCellAt(theTable, localPt, &theTable->m_SelectionAnchor);
		}
		
		TableRecalculate(theTable);
		
		do {
			if (TableGetCellAt(theTable, localPt, &theCell))
			{
				Rect	r1, r2;
								
				topLeft(r1) = theTable->m_SelectionAnchor;
				botRight(r1) = theTable->m_SelectionAnchor;
				r1.right++;
				r1.bottom++;
				
				topLeft(r2) = theCell;
				botRight(r2) = theCell;
				r2.right++;
				r2.bottom++;
				
				UnionRect(&r1, &r2, &r1);
				RectRgn(newSelectionRgn, &r1);
				
				if (!EqualRgn(newSelectionRgn, theTable->m_SelectionRgn))
				{
					short	i;
					short	j;
					
					CopyRgn(newSelectionRgn, diffRgn);

					// is there a selection
					if (!EmptyRgn(theTable->m_SelectionRgn))
						XorRgn(diffRgn, theTable->m_SelectionRgn, diffRgn);
					
					ASSERTCOND(EmptyRgn(diffRgn) == false);
										
					for(i = theTable->m_Visible.left; i < theTable->m_Visible.right; i++)
					{
						theCell.h = i;

						for(j = theTable->m_Visible.top; j < theTable->m_Visible.bottom; j++)
						{
							theCell.v = j;
							
							if (PtInRgn(theCell, diffRgn))
							{
								TableGetCellRect(theTable, theCell, &cellRect);

								RectRgn(tempRgn, &cellRect);
								SectRgn(tempRgn, newClipRgn, tempRgn);
								SetClip(tempRgn);
	
								if (theTable->m_DrawCellSelectedProcPtr == nil)
								{
									LMSetHiliteMode(LMGetHiliteMode() & ~(1<<hiliteBit));
									InvertRect(&cellRect);
								}
								else
									(*theTable->m_DrawCellSelectedProcPtr)(
												theTable,
												PtInRgn(theCell, newSelectionRgn),
												&cellRect,
												*((Ptr *)((char *)*theTable->m_Cells + TableCalcCellOffset(theTable, theCell.h, theCell.v))));
							}
						}
					}

					CopyRgn(newSelectionRgn, theTable->m_SelectionRgn);
				}
			}
						
			// get the new mouse point
			GetMouse(&localPt);
			
			if (theTable->m_ClickLoopProcPtr != nil)
			{
				if (!(*theTable->m_ClickLoopProcPtr)(theTable))
					break;
			}
		} while (StillDown());

		SetClip(oldClipRgn);
		DisposeRgn(oldClipRgn);
		DisposeRgn(newClipRgn);
		DisposeRgn(tempRgn);
		DisposeRgn(diffRgn);
		DisposeRgn(newSelectionRgn);
	}

	return false;
}

Boolean TableIsSelection(TablePtr theTable)
{
	return !EmptyRgn(theTable->m_SelectionRgn);
}

Boolean TableIsEmpty(TablePtr theTable)
{
	return EmptyRect(&theTable->m_DataBounds);
}

void TableSetSelect(TablePtr theTable, Cell theCell, Boolean fSetIt)
{
	Rect		r;
	RgnHandle	tempRgn;
	
	if (!PtInRect(theCell, &theTable->m_DataBounds))
		return;
	
	topLeft(r) = theCell;
	botRight(r) = theCell;
	r.right++;
	r.bottom++;
	
	tempRgn = NewRgn();
	ASSERTCOND(tempRgn != nil);
	
	RectRgn(tempRgn, &r);
	
	if (fSetIt)
		UnionRgn(tempRgn, theTable->m_SelectionRgn, theTable->m_SelectionRgn);
	else
		DiffRgn(theTable->m_SelectionRgn, tempRgn, theTable->m_SelectionRgn);
		
	theTable->m_SelectionAnchor = topLeft((**theTable->m_SelectionRgn).rgnBBox);

	TableDraw(theTable, theCell, false);
	
	DisposeRgn(tempRgn);
}

Boolean TableGetSelect(TablePtr theTable, Boolean advanceIt, Cell* theCell)
{
	Cell		aCell;
	
	if (!advanceIt)
		return PtInRgn(*theCell, theTable->m_SelectionRgn);
		
	aCell = *theCell;
	
	while(!PtInRgn(aCell, theTable->m_SelectionRgn))
	{
		if (!TableNextCell(theTable, true, true, &aCell))
			return false;
	}
	
	*theCell = aCell;
		
	return true;
}

Boolean TableNextCell(TablePtr theTable, Boolean hNext, Boolean vNext, Cell* theCell)
{
	if (hNext && vNext)
	{
		theCell->h++;
		
		if (PtInRect(*theCell, &theTable->m_DataBounds))
			return true;
			
		theCell->h = theTable->m_DataBounds.left;
		theCell->v++;
		
		return PtInRect(*theCell, &theTable->m_DataBounds);
	}
	
	if (hNext)
	{
		theCell->h++;
		
		return PtInRect(*theCell, &theTable->m_DataBounds);
	}
	
	if (vNext)
	{	
		theCell->v++;
		
		return PtInRect(*theCell, &theTable->m_DataBounds);
	}
	
	return false;
}

void TableClearSelection(TablePtr theTable)
{
	if (EmptyRgn(theTable->m_SelectionRgn))
		return;
	
	TableRecalculate(theTable);

	{
		short		i;
		short		j;
		Cell		theCell;
		RgnHandle	oldClipRgn;
		RgnHandle	newClipRgn;
		RgnHandle	tempRgn;
		GrafPtr		oldPort;
		
		GetPort(&oldPort);
		SetPort(theTable->m_Port);
		
		oldClipRgn = NewRgn();
		ASSERTCOND(oldClipRgn != nil);
		GetClip(oldClipRgn);

		tempRgn = NewRgn();
		ASSERTCOND(tempRgn != nil);
		RectRgn(tempRgn, &theTable->m_View);

		newClipRgn = NewRgn();
		ASSERTCOND(newClipRgn != nil);
		SectRgn(oldClipRgn, tempRgn, newClipRgn);
		SetEmptyRgn(tempRgn);
		
		for(i = theTable->m_Visible.left; i < theTable->m_Visible.right; i++)
		{
			for(j = theTable->m_Visible.top; j < theTable->m_Visible.bottom; j++)
			{
				theCell.h = i;
				theCell.v = j;
				
				if (PtInRgn(theCell, theTable->m_SelectionRgn))
				{
					Rect	cellRect;
					
					TableGetCellRect(theTable, theCell, &cellRect);	

					RectRgn(tempRgn, &cellRect);
					SectRgn(tempRgn, newClipRgn, tempRgn);
					SetClip(tempRgn);

					if (theTable->m_DrawCellSelectedProcPtr == nil)
					{
						LMSetHiliteMode(LMGetHiliteMode() & ~(1<<hiliteBit));
						InvertRect(&cellRect);
					}
					else
					{
						(*theTable->m_DrawCellSelectedProcPtr)(theTable, false, &cellRect,
								*((Ptr *)((char *)*theTable->m_Cells + TableCalcCellOffset(theTable, i, j))));
					}
				}
			}
		}
		
		SetClip(oldClipRgn);
		DisposeRgn(oldClipRgn);
		DisposeRgn(newClipRgn);
		DisposeRgn(tempRgn);
	
		SetPort(oldPort);
	}

	SetEmptyRgn(theTable->m_SelectionRgn);
}

#ifndef _MSC_VER
pascal void
#else
void __pascal
#endif
TableTrackScrollH(ControlHandle theControl, short part)
{
	TablePtr	theTable;
	short		delta;

	theTable = (TablePtr)GetCRefCon(theControl);
	ASSERTCOND(theTable != nil);
	ASSERTCOND(theTable->m_NeedsScrollBarUpdate == false);

	switch(part)
	{
		case inPageUp:
				delta = -(theTable->m_View.right - theTable->m_View.left);
				break;
		case inPageDown:
				delta = theTable->m_View.right - theTable->m_View.left;
				break;
		case inUpButton:
				delta = -theTable->m_ColumnWidth[theTable->m_Visible.left - theTable->m_DataBounds.left];
				break;
		case inDownButton:
				delta = theTable->m_ColumnWidth[theTable->m_Visible.right - theTable->m_DataBounds.left - 1];
				break;
		default:
				return;
	}

	{
		short		ctlValue;

		ctlValue = GetCtlValue(theControl);

		SetCtlValue(theControl, (short)(ctlValue + delta));
		delta = ctlValue - GetCtlValue(theControl);

		if (delta != 0)
		{
			RgnHandle	updateRgn;

			updateRgn = NewRgn();
			ASSERTCOND(updateRgn != nil);

			ScrollRect(&theTable->m_View, delta, 0, updateRgn);
			theTable->m_NeedsRecalculation = true;
			TableUpdate(theTable, updateRgn);

			DisposeRgn(updateRgn);
		}
	}
	
	if (theTable->m_ClickLoopProcPtr != nil)
		(*theTable->m_ClickLoopProcPtr)(theTable);
}

#ifndef _MSC_VER
pascal void
#else
void __pascal
#endif
TableTrackScrollV(ControlHandle theControl, short part)
{
	TablePtr	theTable;
	short		delta;

	theTable = (TablePtr)GetCRefCon(theControl);
	ASSERTCOND(theTable != nil);
	ASSERTCOND(theTable->m_NeedsScrollBarUpdate == false);

	switch(part)
	{
		case inPageUp:
				delta = -(theTable->m_View.bottom - theTable->m_View.top);
				break;
		case inPageDown:
				delta = theTable->m_View.bottom - theTable->m_View.top;
				break;
		case inUpButton:
				delta = -theTable->m_RowHeight[theTable->m_Visible.top - theTable->m_DataBounds.top];
				break;
		case inDownButton:
				delta = theTable->m_RowHeight[theTable->m_Visible.bottom - theTable->m_DataBounds.top - 1];
				break;
		default:
				return;
	}

	{
		short		ctlValue;

		ctlValue = GetCtlValue(theControl);

		SetCtlValue(theControl, (short)(ctlValue + delta));
		delta = ctlValue - GetCtlValue(theControl);

		if (delta != 0)
		{
			RgnHandle	updateRgn;

			updateRgn = NewRgn();
			ASSERTCOND(updateRgn != nil);

			ScrollRect(&theTable->m_View, 0, delta, updateRgn);
			theTable->m_NeedsRecalculation = true;
			TableUpdate(theTable, updateRgn);

			DisposeRgn(updateRgn);
		}
	}

	if (theTable->m_ClickLoopProcPtr != nil)
		(*theTable->m_ClickLoopProcPtr)(theTable);
}

void TableSetCell(TablePtr theTable, void* dataPtr, Cell theCell)
{
	ASSERTCOND(theCell.h >= theTable->m_DataBounds.left && theCell.h < theTable->m_DataBounds.right);
	ASSERTCOND(theCell.v >= theTable->m_DataBounds.top && theCell.v < theTable->m_DataBounds.bottom);

	*(void **)((char *)*theTable->m_Cells + TableCalcCellOffset(theTable, theCell.h, theCell.v)) = dataPtr;
}

void TableGetCell(TablePtr theTable, void* dataPtr, Cell theCell)
{
#ifdef __NEVER__
	ASSERTCOND(theCell.h >= theTable->m_DataBounds.left && theCell.h < theTable->m_DataBounds.right);
	ASSERTCOND(theCell.v >= theTable->m_DataBounds.top && theCell.v < theTable->m_DataBounds.bottom);
#endif

	if (theCell.h < theTable->m_DataBounds.left || theCell.h >= theTable->m_DataBounds.right
				|| theCell.v < theTable->m_DataBounds.top || theCell.v >= theTable->m_DataBounds.bottom)
		*((Ptr *)dataPtr) = nil;
	else
		*((Ptr *)dataPtr) = *((Ptr *)((char *)*theTable->m_Cells + TableCalcCellOffset(theTable, theCell.h, theCell.v)));
}

Boolean TableAddRow(TablePtr theTable, short count, short *rowNum)
{
	ASSERTCOND(rowNum != nil);
	ASSERTCOND(*rowNum >= theTable->m_DataBounds.top);
	ASSERTCOND(count > 0);
	ASSERTCOND(count + theTable->m_DataBounds.bottom - theTable->m_DataBounds.top <= kMaxRows);

	if (count + theTable->m_DataBounds.bottom - theTable->m_DataBounds.top > kMaxRows)
		count = 0;
		
	SetHandleSize(theTable->m_Cells, sizeof(Ptr) * (theTable->m_DataBounds.right - theTable->m_DataBounds.left)
												* (theTable->m_DataBounds.bottom - theTable->m_DataBounds.top + count));

	if (*rowNum >= theTable->m_DataBounds.bottom)
	{
		// adding on rows
		*rowNum = theTable->m_DataBounds.bottom;
	}
	else
	{
		// need to insert
		BlockMove((void *)((char *)*theTable->m_Cells + TableCalcCellOffset(theTable, theTable->m_DataBounds.left, *rowNum)),
					(void *)((char *)*theTable->m_Cells + TableCalcCellOffset(theTable, theTable->m_DataBounds.left, (short)(*rowNum + count))),
					sizeof(Ptr) * (theTable->m_DataBounds.right - theTable->m_DataBounds.left) * (theTable->m_DataBounds.bottom - *rowNum));

		BlockMove(&theTable->m_RowHeight[*rowNum - theTable->m_DataBounds.top], &theTable->m_RowHeight[*rowNum - theTable->m_DataBounds.top + count], sizeof(short) * (theTable->m_DataBounds.bottom - *rowNum));
	}

	// update row heights
	{
		short	i;
		short*	p;

		for(i = count, p = &theTable->m_RowHeight[*rowNum - theTable->m_DataBounds.top]; i>0; i--)
			*p++ = theTable->m_CellSize.v;
	}
	
	// if there is a selection then adjust if
	if (!EmptyRgn(theTable->m_SelectionRgn))
	{
		Rect		r;
		RgnHandle	newSelectionRgn;
		RgnHandle	theRgn;
		
		newSelectionRgn = NewRgn();
		ASSERTCOND(newSelectionRgn != nil);
		
		theRgn = NewRgn();
		ASSERTCOND(theRgn != nil);

		// get region above delete
		r = theTable->m_DataBounds;
		r.bottom = *rowNum;
		
		RectRgn(theRgn, &r);
		
		SectRgn(theTable->m_SelectionRgn, theRgn, newSelectionRgn);
		
		// get region below delete
		r = theTable->m_DataBounds;
		r.top = *rowNum;
		
		RectRgn(theRgn, &r);
		
		SectRgn(theTable->m_SelectionRgn, theRgn, theRgn);
		
		// if not empty shift it up
		if (!EmptyRgn(theRgn))
		{
			OffsetRgn(theRgn, 0, count);
			UnionRgn(newSelectionRgn, theRgn, newSelectionRgn);
		}
		
		DisposeRgn(theRgn);

		CopyRgn(newSelectionRgn, theTable->m_SelectionRgn);
		DisposeRgn(newSelectionRgn);

		// need more logic for calculating selection point
		SetPt(&theTable->m_SelectionAnchor, 0, 0);
	}

	theTable->m_DataBounds.bottom += count;
	theTable->m_NeedsRecalculation = true;
	theTable->m_NeedsScrollBarUpdate = true;

#ifdef __NEVER__
	{
		Rect	r;

		r = theTable->m_View;

		r.top = theTable->m_View.top + theTable->m_RowStart[*rowNum - theTable->m_DataBounds.top];
	}
#endif

	if (theTable->m_DrawIt)
	{
		TableUpdateScrollBars(theTable);

		// for now, cause everything to update
		{
			RgnHandle	theRgn;
			
			theRgn = NewRgn();
			ASSERTCOND(theRgn != nil);
			RectRgn(theRgn, &theTable->m_View);
			
			TableUpdate(theTable, theRgn);
			
			DisposeRgn(theRgn);
		}
	}

	return (count != 0);
}

void TableDeleteRow(TablePtr theTable, short count, short rowNum)
{
	ASSERTCOND(rowNum >= theTable->m_DataBounds.top && rowNum < theTable->m_DataBounds.bottom);
	ASSERTCOND(count > 0);
	
	BlockMove((char *)*theTable->m_Cells + TableCalcCellOffset(theTable, theTable->m_DataBounds.left, (short)(rowNum + count)),
				(char *)*theTable->m_Cells + TableCalcCellOffset(theTable, theTable->m_DataBounds.left, rowNum),
				sizeof(Ptr) * (Size)(theTable->m_DataBounds.right - theTable->m_DataBounds.left) * (Size)(theTable->m_DataBounds.bottom - (rowNum + count)));
	
	BlockMove(&theTable->m_RowHeight[rowNum - theTable->m_DataBounds.top + count], &theTable->m_RowHeight[rowNum - theTable->m_DataBounds.top], sizeof(short) * (theTable->m_DataBounds.bottom - (rowNum+count)));

	// if there is a selection then adjust if
	if (!EmptyRgn(theTable->m_SelectionRgn))
	{
		Rect		rDelete;
		Rect		r;
		RgnHandle	newSelectionRgn;
		RgnHandle	theRgn;
		
		rDelete = theTable->m_DataBounds;
		rDelete.top = rowNum;
		rDelete.bottom = rDelete.top + count;
		
		newSelectionRgn = NewRgn();
		ASSERTCOND(newSelectionRgn != nil);
		
		theRgn = NewRgn();
		ASSERTCOND(theRgn != nil);

		// get region above delete
		r = rDelete;
		r.bottom = r.top;
		r.top = theTable->m_DataBounds.top;
		
		RectRgn(theRgn, &r);
		
		SectRgn(theTable->m_SelectionRgn, theRgn, newSelectionRgn);
		
		// get region below delete
		r = rDelete;
		r.top = r.bottom;
		r.bottom = theTable->m_DataBounds.bottom;
		
		RectRgn(theRgn, &r);
		
		SectRgn(theTable->m_SelectionRgn, theRgn, theRgn);
		
		// if not empty shift it up
		if (!EmptyRgn(theRgn))
		{
			OffsetRgn(theRgn, 0, (short)-count);
			UnionRgn(newSelectionRgn, theRgn, newSelectionRgn);
		}
		
		DisposeRgn(theRgn);

		CopyRgn(newSelectionRgn, theTable->m_SelectionRgn);
		DisposeRgn(newSelectionRgn);

		// need more logic for calculating selection point
		SetPt(&theTable->m_SelectionAnchor, 0, 0);
	}
	
	theTable->m_DataBounds.bottom -= count;
	theTable->m_NeedsRecalculation = true;
	theTable->m_NeedsScrollBarUpdate = true;
	
	if (theTable->m_DrawIt)
	{
		TableUpdateScrollBars(theTable);
		
		// for now, cause everything to update
		{
			RgnHandle	theRgn;
			
			theRgn = NewRgn();
			ASSERTCOND(theRgn != nil);
			RectRgn(theRgn, &theTable->m_View);
			
			TableUpdate(theTable, theRgn);
			
			DisposeRgn(theRgn);
		}
	}
}

void TableGetView(TablePtr theTable, Rect* r)
{
	if (theTable->m_HaveTempView)
		*r = theTable->m_TempView;
	else
		*r = theTable->m_View;
}

short TableGetViewWidth(TablePtr theTable)
{
	if (theTable->m_HaveTempView)
		return theTable->m_TempView.right - theTable->m_TempView.left;
	else
		return theTable->m_View.right - theTable->m_View.left;
}

short TableGetViewHeight(TablePtr theTable)
{
	if (theTable->m_HaveTempView)
		return theTable->m_TempView.bottom - theTable->m_TempView.top;
	else
		return theTable->m_View.bottom - theTable->m_View.top;
}

void TableSetRowHeight(TablePtr theTable, short rowNum, short height)
{
	ASSERTCOND(rowNum >= theTable->m_DataBounds.top && rowNum < theTable->m_DataBounds.bottom);

	theTable->m_RowHeight[rowNum - theTable->m_DataBounds.top] = height;

	theTable->m_NeedsRecalculation = true;
	theTable->m_NeedsScrollBarUpdate = true;

	if (theTable->m_DrawIt)
	{
		TableUpdateScrollBars(theTable);
		TableUpdate(theTable, theTable->m_Port->visRgn);
	}
}

void TableSetColumnWidth(TablePtr theTable, short columnNum, short width)
{
	ASSERTCOND(columnNum >= theTable->m_DataBounds.left && columnNum < theTable->m_DataBounds.right);

	theTable->m_ColumnWidth[columnNum - theTable->m_DataBounds.left] = width;

	theTable->m_NeedsRecalculation = true;
	theTable->m_NeedsScrollBarUpdate = true;

	if (theTable->m_DrawIt)
	{
		TableUpdateScrollBars(theTable);
		TableUpdate(theTable, theTable->m_Port->visRgn);
	}
}

short TableGetRowHeight(TablePtr theTable, short rowNum)
{
	ASSERTCOND(rowNum >= theTable->m_DataBounds.top && rowNum < theTable->m_DataBounds.bottom);

	return theTable->m_RowHeight[rowNum - theTable->m_DataBounds.top];
}

short TableGetColumnWidth(TablePtr theTable, short columnNum)
{
	ASSERTCOND(columnNum >= theTable->m_DataBounds.left && columnNum < theTable->m_DataBounds.right);

	return theTable->m_ColumnWidth[columnNum - theTable->m_DataBounds.left];
}

static long TableCalcCellOffset(TablePtr theTable, short h, short v)
{
	return (((v - theTable->m_DataBounds.top) * (theTable->m_DataBounds.right - theTable->m_DataBounds.left))
					+ (h - theTable->m_DataBounds.left))
					* sizeof(Ptr);
	
#ifdef __NEVER__
	return (((h - theTable->m_DataBounds.left) * (theTable->m_DataBounds.bottom - theTable->m_DataBounds.top))
					+ (v - theTable->m_DataBounds.top))
					* sizeof(Ptr);
#endif
}

void TableRecalculate(TablePtr theTable)
{
	short		offset;
	short		i;
	short		*pLength;
	short		*pStart;
	short		numColumns;
	short		numRows;

// REVIEW: asserts when updating linking to ranges
//	ASSERTCOND(theTable->m_DrawIt == TRUE);

	if (!theTable->m_NeedsRecalculation)
		return;

	numColumns = theTable->m_DataBounds.right - theTable->m_DataBounds.left;
	numRows = theTable->m_DataBounds.bottom - theTable->m_DataBounds.top;

	// first process column data
	if (theTable->m_ScrollH != nil)
		offset = -GetCtlValue(theTable->m_ScrollH);
	else
		offset = 0;

	theTable->m_TotalWidth = 0;

	for(i = numColumns, pLength = theTable->m_ColumnWidth, pStart = theTable->m_ColumnStart; i>0; i--)
	{
		short		len;

		*pStart++ = offset;
		len = *pLength++;
		offset += len;
		theTable->m_TotalWidth += len;
	}

	// calculate visible left & right
	theTable->m_Visible.left = theTable->m_DataBounds.left;
	for(i = numColumns, pLength = theTable->m_ColumnWidth, pStart = theTable->m_ColumnStart; i>0; i--)
	{
		if ((*pStart++) + (*pLength++) > 0)
			break;

		theTable->m_Visible.left++;
	}

	{
		short		viewWidth;

		viewWidth = theTable->m_View.right - theTable->m_View.left;

		theTable->m_Visible.right = theTable->m_DataBounds.right;
		for(i = numColumns, pStart = &theTable->m_ColumnStart[i - 1]; i>0; i--)
		{
			if (*pStart-- < viewWidth)
				break;

			theTable->m_Visible.right--;
		}
	}

	// now process row data
	if (theTable->m_ScrollV != nil)
		offset = -GetCtlValue(theTable->m_ScrollV);
	else
		offset = 0;

	theTable->m_TotalHeight = 0;

	for(i = numRows, pLength = theTable->m_RowHeight, pStart = theTable->m_RowStart; i>0; i--)
	{
		short		len;

		*pStart++ = offset;
		len = *pLength++;
		offset += len;
		theTable->m_TotalHeight += len;
	}

	// calculate visible top & bottom
	theTable->m_Visible.top = theTable->m_DataBounds.top;
	for(i = numRows, pLength = theTable->m_RowHeight, pStart = theTable->m_RowStart; i>0; i--)
	{
		if ((*pStart++) + (*pLength++) > 0)
			break;

		theTable->m_Visible.top++;
	}

	{
		short	viewHeight;

		viewHeight = theTable->m_View.bottom - theTable->m_View.top;

		theTable->m_Visible.bottom = theTable->m_DataBounds.bottom;
		for(i = numRows, pStart = &theTable->m_RowStart[i-1]; i>0; i--)
		{
			if (*pStart-- < viewHeight)
				break;

			theTable->m_Visible.bottom--;
		}
	}

	theTable->m_NeedsRecalculation = false;
}

void TableUpdateScrollBars(TablePtr theTable)
{
	short		delta;
	GrafPtr		oldPort;
	Boolean		shiftCells;
	
	if (!theTable->m_DrawIt)
		return;

	if (!theTable->m_NeedsScrollBarUpdate)
		return;

	TableRecalculate(theTable);

	GetPort(&oldPort);
	SetPort(theTable->m_Port);
	
	shiftCells = false;

	if (theTable->m_ScrollH != nil)
	{
		delta = theTable->m_TotalWidth - (theTable->m_View.right - theTable->m_View.left);
		if (delta < 0)
		{
			delta = 0;
			shiftCells = true;
		}

		if (GetCtlMax(theTable->m_ScrollH) != delta)
		{
			SetCtlMax(theTable->m_ScrollH, delta);
			InvalRect(&(*theTable->m_ScrollH)->contrlRect);
		}
	}

	if (theTable->m_ScrollV != nil)
	{
		delta = theTable->m_TotalHeight - (theTable->m_View.bottom - theTable->m_View.top);
		if (delta < 0)
		{
			delta = 0;
			shiftCells = true;
		}

		if (GetCtlMax(theTable->m_ScrollV) != delta)
		{
			SetCtlMax(theTable->m_ScrollV, delta);
			InvalRect(&(*theTable->m_ScrollV)->contrlRect);
		}
	}
	
	SetPort(oldPort);

	theTable->m_NeedsScrollBarUpdate = false;
}

Boolean TableGetCellRect(TablePtr theTable, Cell theCell, Rect* r)
{
	short	index;

	TableRecalculate(theTable);

#if 0
	// check to see if cell is visible
	if (theCell.h < theTable->m_Visible.left || theCell.h >= theTable->m_Visible.right
			|| theCell.v < theTable->m_Visible.top || theCell.v >= theTable->m_Visible.bottom)
		return false;
#endif

	index = theCell.h - theTable->m_DataBounds.left;
	r->left = theTable->m_View.left + theTable->m_ColumnStart[index];
	r->right = r->left + theTable->m_ColumnWidth[index];

	index = theCell.v - theTable->m_DataBounds.top;
	r->top = theTable->m_View.top + theTable->m_RowStart[index];
	r->bottom = r->top + theTable->m_RowHeight[index];

	return true;
}

Boolean TableGetCellAt(TablePtr theTable, Point localPt, Cell* theCell)
{
	short		i;
	short		j;
	short		index;
	short*		pRowStart;
	short*		pColumnStart;
	short*		pRowHeight;
	short*		pColWidth;
	Rect		cellRect;

	TableRecalculate(theTable);

	index = theTable->m_Visible.left - theTable->m_DataBounds.left;
	ASSERTCOND(index >= 0);
	pColumnStart = &theTable->m_ColumnStart[index];
	pColWidth = &theTable->m_ColumnWidth[index];

	cellRect = theTable->m_View;

	for(i = theTable->m_Visible.left; i < theTable->m_Visible.right; i++, pColumnStart++, pColWidth++)
	{
		index = theTable->m_Visible.top - theTable->m_DataBounds.top;
		ASSERTCOND(index >= 0);
		pRowStart = &theTable->m_RowStart[index];
		pRowHeight = &theTable->m_RowHeight[index];

		cellRect.left = *pColumnStart + theTable->m_View.left;
		cellRect.right = cellRect.left + *pColWidth;

		if (PtInRect(localPt, &cellRect))
		{
			for(j = theTable->m_Visible.top; j < theTable->m_Visible.bottom; j++, pRowStart++, pRowHeight++)
			{
				cellRect.top = *pRowStart + theTable->m_View.top;
				cellRect.bottom = cellRect.top + *pRowHeight;

				if (PtInRect(localPt, &cellRect))
				{
					theCell->h = i;
					theCell->v = j;

					return true;
				}
			}
		}
	}

	return false;
}

void TableGetExtent(TablePtr theTable, Rect* rBounds, short* width, short* height)
{
	short totalWidth = 0;
	short totalHeight = 0;
	short row;
	short column;
	
	TableRecalculate(theTable);

	if  (rBounds) {	
		for (row = rBounds->top; row < rBounds->bottom; row++)
			totalHeight += theTable->m_RowHeight[row];
		*height = totalHeight;
		
		for (column = rBounds->left; column < rBounds->right; column++)
			totalWidth += theTable->m_ColumnWidth[column];
		*width = totalWidth;
	}
	else {
		*width = theTable->m_TotalWidth;
		*height = theTable->m_TotalHeight;
	}
}

void TableInvalCell(TablePtr theTable, Cell theCell)
{
	Rect		r;
	GrafPtr		oldPort;
	
	if (!TableGetCellRect(theTable, theCell, &r))
		return;
			
	GetPort(&oldPort);
	SetPort(theTable->m_Port);

	InvalRect(&r);

	SetPort(oldPort);
}

void TableValidCell(TablePtr theTable, Cell theCell)
{
	Rect		r;
	GrafPtr		oldPort;
	
	if (!TableGetCellRect(theTable, theCell, &r))
		return;
		
	GetPort(&oldPort);
	SetPort(theTable->m_Port);
	
	ValidRect(&r);
	
	SetPort(oldPort);
}

Boolean TableCellIsVisible(TablePtr theTable, Cell theCell)
{
	ASSERTCOND(theTable->m_NeedsRecalculation == false);
	
	return PtInRect(theCell, &theTable->m_Visible);
}

void TableScrollToCell(TablePtr theTable, Cell theCell)
{
	Rect	rCell;
	Rect	r;
	Rect	rVisible;
	short	deltaClms;
	short 	deltaRows;
	long	pin;
		
		// get cells rect
		if (TableGetCellRect(theTable, theCell, &rCell))
		{
			// find out if cell is completly visible
			if (SectRect(&rCell, &theTable->m_View, &r))
			{
				// yes it fits
				if (EqualRect(&rCell, &r))
					return;
			}			
	
		rVisible = theTable->m_Visible;
		
		// InsetRect to exclude partially visible items at the rect boundary
		InsetRect(&rVisible, 1, 1);
		pin = PinRect(&rVisible, theCell);		
		deltaClms = theCell.h - LoWord(pin);
		deltaRows = theCell.v - HiWord(pin);

		TableScroll(theTable, deltaClms, deltaRows);
	}
}

void TableScroll(TablePtr theTable, short deltaClms, short deltaRows)
{
	short		deltaH;
	short		deltaV;
	Cell		theCell;
	short		ctlValue;
	GrafPtr		oldPort;

	ASSERTCOND(theTable != nil);
	ASSERTCOND(theTable->m_NeedsScrollBarUpdate == false);

	deltaV = 0;
	if (deltaRows > 0) {
		theCell.v = theTable->m_Visible.bottom - 1;
		for (; deltaRows && (theCell.v < theTable->m_DataBounds.bottom); deltaRows--, theCell.v++)
			deltaV += theTable->m_RowHeight[theCell.v - theTable->m_DataBounds.top - 1];
	}
	else if (deltaRows < 0) {
		theCell.v = theTable->m_Visible.top;
		for (; deltaRows && (theCell.v >= theTable->m_DataBounds.top); deltaRows++, theCell.v--)
			deltaV -= theTable->m_RowHeight[theCell.v - theTable->m_DataBounds.top];
	}
		
	deltaH = 0;
	if (deltaClms > 0) {
		theCell.h = theTable->m_Visible.right - 1;
		for (; deltaClms && (theCell.h < theTable->m_DataBounds.right); deltaClms--, theCell.h++)
			deltaH += theTable->m_ColumnWidth[theCell.h - theTable->m_DataBounds.left - 1];
	}
	else if (deltaClms < 0) {
		theCell.h = theTable->m_Visible.left;
		for (; deltaClms && (theCell.h >= theTable->m_DataBounds.left); deltaClms++, theCell.h--)
			deltaH -= theTable->m_ColumnWidth[theCell.h - theTable->m_DataBounds.left];
	}
		

	ctlValue = GetCtlValue(theTable->m_ScrollV);
	SetCtlValue(theTable->m_ScrollV, (short)(ctlValue + deltaV));
	deltaV = ctlValue - GetCtlValue(theTable->m_ScrollV);

	ctlValue = GetCtlValue(theTable->m_ScrollH);
	SetCtlValue(theTable->m_ScrollH, (short)(ctlValue + deltaH));
	deltaH = ctlValue - GetCtlValue(theTable->m_ScrollH);

	if (deltaV || deltaH)
	{
		RgnHandle	updateRgn;

		updateRgn = NewRgn();
		ASSERTCOND(updateRgn != nil);

		GetPort(&oldPort);
		SetPort(theTable->m_Port);
		ScrollRect(&theTable->m_View, deltaH, deltaV, updateRgn);
		theTable->m_NeedsRecalculation = true;
		TableUpdate(theTable, updateRgn);
		SetPort(oldPort);
		
		DisposeRgn(updateRgn);
	}

	if (theTable->m_ClickLoopProcPtr != nil)
		(*theTable->m_ClickLoopProcPtr)(theTable);
}
