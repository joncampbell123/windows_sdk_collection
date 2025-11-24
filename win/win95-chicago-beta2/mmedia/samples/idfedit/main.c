//************************************************************************
//**
//**  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//**  ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED
//**  TO THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR
//**  A PARTICULAR PURPOSE.
//**
//**  Copyright (C) 1993 - 1995 Microsoft Corporation. All Rights Reserved.
//**
//**  main.c
//**
//**  DESCRIPTION:
//**     Performs window class registration, creations and message 
//**     polling.
//**
//**  HISTORY:
//**     04/22/93       created.
//**     09/07/93       revised to use dialog box.
//**
//************************************************************************

#include "globals.h"
#include "res.h"

//
// Constant read-only strings (allocated from code segment).
//
char  BCODE    gszClassName[] = "IDF'd";
char  BCODE    gszSS[] = "%-25.25s    %-30.30s";
char  BCODE    gszSU[] = "%-25.25s    %-30u";
char  BCODE    gszUS[] = "%-25u    %-30.30s";
char  BCODE    gszUU[] = "%-25u    %u";
char  BCODE    gszSX[] = "%-25.25s    0x%04X";
char  BCODE    gszU[]  = "%u";
char  BCODE    gszNULL[] = "";
char  BCODE    gszHexByte[] = "%02X ";
char  BCODE    gszEllipsis[] = "...";



char           gszTitleBar[20];
char           gszIDFTitle[MAX_TITLE_LEN+1];
char           gszIDFName[MAX_PATH_LEN+1];
HINSTANCE      ghinst;
HWND           ghwndMain;
UINT           guCurrSelection;
HMMIO          ghmmio;
LPINSTRUMENT   gpInst;
DWORD          gdwNumInsts;
DWORD          gdwCurrInst;
BOOL           gfChanged=FALSE;
char           gszbuf[MAX_STR_LEN+1];

char           gszChannelTypes[NUM_CHANNELS_DEFINED][20];
char           gszYes[10];
char           gszNo[10];
char           gszGeneralInit[80];
char           gszDrumInit[80];
BOOL           gfFirstEdit=TRUE;

//************************************************************************
//**
//**  InitClasses();
//**
//**  DESCRIPTION:
//**     This function will register the main window class.
//**
//**  ARGUMENTS:
//**     VOID
//**
//**  RETURNS:
//**     BOOL  -  TRUE if the main window class was successfully 
//**              registered, FALSE otherwise.
//**
//**  HISTORY:
//**     04/22/93       created.
//**     09/07/93       added dialog capabilities.
//**
//************************************************************************

BOOL WINAPI InitClasses(
   VOID)
{
   WNDCLASS wc;

   wc.style          =  CS_HREDRAW | CS_VREDRAW;
   wc.lpfnWndProc    =  MainProc;
   wc.cbClsExtra     =  0;
   wc.cbWndExtra     =  DLGWINDOWEXTRA;
   wc.hInstance      =  ghinst;
   wc.hIcon          =  LoadIcon(ghinst, MAKEINTRESOURCE(ICON_1));
   wc.hCursor        =  LoadCursor(NULL, IDC_ARROW);
   wc.hbrBackground  =  GetStockObject(LTGRAY_BRUSH);
   wc.lpszMenuName   =  MAKEINTRESOURCE(IDFEDIT_MENU);
   wc.lpszClassName  =  gszClassName;

   if (!RegisterClass(&wc))
      return(FALSE);

   return(TRUE);
} //** InitClasses()



//************************************************************************
//**
//**  InitInstance();
//**
//**  DESCRIPTION:
//**     This function will create the main application window.
//**
//**  ARGUMENTS:
//**     int nCmdShow   -  How the window is to be shown.
//**
//**  RETURNS:
//**     BOOL  -  TRUE if the window was successfully created, 
//**              FALSE if not.
//**
//**  HISTORY:
//**     04/22/93       created.
//**
//************************************************************************

BOOL WINAPI InitInstance(
   int nCmdShow)
{

   // Load the caption bars title.
   //
   LoadString(ghinst, IDS_CAPTION_BAR, gszTitleBar, sizeof(gszTitleBar));

   // Create the main window.
   //
   ghwndMain = CreateDialog(ghinst, 
                            MAKEINTRESOURCE(ID_IDFEDITDLG), 
                            NULL, 
                            NULL);

   // Did we succeed.
   //
   if (!ghwndMain)
      return(FALSE);

   // Initialize the Dialog
   InitializeDialog();

   // Show and update..
   //
   ShowWindow(ghwndMain, nCmdShow);
   UpdateWindow(ghwndMain);

   // Return success.
   //
   return(TRUE);
} //** InitInstance()


//************************************************************************
//**
//**  InitializeDialog();
//**
//**  DESCRIPTION:
//**
//**  ARGUMENTS:
//**     HWND  hwnd  -  Handle to the application's window.
//**
//**  RETURNS:
//**     VOID 
//**
//**  HISTORY:
//**     04/27/93       created.
//**     09/06/93       rewrote.
//**
//************************************************************************

VOID FNLOCAL InitializeDialog()
{
   UINT  c;
   UINT  iErr;
   HFONT hfont;
   
   // Change the font for the list box.
   //
   hfont = GetStockObject(SYSTEM_FIXED_FONT);

   // Disable redrawing of the list box.
   //
   SendDlgItemMessage(ghwndMain, ID_LIST_BOX, WM_SETREDRAW, FALSE, 0L);

   // Associate the font with our list box.
   //
   SendDlgItemMessage(ghwndMain, ID_LIST_BOX, WM_SETFONT, (WPARAM)hfont, 0L);

   // Enable redrawing of the list box.
   //
   SendDlgItemMessage(ghwndMain, ID_LIST_BOX, WM_SETREDRAW, TRUE, 0L);

   //
   // Load the strings that we will be displaying to the
   // user from our resource string table.
   //

   // Get the strings for the channel types that are defined.
   //
   for ( c = 0;
         c < NUM_CHANNELS_DEFINED;
         c++)
   {
      // Get the string from our string table.
      //
      iErr = LoadString(ghinst, 
                        IDS_CHANNEL_TYPE_BASE + c, 
                        gszChannelTypes[c], 
                        sizeof(gszChannelTypes[c]));
      if (0 == iErr)
      {
         // We could not load the string.
         //
         gszChannelTypes[c][0] = '\0';
      }
   }

   // Grab other internationalizable things.
   //

   if (0 == LoadString(ghinst, IDS_YES, gszYes, sizeof(gszYes)))
       *gszYes = '\0';

   if (0 == LoadString(ghinst, IDS_NO, gszNo, sizeof(gszNo)))
       *gszNo = '\0';

   if (0 == LoadString(ghinst, IDS_GENERAL_INIT, gszGeneralInit, sizeof(gszGeneralInit)))
       *gszGeneralInit = '\0';

   if (0 == LoadString(ghinst, IDS_DRUM_INIT, gszDrumInit, sizeof(gszDrumInit)))
       *gszDrumInit = '\0';


   // Fill the Section combo-box.
   //
   for ( c = 0;
         c < TOTAL_SECTION_ENTRIES;
         c++)
   {
      // Get the string from our string table.
      //
      LoadString(ghinst, 
                 IDS_SECTION_NAME_BASE + c, 
                 gszbuf, 
                 MAX_STR_LEN);

      // Add the string to the list box.
      //
      ComboBox_AddString(HWND_IDF_CURRENT_SECTION, (LPARAM)(LPCSTR)gszbuf);
   }

} //** InitializeDialog()

#ifdef DEBUG
#endif


//************************************************************************
//**
//**  WinMain();
//**
//**  DESCRIPTION:
//**     Setups of window's and classes, processes messages.
//**
//**  ARGUMENTS:
//**     HINSTANCE   hInstance      - Handle to program's instance.
//**     HINSTANCE   hPrevInstance  - Handle to previous instances.
//**     LPSTR       lpszCmdLine    - Command line.
//**     int         nCmdShow       - How do we show the app.
//**
//**  RETURNS:
//**     int 
//**
//**  HISTORY:
//**     04/22/93       created.
//**
//************************************************************************

int PASCAL WinMain(
   HINSTANCE   hInstance,
   HINSTANCE   hPrevInstance,
   LPSTR       lpszCmdLine,
   int         nCmdShow)
{
   MSG   msg;
   BOOL  f;

   // Save the instance handle in a global for
   // easy reference.
   //
   ghinst = hInstance;

   // If there are no previous instances then register classes.
   //
   if (NULL == hPrevInstance)
   {
      // Create the class structure for all instances.
      //
      f = InitClasses();
      if (!f)
      {
         // Failed to create class,
         // exit program.
         //
         return(FALSE);
      }
   }

   // Create the window.
   //
   f = InitInstance(nCmdShow);
   if (!f)
   {
      // Failed to initialize instance specific 
      // information.
      //
      return(FALSE);
   }

   // Enter message loop.
   //
   while (GetMessage(&msg, NULL, 0, 0))
   {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
   }

   // Return to Windows
   //
   return(msg.wParam);
} //** WinMain()

