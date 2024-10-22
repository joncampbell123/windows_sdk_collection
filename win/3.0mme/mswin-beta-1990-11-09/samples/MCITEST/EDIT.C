/*----------------------------------------------------------------------------*\
|   edit.c - routines for dealing with multi-line edit controls                |
|                                                                              |
|                                                                              |
|   History:                                                                   |
|       01/01/88 toddla     Created                                            |
|       11/04/90 w-dougb    Commented & formatted the code to look pretty      |
|                                                                              |
\*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*\
|                                                                              |
|   i n c l u d e   f i l e s                                                  |
|                                                                              |
\*----------------------------------------------------------------------------*/

#include <windows.h>
#include "edit.h"


/*----------------------------------------------------------------------------*\
|                                                                              |
|   c o n s t a n t   a n d   m a c r o   d e f i n i t i o n s                |
|                                                                              |
\*----------------------------------------------------------------------------*/

/* Flags for _lseek (these are missing from <windows.h>, for some reason) */

#define  SEEK_SET 0
#define  SEEK_CUR 1
#define  SEEK_END 2

#define ISSPACE(c) ((c) == ' ' || (c) == '\t')
#define ISEOL(c)   ((c) == '\n'|| (c) == '\r')
#define ISWHITE(c) (ISSPACE(c) || ISEOL(c))


/*----------------------------------------------------------------------------*\
|   EditOpenFile(hWndE, szFile)                                                |
|                                                                              |
|   Description:                                                               |
|       This function opens the file <szFile>, copies the contents of the file |
|       into the edit control with the handle <hWndE>, and then closes the     |
|       file.                                                                  |
|                                                                              |
|   Arguments:                                                                 |
|       hWndE           window handle of the edit box control                  |
|       szFile          filename of the file to be opened                      |
|                                                                              |
|   Returns:                                                                   |    
|       TRUE if the operation was successful, else FALSE                       |
|                                                                              |
\*----------------------------------------------------------------------------*/

BOOL EditOpenFile(hwndE, szFile)

HWND hwndE;
LPSTR szFile;

{
    OFSTRUCT    of;                 /* structure used by the OpenFile routine */
    int         fh;                 /* DOS file handle returned by OpenFile   */
    LPSTR       lp;                 /* pointer to the opened file's text      */
    int         len;                /* length, in bytes, of the opened file   */
    HCURSOR     hcur;               /* handle to the pre-hourglass cursor     */
    HANDLE      hmem;               /* handle to memory where file is stored  */

    /* If a valid window handle or a filename was not specified, then exit */

    if (!hwndE || !szFile)
        return FALSE;

    /* Open the file for reading */

    fh = OpenFile(szFile, &of, OF_READ);
    if (fh == -1)
        return FALSE;

    /* Seek to the end of the file to find out the file's length in bytes */

    len = (int)_llseek(fh, 0L, SEEK_END);
    _llseek(fh, 0L, SEEK_SET);

    /*
     * Create a pointer to a region of memory large enough to hold the entire
     * contents of the file. If this was successful, then read the file into
     * this region, and use this region as the text for the edit control.
     * Finally, free up the region and its pointer, and close the file.
     *
     */

    if (hmem = GlobalAlloc(GMEM_MOVEABLE, (LONG)len+1)) {

        lp = GlobalLock(hmem);

        /* This could take a while - show the hourglass cursor */

        hcur = SetCursor(LoadCursor(NULL, IDC_WAIT));

        /* Read the file and copy the contents into the edit control */

        _lread(fh, lp, len);
        lp[len]=0;
        SetWindowText(hwndE, lp);

        /* Free up the memory, close the file, and restore the cursor */

        GlobalUnlock(hmem);
        GlobalFree(hmem);
        _lclose(fh);
        SetCursor(hcur);

        return TRUE;
    }

    /*
     * We couldn't allocate the required memory, so close the file and
     * return FALSE.
     *
     */

    _lclose(fh);
    return FALSE;
}


/*----------------------------------------------------------------------------*\
|   EditSaveFile(hWndE, szFile)                                                |
|                                                                              |
|   Description:                                                               |
|       This function saves the contents of the edit control with the handle   |
|       <hWndE> into the file named <szFile>, creating the file if required.   |
|                                                                              |
|   Arguments:                                                                 |
|       hWndE           window handle of the edit box control                  |
|       szFile          filename of the file to be saved                       |
|                                                                              |
|   Returns:                                                                   |    
|       TRUE if the operation was successful, else FALSE                       |
|                                                                              |
\*----------------------------------------------------------------------------*/

BOOL EditSaveFile(hwndE, szFile)

HWND hwndE;
LPSTR szFile;

{
    OFSTRUCT    of;                 /* structure used by the OpenFile routine */
    int         fh;                 /* DOS file handle returned by OpenFile   */
    LPVOID      lp;                 /* pointer to the saved file's text       */
    int         len;                /* length, in bytes, of the saved file    */
    HCURSOR     hcur;               /* handle to the pre-hourglass cursor     */
    HANDLE      hmem;               /* handle to memory where file is stored  */

    /* If a valid window handle or a filename was not specified, then exit */

    if (!hwndE || !szFile)
        return FALSE;

    /* Create (or overwrite) the save file */

    fh = OpenFile(szFile, &of, OF_CREATE);
    if (fh == -1)
        return FALSE;

    /* Find out how big the contents of the edit box are */

    len = GetWindowTextLength(hwndE) + 1;

    /*
     * Create a pointer to a region of memory large enough to hold the entire
     * contents of the edit box. If this was successful, then read the contents
     * of the edit box into this region, and write the contents of this region
     * into the save file. Finally, free up the region and its pointer, and
     * close the file.
     *
     */

     if (hmem = GlobalAlloc(GMEM_MOVEABLE, (LONG)len+1)) {

        lp = GlobalLock(hmem);

        /* This could take a while - show the hourglass cursor */

        hcur = SetCursor(LoadCursor(NULL, IDC_WAIT));

        /* Read the contents of the edit box, and write it to the save file */

        GetWindowText(hwndE, lp, len);
        _lwrite(fh, lp, len);

        /* Free up the memory, close the file, and restore the cursor */

        GlobalUnlock(hmem);
        GlobalFree(hmem);
        _lclose(fh);
        SetCursor(hcur);

        return TRUE;
    }

    /*
     * We couldn't allocate the required memory, so close the file and
     * return FALSE.
     *
     */

    _lclose(fh);
    return FALSE;
}


/*----------------------------------------------------------------------------*\
|   EditGetLineCount(hWndE)                                                    |
|                                                                              |
|   Description:                                                               |
|       This function finds out how many lines of text are in the edit box and |
|       returns this value.                                                    |
|                                                                              |
|   Arguments:                                                                 |
|       hWndE           window handle of the edit box control                  |
|                                                                              |
|   Returns:                                                                   |    
|       The number of lines of text in the edit control.                       |
|                                                                              |
\*----------------------------------------------------------------------------*/

DWORD EditGetLineCount(hwndE)

HWND hwndE;

{
    return SendMessage(hwndE, EM_GETLINECOUNT, 0, 0L);
}


/*----------------------------------------------------------------------------*\
|   EditGetLine(hWndE, iLine, pch, cch)                                        |
|                                                                              |
|   Description:                                                               |
|       This function retrieves the contents of line # <iLine> from the edit   |
|       box control with handle <hWndE>. It returns a pointer to a string      |
|       holding the contents of the given line. All trailing white spaces are  |
|       removed from the string, and it is null-terminated.                    |
|                                                                              |
|   Arguments:                                                                 |
|       hWndE           window handle of the edit box control                  |
|       iLine           line # to get the contents of                          |
|       pch             pointer to the buffer to copy the line to              |
|       cch             max # of characters to copy                            |
|                                                                              |
|   Returns:                                                                   |    
|       TRUE.                                                                  |
|                                                                              |
\*----------------------------------------------------------------------------*/

BOOL EditGetLine(hwndE, iLine, pch, cch)

HWND hwndE;
int iLine;
LPSTR pch;
int cch;

{
    int     nLines;             /* total number of lines in the edit box */                     

    /*
     * Find out how many lines are in the edit control. If the requested line
     * is out of range, then return.
     *
     */

    nLines = EditGetLineCount(hwndE);
    if (iLine < 0 || iLine >= nLines)
        return *pch = 0;

    /* Read the requested line into the string pointed to by <pch> */

    *((LPWORD)pch) = cch;
    cch = SendMessage(hwndE, EM_GETLINE, iLine, (LONG)(LPSTR)pch);

    /* Strip trailing white spaces from the string, and null-terminate it */

    while(cch > 0 && ISWHITE(pch[cch-1]))
        cch--;
    pch[cch] = 0;

    return TRUE;
}


/*----------------------------------------------------------------------------*\
|   EditGetCurLine(hWndE)                                                      |
|                                                                              |
|   Description:                                                               |
|       This function retrieves the line number of the current line in the     |
|       edit box control with handle <hWndE>. It returns this line number.     |
|                                                                              |
|   Arguments:                                                                 |
|       hWndE           window handle of the edit box control                  |
|                                                                              |
|   Returns:                                                                   |    
|       The line number of the cuurent line.                                   |
|                                                                              |
\*----------------------------------------------------------------------------*/

int EditGetCurLine(hwndE)

HWND hwndE;

{
    int iLine;                  /* Line number of the currently active line   */

    iLine = (int)SendMessage(hwndE, EM_LINEFROMCHAR, -1, 0L);

    if (iLine < 0)
        iLine = 0;

    return iLine;
}


/*----------------------------------------------------------------------------*\
|   EditSetCurLine(hWndE, iLine)                                               |
|                                                                              |
|   Description:                                                               |
|       This function sets the current line in the edit box control with       |
|       handle <hWndE> to the number given in <iLine>.                         |
|                                                                              |
|   Arguments:                                                                 |
|       hWndE           window handle of the edit box control                  |
|       iLine           the line number to be made the current line            |
|                                                                              |
|   Returns:                                                                   |    
|       void                                                                   |
|                                                                              |
\*----------------------------------------------------------------------------*/

void EditSetCurLine(hwndE, iLine)

HWND hwndE;
int iLine;

{
    int off;

    off = (int)SendMessage(hwndE, EM_LINEINDEX, iLine, 0L);
    SendMessage(hwndE, EM_SETSEL, 0, MAKELONG(off,off));

}


/*----------------------------------------------------------------------------*\
|   EditSelectLine(hWndE, iLine)                                               |
|                                                                              |
|   Description:                                                               |
|       This function selects line # <iLine> in the edit box control with      |
|       handle <hWndE>.                                                        |
|                                                                              |
|   Arguments:                                                                 |
|       hWndE           window handle of the edit box control                  |
|       iLine           the line number to be selected                         |
|                                                                              |
|   Returns:                                                                   |    
|       void                                                                   |
|                                                                              |
\*----------------------------------------------------------------------------*/

void EditSelectLine(hwndE, iLine)

HWND hwndE;
int iLine;

{
    int offS;
    int offE;

    offS = (int)SendMessage(hwndE, EM_LINEINDEX, iLine, 0L);
    offE = (int)SendMessage(hwndE, EM_LINEINDEX, iLine+1, 0L);

    if (offE < offS)
        offE = 0x7fff;

    SendMessage(hwndE, EM_SETSEL, 0, MAKELONG(offS,offE));
}
