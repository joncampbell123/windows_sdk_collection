//************************************************************************
//**
//**  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//**  ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED
//**  TO THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR
//**  A PARTICULAR PURPOSE.
//**
//**  Copyright (C) 1993 - 1995 Microsoft Corporation. All Rights Reserved.
//**
//**  about.c
//**
//**  DESCRIPTION:
//**
//**
//**  HISTORY:
//**     06/28/93          Created.
//**
//************************************************************************

#include "globals.h"
#include "about.h"

//************************************************************************
//**
//**  About();
//**
//**  DESCRIPTION:
//**
//**
//**  ARGUMENTS:
//**     VOID 
//**
//**  RETURNS:
//**     VOID 
//**
//**  HISTORY:
//**     06/28/93          Created.
//**
//************************************************************************

VOID FAR PASCAL About(
   VOID)
{
   DLGPROC fpDlg;

   fpDlg = (DLGPROC)MakeProcInstance((FARPROC)AboutDlgProc, ghinst);
   DialogBox(ghinst, "About", ghwndMain, fpDlg);
   FreeProcInstance((FARPROC)fpDlg);

} //** About()


//************************************************************************
//**
//**  AboutDlgProc();
//**
//**  DESCRIPTION:
//**
//**
//**  ARGUMENTS:
//**     HWND     hdlg
//**     UINT     umsg
//**     WPARAM   wparam
//**     LPARAM   lparam
//**
//**  RETURNS:
//**     BOOL 
//**
//**  HISTORY:
//**     06/28/93          Created.
//**
//************************************************************************

BOOL FNEXPORT AboutDlgProc(
   HWND     hdlg,
   UINT     umsg,
   WPARAM   wparam,
   LPARAM   lparam)
{
   switch (umsg) 
   {
      case WM_INITDIALOG:
         break;

      case WM_COMMAND:
         EndDialog(hdlg, TRUE);
         break;

      default:
         return(FALSE);
         break;
   }

   return(TRUE);
} //** AboutDlgProc()

