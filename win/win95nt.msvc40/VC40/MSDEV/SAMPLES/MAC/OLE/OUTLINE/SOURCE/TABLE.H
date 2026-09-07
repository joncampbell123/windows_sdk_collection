#ifndef __TABLE_H__
#define __TABLE_H__

#define kMaxColumns		100
#define	kMaxRows		100


#include <Types.h>
#include <QuickDraw.h>
#include "Lists.h"
#include "Controls.h"

struct TableRec;

typedef void (*TableDrawCellProcPtr)(struct TableRec* theTable, Rect* rCell, void* dataPtr);
typedef void (*TableDrawCellSelectedProcPtr)(struct TableRec* theTable, Boolean selected, Rect* rCell, void* dataPtr);
typedef void (*TableDrawCellFramedProcPtr)(struct TableRec* theTable, Rect* rCell, void* dataPtr);
typedef Boolean (*TableClickLoopProcPtr)(struct TableRec* theTable);

struct TableRec {
	Rect							m_View;						// view in port
	GrafPtr							m_Port;

	Cell							m_CellSize;

	short							m_TotalWidth;				// total width of list
	short							m_TotalHeight;				// total height of list

	Boolean							m_HaveTempView;
	Rect							m_TempView;

	short							m_ColumnWidth[kMaxColumns];
	short							m_ColumnStart[kMaxColumns];
	short							m_RowHeight[kMaxRows];
	short							m_RowStart[kMaxRows];

	ControlHandle					m_ScrollV;
	ControlHandle					m_ScrollH;

	Boolean							m_NeedsRecalculation;		// set when recalculation is needed
	Boolean							m_NeedsScrollBarUpdate;		// set when scrollers need re calc
	char							m_SelectionFlags;
	Boolean							m_Active;
	Boolean							m_DrawIt;
	Boolean							m_HasGrow;
	char							m_ListFlags;
	
	Cell							m_SelectionAnchor;
	RgnHandle						m_SelectionRgn;

	TableClickLoopProcPtr			m_ClickLoopProcPtr;

	long							m_LastClickTime;
	Cell							m_LastClickCell;
	Point							m_LastClickLoc;

	void*							m_RefCon;

	Rect							m_Visible;					// visible cells
	Rect							m_DataBounds;				// data bounds of cells

	RgnHandle						m_NeedsUpdateRgn;			// delay drawing region

	Handle							m_Cells;

	TableDrawCellProcPtr			m_DrawCellProcPtr;
	TableDrawCellSelectedProcPtr 	m_DrawCellSelectedProcPtr;
	TableDrawCellFramedProcPtr		m_DrawCellFramedProcPtr;
};

typedef struct TableRec TableRec, *TablePtr;

TablePtr TableNew(Rect* rView, Rect* rDataBounds, Point cellSize, TableDrawCellProcPtr drawProcPtr, WindowPtr theWindow, Boolean drawIt, Boolean hasGrow, Boolean scrollHoriz, Boolean scrollVert);
void TableDispose(TablePtr theTable);
void TableSetDrawCellSelectedProc(TablePtr theTable, TableDrawCellSelectedProcPtr proc);
void TableSetDrawCellFramedProc(TablePtr theTable, TableDrawCellFramedProcPtr proc);
void TableSetRefCon(TablePtr theTable, void* refCon);
void* TableGetRefCon(TablePtr theTable);

Boolean TableAddRow(TablePtr theTable, short count, short *rowNum);
void TableDeleteRow(TablePtr theTable, short count, short rowNum);
short TableAddColumn(TablePtr theTable, short count, short clmNum);
void TableDeleteColumn(TablePtr theTable, short count, short rowNum);

void TableGetView(TablePtr theTable, Rect* r);
short TableGetViewWidth(TablePtr theTable);
short TableGetViewHeight(TablePtr theTable);

void TableSetRowHeight(TablePtr theTable, short rowNum, short height);
void TableSetColumnWidth(TablePtr theTable, short columnNum, short width);
short TableGetRowHeight(TablePtr theTable, short rowNum);
short TableGetColumnWidth(TablePtr theTable, short columnNum);

void TableUpdate(TablePtr theTable, RgnHandle theRgn);
void TableDoDraw(TablePtr theTable, Boolean drawIt);
void TableDraw(TablePtr theTable, Cell theCell, Boolean fClip);
void TableTempSize(TablePtr theTable, short tableWidth, short tableHeight);
void TableSize(TablePtr theTable, short tableWidth, short tableHeight);

void TableActivate(TablePtr theTable, Boolean activateIt);
Boolean TableClick(TablePtr theTable, Point localPt, short modifiers);
Cell TableLastClick(TablePtr theTable);

void TableSetCell(TablePtr theTable, void* dataPtr, Cell theCell);
void TableGetCell(TablePtr theTable, void* dataPtr, Cell theCell);

Boolean TableGetSelect(TablePtr theTable, Boolean advanceIt, Cell* theCell);
void TableSetSelect(TablePtr theTable, Cell theCell, Boolean fSetIt);
Boolean TableNextCell(TablePtr theTable, Boolean hNext, Boolean vNext, Cell* theCell);

void TableClearSelection(TablePtr theTable);
Boolean TableIsSelection(TablePtr theTable);
Boolean TableIsEmpty(TablePtr theTable);

void TableScrollToCell(TablePtr theTable, Cell theCell);
void TableScroll(TablePtr theTable, short deltaClms, short deltaRows);

void TableGetExtent(TablePtr theTable, Rect* rBounds, short* width, short* height);

void TableInvalCell(TablePtr theTable, Cell theCell);
void TableValidCell(TablePtr theTable, Cell theCell);
Boolean TableGetCellRect(TablePtr theTable, Cell theCell, Rect* r);

Boolean TableCellIsVisible(TablePtr theTable, Cell theCell);

void TableDrawIntoPort(TablePtr theTable, GrafPtr thePort, Rect* rBounds);

#endif
