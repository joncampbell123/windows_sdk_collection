/*************************************************************************

      File:  FILE.C

   Purpose:  Contains the file I/O routines and the Dib Information
             dialog box.

 Functions:  HANDLE   OpenDIBFile       (szFilename);
             BOOL     GetFileName       (LPSTR, WORD);
             HANDLE   ReadDIBFile       (int);
             BOOL     MyRead            (int, LPSTR, DWORD);
             int      CheckIfFileExists (char *);
             int      GetDibInfo        (char *, INFOSTRUCT *);
             BOOL     SaveDIBFile       (void);
             HANDLE   WinDibFromBitmap  (HBITMAP, DWORD, WORD, HPALETTE);
             HANDLE   PMDibFromBitmap   (HBITMAP, DWORD, WORD, HPALETTE);
             BOOL     WriteDIB          (LPSTR, HANDLE);
             VOID     ParseCommandLine  (LPSTR);
             HANDLE   GetDIB            (void);
             DWORD PASCAL lwrite        (int, VOID FAR *, DWORD);

  Comments:

   History:   Date     Reason

             6/1/91    Created
             6/27/91   Added Dib Information support
             7/22/91   Added File Save support
             8/8/91    Added Command Line Support
            11/15/91   Added szFilename parm to OpenDIBFile.

*************************************************************************/

#include <windows.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <io.h>
#include <direct.h>
#include <stdlib.h>
#include "commdlg.h"
#include "dib.h"
#include "errors.h"
#include "file.h"
#include "dibview.h"
#include "dlgs.h"
#include "options.h"
#include "child.h"
#include "frame.h"


char szFileName[256];         // Filename of DIB

/*************************************************************************

  Function:  OpenDIBFile (void)

   Purpose:  Prompts user for a filename, opens it, and returns a handle
             to the DIB memory block.

   Returns:  NULL if no file is opened, or an error occurs reading the file
             A valid DIB handle if successful.

  Comments:

   History:   Date     Reason

             6/1/91    Created
            11/15/91   Added szFilename parm.

*************************************************************************/

HANDLE OpenDIBFile (LPSTR szFilename)
{
  HANDLE   hDIB;

  szFilename[0] = '\0';

  if (GetFileName (szFileName, IDS_OPENDLG))
  {
    lstrcpy (szFilename, szFileName);
    hDIB = GetDIB ();
    if (hDIB)
    {
      return hDIB;
    }
  }
  return NULL;
}


/*************************************************************************

  Function:  GetFileName (LPSTR. WORD)

   Purpose:  Prompts user for a filename through the use of a Windows 3.1
             FileOpen common dialog box.

   Returns:  TRUE if a filename is selected.
             FALSE if no filename is selected.

  Comments:  Filename is put into the string passed to the routine.
             If a filename is not selected, NULL is returned.

   History:   Date      Author      Reason

             6/1/91    Created
             6/27/91   Changed OPENFILENAME structure to
                       support customized common dialog box

*************************************************************************/

BOOL GetFileName (LPSTR szFileName, WORD wIDString)
{
   OPENFILENAME   of;
   FARPROC	  fpDlgFunc;	       // far proc to dlg function
   DWORD          flags;
   static char    szTitle[30];         // Dialog Box title
   static char    szTemplate[20];      // Dialog Box template
   static char    szFile[256];         // File name
   static char    szFileTitle[256];    // Title
   static char    szDrive[5];          // Drive
   static char    szDir[256];          // Directory
   static char    szFname[10];         // Filename
   static char    szExt[4];            // Extension
   HANDLE         hDibInfo;            // Handle to extra words
   LPDIBINFO      ptr;                 // pointer to extra words
   char *szFilter[] =                  // Filter
      {
      "Bitmaps",
      "*.bmp;*.dib;*.rle",
      ""
      };


   // Initialize the OPENFILENAME members

   szFile[0] = '\0';

   if (wIDString == IDS_OPENDLG)
     {
     LoadString (hInst, wIDString, szTitle, sizeof (szTitle));
     LoadString (hInst, IDS_FILEOPEN, szTemplate, sizeof (szTemplate));
     fpDlgFunc = (FARPROC) MakeProcInstance(FileOpenHookProc, hInst);
     flags = OFN_ENABLEHOOK | OFN_ENABLETEMPLATE | OFN_HIDEREADONLY;

     }
   else if (wIDString == IDS_SAVEDLG)
     {
     hDibInfo = (HANDLE) GetWindowWord (GetCurrentMDIWnd(), WW_DIB_HINFO);
     ptr = (LPDIBINFO) GlobalLock (hDibInfo);

     LoadString (hInst, wIDString, szTitle, sizeof (szTitle));
     LoadString (hInst, IDS_FILESAVE, szTemplate, sizeof (szTemplate));
     fpDlgFunc = (FARPROC) MakeProcInstance(FileSaveHookProc, hInst);
     flags = OFN_ENABLEHOOK | OFN_ENABLETEMPLATE | OFN_HIDEREADONLY |
             OFN_OVERWRITEPROMPT;
     lstrcpy ((LPSTR) szFileName, ptr->szFileName);
     lstrcpy ((LPSTR) szFile, (LPSTR)ptr->szFileName);
     _splitpath (szFile, szDrive, szDir, szFname, szExt);
     szDirName[0]=0;
     szFile[0]=0;
     lstrcat ((LPSTR)szDirName, (LPSTR) szDrive);
     lstrcat ((LPSTR)szDirName, (LPSTR)szDir);
     lstrcpy((LPSTR)szFile, (LPSTR) szFname);
     lstrcat ((LPSTR)szFile, (LPSTR) szExt);
     GlobalUnlock (hDibInfo);
     }

   of.lStructSize       = sizeof (OPENFILENAME);
   of.hwndOwner         = GetFocus ();
   of.hInstance         = hInst;
   of.lpstrFilter       = szFilter[0];
   of.lpstrCustomFilter = NULL;
   of.nMaxCustFilter    = 0L;
   of.nFilterIndex      = 1L;
   of.lpstrFile         = szFile;
   of.nMaxFile          = sizeof (szFile);
   of.lpstrFileTitle    = szFileTitle;
   of.nMaxFileTitle     = sizeof (szFileTitle);
   of.lpstrInitialDir   = szDirName;
   of.lpstrTitle        = szTitle;
   of.Flags             = flags;
   of.nFileOffset       = 0;
   of.nFileExtension    = 0;
   of.lpstrDefExt       = NULL;
   of.lpfnHook		= (FARHOOK)fpDlgFunc;
   of.lpTemplateName    = szTemplate;

   // Call the GetOpenFilename function

   if (wIDString == IDS_OPENDLG)
    {
    if (GetOpenFileName (&of))
      {
      lstrcpy (szFileName, of.lpstrFile);
      return TRUE;
      }
    else
      return FALSE;
    }
   else
    {
    if (GetSaveFileName (&of))
      {
      lstrcpy (szFileName, of.lpstrFile);
      return TRUE;
      }
    }
}


/*************************************************************************

  Function:  ReadDIBFile (int)

   Purpose:  Reads in the specified DIB file into a global chunk of
             memory.

   Returns:  A handle to a dib (hDIB) if successful.
             NULL if an error occurs.

  Comments:  BITMAPFILEHEADER is stripped off of the DIB.  Everything
             from the end of the BITMAPFILEHEADER structure on is
             returned in the global memory handle.

   History:   Date      Author      Reason

             6/1/91    Created
             6/27/91   Removed PM bitmap conversion routines.
             6/31/91   Removed logic which overallocated memory
                       (to account for bad display drivers).
            11/08/91   Again removed logic which overallocated
                       memory (it had creeped back in!)

*************************************************************************/

HANDLE ReadDIBFile (int hFile)
{
   BITMAPFILEHEADER   bmfHeader;
   DWORD              dwBitsSize;
   HANDLE             hDIB;
   LPSTR              pDIB;


   // get length of DIB in bytes for use when reading

   dwBitsSize = filelength (hFile);

   // Go read the DIB file header and check if it's valid.

   if ((_lread (hFile, (LPSTR) &bmfHeader, sizeof (bmfHeader)) != sizeof (bmfHeader)) ||
        (bmfHeader.bfType != DIB_HEADER_MARKER))
      {
      DIBError (ERR_NOT_DIB);
      return NULL;
      }

   // Allocate memory for DIB

   hDIB = GlobalAlloc (GMEM_MOVEABLE | GMEM_ZEROINIT, dwBitsSize - sizeof(BITMAPFILEHEADER));

   if (hDIB == 0)
     {
     DIBError (ERR_MEMORY);
     return NULL;
     }

   pDIB = GlobalLock (hDIB);

   // Go read the bits.

   if (!MyRead (hFile, pDIB, dwBitsSize - sizeof(BITMAPFILEHEADER)))
      {
      GlobalUnlock (hDIB);
      GlobalFree   (hDIB);
      DIBError (ERR_READ);
      return NULL;
      }


   GlobalUnlock (hDIB);
   return hDIB;
}

/*************************************************************************

  Function:  MyRead (int, LPSTR, DWORD)

   Purpose:  Routine to read files greater than 64K in size.

   Returns:  TRUE if successful.
             FALSE if an error occurs.

  Comments:

   History:   Date     Reason

             6/1/91    Created

*************************************************************************/

BOOL MyRead (int hFile, LPSTR lpBuffer, DWORD dwSize)
{
   char huge *lpInBuf = (char huge *) lpBuffer;
   int       nBytes;


   while (dwSize)
      {
      nBytes = (int) (dwSize > (DWORD) BYTES_PER_READ ? BYTES_PER_READ : 
                                                        LOWORD (dwSize));

      if (_lread (hFile, (LPSTR) lpInBuf, nBytes) != (WORD) nBytes)
         return FALSE;

      dwSize  -= nBytes;
      lpInBuf += nBytes;
      }

   return TRUE;
}

/*************************************************************************

  Function:  FileOpenHookProc (HWND, WORD, WORD, LONG)

   Purpose:  Hook procedure for FileOpen common dialog box.

   Returns:  TRUE if message was processed.
             FALSE if the system needs to process the message.

  Comments:

   History:   Date     Reason

             6/27/91   Created

*************************************************************************/

BOOL FAR PASCAL FileOpenHookProc (HWND hDlg, WORD msg, WORD wParam, LONG lParam)
{
  INFOSTRUCT info;
  FARPROC    lpInfo;

  switch (msg)
    {
    case WM_COMMAND:
        switch (wParam)
          {
          case IDOK:
           getcwd (szDirName, sizeof (szDirName));
           break;

          case IDD_INFO:

            // User has selected the Dib Information button.  Query the
            // text in the edit control, check if it is a valid file, get
            // the file statistics, and then display the dialog box.

            GetDlgItemText (hDlg, edt1, (LPSTR) szFileName, sizeof (szFileName));

            if (!CheckIfFileExists(szFileName))
                {
                DIBError (ERR_OPEN);
                SetFocus (GetDlgItem(hDlg, edt1));
                return FALSE;
                }

            if (!GetDibInfo (szFileName, &info))
                {
                DIBError (ERR_READ);
                return FALSE;
                }

            lpInfo = MakeProcInstance(InfoDlgProc, hInst);
            DialogBoxParam(hInst, "INFO", hDlg, lpInfo,
                          (DWORD) (INFOSTRUCT FAR *) &info);
            FreeProcInstance (lpInfo);
            break;
          }
    break;
    }
  return FALSE;
}

/*************************************************************************

  Function:  CheckIfFileExists (char *)

   Purpose:  Checks to see if the user selected file exists.

   Returns:  TRUE if the file is opened.
             FALSE if the file does not exists.

  Comments:  After checking the file, the file is immediately closed again.

   History:   Date     Reason

             6/27/91   Created

*************************************************************************/

int CheckIfFileExists (char * szFileName)
{
  int      hFile;
  OFSTRUCT ofstruct;

  if (szFileName[0] == '\0')
    return FALSE;

  hFile = OpenFile((LPSTR) szFileName, &ofstruct, OF_EXIST);

  if (hFile > 0)
    return TRUE;
  else
    return FALSE;
}


/*************************************************************************

  Function:  GetDibInfo (char *, INFOSTRUCT *)

   Purpose:  Retrieves the INFOSTRUCT specifications about the selected
             file when the "Dib Information..." button is selected.

   Returns:  TRUE if successful
             FALSE if an error occurs.

  Comments:  This routine will handle both Windows and PM bitmaps.
             If an PM bitmap is found, NULL is returned for the
             compression type.

   History:   Date     Reason

             6/27/91   Created

*************************************************************************/

int GetDibInfo (char * szFileName, INFOSTRUCT * info)
{
  BITMAPFILEHEADER   bmfHeader;
  BITMAPINFOHEADER   DIBHeader;
  DWORD              dwHeaderSize;
  int                hFile;
  OFSTRUCT           ofstruct;
  BITMAPCOREHEADER   bmCore;
  long               lFilePos;
  char               szBuffer[20];


  if (!szFileName[0])
    return NULL;

  // fill in filename into structure.

  lstrcpy ((LPSTR) info->szName, (LPSTR) szFileName);

  hFile=OpenFile((LPSTR) szFileName, &ofstruct, OF_READ | OF_SHARE_DENY_WRITE);

  if (hFile == 0)
    return NULL;

  // read the BITMAPFILEHEADER structure and check for BM marker

  if ((_lread (hFile, (LPSTR) &bmfHeader, sizeof (bmfHeader)) != sizeof (bmfHeader)) ||
       (bmfHeader.bfType != DIB_HEADER_MARKER))
     {
     DIBError (ERR_NOT_DIB);
     _lclose(hFile);
     return NULL;
     }

  // Get the current file pointer position, and then read the next
  // DWORD.  This DWORD is the size of the following structure which
  // will determine if it is a PM DIB or Windows DIB.

  if (((lFilePos = tell (hFile)) == -1) ||
      (_lread (hFile, (LPSTR) &dwHeaderSize, sizeof (dwHeaderSize))
        != sizeof (dwHeaderSize)))
     {
     _lclose(hFile);
     return NULL;
     }

  // Back the file pointer up a DWORD so that we can read the information
  // into the correct data structure.

  lseek (hFile, lFilePos, SEEK_SET);


  if (dwHeaderSize == sizeof (BITMAPCOREHEADER))      // PM dib
     {
     _lread (hFile, (LPSTR) &bmCore, sizeof (bmCore));

     LoadString (hInst, IDS_PMBMP, szBuffer, sizeof(szBuffer));
     lstrcpy ((LPSTR)info->szType, (LPSTR) szBuffer);

     info->cbWidth  = bmCore.bcWidth;
     info->cbHeight = bmCore.bcHeight;
     info->cbColors = (DWORD)1L << (bmCore.bcBitCount);
     szBuffer[0]=0;
     lstrcpy ((LPSTR) info->szCompress, (LPSTR) szBuffer);

     }
  else if (dwHeaderSize == sizeof (BITMAPINFOHEADER))  // windows dib
     {
     _lread (hFile, (LPSTR) &DIBHeader, sizeof (DIBHeader));

     LoadString (hInst, IDS_WINBMP, szBuffer, sizeof(szBuffer));
     lstrcpy ((LPSTR)info->szType, (LPSTR) szBuffer);

     info->cbWidth  = DIBHeader.biWidth;
     info->cbHeight = DIBHeader.biHeight;
     info->cbColors = (DWORD)1L << DIBHeader.biBitCount;

     switch (DIBHeader.biCompression)
       {
       case BI_RGB:
          LoadString (hInst, IDS_RGB, szBuffer, sizeof(szBuffer));
          break;

       case BI_RLE4:
          LoadString (hInst, IDS_RLE4, szBuffer, sizeof(szBuffer));
          break;

       case BI_RLE8:
          LoadString (hInst, IDS_RLE8, szBuffer, sizeof(szBuffer));
          break;

       default:
          szBuffer[0]=0;
       }

     lstrcpy ((LPSTR) info->szCompress, (LPSTR) szBuffer);
     }
  else
    {
    DIBError (ERR_NOT_DIB);
    _lclose(hFile);
    return NULL;
    }

  _lclose(hFile);
  return 1;
}


/*************************************************************************

  Function:  InfoDlgProc (HWND, WORD, WORD, LONG)

   Purpose:  Window procedure for the Dib Information dialog box.

   Returns:  TRUE if the message was processed.
             FALSE if the system needs to process the message.

  Comments:

   History:   Date     Reason

             6/27/91   Created

*************************************************************************/

BOOL FAR PASCAL InfoDlgProc (HWND hDlg, WORD message, WORD wParam, LONG lParam)
{
  INFOSTRUCT * pInfo;
  char         szBuffer[20];

  switch (message)
    {
    case WM_INITDIALOG:
        pInfo = (INFOSTRUCT *) lParam;

        // Set the strings into the dialog box.

        SetDlgItemText(hDlg, IDD_NAME, (LPSTR) pInfo->szName);
        SetDlgItemText(hDlg, IDD_FORMAT, (LPSTR) pInfo->szType);
        wvsprintf ((LPSTR)szBuffer, (LPSTR) "%lu", (LPSTR) &pInfo->cbWidth);
        SetDlgItemText(hDlg, IDD_WIDTH, (LPSTR) szBuffer);
        wvsprintf ((LPSTR)szBuffer, (LPSTR) "%lu", (LPSTR) &pInfo->cbHeight);
        SetDlgItemText(hDlg, IDD_HEIGHT, (LPSTR) szBuffer);
        wvsprintf ((LPSTR)szBuffer, (LPSTR) "%lu", (LPSTR) &pInfo->cbColors);
        SetDlgItemText(hDlg, IDD_COLORS, (LPSTR) szBuffer);

        if (pInfo->szCompress[0] == 0)
           ShowWindow(GetDlgItem (hDlg, IDD_COMPHEAD), SW_HIDE);
        else
           SetDlgItemText(hDlg, IDD_COMPRESS, (LPSTR) pInfo->szCompress);

        return (TRUE);

    case WM_COMMAND:
        if (wParam == IDOK || wParam == IDCANCEL)
           {
           EndDialog(hDlg, TRUE);
           return (TRUE);
           }
           break;

    }
  return (FALSE);
}

/*************************************************************************

  Function:  SaveDIBFile (void)

   Purpose:  Prompts user for a filename, opens it, and writes DIB
             out in format specified.

   Returns:  FALSE if no file is opened, or an error occurs writing the file.
             TRUE  if successful

  Comments:

   History:   Date      Reason

             7/8/91    Created
             10/1/91   BugFix -- wasn't freeing allocated memory,
                        Cleaned up code, and added comments.
             10/2/91   Added error checking/reporting

*************************************************************************/

BOOL SaveDIBFile (void)
{
   HANDLE    hDib, hDibInfo;
   LPDIBINFO lpDIBInfo;


      // Get the FileName to save under.

   if (GetFileName (szFileName, IDS_SAVEDLG))
      {
      SetCursor(LoadCursor(NULL, IDC_WAIT));


         // Get all the info on the current DIB Window.

      hDibInfo  = (HANDLE) GetWindowWord (GetCurrentMDIWnd(), WW_DIB_HINFO);
      lpDIBInfo = (LPDIBINFO) GlobalLock (hDibInfo);


         // Convert the DDB to the format wanted.

      if (biStyle == BI_PM)
        hDib = PMDibFromBitmap (lpDIBInfo->hBitmap, 
                                biStyle, 
                                biBits, 
                                lpDIBInfo->hPal);
      else
        hDib = WinDibFromBitmap (lpDIBInfo->hBitmap, 
                                 biStyle, 
                                 biBits, 
                                 lpDIBInfo->hPal);


         // Write out the DIB in the specified format.

      if (!WriteDIB ((LPSTR)szFileName, hDib))
         DIBError (ERR_WRITEDIB);


         // Clean up and return.

      GlobalFree (hDib);
      GlobalUnlock (hDibInfo);
      SetCursor(LoadCursor(NULL, IDC_ARROW));
      return TRUE;
      }

   return FALSE;
}



/*************************************************************************

  Function:  FileSaveHookProc (HWND, WORD, WORD, LONG)

   Purpose:  Hook procedure for FileSave common dialog box.

   Returns:  TRUE if message was processed.
             FALSE if the system needs to process the message.

  Comments:

   History:   Date    Reason

             7/8/91   Created

*************************************************************************/

BOOL FAR PASCAL FileSaveHookProc (HWND hDlg, WORD msg, WORD wParam, LONG lParam)
{
  HANDLE    hDibInfo;
  LPDIBINFO ptr;
  HWND      hGroup;
  RECT      rect, DlgRect;

  switch (msg)
    {
    case WM_INITDIALOG:

        //  Get the memory handle stored in the extra words.
        //  From this, insert the filename, and the file format

        hDibInfo = (HANDLE) GetWindowWord (GetCurrentMDIWnd(), WW_DIB_HINFO);
        ptr = (LPDIBINFO) GlobalLock (hDibInfo);

        switch (ptr->wDIBType)
          {
          case BI_RGB:
            SendDlgItemMessage (hDlg, IDD_RGB,  BM_SETCHECK, 1, 0L);
            break;

          case BI_RLE4:
            SendDlgItemMessage (hDlg, IDD_RLE4, BM_SETCHECK, 1, 0L);
            break;

          case BI_RLE8:
            SendDlgItemMessage (hDlg, IDD_RLE8, BM_SETCHECK, 1, 0L);
            break;

          case BI_PM:
            SendDlgItemMessage (hDlg, IDD_PM,   BM_SETCHECK, 1, 0L);
            break;
          }

        switch (ptr->wDIBBits)
          {
          case 1:
            SendDlgItemMessage (hDlg, IDD_1,   BM_SETCHECK, 1, 0L);
            break;

          case 4:
            SendDlgItemMessage (hDlg, IDD_4,   BM_SETCHECK, 1, 0L);
            break;

          case 8:
            SendDlgItemMessage (hDlg, IDD_8,   BM_SETCHECK, 1, 0L);
            break;

          case 24:
            SendDlgItemMessage (hDlg, IDD_24,  BM_SETCHECK, 1, 0L);
            break;
          }

        GlobalUnlock (hDibInfo);
        break;

    case WM_COMMAND:
        switch (wParam)
          {
          case IDOK:
            if (SendDlgItemMessage (hDlg, IDD_RGB, BM_GETCHECK, 0, 0L))
               biStyle = BI_RGB;

            else if (SendDlgItemMessage (hDlg, IDD_RLE4, BM_GETCHECK, 0, 0L))
               biStyle = BI_RLE4;

            else if (SendDlgItemMessage (hDlg, IDD_RLE8, BM_GETCHECK, 0, 0L))
               biStyle = BI_RLE8;

            else
               biStyle = BI_PM;


            if (SendDlgItemMessage (hDlg, IDD_1, BM_GETCHECK, 0, 0L))
               biBits = 1;

            else if (SendDlgItemMessage (hDlg, IDD_4, BM_GETCHECK, 0, 0L))
               biBits = 4;

            else if (SendDlgItemMessage (hDlg, IDD_8, BM_GETCHECK, 0, 0L))
               biBits = 8;

            else
               biBits = 24;

            break;

          case IDD_FILETYPE:
            hGroup = GetDlgItem (hDlg, IDD_FILETYPEGROUP);
            GetWindowRect (hGroup, &rect);
            GetWindowRect (hDlg, &DlgRect);
            SetWindowPos (hDlg,0, DlgRect.left, DlgRect.top,
                         (DlgRect.right-DlgRect.left),
                          (rect.bottom+(rect.left-DlgRect.left)-DlgRect.top),
                          SWP_NOMOVE | SWP_NOZORDER);
            EnableWindow (GetDlgItem (hDlg, IDD_FILETYPE), 0);
            SetFocus (hGroup);
            break;

          case IDD_RLE4:
            if (SendDlgItemMessage (hDlg, IDD_1, BM_GETCHECK, 0, 0L))
               SendDlgItemMessage (hDlg, IDD_1, BM_SETCHECK, 0,0L);
            if (SendDlgItemMessage (hDlg, IDD_8, BM_GETCHECK, 0, 0L))
               SendDlgItemMessage (hDlg, IDD_8, BM_SETCHECK, 0,0L);
            if (SendDlgItemMessage (hDlg, IDD_24, BM_GETCHECK, 0, 0L))
               SendDlgItemMessage (hDlg, IDD_24, BM_SETCHECK, 0,0L);

            EnableWindow (GetDlgItem(hDlg, IDD_4), 1);
            SendDlgItemMessage (hDlg, IDD_4, BM_SETCHECK, 1, 0L);
            EnableWindow (GetDlgItem(hDlg, IDD_1), 0);
            EnableWindow (GetDlgItem(hDlg, IDD_8), 0);
            EnableWindow (GetDlgItem(hDlg, IDD_24), 0);
            break;

          case IDD_RLE8:
            if (SendDlgItemMessage (hDlg, IDD_1, BM_GETCHECK, 0, 0L))
               SendDlgItemMessage (hDlg, IDD_1, BM_SETCHECK, 0,0L);
            if (SendDlgItemMessage (hDlg, IDD_4, BM_GETCHECK, 0, 0L))
               SendDlgItemMessage (hDlg, IDD_4, BM_SETCHECK, 0,0L);
            if (SendDlgItemMessage (hDlg, IDD_24, BM_GETCHECK, 0, 0L))
               SendDlgItemMessage (hDlg, IDD_24, BM_SETCHECK, 0,0L);

            EnableWindow (GetDlgItem(hDlg, IDD_8), 1);
            SendDlgItemMessage (hDlg, IDD_8, BM_SETCHECK, 1, 0L);
            EnableWindow (GetDlgItem(hDlg, IDD_1), 0);
            EnableWindow (GetDlgItem(hDlg, IDD_4), 0);
            EnableWindow (GetDlgItem(hDlg, IDD_24), 0);
            break;

          case IDD_RGB:
          case IDD_PM:
            EnableWindow (GetDlgItem(hDlg, IDD_1), 1);
            EnableWindow (GetDlgItem(hDlg, IDD_4), 2);
            EnableWindow (GetDlgItem(hDlg, IDD_8), 3);
            EnableWindow (GetDlgItem(hDlg, IDD_24), 1);
            break;
          }
        break;

    default:
      break;
    }
  return FALSE;
}


/****************************************************************************

  FUNCTION   : WinDibFromBitmap()

  PURPOSE    : Will create a global memory block in DIB format that
		 represents the Device-dependent bitmap (DDB) passed in.

		      biStyle -> DIB format     ==  RGB, RLE
		      biBits  -> Bits per pixel ==  1,4,8,24

  RETURNS    : A handle to the DIB

 ****************************************************************************/
HANDLE WinDibFromBitmap (HBITMAP hBitmap, 
                           DWORD dwStyle, 
                            WORD wBits, 
                        HPALETTE hPal)
{
   BITMAP               bm;
   BITMAPINFOHEADER     bi;
   BITMAPINFOHEADER FAR *lpbi;
   DWORD                dwLen;
   HANDLE               hDIB;
   HANDLE               h;
   HDC                  hDC;

   if (!hBitmap)
      return NULL;

   if (hPal == NULL)
      hPal = GetStockObject (DEFAULT_PALETTE);

   GetObject (hBitmap, sizeof (bm), (LPSTR)&bm);

   if (wBits == 0)
      wBits =  bm.bmPlanes * bm.bmBitsPixel;

   if (wBits <= 1)
      wBits = 1;
   else if (wBits <= 4)
      wBits = 4;
   else if (wBits <= 8)
      wBits = 8;
   else
      wBits = 24;

   bi.biSize               = sizeof (BITMAPINFOHEADER);
   bi.biWidth              = bm.bmWidth;
   bi.biHeight             = bm.bmHeight;
   bi.biPlanes             = 1;
   bi.biBitCount           = wBits;
   bi.biCompression        = dwStyle;
   bi.biSizeImage          = 0;
   bi.biXPelsPerMeter      = 0;
   bi.biYPelsPerMeter      = 0;
   bi.biClrUsed            = 0;
   bi.biClrImportant       = 0;

   dwLen  = bi.biSize + PaletteSize ((LPSTR) &bi);
   hDIB = GlobalAlloc(GHND,dwLen);

   if (!hDIB)
      return NULL;

   lpbi   = (VOID FAR *)GlobalLock(hDIB);
   *lpbi  = bi;
   hDC    = GetDC (NULL);
   hPal   = SelectPalette (hDC, hPal, FALSE);
   RealizePalette(hDC);



    /*   call GetDIBits with a NULL lpBits param, so it will calculate the
     *  biSizeImage field for us
     */
   GetDIBits (hDC, 
              hBitmap, 
              0, 
              (WORD) bi.biHeight,
              NULL, 
              (LPBITMAPINFO) lpbi, 
              DIB_RGB_COLORS);

   bi = *lpbi;
   GlobalUnlock(hDIB);

    /* If the driver did not fill in the biSizeImage field, make one up */
   if (bi.biSizeImage == 0)
      {
      bi.biSizeImage = WIDTHBYTES((DWORD)bm.bmWidth * wBits) * bm.bmHeight;

      if (dwStyle != BI_RGB)
         bi.biSizeImage = (bi.biSizeImage * 3) / 2;
      }

    /*   realloc the buffer big enough to hold all the bits */

   dwLen = bi.biSize + PaletteSize((LPSTR)&bi) + bi.biSizeImage;
   if (h = GlobalReAlloc(hDIB,dwLen,0))
      hDIB = h;
   else
      {
      GlobalFree(hDIB);
      hDIB = NULL;
      SelectPalette(hDC,hPal,FALSE);
      ReleaseDC(NULL,hDC);
      return hDIB;
      }

    /*   call GetDIBits with a NON-NULL lpBits param, and actualy get the
     *  bits this time
     */
   lpbi = (VOID FAR *)GlobalLock(hDIB);

   if (GetDIBits( hDC,
         hBitmap,
         0,
         (WORD) bi.biHeight,
         (LPSTR) lpbi + (WORD) lpbi->biSize + PaletteSize((LPSTR) lpbi),
         (LPBITMAPINFO) lpbi, DIB_RGB_COLORS) == 0)
      {
      GlobalUnlock (hDIB);
      hDIB = NULL;
      SelectPalette (hDC, hPal, FALSE);
      ReleaseDC (NULL, hDC);
      return NULL;
      }

   bi = *lpbi;
   GlobalUnlock (hDIB);

   SelectPalette (hDC, hPal, FALSE);
   ReleaseDC (NULL, hDC);
   return hDIB;
}



/****************************************************************************

 FUNCTION   : WriteDIB(LPSTR szFile,HANDLE hdib)

 PURPOSE    : Write a global handle in CF_DIB format to a file.

 RETURNS    : TRUE  - if successful.
		 FALSE - otherwise

 ****************************************************************************/
BOOL WriteDIB (szFile, hdib)
LPSTR szFile;
HANDLE hdib;
{
    BITMAPFILEHEADER	hdr;
    LPBITMAPINFOHEADER  lpbi;
    int                 fh;
    OFSTRUCT            of;

    if (!hdib)
	return FALSE;

    fh = OpenFile (szFile, &of, OF_CREATE|OF_READWRITE);
    if (fh == -1)
	return FALSE;

    lpbi = (VOID FAR *)GlobalLock (hdib);

    /* Fill in the fields of the file header */
    hdr.bfType		= DIB_HEADER_MARKER;
    hdr.bfSize		= GlobalSize (hdib) + sizeof (BITMAPFILEHEADER);
    hdr.bfReserved1     = 0;
    hdr.bfReserved2     = 0;
    hdr.bfOffBits       = (DWORD)sizeof(BITMAPFILEHEADER) + lpbi->biSize +
                          PaletteSize((LPSTR)lpbi);

    /* Write the file header */
    if (!_lwrite (fh, (LPSTR)&hdr, sizeof (BITMAPFILEHEADER)))
      {
      GlobalUnlock (hdib);
      _lclose (fh);
      return FALSE;
      }

    /* Write the DIB header and the bits */
    if (!lwrite (fh, (LPSTR)lpbi, GlobalSize (hdib)))
      {
      GlobalUnlock (hdib);
      _lclose (fh);
      return FALSE;
      }

    GlobalUnlock (hdib);
    _lclose (fh);
    return TRUE;
}

/****************************************************************************

 FUNCTION   : lwrite(int fh, VOID FAR *pv, DWORD ul)

 PURPOSE    : Writes data in steps of 32k till all the data is written.

 RETURNS    : 0 - If write did not proceed correctly.
		 number of bytes written otherwise.

 ****************************************************************************/
DWORD PASCAL lwrite (fh, pv, ul)
int	     fh;
VOID FAR     *pv;
DWORD	     ul;
{
   DWORD     ulT = ul;
   BYTE huge *hp = pv;

   while (ul > BYTES_PER_READ) 
      {
      if (_lwrite(fh, (LPSTR)hp, (WORD)BYTES_PER_READ) != BYTES_PER_READ)
		   return 0;

      ul -= BYTES_PER_READ;
      hp += BYTES_PER_READ;
      }

   if (_lwrite(fh, (LPSTR)hp, (WORD)ul) != (WORD)ul)
      return 0;

   return ulT;
}

/****************************************************************************

 FUNCTION   : PMDibFromBitmap()

 PURPOSE    : Will create a global memory block in DIB format that
		 represents the Device-dependent bitmap (DDB) passed in.

		      biStyle -> DIB format     ==  PM
		      biBits  -> Bits per pixel ==  1,4,8,24

 RETURNS    : A handle to the DIB

 ****************************************************************************/
HANDLE PMDibFromBitmap (hbm, biStyle, biBits, hpal)
HBITMAP      hbm;
DWORD	     biStyle;
WORD	     biBits;
HPALETTE     hpal;
{
    BITMAP               bm;
    BITMAPCOREINFO     bi;
    BITMAPCOREINFO FAR *lpbi;
    DWORD                dwLen;
    HANDLE               hdib;
    HDC                  hdc;
    DWORD                SizeImage;

    if (!hbm)
	return NULL;

    if (hpal == NULL)
        hpal = GetStockObject(DEFAULT_PALETTE);

    GetObject(hbm,sizeof(bm),(LPSTR)&bm);

    if (biBits == 0)
	biBits =  bm.bmPlanes * bm.bmBitsPixel;

    bi.bmciHeader.bcSize               = sizeof(BITMAPCOREHEADER);
    bi.bmciHeader.bcWidth              = bm.bmWidth;
    bi.bmciHeader.bcHeight             = bm.bmHeight;
    bi.bmciHeader.bcPlanes             = 1;
    bi.bmciHeader.bcBitCount           = biBits;

    SizeImage = WIDTHBYTES((DWORD)bm.bmWidth * biBits) * bm.bmHeight;

    dwLen  = bi.bmciHeader.bcSize + PaletteSize((LPSTR)&bi) + SizeImage;

    hdc = GetDC(NULL);
    hpal = SelectPalette(hdc,hpal,FALSE);
	 RealizePalette(hdc);

    hdib = GlobalAlloc(GHND,dwLen);

    if (!hdib)
      {
	SelectPalette(hdc,hpal,FALSE);
	ReleaseDC(NULL,hdc);
	return NULL;
      }

    lpbi = (VOID FAR *)GlobalLock(hdib);

    *lpbi = bi;

    if (GetDIBits( hdc,
		   hbm,
		   0,
		   (WORD)bi.bmciHeader.bcHeight,
		   (LPSTR)lpbi + (WORD)bi.bmciHeader.bcSize + PaletteSize((LPSTR)&bi),
		   (LPBITMAPINFO)(LPBITMAPCOREINFO) lpbi, DIB_RGB_COLORS) == 0)
         {
	 GlobalUnlock(hdib);
	 hdib = NULL;
	 SelectPalette(hdc,hpal,FALSE);
	 ReleaseDC(NULL,hdc);
	 return NULL;
         }

    bi = *lpbi;
    GlobalUnlock(hdib);

    SelectPalette(hdc,hpal,FALSE);
    ReleaseDC(NULL,hdc);
    return hdib;
}



LPSTR SkipCharacter(LPSTR lpSearch,
                    BYTE  cTarget)
{
  BYTE cLook;

  while(cLook=*lpSearch++)
    if(cLook!=cTarget)
      return --lpSearch;

  return NULL;
}


LPSTR FindCharacter(LPSTR lpSearch,
                    BYTE  cTarget)
{
  BYTE cLook;

  while(cLook=*lpSearch++)
    if(cLook==cTarget)
      return --lpSearch;

  return NULL;
}


/*************************************************************************

  Function:  GetSectionName (lpCmdLine)

   Purpose:  Check for command line switch and save it.

   Return:   Pointer to first non-space, non-section name character

  Comments:

   History:   Date     Reason

            11/13/91   Created
            11/15/91   Added code to skip leading spaces.
                       Otherwise would enter infinite loop
                       if >1 file specified on command line.

*************************************************************************/
LPSTR GetSectionName(LPSTR lpCmdLine)
{
  LPSTR lpWork;
  BOOL  bEndOfString = FALSE;
//  LPSTR lpStartHere;


  // Skip any leading spaces.
  while (*lpCmdLine == ' ')
      *lpCmdLine++;
  lpWork = lpCmdLine;

  // Use a switch instead of if, so we can easily add other switches in
  // the future.

//  switch(*lpWork)
//  {
//    case '/':
//    case '-':
//      switch(*(lpWork+1))
//      {  
//         default:
//          break;
//      }
//      break;
//  }
  
  return lpCmdLine;
}




/*************************************************************************

  Function:  ParseCommandLine (lpCmdLine)

   Purpose:  Parse the command line arguments for filenames, and then
             open the DIB'S

   Return:   void

  Comments:

   History:   Date     Reason

             8/08/91   Created
            11/13/91   Added a switch for the section name
                       (Used only for the CT stuff)

*************************************************************************/
void ParseCommandLine (LPSTR lpCmdLine)
{
int  i;
char szBuffer[256];
HANDLE hDIB;

  while (*lpCmdLine != '\0')
    {
    // remove leading spaces & check for section name
    lpCmdLine = GetSectionName(lpCmdLine);

    i=0;
    while ((*lpCmdLine != ' ') && (*lpCmdLine != '\0'))
       szBuffer[i++]=*lpCmdLine++;
    szBuffer[i] = '\0';

    if (CheckIfFileExists (szBuffer))
      {
      lstrcpy ((LPSTR)szFileName, (LPSTR) szBuffer);

      hDIB = GetDIB ();
      if (hDIB)
        OpenDIBWindow(hDIB, szFileName);
      }
    }
  return;

}

/*************************************************************************

  Function:  GetDIB (void)

   Purpose:  Opens dib file and reads into memory.

   Return:   HDIB if successful.
             NULL if an error occurs

  Comments:

   History:   Date     Reason

             8/8/91    Created

*************************************************************************/


HANDLE GetDIB (void)
{

   int      hFile;
   OFSTRUCT ofs;
   HANDLE   hDIB;

   SetCursor(LoadCursor(NULL, IDC_WAIT));

   if ((hFile = OpenFile (szFileName, &ofs, OF_READ)) != -1)
      {
      hDIB = ReadDIBFile (hFile);
      _lclose (hFile);
      SetCursor(LoadCursor(NULL, IDC_ARROW));
      return hDIB;
      }
   else
      DIBError (ERR_FILENOTFOUND);
      SetCursor(LoadCursor(NULL, IDC_ARROW));
      return NULL;
}
