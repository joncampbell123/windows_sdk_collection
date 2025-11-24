/* resource.h : Defines the resources ID's for the MultiPad application.
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
*/

// Edit control identifier
#define ID_EDIT 0xCAC

// Menu resource ID's
#define IDMULTIPAD  1
#define IDMULTIPAD2 2

#include "menu.h"

// Index to the "Window" menu in each menubar
#define WINDOWMENU  3
#define SHORTMENU   2

// Index to the first and last Edit and Find menu items.
#define IDM_EDITFIRST   IDM_EDITUNDO
#define IDM_EDITLAST    IDM_EDITSETFONT

#define IDD_ABOUT       300

#define IDD_FIND        400
#define IDD_SEARCH      401
#define IDD_PREV        402
#define IDD_NEXT        IDOK
#define IDD_CASE        403

#define IDD_PRINT       600
#define IDD_PRINTDEVICE 601
#define IDD_PRINTPORT   602
#define IDD_PRINTTITLE  603

#define IDD_FONT        700
#define IDD_FACES       701
#define IDD_SIZES       702
#define IDD_BOLD        703
#define IDD_ITALIC      704
#define IDD_FONTTITLE   705

// Strings
#define IDS_CANTOPEN    1
#define IDS_CANTREAD    2
#define IDS_CANTCREATE  3
#define IDS_CANTWRITE   4
#define IDS_ADDEXT      6
#define IDS_CLOSESAVE   7
#define IDS_CANTFIND    8
#define IDS_ALTPMT      9
#define IDS_MENUPMT     10
#define IDS_ACTTHISWIN  11
#define IDS_OPENTHISFILE 12
#define IDS_FILETOOBIG 13

#define IDS_UNTITLED    17

#define IDS_PRINTJOB    24
#define IDS_PRINTERROR  25
