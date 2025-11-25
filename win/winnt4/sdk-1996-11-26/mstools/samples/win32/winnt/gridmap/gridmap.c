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
//  gridmap.c
//
//  Description:
//      This is a sample application that demonstrates how to use the 
//      Console API's in Windows.
//
//==========================================================================;

#include <windows.h>
#include <stdio.h>
#include <mbctype.h>
#include <ime.h>
#include "gridmap.h"

static WORD BeginHorValue = DBCS_START;
static WORD EndHorValue;
MARKSTATE MarkState;
HANDLE ghHeap;

void InitMarkState(HANDLE hConsole)
{
    MarkState.fState = FALSE;
    return;
}


void PrintError(DWORD dwErrCode)
{
    LPVOID lpMsgBuf;

    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
        NULL,
        dwErrCode,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
        (LPTSTR) &lpMsgBuf,
        0,
        NULL
    );

    MessageBox(NULL, lpMsgBuf, NULL, MB_OK | MB_ICONSTOP);
    GlobalFree(lpMsgBuf);
    exit(1);
}


LPTSTR LoadRcString(UINT wID)
{
    static TCHAR szBuff[BUFSIZ];

    LoadString(GetModuleHandle(NULL), wID, szBuff,
                    sizeof(szBuff) / sizeof(TCHAR));
    return szBuff;
}


SHORT GetConsoleBufferWidth(HANDLE hConsole)
{
    CONSOLE_SCREEN_BUFFER_INFO csbi;

    if(!GetConsoleScreenBufferInfo(hConsole, &csbi)) {
        PrintError(GetLastError());
    }
    return(csbi.dwSize.X);
}


SHORT GetConsoleBufferHeight(HANDLE hConsole)
{
    CONSOLE_SCREEN_BUFFER_INFO csbi;

    if(!GetConsoleScreenBufferInfo(hConsole, &csbi)) {
        PrintError(GetLastError());
    }
    return(csbi.dwSize.Y);
}


BOOL DisplayStatusLine(HANDLE hConsole)
{
    LPTSTR lpszBuff;
    COORD  coord;
    DWORD  dwWritten;

    coord.X = MARGINX;
    coord.Y = GetConsoleBufferHeight(hConsole) - 1;
    lpszBuff = LoadRcString(IDS_STATUS_LINE_TEXT); 
    return WriteConsoleOutputCharacter(hConsole,
                    lpszBuff,
                    lstrlen(lpszBuff),
                    coord,
                    &dwWritten);
}


void InitConsole(void)
{
    if(!FreeConsole()) {
        PrintError(GetLastError());
    }
    if(!AllocConsole()) {
        PrintError(GetLastError());
    }
    if(!SetConsoleTitle(LoadRcString(IDS_CONSOLE_TITLE))) {
        PrintError(GetLastError());
    }
    ghHeap = GetProcessHeap();
}


HANDLE InitStdIn(void)
{
    HANDLE hStdIn;
    DWORD  dwMode;

    if((hStdIn = GetStdHandle(STD_INPUT_HANDLE)) == INVALID_HANDLE_VALUE) {
        PrintError(GetLastError());
    }
    if(!GetConsoleMode(hStdIn, &dwMode)) {
        PrintError(GetLastError());
    }
    if(!SetConsoleMode(hStdIn,
        (dwMode & ~(ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT)) |
        ENABLE_WINDOW_INPUT | ENABLE_MOUSE_INPUT)) {
        PrintError(GetLastError());
    }
    return hStdIn;
}


HANDLE InitStdOut(void)
{
    HANDLE              hStdOut;
    CONSOLE_CURSOR_INFO ccInfo = {1, FALSE};
    COORD cSize = {CONBUFF_WIDTH, CONBUFF_HEIGHT};
    SMALL_RECT srct = {0, 0, CONBUFF_WIDTH-1, CONBUFF_HEIGHT-1};

    if((hStdOut = GetStdHandle(STD_OUTPUT_HANDLE)) == INVALID_HANDLE_VALUE) {
        PrintError(GetLastError());
    }
    if(!SetConsoleCursorInfo(hStdOut, &ccInfo)) {
        PrintError(GetLastError());
    }
    if(!SetConsoleWindowInfo(hStdOut, TRUE, &srct)) {
        PrintError(GetLastError());
    }
    if(!SetConsoleScreenBufferSize(hStdOut, cSize)) {
        PrintError(GetLastError());
    }
    InitMarkState(hStdOut);
    DisplayStatusLine(hStdOut);
    return hStdOut;
}


BOOL IncrementBeginHorValue(CHARSET CharSet)
{
    if(CharSet == _DBC_) {
        if(BeginHorValue == DBCS_TOP_ENDPAGE)
            return FALSE;
        while(BeginHorValue < DBCS_TOP_ENDPAGE) {
            BeginHorValue += 0x10;
            if(IsDBCSCode(BeginHorValue)) break;
        }
    }
    else {
        if(BeginHorValue < SBCS_TOP_ENDPAGE)
            BeginHorValue += 0x10;
        else
            return FALSE;
    }
    return TRUE;
}


BOOL IncrementEndHorValue(CHARSET CharSet)
{
    if(CharSet == _DBC_) {
        if(EndHorValue == DBCS_END)
            return FALSE;
        while(EndHorValue < DBCS_END) {
            EndHorValue += 0x10;
            if(IsDBCSCode(EndHorValue)) break;
        }
    }
    else {
        if(EndHorValue < SBCS_END)
            EndHorValue += 0x10;
        else
            return FALSE;
    }
    return TRUE;
}


BOOL DecrementBeginHorValue(CHARSET CharSet)
{
    if(CharSet == _DBC_) {
        if(BeginHorValue == DBCS_START)
            return FALSE;
        while(BeginHorValue > DBCS_START) {
            BeginHorValue -= 0x10;
            if(IsDBCSCode(BeginHorValue)) break;
        }
    }
    else {
        if(BeginHorValue > SBCS_START)
            BeginHorValue -= 0x10;
        else
            return FALSE;
    }
    return TRUE;
}


BOOL DecrementEndHorValue(CHARSET CharSet)
{
    if(CharSet == _DBC_) {
        if(EndHorValue == DBCS_BTM_1STPAGE)
            return FALSE;
        while(EndHorValue > DBCS_START) {
            EndHorValue -= 0x10;
            if(IsDBCSCode(EndHorValue)) break;
        }
    }
    else {
        if(EndHorValue > SBCS_BTM_1STPAGE)
            EndHorValue -= 0x10;
        else
            return FALSE;
    }
    return TRUE;
}


void RedisplayCodeMap(HANDLE hConsole)
{
    ClearScreenBuffer(hConsole);
    DisplayCodeMap(hConsole, CHART_START, CHART_END);
}


BOOL ScrollGridMap(HANDLE hConsole, INT nDirect)
{
    SMALL_RECT SourceRect;
    SMALL_RECT ClipRect;
    COORD      coordDestOrigin;
    CHAR_INFO  chiFill;
    WORD       wAttrb;
    COORD      ReadCoord = {0, 0};
    DWORD      cNumRead;
    SHORT      conWidth, conHeight;
    SHORT      nStartLine, nEndLine;
    CHARSET    CharSet;

    CharSet = CurrentCharSet(FALSE);
    conWidth = GetConsoleBufferWidth(hConsole);
    conHeight = GetConsoleBufferHeight(hConsole);
    ReadConsoleOutputAttribute(hConsole,
                    &wAttrb,
                    sizeof(wAttrb) / sizeof(WORD),
                    ReadCoord,
                    &cNumRead);
    SourceRect.Left = 0;
    SourceRect.Right = conWidth;
    ClipRect.Left = 0;
    ClipRect.Right = conWidth;
    switch(nDirect) {
        case NEXT_LINE:
            if(!IncrementBeginHorValue(CharSet))
                return FALSE;
            SourceRect.Top = 4;
            SourceRect.Bottom = conHeight - 2;
            coordDestOrigin.Y = 2;
            nStartLine = CHART_END - 1;
            nEndLine = CHART_END;
            break;
        case PREV_LINE:
            if(!DecrementBeginHorValue(CharSet))
                return FALSE;
            SourceRect.Top = 2;
            SourceRect.Bottom = conHeight - 4;
            coordDestOrigin.Y = 4;
            nStartLine = CHART_START;
            nEndLine = CHART_START + 1;
            break;
        case NEXT_SCREEN:
        {
                INT i;
                for(i = 0; i < 10; i++)
                    if(!IncrementBeginHorValue(CharSet))
                        break;
                if(i == 10) {
                    SourceRect.Top = conHeight - 3;
                    SourceRect.Bottom = conHeight - 2;
                    coordDestOrigin.Y = 2;
                    nStartLine = CHART_START + 2;
                    nEndLine = CHART_END;
                }
                else {
                    BeginHorValue = DBCS_TOP_ENDPAGE;
                    RedisplayCodeMap(hConsole);
                    return FALSE;
                }
                break;
        }
        case PREV_SCREEN:
        {
                INT i;
                if(!DecrementBeginHorValue(CharSet))
                    return FALSE;
                BeginHorValue += 0x10;
                for(i = 0; i < 10; i++)
                    if(!DecrementBeginHorValue(CharSet))
                        break;
                if(i == 10) {
                    SourceRect.Top = CHART_START;
                    SourceRect.Bottom = CHART_START + 1;
                    coordDestOrigin.Y = CHART_END - 1;
                    nStartLine = CHART_START;
                    nEndLine = CHART_END - 2;
                }
                else {
                    BeginHorValue = DBCS_START;
                    RedisplayCodeMap(hConsole);
                    return FALSE;
                }
                break;
        }
        default:
            return FALSE;
    }
    ClipRect.Top = 2;
    ClipRect.Bottom = conHeight - 2;
    coordDestOrigin.X = 0;
    chiFill.Char.AsciiChar = ' ';
    chiFill.Attributes = wAttrb;

    if(!((nDirect == NEXT_SCREEN || nDirect == PREV_SCREEN) && CharSet == _SBC_))
        ScrollConsoleScreenBuffer(hConsole,
                        &SourceRect,
                        &ClipRect,
                        coordDestOrigin, 
                        &chiFill);
    DisplayGrid(hConsole, nStartLine, nEndLine);
    return TRUE;
}


BOOL ClearScreenBuffer(HANDLE hConsole)
{
    COORD coordChar = {0, CHART_START};
    DWORD dwWritten;

    return FillConsoleOutputCharacter(hConsole, 
                           ' ',
                           GetConsoleBufferWidth(hConsole) * 
                           (GetConsoleBufferHeight(hConsole) - CHART_START - 1),
                           coordChar, 
                           &dwWritten);
}


WORD StringToHexWord(LPTSTR lpsz)
{
    TCHAR szBuff[5];
    int   i;
    WORD  wHex;

    wsprintf(szBuff, TEXT("%04s"), lpsz);
    for(i = 0, wHex = 0; i < 4; i++) {
        if(isdigit(szBuff[i]))
            wHex += (szBuff[i] - '0') * (1 << ((3 - i) << 2));
        else if(szBuff[i] >= 'a' && szBuff[i] <= 'f')
            wHex += (szBuff[i] - 'a' + 10) * (1 << ((3 - i) << 2));
        else if(szBuff[i] >= 'A' && szBuff[i] <= 'F')
            wHex += (szBuff[i] - 'A' + 10) * (1 << ((3 - i) << 2));
        else
            break;
    }
    return wHex;
}


LRESULT SetOpenIME(HWND hWnd, WPARAM wParam)
{
    HIMC himc;
    BOOL bResult = FALSE;

    if(himc = ImmGetContext(hWnd)) {
        bResult = ImmSetOpenStatus(himc, wParam);
        ImmReleaseContext(hWnd, himc);
    }
    return bResult;
}


BOOL APIENTRY AboutProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch(msg) {
        case WM_INITDIALOG:
            SetForegroundWindow(hWnd);
            return TRUE;
        case WM_COMMAND:
            switch(LOWORD(wParam)) {
                case IDOK:
                case IDCANCEL:
                    EndDialog(hWnd, TRUE);
                    return TRUE;
            }
    }
    return FALSE;
}


INT AboutGridMap(void)
{
    HANDLE hInst;
    HWND   hWnd;

    hInst = GetModuleHandle(NULL);
    hWnd = GetForegroundWindow();
    return DialogBox(hInst, MAKEINTRESOURCE(ABOUTBOX), hWnd, AboutProc);
}


BOOL APIENTRY ShiftJISDlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    static LPTSTR *lpText;

    switch(msg) {
        case WM_INITDIALOG:
            lpText = (LPTSTR *)lParam;
            SetDlgItemText(hWnd, IDD_SEARCH_PRMPT,
                        LoadRcString(IDS_SHIFTJIS_PRMPT));
            SendMessage(GetDlgItem(hWnd, IDD_EDIT), EM_LIMITTEXT,
                        CODE_LIMITTEXT, 0);
            SetForegroundWindow(hWnd);
            return TRUE;
        case WM_COMMAND:
            switch(LOWORD(wParam)) {
                case IDOK:
                    SendMessage(GetDlgItem(hWnd, IDD_EDIT), WM_GETTEXT,
                                CODE_LIMITTEXT + 1, (LPARAM)*lpText);
                    EndDialog(hWnd, TRUE);
                    return TRUE;
                case IDCANCEL:
                    EndDialog(hWnd, FALSE);
                    return TRUE;
            }
    }
    return FALSE;
}


BOOL APIENTRY UnicodeDlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    static LPTSTR *lpText;

    switch(msg) {
        case WM_INITDIALOG:
            lpText = (LPTSTR *)lParam;
            SetDlgItemText(hWnd, IDD_SEARCH_PRMPT,
                        LoadRcString(IDS_UNICODE_PRMPT));
            SendMessage(GetDlgItem(hWnd, IDD_EDIT), EM_LIMITTEXT,
                        CODE_LIMITTEXT, 0);
            SetForegroundWindow(hWnd);
            return TRUE;
        case WM_COMMAND:
            switch(LOWORD(wParam)) {
                case IDOK:
                    SendMessage(GetDlgItem(hWnd, IDD_EDIT), WM_GETTEXT,
                                CODE_LIMITTEXT + 1, (LPARAM)*lpText);
                    EndDialog(hWnd, TRUE);
                    return TRUE;
                case IDCANCEL:
                    EndDialog(hWnd, FALSE);
                    return TRUE;
            }
    }
    return FALSE;
}


BOOL APIENTRY CharacterDlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    static LPTSTR    *lpText;
    static IMESTRUCT IME;

    switch(msg) {
        case WM_INITDIALOG:
            lpText = (LPTSTR *)lParam;
            SetDlgItemText(hWnd, IDD_SEARCH_PRMPT,
                        LoadRcString(IDS_CHAR_PRMPT));
            SendMessage(GetDlgItem(hWnd, IDD_EDIT), EM_LIMITTEXT,
                        CHAR_LIMITTEXT, 0);
            SetForegroundWindow(hWnd);
            SetOpenIME(hWnd, TRUE);
            return TRUE;
        case WM_COMMAND:
            switch(LOWORD(wParam)) {
                case IDOK:
                    SendMessage(GetDlgItem(hWnd, IDD_EDIT), WM_GETTEXT,
                                CHAR_LIMITTEXT + 1, (LPARAM)*lpText);
                    SetOpenIME(hWnd, FALSE);
                    EndDialog(hWnd, TRUE);
                    break;
                case IDCANCEL:
                    SetOpenIME(hWnd, FALSE);
                    EndDialog(hWnd, FALSE);
                    return TRUE;
            }
    }
    return FALSE;
}


void MovePage(HANDLE hConsole, WORD wCode)
{
    WORD wHigh;

    wHigh = wCode & 0xfff0;
    if(wHigh >= BeginHorValue && wHigh <= EndHorValue)
        return;                              // in current page
    else if (wHigh <= DBCS_BTM_1STPAGE && wHigh >= DBCS_START) {
        if(CurrentCharSet(FALSE) == _SBC_)
            CurrentCharSet(TRUE);
        BeginHorValue = DBCS_START;          // Move to begining of buffer
    }
    else if (wHigh >= DBCS_TOP_ENDPAGE) {
        if(CurrentCharSet(FALSE) == _SBC_)
            CurrentCharSet(TRUE);
        BeginHorValue = DBCS_TOP_ENDPAGE;    // Move to end of buffer
    }
    else {
        CHARSET CharSet = CurrentCharSet(FALSE);
        if(CharSet == _SBC_ && IsDBCSCode(wCode)) {
            CurrentCharSet(TRUE);
        }
        else if(IsSBCSCode(wCode)) {
            if(wHigh >= 0x00b0)
                wHigh = (WORD)SBCS_TOP_ENDPAGE;
            else if (wHigh <= 0x00a0)
                wHigh = (WORD)SBCS_START;
            if(CharSet == _DBC_)
                CurrentCharSet(TRUE);
        }
        BeginHorValue = wHigh;       // Move to specified page
    }
    RedisplayCodeMap(hConsole);
    return;
}


void ReverseMarkState(HANDLE hConsole)
{
    INT   i;
    WORD  wAttrb[MARKSTATE_ATTRB_SIZE];
    DWORD dwWritten;

    if(!MarkState.fState)
        return;
    ReadConsoleOutputAttribute(hConsole,
                    wAttrb,
                    sizeof(wAttrb) / sizeof(WORD),
                    MarkState.cWrite,
                    &dwWritten);
    for(i = 0; i < MARKSTATE_ATTRB_SIZE; i++)
        wAttrb[i] ^= COMMON_LVB_REVERSE_VIDEO;
    WriteConsoleOutputAttribute(hConsole,
                    wAttrb,
                    sizeof(wAttrb) / sizeof(WORD),
                    MarkState.cWrite,
                    &dwWritten);
    MarkState.cWrite.Y++;
    ReadConsoleOutputAttribute(hConsole,
                    wAttrb,
                    sizeof(wAttrb) / sizeof(WORD),
                    MarkState.cWrite,
                    &dwWritten);
    for(i = 0; i < MARKSTATE_ATTRB_SIZE; i++)
        wAttrb[i] ^= COMMON_LVB_REVERSE_VIDEO;
    WriteConsoleOutputAttribute(hConsole,
                    wAttrb,
                    sizeof(wAttrb) / sizeof(WORD),
                    MarkState.cWrite,
                    &dwWritten);
    MarkState.cWrite.Y--;
}


void ClearMarkState(HANDLE hConsole)
{
    ReverseMarkState(hConsole);
    MarkState.fState = FALSE;
}


void SetMarkState(HANDLE hConsole)
{
    MarkState.fState = TRUE;
    ReverseMarkState(hConsole);
}


BOOL MarkCode(HANDLE hConsole, WORD wCode)
{
    WORD    wCodeTmp;
    WORD    wCount;
    BYTE    bCount;
    BYTE    Trail;
    SHORT   nLine, nColumn;
    CHARSET CharSet;

    CharSet = CurrentCharSet(FALSE);
    wCodeTmp = wCode & 0xfff0;
    for(wCount = BeginHorValue, nLine = CHART_START;
            wCodeTmp != wCount && wCount <= EndHorValue; wCount += 0x10) {
        if(CharSet == _DBC_ && !IsDBCSCode(wCount))
            continue;
        nLine += 2;
    }
    for(nColumn = MARGINX + 4, bCount = 0x0, Trail = LOBYTE(wCode) & 0x0f;
            Trail != bCount; nColumn += 4, bCount++) ;
    MarkState.dwCells = MARKSTATE_ATTRB_SIZE;
    MarkState.cWrite.X = nColumn;
    MarkState.cWrite.Y = nLine;
    SetMarkState(hConsole);
    return TRUE;
}


BOOL SearchCodeMap(HANDLE hConsole, WORD wCode)
{
    MovePage(hConsole, wCode);
    MarkCode(hConsole, wCode);
    return TRUE;
}

void ShiftJISSearchCommand(HANDLE hConsole)
{
    HANDLE hInst;
    HWND   hWnd;
    int    retValue;
    LPTSTR szText;
    WORD   wCode;

    hInst = GetModuleHandle(NULL);
    hWnd = GetForegroundWindow();
    if((szText = (LPTSTR)HeapAlloc(ghHeap,
                                   HEAP_ZERO_MEMORY,
                                   (CODE_LIMITTEXT+1)*sizeof(TCHAR))) == NULL) {
        PrintError(GetLastError());
    }
    retValue = DialogBoxParam(hInst, MAKEINTRESOURCE(SEARCHBOX),
                           hWnd, ShiftJISDlgProc, (LPARAM)&szText);
    if(retValue == TRUE) {
        wCode = StringToHexWord(szText);
        if(IsDBCSCode(wCode) || IsSBCSCode(wCode)) {
            SearchCodeMap(hConsole, wCode);
        }
        else {
            MessageBox(hWnd, LoadRcString(IDS_INVALID_CODE), NULL,
                        MB_OK | MB_ICONEXCLAMATION | MB_TASKMODAL);
        }
    }
    HeapFree(ghHeap, 0, szText);
    return;
}


void UnicodeSearchCommand(HANDLE hConsole)
{
    HANDLE hInst;
    HWND   hWnd;
    int    retValue;
    LPTSTR szText;
    WORD   wCode;

    hInst = GetModuleHandle(NULL);
    hWnd = GetForegroundWindow();
    if((szText = (LPTSTR)HeapAlloc(ghHeap,
                                   HEAP_ZERO_MEMORY,
                                   (CODE_LIMITTEXT+1)*sizeof(TCHAR))) == NULL) {
        PrintError(GetLastError());
    }
    retValue = DialogBoxParam(hInst, MAKEINTRESOURCE(SEARCHBOX),
                           hWnd, UnicodeDlgProc, (LPARAM)&szText);
    if(retValue == TRUE) {
        CHAR szChar[3];
        WCHAR szWChar[2];
        wCode = StringToHexWord(szText);
        szWChar[0] = wCode;
        szWChar[1] = TEXT('\0');
        WideCharToMultiByte(CP_ACP, WC_COMPOSITECHECK, szWChar, -1,
                    szChar, sizeof(szChar), NULL, NULL);
        if(!szChar[1])
            wCode = (WORD)(szChar[0] & 0xff);
        else
            wCode = LOWORD((szChar[0] << 8) | (BYTE)szChar[1]);
        if(IsDBCSCode(wCode) || IsSBCSCode(wCode)) {
            SearchCodeMap(hConsole, wCode);
        }
        else {
            MessageBox(hWnd, LoadRcString(IDS_INVALID_CODE), NULL,
                        MB_OK | MB_ICONEXCLAMATION | MB_TASKMODAL);
        }
    }
    HeapFree(ghHeap, 0, szText);
    return;
}


void CharacterSearchCommand(HANDLE hConsole)
{
    HANDLE hInst;
    HWND   hWnd;
    int    retValue;
    LPTSTR szText;
    WORD   wCode;

    hInst = GetModuleHandle(NULL);
    hWnd = GetForegroundWindow();
    if((szText = (LPTSTR)HeapAlloc(ghHeap,
                                   HEAP_ZERO_MEMORY,
                                   (CHAR_LIMITTEXT+1)*sizeof(TCHAR))) == NULL) {
        PrintError(GetLastError());
    }
    retValue = DialogBoxParam(hInst, MAKEINTRESOURCE(SEARCHBOX),
                           hWnd, CharacterDlgProc, (LPARAM)&szText);
    if(retValue == TRUE) {
        if(!*(szText + 1))
            wCode = (WORD)(*szText & 0xff);
        else
            wCode = LOWORD((*szText << 8) | (BYTE)*(szText+1));
        if(IsDBCSCode(wCode) || IsSBCSCode(wCode)) {
            SearchCodeMap(hConsole, wCode);
        }
        else {
            MessageBox(hWnd, LoadRcString(IDS_INVALID_CHAR), NULL,
                        MB_OK | MB_ICONEXCLAMATION | MB_TASKMODAL);
        }
    }
    HeapFree(ghHeap, 0, szText);
    return;
}


void DisplayHelpText(HANDLE hConsole)
{
    int id;
    DWORD dwData;
    TCHAR szBuff[BUFSIZ];

    for(id = IDS_HELP_TEXT1; id < IDS_HELP_TEXTEND; id++) {
        wsprintf(szBuff, TEXT("%s\n"), LoadRcString(id));
        WriteConsole(hConsole, szBuff, lstrlen(szBuff), &dwData, NULL);
    }
    wsprintf(szBuff, TEXT("%s"), LoadRcString(IDS_HIT_ANY_KEY));
    WriteConsole(hConsole, szBuff, lstrlen(szBuff), &dwData, NULL);
}


BOOL DisplayHelpCommand(HANDLE hStdIn, HANDLE hStdOut)
{
    HANDLE hConsoleHlp;
    DWORD  lpdwData;
    INPUT_RECORD inBuffer;

    if((hConsoleHlp = CreateConsoleScreenBuffer(
                            GENERIC_READ | GENERIC_WRITE,
                            0,
                            NULL,
                            CONSOLE_TEXTMODE_BUFFER,
                            NULL)) == INVALID_HANDLE_VALUE) {
        return FALSE; 
    }
    DisplayHelpText(hConsoleHlp);
    if(!SetConsoleActiveScreenBuffer(hConsoleHlp))
        return FALSE;
    FlushConsoleInputBuffer(hConsoleHlp);
    while(ReadConsoleInput(hStdIn, &inBuffer, 1, &lpdwData)) {
        if(GM_EVENT_TYPE(inBuffer) == KEY_EVENT) {
            if(GM_EVENT_KEYDOWN(inBuffer))
                break;
        } 
    }
    if(!SetConsoleActiveScreenBuffer(hStdOut))
        return FALSE;
    CloseHandle(hConsoleHlp);
}


void ProcessKeyCommand(HANDLE hStdIn, HANDLE hStdOut, WORD wVKeyCode)
{
    INT   nOffset = 0;
    SHORT nStartLine, nEndLine;

    ClearMarkState(hStdOut);
    switch(wVKeyCode) {
        case VK_F1:
            DisplayHelpCommand(hStdIn, hStdOut);
            break;
        case VK_F5:    // Toggle CodeMap _SBC_ <-> _DBC_
            CurrentCharSet(TRUE);
            RedisplayCodeMap(hStdOut);
            break;
        case VK_F6:
            ShiftJISSearchCommand(hStdOut);
            break;
        case VK_F7:
            UnicodeSearchCommand(hStdOut);
            break;
        case VK_F8:
            CharacterSearchCommand(hStdOut);
            break;
        case VK_F10:
            AboutGridMap();
            break;
        case VK_DOWN:
            nOffset = NEXT_LINE;
            nStartLine = CHART_END - 1;
            nEndLine = CHART_END - 1;
            break;
        case VK_UP:
            nOffset = PREV_LINE;
            nStartLine = CHART_START;
            nEndLine = CHART_START;
            break;
        case VK_NEXT:
            if(CurrentCharSet(FALSE) == _DBC_) {
                if(EndHorValue != DBCS_END) {
                    nOffset = NEXT_SCREEN;
                    nStartLine = CHART_START + 2;
                    nEndLine = CHART_END - 1;
                }
            }
            else if (BeginHorValue != SBCS_TOP_ENDPAGE) {
                BeginHorValue = SBCS_TOP_ENDPAGE;
                RedisplayCodeMap(hStdOut);
                EndHorValue = SBCS_END;
            }
            break;
        case VK_PRIOR:
            if(CurrentCharSet(FALSE) == _DBC_) {
                nOffset = PREV_SCREEN;
                nStartLine = CHART_START;
                nEndLine = CHART_END - 3;
            }
            else if (BeginHorValue != 0x00 ) {
                BeginHorValue = 0x00;
                RedisplayCodeMap(hStdOut);
                EndHorValue = SBCS_BTM_1STPAGE;
            }
            break;
        case VK_END:
            if(CurrentCharSet(FALSE) == _DBC_) {
                if(EndHorValue != DBCS_END) {
                    BeginHorValue = DBCS_TOP_ENDPAGE;
                    RedisplayCodeMap(hStdOut);
                }
            }
            else if (BeginHorValue != SBCS_TOP_ENDPAGE) {
                BeginHorValue = SBCS_TOP_ENDPAGE;
                RedisplayCodeMap(hStdOut);
                EndHorValue = SBCS_END;
            }
            break;
        case VK_HOME:
            if(CurrentCharSet(FALSE) == _DBC_) {
                if(BeginHorValue != DBCS_START) {
                    BeginHorValue = DBCS_START;
                    RedisplayCodeMap(hStdOut);
                }
            }
            else if (BeginHorValue != 0x00) {
                BeginHorValue = 0x00;
                RedisplayCodeMap(hStdOut);
            }
            break;
        default:
            break;
    }
    if(nOffset) {
        if(ScrollGridMap(hStdOut, nOffset))
            DisplayCodeMap(hStdOut, nStartLine, nEndLine);
    }
}


void ClearRightSideGrid(HANDLE hConsole)
{
    WORD  wSrcAttrb;
    WORD  wAttrb[MARGINX];
    COORD cCoord;
    DWORD dwNumber;
    SHORT nWidth, nHeight;
    int   i;

    nWidth = GetConsoleBufferWidth(hConsole);
    nHeight = GetConsoleBufferHeight(hConsole);
    cCoord.X = 0;
    cCoord.Y = 0;
    ReadConsoleOutputAttribute(hConsole,
                    &wSrcAttrb,
                    1,
                    cCoord,
                    &dwNumber);
    for(i = 0; i < sizeof(wAttrb) / sizeof(WORD); i++)
        wAttrb[i] = wSrcAttrb;
    for(cCoord.X = nWidth - MARGINX; cCoord.Y < nHeight; cCoord.Y ++) {
        WriteConsoleOutputAttribute(hConsole,
                        wAttrb,
                        sizeof(wAttrb) / sizeof(WORD),
                        cCoord,
                        &dwNumber);
    }
    return;
}


BOOL BufferSizeEvent(
    HANDLE hStdIn,
    HANDLE hStdOut,
    PCONSOLE_SCREEN_BUFFER_INFO pcsbi, 
    WINDOW_BUFFER_SIZE_RECORD WinBuffSizeEvent)
{

    if(pcsbi->dwSize.X == WinBuffSizeEvent.dwSize.X &&
       pcsbi->dwSize.Y == WinBuffSizeEvent.dwSize.Y) {
        return TRUE;
    }
    else {
        HANDLE hInst;
        HWND hWnd;

        SetConsoleScreenBufferSize(hStdOut, pcsbi->dwSize);
        hInst = GetModuleHandle(NULL);
        hWnd = GetForegroundWindow();
        MessageBox(hWnd,
                   LoadRcString(IDS_BUFFER_SIZE_EVENT),
                   TEXT("Grid Map"),
                   MB_OK | MB_ICONINFORMATION | MB_SETFOREGROUND | MB_TASKMODAL);
        if(pcsbi->dwSize.X - 4 > WinBuffSizeEvent.dwSize.X)
            ClearRightSideGrid(hStdOut);
        if(!DisplayGrid(hStdOut, 0, GRID_HEIGHT))
            PrintError(GetLastError());
        DisplayCodeMap(hStdOut, CHART_START, CHART_END);
    }
}


void CommandLoop(HANDLE hStdIn, HANDLE hStdOut)
{
    INPUT_RECORD               inBuffer;
    DWORD                      cRead;
    CONSOLE_SCREEN_BUFFER_INFO csbi;

    GetConsoleScreenBufferInfo(hStdOut, &csbi);
    if(!DisplayGrid(hStdOut, 0, GRID_HEIGHT))
        PrintError(GetLastError());
    if(!DisplayCodeMap(hStdOut, CHART_START, CHART_END))
        PrintError(GetLastError());
    do {
        if(!ReadConsoleInput(hStdIn, &inBuffer, 1, &cRead)) {
            PrintError(GetLastError());
        }
        switch (GM_EVENT_TYPE(inBuffer)) {
            case KEY_EVENT:
                if(GM_EVENT_KEYDOWN(inBuffer)) {
                    ProcessKeyCommand(hStdIn, hStdOut,
                                    GM_EVENT_VKEYCODE(inBuffer));
                }
                break;
            case WINDOW_BUFFER_SIZE_EVENT:
                BufferSizeEvent(hStdIn, hStdOut, 
                            &csbi, GM_EVENT_BUFFER_SIZE(inBuffer));
                break;
            case MOUSE_EVENT:
            case FOCUS_EVENT:
            case MENU_EVENT:
            default:
                break;
        }
    } while (!(GM_EVENT_TYPE(inBuffer) == KEY_EVENT &&
               GM_EVENT_VKEYCODE(inBuffer) == VK_F3 &&
               GM_EVENT_KEYDOWN(inBuffer)));
}


BOOL DisplayCodeMapHeader(HANDLE hConsole)
{
    BOOL  fResult = FALSE;
    UINT  low;
    DWORD dwWritten;
    COORD cWriteCoord = {MARGINX+6, 1};

    for(low = 0x00; low <= 0x0f; low++, cWriteCoord.X += 4) {
        TCHAR szChar[3];
        wsprintf(szChar, TEXT("%X"), low);
        if(!(fResult = WriteConsoleOutputCharacter(hConsole, szChar,
                         lstrlen(szChar), cWriteCoord, &dwWritten)))
            break;
    }
    return fResult;
}


CHARSET CurrentCharSet(BOOL fAction)
{
    static CHARSET CharSet = _DBC_;

    if(fAction) {
        if(CharSet == _DBC_) {
            CharSet = _SBC_;
            BeginHorValue = 0x00;
        }
        else {
            CharSet = _DBC_;
            BeginHorValue = DBCS_START;
        }
    }
    return CharSet;
}


void GetCurrentCharSetCodeRange(PCODERANGE codeRange)
{
    if(CurrentCharSet(FALSE) == _DBC_) {
        codeRange->Start = DBCS_START;
        codeRange->End   = DBCS_END;
    }
    else { // _SBC_
        codeRange->Start = SBCS_START;
        codeRange->End   = SBCS_END;
    }
}


BOOL DisplayGrid(HANDLE hConsole, SHORT nStartLine, SHORT nEndLine)
{
    BOOL   fResult;
    LPWORD lpwAttrb1, lpwAttrb2, lpwTemp;
    WORD   wAttrb;
    COORD  ReadCoord = {0, 0};
    DWORD  cWriteCells;
    COORD  coordWrite;
    DWORD  dwWritten;
    WORD   conWidth, conHeight;
    BOOL   fGrid;

    ReadConsoleOutputAttribute(hConsole, 
                    &wAttrb, 
                    sizeof(wAttrb) / sizeof(WORD),
                    ReadCoord, 
                    &dwWritten);
    conWidth = GRID_WIDTH;
    conHeight = GRID_HEIGHT;
    cWriteCells = conWidth;
    if((lpwAttrb1 = lpwTemp = (LPWORD)HeapAlloc(ghHeap,
                                      HEAP_ZERO_MEMORY,
                                      (conWidth+1) * sizeof(WORD))) == NULL) {
        PrintError(GetLastError());
    }
    // Set output attributes
    *(lpwAttrb1 + conWidth) = COMMON_LVB_GRID_LVERTICAL;
    for(lpwTemp = lpwAttrb1 + conWidth - 1; lpwTemp >= lpwAttrb1; lpwTemp--) {
        *lpwTemp = wAttrb | COMMON_LVB_GRID_HORIZONTAL;
        if(!((lpwTemp - lpwAttrb1) % 4)) {
            *lpwTemp |= COMMON_LVB_GRID_LVERTICAL;
        }
    }
    if((lpwAttrb2 = lpwTemp = (LPWORD)HeapAlloc(ghHeap,
                                      HEAP_ZERO_MEMORY,
                                      (conWidth+1) * sizeof(WORD))) == NULL) {
        PrintError(GetLastError());
    }
    // Set output attributes
    *(lpwAttrb2 + conWidth) = COMMON_LVB_GRID_LVERTICAL;
    for(lpwTemp = lpwAttrb2 + conWidth - 1; lpwTemp >= lpwAttrb2; lpwTemp--) {
        *lpwTemp = wAttrb;
        if(!((lpwTemp - lpwAttrb2) % 4)) {
            *lpwTemp |= COMMON_LVB_GRID_LVERTICAL;
        }
    }
    for(coordWrite.X = MARGINX, coordWrite.Y = nStartLine, fGrid = TRUE;
            coordWrite.Y <= nEndLine; coordWrite.Y++, fGrid ^= 1) {
        if(fGrid) {
            if(!(fResult =  WriteConsoleOutputAttribute(
                                hConsole,
                                lpwAttrb1,
                                cWriteCells + 1,
                                coordWrite,
                                &dwWritten))) {
                break;
            }
        }
        else {
            if(!(fResult =  WriteConsoleOutputAttribute(
                                hConsole,
                                lpwAttrb2,
                                cWriteCells + 1, 
                                coordWrite, 
                                &dwWritten))) {
                break;
            }
        }
    }
    for(lpwTemp = lpwAttrb2; lpwTemp <= lpwAttrb2 + conWidth - 1; lpwTemp++) {
            *lpwTemp = wAttrb | COMMON_LVB_GRID_HORIZONTAL;
    }
    coordWrite.X = MARGINX;
    coordWrite.Y = conHeight + 1;
    fResult =  WriteConsoleOutputAttribute(
                                hConsole,
                                lpwAttrb2,
                                cWriteCells, 
                                coordWrite, 
                                &dwWritten);
    HeapFree(ghHeap, 0, lpwAttrb1);
    HeapFree(ghHeap, 0, lpwAttrb2);
    return fResult;
}


WORD GetStartCodeToDisplay(SHORT nStartLine, SHORT nEndLine)
{
    WORD wCode = BeginHorValue;

    if(nStartLine == CHART_START) {
        if(nEndLine == CHART_START) {
            ;
        }
        else if (nEndLine == CHART_END-1) {
            wCode = ++BeginHorValue;
        }
    }
    else if((nStartLine == CHART_END-1 && nEndLine == CHART_END-1) ||
             nStartLine == CHART_START+2 && nEndLine == CHART_END-1) {
            IncrementEndHorValue(CurrentCharSet(FALSE));
            wCode = EndHorValue;
    }
    return wCode;
}


void SetEndHorValueAfterDispCodeMap(SHORT nStartLine, SHORT nEndLine, WORD wCode)
{
    CHARSET CharSet = CurrentCharSet(FALSE);

    if(nStartLine == nEndLine && nStartLine == CHART_START) {
        DecrementEndHorValue(CharSet);
    }
    else if(nStartLine == CHART_START && nEndLine == (CHART_END - 3))
        EndHorValue = wCode;
    else
        EndHorValue = wCode - 0x10;
}


BOOL DisplayCodeMap(HANDLE hConsole, SHORT nStartLine, SHORT nEndLine)
{
    BOOL      fResult = TRUE;
    WORD      high, low;
    DWORD     dwWritten;
    COORD     cWriteCoord;
    CODERANGE codeRange;
    CHARSET   CharSet;

    CharSet = CurrentCharSet(FALSE);
    GetCurrentCharSetCodeRange(&codeRange);
    if(!(fResult = DisplayCodeMapHeader(hConsole)))
        return fResult;
    for(high = GetStartCodeToDisplay(nStartLine,nEndLine),
        cWriteCoord.Y = nStartLine;
            cWriteCoord.Y <= nEndLine; high += 0x10, cWriteCoord.Y += 2) {
        TCHAR szChar[5];
        WCHAR szWChar[2];
 
        if(CharSet == _DBC_ && !IsDBCSCode(high)) {
            cWriteCoord.Y -= 2;
            continue;
        }
        cWriteCoord.X = (CharSet == _SBC_) ? MARGINX + 1 : MARGINX;
        wsprintf(szChar, TEXT("%02X"), high);
        if(!(fResult = WriteConsoleOutputCharacter(hConsole, szChar,
                    lstrlen(szChar), cWriteCoord, &dwWritten)))
            break;
        for(low = 0x0, cWriteCoord.X = MARGINX + 5; low <= 0xf;
                        low++, cWriteCoord.X += 4) {
            COORD tmpCoord;
            if(CharSet == _DBC_) {
                szChar[0] = HIBYTE(high);
                szChar[1] = LOBYTE(high | low);
                szChar[2] = '\0';
                MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, szChar, -1,
                        szWChar, sizeof(szWChar)/sizeof(WCHAR));
            }
            else {
                if(!_ismbbprint(high | low)) {
                    lstrcpy(szChar,TEXT("  "));
                    szWChar[0] = TEXT('\0');
                }
                else {
                    wsprintf(szChar,TEXT(" %c"), high | low);
                    MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, szChar+1, -1,
                            szWChar, sizeof(szWChar)/sizeof(WCHAR));
                }
            }
            if(!(fResult = WriteConsoleOutputCharacter(hConsole, szChar,
                        lstrlen(szChar), cWriteCoord, &dwWritten)))
                break;
            tmpCoord.X = cWriteCoord.X - 1;
            tmpCoord.Y = cWriteCoord.Y + 1;
            if(szWChar[0] && (szWChar[0] != NOTASSIGNED ||
                    LOWORD(szChar[0] << 8 | szChar[1]) == DBC_CNTR_BLACK)) {
                wsprintf(szChar, TEXT("%04X"), szWChar[0]);
                if(!(fResult = WriteConsoleOutputCharacter(hConsole, szChar,
                                lstrlen(szChar), tmpCoord, &dwWritten)))
                    break;
            }
            else {
                if(!(fResult = WriteConsoleOutputCharacter(hConsole, BLANK4,
                                lstrlen(BLANK4), tmpCoord, &dwWritten)))
                    break;
            }
        }
        if(fResult) {
            if(!(fResult = WriteConsoleOutputCharacter(hConsole, TEXT("\n"), 1,
                    cWriteCoord, &dwWritten)))
                break;
        }
        else
            break;
    }
    SetEndHorValueAfterDispCodeMap(nStartLine, nEndLine, high);
    return fResult;
}


int main(void)
{
    HANDLE hStdIn, hStdOut;

    if(GetVersion() & 0x80000000 && (GetVersion() & 0xFF) == 3) {
        MessageBox(NULL, LoadRcString(IDS_CANT_RUN_WIN31),
            NULL, MB_OK | MB_ICONSTOP);
        return(1);
    }
    InitConsole();
    hStdIn = InitStdIn();
    hStdOut = InitStdOut();

    CommandLoop(hStdIn, hStdOut);
    return(0);
}

