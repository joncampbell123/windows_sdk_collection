/*
 * LISTHSCR.H
 *
 * Private definitions and prototypes for the listhscr DLL.
 */



//Unit of allocation for extent list.
#define CBALLOCUNIT     (sizeof(WORD)<<7)

//Number of units per allocation unit.
#define CALLOCUNITS     128


/*
 * Private prototypes
 */

BOOL FReAllocExtentList(HANDLE, BOOL);
WORD WGetListboxStringExtent(HWND, LPSTR);
WORD IFindExtentInList(WORD *, WORD, WORD);
