// showfont.h : Defines the interfaces of the major classes of ShowFont.
//              ShowFont is a simple Windows font parameter viewing and
//              modifying utility program.
//
// This is a part of the Microsoft Foundation Classes C++ library. 
// Copyright (C) 1992 Microsoft Corporation 
// All rights reserved. 
//  
// This source code is only intended as a supplement to the 
// Microsoft Foundation Classes Reference and Microsoft 
// QuickHelp documentation provided with the library. 
// See these sources for detailed information regarding the 
// Microsoft Foundation Classes product.

#include <afxwin.h>
#include <afxcoll.h>
#include "resource.h"
#include "mainwnd.h"

/////////////////////////////////////////////////////////////////////////////
// Main application class

class CShowFontApp : public CWinApp
{
public:
	virtual BOOL InitInstance();
};

/////////////////////////////////////////////////////////////////////////////
// Global variables

extern CFont* pTheFont; // current font
extern CFont* myFont;       // created font
extern CFont systemFont;
extern char outputText[4][64];
extern int nLineSpace;

extern CPoint ptCurrent;
extern short nBkMode;
extern DWORD rgbBkColor, rgbTextColor;
extern short nAlignLCR, nAlignTBB;
extern char szAppName[];
extern char szWindowTitle[];

/////////////////////////////////////////////////////////////////////////////
// Other helper routines

// Drawing
void ShowCharacterSet(CDC&, CFont*);

// From the cfont.cpp file.
int DoCreateFontDlg(CWnd* pWnd, LOGFONT& rLogFont);

/////////////////////////////////////////////////////////////////////////////
