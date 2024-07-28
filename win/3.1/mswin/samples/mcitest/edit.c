/*
 * edit.c - Routines for dealing with multi-line edit controls.
 */

#include <windows.h>
#include "gmem.h"
#include "edit.h"


#define  SEEK_SET 0             // Flags for _lseek
#define  SEEK_CUR 1
#define  SEEK_END 2

#define ISSPACE(c) ((c) == ' ' || (c) == '\t')
#define ISEOL(c)   ((c) == '\n'|| (c) == '\r')
#define ISWHITE(c) (ISSPACE(c) || ISEOL(c))


/* EditOpenFile - Opens the specified file, copies the contents of the file
 *   into the edit control with the specified handle, and then closes the
 *
 * Params:      hwndEdit - window handle of the edit box control
 *              lszFile - filename of the file to be opened
 *
 * Returns:     TRUE if the operation was successful, else FALSE
 */
BOOL    PASCAL FAR EditOpenFile(
    HWND    hwndEdit,
    LPSTR   lszFile)
{
    OFSTRUCT    of;
    HFILE       fh;
    LPSTR       lszText;
    int         nFileLen;
    HCURSOR     hcur;

    /* If a valid window handle or a filename was not specified, then exit.
     */
    if (!hwndEdit || !lszFile)
        return FALSE;

    /* Open the file for reading.
     */
    fh = OpenFile(lszFile, &of, OF_READ);
    if (fh == HFILE_ERROR)
        return FALSE;

    /* Seek to the end of the file to find out the file's length in bytes.
     */
    nFileLen = (int)_llseek(fh, 0L, SEEK_END);
    _llseek(fh, 0L, SEEK_SET);

    /* Create a pointer to a region of memory large enough to hold the entire
     * contents of the file. If this was successful, then read the file into
     * this region, and use this region as the text for the edit control.
     * Finally, free up the region and its pointer, and close the file.
     */
    if (lszText = GAllocPtr(nFileLen + 1)) {

        /* This could take a while - show the hourglass cursor.
         */
        hcur = SetCursor(LoadCursor(NULL, IDC_WAIT));

        /* Read the file and copy the contents into the edit control.
         */
        _lread(fh, lszText, nFileLen);
        lszText[nFileLen]=0;
        SetWindowText(hwndEdit, lszText);

        /* Free up the memory, close the file, and restore the cursor.
         */
        GFreePtr(lszText);
        _lclose(fh);
        SetCursor(hcur);

        return TRUE;
    }

    /* Could not allocate the required memory, so close the file and
     * return FALSE.
     */
    _lclose(fh);
    return FALSE;
}


/* EditSaveFile - Saves the contents of the edit control with the handle
 *  hwndEdit> into the file named <lszFile>, creating the file if
 *  required.
 *
 * Params:  hwndEdit - window handle of the edit box control
 *          lszFile - filename of the file to be saved
 *
 * Returns: TRUE if the operation was successful, else FALSE
 */
BOOL    PASCAL FAR EditSaveFile(
    HWND    hwndEdit,
    LPSTR   lszFile)
{
    OFSTRUCT    of;
    HFILE       fh;
    LPSTR       lszText;
    int         nFileLen;
    HCURSOR     hcur;

    /* If a valid window handle or a filename was not specified, then exit.\
     */
    if (!hwndEdit || !lszFile)
        return FALSE;

    /* Create (or overwrite) the save file.
     */
    fh = OpenFile(lszFile, &of, OF_CREATE);
    if (fh == HFILE_ERROR)
        return FALSE;

    /* Find out how big the contents of the edit box are.
     */
    nFileLen = GetWindowTextLength(hwndEdit) + 1;

    /* Create a pointer to a region of memory large enough to hold the entire
     * contents of the edit box. If this was successful, then read contents
     * of the edit box into this region, and write the contents of this region
     * into the save file. Finally, free up the region and its pointer, and
     * close the file.
     */
    if (lszText = GAllocPtr(nFileLen)) {

        /* This could take a while - show the hourglass cursor.
         */
        hcur = SetCursor(LoadCursor(NULL, IDC_WAIT));

        /* Read the contents of the edit box, and write it to the save file.
         */
        GetWindowText(hwndEdit, lszText, nFileLen);
        _lwrite(fh, lszText, nFileLen);

        /* Free up the memory, close the file, and restore the cursor.
         */
        GFreePtr(lszText);
        _lclose(fh);
        SetCursor(hcur);

        return TRUE;
    }

    /* Could not allocate the required memory, so close the file and
     * return FALSE.
     */
    _lclose(fh);
    return FALSE;
}


/* EditGetLineCount - Finds out how many lines of text are in the edit 
 *   box and returns this value.
 *
 * Params:  hwndEdit - window handle of the edit box control
 *
 * Returns: The number of lines of text in the edit control.
 */
DWORD   PASCAL FAR EditGetLineCount(
    HWND    hwndEdit)
{
    return SendMessage(hwndEdit, EM_GETLINECOUNT, 0, 0L);
}


/* EditGetLine - Retrieves the contents of line # <iLine> from the edit
 *   box control with handle <hwndEdit>. It returns a pointer to a string
 *   holding the contents of the given line. All trailing white spaces are
 *   removed from the string, and it is null-terminated.
 *
 * Params:  hwndEdit -  window handle of the edit box control
 *          iLine - line # to get the contents of
 *          szLineBuffer - pointer to the buffer to copy the line to
 *          cch - max # of characters to copy
 *
 * Returns: TRUE.
 */
BOOL    PASCAL FAR EditGetLine(
    HWND    hwndEdit,
    int     iLine,
    LPSTR   lszLineBuffer,
    int     cch)
{
    int     nLines;

    /* Find out how many lines are in the edit control. If the requested line
     * is out of range, then return.
     */
    nLines = (int)EditGetLineCount(hwndEdit);
    if (iLine < 0 || iLine >= nLines)
        return *lszLineBuffer = 0;

    /* Read the requested line into the string pointed to by <lszLineBuffer>.
     */
    *((LPWORD)lszLineBuffer) = cch;
    cch = (int)SendMessage(hwndEdit, EM_GETLINE, iLine, (LONG)(LPSTR)lszLineBuffer);

    /* Strip trailing white spaces from the string, and null-terminate it.
     */
    while(cch > 0 && ISWHITE(lszLineBuffer[cch-1]))
        cch--;
    lszLineBuffer[cch] = 0;

    return TRUE;
}


/* EditGetCurLine - Retrieves the line number of the current line in the
 *    edit box control with handle <hwndEdit>. It returns this line number.
 *
 * Params:  hwndEdit - window handle of the edit box control
 *
 * Returns: The line number of the cuurent line.
 */
int PASCAL FAR EditGetCurLine(
    HWND    hwndEdit)
{
    int iLine;

    iLine = (int)SendMessage(hwndEdit,(UINT) EM_LINEFROMCHAR,(WPARAM) -1,(LPARAM) 0L);

    if (iLine < 0)
        iLine = 0;

    return iLine;
}


/* EditSetCurLine - Sets the current line in the edit box control with
 *   handle <hwndEdit> to the number given in <iLine>.
 *
 * Params:  hwndEdit - window handle of the edit box control
 *          iLine - the line number to be made the current line
 *
 * Returns: void
 *
 */
void    PASCAL FAR EditSetCurLine(
    HWND    hwndEdit,
    int     iLine)
{
    int off;

    off = (int)SendMessage(hwndEdit, EM_LINEINDEX, iLine, 0L);
    SendMessage(hwndEdit, EM_SETSEL, 0, MAKELONG(off, off));

}


/* EditSelectLine - Selects line # <iLine> in the edit box control with
 *   handle <hwndEdit>.
 *
 * Params:  hwndEdit - window handle of the edit box control
 *          iLine - the line number to be selected
 *
 * Returns: void
 */
void    PASCAL FAR EditSelectLine(
    HWND    hwndEdit,
    int     iLine)
{
    int offS;
    int offE;

    offS = (int)SendMessage(hwndEdit, EM_LINEINDEX, iLine, 0L);
    offE = (int)SendMessage(hwndEdit, EM_LINEINDEX, iLine + 1, 0L);

    if (offE < offS)
        offE = 0x7fff;

    SendMessage(hwndEdit, EM_SETSEL, 0, MAKELONG(offS, offE));
}
