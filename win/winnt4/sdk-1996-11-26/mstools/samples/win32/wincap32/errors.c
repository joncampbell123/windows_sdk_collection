//
// Errors.c
//
// Contains error messages for DIBAPI.DLL
//
// These error messages all have constants associated with
// them, contained in dibapi.h.
//
// Copyright (C) 1991-1996 Microsoft Corporation. All rights reserved.
//

#define     STRICT      // enable strict type checking

#include <windows.h>
#include "dibapi.h"

extern WORD nLangID;
static char *szErrors[] =
{
    "Not a Windows DIB file!",
    "Couldn't allocate memory!",
    "Error reading file!",
    "Error locking memory!",
    "Error opening file!",
    "Error creating palette!",
    "Error getting a DC!",
    "Error creating Device Dependent Bitmap",
    "StretchBlt() failed!",
    "StretchDIBits() failed!",
    "SetDIBitsToDevice() failed!",
    "Printer: StartDoc failed!",
    "Printing: GetModuleHandle() couldn't find GDI!",
    "Printer: SetAbortProc failed!",
    "Printer: StartPage failed!",
    "Printer: NEWFRAME failed!",
    "Printer: EndPage failed!",
    "Printer: EndDoc failed!",
    "SetDIBits() failed!",
    "File Not Found!",
    "Invalid Handle",
    "General Error on call to DIB function"
};

static char *szErrorsJ[] =
{
    "Windows DIB ファイルではありません。",
    "メモリを確保することができませんでした。",
    "ファイルの読み込みエラー",
    "メモリのロック エラー",
    "ファイルのオープン エラー",
    "パレットの作成エラー",
    "デバイス コンテキストの取得エラー",
    "デバイス依存ビットマップの作成エラー",
    "StretchBlt() が失敗しました。",
    "StretchDIBits() が失敗しました。",
    "SetDIBitsToDevice() が失敗しました。",
    "プリンタ: StartDoc が失敗しました。",
    "印刷: GetModuleHandle() GDI が見つかりませんでした。",
    "プリンタ: SetAbortProc が失敗しました。",
    "プリンタ: StartPage が失敗しました。",
    "プリンタ: NEWFRAME が失敗しました。",
    "プリンタ: EndPage が失敗しました。",
    "プリンタ: EndDoc が失敗しました。",
    "SetDIBits() が失敗しました。",
    "ファイルが見つかりませんでした。",
    "無効なハンドル",
    "DIB ファンクション呼び出しでの一般エラー"
};

void DIBError(int ErrNo)
{
    if (nLangID == LANG_JAPANESE) {
        if ((ErrNo < ERR_MIN) || (ErrNo >= ERR_MAX))
            MessageBox(NULL, "未知のエラー。", NULL, MB_OK | MB_ICONHAND);
        else
            MessageBox(NULL, szErrorsJ[ErrNo], NULL, MB_OK | MB_ICONHAND);
    }
    else {
        if ((ErrNo < ERR_MIN) || (ErrNo >= ERR_MAX))
            MessageBox(NULL, "Undefined Error!", NULL, MB_OK | MB_ICONHAND);
        else
            MessageBox(NULL, szErrors[ErrNo], NULL, MB_OK | MB_ICONHAND);
    }
}
