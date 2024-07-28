/***********************************************************************

  MODULE     : WMFMETA.C

  FUNCTIONS  : MetaEnumProc
               GetMetaFileAndEnum
               LoadParameterLB
               PlayMetaFileToDest
               RenderClipMeta
               RenderPlaceableMeta
               SetPlaceableExts
               SetClipMetaExts
               ProcessFile

  COMMENTS   :

************************************************************************/

#include "windows.h"
#include "wmfdcode.h"
#include "stdlib.h"

/***********************************************************************

  FUNCTION   : MetaEnumProc

  PARAMETERS : HDC           hDC
               LPHANDLETABLE lpHTable
               LPMETARECORD  lpMFR
               int           nObj
               BYTE FAR *    lpClientData


  PURPOSE    : callback for EnumMetaFile.  Handles the stepping of
               each metafile record.

  CALLS      : WINDOWS
                 GlobalAlloc
                 GlobalUnlock
                 GlobalFree
                 MakeProcInstance
                 DialogBox
                 FreeProcInstance
                 MessageBox
                 lstrcat
                 SendDlgItemMessage
                 PlayMetaFileRecord


  MESSAGES   : none

  RETURNS    : int

  COMMENTS   : ENUMMFSTEP is used whenever records are to be played,
               regardless of whether you are playing records from the
               list, stepping all, or stepping a range.

               ENUMMFLIST is used when you need to add strings to a listbox
               that describe the type of reocrd.

  HISTORY    : 1/16/91 - created - drc

************************************************************************/

int FAR PASCAL MetaEnumProc(hDC, lpHTable, lpMFR, nObj, lpClientData)
HDC           hDC;
LPHANDLETABLE lpHTable;
LPMETARECORD  lpMFR;
int           nObj;
BYTE FAR *    lpClientData;

{
  WORD i;
  char szMetaFunction[50];
  static BOOL iMapModeSet = FALSE;
  BOOL DlgRet;

  /* what is the enumeration action that we are taking? */
  switch (iEnumAction) {

    /* if the enumeration was entered ala the step metafile menu selection */
    case ENUMMFSTEP:

       /* keep track of the current metafile record number */
       iRecNum++;

       /* allocate memory for the record.  this memory will be used by
          other functions that need to use the contents of the record */
       hMem = GlobalAlloc(GHND, (LONG)2 * lpMFR->rdSize);

       /* if the memory was successfully allocated */
       if (hMem) {

         /* obtain a long pointer to this memory */
	 lpMFParams = (LPPARAMETERS)GlobalLock(hMem);

         /* copy the contents of the record to the global memory */
         MetaRec.rdSize = lpMFR->rdSize;
         MetaRec.rdFunction = lpMFR->rdFunction;
	 for (i = 0; (DWORD)i < lpMFR->rdSize - 3; i++)
           *lpMFParams++ = lpMFR->rdParm[i];

         GlobalUnlock(hMem);

         /* if STEPPING through metafile records that have been selected
            by selecting the menu options Play - Step - All, Play - Step -
            Range, or selecting records from the View - List listbox */

         if ( !bPlayItAll
	    || ( bEnumRange && iRecNum >= (WORD)iStartRange && iRecNum <= (WORD)iEndRange )
	    || ( bPlayList && !bPlayItAll ) )
	 {

            /* if playing records selected from the View - List
               listbox of records */

	    if (bPlayList)  {

                /* if playing the selected records */
		if (bPlaySelList)  {

                    /* if done playing the selected records then stop the
                       enumeration */
		    if (iCount == iNumSel)
		       return(0);

                    /* if this is a selected record then play it */
		    if ((WORD)lpSelMem[iCount] == iRecNum - 1)	{

                        /* initialize flag */
			bPlayRec = FALSE;

                        /* increment the count */
			iCount = (iCount < iLBItemsInBuf) ? iCount++ : iCount;

                        /* call the dialog box that lets you play or ignore this record */

			lpWMFRecDlgProc = MakeProcInstance ((FARPROC) WMFRecDlgProc, hInst);
			DlgRet = DialogBox(hInst, (LPSTR)"WMFDLG", hWndMain, lpWMFRecDlgProc);
			FreeProcInstance ((FARPROC) lpWMFRecDlgProc);
		    }
		    else
                        /* initialize flag and do nothing else */
			bPlayRec = FALSE;
                }
                /* playing the unselected records */
		else  {

                    /* if this is one of the selected records then increment
                       the record count and init a flag but do nothing else */

		    if ((WORD)lpSelMem[iCount] == iRecNum - 1)	{

			/* set count to next selected record in listbox */
			iCount = (iCount < iLBItemsInBuf) ? iCount++ : iCount;
			bPlayRec = FALSE;
		    }

                    /* this is not one of the selected records which is what we
                       want in this case.  So, init a flag give the user the
                       opportunity to play the record */

		    else  {
			bPlayRec = FALSE;
			lpWMFRecDlgProc = MakeProcInstance ((FARPROC) WMFRecDlgProc, hInst);
			DlgRet = DialogBox(hInst, (LPSTR)"WMFDLG", hWndMain, lpWMFRecDlgProc);
			FreeProcInstance ((FARPROC) lpWMFRecDlgProc);
		    }
                }

	    } /* bPlayList */

            /* stepping records from the Play - Step menu option */
	    else  {

               /* init a flag and show the record contents */

	       bPlayRec = FALSE;
	       iCount = (iCount < iLBItemsInBuf) ? iCount++ : iCount;
	       lpWMFRecDlgProc = MakeProcInstance ((FARPROC) WMFRecDlgProc, hInst);
	       DlgRet = DialogBox(hInst, (LPSTR)"WMFDLG", hWndMain, lpWMFRecDlgProc);
	       FreeProcInstance ((FARPROC) lpWMFRecDlgProc);
	    }
         }

         /* bPlayItAll is TRUE.  This is set when the user either
            selects the menu option Play - All or pushes the GO button
            in the view record dialog box */

	 else  {

            /* we were stepping records selected from the listbox and
               the user pressed the GO button

	       Don't bother returning 0 to stop enumeration.  We need to
	       play to the end of the metafile in this case anyway */

            if (bPlayList)  {

              /* we were playing the selected records */
	      if (bPlaySelList)  {

                  /* if all of the selected records have been played then
                     stop the enumeration */
		  if (iCount == iNumSel)
		      return(0);

                  /* set bPlayRec so the record will be played without user
                     interation and then update the record counter */
		  if ((WORD)lpSelMem[iCount] == iRecNum - 1)  {
		      bPlayRec = TRUE;
		      iCount = (iCount < iLBItemsInBuf) ? iCount++ : iCount;
		  }
		  else
                      /* it wasn't one of the selected records so don't play */
		      bPlayRec = FALSE;
	      }
              /* we were playing the unselected records */
	      else  {

                /* if it is a selected record then set bPlayRec to FALSE
                   so the record is not played */

		if ((WORD)lpSelMem[iCount] == iRecNum - 1)	{
		    bPlayRec = FALSE;
		    iCount = (iCount < iLBItemsInBuf) ? iCount++ : iCount;
		}
		else
                    /* play the record */
		    bPlayRec = TRUE;
	      }
	    }
         }


	 /* Stop the enumeration if you were stepping a range and have
            finished playing that range OR the user selected pushed
            the STOP button in the view record dialog box */

	 if ( ((bEnumRange) && (iRecNum > (WORD)iEndRange)) || (!DlgRet) )	{
            bPlayRec = FALSE;
            return(0); //stop enumeration
         }

       } /* hMem */
       else
          /* we were unable to allocate memory for the record */
          MessageBox(hWndMain, "Memory allocation failed",
                     NULL, MB_OK | MB_ICONHAND);

       /* Regardless of the method the user elected to play the
          records, check the flag.  If it is set then play the
          record */

       if (bPlayRec)
          PlayMetaFileRecord(hDC, lpHTable, lpMFR, (WORD)nObj);

       /* done with the record so get rid of it */
       GlobalFree(hMem);

       /* if we made it this far then continue the enumeration */
       return(1);
       break;

    case ENUMMFLIST:
       iRecNum++;

       /* format the listbox string */
       wsprintf((LPSTR)szMetaFunction, (LPSTR)"%d - ", iRecNum);

       /* get the function number contained in the record */
       MetaRec.rdFunction = lpMFR->rdFunction;

       /* lookup the function number in the structure MetaFunctions */

       for (i = 0; i < NUMMETAFUNCTIONS; i++)  {
	 if ((DWORD)lpMFR->rdFunction == MetaFunctions[i].value)
            break;
       }

       /* if the function number is not found then describe this record
          as an "Unknown" type otherwise use the corresponding name
          found in the lookup */

       if (MetaRec.rdFunction != MetaFunctions[i].value)
           lstrcat((LPSTR)szMetaFunction, (LPSTR)"Unknown");
       else
           lstrcat((LPSTR)szMetaFunction,(LPSTR)MetaFunctions[i].szFuncName);

       /* add the string to the listbox */
       SendDlgItemMessage(CurrenthDlg, IDL_LBREC, LB_ADDSTRING, 0,
                          (LONG)(LPSTR)szMetaFunction);

       /* keep enumerating */
       return(1);

       break;
  }
}

/***********************************************************************

  FUNCTION   : GetMetaFileAndEnum

  PARAMETERS : HDC hDC

  PURPOSE    : load the metafile if it has not already been loaded and
               begin enumerating it

  CALLS      : WINDOWS
                 GetMetaFile
                 MakeProcInstance
                 EnumMetaFile
                 FreeProcInstance
                 DeleteMetaFile
                 MessageBox

  MESSAGES   : none

  RETURNS    : void

  COMMENTS   :

  HISTORY    : 1/16/91 - created - drc

************************************************************************/

void GetMetaFileAndEnum(hDC)
HDC hDC;
{
  /* if the metafile has not already been rendered as a placeable
     or clipboard metafile then open it using GDI */

  if (!bMetaInRam)
    hMF = GetMetaFile( (LPSTR)OpenName);

  /* if there is a valid handle to a metafile begin enumerating it */
  if (hMF) {
    iEnumAction = ENUMMFSTEP;
    lpprocEnumMF = MakeProcInstance ((FARPROC) MetaEnumProc, hInst);
    EnumMetaFile(hDC, hMF, lpprocEnumMF, (LPARAM) NULL);
    FreeProcInstance ((FARPROC) lpprocEnumMF);

    /* if this metafile was loaded using GetMetaFile delete it as we
       are done with it now */
    if (!bMetaInRam)
      hMF = DeleteMetaFile (hMF);
  }
  else
    MessageBox(hWndMain, "GetMetaFile failed",
               NULL, MB_OK | MB_ICONHAND);

  return;
}

/***********************************************************************

  FUNCTION   : LoadParameterLB

  PARAMETERS : HWND  hDlg
	       DWORD dwParams
	       int   nRadix - HEX to display contents in base 16
			      DEC to display contents in base 10

  PURPOSE    : display the parameters of the metafile record in
	       the parameter listbox

  CALLS      : WINDOWS
		 GlobalLock
		 GlobalUnlock
		 SendDlgItemMessage
		 wsprintf
		 lstrlen

  MESSAGES   : WM_SETREDRAW
	       WM_RESETCONTENT
	       LB_ADDSTRING

  RETURNS    : BOOL

  COMMENTS   :

  HISTORY    : 1/16/91 - created - drc

************************************************************************/

BOOL LoadParameterLB(hDlg, dwParams, nRadix)
HWND         hDlg;
DWORD        dwParams;
int          nRadix;
{
  DWORD i;
  BYTE nHiByte, nLoByte;
  char szBuffer[8];
  char szDump[100];
  int  iValue;

  switch (nRadix)  /* if nRadix is not a valid value, return FALSE */
  {
    case IDB_HEX:
    case IDB_DEC:
    case IDB_CHAR:
        break;

    default :
        return FALSE;
  }

  /* lock the memory where the parameters can be found */
  if (NULL == (lpMFParams = (LPPARAMETERS)GlobalLock(hMem)))
    return (FALSE);

  /* init the strings */
  *szDump = '\0';

  /* turn off redrawing of the listbox */
  SendDlgItemMessage(hDlg, IDL_PARAMETERS, WM_SETREDRAW, FALSE, 0L);

  /* reset the contents of the listbox */
  SendDlgItemMessage(hDlg, IDL_PARAMETERS, LB_RESETCONTENT, 0, 0L);

  /* loop through the metafile record parameters */
  for (i = 0; i < dwParams; i++)        {

    /* get the high and low byte of the parameter word */
    nHiByte = HIBYTE(lpMFParams[i]);
    nLoByte = LOBYTE(lpMFParams[i]);

    switch (nRadix)
    {
      case IDB_HEX: /* if we are to display as hexadecimal */
           /* format the bytes for the hex part of dump */
           wsprintf((LPSTR)szBuffer, (LPSTR)"%02x %02x ", nLoByte, nHiByte);
           break;

      case IDB_DEC:
           /* format the bytes for the decimal part of dump */
           iValue = lpMFParams[i];
           wsprintf((LPSTR)szBuffer, (LPSTR)"%d ", iValue );
           break;

      case IDB_CHAR:
           wsprintf((LPSTR)szBuffer, (LPSTR)"%c%c",
                    (nLoByte > 0x20) ? nLoByte : 0x2E,
                    (nHiByte > 0x20) ? nHiByte : 0x2E);
           break;

      default :
          return FALSE;
    }


    /* concatenate it onto whatever we have already formatted */
    lstrcat((LPSTR)szDump, (LPSTR)szBuffer);

    /* use every 8 words for hex/dec dump */
    if (!((i + 1) % 8)) {

      /*add the string to the listbox */
      SendDlgItemMessage(hDlg, IDL_PARAMETERS, LB_ADDSTRING, 0, (LONG)(LPSTR)szDump);

      /* re-init the hex/dec strings in preparation for next 8 words */
      *szDump = '\0';
    }
  }

  /* dump any leftover hex/dec dump */
  if (lstrlen((LPSTR)szDump))
    SendDlgItemMessage(hDlg, IDL_PARAMETERS, LB_ADDSTRING, 0, (LONG)(LPSTR)szDump);

 /* enable redraw to the listbox */
 SendDlgItemMessage(hDlg, IDL_PARAMETERS, WM_SETREDRAW, TRUE, 0L);

 /* redraw it */
 InvalidateRect(GetDlgItem(hDlg,IDL_PARAMETERS), NULL, TRUE);

 /* unlock the memory used for the parameters */
 GlobalUnlock(hMem);

 return (TRUE);

}

/***********************************************************************

  FUNCTION   : PlayMetaFileToDest

  PARAMETERS : HWND hWnd
               int  nDest - DC to play metafile to
                 DESTDISPLAY - play to the display
                 DESTMETA    - play into another metafile

  PURPOSE    : begin the enumeration of the metafile to the user selected
               destination.  Perform the housekeeping needs appropriate
               to that destination.

  CALLS      : WINDOWS
                 GetClientRect
                 InvalidateRect
                 GetDC
                 SetMapMode
                 OpenFileDialog
                 MessageBox
                 CreateMetaFile
                 DeleteMetaFile
                 CloseMetaFile

               APP
                 WaitCursor
                 SetClipMetaExts
                 SetPlaceableExts
                 GetMetaFileAndEnum

  MESSAGES   : none

  RETURNS    : int

  COMMENTS   :

  HISTORY    : 1/16/91 - created - drc

************************************************************************/

BOOL PlayMetaFileToDest(hWnd, nDest)
HWND hWnd;
int  nDest;
{
  HDC hDC;
  RECT rect;
  int iSaveRet;

  /* if the file opened contained a valid metafile */
  if (bValidFile) {

    /* init the record count */
    iRecNum = 0;

    /* if we are stepping the metafile then clear the client area */
    if (!bPlayItAll) {
      GetClientRect(hWnd, (LPRECT)&rect);
      InvalidateRect(hWnd, (LPRECT)&rect, TRUE);
    }


    switch (nDest) {

        /* playing metafile to the display */

	case DESTDISPLAY:
            WaitCursor(TRUE);
	    hDC = GetDC(hWnd);

            /* metafile read in from a clipboard file */
            if ( (bMetaInRam) && (!bAldusMeta) )
              SetClipMetaExts(hDC, MFP, WMFDISPLAY);

            /* placeable metafile */
            if (bAldusMeta)
              SetPlaceableExts(hDC, aldusMFHeader, WMFDISPLAY);

            /* "traditional" windows metafile */
            if (!bMetaInRam)
              SetMapMode(hDC, MM_TEXT);

            /* begin the enumeration of the metafile */
            GetMetaFileAndEnum(hDC);

	    ReleaseDC(hWnd, hDC);
            WaitCursor(FALSE);
	    break;

	case DESTMETA:

            /* get a name of a file to play the metafile into */
	    iSaveRet = SaveFileDialog((LPSTR)SaveName);

            /* if the file selected is this metafile then warn user */
            if (!lstrcmp((LPSTR)OpenName, (LPSTR)SaveName))
                MessageBox(hWnd, (LPSTR)"Cannot overwrite the opened metafile!",
                           (LPSTR)"Play to Metafile", MB_OK | MB_ICONEXCLAMATION);

            else

              /* the user didn't hit the cancel button */
	      if (iSaveRet) {
                   WaitCursor(TRUE);

                   /* create a disk based metafile */
	           hDC = CreateMetaFile((LPSTR)SaveName);

                   /* begin the enumeration of the metafile */
                   GetMetaFileAndEnum(hDC);

                   /* done playing so close the metafile and delete the handle */
  	           DeleteMetaFile( CloseMetaFile(hDC) );

                   WaitCursor(FALSE);
              }

	    break;

	default:
	    break;
    }

    /* if playing list records then free the memory used for the list of
       selected records */

    if (bPlayList) {
      GlobalUnlock(hSelMem);
      GlobalFree(hSelMem);
      bPlayList = FALSE;
    }

    /* success */
    return (TRUE);
  }
  else
    /* not a valid metafile */
    return (FALSE);
}

/***********************************************************************

  FUNCTION   : RenderClipMeta

  PARAMETERS : CLIPFILEFORMAT	*ClipHeader
               int	        fh

  PURPOSE    : read metafile bits, metafilepict and metafile header
               of the metafile contained within a clipboard file

  CALLS      : WINDOWS
                 GlobalAlloc
                 GlobalLock
                 GlobalUnlock
                 GlobalFree
                 MessageBox
                 _llseek
                 _lread
                 _lclose
                 SetMetaFileBits

  MESSAGES   : none

  RETURNS    : BOOL

  COMMENTS   :

  HISTORY    : 1/16/91 - created - drc

************************************************************************/

BOOL RenderClipMeta(ClipHeader, fh)
CLIPFILEFORMAT	*ClipHeader;
int	        fh;
{
  HANDLE            hMem;
  LPSTR 	    lpMem;
  HANDLE	    hMFP;
  LPMETAFILEPICT    lpMFP;
  WORD		    wBytesRead;
  WORD              nSize;
  LONG              lOffset;

  /* allocate enough memory to read the metafile bits into */
  if (!(hMem = GlobalAlloc(GHND, ClipHeader->DataLen - sizeof(METAFILEPICT))))
    return(FALSE);

  /* if unable to lock this memory then return */
  if (!(lpMem = GlobalLock(hMem)))  {
    GlobalFree(hMem);
    return(FALSE);
  }

  /* offset to the metafile bits */
  lOffset = ClipHeader->DataOffset + sizeof(METAFILEPICT);

  nSize = (WORD)(ClipHeader->DataLen - sizeof(METAFILEPICT));

  /* seek to the beginning of the metafile bits */
  _llseek(fh, lOffset, 0);

  /* read the metafile bits */
  wBytesRead = _lread(fh, lpMem, nSize);

  /* if unable to read the metafile bits return */
  if( wBytesRead == -1 || wBytesRead < nSize)  {
    GlobalUnlock(hMem);
    GlobalFree(hMem);
    MessageBox(hWndMain, "Unable to read metafile bits",
                     NULL, MB_OK | MB_ICONHAND);
    return(FALSE);
  }

  /* return to beginning to read metafile header */
  _llseek(fh, lOffset, 0);

  /* read the metafile header */
  wBytesRead = _lread(fh, (LPSTR)&mfHeader, sizeof(METAHEADER));

  /* if unable to read the header return */
  if( wBytesRead == -1 || wBytesRead < sizeof(METAHEADER) )  {
    MessageBox(hWndMain, "Unable to read metafile header",
                     NULL, MB_OK | MB_ICONHAND);
    return(FALSE);
  }

  /* set the metafile bits to the memory allocated for that purpose */
  if (NULL == (hMF = SetMetaFileBits(hMem)))  {
    MessageBox(hWndMain, "Unable to set metafile bits",
                     NULL, MB_OK | MB_ICONHAND);

    return(FALSE);
  }

  GlobalUnlock(hMem);

  /* allocate memory for the metafile pict structure */
  if (!(hMFP = GlobalAlloc(GHND, (DWORD)sizeof(METAFILEPICT))))  {
    MessageBox(hWndMain, "Unable allocate memory for metafile pict",
                     NULL, MB_OK | MB_ICONHAND);
    return(FALSE);
  }

  /* lock the memory */
  if (!(lpMFP = (LPMETAFILEPICT)GlobalLock(hMFP)))
    {
      MessageBox(hWndMain, "unable to lock metafile pict memory",
                     NULL, MB_OK | MB_ICONHAND);
      GlobalFree(hMFP);
      return(FALSE);
    }

  /* reposition to the start of the METAFILEPICT header. */
  _llseek(fh, ClipHeader->DataOffset, 0);

  /* read the metafile pict structure */
  wBytesRead = _lread(fh, (LPSTR)&MFP, sizeof(METAFILEPICT));

  /* if unable to read, return */
  if( wBytesRead == -1 || wBytesRead < sizeof(METAFILEPICT) )  {
    MessageBox(hWndMain, "Unable to read metafile pict",
                     NULL, MB_OK | MB_ICONHAND);
    return(FALSE);
  }

  /* update metafile handle */
  MFP.hMF = hMF;

  /* unlock the header */
  GlobalUnlock(hMFP);

  return(TRUE);
}

/***********************************************************************

  FUNCTION   : RenderPlaceableMeta

  PARAMETERS : int fh - filehandle to the placeable metafile

  PURPOSE    : read the metafile bits, metafile header and placeable
               metafile header of a placeable metafile.

  CALLS      : WINDOWS
                 GlobalAlloc
                 GlobalLock
                 Global
                 DeleteMetaFile
                 SetMetaFileBits
                 _llseek
                 _lread
                 _lclose
                 MessageBox


  MESSAGES   : none

  RETURNS    : BOOL

  COMMENTS   :

  HISTORY    : 1/16/91 - created - drc

************************************************************************/

BOOL RenderPlaceableMeta(fh)
int	fh;
{
  HANDLE   hMem;
  LPSTR    lpMem;
  int	   wBytesRead;

  /* if there is currently a metafile loaded, get rid of it */
  if ((bMetaInRam) && (hMF))
    DeleteMetaFile(hMF);

  /* seek to beginning of file and read aldus header */
  _llseek(fh, 0, 0);

  /* read the placeable header */
  wBytesRead = _lread(fh, (LPSTR)&aldusMFHeader, sizeof(ALDUSMFHEADER));

  /* if there is an error, return */
  if( wBytesRead == -1 || wBytesRead < sizeof(ALDUSMFHEADER) )	{
    MessageBox(hWndMain, "Unable to read placeable header",
                     NULL, MB_OK | MB_ICONHAND);
    return(FALSE);
  }

  /* return to read metafile header */
  _llseek(fh, sizeof(aldusMFHeader), 0);

  /* read the metafile header */
  wBytesRead = _lread(fh, (LPSTR)&mfHeader, sizeof(METAHEADER));

  /* if there is an error return */
  if( wBytesRead == -1 || wBytesRead < sizeof(METAHEADER) )  {
    MessageBox(hWndMain, "Unable to read metafile header",
                     NULL, MB_OK | MB_ICONHAND);
    return(FALSE);
  }

  /* allocate memory for the metafile bits */
  if (!(hMem = GlobalAlloc(GHND, (mfHeader.mtSize * 2L))))  {
    MessageBox(hWndMain, "Unable to allocate memory for metafile bits",
                     NULL, MB_OK | MB_ICONHAND);
    return(FALSE);
  }

  /* lock the memory */
  if (!(lpMem = GlobalLock(hMem)))
    {
      MessageBox(hWndMain, "Unable to lock memory for metafile bits",
                     NULL, MB_OK | MB_ICONHAND);
      GlobalFree(hMem);
      return(FALSE);
    }

  /* seek to the metafile bits */
  _llseek(fh, sizeof(aldusMFHeader), 0);

  /* read metafile bits */
  wBytesRead = _lread(fh, lpMem, (WORD)(mfHeader.mtSize * 2));

  /* if there was an error */
  if( wBytesRead == -1 )  {
    MessageBox(hWndMain, "Unable to read metafile bits",
               NULL, MB_OK | MB_ICONHAND);
    GlobalUnlock(hMem);
    GlobalFree(hMem);
    return(FALSE);
  }

  /* set the metafile bits to the memory that we allocated */
  if (!(hMF = SetMetaFileBits(hMem)))
    return(FALSE);

  GlobalUnlock(hMem);

  return(TRUE);
}

/***********************************************************************

  FUNCTION   : SetPlaceableExts

  PARAMETERS : HDC           hDC
               ALDUSMFHEADER ahdr
               int           nDest

  PURPOSE    : set the origins and extents on the DC to correspond with
               the origins and extents specified within the placeable
               metafile header

  CALLS      : WINDOWS
                 GetClientRect
                 SetMapMode
                 SetWindowOrg
                 SetWindowExt
                 SetViewportOrg
                 SetViewportExt

               C runtime
                 labs

  MESSAGES   : none

  RETURNS    : void

  COMMENTS   :

  HISTORY    : 1/16/91 - created - drc

************************************************************************/

void SetPlaceableExts(hDC, ahdr, nDest)
HDC           hDC;
ALDUSMFHEADER ahdr;
int           nDest;
{
  RECT          rect;

  /* if setting the extents on the display DC */
  if (nDest != WMFPRINTER)
    GetClientRect(hWndMain, &rect);

  SetMapMode(hDC, MM_ANISOTROPIC);

  /* set the windows origin to correspond to the bounding box origin
     contained in the placeable header */
  SetWindowOrg(hDC, ahdr.bbox.left, ahdr.bbox.top);

  /* set the window extents based on the abs value of the bbox coords */
  SetWindowExt(hDC,abs(ahdr.bbox.left) + abs(ahdr.bbox.right),
                   abs(ahdr.bbox.top) + abs(ahdr.bbox.bottom));

  /* set the viewport origin and extents */
  if (nDest != WMFPRINTER)
    {
      SetViewportOrg(hDC, 0, 0);
      SetViewportExt(hDC, rect.right, rect.bottom);
    }
  else
    {
      SetViewportOrg(hPr, 0, 0);
      SetViewportExt(hPr,GetDeviceCaps(hPr, HORZRES),
                         GetDeviceCaps(hPr, VERTRES) );
    }
}

/***********************************************************************

  FUNCTION   : SetClipMetaExts

  PARAMETERS : HDC          hDC
               METAFILEPICT MFP
               int          nDest

  PURPOSE    : set the extents to the client rect for clipboard metafiles

  CALLS      : WINDOWS
                 GetClientRect
                 IntersectClipRect
                 SetMapMode
                 SetViewportOrg
                 SetViewportExt
                 SetWindowExt

  MESSAGES   : none

  RETURNS    : void

  COMMENTS   : this is not as robust as it could be.  A more complete
               approach might be something like Petzold discusses in
               his Programming Windows book on page 793 in the
               function PrepareMetaFile().

  HISTORY    : 1/16/91 - created - drc

************************************************************************/

void SetClipMetaExts(hDC, MFP, nDest)
HDC	        hDC;
METAFILEPICT	MFP;
int             nDest;
{
  int	cx, cy;
  RECT  rect;

  /* extents for the display DC */
  if (nDest != WMFPRINTER)
    {
      GetClientRect(hWndMain, &rect);
      cx = rect.right - rect.left;
      cy = rect.bottom - rect.top;
      IntersectClipRect(hDC, rect.left, rect.top, rect.right, rect.bottom);
    }

  SetMapMode(hDC, MFP.mm);

  /* set physical origin to 0, 0 */
  SetViewportOrg(hDC, 0, 0);

  /* given the mapping mode specified in the metafilepict */
  switch (MFP.mm)  {
    case MM_ISOTROPIC:
      if (MFP.xExt && MFP.yExt)
        SetWindowExt(hDC, MFP.xExt, MFP.yExt);

        /* fall through */

    case MM_ANISOTROPIC:
      if (nDest != WMFPRINTER)
        SetViewportExt(hDC, cx, cy);
      else
        SetViewportExt(hDC, GetDeviceCaps(hDC, HORZRES),
                            GetDeviceCaps(hDC, VERTRES) );
    break;

    default:
      break;
  }

}

/***********************************************************************

  FUNCTION   : ProcessFile

  PARAMETERS : HWND  hWnd
               LPSTR lpFileName

  PURPOSE    : open the metafile, determine if it contains a valid
               metafile, decide what type of metafile it is (wmf,
               clipboard, or placeable) and take care of some menu
               housekeeping tasks.

  CALLS      : WINDOWS
                 _lopen
                 _lread
                 _llseek
                 _lclose
                 lstrcmp
                 lstrcpy
                 lstrcat
                 MessageBox
                 EnableMenuItem
                 DrawMenuBar
                 SetWindowText

               WINCOM
                 SplitPath

               APP
                 RenderPlaceableMeta
                 RenderClipMeta

  MESSAGES   : none

  RETURNS    : BOOL

  COMMENTS   :

  HISTORY    : 1/16/91 - created - drc

************************************************************************/

BOOL ProcessFile(hWnd, lpFileName)
HWND  hWnd;
LPSTR lpFileName;
{
  int		 fh;
  int		 wBytesRead;
  DWORD 	 HeaderPos;
  DWORD          dwIsAldus;
  CLIPFILEHEADER FileHeader;
  CLIPFILEFORMAT ClipHeader;
  char           szCaption[144];

  /* for openfiledialog */
  char drive[3];
  char dir[130];
  char fname[13];
  char ext[5];

  /* split the fully qualified filename into its components */
  SplitPath( lpFileName, (LPSTR)drive,
	      (LPSTR)dir, (LPSTR)fname, (LPSTR)ext);

  /* if the file is a "traditional" or placeable metafile
     as per the normal naming conventions */
  if (lstrcmp((LPSTR)ext, (LPSTR)"WMF") == 0)  {

    /* try to open the file.  It's existence has already been
       checked by WINCOM's OpenFileDialog */
    fh = _lopen(lpFileName, OF_READ);

    /* if opened successfully */
    if (fh != -1)  {

      /* always disable the clipboard header menu if we get here */
      EnableMenuItem(GetMenu(hWnd), IDM_CLIPHDR, MF_DISABLED | MF_GRAYED);

      /* read the first dword of the file to see if it is a placeable wmf */
      wBytesRead = _lread(fh,(LPSTR)&dwIsAldus, sizeof(dwIsAldus));

      if (wBytesRead == -1 || wBytesRead < sizeof(dwIsAldus))  {
        _lclose(fh);
        MessageBox(hWndMain, "unable to read file", NULL,
                   MB_OK | MB_ICONEXCLAMATION);
        return (FALSE);

      }

      /* if this is windows metafile, not a placeable wmf */
      if (dwIsAldus != ALDUSKEY)  {

        /* disable aldus header menu item */
        EnableMenuItem(GetMenu(hWnd), IDM_ALDUSHDR, MF_DISABLED|MF_GRAYED);

        /* seek to the beginning of the file */
        _llseek(fh, 0, 0);

        /* read the wmf header */
	wBytesRead = _lread(fh, (LPSTR)&mfHeader, sizeof(METAHEADER));

        /* done with file so close it */
        _lclose(fh);

        /* if read failed */
	if (wBytesRead == -1 || wBytesRead < sizeof(dwIsAldus))  {
          MessageBox(hWndMain, "unable to read metafile header", NULL,
                     MB_OK | MB_ICONEXCLAMATION);
          return (FALSE);
        }
      }

      /* this is a placeable metafile */
      else  {

        /* enable the placeable header menu item */
        EnableMenuItem(GetMenu(hWnd), IDM_ALDUSHDR, MF_ENABLED);

        /* convert the placeable format into something that can
           be used with GDI metafile functions */
        RenderPlaceableMeta(fh);

        /* close the file */
        _lclose(fh);

      }

      /* at this point we have a metafile header regardless of whether
         the metafile was a windows metafile or a placeable metafile
         so check to see if it is valid.  There is really no good
         way to do this so just make sure that the mtType is either
         1 or 2 (memory or disk file) */

      if ( (mfHeader.mtType != 1) && (mfHeader.mtType != 2) )  {
        /* set the program flags appropriately */
        bBadFile = TRUE;
        bMetaFileOpen = FALSE;
        bValidFile = FALSE;

        /* let the user know that this is an invalid metafile */
        MessageBox(hWndMain, "This file is not a valid metafile",
                   NULL, MB_OK | MB_ICONEXCLAMATION);

        /* restore the caption text to the default */
        SetWindowText(hWnd, (LPSTR)APPNAME);

        /* disable menu items, indicating that a valid metafile has not been
           loaded */
        EnableMenuItem(GetMenu(hWnd), IDM_VIEW, MF_DISABLED|MF_GRAYED|MF_BYPOSITION);
        EnableMenuItem(GetMenu(hWnd), IDM_PLAY, MF_DISABLED|MF_GRAYED|MF_BYPOSITION);
        EnableMenuItem(GetMenu(hWnd), IDM_PRINT, MF_DISABLED|MF_GRAYED);

        /* refresh the menu bar to reflect above changes */
        DrawMenuBar(hWnd);

      }
      /* this is a valid metafile...at least based on the above criteria */
      else  {

        /* modify and update the caption text */

	wsprintf((LPSTR)szCaption, (LPSTR)"%s - %s.%s",
                 (LPSTR)APPNAME, (LPSTR)fname, (LPSTR)ext);

        /* this could be used by the printing routines if unable to print */
	wsprintf((LPSTR)fnameext, (LPSTR)"%s.%s", (LPSTR)fname, (LPSTR)ext);

        SetWindowText(hWnd, (LPSTR)szCaption);

        /* enable the appropriate menu items */
        EnableMenuItem(GetMenu(hWnd), IDM_VIEW, MF_ENABLED|MF_BYPOSITION);
        EnableMenuItem(GetMenu(hWnd), IDM_PLAY, MF_ENABLED|MF_BYPOSITION);
        EnableMenuItem(GetMenu(hWnd), IDM_PRINT, MF_ENABLED);

        /* refresh the menu bar to reflect above changes */
        DrawMenuBar(hWnd);

        /* set program flags appropriately */
        bValidFile = TRUE;
	bMetaFileOpen = TRUE;

	if (dwIsAldus != ALDUSKEY)  {
	  bAldusMeta = FALSE;
	  bMetaInRam = FALSE;
	}
	else  {
	  bAldusMeta = TRUE;
	  bMetaInRam = TRUE;
	}
      }
      return (TRUE);

    } /* if fh != -1 */
    else
      return (FALSE);
  }
  /* file ext is not WMF so check to see if it is a clipboard file */
  else  {
    if (lstrcmp((LPSTR)ext, (LPSTR)"CLP") == 0)  {

      WORD i;

      /* try to open the file.  It's existence has already been
         checked by WINCOM's OpenFileDialog */
      fh = _lopen(lpFileName, OF_READ);

      /* if opened successfully */
      if (fh != -1 )  {

        /* read the clipboard file header */
        FileHeader.FormatCount = 0;
        _lread(fh, (LPSTR)&FileHeader, sizeof(CLIPFILEHEADER));

        /* if this is not a valid clipboard file based on the file
           identifier of the file header */

        if (FileHeader.FileIdentifier != CLP_ID)  {
          _lclose(fh);
          MessageBox(hWndMain, "This file is not a valid clipboard file",
                     NULL, MB_OK | MB_ICONEXCLAMATION);
          return (FALSE);
        }

        HeaderPos = sizeof(CLIPFILEHEADER);

        /* search the formats contained within the clipboard file looking
           for a metafile.  Break if and when it is found */

        for (i=0; i < FileHeader.FormatCount; i++)  {

          _llseek(fh, HeaderPos, 0);

          /* read the clipboard header found at current position */
          if(_lread(fh, (LPSTR)&ClipHeader, sizeof(ClipHeader)) < sizeof(ClipHeader))  {
            _lclose(fh);
            MessageBox(hWndMain, "read of clipboard header failed",
                       NULL, MB_OK | MB_ICONEXCLAMATION);
            return (FALSE);
          }

          /* increment the file offset */
          HeaderPos += sizeof(ClipHeader);

          /* if a metafile was found break */
          if (ClipHeader.FormatID == CF_METAFILEPICT)
            break;
        }

        /* was it really so? */
        if (ClipHeader.FormatID == CF_METAFILEPICT)  {

          /* if there is currently a metafile loaded delete it */
          if ((bMetaInRam) && (hMF))
            DeleteMetaFile(hMF);

          /* modify and update the caption text */
	  wsprintf((LPSTR)szCaption, (LPSTR)"%s - %s.%s",
                   (LPSTR)APPNAME, (LPSTR)fname, (LPSTR)ext);

          /* this could be used by the printing routines if unable to print */
	  wsprintf((LPSTR)fnameext, (LPSTR)"%s.%s", (LPSTR)fname, (LPSTR)ext);

          SetWindowText(hWnd, (LPSTR)szCaption);

          /* enable the appropriate menu items */
          EnableMenuItem(GetMenu(hWnd), IDM_ALDUSHDR, MF_DISABLED|MF_GRAYED);
          EnableMenuItem(GetMenu(hWnd), IDM_CLIPHDR, MF_ENABLED);
          EnableMenuItem(GetMenu(hWnd), IDM_VIEW, MF_ENABLED|MF_BYPOSITION);
          EnableMenuItem(GetMenu(hWnd), IDM_PLAY, MF_ENABLED|MF_BYPOSITION);
          EnableMenuItem(GetMenu(hWnd), IDM_PRINT, MF_ENABLED);

          /* refresh the menu bar */
          DrawMenuBar(hWnd);

          /* set the program flags appropriately */
          bValidFile = TRUE;
          bMetaFileOpen = TRUE;
	  bMetaInRam = TRUE;
	  bAldusMeta = FALSE;

          /* convert the metafile contained within the clipboard file into
             a format useable with GDI metafile functions */

          if (!RenderClipMeta(&ClipHeader, fh))
            MessageBox(hWndMain, "Unable to render format",
                       NULL, MB_OK | MB_ICONEXCLAMATION);

          /* close the file */
          _lclose(fh);

        }
        /* a metafile was not found within the clipboard file */
        else  {
          bBadFile = TRUE;
          bMetaFileOpen = FALSE;
          bValidFile = FALSE;

          /* let the user know */
          MessageBox(hWndMain, "This CLP file doesn't contain a valid metafile",
                     NULL, MB_OK | MB_ICONEXCLAMATION);

          /* restore the caption text to default */
          SetWindowText(hWnd, (LPSTR)APPNAME);

          /* disable previously enabled menu items */
          EnableMenuItem(GetMenu(hWnd), IDM_VIEW, MF_DISABLED|MF_GRAYED|MF_BYPOSITION);
          EnableMenuItem(GetMenu(hWnd), IDM_PLAY, MF_DISABLED|MF_GRAYED|MF_BYPOSITION);
          EnableMenuItem(GetMenu(hWnd), IDM_PRINT, MF_DISABLED|MF_GRAYED);

          /* refresh the menu bar to reflect these changes */
          DrawMenuBar(hWnd);

          _lclose(fh);
        }
        return (TRUE);
      }
      else
        return (FALSE);
    }
  }
}
