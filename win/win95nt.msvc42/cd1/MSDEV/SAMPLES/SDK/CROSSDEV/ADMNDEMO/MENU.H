//*---------------------------------------------------------------------------------
//|  ODBC System Administrator
//|
//|  This code is furnished on an as-is basis as part of the ODBC SDK and is
//|  intended for example purposes only.
//|
//|	Title:	MENU.H
//|		This file contains our menu ids.
//*---------------------------------------------------------------------------------
#ifndef menu_DEFS
#define menu_DEFS

//
// Following are indexes into the menu table
//
enum {
	MENU_SQLTABLES,
	MENU_SQLCOLUMNS,
	MENU_SQLSTATISTICS,
	MENU_SQLPRIMARYKEYS,
	MENU_SQLFOREIGNKEYS,
	MENU_SQLTABLEPRIVILEGES,
	MENU_SQLCOLUMNPRIVILEGES,
	MENU_SQLSPECIALCOLUMNS,
	MENU_SQLPROCEDURES,
	MENU_SQLPROCEDURECOLUMNS,
	MENU_SQLTRANSACT
	};

#define NUM_MENU_FLAGS		MENU_SQLTRANSACT

#define MNU_HAVE_CONNECTS ((lpActiveConn) ? MF_ENABLED : MF_GRAYED)

// Normally menu numbers start at 0 and go up, which is how each offset is shown
//		in this file.  If a child MDI window is zoomed, however, then the child
//		system menu becomes menu number 0 and others are offset.  The following
//		macro will look at the current active window and return the correct
//		0 based menu number.
#define GETMENUNUM(iMenu) (iMenu - ((hwndCurMDIChild) ? (int)IsZoomed(hwndCurMDIChild) : 0))


#define CBITS (sizeof(UDWORD) * 8)
#define BitSet(lpa, pos)	\
	(lpa[((pos) / CBITS)] |= 1 << ((pos) - (CBITS * ((pos) / CBITS))))
#define BitClear(lpa, pos)	\
	(lpa[((pos) / CBITS)] &= ~(1 << ((pos) - (CBITS * ((pos) / CBITS)))))
#define BitGet(lpa, pos)	\
	(lpa[((pos) / CBITS)] & (1 << ((pos) - (CBITS * ((pos) / CBITS)))))
#define BitFlip(lpa,pos)	\
	(BitGet(lpa,pos)) ? BitClear(lpa,pos) : BitSet(lpa,pos)
#define MINREQUIREDINTS(items) (UDWORD)((items / CBITS) + ((items % CBITS) ? 1 : 0))

//*---------------------------------------------------------------------------------
//|	Function prototypes
//*---------------------------------------------------------------------------------
VOID WINAPI ResetMenu(HMENU hMenu, int iMenu);

#endif
