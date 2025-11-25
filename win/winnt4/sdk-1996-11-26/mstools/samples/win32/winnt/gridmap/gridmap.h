//==========================================================================;
//
//  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
//  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
//  PURPOSE.
//
//  Copyright (C) 1994-1996  Microsoft Corporation.  All Rights Reserved.
//
//--------------------------------------------------------------------------;
//
//  gridmap.h
//
//  Description:
//
//
//==========================================================================;

#include "wincon.h"

#define GM_EVENT_TYPE(_ir)         _ir.EventType
#define GM_KEY_EVENT(_ir)          _ir.Event.KeyEvent
#define GM_EVENT_BUFFER_SIZE(_ir)  _ir.Event.WindowBufferSizeEvent
#define GM_EVENT_KEYDOWN(_ir)      GM_KEY_EVENT(_ir).bKeyDown
#define GM_EVENT_VKEYCODE(_ir)     GM_KEY_EVENT(_ir).wVirtualKeyCode
#define GM_EVENT_ASCIICODE(_ir)    GM_KEY_EVENT(_ir).uChar.AsciiChar

#define IsDBCSCode(_wCode) \
        ((HIBYTE(_wCode) >= 0x81 && HIBYTE(_wCode) <= 0x84 || \
          HIBYTE(_wCode) >= 0x87 && HIBYTE(_wCode) <= 0x9f || \
          HIBYTE(_wCode) >= 0xe0 && HIBYTE(_wCode) <= 0xfc) && \
          LOBYTE(_wCode) >= 0x40)

#define IsSBCSCode(_wCode) \
        (!(HIBYTE(_wCode) && LOBYTE(_wCode) >= 0x0000 && \
           LOBYTE(_wCode) <= 0x00ff))

#define PREV_LINE            -1
#define NEXT_LINE             1
#define PREV_SCREEN          -2
#define NEXT_SCREEN           2
#define BEGINING_OF_BUFFER   -9
#define END_OF_BUFFER         9

#define SBCS_START            0x00
#define SBCS_END              0xf0
#define DBCS_START            0x8140
#define DBCS_END              0xfc40
#define DBCS_TOP_ENDPAGE      0xfb60
#define DBCS_BTM_1STPAGE      0x81e0
#define SBCS_TOP_ENDPAGE      0x50
#define SBCS_BTM_1STPAGE      0xa0
#define NOTASSIGNED           0x30fb
#define DBC_CNTR_BLACK        0x8145

#define CONBUFF_WIDTH         80
#define CONBUFF_HEIGHT        25
#define GRID_WIDTH            68
#define GRID_HEIGHT           23
#define CHART_START           2
#define CHART_END             23
#define MARGINX               6
#define MARGINY               0

#define CODE_LIMITTEXT        4
#define CHAR_LIMITTEXT        2
#define BLANK4                TEXT("    ")

#define MARKSTATE_ATTRB_SIZE  4

typedef enum { _SBC_, _DBC_ } CHARSET;

typedef struct tagCodeRange {
    WORD Start;
    WORD End;
} CODERANGE, *PCODERANGE;

typedef struct tagMarkState {
    BOOL fState;
//    WORD wAttrb[MARKSTATE_ATTRB_SIZE];
    DWORD dwCells;
    COORD cWrite;
} MARKSTATE, *PMARKSTATE;

// Dialog ID
#define ABOUTBOX                    100
#define SEARCHBOX                   200
#define IDD_SEARCH_PRMPT            201
#define IDD_EDIT                    202
        
// String ID
#define IDS_CONSOLE_TITLE           300
#define IDS_SHIFTJIS_PRMPT          301
#define IDS_UNICODE_PRMPT           302
#define IDS_CHAR_PRMPT              303
#define IDS_INVALID_CODE            304
#define IDS_INVALID_CHAR            305
#define IDS_CANT_RUN_WIN31          306
#define IDS_BUFFER_SIZE_EVENT       307
#define IDS_HELP_TEXT1              308
#define IDS_HELP_TEXT2              309
#define IDS_HELP_TEXT3              310
#define IDS_HELP_TEXT4              311
#define IDS_HELP_TEXT5              312
#define IDS_HELP_TEXT6              313
#define IDS_HELP_TEXT7              314
#define IDS_HELP_TEXT8              315
#define IDS_HELP_TEXT9              316
#define IDS_HELP_TEXT10             317
#define IDS_HELP_TEXT11             318
#define IDS_HELP_TEXT12             319
#define IDS_HELP_TEXTEND            320
#define IDS_HIT_ANY_KEY             330
#define IDS_STATUS_LINE_TEXT        331

extern CHARSET CurrentCharSet(BOOL fAction);
extern BOOL DisplayGrid(HANDLE hConsole, SHORT nStartLine, SHORT nEndLine);
extern BOOL DisplayCodeMap(HANDLE hConsole, SHORT nStartLine, SHORT nEndLine);
extern BOOL ClearScreenBuffer(HANDLE hConsole);

