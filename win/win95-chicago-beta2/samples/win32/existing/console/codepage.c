#include <windows.h>
#include <stdio.h>
#include <string.h>
#include "console.h"

/* Microsoft Developer Support
   Copyright (c) 1992-1995 Microsoft Corporation. All Rights Reserved */

/*********************************************************************
* FUNCTION: demoCodePage(HANDLE hConOut)                             *
*                                                                    *
* PURPOSE: demonstrate SetConsoleCP, SetConsoleOutputCP,             *
*          GetConsoleCP, and GetConsoleOutputCP. Display the current *
*          input and output code page numbers, set the               *
*                                                                    *
* INPUT: the console output handle to manipulate the codepage for    *
*********************************************************************/

void demoCodePage(HANDLE hConOut)
{
  char szTemp[128];
  UINT uiInputCP, uiOutputCP;  /* hold the input & output code pages */
  /* array of all the possible Win32 code page values */
  int iCodePages[] = {437, 850, 860, 862, 863, 864, 865, 932, 934, 936,
                      938, 852, 866};
  BOOL bSuccess;
  int i, j;

  myPuts(hConOut, "Demo not implemented yet. Hit return to continue...");
  myGetchar();
  return;

  setConTitle(__FILE__);
  uiInputCP = GetConsoleCP();
  uiOutputCP = GetConsoleOutputCP();
  sprintf(szTemp, "The current input code page is: %d", uiInputCP);
  myPuts(hConOut, szTemp);
  sprintf(szTemp, "The current output code page is: %d", uiOutputCP);
  myPuts(hConOut, szTemp);
  myPuts(hConOut, "\nNow let's set the output code page to each of the\n"
                  "following values and dump out some upper-ascii\n"
                  "characters. Hit any key to continue...\n\n"
                  "437  United States\n"
                  "850  Mulitlingual\n"
                  "860  Portuguese\n"
                  "862  Hebrew\n"
                  "863  Canadian French\n"
                  "864  Arabic\n"
                  "865  Nordic\n"
                  "932  Japan\n"
                  "934  Corea\n"
                  "936  Peoples Republic of China\n"
                  "938  Taiwan\n"
                  "852  Slavic\n"
                  "866  Russian\n");
  myGetchar();
  for (i = 0; i < sizeof(iCodePages) / sizeof(int); i++)
    {
    sprintf(szTemp, "Code page %d:", iCodePages[i]);
    myPuts(hConOut, szTemp);
    bSuccess = SetConsoleOutputCP(iCodePages[i]);
    PERR(bSuccess, "SetConsoleOutputCP");
    for (j = 0; j < 128; j++)
      szTemp[j] = j + 128;
    strcat(szTemp, "\n");
    myPuts(hConOut, szTemp);
    }
  return;
}
