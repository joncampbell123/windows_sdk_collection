
/******************************************************************************\
*       This is a part of the Microsoft Source Code Samples. 
*       Copyright (C) 1993-1995 Microsoft Corporation.
*       All rights reserved. 
*       This source code is only intended as a supplement to 
*       Microsoft Development Tools and/or WinHelp documentation.
*       See these sources for detailed information regarding the 
*       Microsoft samples programs.
\******************************************************************************/

#include <windows.h>
#include <stdio.h>
#include <string.h>
#include "console.h"

/*********************************************************************
* FUNCTION: rawOut(HANDLE hConsole, PCHAR s)                         *
*                                                                    *
* PURPOSE: output the contents of the input buffer to the console    *
*          handle in 'raw' mode                                      *
*                                                                    *
* INPUT: the console handle to write to, and the string to output in *
*        'raw' mode                                                  *
*********************************************************************/

void rawOut(HANDLE hConsole, PCHAR s)
{
  BOOL bSuccess;
  DWORD cCharsWritten;
  DWORD dwOutputMode;

  bSuccess = GetConsoleMode(hConsole, &dwOutputMode);
  PERR(bSuccess, "GetconsoleMode");
  /* output the string in raw mode. */
  /* turn off processed output, output the string, and reset output mode */
  bSuccess = SetConsoleMode(hConsole, dwOutputMode & ~ENABLE_PROCESSED_OUTPUT);
  PERR(bSuccess, "SetConsoleMode");
  bSuccess = WriteFile(hConsole, s, strlen(s), &cCharsWritten, NULL);
  PERR(bSuccess, "WriteFile");
  /* now reset output mode */
  bSuccess = SetConsoleMode(hConsole, dwOutputMode);
  PERR(bSuccess, "SetConsoleMode");
  return;
}

/******************************************************************
* FUNCTION: demoConMode(HANDLE hConOut)                           *
*                                                                 *
* PURPOSE: demonstrate GetConsoleMode and SetConsoleMode. Display *
*          the current console input and output modes, then       *
*          demonstrate each console input and output mode         *
*                                                                 *
*                                                                 *
* INPUT: the console output handle to write to                    *
******************************************************************/

void demoConMode(HANDLE hConOut)
{
  BOOL bSuccess;
  DWORD dwOutputMode, dwInputMode; /* save input & output console modes */
  CHAR szTemp[128];
  HANDLE hStdIn; /* standard input handle */
  /* a string to test 'raw' vs. 'processed' output with */
  PCHAR szModeTst = "タブ:\t バックスペース:\b ラインフィード:\n ベル:\a キャリッジリターン:\r";
  DWORD dwRead;
  CHAR chBuf[256]; /* buffer to read a user string from the console */
  PCHAR szLong = "これは１行が８０文字を超えるテキストです。これは ENABLE_WRAP_AT_EOL_OUTPUT のテストです。";

  setConTitle(__FILE__);
  hStdIn = GetStdHandle(STD_INPUT_HANDLE);
  PERR(hStdIn != INVALID_HANDLE_VALUE, "GetStdHandle");
  /* get the input mode and save it so that we can restore it later */
  bSuccess = GetConsoleMode(hStdIn, &dwInputMode);
  PERR(bSuccess, "GetconsoleMode");
  myPuts(hConOut, "GetconsoleMode によって報告された現在のコンソールの入力モード:");
  sprintf(szTemp, "ENABLE_LINE_INPUT: %s", dwInputMode & ENABLE_LINE_INPUT ?
      "オン" : "オフ");
  myPuts(hConOut, szTemp);
  sprintf(szTemp, "ENABLE_ECHO_INPUT: %s", dwInputMode & ENABLE_ECHO_INPUT ?
      "オン" : "オフ");
  myPuts(hConOut, szTemp);
  sprintf(szTemp, "ENABLE_WINDOW_INPUT: %s", dwInputMode &
      ENABLE_WINDOW_INPUT ? "オン" : "オフ");
  myPuts(hConOut, szTemp);
  sprintf(szTemp, "ENABLE_PROCESSED_INPUT: %s", dwInputMode &
      ENABLE_PROCESSED_INPUT ? "オン" : "オフ");
  myPuts(hConOut, szTemp);
  bSuccess = GetConsoleMode(hConOut, &dwOutputMode);
  PERR(bSuccess, "GetconsoleMode");
  myPuts(hConOut, "\n現在のコンソールの出力モード:");
  sprintf(szTemp, "ENABLE_PROCESSED_OUTPUT: %s", dwOutputMode &
      ENABLE_PROCESSED_OUTPUT ? "オン" : "オフ");
  myPuts(hConOut, szTemp);
  sprintf(szTemp, "ENABLE_WRAP_AT_EOL_OUTPUT: %s", dwOutputMode &
      ENABLE_WRAP_AT_EOL_OUTPUT ? "オン" : "オフ");
  myPuts(hConOut, szTemp);
  bSuccess = SetConsoleMode(hConOut, dwOutputMode);  /* back to normal */
  PERR(bSuccess, "SetConsoleMode");
  myPuts(hConOut, "\n続けるには Enter を押してください...");
  myGetchar();
  cls(hConOut);
  myPuts(hConOut, "SetConsoleMode でコンソールの出力モードをテストします。最初に、長いテキスト行を\n"
                  "ENABLE_WRAP_AT_EOL_OUTPUT をオンにして出力します。次に、オフにして行ないます:\n");
  myPuts(hConOut, szLong);
  myPuts(hConOut, "\nここで ENABLE_WRAP_AT_EOL_OUTPUT をオフにして同じ文字列を表示します。１行に表\n"
                  "示しきれない行が次の行に表示されずに、同じ行に表示されることに注意してください:\n");
  /* turn off EOL wrap */
  bSuccess = SetConsoleMode(hConOut, dwOutputMode &
      ~ENABLE_WRAP_AT_EOL_OUTPUT);
  PERR(bSuccess, "SetConsoleMode");
  myPuts(hConOut, szLong);
  /* turn on EOL wrap */
  bSuccess = SetConsoleMode(hConOut, dwOutputMode |
      ENABLE_WRAP_AT_EOL_OUTPUT);
  PERR(bSuccess, "SetConsoleMode");
  myPuts(hConOut, "\n\nプロセス出力のテストをします。ENABLE_PROCESSED_OUTPUT　をオフにして文字列を出\n"
                  "力します。そしてオンにして出力します。バックスペース、タブ、ベル、キャリッジリ\n"
                  "ターンおよびラインフィードが異なる処理をされることに注意してください:\n");
  /* turn off processed ("cooked") output - now in "raw" mode */
  bSuccess = SetConsoleMode(hConOut, dwOutputMode & ~ENABLE_PROCESSED_OUTPUT);
  PERR(bSuccess, "SetConsoleMode");
  /* myPuts() appends a \n, so can't use it for this case */
  bSuccess = WriteFile(hConOut, szModeTst, strlen(szModeTst), &dwRead, NULL);
  PERR(bSuccess, "WriteFile");
  /* turn processed ("cooked") mode back on */
  bSuccess = SetConsoleMode(hConOut, dwOutputMode | ENABLE_PROCESSED_OUTPUT);
  PERR(bSuccess, "SetConsoleMode");
  myPuts(hConOut, "\nここで ENABLE_PROCESSED_OUTPUT をオンにし、同じ文字列を\n"
                  "出力します:\n");
  myPuts(hConOut, szModeTst);
  myPuts(hConOut, "\n次のテストを行なうには Enter を押してください...");
  myGetchar();
  cls(hConOut);
  myPuts(hConOut, "ここで入力モードをテストしましょう。最初の入力は\n"
                  "ENABLE_PROCESSED_INPUT がオンです。このモードで、何が\n"
                  "読み込まれたかが正確に出力されます。バックスペース\n"
                  "タブ、ctrl+g、キャリッジリターンおよびラインフィードが\n"
                  "どのように翻訳されるかに注意してください。１行のテキス\n"
                  "トを入力して Enter を押してください:\n");
  /* turn on processed, line, and echo modes. */
  /* MUST turn on echo mode when turning on line mode */
  bSuccess = SetConsoleMode(hStdIn, dwInputMode | ENABLE_PROCESSED_INPUT |
      ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT);
  PERR(bSuccess, "SetConsoleMode");
  memset(chBuf, 0, sizeof(chBuf));
  bSuccess = ReadFile(hStdIn, chBuf, sizeof(chBuf), &dwRead, NULL);
  PERR(bSuccess, "ReadFile");
  /* output the contents of chBuf in raw mode */
  rawOut(hConOut, chBuf);
  myPuts(hConOut, "\nここで ENABLE_PROCESSED_INPUT をオフにしましょう。\n"
                  "文字列を入力してください:\n");
  /* turn off processed, line, and echo input. */
  /* MUST turn off echo input when turning off line input */
  bSuccess = SetConsoleMode(hStdIn, dwInputMode & ~ENABLE_PROCESSED_INPUT |
      ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT);
  PERR(bSuccess, "SetConsoleMode");
  memset(chBuf, 0, sizeof(chBuf));
  bSuccess = ReadFile(hStdIn, chBuf, sizeof(chBuf), &dwRead, NULL);
  PERR(bSuccess, "ReadFile");    
  rawOut(hConOut, chBuf);
  myPuts(hConOut, "\n続けるには Enter を押してください...");
  myGetchar();
  cls(hConOut);
  myPuts(hConOut, "ENABLE_LINE_INPUT をオフにしましょう - これまでの入力は\n"
                  "すべて行入力でした。このフラグがオフのときには\n"
                  "ENABLE_ECHO_INPUT もオフにしなければなりません。コンソール\n"
                  "から１文字を読み込みましょう。プログラムは直ちに読まれた文\n"
                  "字を返します。文字はエコーしないことに注意してください。\n"
                  "どれかキーを押してください:");
  bSuccess = SetConsoleMode(hStdIn, dwInputMode & ~ENABLE_LINE_INPUT &
      ~ENABLE_ECHO_INPUT);
  PERR(bSuccess, "SetConsoleMode");
  memset(chBuf, 0, sizeof(chBuf));
    bSuccess = ReadFile(hStdIn, chBuf, sizeof(chBuf), &dwRead, NULL);
  PERR(bSuccess, "ReadFile");    
  myPuts(hConOut, "\n読み込まれた文字です:");
  myPuts(hConOut, chBuf);
  myPuts(hConOut, "\n\n続けるには Enter を押してください...");
  bSuccess = SetConsoleMode(hStdIn, dwInputMode | ENABLE_WINDOW_INPUT);
  PERR(bSuccess, "SetConsoleMode");
  myGetchar();
  return;
}
