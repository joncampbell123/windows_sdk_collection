/************************************************************

   PROGRAM: EXPENSE.C

   PURPOSE:
   
      An expense reporting program demonstrating the use
      different dictionaries for different edit control fields.

   COMMENTS:

      EXPENSE has several edit fields, typical of a generic
      expense report (name, employee #, dept. items, etc.).  The
      application registers itself as a pen aware application
      so that the edit controls will be replaced by hedit
      controls.

      This application will run under Win 3.0.  When running
      under Win 3.0, the edit control functionality will be
      present.  The boxed edit field and the picture field
      will not be present.

      The Delay Recognition menu, when checked, will cause
      ink to be captured in the inking field.  When the
      menu item is unchecked, the first tap in the field
      will cause recognition.  After that, the field will
      behave like a normal HEDIT control.


************************************************************/

#include <windows.h>
#include <penwin.h>
#include "expres.h"
#include "expense.h"

/******** Module Variables *********/

char *   szAppName         = "Expense";
char *   szIniFile         = "penwin.ini";
char *   szUserDictLib     = "userdict.dll";
char *   szUserDictProc    = "DictionaryProc";
char *   szCustomDictLib   = "custdict.dll";
char *   szCustomDictProc  = "DictionaryProc";
char *   szExpenseClass    = "ExpenseClass";
char *   szExpenseWnd      = "Sample Expense Report";
BOOL     fUpdateTotalValue = FALSE;
HANDLE   hAccel;           /* Menu Accelerator Table */ 
HANDLE   hwndMain;         /* Parent window to all fields */ 
HANDLE   hPenWin           = NULL;
HANDLE   hUserDictLib;
HANDLE   hCustomDictLib;
int      iLastInUserDict   = DictParam_None; 

LPDF lpdfUserDictProc;
LPDF lpdfCustomDictProc;

BOOL fUserInit=FALSE;
BOOL fCustomInit=FALSE;

VOID (FAR PASCAL *RegPenApp)(WORD, BOOL) = NULL;

WORDLIST rgwordlist[] =
   {
   {"namedict", "names.dic", 0 },
   {"deptnamedict", "deptname.dic", 0 }
   };

TEXT rgtext[cTexts] =   
   {
   {"Name:",               7,    8 },
   {"Employee",            135,  4 },
   {"Number:",             135,  12 },
   {"Dept.",               7,    28 },
   {"Name:",               7,    36 },
   {"Dept.",               135,  28 },
   {"Number:",             135,  36 },
   {"DATE",                29,   62 },
   {"EXPENSE ITEM",        95,   62 },
   {"VALUE ($)",           185,  62 },
   {"Signature:",          9,    165 },
   {"Date",                156,  157 },
   {"Submitted:",          156,  165 },
   };

char lpDateDelimiter[] = "/:.-";
char lpMoneyDelimter[] = "$.,";

EDITFIELD rgeditfield[cEditFields] =   
   {
   /* Name */
   { 37,  6, 75, 13, 
     ES_LEFT | ES_AUTOHSCROLL | WS_BORDER | WS_TABSTOP,
     ALC_DEFAULT, NULL, NULL, DICT_USER, IWORDLIST_NAME,   
     FIELDEDIT, NULL},

   /* Employee # */
   { 177, 7, BXD_CELLWIDTH*EmployNumDigits+1, BXD_CELLHEIGHT, 
     0, 
     ALC_NUMERIC | ALC_GESTURE, NULL, NULL, DICT_NULL, IWORDLIST_NONE, 
     FIELDBEDIT, NULL},

   /* Dept. Name */
   { 37,  30, 75, 13, 
     ES_LEFT | ES_AUTOHSCROLL | WS_BORDER | WS_TABSTOP,
     ALC_DEFAULT, NULL, NULL, DICT_USER, IWORDLIST_DEPTNAME, 
     FIELDEDIT, NULL},

   /* Dept. # */
   { 177,  31, BXD_CELLWIDTH*DeptNumDigits+1, BXD_CELLHEIGHT, 
     0,
     ALC_NUMERIC | ALC_GESTURE, NULL, NULL, DICT_NULL, IWORDLIST_NONE, 
     FIELDBEDIT, NULL},

   /* Date #1 */
   { 7, 74, 64, 12,
     ES_LEFT | WS_BORDER | WS_TABSTOP,
     ALC_USEBITMAP | ALC_NUMERIC | ALC_GESTURE, lpDateDelimiter, NULL, DICT_NULL, IWORDLIST_NONE, 
     FIELDEDIT, NULL},

   /* Expense Item #1 */
   { 71, 74, 95, 12, 
     ES_LEFT | WS_BORDER | WS_TABSTOP,
     ALC_DEFAULT, NULL, RCO_SUGGEST, DICT_CUSTOM, IWORDLIST_NONE, 
     FIELDEDIT, NULL},

   /* Value #1 */
   { 166, 74, 68, 12, 
     ES_MULTILINE | ES_RIGHT | WS_BORDER | WS_TABSTOP,
     ALC_USEBITMAP | ALC_NUMERIC | ALC_GESTURE, lpMoneyDelimter, NULL, DICT_NULL, IWORDLIST_NONE, 
     FIELDEDIT, NULL},
                              
   /* Date #2 */
   { 7, 86, 64, 12,
     ES_LEFT | WS_BORDER | WS_TABSTOP,
     ALC_USEBITMAP | ALC_NUMERIC | ALC_GESTURE, lpDateDelimiter, NULL, DICT_NULL, IWORDLIST_NONE, 
     FIELDEDIT, NULL},

   /* Expense Item #2 */
   { 71, 86, 95, 12,
     ES_LEFT | WS_BORDER | WS_TABSTOP,
     ALC_DEFAULT, NULL, RCO_SUGGEST,  DICT_CUSTOM, IWORDLIST_NONE, 
     FIELDEDIT, NULL},

   /* Value #2 */
   { 166, 86, 68, 12,
     ES_MULTILINE | ES_RIGHT | WS_BORDER | WS_TABSTOP,
     ALC_USEBITMAP | ALC_NUMERIC | ALC_GESTURE, lpMoneyDelimter, NULL, DICT_NULL, IWORDLIST_NONE, 
     FIELDEDIT, NULL},

   /* Date #3 */
   { 7, 98, 64, 12,
     ES_LEFT | WS_BORDER | WS_TABSTOP,
     ALC_USEBITMAP | ALC_NUMERIC | ALC_GESTURE, lpDateDelimiter, NULL, DICT_NULL, IWORDLIST_NONE, 
     FIELDEDIT, NULL},

   /* Expense Item #3 */
   { 71, 98, 95, 12,
     ES_LEFT | WS_BORDER | WS_TABSTOP,
     ALC_DEFAULT, NULL, RCO_SUGGEST, DICT_CUSTOM, IWORDLIST_NONE, 
     FIELDEDIT, NULL},

   /* Value #3 */
   { 166, 98, 68, 12,
     ES_MULTILINE | ES_RIGHT | WS_BORDER | WS_TABSTOP,
     ALC_USEBITMAP | ALC_NUMERIC | ALC_GESTURE, lpMoneyDelimter, NULL, DICT_NULL, IWORDLIST_NONE,  
     FIELDEDIT, NULL},

   /* Date #4 */
   { 7, 110, 64, 12,
     ES_LEFT | WS_BORDER | WS_TABSTOP,
     ALC_USEBITMAP | ALC_NUMERIC | ALC_GESTURE, lpDateDelimiter, NULL, DICT_NULL, IWORDLIST_NONE, 
     FIELDEDIT, NULL},

   /* Expense Item #4 */
   { 71, 110, 95, 12, 
     ES_LEFT | WS_BORDER | WS_TABSTOP,
     ALC_DEFAULT, NULL, RCO_SUGGEST, DICT_CUSTOM, IWORDLIST_NONE, 
     FIELDEDIT, NULL},

   /* Value #4 */
   { 166, 110, 68, 12, 
     ES_MULTILINE | ES_RIGHT | WS_BORDER | WS_TABSTOP,
     ALC_USEBITMAP | ALC_NUMERIC | ALC_GESTURE, lpMoneyDelimter, NULL, DICT_NULL, IWORDLIST_NONE, 
     FIELDEDIT, NULL},
                              
   /* Date #5 */
   { 7, 122, 64, 12,
     ES_LEFT | WS_BORDER | WS_TABSTOP,
     ALC_USEBITMAP | ALC_NUMERIC | ALC_GESTURE, lpDateDelimiter, NULL, DICT_NULL, IWORDLIST_NONE, 
     FIELDEDIT, NULL},

   /* Expense Item #5 */
   { 71, 122, 95, 12,
     ES_LEFT | WS_BORDER | WS_TABSTOP,
     ALC_DEFAULT, NULL, RCO_SUGGEST, DICT_CUSTOM, IWORDLIST_NONE, 
     FIELDEDIT, NULL},

   /* Value #5 */
   { 166, 122, 68, 12,
     ES_MULTILINE | ES_RIGHT | WS_BORDER | WS_TABSTOP,
     ALC_USEBITMAP | ALC_NUMERIC | ALC_GESTURE, lpMoneyDelimter, NULL, DICT_NULL, IWORDLIST_NONE, 
     FIELDEDIT, NULL},

   /* Date #6 */
   { 7, 134, 64, 12,
     ES_LEFT | WS_BORDER | WS_TABSTOP,
     ALC_USEBITMAP | ALC_NUMERIC | ALC_GESTURE, lpDateDelimiter, NULL, DICT_NULL, IWORDLIST_NONE, 
     FIELDEDIT, NULL},

   /* Expense Item #6 */
   { 71, 134, 95, 12,
     ES_LEFT | WS_BORDER | WS_TABSTOP,
     ALC_DEFAULT, NULL, RCO_SUGGEST, DICT_CUSTOM, IWORDLIST_NONE, 
     FIELDEDIT, NULL},

   /* Value #6 */
   { 166, 134, 68, 12,
     ES_MULTILINE | ES_RIGHT | WS_BORDER | WS_TABSTOP,
     ALC_USEBITMAP | ALC_NUMERIC | ALC_GESTURE, lpMoneyDelimter, DICT_NULL, IWORDLIST_NONE, NULL, 
     FIELDEDIT, NULL},

   /* Signature */
   { 47, 153, 100, 20,
     ES_LEFT | WS_BORDER | WS_TABSTOP, 
     ALC_DEFAULT, NULL, NULL, DICT_NULL, IWORDLIST_NONE, 
     FIELDPIC, NULL},

   /* Date Submitted */
   { 194, 161, 40, 12,
     ES_LEFT | WS_BORDER | WS_TABSTOP,
     ALC_USEBITMAP | ALC_NUMERIC | ALC_GESTURE, lpDateDelimiter, NULL, DICT_NULL, IWORDLIST_NONE, 
     FIELDEDIT, NULL},
   };

/*********************************************************************/



/*----------------------------------------------------------
Purpose: Main Windows function
Returns: Exit code
*/
int PASCAL WinMain(
   HANDLE hInstance,       /* Instance handle  */ 
   HANDLE hPrevInstance,   /* Previous instance handle */ 
   LPSTR lpszCommandLine,  /* Command line string */ 
   int cmdShow)            /* ShowWindow flag */ 
   {
   MSG   msg;

   Unused(lpszCommandLine);

   if (!hPrevInstance)
      {
      if (!FInitApp(hInstance))
         {
         return 1;
         }
      }

   /* If running on a Pen Windows system, register this app so all
   ** EDIT controls in dialogs are replaced by HEDIT controls.
   ** (Notice the CONTROL statement in the RC file is "EDIT",
   ** RegisterPenApp will automatically change that control to
   ** an HEDIT.
   */ 

   if ((hPenWin = GetSystemMetrics(SM_PENWINDOWS)) != NULL)
      /* We do this fancy GetProcAddress simply because we don't
      ** know if we're running Pen Windows.
      */ 
      if ( ((FARPROC)RegPenApp = GetProcAddress(hPenWin, "RegisterPenApp"))!= NULL)
         (*RegPenApp)(RPA_DEFAULT, TRUE);

   if (FInitInstance(hInstance, cmdShow))
      {
      while (GetMessage((LPMSG)&msg, NULL, 0, 0) )
         {
         /* Check for menu accelerator message
         */ 
         if (!TranslateAccelerator(hwndMain, hAccel, &msg))
            {
            TranslateMessage((LPMSG)&msg);
            DispatchMessage((LPMSG)&msg);
            }
         }
      }
   else
      {
      /* Close the dictionary DLLs
      */
      CloseUserDictionary();
      CloseCustomDictionary();
      msg.wParam = 0;
      }
   return msg.wParam;
   }



/*----------------------------------------------------------
Purpose: Initialize application data and register window classes
Returns: TRUE if all successful
*/
BOOL FInitApp(HANDLE hInstance)     /* Instance handle */ 
   {
   WNDCLASS wc;
   HCURSOR  hcursor;
   
   hcursor = LoadCursor(NULL, IDC_ARROW);

   /* Register Main window class
   */
   wc.style = 0;
   wc.hCursor = hcursor;
   wc.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(iconExpense));
   wc.lpszMenuName = MAKEINTRESOURCE(menuMain);
   wc.lpszClassName = (LPSTR)szExpenseClass;
   wc.hbrBackground = (HBRUSH)COLOR_WINDOW+1;
   wc.hInstance = hInstance;
   wc.lpfnWndProc = ExpenseWndProc;
   wc.cbClsExtra = 0;
   wc.cbWndExtra = 0;

   return RegisterClass((LPWNDCLASS) &wc);
   }


/*----------------------------------------------------------
Purpose: Initialize data structures; create windows; load dictionary
Returns: TRUE if all successful
*/
BOOL FInitInstance(
   HANDLE hInstance,       /* Instance handle */ 
   int cmdShow)            /* ShowWindow flag */ 
   {
   char  szWordListPath[MAXWORDLISTPATH];
   int   i;
   LONG  lT          = GetDialogBaseUnits();
   int   cxDlgBase   = LOWORD(lT);
   int   cyDlgBase   = HIWORD(lT);
   char  szErrMessage[cERRSIZE];
   UINT	uSavErrMode;

   /* Load the libarary containing the user dictionary procedure
   */

	/* Set error mode so that windows does not show the file not
		found dialog if userdict.dll is not on the path
	*/
	uSavErrMode = SetErrorMode(SEM_NOOPENFILEERRORBOX);

	/* Try to load the userdict.dll */
   if ((hUserDictLib = LoadLibrary(szUserDictLib)) >= 32)
      {
		/*Try getting the entry point and try using it to initialize
		  the dictionary dll
		*/
      if ((((FARPROC)lpdfUserDictProc = GetProcAddress(hUserDictLib, szUserDictProc)) == NULL)
         || !(fUserInit = (*lpdfUserDictProc)(DIRQ_INIT, NULL, NULL, NULL, NULL, NULL)))
			{
			/* Cleanup but go on if either of the above steps failed
			*/
			FreeLibrary(hUserDictLib);
			lpdfUserDictProc = NULL;
			}
		}
	/* Restore the error mode to the original
	*/
	SetErrorMode(uSavErrMode);

   /* Load the libarary containing the custom dictionary procedure
   */
   if ((hCustomDictLib = LoadLibrary(szCustomDictLib)) >= 32)
      {
      if ((((FARPROC)lpdfCustomDictProc = GetProcAddress(hCustomDictLib, szCustomDictProc)) == NULL)
         || !(fCustomInit = (*lpdfCustomDictProc)(DIRQ_INIT, NULL, NULL, 0, NULL, NULL)))
         return FALSE;
      }
   else
      {
      wsprintf (szErrMessage, "Make sure your PATH contains %s", (LPSTR)szCustomDictLib);
      MessageBox (0, szErrMessage, szAppName, MB_OK | MB_ICONEXCLAMATION);
      return FALSE;
      }

   /* Open all the user dictionary word lists if userdict.dll is
		loaded and initialized
   */
	if(fUserInit)
		{
      for (i = 0; i < SIZE_WORDLIST; i++)
         {
         if (GetPrivateProfileString((LPSTR)szAppName, (LPSTR)rgwordlist[i].szProfileString, (LPSTR)rgwordlist[i].szDefault, 
               szWordListPath, sizeof(szWordListPath), (LPSTR)szIniFile))
            {
            if (!((*lpdfUserDictProc)(DIRQ_OPEN, szWordListPath, 
                  &rgwordlist[i].iList, NULL, NULL, NULL)))
               {
               wsprintf (szErrMessage, "Cannot open the word list file %s. Check your %s.", (LPSTR)szWordListPath, (LPSTR)szIniFile);
               MessageBox (0, szErrMessage, szAppName, MB_OK | MB_ICONEXCLAMATION);
               return FALSE;
               }
            }
         }
		}   

   /* Convert dialog units of field and text coordinates
   ** to window coordinates.
   */
   for (i = 0; i < cTexts; i++)                                 
      {
      rgtext[i].x = (rgtext[i].x * cxDlgBase)/4;
      rgtext[i].y = (rgtext[i].y * cyDlgBase)/8;
      }

   for (i = 0; i < cEditFields; i++)
      {
      rgeditfield[i].x = (rgeditfield[i].x * cxDlgBase)/4;
      rgeditfield[i].y = (rgeditfield[i].y * cyDlgBase)/8;
      rgeditfield[i].cx = (rgeditfield[i].cx * cxDlgBase)/4;
      rgeditfield[i].cy = (rgeditfield[i].cy * cyDlgBase)/8;
      }
    

   /* Create Main window
   */
   if (hwndMain = CreateWindow((LPSTR)szExpenseClass, 
      (LPSTR)szExpenseWnd,   
      WS_CLIPCHILDREN | WS_OVERLAPPED | WS_SYSMENU | WS_MINIMIZEBOX,
      CW_USEDEFAULT, 0, (cReportWidth*cxDlgBase)/4, (cReportHeight*cyDlgBase)/8,
      (HWND)NULL,
      (HWND)NULL,
      (HANDLE)hInstance,
      (LPSTR)NULL
      ))
      {
      hAccel = LoadAccelerators (hInstance, IDEXPENSE);

      ShowWindow(hwndMain, cmdShow);
      UpdateWindow(hwndMain);
      }

   return hwndMain != NULL;
   }


/*----------------------------------------------------------
Purpose: Window procedure for main window
Returns: Varies
*/
LRESULT CALLBACK ExpenseWndProc(
   HWND hwnd,        /* Window handle */ 
   UINT message,     /* Message */ 
   WPARAM wParam,    /* Varies */ 
   LPARAM lParam)    /* Varies */ 
   {
   int   i;
   LONG  lRet  = 0L;

   static HWND hwndFocusField = NULL;
   static FARPROC lpfnAboutDlgProc;
   static HANDLE  hInstance;

   switch (message)
      {
   case WM_CREATE:
      /* Create fields
      */
      if (!FCreateReport(hwnd))
         {
         lRet = 1L;  /* Failed */
         break;   
         }

      hInstance = ((LPCREATESTRUCT) lParam)->hInstance;
      lpfnAboutDlgProc = MakeProcInstance (AboutDlgProc, hInstance);

      /* Initialize focus to first edit control
      */
      hwndFocusField = rgeditfield[0].hwnd;
      SetFocus(hwndFocusField);
      break;

   case WM_COMMAND:
      {
      /* Edit control commands
      */
      if (HIWORD(lParam) == EN_SETFOCUS)
         {
         /* Field focus is being set */
         hwndFocusField = LOWORD(lParam);
         break;
         }
      
      /* Menu commands
      */
      switch (wParam)
         {
         case miAbout:
            DialogBox (hInstance, "AboutBox", hwnd, lpfnAboutDlgProc);
            break;

         case miExit:
            DestroyWindow(hwnd);
            break;

         case miClearSig:
            /* Clear the Signature Field
            */
            SendMessage(rgeditfield[SIG_FIELD].hwnd, WM_HEDITCTL, HE_SETINKMODE, (LONG)0L);
            break;

         case miClearAll:
            /* Clear all edit fields
            */
            for (i = 0; i < cEditFields; i++)
               {
               if (rgeditfield[i].hwnd != NULL)
                  if (rgeditfield[i].wFieldType==FIELDPIC)
                     SendMessage(rgeditfield[i].hwnd, WM_HEDITCTL, HE_SETINKMODE, (LONG)0L);
                  else
                     SendMessage(rgeditfield[i].hwnd, WM_SETTEXT, 0, (LONG)(LPSTR)"");
               }
            SetFocus(rgeditfield[0].hwnd);
            break;

         case miNextField:
            /* Focus on the next field
            */ 
            ProcessFieldChange(hwndFocusField, (WORD) chNextField);
            break;

         case miPrecField:
            /* Set Focus on the preceeding field
            */ 
            ProcessFieldChange(hwndFocusField, (WORD) chPrecField);
            break;

         default:
            break;
         }
      break;
      }

   case WM_PAINT:
      {
      PAINTSTRUCT ps;
      HDC         hdc;

      hdc = BeginPaint(hwnd, &ps);
      SetTextColor(hdc, GetSysColor(COLOR_WINDOWTEXT));
      SetBkMode(hdc, TRANSPARENT);

      for (i = 0; i < cTexts; i++)
         {
         PTEXT ptext = &rgtext[i];

         TextOut(hdc, ptext->x, ptext->y, ptext->szDataStr,
            lstrlen(ptext->szDataStr));
         }

      EndPaint(hwnd, &ps);
      break;
      }

   case WM_SETFOCUS:
      SetFocus(hwndFocusField);
      break;

   case WM_DESTROY:
      {
      if (hPenWin)
         {
         /* Unregister this app
         */ 
         if (RegPenApp != NULL)
            (*RegPenApp)(RPA_DEFAULT, FALSE);

         CloseUserDictionary();
         CloseCustomDictionary();
         }

      PostQuitMessage(0);
      break;
      }

   default:
      lRet = DefWindowProc(hwnd, message, wParam, lParam);
      break;
      }

   return lRet;
   }


/*----------------------------------------------------------
Purpose: Create the fields
Returns: TRUE if successful
*/
BOOL FCreateReport(HWND hwndParent)    /* Window handle to main window */
   {
   int   i;
   RC    rcin;
   LONG  lT    = GetDialogBaseUnits();
   int   cx    = LOWORD(lT)/2;
   int   cy    = HIWORD(lT)/2;
   RECTOFS  rectofs;
   
   for (i = 0; i < cEditFields; i++)
      {
      PEDITFIELD peditfield = &rgeditfield[i];
      DWORD dwStyle = WS_CHILD | WS_VISIBLE | (hPenWin ? 0L : WS_BORDER) | peditfield->dwStyle;

      switch (peditfield->wFieldType)
         {
      case FIELDPIC:
      case FIELDEDIT:
         /* Create (H)edit window.
         **
         ** If running on a Pen Windows system, this app will have
         ** been registered, so all EDIT controls will be changed to
         ** HEDIT controls.
         */
         peditfield->hwnd = CreateWindow(
               (LPSTR)"edit",
               (LPSTR)NULL,
               dwStyle,
               peditfield->x,
               peditfield->y,
               peditfield->cx,
               peditfield->cy,
               hwndParent, 
               (HMENU)NULL,
               GetWindowWord(hwndParent, GWW_HINSTANCE),
               (LPSTR)NULL);
         
         /* Place control in delayed recognition mode
         */ 
         if (peditfield->wFieldType == FIELDPIC)
            {
            SendMessage(peditfield->hwnd, WM_HEDITCTL, HE_SETINKMODE, (LONG)0L);
            }
         break;

      case FIELDBEDIT:   /* Comb field */
         /* Create Bedit window.
         **
         ** Unlike hedits, we cannot simply create an edit control
         ** and expect Pen Windows to change to a bedit.  So we must
         ** explicitly create a bedit or an edit control depending
         ** on whether Pen Windows is installed.
         */

         peditfield->hwnd = CreateWindow(
               (LPSTR)(hPenWin ? "bedit" : "edit"),
               (LPSTR)NULL,
               dwStyle,
               peditfield->x,
               peditfield->y,
               peditfield->cx,
               peditfield->cy,
               hwndParent, 
               (HMENU)NULL,
               GetWindowWord(hwndParent, GWW_HINSTANCE),
               (LPSTR)NULL);
         break;
      }
         
      if (!peditfield->hwnd)
         {
         continue;
         }

      /* Set RC preferences for this edit control
      */
      if (hPenWin)
         {
         if (SendMessage(peditfield->hwnd, WM_HEDITCTL, HE_GETRC, (LONG)((LPRC)&rcin)))
            {
            rcin.alc = peditfield->alc;
            rcin.lRcOptions = rcin.lRcOptions | peditfield->lRcOptions;
            if (rcin.alc&ALC_USEBITMAP)
               SetAlcBits(rcin.rgbfAlc, peditfield->lpCh);

            switch (peditfield->iDictType)
               {
            case DICT_USER:
               /* To determine when a word list needs to be set in
               ** the User dictionary, the ExpenseDictionaryProc
               ** (subclassed DictionaryProc) is called.  The
               ** dwDictParam is used to signal which edit field
               ** is being used.
               */
					if(fUserInit)
						{
		       (FARPROC)rcin.rglpdf[0] = MakeProcInstance(ExpenseDictionaryProc, GetWindowWord(peditfield->hwnd, GWW_HINSTANCE));
   	            rcin.rglpdf[1] = NULL;
	               rcin.dwDictParam = i;
						}
					else
						{
						rcin.rglpdf[0] = NULL;
	               rcin.dwDictParam = 0L;
						}
               break;

            case DICT_CUSTOM:
               rcin.rglpdf[0] = lpdfCustomDictProc;
               rcin.rglpdf[1] = NULL;
               break;

            default:
               rcin.rglpdf[0] = NULL;
               break;
               }
            SendMessage(peditfield->hwnd, WM_HEDITCTL, HE_SETRC, (LONG)((LPRC)&rcin));
            }

         /* Change default inflation rectangle offset so it is 
         ** half the base dialog unit for each respective axis.
         */
         rectofs.dLeft = -cx;
         rectofs.dTop = -cy;
         rectofs.dRight = cx;
         rectofs.dBottom = cy;
         SendMessage(peditfield->hwnd, WM_HEDITCTL, HE_SETINFLATE, (LONG)((LPRECTOFS)&rectofs));

         /* If no border, put underline in
         */ 
         if (((peditfield->dwStyle & WS_BORDER) == 0) && (peditfield->wFieldType == FIELDEDIT))
            SendMessage(peditfield->hwnd, WM_HEDITCTL, HE_SETUNDERLINE, (LONG)(1));
         }

      }

   return TRUE;
   }

/*----------------------------------------------------------
Purpose: Set the focus on next or previous field
Returns: TRUE if successful
Comment: Direction is based on the value of wParam. wParam can
         be set to chNextField or chPrecField.  The hwndFocusField
         parameter is assigned the value of the newly focused field.
*/
BOOL ProcessFieldChange(
   HWND hwndFocusField,    /* Newly focused field */
   WORD wParam)            /* chNextField or chPrecField */
   {
   int i, inc, iCount;
   WORD wRet = FALSE;
   LONG lInkData;
   
   if ((wParam != chNextField) && (wParam != chPrecField))
      return FALSE;

   inc = wParam ==chPrecField? cEditFields-1 : 1;
   i = (IFromHwnd(hwndFocusField)+inc) %(cEditFields);

   /* Move to next or preceeding field.  If field is in cold
   ** recognition mode, we do not set focus to it, but skip
   ** to the next available field.
   ** 
   ** To determine if an hedit is in cold recognition mode, send
   ** a GETINKHANDLE message to the control.  If the loword of the
   ** return code is NULL, then the control is not in cold recognition
   ** mode, and the focus can be set. If it is in cold mode, skip
   ** over it and check the next field.  iCount is used to break
   ** the loop if all fields are in cold mode.
   */
   iCount=0;
   while (iCount<cEditFields && !wRet)
      {
      if (!LOWORD(SendMessage(rgeditfield[i].hwnd, WM_HEDITCTL, HE_GETINKHANDLE, (LONG)(LPSTR)&lInkData)))
         {
         hwndFocusField = rgeditfield[i].hwnd;
         SetFocus(hwndFocusField);
         wRet = TRUE;
         }
      else
         i = (i+inc) %(cEditFields);      /* Calculate the next field */ 
      iCount++;
      }

   return wRet;
   }

/*----------------------------------------------------------
Purpose: Get the index into the rgfield which corresponds to
         the entry containing parameter hwnd.
Returns: Index into rgfield based on hwnd, 0 if a match is not
         found
*/
int IFromHwnd(HWND hwnd)   /* Window handle of control */
   {
   register int i;
	LPEDITFIELD lpeditfield;

   for (lpeditfield = rgeditfield, i = 0; i < cEditFields; i++, lpeditfield++)
      if (lpeditfield->hwnd == hwnd)
         return i;

   return 0;    /* default on err */ 
   }


/*----------------------------------------------------------
Purpose: Dialog window procedure
Returns: Varies
*/
BOOL CALLBACK AboutDlgProc(
   HWND hDlg,        /* Dialog handle */
   UINT message,     /* Message */
   WPARAM wParam,    /* Varies */
   LPARAM lParam)    /* Varies */
   {
   switch (message)
      {
   case WM_INITDIALOG:
      return TRUE;

   case WM_COMMAND:
      switch (wParam)
         {
         case IDOK:
         case IDCANCEL:
            EndDialog (hDlg, 0);
            return TRUE;
         }
      break;
      }
   return FALSE;
   }


/*----------------------------------------------------------
Purpose: Subclassed DictionaryProc for the User Dictionary.
Returns: Varies
Comment: Determines if a new wordlist needs to be set based
         on the global variable, iLastInUserDict which is
         initialized to DictParam_None.

         Returns whatever is returned from calling the user
         dictionary's DictionaryProc.
*/
int FAR PASCAL ExpenseDictionaryProc(
   int irq,          /* Subfunction number */
   LPVOID lpIn,      /* Input parameter */
   LPVOID lpOut,     /* Output parameter */
   int cbMax,        /* Size of lpOut */
   DWORD lContext,   /* Reserved */
   DWORD lD)         /* Dictionary-specific data */
   {

	/* This call is never made if userdict.dll is not loaded and initialized
		but we do this anyway
	*/
	if(!fUserInit)
		return(0);

   if (iLastInUserDict != (int)lD)
      {
      iLastInUserDict = (int)lD;
      (*lpdfUserDictProc)(DIRQ_SETWORDLISTS, 
                           &rgwordlist[rgeditfield[lD].iWordList].iList,
                           NULL, 1, NULL, NULL);
      }

   return((*lpdfUserDictProc)(irq, lpIn, lpOut, cbMax, lContext, lD));
   }


/*----------------------------------------------------------
Purpose: Set the bits for ALC_USEBITMAP field
Returns: --
*/
VOID SetAlcBits(
   LPBYTE rgb,
   LPSTR lp)
   {
   int ib;

   for (ib=0; ib<32; ib++)
      rgb[ib] = 0;

   for ( ; *lp != 0; lp++)
      {
      BYTE uch = *lp;
      int iByte = uch/8;
      int iBit = uch%8;

      rgb[iByte] |= (BYTE)(1<<iBit);
      }
   }

/*----------------------------------------------------------
Purpose: Free the Custom Dictionary DLL
Returns: --
*/
VOID CloseCustomDictionary(VOID)
   {
   if(fCustomInit)
      (*lpdfCustomDictProc)(DIRQ_CLEANUP, NULL, NULL, NULL, NULL, NULL);

   /* Free the library
   */
   if(hCustomDictLib)
      FreeLibrary(hCustomDictLib);

   fCustomInit = FALSE;
   lpdfCustomDictProc = NULL;
   hCustomDictLib = NULL;
   }


/*----------------------------------------------------------
Purpose: Close all the wordlists opened and free the User
         Dictionary DLL
Returns: --
*/
VOID CloseUserDictionary(VOID)
   {
   if(fUserInit)
      {
      int i;

      /* Close all the wordlists we opened
      */
      for(i = 0; i < SIZE_WORDLIST; ++i)
         {
         unsigned uTemp;

         if(uTemp = rgwordlist[i].iList)
            (*lpdfUserDictProc)(DIRQ_CLOSE, (LPVOID)&uTemp, NULL, NULL, NULL, NULL);
         rgwordlist[i].iList = 0;
         }

      /* Ask the dictionary DLL to do cleanup on our behalf
      */
      (*lpdfUserDictProc)(DIRQ_CLEANUP, NULL, NULL, NULL, NULL, NULL);
      }

   /* Free the library
   */
   if(hUserDictLib)
      FreeLibrary(hUserDictLib);

   fUserInit = FALSE;
   lpdfUserDictProc = NULL;
   hUserDictLib = NULL;
   }
