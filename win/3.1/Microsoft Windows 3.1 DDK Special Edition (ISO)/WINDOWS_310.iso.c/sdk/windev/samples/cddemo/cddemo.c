/**************************************************************************
*                                                                         *
*      File:  CDDEMO.C                                                    *
*                                                                         *
*   Purpose:  Contains the main window proc and support routines for      *
*             commond dialogs.                                            *
*                                                                         *
* Functions:  int   PASCAL WinMain(HANDLE, HANDLE, LPSTR, int);           *
*             void  NEAR InitializeStruct(WORD, LPSTR);                   *
*             LPSTR NEAR AllocAndLockMem(HANDLE *, WORD);                 *
*	      long  FAR PASCAL MainWndProc(HWND, UINT, WPARAM, LPARAM);   *							 *
*	      BOOL  FAR PASCAL About(HWND, UINT, WPARAM, LPARAM);	  *
*	      BOOL  FAR PASCAL FileOpenHook(HWND, UINT, WPARAM, LPARAM);  *
*	      BOOL  FAR PASCAL FindHook(HWND, UINT, WPARAM, LPARAM);	  *
*	      BOOL  FAR PASCAL FindReplaceHook(HWND, UINT, WPARAM, LPARAM)*
*                                                                         *
*  Comments:  This program demonstrates the correct way to implement the  *
*             new Common Dialogs API's for Windows 3.1.  It has been      *
*             written to help minimize the time it will take you to       *
*             implement the CD's.  In other words, if you are only        *
*             interested in the GetOpenFileName() and GetSaveFileName()   *
*             CD's, then you can easily "cut and paste" parts of this     *
*             app into your own app.                                      *
*                                                                         *
*                                                                         *
*   History:  Date      Reason                                            *
*             --------  -----------------------------------               *
*                                                                         *
*             10/01/91  Created                                           *
*                                                                         *
**************************************************************************/

#define WIN31

#include "windows.h"  
#include "commdlg.h"
#include "dlgs.h"
#include "cddemo.h"  

/**************************************************************************
*                                                                         *
*  Function:  WinMain(HANDLE, HANDLE, LPSTR, int)                         *
*                                                                         *
*   Purpose:  Contains the main window proc and support routines for      *
*             commond dialogs.                                            *
*                                                                         *
*   Returns:  int                                                         *
*                                                                         *
*  Comments:                                                              *
*                                                                         *
*   History:  Date      Reason                                            *
*             --------  -----------------------------------               *
*                                                                         *
*             10/01/91  Created                                           *
*                                                                         *
**************************************************************************/
int PASCAL WinMain( HANDLE hInstance, HANDLE hPrevInstance, 
                    LPSTR lpCmdLine, int nCmdShow)
{
  MSG msg; 
  WNDCLASS wc; 

  if ( !hPrevInstance) 
    {
    wc.style         = NULL; 
    wc.lpfnWndProc   = MainWndProc;
    wc.cbClsExtra    = 0; 
    wc.cbWndExtra    = 0; 
    wc.hInstance     = hInstance; 
    wc.hIcon         = LoadIcon(hInstance, MAKEINTRESOURCE(2));
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = COLOR_WINDOW+1;
    wc.lpszMenuName  = gszMenuName;
    wc.lpszClassName = gszCommonWClass;
    if ( ! RegisterClass(&wc)) return FALSE;
    }  /* end if */ 

  if (!InitInstance(hInstance, nCmdShow))
     return(FALSE);

  while (GetMessage(&msg,NULL,NULL,NULL)) 
    {
       if ( ghFFRDlg==NULL || !IsDialogMessage(ghFFRDlg, &msg) )
          {
             TranslateMessage(&msg);
             DispatchMessage(&msg);
          }
    }   

  return  (msg.wParam);
} /* end WinMain */

/**************************************************************************
*                                                                         *
*  Function:  FormatFilterString(void)                                    *
*                                                                         *
*   Purpose:  To initialize the gszFilter variable with strings from      *
*             the string table.  This method of initializing gszBuffer    *
*             is necessary to ensure that the strings are contiguous      *
*             in memory--which is what COMMDLG.DLL requires.              *
*                                                                         *
*   Returns:  BOOL  TRUE if successful, FALSE if failure loading string   *
*                                                                         *
*  Comments:  The string loaded from the string table has some wild       *
*             character in it.  This wild character is then replaced      *
*             with NULL.  Note that the wild char can be any unique       *
*             character the developer chooses, and must be included       *
*             as the last character of the string.  A typical string      *
*             might look like "Write Files(*.WRI)|*.WRI|" where | is      *
*             the wild character in this case.  Implementing it this      *
*             way also ensures the string is doubly NULL terminated,      *
*             which is also a requirement of this lovely string.          *
*                                                                         *
*   History:  Date      Reason                                            *
*             --------  -----------------------------------               *
*                                                                         *
*             11/19/91  Created                                           *
*                                                                         *
**************************************************************************/
BOOL NEAR FormatFilterString(void)
{
   WORD  wCtr, wStringLen;
   char  chWildChar;

   *gszFilter=0;

   if (!(wStringLen=LoadString(ghInst, IDS_FILTERSTRING, gszFilter, sizeof(gszFilter))))
      {
         ReportError(IDC_LOADSTRINGFAIL);
         return(FALSE);
      }

   chWildChar = gszFilter[wStringLen-1];    //Grab the wild character

   wCtr = 0;

   while (gszFilter[wCtr])
      {
         if (gszFilter[wCtr]==chWildChar)
            gszFilter[wCtr]=0;
         wCtr++;
      }

   return(TRUE);
}

/**************************************************************************
*                                                                         *
*  Function:  InitializeStruct(WORD, LPSTR)                               *
*                                                                         *
*   Purpose:  To initialize a structure for the current common dialog.    *
*             This routine is called just before the common dialogs       *
*             API is called.                                              *
*                                                                         *
*   Returns:  void                                                        *
*                                                                         *
*  Comments:                                                              *
*                                                                         *
*   History:  Date      Reason                                            *
*             --------  -----------------------------------               *
*                                                                         *
*             10/01/91  Created                                           *
*                                                                         *
**************************************************************************/
void NEAR InitializeStruct(WORD wCommDlgType, LPSTR lpStruct)
{

   LPFOCHUNK           lpFOChunk;
   LPFSCHUNK           lpFSChunk;
   LPFINDREPLACECHUNK  lpFChunk, lpFRChunk;
   LPCOLORSCHUNK       lpColorsChunk;
   LPCHOOSEFONT        lpFontChunk;
   LPPRINTDLG          lpPrintChunk;
   WORD                wCtr;
   HDC                 hDC;
   
   switch (wCommDlgType)
      {
      case IDC_OPENFILE:

         lpFOChunk = (LPFOCHUNK)lpStruct;

         GetWindowsDirectory(gszBuffer, sizeof(gszBuffer));

         FormatFilterString();  //Formats gszFilter with strings

         *(lpFOChunk->szFile)            = 0;
         *(lpFOChunk->szFileTitle)       = 0;
         lpFOChunk->of.lStructSize       = sizeof(OPENFILENAME);
         lpFOChunk->of.hwndOwner         = (HWND)ghWnd;
         lpFOChunk->of.hInstance         = (HANDLE)NULL;
         lpFOChunk->of.lpstrFilter       = gszFilter;
         lpFOChunk->of.lpstrCustomFilter = (LPSTR)NULL;
         lpFOChunk->of.nMaxCustFilter    = 0L;
         lpFOChunk->of.nFilterIndex      = 1L;
         lpFOChunk->of.lpstrFile         = lpFOChunk->szFile;
         lpFOChunk->of.nMaxFile          = (DWORD)sizeof(lpFOChunk->szFile);
         lpFOChunk->of.lpstrFileTitle    = lpFOChunk->szFileTitle;
         lpFOChunk->of.nMaxFileTitle     = MAXFILETITLELEN;
         lpFOChunk->of.lpstrInitialDir   = gszBuffer;
         lpFOChunk->of.lpstrTitle        = (LPSTR)NULL;
         lpFOChunk->of.Flags             = OFN_HIDEREADONLY |
                                           OFN_PATHMUSTEXIST |
                                           OFN_FILEMUSTEXIST |
                                           OFN_SHOWHELP | OFN_ENABLEHOOK;
         lpFOChunk->of.nFileOffset       = 0;
         lpFOChunk->of.nFileExtension    = 0;
         lpFOChunk->of.lpstrDefExt       = (LPSTR)NULL;
         lpFOChunk->of.lCustData         = 0L;
	 lpFOChunk->of.lpfnHook 	 = (FARHOOK)lpfnFileOpenHook;
         lpFOChunk->of.lpTemplateName    = (LPSTR)NULL;

         break;

      case IDC_SAVEFILE:

         lpFSChunk = (LPFSCHUNK)lpStruct;

         GetWindowsDirectory(gszBuffer, sizeof(gszBuffer));

         FormatFilterString();  //Formats gszFilter with strings

         *(lpFSChunk->szFile)            = 0;
         lpFSChunk->of.lStructSize       = sizeof(OPENFILENAME);
         lpFSChunk->of.hwndOwner         = (HWND)ghWnd;
         lpFSChunk->of.hInstance         = (HANDLE)NULL;
         lpFSChunk->of.lpstrFilter       = gszFilter;
         lpFSChunk->of.lpstrCustomFilter = (LPSTR)NULL;
         lpFSChunk->of.nMaxCustFilter    = 0L;
         lpFSChunk->of.nFilterIndex      = 1L;
         lpFSChunk->of.lpstrFile         = lpFSChunk->szFile;
         lpFSChunk->of.nMaxFile          = (DWORD)sizeof(lpFSChunk->szFile);
         lpFSChunk->of.lpstrFileTitle    = lpFSChunk->szFileTitle;
         lpFSChunk->of.nMaxFileTitle     = MAXFILETITLELEN;
         lpFSChunk->of.lpstrInitialDir   = gszBuffer;
         lpFSChunk->of.lpstrTitle        = (LPSTR)NULL;
         lpFSChunk->of.Flags             = OFN_SHOWHELP | OFN_OVERWRITEPROMPT;
         lpFSChunk->of.nFileOffset       = 0;
         lpFSChunk->of.nFileExtension    = 0;
         lpFSChunk->of.lpstrDefExt       = (LPSTR)NULL;
         lpFSChunk->of.lCustData         = 0L;
	 lpFSChunk->of.lpfnHook 	 = (FARHOOK)NULL;
         lpFSChunk->of.lpTemplateName    = (LPSTR)NULL;

         break;

      case IDC_FIND:

         lpFChunk = (LPFINDREPLACECHUNK)lpStruct;

         *(lpFChunk->szFindWhat)=0;
         *(lpFChunk->szReplaceWith)=0;

         lpFChunk->fr.lStructSize      = sizeof(FINDREPLACE);
         lpFChunk->fr.hwndOwner        = ghWnd;
         lpFChunk->fr.hInstance        = (HANDLE)NULL;
         lpFChunk->fr.Flags            = FR_ENABLEHOOK;
         lpFChunk->fr.lpstrFindWhat    = (LPSTR)(lpFChunk->szFindWhat);
         lpFChunk->fr.lpstrReplaceWith = (LPSTR)(lpFChunk->szReplaceWith);
         lpFChunk->fr.wFindWhatLen     = sizeof(lpFChunk->szFindWhat);
         lpFChunk->fr.lCustData        = (LONG)CDN_FIND;
	 lpFChunk->fr.lpfnHook	       = (FARHOOK)lpfnFindHook;
         lpFChunk->fr.lpTemplateName   = (LPSTR)NULL;
         break;

      case IDC_FINDREPLACE:

         lpFRChunk = (LPFINDREPLACECHUNK)lpStruct;

         *(lpFRChunk->szFindWhat)=0;
         *(lpFRChunk->szReplaceWith)=0;

         lpFRChunk->fr.lStructSize      = sizeof(FINDREPLACE);
         lpFRChunk->fr.hwndOwner        = ghWnd;
         lpFRChunk->fr.hInstance        = (HANDLE)NULL;
         lpFRChunk->fr.Flags            = FR_ENABLEHOOK;
         lpFRChunk->fr.lpstrFindWhat    = (LPSTR)(lpFRChunk->szFindWhat);
         lpFRChunk->fr.wFindWhatLen     = sizeof(lpFRChunk->szFindWhat);
         lpFRChunk->fr.lpstrReplaceWith = (LPSTR)(lpFRChunk->szReplaceWith);
         lpFRChunk->fr.wReplaceWithLen  = sizeof(lpFRChunk->szReplaceWith);
         lpFRChunk->fr.lCustData        = (LONG)CDN_FINDREPLACE;
	 lpFRChunk->fr.lpfnHook 	= (FARHOOK)lpfnFindReplaceHook;
         lpFRChunk->fr.lpTemplateName   = (LPSTR)NULL;
         break;
         
      case IDC_COLORS:

         lpColorsChunk = (LPCOLORSCHUNK)lpStruct;

         hDC = GetDC(ghWnd);
         lpColorsChunk->dwCustClrs[0]=GetBkColor(hDC);
         ReleaseDC(ghWnd, hDC);
 
         for (wCtr=1; wCtr<=15; wCtr++)
            lpColorsChunk->dwCustClrs[wCtr]= lpColorsChunk->dwCustClrs[0];

         lpColorsChunk->chsclr.lStructSize    = sizeof(CHOOSECOLOR);  
         lpColorsChunk->chsclr.hwndOwner      = ghWnd;
         lpColorsChunk->chsclr.hInstance      = NULL;
         lpColorsChunk->chsclr.rgbResult      = (DWORD)(lpColorsChunk->dwColor);
         lpColorsChunk->chsclr.lpCustColors   = (LPDWORD)(lpColorsChunk->dwCustClrs);
         lpColorsChunk->chsclr.Flags          = 0;
         lpColorsChunk->chsclr.lCustData      = 0L;
	 lpColorsChunk->chsclr.lpfnHook       = (FARHOOK)NULL;
         lpColorsChunk->chsclr.lpTemplateName = (LPSTR)NULL;
         break;

      case IDC_FONT:

         lpFontChunk = (LPCHOOSEFONT)lpStruct;

         lpFontChunk->lStructSize    = sizeof(CHOOSEFONT);
         lpFontChunk->hwndOwner      = ghWnd;

//The hDC field will be initialized when we return--this avoids passing
//the hDC in or getting the DC twice.
//The LOGFONT field will also be initialized when we get back.
//            lpFontChunk->hDC            = hDC;
//            lpFontChunk->lpLogFont      = &lf;

         lpFontChunk->Flags          = CF_SCREENFONTS | CF_EFFECTS |
                                       CF_INITTOLOGFONTSTRUCT | CF_APPLY;
         lpFontChunk->rgbColors      = RGB(0, 0, 255);
         lpFontChunk->lCustData      = 0L;
	 lpFontChunk->lpfnHook	     = (FARHOOK)NULL;
         lpFontChunk->lpTemplateName = (LPSTR)NULL;
         lpFontChunk->hInstance      = (HANDLE)NULL;
         lpFontChunk->lpszStyle      = (LPSTR)NULL;
         lpFontChunk->nFontType      = SCREEN_FONTTYPE;
         lpFontChunk->nSizeMin       = 0;
         lpFontChunk->nSizeMax       = 0;
         break;

      case IDC_PRINTDLG:
         
         lpPrintChunk = (LPPRINTDLG)lpStruct;

         lpPrintChunk->lStructSize         = sizeof(PRINTDLG);
         lpPrintChunk->hwndOwner           = ghWnd;
         lpPrintChunk->hDevMode            = (HANDLE)NULL;
         lpPrintChunk->hDevNames           = (HANDLE)NULL;
         lpPrintChunk->hDC                 = (HDC)NULL;
         lpPrintChunk->Flags               = PD_RETURNDC;
         lpPrintChunk->nFromPage           = 0;
         lpPrintChunk->nToPage             = 0;
         lpPrintChunk->nMinPage            = 0;
         lpPrintChunk->nMaxPage            = 0;
         lpPrintChunk->nCopies             = 0;
         lpPrintChunk->hInstance           = (HANDLE)NULL;
         lpPrintChunk->lCustData           = 0L;
	 lpPrintChunk->lpfnPrintHook	   = (FARHOOK)NULL;
	 lpPrintChunk->lpfnSetupHook	   = (FARHOOK)NULL;
         lpPrintChunk->lpPrintTemplateName = (LPSTR)NULL;
         lpPrintChunk->lpSetupTemplateName = (LPSTR)NULL;
         lpPrintChunk->hPrintTemplate      = (HANDLE)NULL;
         lpPrintChunk->hSetupTemplate      = (HANDLE)NULL;
         break;
                                           
      default:

         break;

      }

   return;

}

/**************************************************************************
*                                                                         *
*  Function:  AllocAndLockMem(HANDLE *, WORD)                             *
*                                                                         *
*   Purpose:  To allocate and lock a chunk of memory for the CD           *
*             structure.                                                  *
*                                                                         *
*   Returns:  LPSTR                                                       *
*                                                                         *
*  Comments:                                                              *
*                                                                         *
*   History:  Date      Reason                                            *
*             --------  -----------------------------------               *
*                                                                         *
*             10/01/91  Created                                           *
*                                                                         *
**************************************************************************/
LPSTR NEAR AllocAndLockMem(HANDLE *hChunk, WORD wSize)
{
   LPSTR lpChunk;

   *hChunk = GlobalAlloc(GMEM_FIXED, wSize);

   if (*hChunk)
      {
         lpChunk = GlobalLock(*hChunk);
         if (!lpChunk)
            {
               GlobalFree(*hChunk);
               ReportError(IDC_LOCKFAIL);
               lpChunk=NULL;
            }
      }
   else
      {
         ReportError(IDC_ALLOCFAIL);
         lpChunk=NULL;
      }
   return(lpChunk);
}

/**************************************************************************
*                                                                         *
*  Function:  MainWndProc(HWND, UINT, WPARAM, LPARAM)			  *
*                                                                         *
*   Purpose:  Standard main window procedure to process messages          *
*             for the main window.                                        *
*                                                                         *
*   Returns:  void                                                        *
*                                                                         *
*  Comments:  Note the logic for processing of CD Help messages and       *
*             Find/FindReplace notification messages.  This is done       *
*             under the default processing of the main switch(message)    *
*             statement.                                                  *
*                                                                         *
*   History:  Date      Reason                                            *
*             --------  -----------------------------------               *
*                                                                         *
*             10/01/91  Created                                           *
*                                                                         *
**************************************************************************/
long FAR PASCAL MainWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
   FARPROC      lpProcAbout; 
   RECT         rc;
   WORD         wMyMessage;   //Used for default processing of message
   WORD         wSize;
   DWORD        dwLen;
   PAINTSTRUCT  ps;
   static DWORD dwFontColor;  //Color of font if one has been selected
   int          iResult;



   /*******************************************************************
   *                                                                  * 
   *                    FILEOPEN/FILESAVE VARIABLES                   * 
   *                                                                  * 
   *******************************************************************/
   LPFOCHUNK lpFOChunk;    //Pointer to File Open block
   LPFSCHUNK lpFSChunk;    //Pointer to File Save block
   HANDLE    hfoChunk;     //Handle to File Open block of memory
   HANDLE    hfsChunk;     //Handle to File Save block of memory

   /*******************************************************************
   *                                                                  * 
   *                             COLORS VARIABLES                     *
   *                                                                  * 
   *******************************************************************/
   static LPCOLORSCHUNK lpColorsChunk=0;
   static HANDLE        hColorsChunk;


   /*******************************************************************
   *                                                                  * 
   *                             FONTS VARIABLES                      * 
   *                                                                  * 
   *******************************************************************/

   #define DESIREDPOINTSIZE 12

//   HDC            hDC; 
   int          iLogPixsY;
   LOGFONT      lf;
   LPCHOOSEFONT lpFontChunk;
   HANDLE       hFontChunk;
   HFONT        hOldFont;


   /********************************************************************
   *                                                                   * 
   *                             FIND/FINDREPLACE VARIABLES            * 
   *                                                                   * 
   ********************************************************************/
   LPFINDREPLACECHUNK lpFChunk, lpFRChunk;
   static HANDLE      hFChunk, hFRChunk;
   BOOL               bFCase,  bFReverse,  bFWord;
   BOOL               bFRCase, bFRWord;
   LPFINDREPLACE      lpF;
   DWORD              dwFFlags;
   char               szFDirection[MAXFINDDIRECTIONLEN];
   char               szFCase[MAXFINDCASELEN], szFRCase[MAXFINDCASELEN];
   char               szFWord[MAXFINDWORDLEN], szFRWord[MAXFINDWORDLEN];
   static HDC         hDC;


   /*******************************************************************
   *                                                                  * 
   *                             PRINTDLG VARIABLES                   * 
   *                                                                  * 
   *******************************************************************/

   LPPRINTDLG lpPDChunk;
   HANDLE     hPDChunk;
   short      xPage,yPage;

  switch (message)
    {

       case WM_CREATE:

          wFRMsg   = RegisterWindowMessage((LPSTR)FINDMSGSTRING);
          wHelpMsg = RegisterWindowMessage((LPSTR)HELPMSGSTRING);

	  lpfnFileOpenHook    = (FARHOOK)MakeProcInstance(FileOpenHook, ghInst);
	  lpfnFindHook	      = (FARHOOK)MakeProcInstance(FindHook, ghInst);
	  lpfnFindReplaceHook = (FARHOOK)MakeProcInstance(FindReplaceHook, ghInst);

          if (lpfnFileOpenHook==NULL || lpfnFindHook==NULL ||
              lpfnFindReplaceHook==NULL)
                return(FALSE);

//Are we in monochrome land????

          hDC = GetDC ( NULL );
          gbMonochrome = (2 == GetDeviceCaps(hDC, NUMCOLORS));  // Monochrome!
          ReleaseDC(NULL, hDC);

//Load brush for painting GetOpenFileName dlg box

          if (!gbMonochrome)
             {
                HBITMAP    hTempBitmap;

                hTempBitmap=LoadBitmap(ghInst, MAKEINTRESOURCE(1));
                if (hTempBitmap)
                   {
                      ghDlgBrush = CreatePatternBrush(hTempBitmap);
                      DeleteObject (hTempBitmap);
                   }
             }

          break;

       case WM_ERASEBKGND:

          if (ghBkgndBrush)
            {
               GetClientRect(ghWnd, &rc);
               FillRect((HDC)wParam, &rc, ghBkgndBrush);
               break;
            }
          else
            return(DefWindowProc(hWnd,message,wParam,lParam));

       case WM_PAINT:

          if (ghSelectedFont)  //Let's output the string
             {
                hDC=BeginPaint(hWnd, &ps);

                hOldFont=SelectObject(hDC, ghSelectedFont);
                SetBkMode(hDC, TRANSPARENT);
                SetTextColor(hDC, (COLORREF)dwFontColor);
 
                TextOut(hDC, 10, 10, gszFontMsg, 20);
                SelectObject(hDC, hOldFont);

                EndPaint(hWnd, &ps);
                break;
             }
          else
            return(DefWindowProc(hWnd,message,wParam,lParam));

       case WM_COMMAND:

          switch (wParam)
          {
             case IDM_FILEOPEN:

                wSize=sizeof(FOCHUNK);

                if (!(lpFOChunk=(LPFOCHUNK)AllocAndLockMem(&hfoChunk, wSize)))
                   break;

                InitializeStruct(IDC_OPENFILE, (LPSTR)lpFOChunk);

                if ( GetOpenFileName( &(lpFOChunk->of) ) )
                   {
                      HANDLE   hFile;
                      OFSTRUCT ofstruct;

                      hFile=OpenFile(lpFOChunk->of.lpstrFile, &ofstruct, OF_EXIST);
                      if (hFile != -1)
                         MessageBox(hWnd, (LPSTR)gszFOSuccess, gszAppName, MB_OK);
                      else
                         MessageBox(hWnd, (LPSTR)gszFOFailure, gszAppName, MB_OK);

//NOTE!!!  On a closed system (ie, not running on a network) this OpenFile
//         call should NEVER fail.  This because we passed in the 
//         OFN_FILEMUSTEXIST flag to CD.  However, on a network system,
//         there is a *very* small chance that between the time CD's checked
//         for existance of the file and the time the call to OpenFile
//         was made here, someone else on the network has deleted the file.
//         MORAL: ALWAYS, ALWAYS, ALWAYS check the return code from your
//         call to OpenFile() or _lopen.

                   }
                else
                   {
                      ProcessCDError(CommDlgExtendedError());
                   }

                GlobalUnlock ( hfoChunk );
                GlobalFree   ( hfoChunk );

                break;

             case IDM_FILESAVE:

                wSize=sizeof(FOCHUNK);

                if (!(lpFSChunk=(LPFOCHUNK)AllocAndLockMem(&hfsChunk, wSize)))
                   break;

                InitializeStruct(IDC_SAVEFILE, (LPSTR)lpFSChunk);

                if ( GetSaveFileName( &(lpFSChunk->of) ) )
                   {
                      wsprintf(gszBuffer, "Save file as: %s",
                               (LPSTR)lpFSChunk->of.lpstrFile);
                      MessageBox(hWnd, (LPSTR)gszBuffer, gszAppName, MB_OK );
                   }
                else
                   {
                      ProcessCDError(CommDlgExtendedError());
                   }

                GlobalUnlock ( hfsChunk );
                GlobalFree   ( hfsChunk );

                break;

             case IDM_FIND:

                EnableMenuItem(GetMenu(ghWnd), IDM_FIND, MF_GRAYED);

                wSize=sizeof(FINDREPLACECHUNK);

                if (!(lpFChunk=(LPFINDREPLACECHUNK)AllocAndLockMem(&hFChunk, wSize)))
                   break;

                InitializeStruct(IDC_FIND, (LPSTR)lpFChunk);

                if (!(ghFFRDlg=FindText(&(lpFChunk->fr))))
                   {
                      GlobalUnlock ( hFChunk );
                      GlobalFree   ( hFChunk );
                      ghFFRDlg=NULL;
                      EnableMenuItem(GetMenu(ghWnd), IDM_FIND, MF_ENABLED);
                      ProcessCDError(CommDlgExtendedError());
                   }

//Note:  DO NOT call GlobalUnlock() and GlobalFree() here since this is
//       a modeless dialog.  See default processing below.

                break;

             case IDM_FINDANDREPLACE:

                EnableMenuItem(GetMenu(ghWnd), IDM_FINDANDREPLACE, MF_GRAYED);

                wSize=sizeof(FINDREPLACECHUNK);

                if (!(lpFRChunk=(LPFINDREPLACECHUNK)AllocAndLockMem(&hFRChunk, wSize)))
                   break;

                InitializeStruct(IDC_FINDREPLACE, (LPSTR)lpFRChunk);

                if (!(ghFFRDlg=ReplaceText(&(lpFRChunk->fr))))
                   {
                      GlobalUnlock ( hFRChunk );
                      GlobalFree   ( hFRChunk );
                      ghFFRDlg=NULL;
                      EnableMenuItem(GetMenu(ghWnd), IDM_FINDANDREPLACE,
                                     MF_ENABLED);
                      ProcessCDError(CommDlgExtendedError());
                   }

//Note:  DO NOT call GlobalUnlock() and GlobalFree() here since this is
//       a modeless dialog.  See default processing below.

                break;

             case IDM_COLORS:

                if (!lpColorsChunk)   //ie, haven't called colors yet
                                      //so let's initialize everything
                   { 
                      wSize=sizeof(COLORSCHUNK);

                      if (!(lpColorsChunk=(LPCOLORSCHUNK)AllocAndLockMem(&hColorsChunk, wSize)))
                         break;

                      InitializeStruct(IDC_COLORS, (LPSTR)lpColorsChunk);
                   }

                if (ChooseColor( &(lpColorsChunk->chsclr) ))
                   {
                      if (ghBkgndBrush)
                         DeleteObject(ghBkgndBrush);

                      ghBkgndBrush = CreateSolidBrush(lpColorsChunk->chsclr.rgbResult);
                      InvalidateRect(ghWnd, NULL, TRUE); 
                   }
                else
                   {
                      ProcessCDError(CommDlgExtendedError());
                   }

                break;

             case IDM_FONTS:

                wSize=sizeof(CHOOSEFONT);

                if (!(lpFontChunk=(LPCHOOSEFONT)AllocAndLockMem(&hFontChunk, wSize)))
                   break;

                InitializeStruct(IDC_FONT, (LPSTR)lpFontChunk);

//Because we are only getting screen fonts, we can set hDC = NULL
//If printer fonts are desired, a handle to a printer DC must be passed in

                lpFontChunk->hDC       = NULL;

//Now let's initialize the logfont structure.

                hDC = GetDC(ghWnd);
                iLogPixsY = GetDeviceCaps(hDC, LOGPIXELSY);
                ReleaseDC(ghWnd, hDC); 

                lf.lfHeight = -1 * (iLogPixsY * DESIREDPOINTSIZE / 72);
                lf.lfWidth  = 0;
                lf.lfEscapement  = 0;
                lf.lfOrientation = 0;
                lf.lfWeight = 400;
                lf.lfItalic = 0;
                lf.lfUnderline = 0;
                lf.lfStrikeOut = 0;
                lf.lfCharSet    = ANSI_CHARSET;
                lf.lfOutPrecision = OUT_DEFAULT_PRECIS;
                lf.lfClipPrecision = CLIP_DEFAULT_PRECIS;
                lf.lfQuality = DEFAULT_QUALITY;
                lf.lfPitchAndFamily = DEFAULT_PITCH | FF_DONTCARE;
                lstrcpy(lf.lfFaceName, "Modern");

                lpFontChunk->lpLogFont = &lf;

                if ( ChooseFont(lpFontChunk) ) //Now let's create the selected font!
                   {
                      if (ghSelectedFont) DeleteObject(ghSelectedFont);

                      ghSelectedFont = CreateFontIndirect((LPLOGFONT)(lpFontChunk->lpLogFont));
                      dwFontColor=lpFontChunk->rgbColors;
                      InvalidateRect(ghWnd, NULL, TRUE);
                   }
                else
                   {
                      ProcessCDError(CommDlgExtendedError());
                   }

                GlobalUnlock (hFontChunk);
                GlobalFree   (hFontChunk);

                break;

             case IDM_PRINT:

                wSize=sizeof(PRINTDLG);

                if (!(lpPDChunk=(LPPRINTDLG)AllocAndLockMem(&hPDChunk, wSize)))
                   break;

                InitializeStruct(IDC_PRINTDLG, (LPSTR)lpPDChunk);

                if (PrintDlg(lpPDChunk) != 0)
                   {
                      BOOL bError;
                      FARPROC lpfnAbortProc, lpfnPrintDlgProc;

                      gbUserAbort=FALSE;
                      bError=FALSE;

                      lpfnPrintDlgProc=MakeProcInstance(PrintDlgProc, ghInst);
                      ghPrintingDlg=CreateDialog(ghInst, "PRINTING", ghWnd, lpfnPrintDlgProc);
           
                      lpfnAbortProc=MakeProcInstance(AbortProc, ghInst);
                      Escape(lpPDChunk->hDC, SETABORTPROC, 0, (LPSTR)lpfnAbortProc, NULL);

                      if (Escape(lpPDChunk->hDC, STARTDOC, 8, gszTestDoc, NULL) >0 )
                      {
                         xPage=GetDeviceCaps(lpPDChunk->hDC, HORZRES);
                         yPage=GetDeviceCaps(lpPDChunk->hDC, VERTRES);
                         Rectangle(lpPDChunk->hDC, 0, 0, xPage, yPage);
                         MoveTo(lpPDChunk->hDC, 0, 0);
                         LineTo(lpPDChunk->hDC, xPage, yPage);
                         MoveTo(lpPDChunk->hDC, 0, yPage);
                         LineTo(lpPDChunk->hDC, xPage, 0);
                         dwLen=GetTextExtent(lpPDChunk->hDC, gszBuffer, 26);
                         TextOut(lpPDChunk->hDC, (xPage/2)-(LOWORD(dwLen)/2), 50,
                                 gszPrintMsg, 26);

                         Escape(lpPDChunk->hDC, NEWFRAME, 0, NULL, NULL);
                         Escape(lpPDChunk->hDC, ENDDOC, 0, NULL, NULL);

                         DeleteDC(lpPDChunk->hDC);
                         if (lpPDChunk->hDevMode)
                            GlobalFree(lpPDChunk->hDevMode);
                         if (lpPDChunk->hDevNames)
                            GlobalFree(lpPDChunk->hDevNames);
                      }
                      else
                         bError=TRUE;
  
                      if (!gbUserAbort)
                         {  
                            DestroyWindow (ghPrintingDlg) ;
                            ghPrintingDlg = NULL;
                         }  

                      if (bError)
                         MessageBox(ghWnd, "Error while printing",
                                    gszAppName, MB_OK );
                      else
                         {
                            if (gbUserAbort)
                               MessageBox(ghWnd, "Printing Aborted",
                                          gszAppName, MB_OK );
                         }

                      FreeProcInstance(lpfnAbortProc) ;
                      FreeProcInstance(lpfnPrintDlgProc);

                   }
                else
                   {
//Process error.  Be sure to free any memory that may be associated with
//hDevMode or hDevNames.  
                      if (lpPDChunk->hDevMode)
                         GlobalFree(lpPDChunk->hDevMode);
                      if (lpPDChunk->hDevNames)
                         GlobalFree(lpPDChunk->hDevNames);

                      ProcessCDError(CommDlgExtendedError());
                   }

                GlobalUnlock (hPDChunk);
                GlobalFree   (hPDChunk);

                break;

             case IDM_ABOUT:

                lpProcAbout = MakeProcInstance(About, ghInst);

                DialogBox(ghInst,MAKEINTRESOURCE(1),hWnd, lpProcAbout);
                          FreeProcInstance(lpProcAbout);                           

                break;

             case IDM_EXIT:

                PostMessage(hWnd, WM_CLOSE, 0, 0L);
                break;

             default:

                return(DefWindowProc(hWnd,message,wParam,lParam));
          }  

          break;

       case WM_DESTROY:

          if (ghDlgBrush)     DeleteObject(ghDlgBrush);
          if (ghBkgndBrush)   DeleteObject(ghBkgndBrush);
          if (ghSelectedFont) DeleteObject(ghSelectedFont);

          if (lpColorsChunk)  //Clean up memory for colors dialog
             {
                GlobalUnlock (hColorsChunk);
                GlobalFree   (hColorsChunk);
             }

          PostQuitMessage(0);                                                    
          break;

       default:

//Let's keep the logic readable and put it in the switch statement.
//Assign some user defined constants here so a switch can be used.
   
          if ( message == wHelpMsg )  wMyMessage=IDC_HELPMSG;
          if ( message == wFRMsg   )  wMyMessage=IDC_FINDREPLACEMSG;

          switch ( wMyMessage )
             {
                case IDC_HELPMSG:

                   WinHelp (ghWnd, gszWin31wh, HELP_INDEX, 0L );
                   break;

                case IDC_FINDREPLACEMSG:

                   lpF     = (LPFINDREPLACE)lParam;
                   dwFFlags  = lpF->Flags;
                   if ( dwFFlags & FR_DIALOGTERM )
                      {
                         if (lpF->lCustData == CDN_FIND)
                            {
                               GlobalUnlock ( hFChunk );
                               GlobalFree   ( hFChunk );
                               ghFFRDlg=NULL;
                               EnableMenuItem(GetMenu(ghWnd), IDM_FIND,
                                              MF_ENABLED);
                            }  
                         else
                            if (lpF->lCustData == CDN_FINDREPLACE)
                               {
                                  GlobalUnlock ( hFRChunk );
                                  GlobalFree   ( hFRChunk );
                                  ghFFRDlg=NULL;
                                  EnableMenuItem(GetMenu(ghWnd), IDM_FINDANDREPLACE,
                                                 MF_ENABLED);
                            }
                         break;
                      } 

                   if ( lpF->lCustData == CDN_FIND )
                      {
                         bFReverse = (dwFFlags & FR_DOWN      ? FALSE:TRUE );
                         bFCase    = (dwFFlags & FR_MATCHCASE ? TRUE :FALSE);
                         bFWord    = (dwFFlags & FR_WHOLEWORD ? TRUE :FALSE);  
   
                         if ( bFReverse )
                            iResult=LoadString(ghInst, IDS_SEARCHUP,
                                               szFDirection, sizeof(szFDirection));
                         else
                            iResult=LoadString(ghInst, IDS_SEARCHDOWN,
                                               szFDirection, sizeof(szFDirection));
                         if (!iResult)
                            {
                               ReportError(IDC_LOADSTRINGFAIL);
                               break;
                            }

                         if ( bFCase )
                            iResult=LoadString(ghInst, IDS_CASESENSITIVE, szFCase,
                                               sizeof(szFCase));
                         else
                            iResult=LoadString(ghInst, IDS_IGNORECASE, szFCase,
                                               sizeof(szFCase));
                         if (!iResult)
                            {
                               ReportError(IDC_LOADSTRINGFAIL);
                               break;
                            }

                         if ( bFWord )
                            iResult=LoadString(ghInst, IDS_WHOLEWORD, szFWord,
                                       sizeof(szFWord));
                         else
                            iResult=LoadString(ghInst, IDS_WHOLEANDSUB, szFWord,
                                       sizeof(szFWord));
                         if (!iResult)
                            {
                               ReportError(IDC_LOADSTRINGFAIL);
                               break;
                            }

                         wsprintf(gszBuffer, "Find %s, %s, %s, %s",
                                  (LPSTR)(lpF->lpstrFindWhat),
                                  (LPSTR)szFDirection, (LPSTR)szFWord, (LPSTR)szFCase );
                         MessageBox(ghFFRDlg, gszBuffer, gszAppName, MB_OK);  
                         break;
                      }  

                   if ( lpF->lCustData == CDN_FINDREPLACE )
                      {
                         bFRCase    = (dwFFlags & FR_MATCHCASE ? TRUE:FALSE);
                         bFRWord    = (dwFFlags & FR_WHOLEWORD ? TRUE:FALSE);  
   
                         if ( bFRCase )
                            iResult=LoadString(ghInst, IDS_CASESENSITIVE, szFRCase,
                                               sizeof(szFRCase));
                         else
                            iResult=LoadString(ghInst, IDS_IGNORECASE, szFRCase,
                                               sizeof(szFRCase));
                         if (!iResult)
                            {
                               ReportError(IDC_LOADSTRINGFAIL);
                               break;
                            }

                         if ( bFRWord )
                            iResult=LoadString(ghInst, IDS_WHOLEWORD, szFRWord,
                                       sizeof(szFRWord));
                         else
                            iResult=LoadString(ghInst, IDS_WHOLEANDSUB, szFRWord,
                                       sizeof(szFRWord));
                         if (!iResult)
                            {
                               ReportError(IDC_LOADSTRINGFAIL);
                               break;
                            }

                         wsprintf(gszBuffer, "Find %s, Replace with %s, %s, %s",
                                  (LPSTR)(lpF->lpstrFindWhat),
                                  (LPSTR)(lpF->lpstrReplaceWith),(LPSTR)szFRWord,
                                  (LPSTR)szFRCase );
                         MessageBox(ghFFRDlg, gszBuffer, gszAppName, MB_OK);  
                         break;
                      }  

                default:
                   return(DefWindowProc(hWnd,message,wParam,lParam));
             }

          break;
    }  

  return  (NULL);

} /* end MainWndProc */

/**************************************************************************
*                                                                         *
*  Function:  About(HWND, UNINT, WPARAM, LPARAM)			  *
*                                                                         *
*   Purpose:  Standard dialog procedure for the About box.                *
*                                                                         *
*   Returns:  BOOL                                                        *
*                                                                         *
*  Comments:                                                              *
*                                                                         *
*   History:  Date      Reason                                            *
*             --------  -----------------------------------               *
*                                                                         *
*             10/01/91  Created                                           *
*                                                                         *
**************************************************************************/
BOOL FAR PASCAL About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
  switch (message)
    {
       case WM_COMMAND:

          switch(wParam)
             {
                case IDOK:
                   EndDialog(hDlg, TRUE);
                   return  (TRUE);
               
                default:
                   break;
             }
    
          break;
       
       default:
          break;
    }  

  return  (FALSE);
} 

/**************************************************************************
*                                                                         *
*  Function:  FileOpenHook(HWND, UINT, WPARAM, LPARAM)			  *
*                                                                         *
*   Purpose:  This function "hooks" the CD GetOpenFileName() procedure    *
*             and allows you to process any messages you want.  In this   *
*             example, the WM_CTLCOLOR message is being processed to      *
*             give the background of the CD a cool color.                 *
*                                                                         *
*   Returns:  BOOL                                                        *
*                                                                         *
*  Comments:  This function must be exported and must have an instance    *
*             thunk created for it (ie, call MakeProcInstance; see        *
*             WM_CREATE processing in the main window procedure above)    *
*                                                                         *
*   History:  Date      Reason                                            *
*             --------  -----------------------------------               *
*                                                                         *
*             10/01/91  Created                                           *
*                                                                         *
**************************************************************************/
BOOL FAR PASCAL FileOpenHook(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
  switch (message)
    {
       case WM_INITDIALOG:
      
          return  (TRUE);

       case WM_CTLCOLOR:

          if (gbMonochrome || !ghDlgBrush)
             return(FALSE);

          switch (HIWORD(lParam))
             {
                case CTLCOLOR_LISTBOX:   //Don't mess with the listboxes
                   return(FALSE);        

                case CTLCOLOR_DLG:
                   UnrealizeObject(ghDlgBrush);  
                   break;                          

                default:
                   break;
             }

          SelectObject((HDC)wParam, ghDlgBrush);

          if (HIWORD(lParam) == CTLCOLOR_DLG)
            SetBrushOrg((HDC)wParam, 0 , 0);

          SetBkMode    ((HDC)wParam, TRANSPARENT);

          SetTextColor ((HDC)wParam, RGB(0, 0, 0));

          return (ghDlgBrush);

       case WM_COMMAND:
         
          switch(wParam)
             {
                case stc1:
                   break;

                default:
                   break;
             }
          break;

       default:
          break;            
    }  

  return  (FALSE);
}

/**************************************************************************
*                                                                         *
*  Function:  FindHook(HWND, UINT, WPARAM, LPARAM)			  *
*                                                                         *
*   Purpose:  This function "hooks" the CD FindText() procedure           *
*             and allows you to process and messages you want.            *
*             The ONLY reason we are hooking the FindText() CD is to      *
*             keep track of ghFFRDlg.  This allows proper processing      *
*             of IsDialogMessage() in the main message loop.  If your     *
*             app will *not* allow both the FindText() and ReplaceText()  *
*             CD's to be up at the same time, then hooking will not       *
*             be necessary.                                               *
*                                                                         *
*   Returns:  BOOL                                                        *
*                                                                         *
*  Comments:  This function must be exported and must have an instance    *
*             thunk created for it (ie, call MakeProcInstance; see        *
*             WM_CREATE processing in the main window procedure above)    *
*                                                                         *
*   History:  Date      Reason                                            *
*             --------  -----------------------------------               *
*                                                                         *
*             10/01/91  Created                                           *
*                                                                         *
**************************************************************************/
BOOL FAR PASCAL FindHook(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
  switch (message)
    {
       case WM_INITDIALOG:
      
          return  (TRUE);

       case WM_ACTIVATE:

          ghFFRDlg=hDlg;
          break;

       default:
          break;            
    }  

  return  (FALSE);
}

/**************************************************************************
*                                                                         *
*  Function:  FindReplaceHook(HWND, UINT, WPARAM, LPARAM)		  *
*                                                                         *
*   Purpose:  This function "hooks" the CD ReplaceText() procedure        *
*             and allows you to process and messages you want.            *
*             The ONLY reason we are hooking the FindText() CD is to      *
*             keep track of ghFFRDlg.  This allows proper processing      *
*             of IsDialogMessage() in the main message loop.  If your     *
*             app will *not* allow both the FindText() and ReplaceText()  *
*             CD's to be up at the same time, then hooking will not       *
*             be necessary.                                               *
*                                                                         *
*   Returns:  BOOL                                                        *
*                                                                         *
*  Comments:  This function must be exported and must have an instance    *
*             thunk created for it (ie, call MakeProcInstance; see        *
*             WM_CREATE processing in the main window procedure above)    *
*                                                                         *
*   History:  Date      Reason                                            *
*             --------  -----------------------------------               *
*                                                                         *
*             10/01/91  Created                                           *
*                                                                         *
**************************************************************************/
BOOL FAR PASCAL FindReplaceHook(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
  switch (message)
    {
       case WM_INITDIALOG:
      
          return  (TRUE);

       case WM_ACTIVATE:

          ghFFRDlg=hDlg;
          break;

       default:
          break;            
    }  

  return  (FALSE);
}

/**************************************************************************
*                                                                         *
*  Function:  AbortProc (HDC, int)                                        *
*                                                                         *
*   Purpose:  This is the abort procedure for the PrintDlg sample.        *
*                                                                         *
*   Returns:  BOOL                                                        *
*                                                                         *
*  Comments:  This function must be exported and must have an instance    *
*             thunk created for it (ie, call MakeProcInstance)            *
*                                                                         *
*   History:  Date      Reason                                            *
*             --------  -----------------------------------               *
*                                                                         *
*             01/31/92  Created                                           *
*                                                                         *
**************************************************************************/
BOOL FAR PASCAL AbortProc ( HDC hPrinterDC, int nCode )
{
  MSG msg;

  while (!gbUserAbort && PeekMessage (&msg, NULL, 0, 0, PM_REMOVE))
    {
    if (!ghPrintingDlg)
      {
      TranslateMessage ( &msg );
      DispatchMessage ( &msg );
      }
    else
      if (IsWindow(ghPrintingDlg))
        if (!IsDialogMessage (ghPrintingDlg, &msg))
          {
          TranslateMessage ( &msg );
          DispatchMessage ( &msg );
          }
    }

  return !gbUserAbort;
}

/**************************************************************************
*									  *
*  Function:  PrintDlgProc (HWND, UINT, WPARAM, LPARAM) 		  *
*                                                                         *
*   Purpose:  This is the dialog box procedure for the dialog that pops   *
*             up indicating that we are printing.                         *
*                                                                         *
*   Returns:  BOOL                                                        *
*                                                                         *
*  Comments:  This function must be exported and must have an instance    *
*             thunk created for it (ie, call MakeProcInstance)            *
*                                                                         *
*   History:  Date      Reason                                            *
*             --------  -----------------------------------               *
*                                                                         *
*             01/31/92  Created                                           *
*                                                                         *
**************************************************************************/
BOOL FAR PASCAL PrintDlgProc (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
  switch (message)
    {
    case WM_COMMAND:
      
       switch (wParam)
         {
         case IDCANCEL:

            gbUserAbort = TRUE;
            DestroyWindow (hDlg);
            ghPrintingDlg = NULL;
            return TRUE;
        
         default:
          
            return FALSE;
         }
       break;

    default:

       return FALSE;
    }

  return TRUE;
}
