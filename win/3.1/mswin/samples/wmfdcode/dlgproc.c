/***********************************************************************

  MODULE     : DLGPROC.C

  FUNCTIONS  : WMFRecDlgProc
               EnumRangeDlgProc
               PlayFromListDlgProc
               HeaderDlgProc
               AldusHeaderDlgProc
               ClpHeaderDlgProc
               ListDlgProc
               About

  COMMENTS   :

  HISTORY    : 1/16/91 - created - drc

************************************************************************/

#include "windows.h"
#include "wmfdcode.h"

/***********************************************************************

  FUNCTION   : WMFRecDlgProc

  PARAMETERS : HWND hDlg
               unsigned message
               WORD wParam
               LONG lParam

  PURPOSE    : dialog procedure to handle the user input from the
               dialog box that displays the contents of the metafile
               record.

  CALLS      : WINDOWS
                 lstrcpy
                 GlobalLock
                 GlobalUnlock
                 wsprintf
                 SendDlgItemMessage
                 EndDialog
               APP
                 WaitCursor

  MESSAGES   : WM_INITDIALOG
               WM_COMMAND

  RETURNS    : BOOL

  COMMENTS   :

  HISTORY    : 1/16/91 - created - drc

************************************************************************/

BOOL FAR PASCAL WMFRecDlgProc(hDlg, message, wParam, lParam)
HWND hDlg;
unsigned message;
WORD wParam;
LONG lParam;
{
  int i;
  char szMetaFunction[50];
  HFONT hFont;

  switch (message)
  {
  case WM_INITDIALOG:

      /* font for the parameters listbox */
      hFont = GetStockObject(ANSI_FIXED_FONT);

      /* select that font into the parameter listbox */
      SendDlgItemMessage(hDlg, IDL_PARAMETERS, WM_SETFONT, hFont, (LONG)FALSE);

      /* initialize the controls of the dialog box to reflect the
         contents of the current metafile record */

      /* lookup the metafile function */
      for (i = 0; i < NUMMETAFUNCTIONS; i++) {
        if (MetaRec.rdFunction == MetaFunctions[i].value)
            break;
        }

        /* if not found then it is an unknown record */
        if (MetaRec.rdFunction != MetaFunctions[i].value)
            lstrcpy((LPSTR)szMetaFunction, (LPSTR)"Unknown");
        else
            lstrcpy((LPSTR)szMetaFunction,(LPSTR)MetaFunctions[i].szFuncName);

      /* init the record number */
      SetDlgItemInt(hDlg, IDE_RECNUM, iRecNum, FALSE);

      /* init the size control */
      SetDlgItemInt(hDlg, IDE_RECSIZE, (WORD)MetaRec.rdSize, FALSE);

      /* init the function name control */
      SetDlgItemText(hDlg, IDE_FUNCTION, (LPSTR)szMetaFunction);

      /* check the Hex radio button */
      SendDlgItemMessage(hDlg, IDB_HEX, BM_SETCHECK, TRUE, 0L);

      /* load the parameter listbox with the parameters displayed in hex bytes */
      LoadParameterLB(hDlg, MetaRec.rdSize - 3, IDB_HEX);

      return(TRUE);
      break;
   
   case WM_COMMAND:
      switch(wParam)
        {

        /* this will handle the checking and  unchecking of the three buttons */
        case IDB_HEX:
        case IDB_DEC:
        case IDB_CHAR:
            CheckRadioButton(hDlg, IDB_HEX,  IDB_CHAR, wParam );
            LoadParameterLB(hDlg, MetaRec.rdSize - 3,  wParam);
            break;

        case IDGO:
            /* display the hourglass cursor while metafile is playing */
            WaitCursor(TRUE);

            bPlayItAll = TRUE;
            bEnumRange = FALSE;
            /* fall through with appropriate flags set */

        case IDOK:
            bPlayRec = TRUE;
            /* fall through with appropriate flags set */

        case IDCANCEL:
            EndDialog(hDlg, TRUE);
            return(TRUE);
            break;

        case IDQUITENUM:
            /* quit the enumeration.  Setup Dialogbox to return
               FALSE as this return value is checked in a test
               to end the enumeration */
            EndDialog(hDlg, FALSE);
            return(TRUE);
            break; 

        default:
           return (FALSE);
        }
     break;

  default:
     return(FALSE);
     break;
   }
return (TRUE);
}

/***********************************************************************

  FUNCTION   : EnumRangeDlgProc

  PARAMETERS : HWND hDlg
               unsigned message
               WORD wParam
               LONG lParam

  PURPOSE    : This dialog box lets the user specify whether all records
               or a range are to be played.

  CALLS      : WINDOWS
                 SendDlgItemMessage
                 GetDlgItemInt
                 HIWORD
                 MessageBox
                 SetFocus
                 InvalidateClientRect
                 EndDialog

  MESSAGES   : WM_INITDIALOG
               WM_COMMAND

  RETURNS    : BOOL

  COMMENTS   :

  HISTORY    : 1/16/91 - created - Dennis Crain

************************************************************************/

BOOL FAR PASCAL EnumRangeDlgProc(hDlg, message, wParam, lParam)
HWND hDlg;                                /* window handle of the dialog box */
unsigned message;                         /* type of message                 */
WORD wParam;                              /* message-specific information    */
LONG lParam;
{
    BOOL lpTranslated;
    RECT rect;

    switch (message) {
        case WM_INITDIALOG:
            /* play all of the mf records is the default */
            SendDlgItemMessage(hDlg, IDCB_ALL, BM_SETCHECK, 1, 0L);
            return (TRUE);

        case WM_COMMAND:
            switch (wParam)
                {
                case IDE_FROM:

                    /* if the user elects to play a range of record then
                       turn the play all check off */

                    if (HIWORD(lParam) == EN_CHANGE)
                        SendDlgItemMessage(hDlg, IDCB_ALL, BM_SETCHECK, 0, 0L);
                    break;

                case IDE_TO:
                    if (HIWORD(lParam) == EN_CHANGE)
                        SendDlgItemMessage(hDlg, IDCB_ALL, BM_SETCHECK, 0, 0L);
                    break;

                case IDOK:

                   /* if a range of records is to be played */

                   if ( !IsDlgButtonChecked(hDlg, IDCB_ALL) ) {

                     /* set the enumerate range flag */
                     bEnumRange = TRUE;

                     /* initialize the play record flag */
                     bPlayRec = FALSE;

                     /* get the range */
                     iStartRange = GetDlgItemInt(hDlg, IDE_FROM, (BOOL FAR *)&lpTranslated, FALSE);

                     /* trap the error where the start value has not been entered */
                     if (!iStartRange) {
                         MessageBox(hWndMain, "Invalid FROM value",
                                    NULL, MB_OK | MB_ICONEXCLAMATION);
                         SetFocus(GetDlgItem(hDlg, IDE_FROM));
                     break;

                     }

                     iEndRange = GetDlgItemInt(hDlg, IDE_TO, (BOOL FAR *)&lpTranslated, FALSE);
                     if (!iEndRange) {
                         MessageBox(hWndMain, "Invalid TO value",
                                    NULL, MB_OK | MB_ICONEXCLAMATION);
                         SetFocus(GetDlgItem(hDlg, IDE_TO));

                        break;
                     }

                   }
                   /* all records are to be played */
                   else {
                     /* set the enumerate range to false */
                     bEnumRange = FALSE;

                     /* initialize the play it all flag - yes this should
                        be false! */
                     bPlayItAll = FALSE;

                     /* init the play record flag */
                     bPlayRec = TRUE;
                   }
                   /* force paint of the client area */
                   GetClientRect(hWndMain, (LPRECT)&rect);
                   InvalidateRect(hWndMain, (LPRECT)&rect, TRUE);

                   EndDialog(hDlg, TRUE);
                   return (TRUE);
                   break;

                case IDCANCEL:
                   /* user didn't really want to play the metafile */
                   bEnumRange = FALSE;
                   bPlayItAll = TRUE;
                   bPlayRec   = FALSE;
                   EndDialog(hDlg, IDCANCEL);
                   return (TRUE);
                   break;

                default:
                   return (FALSE);
                }
        break;
    }
    return (FALSE);                           /* Didn't process a message    */
}

/***********************************************************************

  FUNCTION   : PlayFromListDlgProc

  PARAMETERS : HWND hDlg
               unsigned message
               WORD wParam
               LONG lParam

  PURPOSE    : a means to indicate whether the selected or unselected
               records among the list of metafile records are to be
               played.

  CALLS      : WINDOWS
                 SendDlgItemMessage
                 IsDlgButtonChecked
                 HIWORD

  MESSAGES   : WM_INITDIALOG
               WM_COMMAND

  RETURNS    : BOOL

  COMMENTS   :

  HISTORY    : 1/16/91 - created - Dennis Crain

************************************************************************/

BOOL FAR PASCAL PlayFromListDlgProc(hDlg, message, wParam, lParam)
HWND hDlg;
unsigned message;
WORD wParam;
LONG lParam;
{
    switch (message) {
        case WM_INITDIALOG:
            /* the default is to play the selected records */
            SendDlgItemMessage(hDlg, IDCB_SEL, BM_SETCHECK, 1, 0L);
            return (TRUE);

        case WM_COMMAND:
            switch (wParam)
            {
              case IDOK:
                /* was the play selected or play unselected button checked? */
                if ( IsDlgButtonChecked(hDlg, IDCB_SEL) )
                    bPlaySelList = TRUE;
                else
                    bPlaySelList = FALSE;

                EndDialog(hDlg, TRUE);
                return (TRUE);

              case IDCB_SEL:
                /* show the button click */
                if (HIWORD(lParam) == BN_CLICKED)
                   SendDlgItemMessage(hDlg, IDCB_UNSEL, BM_SETCHECK, 0, 0L);
                break;

              case IDCB_UNSEL:
                /* show the button click */
                if (HIWORD(lParam) == BN_CLICKED)
                   SendDlgItemMessage(hDlg, IDCB_SEL, BM_SETCHECK, 0, 0L);
                break;

              default:
                return (FALSE);

            }
            break;
    }
    return (FALSE);
}

/***********************************************************************

  FUNCTION   : HeaderDlgProc

  PARAMETERS : HWND hDlg
               unsigned message
               WORD wParam
               LONG lParam

  PURPOSE    : show the "standard" metafile header as described in the
               Windows SDK section 9.5.1 of the SDK Reference volume 2

  CALLS      : WINDOWS
                 wsprintf
                 SetDlgItemText
                 EndDialog

  MESSAGES   : WM_INITDIALOG
               WM_COMMAND

  RETURNS    : BOOL

  COMMENTS   : Metafile header format

               WORD    mtType;
               WORD    mtHeaderSize;
               WORD    mtVersion;
               DWORD   mtSize;
               WORD    mtNoObjects;
               DWORD   mtMaxRecord;

               These fields have the following  meanings:

               Field          Definition

               mtType         specifies whether the metafile is in
                              memory or recorded in a disk file.
                              1 == memory 2 == disk

               mtHeaderSize   Specifies the size in words of the metafile
                              header

               mtVersion      Specifies the Windows version number.

               mtSize         Specifies the size in words of the file

               mtNoObjects    Specifies the maximum number of objects that
                              exist in the metafile at the same time

               mtMaxRecord    Specifies the size in words of the largest
                              record in the metafile.

               mtNoParameters Is not used

  HISTORY    : 1/16/91 - created - Dennis Crain

************************************************************************/

BOOL FAR PASCAL HeaderDlgProc(hDlg, message, wParam, lParam)
HWND hDlg;                                /* window handle of the dialog box */
unsigned message;                         /* type of message                 */
WORD wParam;                              /* message-specific information    */
LONG lParam;
{
    char szBuf[30];

    switch (message) {
        case WM_INITDIALOG:
            /* format the Windows version number */
            wsprintf((LPSTR)szBuf, "%x", mfHeader.mtVersion);
            SetDlgItemText(hDlg, IDS_VER, (LPSTR)szBuf);

            /* format the size of the metafile */
            wsprintf((LPSTR)szBuf, "%lu", mfHeader.mtSize * 2L);
            SetDlgItemText(hDlg, IDS_SIZE, (LPSTR)szBuf);

            /* format the maximum numbers of objects that exist
               in the metafile at the same time */
            wsprintf((LPSTR)szBuf, "%d", mfHeader.mtNoObjects);
            SetDlgItemText(hDlg, IDS_OBJECTS, (LPSTR)szBuf);

            /* format the size of the largest record in the metafile */
            wsprintf((LPSTR)szBuf, "%lu", mfHeader.mtMaxRecord);
            SetDlgItemText(hDlg, IDS_MAXREC, (LPSTR)szBuf);
            return (TRUE);

        case WM_COMMAND:
            if (wParam == IDOK) {
                EndDialog(hDlg, TRUE);
                return (TRUE);
            }
            break;
    }
    return (FALSE);
}

/***********************************************************************

  FUNCTION   : AldusHeaderDlgProc

  PARAMETERS : HWND hDlg
               unsigned message
               WORD wParam
               LONG lParam

  PURPOSE    : show the "extended" header of Aldus Placeable Metafiles.

  CALLS      : WINDOWS
                 wsprintf
                 SetDlgItemText
                 EndDialog

  MESSAGES   : WM_INITDIALOG
               WM_COMMAND

  RETURNS    : BOOL

  COMMENTS   : Aldus placeable metafile format

               DWORD    key;
               HANDLE   hmf;
               RECT     bbox;
               WORD     inch;
               DWORD    reserved;
               WORD     checksum;
               char     metafileData[];

               These fields have the following  meanings:

               Field         Definition

               key           Binary key that uniquely identifies this
                             file type.  This must be 0x9AC6CDD7L.

               hmf           Unused;  must be zero.

               bbox          The coordinates of a rectangle that tightly
                             bounds the picture. These coordinates are in
                             metafile units as defined below.

               inch          The number of metafile units to the inch.  To
                             avoid numeric overflow in PageMaker, this value
                             should be less than 1440.

               reserved      A reserved double word.  Must be zero.

               checksum      A checksum of the 10 words that precede it,
                             calculated by XORing zero with these 10 words
                             and putting the result in the checksum field.

               metafileData  The actual content of the Windows metafile
                             retrieved by copying the data returned by
                             GetMetafileBits to the file.  The number of
                             bytes should be equal to the MS-DOS file length
                             minus 22.  The content of a PageMaker placeable
                             metafile  cannot currently exceed 64K (this may
                             have changed in 4.0).

  HISTORY    : 1/16/91 - created - Dennis Crain

************************************************************************/

BOOL FAR PASCAL AldusHeaderDlgProc(hDlg, message, wParam, lParam)
HWND hDlg;                                /* window handle of the dialog box */
unsigned message;                         /* type of message                 */
WORD wParam;                              /* message-specific information    */
LONG lParam;
{
    char szBuf[30];

    switch (message) {
        case WM_INITDIALOG:
            /* format the key */
            wsprintf((LPSTR)szBuf, "%lx", aldusMFHeader.key);
            SetDlgItemText(hDlg, IDS_KEY, (LPSTR)szBuf);

            /* format the x origin of the bounding rectangle */
            wsprintf((LPSTR)szBuf, "%d", aldusMFHeader.bbox.left);
            SetDlgItemText(hDlg, IDS_LEFT, (LPSTR)szBuf);

            /* format the x extent of the bounding rectangle */
            wsprintf((LPSTR)szBuf, "%d", aldusMFHeader.bbox.right);
            SetDlgItemText(hDlg, IDS_RIGHT, (LPSTR)szBuf);

            /* format the y origin of the bounding rectangle */
            wsprintf((LPSTR)szBuf, "%d", aldusMFHeader.bbox.top);
            SetDlgItemText(hDlg, IDS_TOP, (LPSTR)szBuf);

            /* format the y extent of the bounding rectangle */
            wsprintf((LPSTR)szBuf, "%d", aldusMFHeader.bbox.bottom);
            SetDlgItemText(hDlg, IDS_BOT, (LPSTR)szBuf);

            /* format the number of metafile units per inch */
            wsprintf((LPSTR)szBuf, "%d", aldusMFHeader.inch);
            SetDlgItemText(hDlg, IDS_INCH, (LPSTR)szBuf);

            /* format the checksum */
            wsprintf((LPSTR)szBuf, "%x", aldusMFHeader.checksum);
            SetDlgItemText(hDlg, IDS_CHKSUM, (LPSTR)szBuf);

            return (TRUE);

        case WM_COMMAND:                      /* message: received a command */
            if (wParam == IDOK) {
                EndDialog(hDlg, TRUE);        /* Exits the dialog box        */
                return (TRUE);
            }
            break;
    }
    return (FALSE);                           /* Didn't process a message    */
}

/***********************************************************************

  FUNCTION   : ClpHeaderDlgProc

  PARAMETERS : HWND hDlg
               unsigned message
               WORD wParam
               LONG lParam

  PURPOSE    : show the METAFILEPICT associated with the clipboard
               metafile.  This format is described on page 7-52 of
               the Windows SDK Reference Volume 2.

  CALLS      : WINDOWS
                 lstrcpy
                 wsprintf
                 SetDlgItemText
                 EndDialog

  MESSAGES   : WM_INITDIALOG
               WM_COMMAND

  RETURNS    : BOOL

  COMMENTS   : METAFILEPICT format

               int    mm;
               int    xExt;
               int    yExt;
               HANDLE hMF;

               These fields have the following  meanings:

               Field         Definition

               mm            specifies the mapping mode in which the picture
                             is drawn.

               xExt          specifies the size of the metafile picture for
                             all modes except MM_ISOTROPIC and ANISOTROPIC
                             modes. See SDK reference for more info.

               yExt          as above...

               hMF           Identifies a memory metafile.

  HISTORY    : 1/16/91 - created - Dennis Crain

************************************************************************/

BOOL FAR PASCAL ClpHeaderDlgProc(hDlg, message, wParam, lParam)
HWND hDlg;
unsigned message;
WORD wParam;
LONG lParam;
{
    char szBuf[30];

    switch (message) {
        case WM_INITDIALOG:
            /*format the mapping mode */
            lstrcpy((LPSTR)szBuf, (MFP.mm == MM_TEXT)        ? (LPSTR)"MM_TEXT"     :
                                  (MFP.mm == MM_LOMETRIC)    ? (LPSTR)"MM_LOMETRIC" :
                                  (MFP.mm == MM_HIMETRIC)    ? (LPSTR)"MM_HIMETRIC" :
                                  (MFP.mm == MM_LOENGLISH)   ? (LPSTR)"MM_LOENGLISH":
                                  (MFP.mm == MM_HIENGLISH)   ? (LPSTR)"MM_HIENGLISH":
                                  (MFP.mm == MM_TWIPS)       ? (LPSTR)"MM_TWIPS"    :
                                  (MFP.mm == MM_ISOTROPIC)   ? (LPSTR)"MM_ISOTROPIC":
                                  (MFP.mm == MM_ANISOTROPIC) ? (LPSTR)"MM_ANISOTROPIC":
                                  (LPSTR)"UNKOWN");
            SetDlgItemText(hDlg, IDE_MM, (LPSTR)szBuf);

            /* format the xExt */
            wsprintf((LPSTR)szBuf, "%d", MFP.xExt);
            SetDlgItemText(hDlg, IDE_XEXT, (LPSTR)szBuf);

            /* format the yExt */
            wsprintf((LPSTR)szBuf, "%d", MFP.yExt);
            SetDlgItemText(hDlg, IDE_YEXT, (LPSTR)szBuf);

            return (TRUE);

        case WM_COMMAND:
            if (wParam == IDOK) {
                EndDialog(hDlg, TRUE);
                return (TRUE);
            }
            break;
    }
    return (FALSE);
}

/***********************************************************************

  FUNCTION   : ListDlgProc

  PARAMETERS : HWND hDlg
               unsigned message
               WORD wParam
               LONG lParam

  PURPOSE    :

  CALLS      : WINDOWS
                 GetMetaFile
                 GetDC
                 EnumMetaFile
                 MakeProcInstance
                 FreeProcInstance
                 ReleaseDC
                 EndDialog
                 DeleteMetaFile
                 MessageBox
                 SendDlgItemMessage
                 GlobalAlloc
                 GlobalLock
                 DialogBox
               APP
                 PlayIt

  MESSAGES   : WM_INITDIALOG
               WM_COMMAND

  RETURNS    : BOOL

  COMMENTS   :

  HISTORY    :

************************************************************************/

BOOL FAR PASCAL ListDlgProc(hDlg, message, wParam, lParam)
HWND hDlg;
unsigned message;
WORD wParam;
LONG lParam;
{
    HDC hDC;

    CurrenthDlg = hDlg;
    switch (message) {
        case WM_INITDIALOG:
            /* initalize the current record number */
            iRecNum = 0;

            /* if this metafile is not already in memory then get it*/
            if (!bMetaInRam)
              hMF = GetMetaFile( (LPSTR)OpenName);

            /* if hMF is a valid handle */
            if (hMF) {
                hDC = GetDC(hWndMain);

                /* let the enumeration callback function know that we are
                   enumerating the records to a list */
                iEnumAction = ENUMMFLIST;

                lpprocEnumMF = MakeProcInstance ((FARPROC) MetaEnumProc, hInst);
                EnumMetaFile(hDC, hMF, lpprocEnumMF, (LONG) NULL);
                FreeProcInstance ((FARPROC) lpprocEnumMF);
                if (!bMetaInRam)
                  hMF = DeleteMetaFile (hMF);
                ReleaseDC(hWndMain, hDC);
            }
            else
              MessageBox(hWndMain, "GetMetaFile failed",
                        NULL, MB_OK | MB_ICONHAND);
            return (TRUE);

        case WM_COMMAND:
            switch(wParam)
            {
              case IDOK:

              case IDCANCEL:
                 EndDialog(hDlg, TRUE);
                 return(TRUE);
                 break;

              case IDL_PLAY:

                 //get the number of selected items

                 iNumSel = SendDlgItemMessage(hDlg,
                                              IDL_LBREC,
                                              LB_GETSELCOUNT,
                                              0,
                                              0L);

                 //allocate a buffer large enough to save the indexes

                 hSelMem = GlobalAlloc(GHND, iNumSel * sizeof(int));

                 //lock it down and assign a long ptr to it

                 if (hSelMem) {
                    lpSelMem = (int FAR *)GlobalLock(hSelMem);
                    if (!lpSelMem)
                        return(FALSE);
                 }
                 else
                    return(FALSE);

                 //get the actual indexes and put in buffer

                 iLBItemsInBuf = SendDlgItemMessage(hDlg,
                                                  IDL_LBREC,
                                                  LB_GETSELITEMS,
                                                  (WORD)iNumSel,
                                                  (DWORD)lpSelMem);

                 bEnumRange = FALSE;
                 bPlayItAll = FALSE;
                 bPlayList  = TRUE;
                 iCount = 0; //reset index into lpSelMem

                 /*dialog to play selected or unselected records*/

                 lpPlayFromListDlgProc = MakeProcInstance(PlayFromListDlgProc, hInst);
                 DialogBox(hInst,
                           "PLAYWHAT",
                           hDlg,
                           lpPlayFromListDlgProc);

                 FreeProcInstance(lpPlayFromListDlgProc);

                 /* end this dialog prematurely to get on with playing of recs */
                 EndDialog(hDlg, TRUE);

                 /* play the metafile to the appropriate destination */
                 PlayMetaFileToDest(hWndMain, iDestDC);

                 break;

              default:
                 return (FALSE);
            }
            break;

    }
    return (FALSE);
}

/****************************************************************************

    FUNCTION: About(HWND, unsigned, WORD, LONG)

    PURPOSE:  Processes messages for "About" dialog box

    MESSAGES:

        WM_INITDIALOG - initialize dialog box
        WM_COMMAND    - Input received

    COMMENTS:

        No initialization is needed for this particular dialog box, but TRUE
        must be returned to Windows.

        Wait for user to click on "Ok" button, then close the dialog box.

****************************************************************************/

BOOL FAR PASCAL About(hDlg, message, wParam, lParam)
HWND hDlg;                                /* window handle of the dialog box */
unsigned message;                         /* type of message                 */
WORD wParam;                              /* message-specific information    */
LONG lParam;
{
    switch (message) {
        case WM_INITDIALOG:                /* message: initialize dialog box */
            return (TRUE);

        case WM_COMMAND:                      /* message: received a command */
            if (wParam == IDOK                /* "OK" box selected?          */
                || wParam == IDCANCEL) {      /* System menu close command? */
                EndDialog(hDlg, TRUE);        /* Exits the dialog box        */
                return (TRUE);
            }
            break;
    }
    return (FALSE);                           /* Didn't process a message    */
}
