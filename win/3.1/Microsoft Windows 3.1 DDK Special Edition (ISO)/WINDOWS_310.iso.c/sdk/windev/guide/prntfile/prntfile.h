/* file menu items */

#define     IDM_NEW        100
#define     IDM_OPEN       101
#define     IDM_SAVE       102
#define     IDM_SAVEAS     103
#define     IDM_PRINT      104
#define     IDM_EXIT       105
#define     IDM_ABOUT      106
#define     IDM_PRINTSETUP 107

/* edit menu items */

#define     IDM_UNDO     200
#define     IDM_CUT      201
#define     IDM_COPY     202
#define     IDM_PASTE    203
#define     IDM_CLEAR    204

/* Control IDs */

#define     IDC_EDIT      401
#define     IDC_FILENAME  402

#define MAXFILESIZE 1000000    /* maximum file size (in bytes) that can be loaded */
#define MAXFILENAME 256 	     /* maximum length of file pathname      */
#define MAXCUSTFILTER 40	     /* maximum size of custom filter buffer */


int PASCAL WinMain(HANDLE, HANDLE, LPSTR, int);
BOOL InitApplication(HANDLE);
BOOL InitInstance(HANDLE, int);
long FAR PASCAL MainWndProc(HWND, UINT, WPARAM, LPARAM);
BOOL FAR PASCAL About(HWND, unsigned, WORD, LONG);
BOOL SaveFile(HWND);
BOOL QuerySaveFile(HWND);
void SetNewBuffer(HWND, HANDLE, PSTR);
HANDLE GetPrinterDC();
int FAR PASCAL AbortProc(HDC, int);
int FAR PASCAL AbortDlg(HWND, unsigned, WORD, LONG);

