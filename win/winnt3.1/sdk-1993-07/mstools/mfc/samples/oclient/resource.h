// This is a part of the Microsoft Foundation Classes C++ library.
// Copyright (C) 1992 Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Microsoft Foundation Classes Reference and Microsoft
// QuickHelp documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft Foundation Classes product.

/* Application resource ID */
#define ID_APPLICATION  1

// File menu
#define IMENU_FILE          0
#define IDM_NEW             0x100
#define IDM_OPEN            0x101
#define IDM_SAVE            0x102
#define IDM_SAVEAS          0x103
#define IDM_EXIT            0x104
#define IDM_ABOUT           0x105

// Edit menu
#define IMENU_EDIT          1
#define IDM_CUT             0x200
#define IDM_COPY            0x201
#define IDM_PASTE           0x202
#define IDM_PASTELINK       0x203
#define IDM_CLEAR           0x204
#define IDM_CLEARALL        0x205
#define IDM_INSERT_OBJECT   0x206
#define IDM_LINKS           0x207

// Object popup menu (tacked onto the end of IMENU_EDIT)
#define IDM_OBJECT_VERB_MIN 0x300
#define IDM_OBJECT_VERB_MAX 0x3ff
	/* leave 0x300 -> 0x3ff for object verbs */

// Dialogs provided by the app
#define IDDT_ABOUT          1

// String Tables
#define IDS_APPNAME         0x100
#define IDS_UNTITLED        0x101

#define IDS_FILTER          0x102
#define IDS_EXTENSION       0x103
#define IDS_ALLFILTER       0x104

// dialog titles
#define IDS_OPENFILE        0x105
#define IDS_SAVEFILE        0x106

// Prompts
#define IDS_MAYBESAVE       0x180
#define IDS_SAVEINCOMPLETE  0x181

// Error Messages
#define E_FAILED_TO_OPEN_FILE   0x200
#define E_FAILED_TO_READ_FILE   0x201
#define E_FAILED_TO_SAVE_FILE   0x202
#define E_INVALID_FILENAME      0x203
#define E_CLIPBOARD_CUT_FAILED  0x204
#define E_CLIPBOARD_COPY_FAILED 0x205
#define E_GET_FROM_CLIPBOARD_FAILED 0x206
#define E_BUSY                  0x207
#define E_FAILED_TO_CREATE      0x208
#define E_FAILED_TO_LAUNCH      0x209

// Warning messages
#define W_FAILED_TO_NOTIFY  0x303


