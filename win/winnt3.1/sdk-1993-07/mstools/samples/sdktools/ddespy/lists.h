/*
 * LISTS.H
 *
 * Header file for multi-column listbox module.
 */

typedef struct {
    LPSTR   lpszHeadings;
} MCLBCREATESTRUCT;


typedef struct {
    HWND    hwndLB;
    PSTR    pszHeadings;
    INT     cCols;
    INT     SortCol;
} MCLBSTRUCT;

#define MYLBSTYLE   WS_CHILD|WS_BORDER |LBS_SORT| \
                    WS_VSCROLL|LBS_OWNERDRAWFIXED|LBS_NOINTEGRALHEIGHT

HWND CreateMCLBFrame(
                    HWND hwndParent,
                    LPSTR lpszTitle,       /* frame title string */
                    UINT dwStyle,          /* frame styles */
                    HICON hIcon,           /* icon */
                    HBRUSH hbrBkgnd,       /* background for heading.*/
                    LPSTR lpszHeadings);   /* tab delimited list of headings.  */
                                           /* The number of headings indicate  */
                                           /* the number of collumns. */

VOID AddMCLBText(PSTR pszSearch, PSTR pszReplace, HWND hwndLBFrame);
INT GetMCLBColValue(PSTR pszSearch, HWND hwndLBFrame, int  cCol);
BOOL DeleteMCLBText(PSTR pszSearch, HWND hwndLBFrame);

