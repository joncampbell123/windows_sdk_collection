/*
 * LISTHAPI.H
 *
 * Include file that prototypes the exported functions that are in the
 * LISTHSCR DLL to support horizontal listbox scrolling.
 *
 */

BOOL FAR PASCAL FInitListboxExtents(HWND);
WORD FAR PASCAL WAddExtentEntry(HWND, LPSTR);
WORD FAR PASCAL WRemoveExtentEntry(HWND, WORD);
BOOL FAR PASCAL FFreeListboxExtents(HWND);
void FAR PASCAL ResetListboxExtents(HWND);
