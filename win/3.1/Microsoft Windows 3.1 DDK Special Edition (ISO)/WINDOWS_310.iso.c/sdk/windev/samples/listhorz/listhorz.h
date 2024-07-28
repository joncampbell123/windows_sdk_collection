/*
 * Control Identifiers.
 */

#define ID_STRINGEDIT    100
#define ID_ADD           101
#define ID_DELETE        102
#define ID_LISTBOX       103


long	FAR PASCAL ListHScrollWndProc(HWND, UINT, WPARAM, LPARAM);

/*
 * Protoypes of the 'API' defined in listhelp.c
 */
BOOL FAR PASCAL FInitListboxExtents(HWND);
WORD FAR PASCAL WAddExtentEntry(HWND, LPSTR);
WORD FAR PASCAL WRemoveExtentEntry(HWND, WORD);
BOOL FAR PASCAL FFreeListboxExtents(HWND);
void FAR PASCAL ResetListboxExtents(HWND);
